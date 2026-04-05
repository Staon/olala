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
#include <olala/inputsequence.h>

#include <cassert>
#include <utility>

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

InputSequence::InputSequence() :
  stream_stack(),
  window_begin(0),
  window_end(0),
  window() {

}

InputSequence::~InputSequence() = default;

void InputSequence::pushStream(std::shared_ptr<InputStream> stream_) {
  assert(stream_ != nullptr);

  /* -- shelve the current window into the new frame */
  stream_stack.push_back({std::move(stream_), std::move(window)});
  window.clear();
  window_end = window_begin;
}

void InputSequence::popStream() {
  assert(!stream_stack.empty());

  /* -- restore the shelved window */
  auto& saved(stream_stack.back().saved_window);
  for(auto c : saved) {
    window.push_back(c);
  }
  window_end += saved.size();
  stream_stack.pop_back();
}

InputRange InputSequence::openRange() {
  return InputRange(this, doOpenRange());
}

std::size_t InputSequence::doOpenRange() {
  return window_begin;
}

char32_t InputSequence::doFetchCharacter(
    std::size_t range_end_) {
  assert(range_end_ >= window_begin);

  /* -- extend the window until the requested offset is covered */
  while(range_end_ >= window_end) {
    if(!extendWindow()) {
      return EOS;
    }
  }

  return window[range_end_ - window_begin];
}

std::string InputSequence::doGetString(
    std::size_t begin_,
    std::size_t end_) const {
  assert(begin_ >= window_begin && end_ <= window_end);

  std::string result_;
  for(std::size_t i_(begin_); i_ < end_; ++i_) {
    auto cp_(window[i_ - window_begin]);
    if(cp_ != EOS) {
      encodeUtf8(cp_, result_);
    }
  }
  return result_;
}

void InputSequence::doCommitRange(
    std::size_t begin_,
    std::size_t end_) {
  assert(begin_ == window_begin);
  assert(end_ >= begin_ && end_ <= window_end);
  window.erase(
      window.begin(),
      window.begin() + static_cast<std::deque<char32_t>::difference_type>(end_ - begin_));
  window_begin = end_;
}

bool InputSequence::extendWindow() {
  while(!stream_stack.empty()) {
    auto cp(stream_stack.back().stream->readCodepoint());
    if(cp != EOS) {
      window.push_back(cp);
      ++window_end;
      return true;
    }

    /* -- stream exhausted: restore the shelved window and pop the frame */
    auto& saved(stream_stack.back().saved_window);
    auto saved_size(saved.size());
    for(auto c : saved) {
      window.push_back(c);
    }
    window_end += saved_size;
    stream_stack.pop_back();

    /* -- if shelved chars were restored, the window was extended */
    if(saved_size > 0) {
      return true;
    }
  }
  return false;
}

void InputSequence::encodeUtf8(
    char32_t cp_,
    std::string& out_) {
  if(cp_ < 0x80) {
    out_ += static_cast<char>(cp_);
  }
  else if(cp_ < 0x800) {
    out_ += static_cast<char>(0xc0 | (cp_ >> 6));
    out_ += static_cast<char>(0x80 | (cp_ & 0x3f));
  }
  else if(cp_ < 0x10000) {
    out_ += static_cast<char>(0xe0 | (cp_ >> 12));
    out_ += static_cast<char>(0x80 | ((cp_ >> 6) & 0x3f));
    out_ += static_cast<char>(0x80 | (cp_ & 0x3f));
  }
  else {
    out_ += static_cast<char>(0xf0 | (cp_ >> 18));
    out_ += static_cast<char>(0x80 | ((cp_ >> 12) & 0x3f));
    out_ += static_cast<char>(0x80 | ((cp_ >> 6) & 0x3f));
    out_ += static_cast<char>(0x80 | (cp_ & 0x3f));
  }
}

} /* -- namespace OLala */
