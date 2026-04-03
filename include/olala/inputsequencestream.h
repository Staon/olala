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
#ifndef OLALA_INPUTSEQUENCESTREAM_H
#define OLALA_INPUTSEQUENCESTREAM_H

#include <deque>
#include <iosfwd>

#include <olala/inputsequence.h>

namespace OLala {

/**
 * @brief Implementation of the input sequence based on a UTF-8 C++ stream
 *
 * The stream is expected to provide UTF-8 encoded bytes. Characters are
 * decoded into Unicode codepoints (char32_t).
 */
class InputSequenceStream : public InputSequence {
  public:
    /**
     * @brief Ctor
     *
     * @param is_ The input stream (UTF-8 encoded). The ownership is not taken.
     */
    explicit InputSequenceStream(
        std::istream* is_);

    /**
     * @brief Dtor
     */
    virtual ~InputSequenceStream() override;

    /* -- avoid copying */
    InputSequenceStream(
        const InputSequenceStream&) = delete;
    InputSequenceStream& operator=(
        const InputSequenceStream&) = delete;

  private:
    /* -- InputSequence interface */
    virtual std::size_t doOpenRange() override;
    virtual char32_t doFetchCharacter(
        std::size_t range_end_) override;
    virtual std::string doGetString(
        std::size_t begin_,
        std::size_t end_) const override;
    virtual void doCommitRange(
        std::size_t begin_,
        std::size_t end_) override;

    /**
     * @brief Read one Unicode codepoint from the UTF-8 stream
     *
     * @return The codepoint or EOS
     */
    char32_t readCodepoint();

    /**
     * @brief Encode a Unicode codepoint as UTF-8
     *
     * @param cp_ The codepoint
     * @param out_ Output string to append to
     */
    static void encodeUtf8(
        char32_t cp_,
        std::string& out_);

    std::istream* is;
    std::size_t window_begin;
    std::size_t window_end;
    std::deque<char32_t> window;
};

} /* -- namespace OLala */

#endif /* OLALA_INPUTSEQUENCESTREAM_H */
