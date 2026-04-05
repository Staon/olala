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
#ifndef OLALA_INPUTSTREAMSTREAM_H
#define OLALA_INPUTSTREAMSTREAM_H

#include <iosfwd>
#include <memory>

#include <olala/inputstream.h>

namespace OLala {

/**
 * @brief Implementation of InputStream based on a UTF-8 C++ stream
 *
 * The stream is expected to provide UTF-8 encoded bytes. Characters are
 * decoded into Unicode codepoints (char32_t).
 */
class InputStreamStream : public InputStream {
  public:
    /**
     * @brief Ctor
     *
     * @param is_ The input stream (UTF-8 encoded). The ownership is not taken.
     */
    explicit InputStreamStream(
        std::unique_ptr<std::istream>&& is_);

    /**
     * @brief Dtor
     */
    virtual ~InputStreamStream() override;

    /* -- avoid copying */
    InputStreamStream(
        const InputStreamStream&) = delete;
    InputStreamStream& operator=(
        const InputStreamStream&) = delete;

    /* -- InputStream interface */
    virtual char32_t readCodepoint() override;

    std::unique_ptr<std::istream> is;
};

} /* -- namespace OLala */

#endif /* OLALA_INPUTSTREAMSTREAM_H */
