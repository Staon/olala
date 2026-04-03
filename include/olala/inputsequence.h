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
#include <string>

namespace OLala {

/**
 * @brief Representation of end of sequence
 */
constexpr char32_t EOS(0xffffffff);

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
 * @brief Generic interface of an input sequence
 *
 * The input sequence keeps a physical window above a sequence of characters.
 * The end of the window expands with fetching of input characters. The begin
 * of the window moves when a range is committed.
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
    virtual ~InputSequence();

    /* -- avoid copying */
    InputSequence(
        const InputSequence&) = delete;
    InputSequence& operator=(
        const InputSequence&) = delete;

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
    /**
     * @brief Create new empty range at the beginning of the window
     *
     * @return Beginning offset of the opened range
     * @exception If some error happens
     */
    virtual std::size_t doOpenRange() = 0;

    /**
     * @brief Fetch character at specified offset
     *
     * The method fetches a character at offset @a range_end_. The offset
     * must be inside current window or it must point a character just
     * immediately following the window (max. 1 character out of the window).
     * If the following character is requested the current window is extended.
     *
     * @param range_end_ Offset of the range end.
     * @return The fetched character.
     */
    virtual char32_t doFetchCharacter(std::size_t range_end_) = 0;

    /**
     * @brief Get the string representation of a range
     *
     * @param begin_ Begin of the range
     * @param end_ End of the range (first offset not in the range)
     * @return UTF-8 encoded string
     */
    virtual std::string doGetString(
        std::size_t begin_,
        std::size_t end_) const = 0;

    /**
     * @brief Commit the range
     *
     * The method removes the range from the window. The range must be
     * the beginning of the window!
     *
     * @param begin_ Begin of the range (must be exactly the begin of the
     *     window).
     * @param end_ End of the range (first offset not in the range)
     */
    virtual void doCommitRange(std::size_t begin_, std::size_t end_) = 0;
    friend class InputRange;
};

} /* -- namespace OLala */

#endif /* OLALA_INPUTSEQUENCE_H */
