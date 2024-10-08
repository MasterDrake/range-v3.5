/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_GENERATE_N_HPP
#define EARANGES_VIEW_GENERATE_N_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/range/primitives.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/facade.hpp>
#include <EARanges/view/generate.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename G>
    struct generate_n_view : view_facade<generate_n_view<G>, finite>
    {
    private:
        friend range_access;
        using result_t = invoke_result_t<G &>;
        semiregular_box_t<G> gen_;
        detail::non_propagating_cache<result_t> val_;
        eastl::size_t n_;
        struct cursor
        {
        private:
            generate_n_view * rng_;

        public:
            cursor() = default;
            explicit cursor(generate_n_view * rng)
              : rng_(rng)
            {}
            bool equal(default_sentinel_t) const
            {
                return 0 == rng_->n_;
            }
            result_t && read() const
            {
                if(!rng_->val_)
                    rng_->val_.emplace(rng_->gen_());
                return static_cast<result_t &&>(static_cast<result_t &>(*rng_->val_));
            }
            void next()
            {
                EARANGES_EXPECT(0 != rng_->n_);
                if(rng_->val_)
                    rng_->val_.reset();
                else
                    static_cast<void>(rng_->gen_());
                --rng_->n_;
            }
        };
        cursor begin_cursor()
        {
            return cursor{this};
        }

    public:
        generate_n_view() = default;
        explicit generate_n_view(G g, eastl::size_t n)
          : gen_(eastl::move(g))
          , n_(n)
        {}
        result_t & cached()
        {
            return *val_;
        }
        eastl::size_t size() const
        {
            return n_;
        }
    };

    namespace views
    {
        struct generate_n_fn
        {
            template(typename G)(
                requires invocable<G &> AND copy_constructible<G> AND
                    eastl::is_object<detail::decay_t<invoke_result_t<G &>>>::value AND
                    constructible_from<detail::decay_t<invoke_result_t<G &>>,
                                       invoke_result_t<G &>> AND
                    assignable_from<detail::decay_t<invoke_result_t<G &>> &,
                                    invoke_result_t<G &>>)
            generate_n_view<G> operator()(G g, eastl::size_t n) const
            {
                return generate_n_view<G>{eastl::move(g), n};
            }
        };

        /// \relates generate_n_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(generate_n_fn, generate_n)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::generate_n_view)

#endif
