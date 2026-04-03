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
#ifndef OLALA_PARSERCONTEXT_H
#define OLALA_PARSERCONTEXT_H

#include <cassert>

namespace OLala {

class SemanticStack;
class InputSequence;

/**
 * @brief Parser context bundling parser state passed through recursive descent
 */
class ParserContext {
  public:
    InputSequence* const sequence;
    SemanticStack* const stack;

  private:
    void* const user_data;

  public:
    explicit ParserContext(
        InputSequence* sequence_,
        SemanticStack* stack_,
        void* user_data_ = nullptr);

    /* -- avoid copying */
    ParserContext(
        const ParserContext&) = delete;
    ParserContext& operator=(
        const ParserContext&) = delete;

    /**
     * @brief Get typed user data
     *
     * @tparam Type Type of the user data
     * @return The user data
     */
    template<typename Type> Type* userData() {
        auto user_data_(dynamic_cast<Type*>(user_data));
        assert(user_data_ != nullptr);
        return user_data_;
    }
};

} /* -- namespace OLala */

#endif /* -- OLALA_PARSERCONTEXT_H */
