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
#include "olala/symbol.h"

#include <olala/lookaheadstate.h>

namespace OLala {

Symbol::Symbol() = default;
Symbol::~Symbol() = default;

LookaheadStatus Symbol::lookahead(
    const ParserContext& context_) {
  return doLookahead(context_);
}

void Symbol::parse(
    const ParserContext& context_,
    const LookaheadStatePtr& state_) {
  /* -- If we have the lookahead state, use it. If not
   *    do the entire parse action. */
  if(state_ != nullptr) {
    state_->apply(context_, *this);
  }
  else {
    doParse(context_);
  }
  /* -- an action after consumption of this symbol */
  doAfterAction(context_);
}

void Symbol::doAfterAction(
    const ParserContext& context_) {
  /* -- empty by default, child can override it */
}

const Adapted* Symbol::doAdapt(
    const std::type_info &info_) const {
  return nullptr;
}

} /* -- namespace OLala */
