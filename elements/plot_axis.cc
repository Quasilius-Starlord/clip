/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "plot_axis.h"
#include <assert.h>
#include <iostream>
#include <common/config_helpers.h>
#include <common/domain.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include <graphics/brush.h>

namespace plotfx {

static const double kDefaultLabelPaddingEM = 0.8;
static const double kDefaultLabelFontSizeEM = 1;
static const double kDefaultLineWidthPT = 1;
static const double kDefaultTickLengthPT = 4;

AxisDefinition::AxisDefinition() :
    mode(AxisMode::AUTO),
    label_position(AxisLabelPosition::OUTSIDE),
    tick_position(AxisLabelPosition::INSIDE),
    label_formatter(format_decimal_fixed(1)) {}

ReturnCode parseAxisMode(
    const std::string& str,
    AxisMode* value) {
  static const EnumDefinitions<AxisMode> defs = {
    { "auto", AxisMode::AUTO },
    { "off", AxisMode::OFF },
    { "manual", AxisMode::MANUAL },
  };

  return parseEnum(defs, str, value);
}

ReturnCode parseAxisModeProp(const plist::Property& prop, AxisMode* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop, value);
}

static Status renderAxisVertical(
    const AxisDefinition& axis_config,
    double x,
    double y0,
    double y1,
    Layer* target) {
  /* draw axis line */
  {
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(target, {x, y0}, {x, y1}, style);
  }

  /* draw ticks */
  double tick_position = 0;
  switch (axis_config.tick_position) {
    case AxisLabelPosition::RIGHT:
      tick_position = 1;
      break;
    case AxisLabelPosition::LEFT:
      tick_position = -1;
      break;
    default:
      break;
  }

  auto tick_length = measure_or(
      axis_config.tick_length,
      from_pt(kDefaultTickLengthPT, target->dpi));

  for (const auto& tick : axis_config.ticks) {
    auto y = y0 + (y1 - y0) * (1.0 - tick);
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(
        target,
        {x, y},
        {x + tick_length * tick_position, y},
        style);
  }

  /* draw labels */
  double label_position = 0;
  switch (axis_config.label_position) {
    case AxisLabelPosition::RIGHT:
      label_position = 1;
      break;
    case AxisLabelPosition::LEFT:
      label_position = -1;
      break;
    default:
      break;
  }

  auto label_padding = measure_or(
      axis_config.label_padding,
      from_em(kDefaultLabelPaddingEM, target->font_size));

  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;

    Point p;
    p.x = x + label_padding * label_position;
    p.y = y0 + (y1 - y0) * (1.0 - tick);

    TextStyle style;
    style.font = axis_config.font;
    style.colour = axis_config.text_colour;
    style.font_size = from_em(kDefaultLabelFontSizeEM, target->font_size);

    auto ax = label_position > 0 ? HAlign::LEFT : HAlign::RIGHT;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); rc != OK) {
      return rc;
    }
  }

  return OK;
}

static Status renderAxisHorizontal(
    const AxisDefinition& axis_config,
    double y,
    double x0,
    double x1,
    Layer* target) {
  /* draw axis line */ 
  {
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(target, {x0, y}, {x1, y}, style);
  }

  /* draw ticks */
  double tick_position = 0;
  switch (axis_config.tick_position) {
    case AxisLabelPosition::BOTTOM:
      tick_position = 1;
      break;
    case AxisLabelPosition::TOP:
      tick_position = -1;
      break;
    default:
      break;
  }

  auto tick_length = measure_or(
      axis_config.tick_length,
      from_pt(kDefaultTickLengthPT, target->dpi));

  for (const auto& tick : axis_config.ticks) {
    auto x = x0 + (x1 - x0) * tick;
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(
        target,
        {x, y},
        {x, y + tick_length * tick_position},
        style);
  }

  /* draw labels */
  double label_position = 0;
  switch (axis_config.label_position) {
    case AxisLabelPosition::BOTTOM:
      label_position = 1;
      break;
    case AxisLabelPosition::TOP:
      label_position = -1;
      break;
    default:
      break;
  }

  auto label_padding = measure_or(
      axis_config.label_padding,
      from_em(kDefaultLabelPaddingEM, target->font_size));

  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;
    Point p;
    p.x = x0 + (x1 - x0) * tick;
    p.y = y + label_padding * label_position;

    TextStyle style;
    style.font = axis_config.font;
    style.colour = axis_config.text_colour;
    style.font_size = from_em(kDefaultLabelFontSizeEM, target->font_size);

    auto ax = HAlign::CENTER;
    auto ay = label_position > 0 ? VAlign::TOP : VAlign::BOTTOM;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); rc) {
      return rc;
    }
  }

  return OK;
}

