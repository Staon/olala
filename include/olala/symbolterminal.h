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
#ifndef OLALA_SYMBOLTERMINAL_H
#define OLALA_SYMBOLTERMINAL_H

#include <optional>
#include <string>

#include <olala/inputsequence.h>
#include <olala/symbol.h>

namespace OLala {

namespace Detail {
class TerminalLookaheadState;
} /* -- namespace Detail */

/**
 * @brief Terminal symbol
 *
 * The terminal symbol represents a lexical element (keyword, string, number etc.)
 * Subclasses implement doMatch() to probe the input and return the matched
 * range.
 */
class SymbolTerminal : public Symbol {
    friend class Detail::TerminalLookaheadState;

  public:
    SymbolTerminal();
    virtual ~SymbolTerminal() override;

    /* -- avoid copying */
    SymbolTerminal(
        const SymbolTerminal&) = delete;
    SymbolTerminal& operator=(
        const SymbolTerminal&) = delete;

  protected:
    virtual LookaheadStatus doLookahead(
        const ParserContext& context_) const override;

    virtual void doParse(
        const ParserContext& context_) const override;

    /**
     * @brief Match the input against this terminal
     *
     * @param context_ The parser context
     * @return The matched input range, or empty if the input doesn't match.
     */
    virtual std::optional<InputRange> doMatch(
        const ParserContext& context_) const = 0;

    /**
     * @brief Handle the matched input during commit
     *
     * Called after a successful match with the matched string value.
     * The default implementation pushes a ValueString onto the semantic
     * stack. Subclasses can override to push different values (e.g.
     * a parsed number).
     *
     * @param context_ The parser context
     * @param value_ The matched string (UTF-8)
     * @param range_ Source range of the matched input
     */
    virtual void doCommitValue(
        const ParserContext& context_,
        std::string&& value_,
        SourceRange&& range_) const;

  private:
    void applyRange(
        const ParserContext& context_,
        InputRange&& range_) const;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLTERMINAL_H */
