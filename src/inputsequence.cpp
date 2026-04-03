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
#include "olala/inputsequence.h"

#include <cassert>

namespace OLala {

InputRange::InputRange() :
  sequence(nullptr),
  begin(0),
  end(0) {

}

InputRange::InputRange(
    InputSequence* sequence_,
    std::size_t begin_) :
  sequence(sequence_),
  begin(begin_),
  end(begin_) {
  assert(sequence != nullptr);
}

InputRange::InputRange(const InputRange& other) = default;
InputRange::InputRange(InputRange&& other) noexcept = default;
InputRange::~InputRange() = default;
InputRange& InputRange::operator=(const InputRange& other) = default;
InputRange& InputRange::operator=(InputRange&& other) noexcept = default;

char32_t InputRange::fetchCharacter() {
  assert(sequence != nullptr);

  auto character(sequence->doFetchCharacter(end));
  ++end;
  return character;
}

std::string InputRange::getString() const {
  assert(sequence != nullptr);
  return sequence->doGetString(begin, end);
}

void InputRange::commitRange() {
  assert(sequence != nullptr);
  sequence->doCommitRange(begin, end);
}

InputSequence::InputSequence() = default;
InputSequence::~InputSequence() = default;

InputRange InputSequence::openRange() {
  return InputRange(this, doOpenRange());
}

} /* -- namespace OLala */
