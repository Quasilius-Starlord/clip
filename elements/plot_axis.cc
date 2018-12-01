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

AxisDefinition::AxisDefinition() :
    mode(AxisMode::AUTO),
    label_placement(AxisLabelPlacement::OUTSIDE),
    label_formatter(format_decimal_fixed(1)),
    label_padding_rem(1.0f),
    label_font_size_rem(1.0f),
    tick_length_rem(0.4f) {}

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
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], value);
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
    strokeLine(target, x, y0, x, y1, style);
  }

  double label_placement = 0;
  switch (axis_config.label_placement) {
    case AxisLabelPlacement::RIGHT:
      label_placement = 1;
      break;
    case AxisLabelPlacement::LEFT:
      label_placement = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.ticks) {
    auto y = y0 + (y1 - y0) * (1.0 - tick);
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    strokeLine(
        target,
        x,
        y,
        x + from_rem(*target, axis_config.tick_length_rem) * label_placement,
        y,
        style);
  }

  /* draw labels */
  auto label_padding = from_rem(*target, axis_config.label_padding_rem);
  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;
    auto sy = y0 + (y1 - y0) * (1.0 - tick);
    auto sx = x + label_padding * label_placement;

    TextStyle style;
    style.font_file = axis_config.font;
    style.colour = axis_config.text_colour;
    style.halign = label_placement > 0 ? TextHAlign::LEFT : TextHAlign::RIGHT;
    style.valign = TextVAlign::MIDDLE;
    if (auto rc = drawText(label_text, sx, sy, style, target); rc != OK) {
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
    strokeLine(target, x0, y, x1, y, style);
  }

  double label_placement = 0;
  switch (axis_config.label_placement) {
    case AxisLabelPlacement::BOTTOM:
      label_placement = 1;
      break;
    case AxisLabelPlacement::TOP:
      label_placement = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.ticks) {
    auto x = x0 + (x1 - x0) * tick;
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    strokeLine(
        target,
        x,
        y,
        x,
        y + from_rem(*target, axis_config.tick_length_rem) * label_placement,
        style);
  }

  /* draw labels */
  auto label_padding = from_rem(*target, axis_config.label_padding_rem);
  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;
    auto sx = x0 + (x1 - x0) * tick;
    auto sy = y + label_padding * label_placement;

    TextStyle style;
    style.font_file = axis_config.font;
    style.halign = TextHAlign::CENTER;
    style.valign = label_placement > 0 ? TextVAlign::TOP : TextVAlign::BOTTOM;
    style.colour = axis_config.text_colour;
    if (auto rc = drawText(label_text, sx, sy, style, target); rc) {
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

  auto padding = from_rem(*frame, 1); // FIXME

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
  }

  return rc;
}

ReturnCode axis_expand_linear_geom(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  uint32_t num_ticks = 6; // FIXME make configurable
  double min = domain.min.value_or(0.0f);
  double max = domain.max.value_or(0.0f);

  for (size_t i = 0; i < num_ticks; ++i) {
    auto o = (1.0f / (num_ticks - 1)) * i;
    auto v = domain_untranslate(domain, o);
    axis->ticks.emplace_back(o);

    if (axis->label_formatter.format_number) {
      axis->labels.emplace_back(o, axis->label_formatter.format_number(v));
    }
  }

  return OK;
}

ReturnCode axis_expand_auto(
    const AxisDefinition& in,
    const AxisPosition& pos,
    const DomainConfig& domain,
    AxisDefinition* out) {
  *out = in;

  switch (out->label_placement) {
    case AxisLabelPlacement::OUTSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->label_placement = AxisLabelPlacement::TOP;
          break;
        case AxisPosition::RIGHT:
          out->label_placement = AxisLabelPlacement::RIGHT;
          break;
        case AxisPosition::BOTTOM:
          out->label_placement = AxisLabelPlacement::BOTTOM;
          break;
        case AxisPosition::LEFT:
          out->label_placement = AxisLabelPlacement::LEFT;
          break;
      }
      break;
    case AxisLabelPlacement::INSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->label_placement = AxisLabelPlacement::BOTTOM;
          break;
        case AxisPosition::RIGHT:
          out->label_placement = AxisLabelPlacement::LEFT;
          break;
        case AxisPosition::BOTTOM:
          out->label_placement = AxisLabelPlacement::TOP;
          break;
        case AxisPosition::LEFT:
          out->label_placement = AxisLabelPlacement::RIGHT;
          break;
      }
      break;
  };

  return axis_expand_linear_geom(domain, out); // FIXME
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

} // namespace plotfx

