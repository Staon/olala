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
#include <olala/inputsequencestream.h>

#include <cassert>
#include <iostream>

#include <olala/error.h>

namespace OLala {

InputSequenceStream::InputSequenceStream(
    std::istream* is_) :
  is(is_),
  window_begin(0),
  window_end(0),
  window() {
  assert(is != nullptr);
}

InputSequenceStream::~InputSequenceStream() = default;

std::size_t InputSequenceStream::doOpenRange() {
  return window_begin;
}

char32_t InputSequenceStream::doFetchCharacter(
    std::size_t range_end_) {
  assert(range_end_ >= window_begin && range_end_ <= window_end);

  /* -- character already in the window */
  if(range_end_ < window_end) {
    return window[range_end_ - window_begin];
  }

  /* -- expand the window - read a codepoint from the stream */
  auto cp(readCodepoint());
  if(cp == EOS) {
    return EOS;
  }
  window.push_back(cp);
  ++window_end;
  return cp;
}

std::string InputSequenceStream::doGetString(
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

void InputSequenceStream::doCommitRange(
    std::size_t begin_,
    std::size_t end_) {
  assert(begin_ == window_begin);
  assert(end_ >= begin_ && end_ <= window_end);
  window.erase(
      window.begin(),
      window.begin() + static_cast<std::deque<char32_t>::difference_type>(end_ - begin_));
  window_begin = end_;
}

char32_t InputSequenceStream::readCodepoint() {
  auto byte0(is->get());
  if(byte0 == std::istream::traits_type::eof()) {
    if(is->eof())
      return EOS;
    throw Error("reading IO error");
  }

  auto b0(static_cast<unsigned char>(byte0));

  /* -- single byte (ASCII) */
  if(b0 < 0x80) {
    return static_cast<char32_t>(b0);
  }

  /* -- determine the number of continuation bytes */
  int cont_bytes_;
  char32_t cp_;
  if((b0 & 0xe0) == 0xc0) {
    cont_bytes_ = 1;
    cp_ = b0 & 0x1f;
  }
  else if((b0 & 0xf0) == 0xe0) {
    cont_bytes_ = 2;
    cp_ = b0 & 0x0f;
  }
  else if((b0 & 0xf8) == 0xf0) {
    cont_bytes_ = 3;
    cp_ = b0 & 0x07;
  }
  else {
    throw Error("invalid UTF-8 leading byte");
  }

  /* -- read continuation bytes */
  for(int i_(0); i_ < cont_bytes_; ++i_) {
    auto byte(is->get());
    if(byte == std::istream::traits_type::eof()) {
      throw Error("truncated UTF-8 sequence");
    }
    auto b(static_cast<unsigned char>(byte));
    if((b & 0xc0) != 0x80) {
      throw Error("invalid UTF-8 continuation byte");
    }
    cp_ = (cp_ << 6) | (b & 0x3f);
  }

  return cp_;
}

void InputSequenceStream::encodeUtf8(
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
