/**
 * This file is part of the "plotfx" project
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
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include "graphics/layer.h"

namespace plotfx {

static const int kLegendLabelPadding = 20; // FIXME make configurable
static const int kLegendLineHeight = 20; // FIXME make configurable
static const int kLegendInsideVertPadding = 10;
static const int kLegendInsideHorizPadding = 15;
static const int kLegendOutsideVertPadding = 10;
static const int kLegendOutsideHorizPadding = 25;
static const int kLegendPointY = 6;
static const int kLegendPointWidth = 8;
static const int kLegendPointSize = 3;

class LegendDefinition {
public:

  enum kVerticalPosition {
    LEGEND_TOP = 0,
    LEGEND_BOTTOM = 1
  };

  enum kHorizontalPosition {
    LEGEND_LEFT = 0,
    LEGEND_RIGHT = 1
  };

  enum kPlacement {
    LEGEND_INSIDE = 0,
    LEGEND_OUTSIDE = 1
  };

  /**
   * Create a new legend definition
   */
  LegendDefinition(
      kVerticalPosition vert_pos,
      kHorizontalPosition horiz_pos,
      kPlacement placement,
      const std::string& title);

  const std::string title() const;
  kVerticalPosition verticalPosition() const;
  kHorizontalPosition horizontalPosition() const;
  kPlacement placement() const;

  void addEntry(
      const std::string& name,
      const std::string& color,
      const std::string& shape = "circle");

  const std::vector<std::tuple<std::string, std::string, std::string>>
      entries() const;

protected:
  kVerticalPosition vert_pos_;
  kHorizontalPosition horiz_pos_;
  kPlacement placement_;
  const std::string title_;
  std::vector<std::tuple<std::string, std::string, std::string>> entries_;
};

void renderOutsideLegends(Layer* target, const Rectangle& clip);

void renderInsideLegends(Layer* target, const Rectangle& clip);

void renderRightLegend(
    Layer* target,
    const Rectangle& clip,
    LegendDefinition* legend,
    double horiz_padding,
    bool bottom,
    bool outside);

void renderLeftLegend(
    Layer* target,
    const Rectangle& clip,
    LegendDefinition* legend,
    double horiz_padding,
    bool bottom,
    bool outside);


} // namespace plotfx

