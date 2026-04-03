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
#ifndef OLALA_VALUESTRING_H
#define OLALA_VALUESTRING_H

#include <string>

#include <olala/value.h>

namespace OLala {

/**
 * @brief Semantic value holding a string (e.g. matched text of a terminal)
 */
class ValueString : public Value {
  public:
    explicit ValueString(
        std::string value_);
    virtual ~ValueString() override;

    /* -- avoid copying */
    ValueString(
        const ValueString&) = delete;
    ValueString& operator=(
        const ValueString&) = delete;

    const std::string& getValue() const;

  private:
    std::string value;
};

} /* -- namespace OLala */

#endif /* OLALA_VALUESTRING_H */
