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
#include <graphics/text.h>
#include <graphics/text_shaper.h>
#include "graphics/text_layout.h"
#include <graphics/layer.h>

namespace fviz {

TextStyle::TextStyle() :
    direction(TextDirection::LTR) {}

Status drawTextLabel(
    const text::TextSpan* text_begin,
    const text::TextSpan* text_end,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    const TextStyle& style,
    Layer* layer) {
  std::string text;
  for (auto text_iter = text_begin; text_iter != text_end; ++text_iter) {
    text += text_iter->text;
  }

  Rectangle bbox;
  std::vector<text::GlyphSpan> glyphs;
  auto rc = text::text_layout_hline(
      text_begin,
      text_end,
      style.direction,
      style.font,
      style.font_size,
      layer->dpi,
      &glyphs,
      &bbox);

  auto offset = layout_align(bbox, position, align_x, align_y);

  if (rc != OK) {
    return rc;
  }

  for (auto& gg : glyphs) {
    for (auto& g : gg.glyphs) {
      g.x += offset.x;
      g.y += offset.y;
    }
  }

  StrokeStyle ss;
  ss.line_width = from_px(1.0);
  ss.color = Color::fromRGB(0.5,0.5,0.5);

  layer_ops::TextSpanOp op;
  op.text = text;
  op.rotate = rotate;
  op.rotate_pivot = position;
  op.style = style;
  op.spans = std::move(glyphs);
  op.origin = offset;

  return layer->apply(op);
}

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    const TextStyle& style,
    Layer* layer) {
  text::TextSpan span;
  span.text = text;
  return drawTextLabel(&span, &span + 1, position, align_x, align_y, 0, style, layer);
}

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    const TextStyle& style,
    Layer* layer) {
  return drawTextLabel(text, position, align_x, align_y, 0, style, layer);
}

} // namespace fviz

