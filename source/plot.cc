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
#include "plot.h"
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "source/config_helpers.h"
#include "source/element_factory.h"
#include "source/utils/algo.h"
#include "plot_area.h"
#include "plot_bars.h"
#include "plot_labels.h"
#include "legend.h"

using namespace std::placeholders;
using std::ref;

namespace plotfx {
namespace plot {

ReturnCode draw(
    const PlotConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  // setup layout
  auto bbox = layout_margin_box(
      clip,
      config.margins[0],
      config.margins[1],
      config.margins[2],
      config.margins[3]);

  // render grid
  if (auto rc = grid_draw(config.grid, bbox, layer); !rc) {
    return rc;
  }

  // render legend
  if (auto rc = legend_draw(config.legends, bbox, layer); !rc) {
    return rc;
  }

  return ReturnCode::success();
}

ReturnCode configure_layer(
    const plist::Property& prop,
    const Document& doc,
    const DataContext& data,
    const DomainMap& scales,
    LegendItemMap* legend_items,
    PlotConfig* config) {
  std::string type = "points";
  static const ParserDefinitions pdefs = {
    {"type", bind(&configure_string, _1, &type)},
  };

  if (auto rc = parseAll(*prop.next, pdefs); !rc) {
    return rc;
  }

  const auto& layer_props = *prop.next;
  ElementBuilder layer_builder;
  if (!layer_builder) {
    return ReturnCode::errorf("EARG", "invalid layer type: '$0'", type);
  }

  return OK;
}

ReturnCode configure_layers(
    const plist::PropertyList& plist,
    const Document& doc,
    const DataContext& data,
    const DomainMap& scales,
    LegendItemMap* legend_items,
    PlotConfig* config) {
  static const ParserDefinitions pdefs_layer = {
    {"layer", bind(&configure_layer, _1, doc, data, scales, legend_items, config)}
  };

  return parseAll(plist, pdefs_layer);
}

ReturnCode configure_data_refs(
    const plist::PropertyList& plist,
    DataContext* data) {
  SeriesRef data_x;
  SeriesRef data_y;
  SeriesRef data_group;

  auto rc = parseAll(plist, {
    {"x", configure_series_fn(*data, &data_x)},
    {"y", configure_series_fn(*data, &data_y)},
    {"group", configure_series_fn(*data, &data_group)},
  });

  if (!rc) {
    return rc;
  }

  /* extend data context */
  data->defaults["x"] = data_x;
  data->defaults["y"] = data_y;
  data->defaults["group"] = data_group;
  return OK;
}

ReturnCode configure_style(
    const plist::PropertyList& plist,
    const Document& doc,
    DomainMap* scales,
    PlotConfig* config) {
  // TODO: improved style configuration
  config->axis_top.font = doc.font_sans;
  config->axis_top.label_font_size = doc.font_size;
  config->axis_top.border_color = doc.border_color;
  config->axis_top.text_color = doc.text_color;
  config->axis_right.font = doc.font_sans;
  config->axis_right.label_font_size = doc.font_size;
  config->axis_right.border_color = doc.border_color;
  config->axis_right.text_color = doc.text_color;
  config->axis_bottom.font = doc.font_sans;
  config->axis_bottom.label_font_size = doc.font_size;
  config->axis_bottom.border_color = doc.border_color;
  config->axis_bottom.text_color = doc.text_color;
  config->axis_left.font = doc.font_sans;
  config->axis_left.label_font_size = doc.font_size;
  config->axis_left.border_color = doc.border_color;
  config->axis_left.text_color = doc.text_color;

  config->margins[0] = from_em(1.0, doc.font_size);
  config->margins[1] = from_em(1.0, doc.font_size);
  config->margins[2] = from_em(1.0, doc.font_size);
  config->margins[3] = from_em(1.0, doc.font_size);

  auto domain_x = find_ptr(scales, SCALE_DEFAULT_X);
  auto domain_y = find_ptr(scales, SCALE_DEFAULT_Y);

  static const ParserDefinitions pdefs = {
    {
      "margin",
      configure_multiprop({
          bind(&configure_measure, _1, &config->margins[0]),
          bind(&configure_measure, _1, &config->margins[1]),
          bind(&configure_measure, _1, &config->margins[2]),
          bind(&configure_measure, _1, &config->margins[3])
      })
    },
    {"margin-top", bind(&configure_measure, _1, &config->margins[0])},
    {"margin-right", bind(&configure_measure, _1, &config->margins[1])},
    {"margin-bottom", bind(&configure_measure, _1,&config->margins[2])},
    {"margin-left", bind(&configure_measure, _1, &config->margins[3])},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const DataContext& data_in,
    const Document& doc,
    PlotConfig* config) {
  DataContext data = data_in;
  DomainMap scales;
  LegendItemMap legend_items;

  return try_chain({
    bind(&configure_datasource, ref(plist), &data),
    bind(&configure_data_refs, ref(plist), &data),
    bind(&configure_style, ref(plist), doc, &scales, ref(config)),
    bind(&configure_layers,
        ref(plist),
        doc,
        ref(data),
        ref(scales),
        &legend_items,
        config),
    bind(&grid_configure,
        ref(plist),
        ref(doc),
        ref(scales),
        &config->grid),
    bind(&legend_configure_all,
        doc,
        ref(plist),
        ref(legend_items),
        &config->legends),
  });
}

} // namespace plot
} // namespace plotfx

