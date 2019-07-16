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

namespace fviz {

template <typename T=void>
ExprStorage expr_build_next() {
  return ExprStorage(nullptr);
}

template <typename... T>
ExprStorage expr_build_next(ExprStorage head, T&&... tail) {
  auto e = std::move(head);
  auto n = expr_build_next(std::forward<T>(tail)...);
  expr_set_next(e.get(), std::move(n));
  return e;
}

template <typename... T>
ExprStorage expr_build_next(std::string head, T&&... tail) {
  auto e = expr_create_value(head);
  auto n = expr_build_next(std::forward<T>(tail)...);
  expr_set_next(e.get(), std::move(n));
  return e;
}

template <typename... T>
ExprStorage expr_build(T&&... items) {
  return expr_create_list(expr_build_next(std::forward<T>(items)...));
}

} // namespace fviz

