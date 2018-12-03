/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <string>
#include <vector>

namespace plist {
struct Property;

using PropertyList = std::vector<Property>;

enum class PropertyKind {
  MAP, TUPLE, LIST, ENUM, VALUE, VALUE_LITERAL
};

struct Property {
  std::string name;
  PropertyKind kind;
  std::unique_ptr<std::vector<Property>> next;
  std::string value;

  const Property& operator[](size_t i) const;
  size_t size() const;

  operator const std::string&() const;

};

} // namespace plist