Status renderAxis(
    const AxisDefinition& axis,
    const Rectangle& clip,
    AxisPosition axis_position,
    Layer* frame) {
  switch (axis.mode) {
    case AxisMode::OFF:
      return OK;
    default:
      break;
  };

  Status rc;
  switch (axis_position) {
    case AxisPosition::LEFT:
      rc = renderAxisVertical(
          axis,
          clip.x,
          clip.y,
          clip.y + clip.h,
          frame);
      break;
    case AxisPosition::RIGHT:
      rc = renderAxisVertical(
          axis,
          clip.x + clip.w,
          clip.y,
          clip.y + clip.h,
          frame);
      break;
    case AxisPosition::TOP:
      rc = renderAxisHorizontal(
          axis,
          clip.y,
          clip.x,
          clip.x + clip.w,
          frame);
      break;
    case AxisPosition::BOTTOM:
      rc = renderAxisHorizontal(
          axis,
          clip.y + clip.h,
          clip.x,
          clip.x + clip.w,
          frame);
      break;
    case AxisPosition::CENTER_HORIZ:
    case AxisPosition::CENTER_VERT:
      return ERROR_NOT_IMPLEMENTED;
  }

  return rc;
}

ReturnCode axis_expand_auto(
    const AxisDefinition& in,
    const AxisPosition& pos,
    const DomainConfig& domain,
    AxisDefinition* out) {
  *out = in;

  switch (out->tick_position) {
    case AxisLabelPosition::OUTSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->tick_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::RIGHT:
          out->tick_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::BOTTOM:
          out->tick_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::LEFT:
          out->tick_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR_NOT_IMPLEMENTED;
      }
      break;
    case AxisLabelPosition::INSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->tick_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::RIGHT:
          out->tick_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::BOTTOM:
          out->tick_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::LEFT:
          out->tick_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR_NOT_IMPLEMENTED;
      }
      break;
    default:
      break;
  };

  switch (out->label_position) {
    case AxisLabelPosition::OUTSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->label_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::RIGHT:
          out->label_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::BOTTOM:
          out->label_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::LEFT:
          out->label_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR_NOT_IMPLEMENTED;
      }
      break;
    case AxisLabelPosition::INSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->label_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::RIGHT:
          out->label_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::BOTTOM:
          out->label_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::LEFT:
          out->label_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR_NOT_IMPLEMENTED;
      }
      break;
    default:
      break;
  };

  if (in.label_placement) {
    if (auto rc = in.label_placement(domain, out); !rc) {
      return rc;
    }
  } else {
    if (auto rc = axis_place_labels_default(domain, out); !rc) {
      return rc;
    }
  }

  return OK;
}

void axis_layout(
    const AxisDefinition& axis,
    const AxisPosition& axis_position,
    double* margin) {
  /* add margin for ticks */
  bool reflow_ticks = false;
  switch (axis.label_position) {
    case AxisLabelPosition::OUTSIDE:
      reflow_ticks = true;
      break;
    case AxisLabelPosition::INSIDE:
      reflow_ticks = false;
      break;
    case AxisLabelPosition::TOP:
      reflow_ticks = (axis_position == AxisPosition::TOP);
      break;
    case AxisLabelPosition::RIGHT:
      reflow_ticks = (axis_position == AxisPosition::RIGHT);
      break;
    case AxisLabelPosition::BOTTOM:
      reflow_ticks = (axis_position == AxisPosition::BOTTOM);
      break;
    case AxisLabelPosition::LEFT:
      reflow_ticks = (axis_position == AxisPosition::LEFT);
      break;
  }

  /* add margin for labels */
  bool reflow_labels = true;
  switch (axis.label_position) {
    case AxisLabelPosition::OUTSIDE:
      reflow_labels = true;
      break;
    case AxisLabelPosition::INSIDE:
      reflow_labels = false;
      break;
    case AxisLabelPosition::TOP:
      reflow_labels = (axis_position == AxisPosition::TOP);
      break;
    case AxisLabelPosition::RIGHT:
      reflow_labels = (axis_position == AxisPosition::RIGHT);
      break;
    case AxisLabelPosition::BOTTOM:
      reflow_labels = (axis_position == AxisPosition::BOTTOM);
      break;
    case AxisLabelPosition::LEFT:
      reflow_labels = (axis_position == AxisPosition::LEFT);
      break;
  }

  if (reflow_labels) {
    *margin += 0;
  }
}

