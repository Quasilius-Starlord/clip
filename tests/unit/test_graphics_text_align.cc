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
#include "graphics/brush.h"
#include "graphics/color.h"
#include "graphics/layer.h"
#include "graphics/layer_svg.h"
#include "graphics/layer_pixmap.h"
#include "graphics/text.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace fviz;

void draw_test(
    Layer* l,
    const FontInfo& font,
    uint32_t x,
    uint32_t y,
    TextDirection dir,
    HAlign ax,
    VAlign ay) {
  TextStyle ts;
  ts.font = font;
  ts.font_size = from_unit(32);
  ts.color = Color::fromRGB(0,0,0);
  ts.direction = dir;

  StrokeStyle ss;
  ss.line_width = from_unit(1);

  strokeLine(l, Point(x - 10, y), Point(x + 10, y), ss);
  strokeLine(l, Point(x, y - 10), Point(x, y + 10), ss);
  drawTextLabel("Hello תל אביב !", Point(x, y), ax, ay, 0, ts, l);
}

void draw_test(Layer* layer, const FontInfo& font) {
  draw_test(layer, font, 100 * 2,  100 * 2, TextDirection::LTR, HAlign::LEFT, VAlign::TOP);
  draw_test(layer, font, 400 * 2,  100 * 2, TextDirection::LTR, HAlign::CENTER, VAlign::TOP);
  draw_test(layer, font, 700 * 2,  100 * 2, TextDirection::LTR, HAlign::RIGHT, VAlign::TOP);

  draw_test(layer, font, 100 * 2,  200 * 2, TextDirection::LTR, HAlign::LEFT, VAlign::CENTER);
  draw_test(layer, font, 400 * 2,  200 * 2, TextDirection::LTR, HAlign::CENTER, VAlign::CENTER);
  draw_test(layer, font, 700 * 2,  200 * 2, TextDirection::LTR, HAlign::RIGHT, VAlign::CENTER);

  draw_test(layer, font, 100 * 2,  300 * 2, TextDirection::LTR, HAlign::LEFT, VAlign::BOTTOM);
  draw_test(layer, font, 400 * 2,  300 * 2, TextDirection::LTR, HAlign::CENTER, VAlign::BOTTOM);
  draw_test(layer, font, 700 * 2,  300 * 2, TextDirection::LTR, HAlign::RIGHT, VAlign::BOTTOM);

  draw_test(layer, font, 100 * 2,  400 * 2, TextDirection::RTL, HAlign::LEFT, VAlign::TOP);
  draw_test(layer, font, 400 * 2,  400 * 2, TextDirection::RTL, HAlign::CENTER, VAlign::TOP);
  draw_test(layer, font, 700 * 2,  400 * 2, TextDirection::RTL, HAlign::RIGHT, VAlign::TOP);

  draw_test(layer, font, 100 * 2,  500 * 2, TextDirection::RTL, HAlign::LEFT, VAlign::CENTER);
  draw_test(layer, font, 400 * 2,  500 * 2, TextDirection::RTL, HAlign::CENTER, VAlign::CENTER);
  draw_test(layer, font, 700 * 2,  500 * 2, TextDirection::RTL, HAlign::RIGHT, VAlign::CENTER);

  draw_test(layer, font, 100 * 2,  600 * 2, TextDirection::RTL, HAlign::LEFT, VAlign::BOTTOM);
  draw_test(layer, font, 400 * 2,  600 * 2, TextDirection::RTL, HAlign::CENTER, VAlign::BOTTOM);
  draw_test(layer, font, 700 * 2,  600 * 2, TextDirection::RTL, HAlign::RIGHT, VAlign::BOTTOM);

}

int main(int argc, char** argv) {
  FontRef font_ref1;
  EXPECT_OK(font_load("/usr/share/fonts/msttcore/arial.ttf", &font_ref1));

  FontRef font_ref2;
  EXPECT_OK(font_load("/usr/share/fonts/msttcore/comic.ttf", &font_ref2));

  FontInfo font;
  font.fonts = {font_ref2, font_ref1};

  {
    LayerRef layer;
    auto rc = layer_bind_svg(
        1600,
        1400,
        96,
        from_unit(12),
        Color::fromRGB(1.0, 1.0, 1.0),
        [&] (auto svg) {
          FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
          return OK;
        },
        &layer);

    draw_test(layer.get(), font);
    layer_submit(layer.get());
  }

  {
    LayerRef layer;
    auto rc = layer_bind_png(
        1600,
        1400,
        96,
        from_unit(12),
        Color::fromRGB(1.0, 1.0, 1.0),
        [&] (auto svg) {
          FileUtil::write(std::string(argv[0]) + ".png", Buffer(svg.data(), svg.size()));
          return OK;
        },
        &layer);

    draw_test(layer.get(), font);
    layer_submit(layer.get());
  }
}

