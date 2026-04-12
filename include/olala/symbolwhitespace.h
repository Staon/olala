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
#ifndef OLALA_SYMBOLWHITESPACE_H
#define OLALA_SYMBOLWHITESPACE_H

#include <olala/symbolterminal.h>

namespace OLala {

/**
 * @brief Terminal symbol matching a non-empty run of whitespace characters
 *
 * Matches one or more characters from the set: space (U+0020), horizontal
 * tab (U+0009), line feed (U+000A), carriage return (U+000D), vertical tab
 * (U+000B), and form feed (U+000C).
 *
 * Typical use: pass as the skipper to InputSequence to consume whitespace
 * between tokens.
 */
class SymbolWhitespace : public SymbolTerminal {
  public:
    SymbolWhitespace();
    virtual ~SymbolWhitespace() override;

    /* -- avoid copying */
    SymbolWhitespace(
        const SymbolWhitespace&) = delete;
    SymbolWhitespace& operator=(
        const SymbolWhitespace&) = delete;

  protected:
    virtual std::optional<InputRange> doMatch(
        const ParserContext& context_) const override;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLWHITESPACE_H */