ReturnCode axis_layout(
    const Rectangle& parent,
    const AxisDefinition& axis_top,
    const AxisDefinition& axis_right,
    const AxisDefinition& axis_bottom,
    const AxisDefinition& axis_left,
    Rectangle* bbox) {
  double margins[4] = {0, 0, 0, 0};
  axis_layout(axis_top, AxisPosition::TOP, &margins[0]);
  axis_layout(axis_right, AxisPosition::RIGHT, &margins[1]);
  axis_layout(axis_bottom, AxisPosition::BOTTOM, &margins[2]);
  axis_layout(axis_left, AxisPosition::LEFT, &margins[3]);

  *bbox = layout_margin_box(
      parent,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  return OK;
}

ReturnCode axis_draw_all(
    const Rectangle& clip,
    const DomainConfig& domain_x,
    const DomainConfig& domain_y,
    const AxisDefinition& axis_top_,
    const AxisDefinition& axis_right_,
    const AxisDefinition& axis_bottom_,
    const AxisDefinition& axis_left_,
    Layer* layer) {
  AxisDefinition axis_top;
  if (auto rc = axis_expand_auto(axis_top_, AxisPosition::TOP, domain_x, &axis_top); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_top, clip, AxisPosition::TOP, layer); rc) {
    return rc;
  }

  AxisDefinition axis_right;
  if (auto rc = axis_expand_auto(axis_right_, AxisPosition::RIGHT, domain_y, &axis_right); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_right, clip, AxisPosition::RIGHT, layer); rc) {
    return rc;
  }

  AxisDefinition axis_bottom;
  if (auto rc = axis_expand_auto(axis_bottom_, AxisPosition::BOTTOM, domain_x, &axis_bottom); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_bottom, clip, AxisPosition::BOTTOM, layer); rc) {
    return rc;
  }

  AxisDefinition axis_left;
  if (auto rc = axis_expand_auto(axis_left_, AxisPosition::LEFT, domain_y, &axis_left); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_left, clip, AxisPosition::LEFT, layer); rc) {
    return rc;
  }

  return OK;
}

ReturnCode axis_place_labels_default(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  if (domain.kind == DomainKind::CATEGORICAL) {
    return axis_place_labels_categorical(domain, axis);
  }

  return axis_place_labels_geom(domain, axis);
}

ReturnCode axis_place_labels_geom(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  uint32_t num_ticks = 8; // FIXME make configurable
  double min = domain.min.value_or(0.0f);
  double max = domain.max.value_or(0.0f);

  axis->ticks.clear();
  axis->labels.clear();

  for (size_t i = 0; i < num_ticks; ++i) {
    axis->ticks.emplace_back((1.0f / (num_ticks - 1)) * i);
  }

  auto tick_values = domain_untranslate(domain, axis->ticks);
  for (size_t i = 0; i < num_ticks; ++i) {
    axis->labels.emplace_back(
        axis->ticks[i],
        axis->label_formatter.format_value(tick_values[i]));
  }

  return OK;
}

ReturnCode axis_place_labels_categorical(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  if (domain.kind != DomainKind::CATEGORICAL) {
    return ReturnCode::error(
        "EARG",
        "axis-label-placement: categorial is invalid for non-categorical domains");
  }

  auto category_count = domain.categories.size();

  axis->labels.clear();
  axis->ticks.clear();
  axis->ticks.push_back(0.0f);

  std::vector<double> label_positions;
  for (size_t i = 0; i < category_count; ++i) {
    auto o = (1.0f / category_count) * (i + 1);
    label_positions.push_back(o - 0.5 / category_count);
    axis->ticks.push_back(o);
  }

  auto label_values = domain_untranslate(domain, label_positions);
  for (size_t i = 0; i < label_positions.size(); ++i) {
    axis->labels.emplace_back(
        label_positions[i],
        axis->label_formatter.format_value(label_values[i]));
  }

  return OK;
}

ReturnCode axis_configure_label_placement(
    const plist::Property& prop,
    AxisLabelPlacement* label_placement) {
  if (prop.size() < 1) {
    return ERROR_INVALID_ARGUMENT;
  }

  if (plist::is_value(prop, "geometric") ||
      plist::is_enum(prop, "geometric")) {
    *label_placement = std::bind(
        &axis_place_labels_geom,
        std::placeholders::_1,
        std::placeholders::_2);

    return OK;
  }

  if (plist::is_value(prop, "categorical") ||
      plist::is_enum(prop, "categorical")) {
    *label_placement = std::bind(
        &axis_place_labels_geom,
        std::placeholders::_1,
        std::placeholders::_2);

    return OK;
  }

  return ERROR_INVALID_ARGUMENT;
}

} // namespace plotfx

