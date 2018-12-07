/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "plotfx.h"
#include "graphics/text_layout.h"
#include "graphics/text_shaper.h"

namespace plotfx {
namespace text {

Status text_measure_span(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    Rectangle* rect) {
  double line_length = 0;
  double top = 0.0f;
  double bottom = 0.0f;

  std::vector<GlyphInfo> glyphs;
  auto rc = shaper->shapeText(
      text,
      font_info,
      font_size,
      dpi,
      [&] (const GlyphInfo& gi) {
        line_length += gi.advance_x;
        top = std::min(-gi.metrics_ascender, top);
        bottom = std::max(-gi.metrics_descender, bottom);
        glyphs.emplace_back(gi);
      });

  if (rc != OK) {
    return rc;
  }

  *rect = Rectangle(0, top, line_length, bottom - top);
  return OK;
}

Status layoutTextLTR(
    const std::string& text,
    double x,
    double y,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    std::function<void (const GlyphPlacement&)> glyph_cb) {
  double line_length;
  std::vector<GlyphInfo> glyphs;
  auto rc = shaper->shapeText(
      text,
      font_info,
      font_size,
      dpi,
      [&] (const GlyphInfo& gi) {
        line_length += gi.advance_x;
        glyphs.emplace_back(gi);
      });

  if (rc != OK) {
    return rc;
  }

  auto gx = x;
  auto gy = y;


  for (const auto& gi : glyphs) {
    double baseline_offset = 0;

    GlyphPlacement g;
    glyph_cb(GlyphPlacement {
      .codepoint = gi.codepoint,
      .x = gx,
      .y = gy + baseline_offset
    });

    gx += gi.advance_x;
  }

  return OK;
}

Status layoutText(
    const std::string& text,
    double x,
    double y,
    const FontInfo& font,
    double font_size,
    double dpi,
    TextDirection direction,
    const TextShaper* shaper,
    std::function<void (const GlyphPlacement&)> glyph_cb) {
  switch (direction) {
    case TextDirection::LTR:
      return layoutTextLTR(text, x, y, font, font_size, dpi, shaper, glyph_cb);
    case TextDirection::RTL:
      return ERROR_NOT_IMPLEMENTED;
    default:
      return ERROR_INVALID_ARGUMENT;
  }
}


} // namespace text
} // namespace plotfx

