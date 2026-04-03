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
#ifndef OLALA_SYMBOLPTR_H
#define OLALA_SYMBOLPTR_H

#include <memory>

namespace OLala {

class Symbol;
typedef std::shared_ptr<Symbol> SymbolPtr;

/**
 * @brief Create a symbol
 *
 * @tparam Symbol Type of the symbol
 * @param args_ Constructor parameters
 * @return The created symbol
 */
template<typename Symbol, typename... Args> SymbolPtr makeSymbol(
    Args&& ...args_) {
  return std::make_shared<Symbol>(std::forward<Args>(args_)...);
}

} /* -- namespace OLala */

#endif //OLALA_SYMBOLPTR_H
