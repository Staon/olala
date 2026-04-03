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
#ifndef OLALA_SYMBOLIDENTIFIER_H
#define OLALA_SYMBOLIDENTIFIER_H

#include <olala/symbolterminal.h>

#include "symbolptr.h"

namespace OLala {

/**
 * @brief Terminal symbol matching C/C++ identifiers
 *
 * Matches sequences starting with [a-zA-Z_] followed by [a-zA-Z0-9_].
 */
class SymbolIdentifier : public SymbolTerminal {
  public:
    SymbolIdentifier();
    virtual ~SymbolIdentifier() override;

    /* -- avoid copying */
    SymbolIdentifier(
        const SymbolIdentifier&) = delete;
    SymbolIdentifier& operator=(
        const SymbolIdentifier&) = delete;

  protected:
    virtual std::optional<InputRange> doMatch(
        const ParserContext& context_) override;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLIDENTIFIER_H */
