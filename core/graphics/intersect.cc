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
#include "intersect.h"

namespace fviz {

bool intersect_line_line(
    vec2 p0,
    vec2 v0,
    vec2 p1,
    vec2 v1,
    vec2* p /* = nullptr */) {
  // ensure the direction vectors are normalized
  v0 = vec2_normalize(v0);
  v1 = vec2_normalize(v1);

  // consider lines as parallel if the angle between them is smaller than some
  // threshold
  const double a_epsilon = 0.001;
  double a = acos(vec2_dot(v0, v1));
  double a_norm = fmod(M_PI - a, M_PI);

  if (a_norm < a_epsilon) {
    return false;
  }

  // if the lines are not parallel, but the user is not interested in the
  // intersection point, return
  if (!p) {
    return true;
  }

  // the next step is to compute the point of intersection. we start with the
  // implicit expressions that define the global y coordinates of all points on
  // the input lines in terms of the global x coordinate written in the standard
  // form:
  //
  //     aN * x + bN * y = cN
  //
  // we already have the direction vector (a and b) as an input argument, so
  // we can find a valid c that satisfies the equation simply by evaluating the
  // left part of the equation with the known point on the line that we also
  // have.
  auto a0 = -v0.y;
  auto a1 = -v1.y;
  auto b0 = v0.x;
  auto b1 = v1.x;
  auto c0 = a0 * p0.x + b0 * p0.y;
  auto c1 = a1 * p1.x + b1 * p1.y;

  // the x coordinate of the point of intersection can now be determined
  // by solving the following system of linear equations. the two equations are
  // the standard-form line equations derived above.
  //
  //    a0 * x + b0 * y = c0
  //    a1 * x + b1 * y = c1
  //
  // or written in matrix coefficient form:
  //
  //    | a0  b0 |     | x |     | c0 |
  //    |        |  +  |   |  =  |    |
  //    | a1  b1 |     | y |     | c1 |
  //
  // this system can be solved using cramer's rule (there must be a solution
  // since the lines are already determined not to be parallel). to do so, we
  // first calculate the three "denominator", "x numerator" and "y numerator"
  // determinants of the (substituted) coefficient matrix and then compute the
  // result by dividing the determinants as defined by cramer's rule:

  auto dd = (a0 * b1 - b0 * a1); // standard determinant
  auto dx = (b1 * c0 - b0 * c1); // replace x coefficients (a0/a1) with solution
  auto dy = (a0 * c1 - a1 * c0); // replace y coefficients (b0/b1) with solution

  p->x = dx / dd;
  p->y = dy / dd;

  return true;
}

} // namespace fviz

