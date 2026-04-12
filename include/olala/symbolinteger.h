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
#ifndef OLALA_SYMBOLINTEGER_H
#define OLALA_SYMBOLINTEGER_H

#include <olala/symbolterminal.h>

namespace OLala {

/**
 * @brief Terminal symbol matching C/C++ integer literals
 *
 * Matches decimal, octal, hexadecimal and binary (C++14) integer literals
 * with optional suffix combinations.
 *
 * Decimal: @code [1-9][0-9]* | 0 @endcode
 * Octal:   @code 0[0-7]+ @endcode
 * Hex:     @code 0x[0-9a-fA-F]+ | 0X... @endcode
 * Binary:  @code 0b[01]+ | 0B... @endcode
 *
 * Suffix:  @code [u|U] [l|L|ll|LL]  |  [l|L|ll|LL] [u|U] @endcode
 *
 * On a successful match a ValueInteger is pushed onto the semantic stack.
 */
class SymbolInteger : public SymbolTerminal {
  public:
    SymbolInteger();
    virtual ~SymbolInteger() override;

    /* -- avoid copying */
    SymbolInteger(
        const SymbolInteger&) = delete;
    SymbolInteger& operator=(
        const SymbolInteger&) = delete;

  protected:
    virtual std::optional<InputRange> doMatch(
        const ParserContext& context_) const override;

    virtual void doCommitValue(
        const ParserContext& context_,
        std::string&& value_,
        SourceRange&& range_) const override;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLINTEGER_H */
