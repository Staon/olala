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
#ifndef OLALA_SYMBOLACTIONABLE_H
#define OLALA_SYMBOLACTIONABLE_H

#include <olala/action.h>
#include <olala/adaptable.h>

namespace OLala {

/**
 * @brief Adapted interface for symbols that support a sidekick action
 *
 * The action is invoked after the symbol is fully parsed (via doAfterAction).
 * This avoids wrapping every non-terminal into a sequence(symbol, action).
 */
class SymbolActionable : public virtual Adapted {
  public:

    SymbolActionable();
    virtual ~SymbolActionable() override;

    /* -- avoid copying */
    SymbolActionable(
        const SymbolActionable&) = delete;
    SymbolActionable& operator=(
        const SymbolActionable&) = delete;

    /**
     * @brief Set the after-parse action
     *
     * @param action_ The action to run after this symbol is parsed
     */
    virtual void setAction(
        Action action_) = 0;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLACTIONABLE_H */
