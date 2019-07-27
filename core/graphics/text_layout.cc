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
#include "fviz.h"
#include "graphics/text_backend.h"
#include "graphics/text_layout.h"
#include "graphics/text_shaper.h"

namespace fviz {
namespace text {

Status text_layout_hrun(
    const std::string& text_logical,
    const TextDirection text_direction,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphPlacement>* glyphs,
    double* span_length,
    double* span_top,
    double* span_bottom) {
  std::vector<GlyphInfo> glyph_list;
  auto shaping_rc = text_shape_run_with_font_fallback(
      text_logical,
      text_direction,
      font_info,
      font_size,
      dpi,
      &glyph_list);

  if (!shaping_rc) {
    return shaping_rc;
  }

  for (const auto& gi : glyph_list) {
    GlyphPlacement gp;
    gp.font = gi.font;
    gp.codepoint = gi.codepoint;
    gp.y = 0;

    switch (text_direction) {
      case TextDirection::LTR:
        gp.x = *span_length;
        break;
      case TextDirection::RTL:
        gp.x = -*span_length - gi.advance_x;
        break;
    }

    glyphs->emplace_back(gp);

    *span_length += gi.advance_x;
    *span_top = std::min(-gi.metrics_ascender, *span_top);
    *span_bottom = std::max(-gi.metrics_descender, *span_bottom);
  }

  return OK;
}

Status text_layout_hline(
    const std::string& text_logical,
    const TextDirection base_text_direction,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphSpan>* glyph_spans,
    Rectangle* bbox) {
  TextLine text_line;
  text_analyze_bidi_line(text_logical, base_text_direction, &text_line);

  double line_top = 0.0;
  double line_bottom = 0.0;
  double line_length = 0;
  for (size_t i = 0; i < text_line.text_runs.size(); ++i) {
    double span_length = 0.0;
    std::vector<GlyphPlacement> span_glyphs;

    auto rc = text_layout_hrun(
        text_line.text_runs[i],
        text_line.text_directions[i],
        font_info,
        font_size,
        dpi,
        &span_glyphs,
        &span_length,
        &line_top,
        &line_bottom);

    if (rc != OK) {
      return rc;
    }

    for (auto& gi : span_glyphs) {
      if (base_text_direction != text_line.text_directions[i]) {
        switch (base_text_direction) {
          case TextDirection::LTR:
            gi.x += span_length;
            break;
          case TextDirection::RTL:
            gi.x -= span_length;
            break;
        }
      }

      switch (base_text_direction) {
        case TextDirection::LTR:
          gi.x += line_length;
          break;
        case TextDirection::RTL:
          gi.x -= line_length;
          break;
      }

      GlyphSpan gs;
      gs.font = gi.font;
      gs.glyphs.emplace_back(gi);

      // TODO merge glyph spans witht the same font
      if (glyph_spans) {
        glyph_spans->emplace_back(gs);
      }
    }

    line_length += span_length;
  }

  double line_left = 0.0;
  switch (base_text_direction) {
    case TextDirection::LTR:
      line_left = 0;
      break;
    case TextDirection::RTL:
      line_left = -line_length;
      break;
  }

  *bbox = Rectangle(
      line_left,
      line_top,
      line_length,
      line_bottom - line_top);

  return OK;
}


Status text_measure_span(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    Rectangle* bbox) {
  return text_layout_hline(
      text,
      TextDirection::LTR,
      font_info,
      font_size,
      dpi,
      nullptr,
      bbox);
}

} // namespace text
} // namespace fviz

