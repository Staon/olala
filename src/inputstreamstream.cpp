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
#include <olala/inputstreamstream.h>

#include <cassert>
#include <iostream>

#include <olala/error.h>

namespace OLala {

InputStreamStream::InputStreamStream(
    std::unique_ptr<std::istream>&& is_) :
  is(std::move(is_)) {
  assert(is != nullptr);
}

InputStreamStream::~InputStreamStream() = default;

char32_t InputStreamStream::readCodepoint() {
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

} /* -- namespace OLala */
