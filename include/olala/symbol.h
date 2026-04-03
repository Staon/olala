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
#ifndef OLALA_SYMBOL_H
#define OLALA_SYMBOL_H

#include <olala/adaptable.h>
#include <olala/lookaheadstateptr.h>

namespace OLala {

class ParserContext;

enum class LookaheadResult {
    REJECT,
    ACCEPT,
    EPSILON,
};

struct LookaheadStatus {
    LookaheadResult result;
    LookaheadStatePtr state;
};

/**
 * @brief Generic interface of a grammar symbol (NVI pattern)
 *
 * This is the core abstraction of the parser. Each symbol provides
 * two operations:
 *   - lookahead: probe the input to determine if this symbol matches,
 *     returning the result and a state object capturing the decision
 *   - parse: recursively descend into the symbol, consuming input.
 *     If a lookahead state is provided, it is applied to the symbol.
 *     Otherwise, the symbol performs parsing from scratch.
 */
class Symbol : public Adaptable {
  public:
    Symbol();
    virtual ~Symbol();

    /* -- avoid copying */
    Symbol(
        const Symbol&) = delete;
    Symbol& operator=(
        const Symbol&) = delete;

    /**
     * @brief Probe the input to determine if this symbol matches
     *
     * @param context_ The parser context
     * @return The lookahead result (REJECT, ACCEPT or EPSILON) together
     *         with symbol-specific state capturing decisions made
     *         during the probe.
     */
    LookaheadStatus lookahead(
        const ParserContext& context_);

    /**
     * @brief Recursively descend into this symbol, consuming input
     *
     * If @a state_ is not null, the cached decision is applied to this
     * symbol. Otherwise, doParse is invoked to perform parsing from scratch.
     *
     * @param context_ The parser context
     * @param state_ Optional state from a prior lookahead call.
     */
    void parse(
        const ParserContext& context_,
        const LookaheadStatePtr& state_);

  protected:
    /**
     * @sa Symbol::lookahead()
     */
    virtual LookaheadStatus doLookahead(
        const ParserContext& context_) = 0;

    /**
     * @brief Implementation of the input consumption without stored
     *     lookahead state.
     * @sa Symbol::parse()
     */
    virtual void doParse(
        const ParserContext& context_) = 0;

    /**
     * @brief An action after the consumption of the symbol
     *
     * The default implementation is empty.
     *
     * @param context_ The parser context
     */
    virtual void doAfterAction(
        const ParserContext& context_);

    /* -- adaptable interface */
    virtual const Adapted* doAdapt(
        const std::type_info &info_) const override;
};

} /* -- namespace OLala */

#endif //OLALA_SYMBOL_H
