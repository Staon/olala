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
#include "olala/symbolterminal.h"

#include <utility>

#include <olala/error.h>
#include <olala/lookaheadstate.h>
#include <olala/parsercontext.h>
#include <olala/semanticstack.h>
#include <olala/valuestring.h>

namespace OLala {

namespace Detail {

class TerminalLookaheadState : public LookaheadState {
  public:
    InputRange range;

    explicit TerminalLookaheadState(
        InputRange&& range_);

    virtual void apply(
        const ParserContext& context_,
        const Symbol& symbol_) override;
};

TerminalLookaheadState::TerminalLookaheadState(
    InputRange&& range_)
  : range(std::move(range_)) {

}

void TerminalLookaheadState::apply(
    const ParserContext& context_,
    const Symbol& symbol_) {
  dynamic_cast<const SymbolTerminal&>(symbol_).applyRange(
      context_, std::move(range));
}

} /* -- namespace Detail */

SymbolTerminal::SymbolTerminal() = default;
SymbolTerminal::~SymbolTerminal() = default;

void SymbolTerminal::applyRange(
    const ParserContext& context_,
    InputRange&& range_) const {
  auto value_(range_.getString());
  range_.commitRange();
  doCommitValue(context_, std::move(value_));
}

LookaheadStatus SymbolTerminal::doLookahead(
  const ParserContext& context_) const {
  auto range(doMatch(context_));
  if(range.has_value()) {
    return {
      LookaheadResult::ACCEPT,
      std::make_unique<Detail::TerminalLookaheadState>(std::move(*range))};
  }
  else {
    return {LookaheadResult::REJECT, nullptr};
  }
}

void SymbolTerminal::doParse(
    const ParserContext& context_) const {
  auto range(doMatch(context_));
  if(range.has_value()) {
    auto value_(range->getString());
    range->commitRange();
    doCommitValue(context_, std::move(value_));
  }
  else {
    throw Error("syntax error: unexpected input");
  }
}

void SymbolTerminal::doCommitValue(
    const ParserContext& context_,
    std::string&& value_) const {
  context_.stack->pushValue(
      std::make_shared<ValueString>(std::move(value_)));
}

} /* -- namespace OLala */
