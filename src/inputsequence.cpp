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
  end(0),
  location() {

}

InputRange::InputRange(
    InputSequence* sequence_,
    std::size_t begin_,
    SourceLocation location_) :
  sequence(sequence_),
  begin(begin_),
  end(begin_),
  location(std::move(location_)) {
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

const SourceLocation& InputRange::getLocation() const {
  return location;
}

SourceRange InputRange::commitRange() {
  assert(sequence != nullptr);
  return sequence->doCommitRange(begin, end);
}

SourceRange::SourceRange() :
  spans_data() {

}

SourceRange::SourceRange(
    std::vector<SourceSpan> spans_) :
  spans_data(std::move(spans_)) {

}

const std::vector<SourceSpan>& SourceRange::spans() const {
  return spans_data;
}

bool SourceRange::empty() const {
  return spans_data.empty();
}

InputSequence::InputSequence() :
  stream_stack(),
  window_begin(0),
  window_end(0),
  window() {

}

InputSequence::~InputSequence() = default;

void InputSequence::pushStream(
    std::shared_ptr<InputStream> stream_,
    std::string file_) {
  assert(stream_ != nullptr);

  auto file_ptr(std::make_shared<const std::string>(std::move(file_)));

  /* -- shelve the current window into the new frame */
  stream_stack.push_back({
      std::move(stream_),
      std::move(window),
      std::move(file_ptr),
      1,     /* -- line */
      1,     /* -- column */
      false  /* -- last_was_cr */
  });
  window.clear();
  window_end = window_begin;
}

void InputSequence::popStream() {
  assert(!stream_stack.empty());

  /* -- restore the shelved window */
  auto& saved(stream_stack.back().saved_window);
  for(auto& lc : saved) {
    window.push_back(std::move(lc));
  }
  window_end += saved.size();
  stream_stack.pop_back();
}

InputRange InputSequence::openRange() {
  return InputRange(this, doOpenRange(), currentLocation());
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

  return window[range_end_ - window_begin].codepoint;
}

std::string InputSequence::doGetString(
    std::size_t begin_,
    std::size_t end_) const {
  assert(begin_ >= window_begin && end_ <= window_end);

  std::string result_;
  for(std::size_t i_(begin_); i_ < end_; ++i_) {
    auto cp_(window[i_ - window_begin].codepoint);
    if(cp_ != EOS) {
      encodeUtf8(cp_, result_);
    }
  }
  return result_;
}

SourceRange InputSequence::doCommitRange(
    std::size_t begin_,
    std::size_t end_) {
  assert(begin_ == window_begin);
  assert(end_ >= begin_ && end_ <= window_end);

  /* -- build source spans: group consecutive characters by file */
  std::vector<SourceSpan> spans_;
  auto count_(end_ - begin_);
  if(count_ > 0) {
    auto& first_(window[0].location);
    auto current_file_(first_.file.get());
    SourceLocation span_start_(first_);
    SourceLocation span_end_(first_);

    for(std::size_t i_(1); i_ < count_; ++i_) {
      auto& loc_(window[i_].location);
      if(loc_.file.get() != current_file_) {
        /* -- file boundary: close current span and start a new one */
        spans_.push_back({std::move(span_start_), std::move(span_end_)});
        current_file_ = loc_.file.get();
        span_start_ = loc_;
      }
      span_end_ = loc_;
    }
    spans_.push_back({std::move(span_start_), std::move(span_end_)});
  }

  window.erase(
      window.begin(),
      window.begin() + static_cast<std::ptrdiff_t>(count_));
  window_begin = end_;

  return SourceRange(std::move(spans_));
}

SourceLocation InputSequence::currentLocation() const {
  /* -- if the window has characters, use the first one's location */
  if(!window.empty()) {
    return window.front().location;
  }
  /* -- otherwise use the tracking state of the top stream frame */
  if(!stream_stack.empty()) {
    auto& frame(stream_stack.back());
    return {frame.file, frame.line, frame.column};
  }
  return {};
}

bool InputSequence::extendWindow() {
  while(!stream_stack.empty()) {
    auto& frame(stream_stack.back());
    auto cp(frame.stream->readCodepoint());
    if(cp != EOS) {
      /* -- capture the location BEFORE updating the tracking state */
      SourceLocation loc{frame.file, frame.line, frame.column};

      /* -- update line/column tracking with CR/LF/CRLF handling:
       *    Both CR and LF are line breaks. LF immediately after CR
       *    is suppressed (CRLF counts as one line break). */
      if(cp == '\n' && frame.last_was_cr) {
        /* -- LF after CR: CRLF sequence, suppress the extra line break */
        frame.last_was_cr = false;
      }
      else if(cp == '\r') {
        frame.line++;
        frame.column = 1;
        frame.last_was_cr = true;
      }
      else if(cp == '\n') {
        frame.line++;
        frame.column = 1;
        frame.last_was_cr = false;
      }
      else {
        frame.column++;
        frame.last_was_cr = false;
      }

      window.push_back({cp, std::move(loc)});
      ++window_end;
      return true;
    }

    /* -- restore the shelved window and pop the frame */
    auto& saved(frame.saved_window);
    auto saved_size(saved.size());
    for(auto& lc : saved) {
      window.push_back(std::move(lc));
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
