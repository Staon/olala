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
#ifndef OLALA_SYMBOLNONTERMINAL_H
#define OLALA_SYMBOLNONTERMINAL_H

#include <vector>

#include <olala/symbolcontainer.h>
#include <olala/symbolptr.h>

namespace OLala {

/**
 * @brief Abstract non-terminal symbol with vector-based child storage
 *
 * Implements the SymbolContainer interface using a vector. Concrete
 * non-terminal symbols (sequence, alternation, etc.) inherit from this
 * class and implement doLookahead/doParse.
 */
class SymbolNonTerminal : public Symbol, public SymbolContainer {
  public:
    SymbolNonTerminal();
    virtual ~SymbolNonTerminal() override;

    /* -- avoid copying */
    SymbolNonTerminal(
        const SymbolNonTerminal&) = delete;
    SymbolNonTerminal& operator=(
        const SymbolNonTerminal&) = delete;

    /* -- SymbolContainer interface */
    virtual void appendChild(
        SymbolPtr child_) override;
    virtual void prependChild(
        SymbolPtr child_) override;

  protected:
    virtual const Adapted* doAdapt(
        const std::type_info &info_) const override;

    typedef std::vector<SymbolPtr> Children;
    Children children;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLNONTERMINAL_H */
