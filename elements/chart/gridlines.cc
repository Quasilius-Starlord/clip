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
#include "gridlines.h"
#include "environment.h"
#include "format.h"
#include "layout.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "graphics/brush.h"
#include "scale.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

using namespace std::placeholders;

namespace fviz::elements::chart::gridlines {

static const double kDefaultLineWidthPT = 1;

using GridlineLayout = std::function<ReturnCode (
    const ScaleConfig& domain,
    std::vector<double>*)>;

struct GridlineDefinition {
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  ScaleLayoutFn layout_x;
  ScaleLayoutFn layout_y;
  Measure line_width;
  Color line_color;
};

ReturnCode draw(
    std::shared_ptr<GridlineDefinition> config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& bbox = layout.content_box;

  StrokeStyle style;
  style.line_width = config->line_width;
  style.color = config->line_color;

  ScaleLayout slayout_x;
  config->layout_x(config->scale_x, &slayout_x);

  ScaleLayout slayout_y;
  config->layout_y(config->scale_y, &slayout_y);

  for (const auto& tick : slayout_x.ticks) {
    auto line_x = bbox.x + bbox.w * tick;

    strokeLine(
        layer,
        Point(line_x, bbox.y),
        Point(line_x, bbox.y + bbox.h),
        style);
  }

  for (const auto& tick : slayout_y.ticks) {
    auto line_y = bbox.y + bbox.h * (1.0 - tick);

    strokeLine(
        layer,
        Point(bbox.x, line_y),
        Point(bbox.x + bbox.w, line_y),
        style);
  }

  return OK;
}

ReturnCode build(const Environment& env, const Expr* expr, ElementRef* elem) {
  /* set defaults from environment */
  auto config = std::make_shared<GridlineDefinition>();
  config->line_width = from_pt(1);
  config->line_color = Color::fromRGB(.9, .9, .9); // TODO
  config->layout_x = bind(&scale_layout_subdivide, _1, _2, 6);
  config->layout_y = bind(&scale_layout_subdivide, _1, _2, 6);

  /* parse properties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {"xmin", bind(&expr_to_float64_opt, _1, &config->scale_x.min)},
    {"xmax", bind(&expr_to_float64_opt, _1, &config->scale_x.max)},
    {"xlayout", bind(&scale_configure_layout, _1, &config->layout_x)},
    {"xscale", bind(&scale_configure_kind, _1, &config->scale_x)},
    {"xscale-padding", bind(&expr_to_float64, _1, &config->scale_x.padding)},
    {"ymin", bind(&expr_to_float64_opt, _1, &config->scale_y.min)},
    {"ymax", bind(&expr_to_float64_opt, _1, &config->scale_y.max)},
    {"ylayout", bind(&scale_configure_layout, _1, &config->layout_y)},
    {"yscale", bind(&scale_configure_kind, _1, &config->scale_y)},
    {"yscale-padding", bind(&expr_to_float64, _1, &config->scale_y.padding)},
    {"color", bind(&expr_to_color, _1, &config->line_color)},
    {"stroke", bind(&expr_to_measure, _1, &config->line_width)},
  });

  if (!config_rc) {
    return config_rc;
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::gridlines

