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
#ifndef OLALA_ADAPTABLE_H
#define OLALA_ADAPTABLE_H

#include <typeinfo>

#include <olala/adaptableexception.h>

namespace OLala {

/**
 * @brief An interface marking adaptable interfaces
 */
class Adapted {
  public:
    Adapted();
    virtual ~Adapted();

    /* -- avoid copying */
    Adapted(
        const Adapted&) = delete;
    virtual Adapted& operator=(
        const Adapted&) = delete;
};

/**
 * @brief Adaptable interface
 */
class Adaptable {
  public:
    Adaptable();
    virtual ~Adaptable();

    /* -- avoid copying */
    Adaptable(
        const Adaptable&) = delete;
    Adaptable& operator=(
        const Adaptable&) = delete;

    /**
     * @brief Check whether the object can be adapted to a type
     *
     * @tparam Adapt Requested type
     * @return True if it can be adapted
     */
    template<typename Adapt> [[nodiscard]] bool canAdaptTo() const {
      Adapt* adapted_(static_cast<Adapt*>(doAdapt(typeid(Adapt))));
      return adapted_ != nullptr;
    }

    /**
     * @brief Adapt to a type
     *
     * @tparam Adapt Requested type
     * @return The adapted type
     */
    template<typename Adapt> const Adapt& adaptTo() const {
      auto* adapted_(doAdapt(typeid(Adapt)));
      if(adapted_ == nullptr)
        throw AdaptableException(
            typeid(*this).name(),
            typeid(Adapt).name());
      return *static_cast<Adapt*>(adapted_);
    }

    /**
     * @brief Adapt to a type
     *
     * @tparam Adapt Requested type
     * @return The adapted type
     */
    template<typename Adapt> Adapt& adaptTo() {
      const auto* adapted_(doAdapt(typeid(Adapt)));
      if(adapted_ == nullptr)
        throw AdaptableException(
            typeid(*this).name(),
            typeid(Adapt).name());
      return *static_cast<Adapt*>(const_cast<Adapted*>(adapted_));
    }

  protected:
    [[nodiscard]] virtual const Adapted* doAdapt(
        const std::type_info& info_) const = 0;
};

} // OLala

#endif //OLALA_ADAPTABLE_H
