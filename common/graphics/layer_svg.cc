/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include "layer_svg.h"
#include "utils/fileutil.h"
#include "utils/exception.h"

namespace plotfx {

SVGData::SVGData() {}

Status svg_text_span(const TextSpanOp& op, SVGData* svg) {
  const auto& style = op.style;

  std::string anchor = "middle";
  std::string baseline = "middle";

  svg->buffer
    << "  "
    << "<text "
    << "x='" << op.x << "' "
    << "y='" << op.y << "' "
    << "fill='" << style.colour.to_hex_str() << "' "
    << "font-size='" << style.font_size << "' "
    << "text-anchor='" << anchor << "' "
    << "dominant-baseline='" << baseline << "' "
    << ">"
    << op.text // FIXME escape
    << "</text>";

  return OK;
}

Status svg_stroke_path(const BrushStrokeOp& op, SVGData* svg) {
  const auto& clip = op.clip;
  const auto& path = op.path;
  const auto& style = op.style;

  if (path.size() < 2) {
    return ERROR_INVALID_ARGUMENT;
  }

  svg->buffer << StringUtil::format(
      "  <path stroke-width='$0' stroke='$1' fill='none' d=\"",
      style.line_width.value, // FIXME
      style.colour.to_hex_str());

  for (const auto& cmd : path) {
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        svg->buffer << StringUtil::format("M$0 $1 ", cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        svg->buffer << StringUtil::format("L$0 $1 ", cmd[0], cmd[1]);
        break;
      case PathCommand::ARC_TO:
        break;
    }
  }

  svg->buffer << "\" />\n";

  return OK;
}

std::string SVGData::to_svg() const {
  return StringUtil::format(
      "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox='0 0 $0 $1' viewport-fill='white'>\n$2\n</svg>",
      width,
      height,
      buffer.str());
}

Status SVGData::writeToFile(const std::string& path) {
  auto svg = to_svg();
  FileUtil::write(path, Buffer(svg.data(), svg.size()));
  return OK;
}

ReturnCode layer_new_svg(Layer* layer, SVGData* svg) {
  svg->width = layer->width;
  svg->height = layer->height;

  layer->op_brush_stroke = std::bind(&svg_stroke_path, std::placeholders::_1, svg);
  layer->op_brush_fill = [] (auto op) { return OK; };
  layer->op_text_span = std::bind(&svg_text_span, std::placeholders::_1, svg);

  return OK;
}

} // namespace plotfx

