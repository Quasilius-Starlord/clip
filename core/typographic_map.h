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
#pragma once
#include "graphics/measure.h"
#include "graphics/color.h"
#include "return_code.h"

namespace fviz {

using MeasureMap = std::function<ReturnCode (const std::string& v, Measure* m)>;

MeasureMap measure_map_linear(const Measure& min, const Measure& max);

} // namespace fviz
