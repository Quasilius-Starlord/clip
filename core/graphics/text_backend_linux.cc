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
#include "text_backend.h"

#include <fribidi/fribidi.h>

namespace fviz::text {

ReturnCode text_analyze_bidi_span(
    const std::string& text,
    TextDirection text_direction,
    TextSpan* text_span) {
  FriBidiParType fb_basedir;
  switch (text_direction) {
    case TextDirection::LTR:
      fb_basedir = FRIBIDI_PAR_LTR;
      break;
    case TextDirection::RTL:
      fb_basedir = FRIBIDI_PAR_RTL;
      break;
  }

  // convert to fribidi string
  std::vector<FriBidiChar> fb_str(text.size(), 0);
  auto fb_str_len = fribidi_charset_to_unicode(
      FRIBIDI_CHAR_SET_UTF8,
      text.data(),
      text.size(),
      fb_str.data());

  // find character directionalities and embedding levels using fribidi
  std::vector<FriBidiCharType> fb_types(fb_str_len, 0);
  std::vector<FriBidiLevel> fb_levels(fb_str_len, 0);
  fribidi_get_bidi_types(fb_str.data(), fb_str_len, fb_types.data());

  auto fribidi_rc = fribidi_get_par_embedding_levels(
        fb_types.data(),
        fb_str_len,
        &fb_basedir,
        fb_levels.data());

  if (!fribidi_rc) {
    return errorf(ERROR, "error while performing bidi layout using fribidi");
  }

  // find the bidi runs in the output
  std::vector<size_t> run_bounds;
  for (size_t i = 0; i < fb_str_len; ++i) {
    if (i > 0 && fb_levels[i] != fb_levels[i - 1]) {
      run_bounds.emplace_back(i);
    }
  }

  for (size_t i = 0; i < run_bounds.size() + 1; ++i) {
    auto run_begin = i == 0 ? 0 : run_bounds[i - 1];
    auto run_end = i == run_bounds.size() ? fb_str_len : run_bounds[i];
    auto run_len = run_end - run_begin;

    std::string run(run_len * 4 + 1, 0);
    run.resize(fribidi_unicode_to_charset(
        FRIBIDI_CHAR_SET_UTF8,
        fb_str.data() + run_begin,
        run_len,
        run.data()));

    auto run_direction =
        int(fb_levels[run_begin]) & 1 ?
            TextDirection::RTL :
            TextDirection::LTR;

    text_span->text_runs.emplace_back(run);
    text_span->text_directions.emplace_back(run_direction);
  }

  return OK;
}

} // namespace fviz::text

