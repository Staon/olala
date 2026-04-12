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
#include <olala/symbolinteger.h>

#include <string>
#include <utility>

#include <olala/inputsequence.h>
#include <olala/parsercontext.h>
#include <olala/semanticstack.h>
#include <olala/valueinteger.h>

namespace OLala {

namespace {

bool isDecDigit(char32_t c_) {
  return c_ >= U'0' && c_ <= U'9';
}

bool isOctDigit(char32_t c_) {
  return c_ >= U'0' && c_ <= U'7';
}

bool isHexDigit(char32_t c_) {
  return isDecDigit(c_)
      || (c_ >= U'a' && c_ <= U'f')
      || (c_ >= U'A' && c_ <= U'F');
}

bool isBinDigit(char32_t c_) {
  return c_ == U'0' || c_ == U'1';
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

/* -- Consume one or more binary digits. Returns true if any were consumed. */
bool fetchBinDigits(InputRange& range_) {
  if(!isBinDigit(range_.peekCharacter())) {
    return false;
  }
  do {
    range_.fetchCharacter();
  } while(isBinDigit(range_.peekCharacter()));
  return true;
}

/* -- Consume l/ll or L/LL suffix. Returns true if any were consumed.
 * Mixed case (lL, Ll) is intentionally not accepted. */
bool fetchLongSuffix(InputRange& range_) {
  auto c(range_.peekCharacter());
  if(c == U'l') {
    range_.fetchCharacter();
    if(range_.peekCharacter() == U'l') {
      range_.fetchCharacter();  /* -- ll */
    }
    return true;
  }
  else if(c == U'L') {
    range_.fetchCharacter();
    if(range_.peekCharacter() == U'L') {
      range_.fetchCharacter();  /* -- LL */
    }
    return true;
  }
  return false;
}

/* -- Consume optional integer suffix in any valid u/l combination. */
void fetchIntSuffix(InputRange& range_) {
  auto c(range_.peekCharacter());
  if(c == U'u' || c == U'U') {
    range_.fetchCharacter();
    fetchLongSuffix(range_);  /* -- optional long qualifier after u */
  }
  else if(fetchLongSuffix(range_)) {
    /* -- optional unsigned after long qualifier */
    c = range_.peekCharacter();
    if(c == U'u' || c == U'U') {
      range_.fetchCharacter();
    }
  }
}

/* -- Suffix description extracted from the string representation. */
struct SuffixInfo {
  bool is_unsigned;
  int long_count;       /* -- 0 = int, 1 = long, 2 = long long */
  std::size_t digits_end; /* -- position past the digits (start of suffix) */
};

/* -- Parse integer suffix from the right end of @a s.
 * Returns a SuffixInfo with long_count and is_unsigned flags, and the
 * position where the digit string ends. */
SuffixInfo parseSuffix(const std::string& s) {
  SuffixInfo info{false, 0, s.size()};
  std::size_t end(s.size());

  if(end == 0) {
    return info;
  }

  char last(s[end - 1]);

  if(last == 'u' || last == 'U') {
    /* -- unsigned at end; optionally preceded by long qualifier */
    info.is_unsigned = true;
    --end;
    if(end >= 2
        && (s[end - 2] == 'l' && s[end - 1] == 'l'
            || s[end - 2] == 'L' && s[end - 1] == 'L')) {
      info.long_count = 2;
      end -= 2;
    }
    else if(end >= 1 && (s[end - 1] == 'l' || s[end - 1] == 'L')) {
      info.long_count = 1;
      --end;
    }
  }
  else if(last == 'l' || last == 'L') {
    /* -- long qualifier at end; check for ll/LL, then optional u before */
    if(end >= 2 && s[end - 2] == last) {
      info.long_count = 2;
      end -= 2;
    }
    else {
      info.long_count = 1;
      --end;
    }
    if(end >= 1 && (s[end - 1] == 'u' || s[end - 1] == 'U')) {
      info.is_unsigned = true;
      --end;
    }
  }

  info.digits_end = end;
  return info;
}

ValueInteger::Kind toKind(bool is_unsigned_, int long_count_) {
  if(!is_unsigned_ && long_count_ == 0) return ValueInteger::Kind::INT;
  if( is_unsigned_ && long_count_ == 0) return ValueInteger::Kind::UNSIGNED_INT;
  if(!is_unsigned_ && long_count_ == 1) return ValueInteger::Kind::LONG;
  if( is_unsigned_ && long_count_ == 1) return ValueInteger::Kind::UNSIGNED_LONG;
  if(!is_unsigned_ && long_count_ == 2) return ValueInteger::Kind::LONG_LONG;
  /* is_unsigned && long_count == 2 */
  return ValueInteger::Kind::UNSIGNED_LONG_LONG;
}

} /* -- namespace */

SymbolInteger::SymbolInteger() = default;
SymbolInteger::~SymbolInteger() = default;

std::optional<InputRange> SymbolInteger::doMatch(
    const ParserContext& context_) const {
  auto range(context_.sequence->openRange());

  auto c(range.peekCharacter());

  if(c == U'0') {
    range.fetchCharacter();
    auto x(range.peekCharacter());
    if(x == U'x' || x == U'X') {
      /* -- hexadecimal */
      range.fetchCharacter();
      if(!fetchHexDigits(range)) {
        return std::nullopt;
      }
    }
    else if(x == U'b' || x == U'B') {
      /* -- binary (C++14) */
      range.fetchCharacter();
      if(!fetchBinDigits(range)) {
        return std::nullopt;
      }
    }
    else {
      /* -- octal (or the literal zero) */
      while(isOctDigit(range.peekCharacter())) {
        range.fetchCharacter();
      }
    }
  }
  else if(isDecDigit(c)) {
    /* -- decimal (non-zero first digit) */
    fetchDecDigits(range);
  }
  else {
    return std::nullopt;
  }

  fetchIntSuffix(range);
  return std::move(range);
}

void SymbolInteger::doCommitValue(
    const ParserContext& context_,
    std::string&& value_,
    SourceRange&& range_) const {
  auto suffix(parseSuffix(value_));
  auto kind(toKind(suffix.is_unsigned, suffix.long_count));

  /* -- extract digit string and detect base */
  std::string digits(value_.substr(0, suffix.digits_end));

  int base(0);  /* -- auto-detect: 0x→hex, 0→octal, else→decimal */
  if(digits.size() >= 2
      && digits[0] == '0'
      && (digits[1] == 'b' || digits[1] == 'B')) {
    base = 2;
    digits = digits.substr(2);  /* -- strip 0b/0B prefix */
  }

  unsigned long long number(std::stoull(digits, nullptr, base));

  context_.stack->pushValue(
      std::make_shared<ValueInteger>(number, kind),
      std::move(range_));
}

} /* -- namespace OLala */
