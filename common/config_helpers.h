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
#pragma once
#include <unordered_map>
#include <optional>
#include <plist/plist.h>
#include <graphics/measure.h>
#include <graphics/color.h>
#include "utils/return_code.h"
#include "common/data_frame.h"
#include "common/dimension.h"

namespace plotfx {

using ParserFn = std::function<ReturnCode (const plist::Property&)>;

template <typename T>
using ParserAtFn = std::function<ReturnCode (const plist::Property&, T*)>;

using ParserDefinitions = std::unordered_map<std::string, ParserFn>;

inline ReturnCode parseAll(
    const plist::PropertyList& plist,
    const ParserDefinitions& pdefs) {
  for (const auto& prop : plist) {
    const auto& pdef = pdefs.find(prop.name);
    if (pdef != pdefs.end()) {
      if (auto rc = pdef->second(prop); !rc.isSuccess()) {
        return ReturnCode::errorf(
            "EPARSE",
            "error while parsing property '$0': $1",
            prop.name,
            rc.getMessage());

        return rc;
      }
    }
  }

  return ReturnCode::success();
}

template <typename T>
using EnumDefinitions = std::unordered_map<std::string, T>;

template<typename T>
ReturnCode parseEnum(
    const EnumDefinitions<T>& defs,
    const std::string& str,
    T* value) {
  const auto& def = defs.find(str);
  if (def == defs.end()) {
    return ReturnCode::errorf("EPARSE", "invalid value '$0'", str);
  }

  *value = def->second;
  return ReturnCode::success();
}

ReturnCode parse_classlike(
    const plist::Property& prop,
    const std::string& fn,
    std::vector<std::string>* args);

template <typename T>
struct Variable {
  std::optional<T> constant;
  SeriesRef variable;
};

template <typename T>
std::vector<T> resolve(
    const Variable<T>& var,
    const DimensionMapFn<T>& map);

template <typename T>
ParserFn configure_var(
    Variable<T>* var,
    ParserAtFn<T> parser);

ParserFn configure_multiprop(const std::vector<ParserFn>& parsers);

ParserFn configure_key(std::string* key);

ReturnCode configure_measure_rel(
    const plist::Property& prop,
    double dpi,
    double font_size,
    Measure* value);

ReturnCode configure_color(
    const plist::Property& prop,
    Color* value);

ParserFn configure_color_fn(Color* var);
ParserAtFn<Color> configure_color_fn();

ReturnCode configure_float(
    const plist::Property& prop,
    double* value);

ReturnCode configure_string(
    const plist::Property& prop,
    std::string* value);

ReturnCode configure_float_opt(
    const plist::Property& prop,
    std::optional<double>* value);

ReturnCode configure_data_frame(
    const plist::Property& prop,
    DataFrame* data);

ReturnCode configure_series(
    const plist::Property& prop,
    SeriesRef* data);

ParserFn configure_series_fn(SeriesRef* data);

} // namespace plotfx

#include "config_helpers_impl.h"
