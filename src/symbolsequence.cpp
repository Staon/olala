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
#include <olala/symbolsequence.h>

#include <utility>
#include <vector>

#include <olala/lookaheadstate.h>
#include <olala/symbol.h>

namespace OLala {

namespace Detail {

class SequenceLookaheadState : public LookaheadState {
  public:
    /* -- lookahead states for epsilon children and the first accepted child */
    std::vector<LookaheadStatus> probed_states;
    /* -- index of the first non-probed child (rest parsed without state) */
    std::size_t rest_begin;

    SequenceLookaheadState(
        std::vector<LookaheadStatus>&& probed_states_,
        std::size_t rest_begin_);

    virtual void apply(
        const ParserContext& context_,
        const Symbol& symbol_) override;
};

SequenceLookaheadState::SequenceLookaheadState(
    std::vector<LookaheadStatus>&& probed_states_,
    std::size_t rest_begin_)
  : probed_states(std::move(probed_states_)),
    rest_begin(rest_begin_) {

}

void SequenceLookaheadState::apply(
    const ParserContext& context_,
    const Symbol& symbol_) {
  auto& seq_(static_cast<const SymbolSequence&>(symbol_));

  /* -- apply cached states (epsilon children + the first accepted child) */
  for(std::size_t i_(0); i_ < probed_states.size(); ++i_) {
    seq_.children[i_]->parse(
        context_, probed_states[i_].state);
  }

  /* -- parse the rest without cached state */
  for(std::size_t i_(rest_begin); i_ < seq_.children.size(); ++i_) {
    seq_.children[i_]->parse(context_, nullptr);
  }
}

} /* -- namespace Detail */

SymbolSequence::SymbolSequence() = default;
SymbolSequence::~SymbolSequence() = default;

LookaheadStatus SymbolSequence::doLookahead(
    const ParserContext& context_) const {
  std::vector<LookaheadStatus> probed_states_;

  for(std::size_t i_(0); i_ < children.size(); ++i_) {
    auto status_(children[i_]->lookahead(context_));

    if(status_.result == LookaheadResult::REJECT) {
      return {LookaheadResult::REJECT, nullptr};
    }

    if(status_.result == LookaheadResult::ACCEPT) {
      /* -- first non-epsilon child accepted: the sequence accepts */
      probed_states_.push_back(std::move(status_));
      return {
          LookaheadResult::ACCEPT,
          std::make_unique<Detail::SequenceLookaheadState>(
              std::move(probed_states_), i_ + 1)};
    }

    /* -- epsilon: store the state and continue to the next child */
    probed_states_.push_back(std::move(status_));
  }

  /* -- all children returned epsilon */
  return {
      LookaheadResult::EPSILON,
      std::make_unique<Detail::SequenceLookaheadState>(
          std::move(probed_states_), children.size())};
}

void SymbolSequence::doParse(
    const ParserContext& context_) const {
  for(auto& child_ : children) {
    child_->parse(context_, nullptr);
  }
}

} /* -- namespace OLala */
