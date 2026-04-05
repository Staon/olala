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
#include <olala/symbolalternation.h>

#include <utility>

#include <olala/error.h>
#include <olala/lookaheadstate.h>
#include <olala/symbol.h>

namespace OLala {

namespace Detail {

class AlternationLookaheadState : public LookaheadState {
  public:
    std::size_t selected_index;
    LookaheadStatePtr selected_state;

    AlternationLookaheadState(
        std::size_t selected_index_,
        LookaheadStatePtr selected_state_);

    virtual void apply(
        const ParserContext& context_,
        const Symbol& symbol_) override;
};

AlternationLookaheadState::AlternationLookaheadState(
    std::size_t selected_index_,
    LookaheadStatePtr selected_state_)
  : selected_index(selected_index_),
    selected_state(std::move(selected_state_)) {

}

void AlternationLookaheadState::apply(
    const ParserContext& context_,
    const Symbol& symbol_) {
  if(!selected_state) {
    return;  /* -- empty alternation, nothing to apply */
  }
  auto& alt_(static_cast<const SymbolAlternation&>(symbol_));
  alt_.children[selected_index]->parse(
      context_, selected_state);
}

} /* -- namespace Detail */

SymbolAlternation::SymbolAlternation() = default;
SymbolAlternation::~SymbolAlternation() = default;

LookaheadStatus SymbolAlternation::doLookahead(
    const ParserContext& context_) const {
  /* -- empty alternation is epsilon */
  if(children.empty()) {
    return {
        LookaheadResult::EPSILON,
        std::make_unique<Detail::AlternationLookaheadState>(
            0, nullptr)};
  }

  std::size_t first_epsilon_index_(0);
  bool has_epsilon_(false);
  LookaheadStatePtr first_epsilon_state_;

  for(std::size_t i_(0); i_ < children.size(); ++i_) {
    auto status_(children[i_]->lookahead(context_));

    if(status_.result == LookaheadResult::ACCEPT) {
      return {
          LookaheadResult::ACCEPT,
          std::make_unique<Detail::AlternationLookaheadState>(
              i_, std::move(status_.state))};
    }

    if(status_.result == LookaheadResult::EPSILON && !has_epsilon_) {
      has_epsilon_ = true;
      first_epsilon_index_ = i_;
      first_epsilon_state_ = std::move(status_.state);
    }
  }

  if(has_epsilon_) {
    return {
        LookaheadResult::EPSILON,
        std::make_unique<Detail::AlternationLookaheadState>(
            first_epsilon_index_, std::move(first_epsilon_state_))};
  }

  return {LookaheadResult::REJECT, nullptr};
}

void SymbolAlternation::doParse(
    const ParserContext& context_) const {
  if(children.empty()) {
    return;  /* -- empty alternation is epsilon */
  }

  for(std::size_t i_(0); i_ < children.size(); ++i_) {
    auto status_(children[i_]->lookahead(context_));

    if(status_.result == LookaheadResult::ACCEPT) {
      children[i_]->parse(context_, std::move(status_.state));
      return;
    }

    if(status_.result == LookaheadResult::EPSILON) {
      children[i_]->parse(context_, std::move(status_.state));
      return;
    }
  }

  throw Error("syntax error: no alternative matched");
}

} /* -- namespace OLala */
