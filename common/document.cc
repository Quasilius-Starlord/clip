/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "document.h"
#include "plist/plist_parser.h"
#include "element_factory.h"
#include "graphics/layer.h"
#include "graphics/layout.h"
#include "graphics/font_lookup.h"
#include "common/config_helpers.h"

namespace plotfx {

Document::Document() :
    width({Unit::PX, 1200}),
    height({Unit::PX, 600}),
    background_colour(Colour::fromRGB(1,1,1)),
    text_colour(Colour::fromRGB(.3,.3,.3)),
    border_colour(Colour::fromRGB(.45,.45,.45)) {}

ReturnCode setupDocumentDefaults(Document* doc) {
  if (auto path = getenv("PLOTFX_FONT_PATH"); path) {
    auto path_parts = StringUtil::split(path, ":");
    doc->font_searchpath.insert(
        doc->font_searchpath.end(),
        path_parts.begin(),
        path_parts.end());
  }


  static const auto default_font_sans = "Roboto-Medium";
  if (!findFontSimple(default_font_sans, doc->font_searchpath, &doc->font_sans)) {
    return ReturnCode::errorf(
        "EARG",
        "unable to find default sans-sans font ($0); searched in: $1",
        default_font_sans,
        StringUtil::join(doc->font_searchpath, ", "));
  }

  return OK;
}

ReturnCode buildDocument(
    const PropertyList& plist,
    Document* doc) {
  static const ParserDefinitions pdefs = {
    {"width", std::bind(&parseMeasureProp, std::placeholders::_1, &doc->width)},
    {"height", std::bind(&parseMeasureProp, std::placeholders::_1, &doc->height)},
    {"background-colour", std::bind(&configure_colour, std::placeholders::_1, &doc->background_colour)},
    {
      "foreground-colour",
      configure_multiprop({
          std::bind(&configure_colour, std::placeholders::_1, &doc->text_colour),
          std::bind(&configure_colour, std::placeholders::_1, &doc->border_colour),
      })
    },
    {"text-colour", std::bind(&configure_colour, std::placeholders::_1, &doc->text_colour)},
    {"border-colour", std::bind(&configure_colour, std::placeholders::_1, &doc->border_colour)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  for (size_t i = 0; i < plist.size(); ++i) {
    const auto& elem_name = plist[i].name;

    if (!plist::is_map(plist[i])) {
      continue;
    }

    const auto& elem_config = plist[i].next.get();

    std::unique_ptr<Element> elem;
    if (auto rc = buildElement(*doc, elem_name, *elem_config, &elem); !rc.isSuccess()) {
      return rc;
    }

    doc->roots.emplace_back(std::move(elem));
  }

  return ReturnCode::success();
}

ReturnCode buildDocument(
    const std::string& spec,
    Document* tree) {
  PropertyList plist;
  plist::PropertyListParser plist_parser(spec.data(), spec.size());
  if (!plist_parser.parse(&plist)) {
    return ReturnCode::errorf(
        "EPARSE",
        "invalid element specification: $0",
        plist_parser.get_error());
  }

  return buildDocument(plist, tree);
}

ReturnCode renderElements(
    const Document& tree,
    Layer* frame) {
  Rectangle clip(0, 0, frame->width, frame->height);

  for (const auto& e : tree.roots) {
    if (auto rc = e->draw(clip, frame); !rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}


} // namespace plotfx

