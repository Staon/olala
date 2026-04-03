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
#include <olala/symbolaction.h>

#include <utility>

#include <olala/lookaheadstate.h>

namespace OLala {

SymbolAction::SymbolAction(
    Action action_)
  : action(std::move(action_)) {

}

SymbolAction::~SymbolAction() = default;

LookaheadStatus SymbolAction::doLookahead(
    const ParserContext& context_) {
  return {LookaheadResult::EPSILON, nullptr};
}

void SymbolAction::doParse(
    const ParserContext& context_) {
  action(context_);
}

} /* -- namespace OLala */
