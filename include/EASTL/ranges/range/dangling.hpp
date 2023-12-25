/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_RANGE_DANGLING_HPP
#define RANGES_V3_RANGE_DANGLING_HPP

//#include <EASTL/utility.h>
#include "EASTL/utility.h"

#include "../concepts/concepts.hpp"

#include "../range_fwd.hpp"

#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \ingroup group-range
    /// A placeholder for an iterator or a sentinel into a range that may
    /// no longer be valid.
    struct dangling
    {
        dangling() = default;
        /// Implicit converting constructor; ignores argument
        template(typename T)(requires not_same_as_<T, dangling>)
        constexpr dangling(T &&)
        {}
    };

    /// \cond
    namespace detail
    {
        template(class R, class U)(requires range<R>)
            using maybe_dangling_ =     //
                meta::conditional_t<detail::_borrowed_range<R>, U, dangling>;
    }
    /// \endcond

    template<typename Rng>
    using borrowed_iterator_t = detail::maybe_dangling_<Rng, iterator_t<Rng>>;

} // namespace ranges

#include "../detail/epilogue.hpp"

#endif