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
#include <deque>
#include <list>
#include <utility>
#include <vector>

#include <olala/inputstream.h>
#include <olala/lookaheadstate.h>
#include <olala/parsercontext.h>
#include <olala/semanticstack.h>
#include <olala/sourcelocation.h>
#include <olala/symbol.h>
#include <olala/symbolptr.h>

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

struct InputSequence::Impl {
    struct LocatedCodepoint {
      char32_t codepoint;
      SourceLocation location;
    };

    struct StreamFrame {
      std::shared_ptr<InputStream> stream;
      std::deque<LocatedCodepoint> saved_window;
      std::shared_ptr<const std::string> file;
      int line;
      int column;
      bool last_was_cr;
    };

    std::vector<StreamFrame> stream_stack;
    std::size_t window_begin;
    std::size_t window_end;
    std::deque<LocatedCodepoint> window;
    SymbolPtr skipper;
    std::unique_ptr<SemanticStack> skipper_stack;
    std::unique_ptr<ParserContext> skipper_context;
    bool skipper_running;

    /**
     * @brief Get the source location at the current window beginning
     */
    SourceLocation currentLocation() const {
      if(!window.empty()) {
        return window.front().location;
      }
      if(!stream_stack.empty()) {
        auto& frame(stream_stack.back());
        return {frame.file, frame.line, frame.column};
      }
      return {};
    }

    /**
     * @brief Extend the window by one or more characters
     */
    bool extendWindow() {
      while(!stream_stack.empty()) {
        auto& frame(stream_stack.back());
        auto cp(frame.stream->readCodepoint());
        if(cp != EOS) {
          SourceLocation loc{frame.file, frame.line, frame.column};

          if(cp == '\n' && frame.last_was_cr) {
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

        auto& saved(frame.saved_window);
        auto saved_size(saved.size());
        for(auto& lc : saved) {
          window.push_back(std::move(lc));
        }
        window_end += saved_size;
        stream_stack.pop_back();

        if(saved_size > 0) {
          return true;
        }
      }
      return false;
    }

    /**
     * @brief Encode a Unicode codepoint as UTF-8
     */
    static void encodeUtf8(
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

    /**
     * @brief Run the skipper symbol if set
     */
    void runSkipper() {
      if(skipper == nullptr || skipper_running) {
        return;
      }

      struct SkipperGuard {
        bool& running;
        SemanticStack& stack;
        explicit SkipperGuard(bool& running_, SemanticStack& stack_)
          : running(running_), stack(stack_) { running = true; }
        ~SkipperGuard() { stack.clear(); running = false; }
      } guard_(skipper_running, *skipper_stack);

      skipper->parse(*skipper_context, nullptr);
    }
};

InputSequence::InputSequence(
    SymbolPtr skipper_) :
  impl(std::make_unique<Impl>()) {
  impl->stream_stack = {};
  impl->window_begin = 0;
  impl->window_end = 0;
  impl->window = {};
  impl->skipper = std::move(skipper_);
  impl->skipper_stack = impl->skipper ? std::make_unique<SemanticStack>() : nullptr;
  impl->skipper_context = impl->skipper ? std::make_unique<ParserContext>(this, impl->skipper_stack.get()) : nullptr;
  impl->skipper_running = false;
}

InputSequence::~InputSequence() = default;

void InputSequence::pushStream(
    const std::shared_ptr<InputStream>& stream_,
    std::string file_) {
  assert(stream_ != nullptr);

  auto file_ptr(std::make_shared<const std::string>(std::move(file_)));

  impl->stream_stack.push_back({
      std::move(stream_),
      std::move(impl->window),
      std::move(file_ptr),
      1,     /* -- line */
      1,     /* -- column */
      false  /* -- last_was_cr */
  });
  impl->window.clear();
  impl->window_end = impl->window_begin;

  impl->runSkipper();
}

void InputSequence::popStream() {
  assert(!impl->stream_stack.empty());

  auto& saved(impl->stream_stack.back().saved_window);
  for(auto& lc : saved) {
    impl->window.push_back(std::move(lc));
  }
  impl->window_end += saved.size();
  impl->stream_stack.pop_back();
}

InputRange InputSequence::openRange() {
  return InputRange(this, doOpenRange(), impl->currentLocation());
}

std::size_t InputSequence::doOpenRange() {
  return impl->window_begin;
}

char32_t InputSequence::doFetchCharacter(
    std::size_t range_end_) {
  assert(range_end_ >= impl->window_begin);

  while(range_end_ >= impl->window_end) {
    if(!impl->extendWindow()) {
      return EOS;
    }
  }

  return impl->window[range_end_ - impl->window_begin].codepoint;
}

std::string InputSequence::doGetString(
    std::size_t begin_,
    std::size_t end_) const {
  assert(begin_ >= impl->window_begin && end_ <= impl->window_end);

  std::string result_;
  for(std::size_t i_(begin_); i_ < end_; ++i_) {
    auto cp_(impl->window[i_ - impl->window_begin].codepoint);
    if(cp_ != EOS) {
      Impl::encodeUtf8(cp_, result_);
    }
  }
  return result_;
}

SourceRange InputSequence::doCommitRange(
    std::size_t begin_,
    std::size_t end_) {
  assert(begin_ == impl->window_begin);
  assert(end_ >= begin_ && end_ <= impl->window_end);

  std::list<SourceSpan> spans_;
  auto count_(end_ - begin_);
  if(count_ > 0) {
    auto& first_(impl->window[0].location);
    auto current_file_(first_.file.get());
    SourceLocation span_start_(first_);
    SourceLocation span_end_(first_);

    for(std::size_t i_(1); i_ < count_; ++i_) {
      auto& loc_(impl->window[i_].location);
      if(loc_.file.get() != current_file_) {
        spans_.push_back({std::move(span_start_), std::move(span_end_)});
        current_file_ = loc_.file.get();
        span_start_ = loc_;
      }
      span_end_ = loc_;
    }
    spans_.push_back({std::move(span_start_), std::move(span_end_)});
  }

  impl->window.erase(
      impl->window.begin(),
      impl->window.begin() + static_cast<std::ptrdiff_t>(count_));
  impl->window_begin = end_;

  impl->runSkipper();

  return SourceRange(std::move(spans_));
}

} /* -- namespace OLala */
