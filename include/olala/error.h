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
#ifndef OLALA_ERROR_H
#define OLALA_ERROR_H

#include <exception>
#include <string>

namespace OLala {

/**
 * @brief OLala error object
 */
class Error : public std::exception {
  public:
    explicit Error(
        const std::string& message_);
    Error(
        Error&& other) noexcept;
    virtual ~Error() override;

    /* -- avoid copying */
    Error(
        const Error&) = delete;
    Error& operator=(
        const Error&) = delete;

    /* -- std::exception interface */
    virtual const char* what() const noexcept override;

  private:
    std::string message;
};

} /* -- namespace OLala */

#endif /* OLALA_ERROR_H */
