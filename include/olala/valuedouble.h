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
#ifndef OLALA_VALUEDOUBLE_H
#define OLALA_VALUEDOUBLE_H

#include <olala/value.h>

namespace OLala {

/**
 * @brief Semantic value holding a floating-point number
 *
 * The numeric value is always stored as @c long @c double regardless of the
 * original literal suffix. The kind records the precision implied by the
 * literal suffix: @c f / @c F → FLOAT, no suffix → DOUBLE,
 * @c l / @c L → LONG_DOUBLE.
 */
class ValueDouble : public Value {
  public:
    /**
     * @brief Precision kind implied by the literal suffix
     */
    enum class Kind {
        FLOAT,        ///< f or F suffix
        DOUBLE,       ///< no suffix
        LONG_DOUBLE,  ///< l or L suffix
    };

    ValueDouble(
        long double value_,
        Kind kind_);
    virtual ~ValueDouble() override;

    /* -- avoid copying */
    ValueDouble(
        const ValueDouble&) = delete;
    ValueDouble& operator=(
        const ValueDouble&) = delete;

    /**
     * @brief Get the numeric value
     */
    long double getValue() const;

    /**
     * @brief Get the precision kind implied by the literal suffix
     */
    Kind getKind() const;

  private:
    long double value;
    Kind kind;
};

} /* -- namespace OLala */

#endif /* OLALA_VALUEDOUBLE_H */
