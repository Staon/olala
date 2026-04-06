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
#ifndef OLALA_SOURCELOCATION_H
#define OLALA_SOURCELOCATION_H

#include <list>
#include <memory>
#include <string>

namespace OLala {

/**
 * @brief Source location in a parsed file
 */
struct SourceLocation {
    std::shared_ptr<const std::string> file;
    int line;
    int column;
};

/**
 * @brief A contiguous span within a single source file
 */
struct SourceSpan {
    SourceLocation start;
    SourceLocation end;
};

/**
 * @brief Location of a parsed object in the source
 *
 * A parsed object may span multiple source files (e.g. when an include
 * is expanded within the matched range). The location is represented
 * as an ordered list of per-file spans.
 */
class SourceRange {
  public:
    SourceRange();
    explicit SourceRange(
        std::list<SourceSpan> spans_);

    /**
     * @brief Get the list of per-file spans
     */
    const std::list<SourceSpan>& spans() const;

    /**
     * @brief Merge with another range
     *
     * Creates a new range starting at the beginning of this range and
     * ending at the end of @a other_. If the last span of this range
     * and the first span of the other range are from the same file,
     * they are merged into a single span.
     *
     * @param other_ The range to append
     * @return The merged range
     */
    SourceRange merge(const SourceRange& other_) const;

    /**
     * @brief Check whether the range is empty
     */
    bool empty() const;

  private:
    std::list<SourceSpan> spans_data;
};

} /* -- namespace OLala */

#endif /* OLALA_SOURCELOCATION_H */
