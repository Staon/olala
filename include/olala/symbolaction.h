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
#ifndef OLALA_SYMBOLACTION_H
#define OLALA_SYMBOLACTION_H

#include <functional>

#include <olala/symbol.h>

namespace OLala {

class ParserContext;

/**
 * @brief Symbol that always returns epsilon and runs an action during parse
 *
 * The action receives the parser context so it can manipulate the semantic
 * stack or perform other side effects.
 */
class SymbolAction : public Symbol {
  public:
    typedef std::function<void(const ParserContext&)> Action;

    explicit SymbolAction(
        Action action_);
    virtual ~SymbolAction() override;

    /* -- avoid copying */
    SymbolAction(
        const SymbolAction&) = delete;
    SymbolAction& operator=(
        const SymbolAction&) = delete;

  protected:
    virtual LookaheadStatus doLookahead(
        const ParserContext& context_) override;

    virtual void doParse(
        const ParserContext& context_) override;

  private:
    Action action;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLACTION_H */
