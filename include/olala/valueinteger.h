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
#ifndef OLALA_VALUEINTEGER_H
#define OLALA_VALUEINTEGER_H

#include <olala/value.h>

namespace OLala {

/**
 * @brief Semantic value holding an integer literal
 *
 * The numeric value is always stored as @c unsigned @c long @c long
 * regardless of the original literal suffix. The kind records the C/C++
 * type implied by the suffix combination.
 */
class ValueInteger : public Value {
  public:
    /**
     * @brief C/C++ type implied by the literal suffix
     */
    enum class Kind {
        INT,                 ///< no suffix
        UNSIGNED_INT,        ///< u or U
        LONG,                ///< l or L
        UNSIGNED_LONG,       ///< ul, lu, uL, Lu, ...
        LONG_LONG,           ///< ll or LL
        UNSIGNED_LONG_LONG,  ///< ull, llu, uLL, LLu, ...
    };

    ValueInteger(
        unsigned long long value_,
        Kind kind_);
    virtual ~ValueInteger() override;

    /* -- avoid copying */
    ValueInteger(
        const ValueInteger&) = delete;
    ValueInteger& operator=(
        const ValueInteger&) = delete;

    /**
     * @brief Get the numeric value
     */
    unsigned long long getValue() const;

    /**
     * @brief Get the C/C++ type implied by the literal suffix
     */
    Kind getKind() const;

  private:
    unsigned long long value;
    Kind kind;
};

} /* -- namespace OLala */

#endif /* OLALA_VALUEINTEGER_H */
