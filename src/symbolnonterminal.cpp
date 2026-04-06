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
#include <olala/symbolnonterminal.h>

#include <utility>

namespace OLala {

SymbolNonTerminal::SymbolNonTerminal() = default;

SymbolNonTerminal::~SymbolNonTerminal() = default;

SymbolNonTerminal& SymbolNonTerminal::appendChild(
    SymbolPtr child_) {
  children.push_back(std::move(child_));
  return *this;
}

SymbolNonTerminal& SymbolNonTerminal::prependChild(
    SymbolPtr child_) {
  children.insert(children.begin(), std::move(child_));
  return *this;
}

void SymbolNonTerminal::setAction(
    Action action_) {
  action = std::move(action_);
}

const Adapted *SymbolNonTerminal::doAdapt(
    const std::type_info &info_) const {
  if(info_ == typeid(SymbolContainer))
    return this;
  if(info_ == typeid(SymbolActionable))
    return this;
  return nullptr;
}

void SymbolNonTerminal::doAfterAction(
    const ParserContext& context_) const {
  if(action) {
    action(context_);
  }
}

} /* -- namespace OLala */
