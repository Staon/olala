/*
 * Copyright (C) 2021 Ondrej Starek
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
#include "olala/symbolidentifier.h"

#include <olala/inputsequence.h>
#include <olala/lookaheadstate.h>
#include <olala/parsercontext.h>

namespace OLala {

namespace {

bool isIdentStart(char32_t c_) {
  return (c_ >= 'a' && c_ <= 'z')
      || (c_ >= 'A' && c_ <= 'Z')
      || c_ == '_';
}

bool isIdentContinue(char32_t c_) {
  return isIdentStart(c_)
      || (c_ >= '0' && c_ <= '9');
}

} /* -- namespace */

SymbolIdentifier::SymbolIdentifier() = default;
SymbolIdentifier::~SymbolIdentifier() = default;

std::optional<InputRange> SymbolIdentifier::doMatch(
    const ParserContext& context_) const {
  auto range(context_.sequence->openRange());

  /* -- the first character must be a letter or underscore */
  auto c(range.fetchCharacter());
  if(!isIdentStart(c)) {
    return std::nullopt;
  }

  /* -- fetch remaining identifier characters */
  while(true) {
    c = range.fetchCharacter();
    if(!isIdentContinue(c)) {
      break;
    }
  }

  return std::move(range);
}

} /* -- namespace OLala */
