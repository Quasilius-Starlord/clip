/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "points.h"
#include <numeric>
#include "plotfx.h"
#include "document.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "source/utils/algo.h"
#include "source/config_helpers.h"

using namespace std::placeholders;

namespace plotfx {
namespace plot {
namespace points {

static const double kDefaultPointSizePT = 3;
static const double kDefaultPointSizeMinPT = 1;
static const double kDefaultPointSizeMaxPT = 24;
static const double kDefaultLabelPaddingEM = 0.4;

ReturnCode draw(
    PlotPointsConfig config,
    const Rectangle& clip,
    Layer* layer) {
  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_relative, clip.x, clip.x + clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_relative, clip.y, clip.y + clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.sizes.begin(),
      &*config.sizes.end());

  /* draw points */
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy = clip.y + config.y[i];

    const auto& color = config.colors.empty()
        ? Color{}
        : config.colors[i % config.colors.size()];

    auto size = config.sizes.empty()
        ? Measure{}
        : config.sizes[i % config.sizes.size()];

    FillStyle style;
    style.color = color;

    // TODO point style
    Path path;
    path.moveTo(sx + size, sy);
    path.arcTo(sx, sy, size, 0, M_PI * 2);
    fillPath(layer, clip, path, style);
  }

  /* draw labels */
  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& label_text = config.labels[i];

    auto size = config.sizes.empty()
        ? 0
        : config.sizes[i % config.sizes.size()].value;

    auto label_padding = size + measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingEM, config.label_font_size));

    Point p(
        clip.x + config.x[i] * clip.w,
        clip.y + (1.0 - config.y[i]) * clip.h - label_padding);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::BOTTOM;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const DataContext& data,
    const Document& doc,
    const Environment& env,
    PlotPointsConfig* config) {
  DomainMap scales;
  SeriesRef data_labels;

  DomainConfig domain_x = env.scale_x;
  DomainConfig domain_y = env.scale_y;

  std::optional<Color> color;
  SeriesRef colors = find_maybe(data.defaults, "colors");
  DomainConfig color_domain;
  ColorScheme color_palette;

  static const ParserDefinitions pdefs = {
    {"xs", bind(&configure_measures, _1, &config->x)},
    {"ys", bind(&configure_measures, _1, &config->y)},
    {"sizes", bind(&configure_measures, _1, &config->sizes)},
    //{"color", configure_color_opt(&color)},
    //{"colors", configure_series_fn(data, &colors)},
    //{"labels", configure_series_fn(data, &data_labels)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  /* check dataset */
  if (config->x.size() != config->y.size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'x' and 'y' properties must be equal");
  }

  //if (data_labels && (data_x->size() != data_labels->size())) {
  //  return ReturnCode::error(
  //      "EARG",
  //      "the length of the 'x', 'y' and 'labels' properties must be equal");
  //}

  ///* fetch domains */
  //auto domain_x = find_ptr(scales, scale_x);
  //if (!domain_x) {
  //  return ReturnCode::errorf("EARG", "scale not found: $0", scale_x);
  //}

  //auto domain_y = find_ptr(scales, scale_y);
  //if (!domain_y) {
  //  return ReturnCode::errorf("EARG", "scale not found: $0", scale_y);
  //}

  /* group data */
  //std::vector<DataGroup> groups;
  //if (data_group) {
  //  if (data_x->size() != data_group->size()) {
  //    return ERROR;
  //  }

  //  groups = plotfx::series_group(*data_group);
  //} else {
  //  DataGroup g;
  //  g.index = std::vector<size_t>(data_x->size());
  //  std::iota(g.index.begin(), g.index.end(), 0);
  //  groups.emplace_back(g);
  //}

  /* return element */
  //config->x = domain_translate(*domain_x, *data_x);
  //config->y = domain_translate(*domain_y, *data_y);

  //config->colors = fallback(
  //    color,
  //    series_to_colors(colors, color_domain, color_palette),
  //    groups_to_colors(data_x->size(), groups, color_palette));

  config->label_font = doc.font_sans;
  config->label_font_size = doc.font_size;
  if (data_labels) {
    config->labels = *data_labels;
  }

  return OK;
}

} // namespace points
} // namespace plot
} // namespace plotfx

