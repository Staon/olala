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
#ifndef OLALA_SYMBOLALTERNATION_H
#define OLALA_SYMBOLALTERNATION_H

#include <olala/symbolnonterminal.h>

namespace OLala {

namespace Detail {
class AlternationLookaheadState;
} /* -- namespace Detail */

/**
 * @brief Non-terminal symbol selecting the first accepting child
 *
 * Children are probed in order. The first child that accepts is selected.
 * Rejected children are skipped. If all children return epsilon, the
 * alternation returns epsilon (the first epsilon child is selected).
 */
class SymbolAlternation : public SymbolNonTerminal {
    friend class Detail::AlternationLookaheadState;

  public:
    SymbolAlternation();
    virtual ~SymbolAlternation() override;

    /* -- avoid copying */
    SymbolAlternation(
        const SymbolAlternation&) = delete;
    SymbolAlternation& operator=(
        const SymbolAlternation&) = delete;

  protected:
    virtual LookaheadStatus doLookahead(
        const ParserContext& context_) const override;

    virtual void doParse(
        const ParserContext& context_) const override;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLALTERNATION_H */
