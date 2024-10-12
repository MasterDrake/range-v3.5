#pragma once

#include <EASTL/internal/random/pcg_random.hpp>

namespace eastl
{

    struct RNG32Tag
    {};
    struct RNG64Tag
    {};

    template<typename T>
    struct RNGTraits;

    template<>
    struct RNGTraits<RNG32Tag>
    {
        using engine = pcg32_oneseq;
    };

    template<>
    struct RNGTraits<RNG64Tag>
    {
        using engine = pcg64_oneseq;
    };

    template<class Tag = RNG32Tag>
    class RNG
    {
    public:
        using engine = typename RNGTraits<Tag>::engine;
        using seed_type = typename engine::state_type;
        using result_type = typename engine::result_type;

        RNG() = default;

        explicit RNG(seed_type seed)
          : rng(seed)
        {}

        RNG(const RNG & o)
          : rng(o.rng)
        {}

        RNG(RNG && o) noexcept
          : rng(std::move(o.rng))
        {}

        RNG & operator=(const RNG & other)
        {
            return *this = RNG(other);
        }

        RNG & operator=(RNG && other) noexcept
        {
            eastl::swap(rng, other.rng);
            return *this;
        }
        
        template<typename SeedSeq, typename = eastl::enable_if_t<!eastl::is_same_v<eastl::remove_reference_t<SeedSeq>, RNG>>>
        RNG(SeedSeq && seedSeq) : rng(seedSeq)
        {}

        inline void seed()
        {
            rng.seed();
        }

        inline void seed(seed_type seed)
        {
            rng.seed(seed);
        }

        inline result_type operator()()
        {
            return rng();
        }
        // It generates a random number n such that 0 <= n < upper_bound
        inline result_type operator()(result_type upper_bound)
        {
            return rng(upper_bound);
        }

        // It generates a random number n such that lower_bound <= n < upper_bound
        inline result_type operator()(result_type lower_bound, result_type upper_bound)
        {
            return rng(upper_bound - lower_bound + 1) + lower_bound;
        }

        inline void discard(seed_type n)
        {
            rng.discard(n);
        }

        static constexpr result_type min()
        {
            return result_type(0UL);
        }

        static constexpr result_type max()
        {
            return result_type(~result_type(0UL));
        }

        inline bool operator==(const RNG & r) const
        {
            return rng == r.rng;
        }

        inline bool operator!=(const RNG & r) const
        {
            return !(*this == r);
        }

        // inline std::ostream& operator<<(std::ostream& out) const
        //{
        //	out << rng;
        //	return out;
        // }
        // inline std::istream& operator>>(std::istream& in)
        //{
        //	in >> rng;
        //	return in;
        // }

        // Advances the generator forward delta steps, but does so in logarithmic time.
        inline void advance(seed_type delta)
        {
            rng.advance(delta);
        }

        // Move the generator backwards delta steps, but does so in logarithmic time.
        inline void backstep(seed_type delta)
        {
            rng.backstep(delta);
        }

        inline seed_type operator-(const RNG & o) const
        {
            return rng - o.rng;
        }

    private:
        engine rng{};
    };

    using RNG32 = RNG<RNG32Tag>;
    using RNG64 = RNG<RNG64Tag>;

} // namespace eastl