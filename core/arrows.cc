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
#include "arrows.h"

using namespace std::placeholders;

namespace fviz {

ReturnCode arrow_draw_default(
    const Point& from,
    const Point& to,
    const Measure& size,
    const Color& color,
    Layer* layer) {
  auto direction = vec2_normalize(vec2_sub(to, from));
  auto ortho = vec2{direction.y, direction.x * -1};

  double head_width_factor = 2;
  double head_length_factor = 4;
  Path head_path;
  head_path.moveTo(vec2_add(to, vec2_mul(ortho, size * head_width_factor)));
  head_path.lineTo(vec2_sub(to, vec2_mul(ortho, size * head_width_factor)));
  head_path.lineTo(vec2_add(to, vec2_mul(direction, size * head_length_factor)));
  head_path.closePath();

  StrokeStyle line_style;
  line_style.color = color;
  line_style.line_width = size;
  strokeLine(layer, from, to, line_style);

  FillStyle head_style;
  head_style.color = color;
  fillPath(layer, head_path, head_style);

  return OK;
}

Arrow arrow_create_default() {
  return bind(&arrow_draw_default, _1, _2, _3, _4, _5);
}

} // namespace fviz

