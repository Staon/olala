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
#ifndef OLALA_LOOKAHEADSTATE_H
#define OLALA_LOOKAHEADSTATE_H

#include <olala/lookaheadstateptr.h>

namespace OLala {

class ParserContext;
class Symbol;

/**
 * @brief Base class for lookahead state
 *
 * Each symbol subclass defines its own state capturing decisions made
 * during lookahead (e.g. matched input range for terminals, selected
 * child rule for non-terminals). The state can be passed to parse()
 * to avoid repeating the decision.
 */
class LookaheadState {
  public:
    LookaheadState();
    virtual ~LookaheadState();

    /* -- avoid copying */
    LookaheadState(
        const LookaheadState&) = delete;
    LookaheadState& operator=(
        const LookaheadState&) = delete;

    /**
     * @brief Apply the cached lookahead decision to a symbol
     *
     * @param context_ The parser context
     * @param symbol_ The symbol to apply the decision to
     */
    virtual void apply(
        const ParserContext& context_,
        Symbol& symbol_) = 0;
};

} /* -- namespace OLala */

#endif //OLALA_LOOKAHEADSTATE_H
