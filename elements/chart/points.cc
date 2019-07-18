/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "points.h"
#include "data.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "core/environment.h"
#include "core/layout.h"
#include "core/scale.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;

namespace fviz::elements::chart::points {

static const double kDefaultPointSizePT = 2;
static const double kDefaultPointSizeMinPT = 1;
static const double kDefaultPointSizeMaxPT = 24;
static const double kDefaultLabelPaddingEM = 0.4;

struct PlotPointsConfig {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::vector<Color> colors;
  std::vector<Measure> sizes;
  std::vector<std::string> labels;
  FontInfo label_font;
  Measure label_padding;
  Measure label_font_size;
  Color label_color;
  LayoutSettings layout;
};

ReturnCode draw(
    std::shared_ptr<PlotPointsConfig> config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x.begin(),
      &*config->x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config->sizes.begin(),
      &*config->sizes.end());

  /* draw points */
  for (size_t i = 0; i < config->x.size(); ++i) {
    auto sx = clip.x + config->x[i];
    auto sy = clip.y + clip.h - config->y[i];

    const auto& color = config->colors.empty()
        ? Color{}
        : config->colors[i % config->colors.size()];

    auto size = config->sizes.empty()
        ? from_pt(kDefaultPointSizePT, layer->dpi)
        : config->sizes[i % config->sizes.size()];

    FillStyle style;
    style.color = color;

    // TODO point style
    Path path;
    path.moveTo(sx + size, sy);
    path.arcTo(sx, sy, size, 0, M_PI * 2);
    fillPath(layer, clip, path, style);
  }

  /* draw labels */
  for (size_t i = 0; i < config->labels.size(); ++i) {
    const auto& label_text = config->labels[i];

    auto size = config->sizes.empty()
        ? 0
        : config->sizes[i % config->sizes.size()].value;

    auto label_padding = size + measure_or(
        config->label_padding,
        from_em(kDefaultLabelPaddingEM, config->label_font_size));

    Point p(
        clip.x + config->x[i],
        clip.y + clip.h - config->y[i] - label_padding);

    TextStyle style;
    style.font = config->label_font;
    style.color = config->label_color;
    style.font_size = config->label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::BOTTOM;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* set defaults from environment */
  auto config = std::make_shared<PlotPointsConfig>();
  config->label_font = env.font;
  config->label_font_size = env.font_size;

  /* parse properties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {"data-x", bind(&data_load, _1, &config->x)},
    {"data-y", bind(&data_load, _1, &config->y)},
    {"range-x-min", bind(&expr_to_float64_opt, _1, &config->scale_x.min)},
    {"range-x-max", bind(&expr_to_float64_opt, _1, &config->scale_x.max)},
    {"scale-x", bind(&scale_configure_kind, _1, &config->scale_x)},
    {"scale_x-padding", bind(&expr_to_float64, _1, &config->scale_x.padding)},
    {"range-y-min", bind(&expr_to_float64_opt, _1, &config->scale_y.min)},
    {"range-y-max", bind(&expr_to_float64_opt, _1, &config->scale_y.max)},
    {"scale-y", bind(&scale_configure_kind, _1, &config->scale_y)},
    {"scale_y-padding", bind(&expr_to_float64, _1, &config->scale_y.padding)},
    {"size", bind(&data_load, _1, &config->sizes)},
    {"sizes", bind(&data_load, _1, &config->sizes)},
    {"color", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &config->colors)},
    {"colors", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &config->colors)},
    {"labels", bind(&data_load_strings, _1, &config->labels)},
    {"label-font-size", bind(&expr_to_measure, _1, &config->label_font_size)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* check configuraton */
  if (config->x.size() != config->y.size()) {
    return error(
        ERROR,
        "the length of the 'xs' and 'ys' properties must be equal");
  }

  /* scale autoconfig */
  for (const auto& v : config->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &config->scale_y);
    }
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::points

