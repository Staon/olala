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
#ifndef OLALA_SYMBOLSEQUENCE_H
#define OLALA_SYMBOLSEQUENCE_H

#include <olala/symbolnonterminal.h>

namespace OLala {

namespace Detail {
class SequenceLookaheadState;
} /* -- namespace Detail */

/**
 * @brief Non-terminal symbol matching all children in order
 *
 * The sequence accepts if and only if every child accepts (or is epsilon)
 * in order. If any child rejects, the whole sequence rejects.
 */
class SymbolSequence : public SymbolNonTerminal {
    friend class Detail::SequenceLookaheadState;

  public:
    SymbolSequence();
    virtual ~SymbolSequence() override;

    /* -- avoid copying */
    SymbolSequence(
        const SymbolSequence&) = delete;
    SymbolSequence& operator=(
        const SymbolSequence&) = delete;

  protected:
    virtual LookaheadStatus doLookahead(
        const ParserContext& context_) override;

    virtual void doParse(
        const ParserContext& context_) override;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLSEQUENCE_H */
