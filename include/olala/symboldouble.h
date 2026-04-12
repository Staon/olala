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
#ifndef OLALA_SYMBOLDOUBLE_H
#define OLALA_SYMBOLDOUBLE_H

#include <olala/symbolterminal.h>

namespace OLala {

/**
 * @brief Terminal symbol matching C/C++ floating-point literals
 *
 * Matches all decimal and hexadecimal floating-point literal forms from
 * the C/C++ standard, including the optional @c f / @c F (float) and
 * @c l / @c L (long double) suffixes.
 *
 * Decimal forms:
 * @code
 *   digit-sequence . [digit-sequence] [exponent] [suffix]
 *   [digit-sequence] . digit-sequence [exponent] [suffix]
 *   digit-sequence exponent [suffix]
 * @endcode
 *
 * Hexadecimal forms (C99 / C++17):
 * @code
 *   0x hex-digit-sequence [.] binary-exponent [suffix]
 *   0x [hex-digit-sequence] . hex-digit-sequence binary-exponent [suffix]
 * @endcode
 *
 * On a successful match a ValueDouble is pushed onto the semantic stack.
 */
class SymbolDouble : public SymbolTerminal {
  public:
    SymbolDouble();
    virtual ~SymbolDouble() override;

    /* -- avoid copying */
    SymbolDouble(
        const SymbolDouble&) = delete;
    SymbolDouble& operator=(
        const SymbolDouble&) = delete;

  protected:
    virtual std::optional<InputRange> doMatch(
        const ParserContext& context_) const override;

    virtual void doCommitValue(
        const ParserContext& context_,
        std::string&& value_,
        SourceRange&& range_) const override;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLDOUBLE_H */
