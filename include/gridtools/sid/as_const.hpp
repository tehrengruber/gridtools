/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <type_traits>
#include <utility>

#include "../common/host_device.hpp"
#include "../meta/macros.hpp"
#include "../meta/type_traits.hpp"
#include "concept.hpp"
#include "delegate.hpp"
#include "simple_ptr_holder.hpp"

namespace gridtools {
    namespace sid {
        namespace as_const_impl_ {
            template <class Sid>
            class const_adapter : public delegate<Sid> {
                struct const_ptr_holder {
                    sid::ptr_holder_type<Sid> m_impl;

                    GT_CONSTEXPR GT_FUNCTION std::add_const_t<sid::element_type<Sid>> *operator()() const {
                        return m_impl();
                    }

                    friend GT_CONSTEXPR GT_FUNCTION const_ptr_holder operator+(
                        const_ptr_holder const &obj, sid::ptr_diff_type<Sid> offset) {
                        return {obj.m_impl + offset};
                    }
                };

                friend const_ptr_holder sid_get_origin(const_adapter const &obj) {
                    return {sid::get_origin(const_cast<Sid &>(obj.impl()))};
                }
                using sid::delegate<Sid>::delegate;
            };
        } // namespace as_const_impl_

        /**
         *   Returns a `SID`, which ptr_type is a pointer to const.
         *   enabled only if the original ptr_type is a pointer.
         *
         *   TODO(anstaf): at a moment the generated ptr holder always has `host_device` `operator()`
         *                 probably might we need the `host` and `device` variations as well
         */
        template <class SrcRef,
            class Src = std::decay_t<SrcRef>,
            std::enable_if_t<std::is_pointer<sid::ptr_type<Src>>::value, int> = 0>
        as_const_impl_::const_adapter<Src> as_const(SrcRef &&src) {
            return as_const_impl_::const_adapter<Src>{std::forward<SrcRef>(src)};
        }

        template <class Src>
        decltype(auto) add_const(std::false_type, Src &&src) {
            return std::forward<Src>(src);
        }

        template <class SrcRef,
            class Src = std::decay_t<SrcRef>,
            class Ptr = sid::ptr_type<Src>,
            std::enable_if_t<std::is_pointer<Ptr>::value && !std::is_const<std::remove_pointer_t<Ptr>>::value, int> = 0>
        auto add_const(std::true_type, SrcRef &&src) {
            return as_const(std::forward<SrcRef>(src));
        }

        template <class SrcRef,
            class Src = std::decay_t<SrcRef>,
            class Ptr = sid::ptr_type<Src>,
            std::enable_if_t<!std::is_pointer<Ptr>::value || std::is_const<std::remove_pointer_t<Ptr>>::value, int> = 0>
        auto add_const(std::true_type, SrcRef &&src) {
            return std::forward<SrcRef>(src);
        }

    } // namespace sid
} // namespace gridtools
