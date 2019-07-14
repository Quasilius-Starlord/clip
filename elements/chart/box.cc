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
#include "elements/box.h"
#include "element_factory.h"
#include "config_helpers.h"
#include "graphics/layer.h"

#include <functional>

using namespace std::placeholders;

namespace fviz {
namespace box {

ReturnCode draw(
    const BoxConfig& config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto bbox = layout.content_box;

  /* convert units  */
  auto margins = config.margins;
  for (auto& m : margins) {
    convert_unit_typographic(layer->dpi, config.font_size, &m);
  }

  /* calculate margin box */
  auto margin_box = layout_margin_box(
      bbox,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  /* layout children */
  std::vector<ElementPlacement> children;
  for (const auto& c : config.children) {
    ElementPlacement e;
    e.element = c;
    children.emplace_back(e);
  }

  Rectangle content_box;
  if (auto rc = layout_elements(*layer, margin_box, &children, &content_box); !rc) {
    return rc;
  }

  /* draw background */
  if (config.background) {
    const auto& bg_box = bbox;
    FillStyle bg_fill;
    bg_fill.color = *config.background;

    fillRectangle(
        layer,
        Point(bg_box.x, bg_box.y),
        bg_box.w,
        bg_box.h,
        bg_fill);
  }

  /* draw children */
  for (const auto& c : children) {
    if (auto rc = c.element->draw(c.layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  /* draw top border  */
  if (config.borders[0].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config.borders[0].width;
    border_style.color = config.borders[0].color;

    strokeLine(
        layer,
        Point(bbox.x, bbox.y),
        Point(bbox.x + bbox.w, bbox.y),
        border_style);
  }

  /* draw right border  */
  if (config.borders[1].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config.borders[1].width;
    border_style.color = config.borders[1].color;

    strokeLine(
        layer,
        Point(bbox.x + bbox.w, bbox.y),
        Point(bbox.x + bbox.w, bbox.y + bbox.h),
        border_style);
  }

  /* draw top border  */
  if (config.borders[2].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config.borders[2].width;
    border_style.color = config.borders[2].color;

    strokeLine(
        layer,
        Point(bbox.x, bbox.y + bbox.h),
        Point(bbox.x + bbox.w, bbox.y + bbox.h),
        border_style);
  }

  /* draw left border  */
  if (config.borders[3].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config.borders[3].width;
    border_style.color = config.borders[3].color;

    strokeLine(
        layer,
        Point(bbox.x, bbox.y),
        Point(bbox.x, bbox.y + bbox.h),
        border_style);
  }

  return OK;
}

ReturnCode reflow(
    const BoxConfig& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height) {
  return OK; // TODO
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    BoxConfig* config) {
  config->font = env.font;
  config->font_size = env.font_size;
  config->color_scheme = env.color_scheme;
  config->text_color = env.text_color;
  config->border_color = env.border_color;

  ParserDefinitions pdefs = {
    {"position", bind(&configure_position, _1, &config->layout.position)},
    {"width", bind(&configure_measure_opt, _1, &config->layout.width)},
    {"height", bind(&configure_measure_opt, _1, &config->layout.height)},
    {
      "margin",
      configure_multiprop({
        bind(&configure_measure, _1, &config->margins[0]),
        bind(&configure_measure, _1, &config->margins[1]),
        bind(&configure_measure, _1, &config->margins[2]),
        bind(&configure_measure, _1, &config->margins[3]),
      })
    },
    {"margin-top", bind(&configure_measure, _1, &config->margins[0])},
    {"margin-right", bind(&configure_measure, _1, &config->margins[1])},
    {"margin-bottom", bind(&configure_measure, _1, &config->margins[2])},
    {"margin-left", bind(&configure_measure, _1, &config->margins[3])},
    {"border-top-color", bind(&configure_color, _1, &config->borders[0].color)},
    {"border-right-color", bind(&configure_color, _1, &config->borders[1].color)},
    {"border-bottom-color", bind(&configure_color, _1, &config->borders[2].color)},
    {"border-left-color", bind(&configure_color, _1, &config->borders[3].color)},
    {"border-top-width", bind(&configure_measure, _1, &config->borders[0].width)},
    {"border-right-width", bind(&configure_measure, _1, &config->borders[1].width)},
    {"border-bottom-width", bind(&configure_measure, _1, &config->borders[2].width)},
    {"border-left-width", bind(&configure_measure, _1, &config->borders[3].width)},
    {"background-color", configure_color_opt(&config->background)},
    {
      "foreground-color",
      configure_multiprop({
          bind(&configure_color, _1, &config->text_color),
          bind(&configure_color, _1, &config->border_color),
      })
    },
    {"text-color", bind(&configure_color, _1, &config->text_color)},
    {"border-color", bind(&configure_color, _1, &config->border_color)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  Environment child_env;
  child_env.screen_width = env.screen_width;
  child_env.screen_height = env.screen_height;
  child_env.dpi = env.dpi;
  child_env.font = config->font;
  child_env.font_size = config->font_size;
  child_env.color_scheme = config->color_scheme;
  child_env.text_color = config->text_color;
  child_env.border_color = config->border_color;
  child_env.background_color = config->background.value_or(env.background_color);

  for (size_t i = 0; i < plist.size(); ++i) {
    if (!plist::is_map(plist[i])) {
      continue;
    }

    const auto& elem_name = plist[i].name;
    const auto& elem_config = plist[i].next.get();

    ElementRef elem;
    auto rc = buildElement(
        elem_name,
        *elem_config,
        child_env,
        &elem);

    if (!rc) {
      return rc;
    }

    config->children.emplace_back(std::move(elem));
  }

  return OK;
}

} // namespace box
} // namespace fviz

