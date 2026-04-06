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
#include <olala/sourcelocation.h>

#include <utility>

namespace OLala {

SourceRange::SourceRange() :
  spans_data() {

}

SourceRange::SourceRange(
    std::list<SourceSpan> spans_) :
  spans_data(std::move(spans_)) {

}

const std::list<SourceSpan>& SourceRange::spans() const {
  return spans_data;
}

SourceRange SourceRange::merge(const SourceRange& other_) const {
  if(other_.spans_data.empty()) {
    return *this;
  }
  if(spans_data.empty()) {
    return other_;
  }

  std::list<SourceSpan> merged_(spans_data);

  auto it_(other_.spans_data.begin());
  /* -- merge the boundary spans if they are from the same file */
  if(merged_.back().end.file.get() == it_->start.file.get()) {
    merged_.back().end = it_->end;
    ++it_;
  }
  merged_.insert(merged_.end(), it_, other_.spans_data.end());

  return SourceRange(std::move(merged_));
}

bool SourceRange::empty() const {
  return spans_data.empty();
}

} /* -- namespace OLala */
