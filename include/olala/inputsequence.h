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
#ifndef OLALA_INPUTSEQUENCE_H
#define OLALA_INPUTSEQUENCE_H

#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include <vector>

#include <olala/inputstream.h>

namespace OLala {

class InputSequence;

/**
 * @brief Range of characters in an input sequence
 */
class InputRange {
  public:
    /**
     * @brief Ctor - invalid range
     */
    InputRange();

    /**
     * @brief Copy ctor
     */
    InputRange(const InputRange& other);

    /**
     * @brief Move ctor
     */
    InputRange(InputRange&& other) noexcept;

    /**
     * @brief Dtor
     */
    ~InputRange();

    /**
     * @brief Copy assignment
     */
    InputRange& operator=(const InputRange& other);

    /**
     * @brief Move assignment
     */
    InputRange& operator=(InputRange&& other) noexcept;

    /**
     * @brief Fetch a character from an input sequence
     *
     * This method gets next character from the sequence and expands the
     * range.
     *
     * @return The character or EOS.
     * @exception If an error happens
     */
    char32_t fetchCharacter();

    /**
     * @brief Get the string value of this range
     *
     * @return UTF-8 encoded string of the characters in the range
     */
    std::string getString() const;

    /**
     * @brief Commit the character range
     *
     * The character range is removed from the input sequence. The range
     * must be the beginning of the sequence!
     */
    void commitRange();

  private:
    explicit InputRange(
        InputSequence* sequence_,
        std::size_t begin_);
    friend class InputSequence;

    InputSequence* sequence;
    std::size_t begin;
    std::size_t end;
};

/**
 * @brief Concrete input sequence with lookahead buffer and stream stack
 *
 * The input sequence keeps a physical window above a sequence of characters.
 * The end of the window expands with fetching of input characters. The begin
 * of the window moves when a range is committed.
 *
 * The sequence owns a stack of input streams. Characters are read from the
 * top stream. When EOS is reached, prefetched characters from the previous
 * stream level are restored and reading continues from the stream below.
 *
 * When a stream is pushed, the current window contents are saved (shelved)
 * into the stream frame. This ensures the pushed stream's content appears
 * immediately, before any prefetched characters from the outer stream.
 * The shelved characters are restored when the pushed stream is exhausted.
 *
 * Contract: pushStream() must be called at a synchronization point — after
 * a range commit — when no active range references any prefetched characters
 * in the window.
 *
 * Generally, the interface works:
 * <pre>
 *     auto range(sequence.openRange());
 *     range.fetchCharacter();
 *     range.fetchCharacter();
 *     range.commitRange();
 * </pre>
 *
 * The fetchCharacter() functions expand the window. The commitRange()
 * function moves the beginning of the window to the end of the range.
 *
 * User may have several ranges. However, just one range starting at
 * the beginning of the window may be committed.
 */
class InputSequence {
  public:
    InputSequence();
    ~InputSequence();

    /* -- avoid copying */
    InputSequence(
        const InputSequence&) = delete;
    InputSequence& operator=(
        const InputSequence&) = delete;

    /**
     * @brief Push a new input stream onto the stack
     *
     * The current window contents are shelved and restored when this stream
     * is exhausted. This must be called at a synchronization point (after
     * a commit) when no active range references prefetched window characters.
     *
     * @param stream_ The stream (ownership is taken)
     */
    void pushStream(std::shared_ptr<InputStream> stream_);

    /**
     * @brief Pop the current input stream from the stack
     *
     * Restores the shelved window contents from this stream's frame.
     */
    void popStream();

    /**
     * @brief Open an input range
     *
     * The method opens new input range beginning at current start of the input
     * window.
     *
     * @return The opened range
     * @exception If some error happens
     */
    InputRange openRange();

  private:
    struct StreamFrame {
      std::shared_ptr<InputStream> stream;
      std::deque<char32_t> saved_window;
    };

    std::size_t doOpenRange();
    char32_t doFetchCharacter(std::size_t range_end_);
    std::string doGetString(
        std::size_t begin_,
        std::size_t end_) const;
    void doCommitRange(std::size_t begin_, std::size_t end_);

    /**
     * @brief Extend the window by one or more characters
     *
     * Reads from the top stream. On EOS, restores the shelved window
     * from that frame and pops it, then continues with the next frame.
     *
     * @return true if the window was extended, false if no more data
     */
    bool extendWindow();

    /**
     * @brief Encode a Unicode codepoint as UTF-8
     *
     * @param cp_ The codepoint
     * @param out_ Output string to append to
     */
    static void encodeUtf8(
        char32_t cp_,
        std::string& out_);

    friend class InputRange;

    std::vector<StreamFrame> stream_stack;
    std::size_t window_begin;
    std::size_t window_end;
    std::deque<char32_t> window;
};

} /* -- namespace OLala */

#endif /* OLALA_INPUTSEQUENCE_H */
