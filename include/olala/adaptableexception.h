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
#ifndef OLALA_ADAPTABLEEXCEPTION_H
#define OLALA_ADAPTABLEEXCEPTION_H

#include <exception>
#include <string>

namespace OLala {

/**
 * @brief Adaptable exception - object cannot be adapted
 */
class AdaptableException : std::exception {
  public:
    AdaptableException(
        const char* type_,
        const char* requested_);
    AdaptableException(
        AdaptableException&& src_);
    virtual ~AdaptableException() override;

    /* -- avoid copying */
    AdaptableException(
        const AdaptableException&) = delete;
    AdaptableException& operator=(
        const AdaptableException&) = delete;

    /* -- std::exception interface */
    virtual const char* what() const noexcept override;

  private:
    std::string message_;
};

} /* -- namespace OLala */

#endif /* -- OLALA_ADAPTABLEEXCEPTION_H */
