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
#ifndef OLALA_SYMBOLEPSILON_H
#define OLALA_SYMBOLEPSILON_H

#include <olala/symbol.h>

namespace OLala {

/**
 * @brief Symbol that always returns epsilon and does nothing during parse
 */
class SymbolEpsilon : public Symbol {
  public:
    SymbolEpsilon();
    virtual ~SymbolEpsilon() override;

    /* -- avoid copying */
    SymbolEpsilon(
        const SymbolEpsilon&) = delete;
    SymbolEpsilon& operator=(
        const SymbolEpsilon&) = delete;

  protected:
    virtual LookaheadStatus doLookahead(
        const ParserContext& context_) const override;

    virtual void doParse(
        const ParserContext& context_) const override;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLEPSILON_H */
