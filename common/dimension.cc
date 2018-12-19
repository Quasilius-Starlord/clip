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
#include "dimension.h"
#include <iostream>

namespace plotfx {

const DimensionConfig* dimension_find(
    const DimensionMap& map,
    const std::string& key) {
  const auto& iter = map.find(key);
  if (iter == map.end()) {
    return nullptr;
  } else {
    return &iter->second;
  }
}

void dimension_add(
    DimensionMap* map,
    const std::string& key) {
  if (dimension_find(*map, key)) {
    return;
  }

  DimensionConfig d;
  d.key = key;
  map->emplace(d.key, d);
}

std::vector<Color> series_to_colors(
    SeriesRef series,
    const DomainConfig& domain_config,
    const ColorScheme& palette) {
  if (!series) {
    return {};
  }

  auto domain = domain_config;
  domain_fit(*series, &domain);

  std::vector<Color> colors;
  for (const auto& v : *series) {
    auto value = domain_translate(domain, v) * domain_cardinality(domain);
    colors.emplace_back(palette.get(value));
  }

  return colors;
}

std::vector<Color> groups_to_colors(
    const std::vector<DataGroup>& groups,
    const ColorScheme& palette) {
  auto max_idx =
      std::max_element(
          groups.begin(),
          groups.end(),
          [] (const DataGroup& a, const DataGroup& b) { return a.end < b.end; })
      ->end;

  std::vector<Color> colors(max_idx);
  for (size_t i = 0; i < groups.size(); ++i) {
    std::fill(
        colors.begin() + groups[i].begin,
        colors.begin() + groups[i].end,
        palette.get(i));
  }

  return colors;
}

} // namespace plotfx

