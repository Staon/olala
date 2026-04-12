/*
 * Copyright (C) 2026 Ondrej Starek
 *
 * This file is part of OLala.
 *
 * OLala is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * OLala is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OLala.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <olala/symbolwhitespace.h>

#include <olala/inputsequence.h>
#include <olala/parsercontext.h>

namespace OLala {

namespace {

bool isWhitespace(char32_t c_) {
  return c_ == U' ' || c_ == U'\t' || c_ == U'\n'
      || c_ == U'\r' || c_ == U'\v' || c_ == U'\f';
}

} /* -- namespace */

SymbolWhitespace::SymbolWhitespace() = default;
SymbolWhitespace::~SymbolWhitespace() = default;

std::optional<InputRange> SymbolWhitespace::doMatch(
    const ParserContext& context_) const {
  auto range(context_.sequence->openRange());

  /* -- at least one whitespace character is required */
  if(!isWhitespace(range.peekCharacter())) {
    return std::nullopt;
  }

  auto f = 1.35l;
  
  /* -- consume all consecutive whitespace */
  do {
    range.fetchCharacter();
  } while(isWhitespace(range.peekCharacter()));

  return std::move(range);
}

} /* -- namespace OLala */
