/**
 * This file is part of the "clip" project
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
#include "element_factory.h"
#include "environment.h"

#include <unordered_map>

namespace clip {

ReturnCode element_build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  if (!expr || !expr_is_list(expr)) {
    return error(ERROR, "expected an element list");
  }

  auto args = expr_get_list(expr);
  if (!args || !expr_is_value(args)) {
    return error(ERROR, "expected an element name");
  }

  auto elem_name = expr_get_value(args);
  auto elem_iter = env.element_map.elements.find(elem_name);
  if (elem_iter == env.element_map.elements.end()) {
    return errorf(ERROR, "no such element: {}", elem_name);
  }

  auto rc = elem_iter->second(env, args, elem);
  if (!rc) {
    rc.trace.push_back(expr);
  }

  return rc;
}

ReturnCode element_build_all(
    const Environment& env,
    const Expr* expr,
    std::vector<ElementRef>* elems) {
  for (; expr; expr = expr_next(expr)) {
    if (!expr || !expr_is_list(expr)) {
      return error(ERROR, "expected an element list");
    }

    auto args = expr_get_list(expr);
    if (!args || !expr_is_value(args)) {
      return error(ERROR, "expected an element name");
    }

    auto arg0 = expr_get_value(args);
    if (arg0 == "set" || arg0 == "define") {
      continue;
    }

    ElementRef elem;
    if (auto rc = element_build(env, expr, &elem); !rc) {
      return rc;
    }

    elems->emplace_back(std::move(elem));
  }

  return OK;
}

ReturnCode element_build_list(
    const Environment& env,
    const Expr* expr,
    std::vector<ElementRef>* elems) {
  if (!expr_is_list(expr)) {
    return error(ERROR, "expected an element list");
  }

  return element_build_all(env, expr_get_list(expr), elems);
}

struct MacroElement {
  ExprStorage expr;
  ElementRef elem;
};

ReturnCode element_build_macro(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  auto rc = element_build(env, expr, elem);
  rc.trace.clear();
  return rc;
}

void element_bind(
    ElementMap* factory,
    const std::string& name,
    ElementConfigureFn configure_fn) {
  factory->elements[name] = configure_fn;
}

} // namespace clip
