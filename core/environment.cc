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
#include "environment.h"
#include "core/format.h"
#include "color_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "graphics/font_lookup.h"

#include "elements/text.h"
#include "elements/plot.h"
#include "elements/plot/areas.h"
#include "elements/plot/axis.h"
#include "elements/plot/bars.h"
#include "elements/plot/errorbars.h"
#include "elements/plot/grid.h"
#include "elements/plot/labels.h"
#include "elements/plot/lines.h"
#include "elements/plot/points.h"
#include "elements/plot/vectors.h"
#include "elements/chart/linechart.h"
#include "elements/chart/scatterplot.h"
#include "elements/layout/box.h"
#include "elements/legend.h"
#include "elements/legend/item.h"

#include <functional>

using namespace std::placeholders;
using std::bind;

namespace fviz {

Environment::Environment() :
    screen_width(Unit::UNIT, 900),
    screen_height(Unit::UNIT, 480),
    dpi(96),
    font_defaults(true),
    background_color(Color::fromRGB(1,1,1)),
    foreground_color(Color::fromRGB(0,0,0)),
    text_color(Color::fromRGB(0,0,0)),
    font_size(from_pt(11)),
    color_palette(color_palette_default()) {}

ReturnCode environment_setup_defaults(Environment* env) {
  auto elems = &env->element_map;
  element_bind(elems, "chart/linechart", bind(elements::chart::linechart::build, _1, _2, _3));
  element_bind(elems, "chart/scatterplot", bind(elements::chart::scatterplot::build, _1, _2, _3));
  element_bind(elems, "plot", bind(elements::plot::build, _1, _2, _3));
  element_bind(elems, "plot/areas", bind(elements::plot::areas::build, _1, _2, _3));
  element_bind(elems, "plot/axis", bind(elements::plot::axis::build, _1, _2, _3));
  element_bind(elems, "plot/bars", bind(elements::plot::bars::build, _1, _2, _3));
  element_bind(elems, "plot/errorbars", bind(elements::plot::errorbars::build, _1, _2, _3));
  element_bind(elems, "plot/grid", bind(elements::plot::grid::build, _1, _2, _3));
  element_bind(elems, "plot/labels", bind(elements::plot::labels::build, _1, _2, _3));
  element_bind(elems, "plot/lines", bind(elements::plot::lines::build, _1, _2, _3));
  element_bind(elems, "plot/points", bind(elements::plot::points::build, _1, _2, _3));
  element_bind(elems, "plot/vectors", bind(elements::plot::vectors::build, _1, _2, _3));
  element_bind(elems, "legend", bind(elements::legend::build, _1, _2, _3));
  element_bind(elems, "legend/item", bind(elements::legend::item::build, _1, _2, _3));
  element_bind(elems, "layout/box", bind(elements::layout::box::build, _1, _2, _3));
  element_bind(elems, "text", bind(elements::text::build, _1, _2, _3));

  if (env->font_defaults) {
    if (auto rc = font_load_defaults(&env->font); !rc) {
      return rc;
    }
  }

  for (const auto& f : env->font_load) {
    if (auto rc = font_load_best(f, &env->font); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode environment_set(Environment* env, const Expr* expr) {
  auto args = expr_collect(expr);
  if (args.size() < 2) {
    return error(ERROR, "'set' expects two arguments");
  }

  if (expr_is_value(args[0], "width")) {
    return measure_read(args[1], &env->screen_width);
  }

  if (expr_is_value(args[0], "height")) {
    return measure_read(args[1], &env->screen_height);
  }

  if (expr_is_value(args[0], "dpi")) {
    return expr_to_float64(args[1], &env->dpi);
  }

  if (expr_is_value(args[0], "background-color")) {
    return color_read(*env, args[1], &env->background_color);
  }

  if (expr_is_value(args[0], "foreground-color")) {
    return color_read(*env, args[1], &env->foreground_color);
  }

  if (expr_is_value(args[0], "text-color")) {
    return color_read(*env, args[1], &env->text_color);
  }

  if (expr_is_value(args[0], "font")) {
    return expr_to_strings_flat(args[1], &env->font_load);
  }

  if (expr_is_value(args[0], "font-defaults")) {
    return expr_to_switch(args[1], &env->font_defaults);
  }

  if (expr_is_value(args[0], "font-size")) {
    return measure_read(args[1], &env->font_size);
  }

  if (expr_is_value(args[0], "color-palette")) {
    return color_palette_read(*env, args[1], &env->color_palette);
  }

  if (expr_is_value(args[0], "margin-top")) {
    return measure_read(args[1], &env->margins[0]);
  }

  if (expr_is_value(args[0], "margin-right")) {
    return measure_read(args[1], &env->margins[1]);
  }

  if (expr_is_value(args[0], "margin-bottom")) {
    return measure_read(args[1], &env->margins[2]);
  }

  if (expr_is_value(args[0], "margin-left")) {
    return measure_read(args[1], &env->margins[3]);
  }

  if (expr_is_value(args[0], "text-script")) {
    return expr_to_string(args[1], &env->text_default_script);
  }

  if (expr_is_value(args[0], "text-language")) {
    return expr_to_string(args[1], &env->text_default_language);
  }

  if (expr_is_value(args[0], "margin")) {
    return expr_calln(args[1], {
      bind(&measure_read, _1, &env->margins[0]),
      bind(&measure_read, _1, &env->margins[1]),
      bind(&measure_read, _1, &env->margins[2]),
      bind(&measure_read, _1, &env->margins[3]),
    });
  }

  return errorf(ERROR, "invalid property: {}", expr_get_value(args[0]));
}

ReturnCode environment_configure(Environment* env, const Expr* expr) {
  /* parse options */
  for (; expr; expr = expr_next(expr)) {
    if (!expr || !expr_is_list(expr)) {
      return errorf(ERROR, "expected a list but got: {}", expr_inspect(expr));
    }

    auto args = expr_get_list(expr);
    if (!args || !expr_is_value(args)) {
      return error(ERROR, "expected an element name");
    }

    auto arg0 = expr_get_value(args);
    if (arg0 == "set" ) {
      if (auto rc = environment_set(env, expr_next(args)); !rc) {
        return rc;
      }
    }
  }

  /* convert units */
  convert_unit_typographic(env->dpi, env->font_size, &env->font_size);

  for (auto& m : env->margins) {
    convert_unit_typographic(env->dpi, env->font_size, &m);
  }

  return OK;
}

} // namespace fviz

