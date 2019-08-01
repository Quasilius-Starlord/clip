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
#include <iostream>
#include "path.h"

namespace fviz {

double PathData::operator[](size_t idx) const {
  return coefficients[idx];
}

Path::Path() {}

Path::Path(const PathData* data, size_t size) {
  data_.reserve(size);

  for (size_t i = 0; i < size; ++i) {
    data_.emplace_back(data[i]);
  }
}

void Path::moveTo(double x, double y) {
  PathData d;
  d.command = PathCommand::MOVE_TO;
  d.coefficients[0] = x;
  d.coefficients[1] = y;
  data_.emplace_back(std::move(d));
}

void Path::moveTo(const Point& p) {
  PathData d;
  d.command = PathCommand::MOVE_TO;
  d.coefficients[0] = p.x;
  d.coefficients[1] = p.y;
  data_.emplace_back(std::move(d));
}

void Path::lineTo(double x, double y) {
  PathData d;
  d.command = PathCommand::LINE_TO;
  d.coefficients[0] = x;
  d.coefficients[1] = y;
  data_.emplace_back(std::move(d));
}

void Path::lineTo(const Point& p) {
  PathData d;
  d.command = PathCommand::LINE_TO;
  d.coefficients[0] = p.x;
  d.coefficients[1] = p.y;
  data_.emplace_back(std::move(d));
}

void Path::quadraticCurveTo(double cx, double cy, double x, double y) {
  PathData d;
  d.command = PathCommand::QUADRATIC_CURVE_TO;
  d.coefficients[0] = cx;
  d.coefficients[1] = cy;
  d.coefficients[2] = x;
  d.coefficients[3] = y;
  data_.emplace_back(std::move(d));
}

void Path::cubicCurveTo(double c1x, double c1y, double c2x, double c2y, double x, double y) {
  PathData d;
  d.command = PathCommand::CUBIC_CURVE_TO;
  d.coefficients[0] = c1x;
  d.coefficients[1] = c1y;
  d.coefficients[2] = c2x;
  d.coefficients[3] = c2y;
  d.coefficients[4] = x;
  d.coefficients[5] = y;
  data_.emplace_back(std::move(d));
}

void Path::arcTo(double cx, double cy, double r, double a1, double a2) {
  PathData d;
  d.command = PathCommand::ARC_TO;
  d.coefficients[0] = cx;
  d.coefficients[1] = cy;
  d.coefficients[2] = r;
  d.coefficients[3] = a1;
  d.coefficients[4] = a2;
  data_.emplace_back(std::move(d));
}

void Path::closePath() {
  PathData d;
  d.command = PathCommand::CLOSE;
  data_.emplace_back(std::move(d));
}

const PathData& Path::operator[](size_t idx) const {
  return data_[idx];
}

PathData& Path::operator[](size_t idx) {
  return data_[idx];
}

const PathData* Path::data() const {
  return &*data_.data();
}

PathData* Path::data() {
  return &*data_.data();
}

const PathData* Path::begin() const {
  return &*data_.begin();
}

PathData* Path::begin() {
  return &*data_.begin();
}

const PathData* Path::end() const {
  return &*data_.end();
}

PathData* Path::end() {
  return &*data_.end();
}

size_t Path::size() const {
  return data_.size();
}

bool Path::empty() const {
  return data_.size() == 0;
}

} // namespace fviz
