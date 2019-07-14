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
#include "bars.h"
#include <numeric>
#include <fviz.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include <source/utils/algo.h>
#include "source/config_helpers.h"
#include "core/environment.h"
#include "core/layout.h"

using namespace std::placeholders;

namespace fviz {
namespace plot {
namespace bars {

static const double kDefaultBarSizePT = 10;
static const double kDefaultLabelPaddingHorizEM = 0.6;
static const double kDefaultLabelPaddingVertEM = 0.6;

PlotBarsConfig::PlotBarsConfig() :
    direction(Direction::VERTICAL) {}

ReturnCode draw_horizontal(
    PlotBarsConfig config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.sizes.begin(),
      &*config.sizes.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.offsets.begin(),
      &*config.offsets.end());

  /* draw bars */
  auto x0 = std::clamp(clip.h * domain_translate(config.scale_x, 0), 0.0, 1.0);
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sy = clip.y + clip.h - config.y[i];
    auto sx1 = clip.x + (config.xoffset.empty() ? x0 : config.xoffset[i]);
    auto sx2 = clip.x + config.x[i];

    auto size = config.sizes.empty()
        ? from_pt(kDefaultBarSizePT, layer->dpi)
        : config.sizes[i % config.sizes.size()];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    const auto& color = config.colors.empty()
        ? Color{}
        : config.colors[i % config.colors.size()];

    FillStyle style;
    style.color = color;

    Path path;
    path.moveTo(sx1, sy + -offset - size * 0.5);
    path.lineTo(sx2, sy + -offset - size * 0.5);
    path.lineTo(sx2, sy + -offset + size * 0.5);
    path.lineTo(sx1, sy + -offset + size * 0.5);
    path.closePath();


    fillPath(layer, clip, path, style);
  }

  /* draw labels */
  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& text = config.labels[i];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    auto padding = measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingHorizEM, config.label_font_size));

    Point p(
        clip.x + config.x[i] + padding,
        clip.y + -offset + clip.h - config.y[i]);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::LEFT;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw_vertical(
    PlotBarsConfig config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.sizes.begin(),
      &*config.sizes.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.offsets.begin(),
      &*config.offsets.end());

  /* draw bars */
  auto y0 = clip.h * std::clamp(domain_translate(config.scale_y, 0), 0.0, 1.0);
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy1 = clip.y + clip.h - (config.yoffset.empty() ? y0 : config.yoffset[i]);
    auto sy2 = clip.y + clip.h - config.y[i];

    auto size = config.sizes.empty()
        ? from_pt(kDefaultBarSizePT, layer->dpi)
        : config.sizes[i % config.sizes.size()];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    const auto& color = config.colors.empty()
        ? Color{}
        : config.colors[i % config.colors.size()];

    FillStyle style;
    style.color = color;

    Path path;
    path.moveTo(sx + offset - size * 0.5, sy1);
    path.lineTo(sx + offset - size * 0.5, sy2);
    path.lineTo(sx + offset + size * 0.5, sy2);
    path.lineTo(sx + offset + size * 0.5, sy1);
    path.closePath();

    fillPath(layer, clip, path, style);
  }

  /* draw labels */
  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& text = config.labels[i];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    auto padding = measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingVertEM, config.label_font_size));

    Point p(
        clip.x + offset + config.x[i],
        clip.y + clip.h - config.y[i] - padding);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::BOTTOM;
    if (auto rc = drawTextLabel(text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw(
    const PlotBarsConfig& config,
    const LayoutInfo& layout,
    Layer* layer) {
  switch (config.direction) {
    case Direction::HORIZONTAL:
      return draw_horizontal(config, layout, layer);
    case Direction::VERTICAL:
      return draw_vertical(config, layout, layer);
    default:
      return ERROR;
  }
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    PlotBarsConfig* config) {
  /* set defaults from environment */
  config->scale_x = env.scale_x;
  config->scale_y = env.scale_y;
  config->label_font = env.font;
  config->label_font_size = env.font_size;

  ParserDefinitions pdefs = {
    {"xs", bind(&configure_measures, _1, &config->x)},
    {"ys", bind(&configure_measures, _1, &config->y)},
    {"x-offsets", bind(&configure_measures, _1, &config->xoffset)},
    {"y-offsets", bind(&configure_measures, _1, &config->yoffset)},
    {"scale-x", bind(&domain_configure, _1, &config->scale_x)},
    {"scale-x-min", bind(&configure_float_opt, _1, &config->scale_x.min)},
    {"scale-x-max", bind(&configure_float_opt, _1, &config->scale_x.max)},
    {"scale-x-padding", bind(&configure_float, _1, &config->scale_x.padding)},
    {"scale-y", bind(&domain_configure, _1, &config->scale_y)},
    {"scale-y-min", bind(&configure_float_opt, _1, &config->scale_y.min)},
    {"scale-y-max", bind(&configure_float_opt, _1, &config->scale_y.max)},
    {"scale-y-padding", bind(&configure_float, _1, &config->scale_y.padding)},
    {"size", bind(&configure_measures, _1, &config->sizes)},
    {"sizes", bind(&configure_measures, _1, &config->sizes)},
    {"offset", bind(&configure_measures, _1, &config->offsets)},
    {"offsets", bind(&configure_measures, _1, &config->offsets)},
    {"direction", bind(&configure_direction, _1, &config->direction)},
    {"color", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
    {"colors", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
    {"labels", bind(&configure_strings, _1, &config->labels)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  /* check configuraton */
  if (config->x.size() != config->y.size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'xs' and 'ys' properties must be equal");
  }

  if (!config->xoffset.empty() &&
      config->xoffset.size() != config->x.size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'xs' and 'x-offsets' properties must be equal");
  }

  if (!config->yoffset.empty() &&
      config->yoffset.size() != config->y.size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'ys' and 'y-offsets' properties must be equal");
  }

  /* scale autoconfig */
  for (const auto& v : config->x) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->xoffset) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->y) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_y);
    }
  }

  for (const auto& v : config->yoffset) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_y);
    }
  }

  return OK;
}

} // namespace bars
} // namespace plot
} // namespace fviz

