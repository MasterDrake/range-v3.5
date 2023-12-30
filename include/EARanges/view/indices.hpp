/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_INDICES_HPP
#define EARANGES_VIEW_INDICES_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/range/concepts.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/iota.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    namespace views
    {
        /// Half-open range of indices: [from, to).
        struct indices_fn : iota_view<std::size_t>
        {
            indices_fn() = default;

            template(typename Val)(requires integral<Val>)
            iota_view<Val, Val> operator()(Val to) const
            {
                return {Val(), to};
            }
            template(typename Val)(requires integral<Val>)
            iota_view<Val, Val> operator()(Val from, Val to) const
            {
                return {from, to};
            }
        };

        /// Inclusive range of indices: [from, to].
        struct closed_indices_fn
        {
            template(typename Val)(requires integral<Val>)
            closed_iota_view<Val> operator()(Val to) const
            {
                return {Val(), to};
            }
            template(typename Val)(requires integral<Val>)
            closed_iota_view<Val> operator()(Val from, Val to) const
            {
                return {from, to};
            }
        };

        /// \relates indices_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(indices_fn, indices)

        /// \relates closed_indices_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(closed_indices_fn, closed_indices)
    } // namespace views
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_VIEW_INDICES_HPP