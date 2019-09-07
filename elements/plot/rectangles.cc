/**
 * This file is part of the "clip" project
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
#include "rectangles.h"
#include "data.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "core/environment.h"
#include "core/color_reader.h"
#include "core/typographic_map.h"
#include "core/typographic_reader.h"
#include "core/layout.h"
#include "core/marker.h"
#include "core/scale.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::elements::plot::rectangles {

static const double kDefaultPointSizePT = 4;
static const double kDefaultLabelPaddingEM = 0.2;

struct PlotRectanglesConfig {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::vector<Measure> size_x;
  std::vector<Measure> size_y;
  Measure size_x_default;
  Measure size_y_default;
  Color color;
  std::vector<Color> colors;
  Measure size;
  LayoutSettings layout;
};

ReturnCode draw(
    std::shared_ptr<PlotRectanglesConfig> config,
    const LayoutInfo& layout,
    const Page& page,
    PageElementList* page_elements) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, page.dpi, page.font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x.begin(),
      &*config->x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, page.dpi, page.font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, page.dpi, page.font_size, _1),
        bind(&convert_unit_user, scale_translate_magnitude_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->size_x.begin(),
      &*config->size_x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, page.dpi, page.font_size, _1),
        bind(&convert_unit_user, scale_translate_magnitude_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->size_y.begin(),
      &*config->size_y.end());

  convert_unit(
      {
        bind(&convert_unit_typographic, page.dpi, page.font_size, _1),
        bind(&convert_unit_user, scale_translate_magnitude_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &config->size_x_default);

  convert_unit(
      {
        bind(&convert_unit_typographic, page.dpi, page.font_size, _1),
        bind(&convert_unit_user, scale_translate_magnitude_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &config->size_y_default);

  /* draw markers */
  for (size_t i = 0; i < config->x.size(); ++i) {
    auto sx = clip.x + config->x[i];
    auto sy = clip.y + config->y[i];

    const auto& color = config->colors.empty()
        ? config->color
        : config->colors[i % config->colors.size()];

    double size_x = config->size_x.empty()
        ? config->size_x_default
        : config->size_x[i % config->size_x.size()];

    double size_y = config->size_y.empty()
        ? config->size_y_default
        : config->size_y[i % config->size_y.size()];

    PageShapeElement rect;
    rect.fill_style.color = color;
    rect.antialiasing_mode = AntialiasingMode::DISABLE;
    path_add_rectangle(&rect.path, Point(sx, sy), {size_x, size_y});
    page_add_shape(page_elements, rect);
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* set defaults from environment */
  auto c = std::make_shared<PlotRectanglesConfig>();
  c->color = env.foreground_color;
  c->size = from_pt(kDefaultPointSizePT);

  /* parse properties */
  std::vector<std::string> data_x;
  std::vector<std::string> data_y;
  std::vector<std::string> data_colors;
  std::vector<std::string> data_size_x;
  std::vector<std::string> data_size_y;
  ColorMap color_map;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"data-x", bind(&data_load_strings, _1, &data_x)},
    {"data-y", bind(&data_load_strings, _1, &data_y)},
    {"data-color", bind(&data_load_strings, _1, &data_colors)},
    {
      "data-size",
      expr_calln_fn({
        bind(&data_load_strings, _1, &data_size_x),
        bind(&data_load_strings, _1, &data_size_y),
      })
    },
    {"data-size-x", bind(&data_load_strings, _1, &data_size_x)},
    {"data-size-y", bind(&data_load_strings, _1, &data_size_y)},
    {
      "size",
      expr_calln_fn({
        bind(&measure_read, _1, &c->size_x_default),
        bind(&measure_read, _1, &c->size_y_default),
      })
    },
    {"size-x", bind(&measure_read, _1, &c->size_x_default)},
    {"size-y", bind(&measure_read, _1, &c->size_y_default)},
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &c->scale_x.min, &c->scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &c->scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &c->scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &c->scale_y.min, &c->scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &c->scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &c->scale_y.max)},
    {"scale-x", bind(&scale_configure_kind, _1, &c->scale_x)},
    {"scale-y", bind(&scale_configure_kind, _1, &c->scale_y)},
    {"scale-x-padding", bind(&expr_to_float64, _1, &c->scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &c->scale_y.padding)},
    {"color", bind(&color_read, env, _1, &c->color)},
    {"color-map", bind(&color_map_read, env, _1, &color_map)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* scale configuration */
  if (auto rc = data_to_measures(data_x, c->scale_x, &c->x); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_y, c->scale_y, &c->y); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_size_x, c->scale_x, &c->size_x); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_size_y, c->scale_y, &c->size_y); !rc){
    return rc;
  }

  for (const auto& v : c->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_y);
    }
  }

  /* check configuration */
  if (c->x.size() != c->y.size()) {
    return error(
        ERROR,
        "the length of the 'data-x' and 'data-y' properties must be equal");
  }

  /* convert color data */
  for (const auto& value : data_colors) {
    Color color;
    if (color_map) {
      if (auto rc = color_map(value, &color); !rc) {
        return rc;
      }
    } else {
      if (auto rc = color.parse(value); !rc) {
        return errorf(
            ERROR,
            "invalid data; can't parse '{}' as a color hex code; maybe you "
            "forgot to set the 'color-map' option?",
            value);
      }
    }

    c->colors.push_back(color);
  }

  /* return element */
  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, c, _1, _2, _3);
  return OK;
}

} // namespace clip::elements::plot::rectangles

