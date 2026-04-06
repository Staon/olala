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
#ifndef OLALA_SYMBOLCONTAINER_H
#define OLALA_SYMBOLCONTAINER_H

#include <olala/symbol.h>
#include <olala/symbolptr.h>

namespace OLala {

/**
 * @brief Interface for symbols that hold child symbols
 *
 * Pure interface declaring methods for adding children. Implementors
 * are free to choose their own storage strategy.
 */
class SymbolContainer : public virtual Adapted {
  public:
    SymbolContainer();
    virtual ~SymbolContainer() override;

    /* -- avoid copying */
    SymbolContainer(
        const SymbolContainer&) = delete;
    SymbolContainer& operator=(
        const SymbolContainer&) = delete;

    /**
     * @brief Append a child symbol at the end
     *
     * @param child_ The child symbol
     */
    virtual SymbolContainer& appendChild(
        SymbolPtr child_) = 0;

    /**
     * @brief Prepend a child symbol at the beginning
     *
     * @param child_ The child symbol
     */
    virtual SymbolContainer& prependChild(
        SymbolPtr child_) = 0;
};

} /* -- namespace OLala */

#endif /* OLALA_SYMBOLCONTAINER_H */
