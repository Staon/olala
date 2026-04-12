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
#include <olala/symboldouble.h>

#include <string>
#include <utility>

#include <olala/inputsequence.h>
#include <olala/parsercontext.h>
#include <olala/semanticstack.h>
#include <olala/valuedouble.h>

namespace OLala {

namespace {

bool isDecDigit(char32_t c_) {
  return c_ >= U'0' && c_ <= U'9';
}

bool isHexDigit(char32_t c_) {
  return isDecDigit(c_)
      || (c_ >= U'a' && c_ <= U'f')
      || (c_ >= U'A' && c_ <= U'F');
}

/* -- Consume one or more decimal digits. Returns true if any were consumed. */
bool fetchDecDigits(InputRange& range_) {
  if(!isDecDigit(range_.peekCharacter())) {
    return false;
  }
  do {
    range_.fetchCharacter();
  } while(isDecDigit(range_.peekCharacter()));
  return true;
}

/* -- Consume one or more hex digits. Returns true if any were consumed. */
bool fetchHexDigits(InputRange& range_) {
  if(!isHexDigit(range_.peekCharacter())) {
    return false;
  }
  do {
    range_.fetchCharacter();
  } while(isHexDigit(range_.peekCharacter()));
  return true;
}

/* -- Consume decimal exponent: (e|E) [+|-] digit-sequence.
 * Returns true if a complete exponent was consumed. */
bool fetchDecExp(InputRange& range_) {
  auto c(range_.peekCharacter());
  if(c != U'e' && c != U'E') {
    return false;
  }
  range_.fetchCharacter();
  c = range_.peekCharacter();
  if(c == U'+' || c == U'-') {
    range_.fetchCharacter();
  }
  return fetchDecDigits(range_);
}

/* -- Consume binary exponent: (p|P) [+|-] digit-sequence.
 * Returns true if a complete exponent was consumed. */
bool fetchBinExp(InputRange& range_) {
  auto c(range_.peekCharacter());
  if(c != U'p' && c != U'P') {
    return false;
  }
  range_.fetchCharacter();
  c = range_.peekCharacter();
  if(c == U'+' || c == U'-') {
    range_.fetchCharacter();
  }
  return fetchDecDigits(range_);
}

/* -- Consume optional floating-point suffix (f|F|l|L). */
void fetchOptSuffix(InputRange& range_) {
  auto c(range_.peekCharacter());
  if(c == U'f' || c == U'F' || c == U'l' || c == U'L') {
    range_.fetchCharacter();
  }
}

} /* -- namespace */

SymbolDouble::SymbolDouble() = default;
SymbolDouble::~SymbolDouble() = default;

std::optional<InputRange> SymbolDouble::doMatch(
    const ParserContext& context_) const {
  auto range(context_.sequence->openRange());

  auto c(range.peekCharacter());

  /* -- hexadecimal floating-point literal: 0x / 0X */
  if(c == U'0') {
    range.fetchCharacter();
    auto x(range.peekCharacter());
    if(x == U'x' || x == U'X') {
      range.fetchCharacter();

      /* -- optional integer hex digits */
      bool has_int(fetchHexDigits(range));

      /* -- optional dot and fractional hex digits */
      bool has_frac(false);
      if(range.peekCharacter() == U'.') {
        range.fetchCharacter();
        has_frac = fetchHexDigits(range);
      }

      /* -- at least one hex digit is required in total */
      if(!has_int && !has_frac) {
        return std::nullopt;
      }

      /* -- mandatory binary exponent */
      if(!fetchBinExp(range)) {
        return std::nullopt;
      }

      fetchOptSuffix(range);
      return std::move(range);
    }
    /* -- decimal literal starting with '0': consume remaining digits */
    while(isDecDigit(range.peekCharacter())) {
      range.fetchCharacter();
    }
    /* -- fall through to decimal tail */
  }
  else if(isDecDigit(c)) {
    /* -- decimal literal starting with a non-zero digit */
    fetchDecDigits(range);
    /* -- fall through to decimal tail */
  }
  else if(c == U'.') {
    /* -- literal starting with '.': requires at least one digit after */
    range.fetchCharacter();
    if(!fetchDecDigits(range)) {
      return std::nullopt;
    }
    fetchDecExp(range);   /* -- optional exponent */
    fetchOptSuffix(range);
    return std::move(range);
  }
  else {
    return std::nullopt;
  }

  /* -- decimal tail: must have a dot or exponent to be a float literal */
  if(range.peekCharacter() == U'.') {
    range.fetchCharacter();
    fetchDecDigits(range);   /* -- optional fractional digits */
    fetchDecExp(range);       /* -- optional exponent */
    fetchOptSuffix(range);
    return std::move(range);
  }
  else if(fetchDecExp(range)) {
    fetchOptSuffix(range);
    return std::move(range);
  }

  /* -- no dot and no exponent: this is an integer literal, not a float */
  return std::nullopt;
}

void SymbolDouble::doCommitValue(
    const ParserContext& context_,
    std::string&& value_,
    SourceRange&& range_) const {
  /* -- determine kind and strip suffix before conversion */
  ValueDouble::Kind kind(ValueDouble::Kind::DOUBLE);
  std::string to_parse(value_);
  if(!to_parse.empty()) {
    char last(to_parse.back());
    if(last == 'f' || last == 'F') {
      kind = ValueDouble::Kind::FLOAT;
      to_parse.pop_back();
    }
    else if(last == 'l' || last == 'L') {
      kind = ValueDouble::Kind::LONG_DOUBLE;
      to_parse.pop_back();
    }
  }

  long double number(std::stold(to_parse));

  context_.stack->pushValue(
      std::make_shared<ValueDouble>(number, kind),
      std::move(range_));
}

} /* -- namespace OLala */
