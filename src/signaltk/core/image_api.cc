/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <signaltk/util/flagparser.h>
#include "image_api.h"

namespace signaltk {

int cmd_image_new(Context* ctx, const char** args, int arg_count) {
  FlagParser flag_parser;

  std::string flag_out;
  flag_parser.defineString("out", true, &flag_out);

  uint64_t flag_width;
  flag_parser.defineUInt64("width", true, &flag_width);

  uint64_t flag_height;
  flag_parser.defineUInt64("height", true, &flag_height);

  auto rc = flag_parser.parseArgv(arg_count, args);
  if (!rc.isSuccess()) {
    std::cerr << "ERROR: " << rc.getMessage() << std::endl;
    return -1;
  }

  return 0;
}

} // namespace signaltk

