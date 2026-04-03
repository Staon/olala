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
#ifndef OLALA_SEMANTICSTACK_H
#define OLALA_SEMANTICSTACK_H

#include <vector>

#include <olala/valueptr.h>

namespace OLala {

/**
 * @brief Stack of semantic values produced during parsing
 */
class SemanticStack final {
  public:
    SemanticStack();
    ~SemanticStack();

    /* -- avoid copying */
    SemanticStack(
        const SemanticStack&) = delete;
    SemanticStack& operator=(
        const SemanticStack&) = delete;

    /**
     * @brief Push a value at the top of the stack
     * @param value_ The value
     */
    void pushValue(
        ValuePtr value_);

    /**
     * @brief Pop a value from the stack top
     * @return The value
     */
    ValuePtr popValue();

  private:
    typedef std::vector<ValuePtr> Stack;
    Stack stack;
};

} /* -- namespace OLala */

#endif /* OLALA_SEMANTICSTACK_H */
