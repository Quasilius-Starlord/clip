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
#include "sexpr.h"
#include "graphics/color.h"
#include "graphics/measure.h"

namespace fviz {

using ExprConv = std::function<ReturnCode (const Expr*)>;

ReturnCode expr_to_string(
    const Expr* prop,
    std::string* value);

ReturnCode expr_to_measure(
    const Expr* expr,
    Measure* value);

ReturnCode expr_to_measure_opt(
    const Expr* expr,
    std::optional<Measure>* value);

ReturnCode expr_to_color(
    const Expr* prop,
    Color* value);

ReturnCode expr_to_color_opt(
    const Expr* expr,
    std::optional<Color>* var);

} // namespace fviz

