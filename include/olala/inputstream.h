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
#ifndef OLALA_INPUTSTREAM_H
#define OLALA_INPUTSTREAM_H

namespace OLala {

/**
 * @brief Representation of end of sequence
 */
constexpr char32_t EOS(0xffffffff);

/**
 * @brief Abstract source of Unicode codepoints
 */
class InputStream {
  public:
    InputStream();
    virtual ~InputStream();

    /* -- avoid copying */
    InputStream(const InputStream&) = delete;
    InputStream& operator=(const InputStream&) = delete;

    /**
     * @brief Read one Unicode codepoint from the stream
     *
     * @return The codepoint or EOS
     */
    virtual char32_t readCodepoint() = 0;
};

} /* -- namespace OLala */

#endif /* OLALA_INPUTSTREAM_H */
