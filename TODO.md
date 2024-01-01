* Add contiguous iterator utilities. How about `is_contiguous_iterator` and `as_contiguous_range`:

    ```
    CPP_template(typename I, typename S)(
        requires RandomAccessIterator<I> &&
            SizedSentinel<S, I> &&
            is_contiguous_iterator<I>())
    subrange<std::add_pointer_t<iter_reference_t<I>>>
    as_contiguous_range(I begin, S end)
    {
        if(begin == end)
            return {nullptr, nullptr};
        else
            return {addressof(*begin), addressof(*begin) + (end - begin)};
    }
    ```
* Longer-term goals:
  - Make `inplace_merge` work with forward iterators
  - Make the sorting algorithms work with forward iterators

* Maybe iterators are not necessarily countable. Is there a relation between
  the ability to be able to subtract two iterators to find the distance, and
  with the existence of a DistanceType associated type? Think of:
  - counted iterators (subtractable regardless of traversal category)
  - repeat_view iterators (*not* subtractable but could be random access otherwise)
  - infinite ranges (only countable with an infinite precision integer which we lack)

  # TODO:
  - Fix broken views:
      - Tokenize
      - Split: ha senso visto che non gli piacciono le stringhe.

  - Add execution policies to algorithms:
    - seq
    - seq_vec/sec_simd
    - par
    - par_vec/par_simd
  
  - Check if eastl::algorithms == ranges::algorithms
  
  - Add ranges::clamp that clamps every element in a range.
      - https://en.cppreference.com/w/cpp/algorithm/ranges/clamp 
  - Rimuovere il concetto di DEPRECATED, incluso iterator_range
  - Sostituire EARANGES_ASSERT con EASTL_ASSERT o EASTL_ASSERT_MSG o EASTL_CT_ASSERT
  - Contorllare se ranges ha un abort o un fail e usare EASTL_FAIL_MSG
  - Sostituire tutte le chiamate a throw e exceptions con EASTL_EXCEPTIONS_ENABLED
  - Controllare se serve in config o dove altro, usare EASTL_CPP17_INLINE_VARIABLE invece di chissà che diavoleria
  - Creare un namespace per EASTL_SIZE_T e usare quello invece che std::size_t, ma occhio a uintptr_t che mi sembra unsigned al contrario di EASTL_SSIZE_T

 ## ADDRESSOF CONSTEXPR
 - In EASTL/internal/config.h after EASTL_EXCEPTIONS_ENABLED.
  ```c++
  ///////////////////////////////////////////////////////////////////////////////
    // EASTL_ADDRESSOF_CONSTEXPR
    //
    // Defined as 0 or 1. Default is 1.
    // This is additional code for EASTL/internal/config.h for the users that care about EARanges constexpr facilities.
	// This basically allows you to control whether eastl::addressof should be constexpr or not, this is a C++17 change.
	// (The expression std::addressof(e) is a constant subexpression, if e is an lvalue constant subexpression.) - https://en.cppreference.com/w/cpp/memory/addressof
	// This allows ranges to be evaluated a compile time.
	// This config is required because the definition of addressof changes, due the fact that the implementation is not compatibile with compile-time due the use or reinterpret_cast.
	// According to cppreference, MSVC, GCC and Clang all use the instrinsic function __builtin_addressof(value).
	// So this is a change that you may or may not want, depending on whether you're interested in the compile-time facilities of ranges.
	// It requires changes to <EASTL/internal/memory_base.h>
	// 
	// #if !EASTL_ADDRESSOF_CONSTEXPR
	//
	// template<typename T>
	// T* addressof(T& value) EA_NOEXCEPT
	// {
	// 	return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(value)));
	// }
	// 
	// #else
	// 
	// 	template<typename T>
	// 	constexpr T* addressof(T& value) EA_NOEXCEPT
	// 	{
	// 		return __builtin_addressof(value);
	// 	}
	// 
	// #endif
	// 
    //

	#ifndef EASTL_ADDRESSOF_CONSTEXPR
	#define EASTL_ADDRESSOF_CONSTEXPR 1
	#endif

```


- That's EASTL/internal/memory_base.h

```c++
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_INTERNAL_MEMORY_BASE_H
#define EASTL_INTERNAL_MEMORY_BASE_H

#include <EASTL/internal/config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif


////////////////////////////////////////////////////////////////////////////////////////////
// This file contains basic functionality found in the standard library 'memory' header that
// have limited or no dependencies.  This allows us to utilize these utilize these functions
// in other EASTL code while avoid circular dependencies.
////////////////////////////////////////////////////////////////////////////////////////////

namespace eastl
{
	/// addressof
	///
	/// From the C++11 Standard, section 20.6.12.1
	/// Returns the actual address of the object or function referenced by r, even in the presence of an overloaded operator&.
	///
#if !EASTL_ADDRESSOF_CONSTEXPR

	template<typename T>
	T* addressof(T& value) EA_NOEXCEPT
	{
		return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(value)));
	}

#else

	template<typename T>
	constexpr T* addressof(T& value) EA_NOEXCEPT
	{
		return __builtin_addressof(value);
	}

#endif

} // namespace eastl

#endif // EASTL_INTERNAL_MEMORY_BASE_H
```

## RANDOM
- Q: Why it's not an option??
- A: Even tho random_number_generator and discrete_distribution are optional, the original <EASTL/random.h> implementation is very very scarce, with a broken int_distribution that doesn't take ints also it doesn't provide a real_distribution, for discrete_distribution for algorithms using weights, like loot drops.
- So you can skip the includes of rng and dd but the int_distribution static assert will fail with RNGs like std::mt19973 since its result_type should be an uint, not int.
- Also, it's kinda a big change, so you have to want it.
- https://en.cppreference.com/w/cpp/header/random
- TODO: 2) Creare un eastl::random_device, somehow :(

- <EASTL/random_number_generator.h>
- Needs pcg files in EASTL/internal/random/
```c++
#pragma once

#include <EASTL/internal/random/pcg_random.hpp>

//TODO: Utilizzare template per creare più versioni a seconda se si vuole 64 o 32 bit come return value, fast or unique e blah blah blah
class RNG
{
public:
	using seed_type   =	pcg32_oneseq::state_type;
	using result_type = pcg32_oneseq::result_type;

	RNG() = default;

	explicit RNG(seed_type seed) : rng(seed){}

	RNG(const RNG& o) :rng(o.rng) {}

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
	//It generates a random number n such that 0 <= n < upper_bound
	inline result_type operator()(result_type upper_bound)
	{
		return rng(upper_bound);
	}

	//It generates a random number n such that lower_bound <= n < upper_bound
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

	inline bool operator==( const RNG& r) const
	{
		return rng == r.rng;
	}
	//bool operator!=(const RNG& r) const
	//{
	//	return !(*this == r);
	//}
	inline std::ostream& operator<<(std::ostream& out) const
	{
		out << rng;
		return out;
	}
	inline std::istream& operator>>(std::istream& in)
	{
		in >> rng;
		return in;
	}
	//Advances the generator forward delta steps, but does so in logarithmic time.
	inline void advance(seed_type delta)
	{
		rng.advance(delta);
	}
	//Move the generator backwards delta steps, but does so in logarithmic time.
	inline void backstep(seed_type delta)
	{
		rng.backstep(delta);
	}
	inline seed_type operator-(const RNG& o) const
	{
		return rng - o.rng;
	}
private:
	pcg32_oneseq rng{};
};


class RNG_64
{
public:
	using seed_type   = pcg64_oneseq::state_type;
	using result_type = pcg64_oneseq::result_type;

	RNG_64() = default;

	explicit RNG_64(seed_type seed) : rng(seed) {}

	RNG_64(const RNG_64& o) :rng(o.rng) {}

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
	//It generates a random number n such that 0 <= n < upper_bound
	inline result_type operator()(result_type upper_bound)
	{
		return rng(upper_bound);
	}

	//It generates a random number n such that lower_bound <= n < upper_bound
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

	inline bool operator==(const RNG_64& r) const
	{
		return rng == r.rng;
	}
	//bool operator!=(const RNG& r) const
	//{
	//	return !(*this == r);
	//}
	inline std::ostream& operator<<(std::ostream& out) const
	{
		out << rng;
		return out;
	}
	inline std::istream& operator>>(std::istream& in)
	{
		in >> rng;
		return in;
	}
	//Advances the generator forward delta steps, but does so in logarithmic time.
	inline void advance(seed_type delta)
	{
		rng.advance(delta);
	}
	//Move the generator backwards delta steps, but does so in logarithmic time.
	inline void backstep(seed_type delta)
	{
		rng.backstep(delta);
	}
	inline seed_type operator-(const RNG_64& o) const
	{
		return rng - o.rng;
	}
private:
	pcg64_oneseq rng{};
};
```


- New <EASTL/random.h> file with fixed int and a new real distribution.

```c++
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file defines random number generation like the std C++ <random> header. 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_RANDOM_H
#define EASTL_RANDOM_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#include <EASTL/internal/config.h>
#include <EASTL/numeric_limits.h>

#include <EASTL/random_number_generator.h>


///////////////////////////////////////////////////////////////////////////////
// min/max workaround
//
// MSVC++ has #defines for min/max which collide with the min/max algorithm
// declarations. The following may still not completely resolve some kinds of
// problems with MSVC++ #defines, though it deals with most cases in production
// game code.
//
#if EASTL_NOMINMAX
	#ifdef min
		#undef min
	#endif
	#ifdef max
		#undef max
	#endif
#endif


namespace eastl
{
	using default_random_engine = RNG;

	//TODO: 2)No idea on where to begin;
	class random_device;


	// Implements a uniform distribution of values generated by a Generator, 
	// where Generator is typically a random or pseudo-random number generator.
	// Note that the min/max range for this class is inclusive, so if you want 
	// random integers 0, 1, 2, and 3, then you need to init this class with (0, 3)
	// and not (0, 4).
	// See the C++11 Standard, section 26.5.1.6
	template<class IntType = int>
	class uniform_int_distribution
	{
		static_assert(eastl::is_integral<IntType>::value, "uniform_int_distribution: IntType must be integral.");

	public:
		typedef IntType result_type;

		// For uniform_int_distribution, param_type defines simply the min and max values of
		// the range returned by operator(). It may mean something else for other distribution types.
		struct param_type
		{
			explicit param_type(IntType a = 0, IntType b = eastl::numeric_limits<IntType>::max());

			result_type a() const;
			result_type b() const;

			bool operator==(const param_type& x) { return (x.mA == mA) && (x.mB == mB); }
			bool operator!=(const param_type& x) { return (x.mA != mA) || (x.mB != mB); }

		protected:
			IntType mA;
			IntType mB;
		};

		uniform_int_distribution(IntType a = 0, IntType b = eastl::numeric_limits<IntType>::max());
		uniform_int_distribution(const param_type& params);

		void reset();

		template<class Generator>
		result_type operator()(Generator& g) const;

		template<class Generator>
		result_type operator()(Generator& g, const param_type& params) const;

		result_type a() const;
		result_type b() const;

		param_type param() const;
		void param(const param_type& params);

		result_type min() const;
		result_type max() const;

		protected:
			param_type mParam;
	};



	///////////////////////////////////////////////////////////////////////
	// uniform_int_distribution
	///////////////////////////////////////////////////////////////////////

	template<class IntType>
	inline uniform_int_distribution<IntType>::param_type::param_type(IntType aValue, IntType bValue) : mA(aValue), mB(bValue)
	{
		EASTL_ASSERT(aValue <= bValue);
	}

	template<class IntType>
	inline typename uniform_int_distribution<IntType>::result_type 
	uniform_int_distribution<IntType>::param_type::a() const
	{
		return mA;
	}

	template<class IntType>
	inline typename uniform_int_distribution<IntType>::result_type 
	uniform_int_distribution<IntType>::param_type::b() const
	{
		return mB;
	}

	template<class IntType>
	inline uniform_int_distribution<IntType>::uniform_int_distribution(IntType aValue, IntType bValue) : mParam(aValue, bValue)
	{
		// Nothing more to do.
	}

	template<class IntType>
	inline uniform_int_distribution<IntType>::uniform_int_distribution(const param_type& params) : mParam(params)
	{
		// Nothing more to do.
	}

	template<class IntType>
	void uniform_int_distribution<IntType>::reset()
	{
		// Nothing to do.
	}

	template<class IntType>
	template<class Generator>
	inline typename uniform_int_distribution<IntType>::result_type 
	uniform_int_distribution<IntType>::operator()(Generator& g) const
	{
		return operator()(g, mParam);
	}

	template<class IntType>
	template<class Generator>
	inline typename uniform_int_distribution<IntType>::result_type 
	uniform_int_distribution<IntType>::operator()(Generator& g, const param_type& params) const
	{
		//TODO:3) This code is taken from Conclusions here https://www.pcg-random.org/posts/bounded-rands.html
		//TODO:3) Maybe we should use xoshiro or something like that. Tehcnically we should do like Eric Niebler and put the license on top besides the real license.txt 
		static_assert(sizeof(typename Generator::result_type) == 4, "Int distribution generator result_type should be at least 4 bytes.");
		const auto range = params.b() - params.a();
		uint32_t x = g();
		uint64_t m = uint64_t(x) * uint64_t(range);
		uint32_t l = uint32_t(m);
		if (l < range)
		{
			uint32_t t = -range;
			if (t >= range)
			{
				t -= range;
				if (t >= range)
					t %= range;
			}
			while (l < t)
			{
				x = g();
				m = uint64_t(x) * uint64_t(range);
				l = uint32_t(m);
			}
		}
		return m >> 32;
	}

	template<class IntType>
	inline typename uniform_int_distribution<IntType>::result_type
	uniform_int_distribution<IntType>::a() const
	{
		return mParam.mA;
	}

	template<class IntType>
	inline typename uniform_int_distribution<IntType>::result_type
	uniform_int_distribution<IntType>::b() const
	{
		return mParam.mB;
	}

	template<class IntType>
	inline typename uniform_int_distribution<IntType>::param_type
	uniform_int_distribution<IntType>::param() const
	{
		return mParam;
	}

	template<class IntType>
	inline void
	uniform_int_distribution<IntType>::param(const param_type& params)
	{
		mParam = params;
	}

	template<class IntType>
	inline typename uniform_int_distribution<IntType>::result_type
	uniform_int_distribution<IntType>::min() const
	{
		return mParam.mA;
	}

	template<class IntType>
	inline typename uniform_int_distribution<IntType>::result_type
	uniform_int_distribution<IntType>::max() const
	{
		return mParam.mB;
	}

	template<class ResultType>
	inline bool operator==(const uniform_int_distribution<ResultType>& lhs, const uniform_int_distribution<ResultType>& rhs)
	{
		return (lhs.param() == rhs.param());
	}

	template<class ResultType>
	inline bool operator!=(const uniform_int_distribution<ResultType>& lhs, const uniform_int_distribution<ResultType>& rhs)
	{
		return (lhs.param() != rhs.param());
	}

	// EASTL doesn't currently implement IO stream-related functionality.
	// It may be useful to forward declare these templates and let the user implement them in the meantime.
	//
	// template<class CharT, class Traits, class ResultType>
	// eastl::basic_ostream<CharT, Traits>& operator<<(eastl::basic_ostream<CharT, Traits>& os, const uniform_int_distribution& uid);
	//
	// template<class CharT, class Traits, class ResultType>
	// eastl::basic_istream<CharT, Traits>& operator>>(eastl::basic_istream<CharT, Traits>& is, uniform_int_distribution& uid);





	// Implements a uniform eral distribution of values generated by a Generator, 
	// where Generator is typically a random or pseudo-random number generator.
	// Note that the min/max range for this class is inclusive, so if you want 
	// random reals 0.0, 1.1, 1.2, and 3.0, then you need to init this class with (0.0, 3.0)
	// and not (0.0, 4.0).
	// See the C++11 Standard, section 26.5.1.6
	template<class RealType = double>
	class uniform_real_distribution
	{
		static_assert(eastl::is_floating_point_v<RealType>, "uniform_real_distribution: RealType must be floating point.");

	public:
		typedef RealType result_type;

		// For uniform_real_distribution, param_type defines simply the min and max values of
		// the range returned by operator(). It may mean something else for other distribution types.
		struct param_type
		{
			explicit param_type(RealType a = 0, RealType b = eastl::numeric_limits<RealType>::max());

			result_type a() const;
			result_type b() const;

			bool operator==(const param_type& x) { return (x.mA == mA) && (x.mB == mB); }
			bool operator!=(const param_type& x) { return (x.mA != mA) || (x.mB != mB); }

		protected:
			RealType mA;
			RealType mB;
		};

		uniform_real_distribution(RealType a = 0, RealType b = eastl::numeric_limits<RealType>::max());
		uniform_real_distribution(const param_type& params);

		void reset();

		template<class Generator>
		result_type operator()(Generator& g) const;

		template<class Generator>
		result_type operator()(Generator& g, const param_type& params) const;

		result_type a() const;
		result_type b() const;

		param_type param() const;
		void param(const param_type& params);

		result_type min() const;
		result_type max() const;

	protected:
		param_type mParam;
	};



	///////////////////////////////////////////////////////////////////////
	// uniform_real_distribution
	///////////////////////////////////////////////////////////////////////

	template<class RealType>
	inline uniform_real_distribution<RealType>::param_type::param_type(RealType aValue, RealType bValue) : mA(aValue), mB(bValue)
	{
		EASTL_ASSERT(aValue <= bValue);
	}

	template<class RealType>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::param_type::a() const
	{
		return mA;
	}

	template<class RealType>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::param_type::b() const
	{
		return mB;
	}

	template<class RealType>
	inline uniform_real_distribution<RealType>::uniform_real_distribution(RealType aValue, RealType bValue) : mParam(aValue, bValue)
	{
		// Nothing more to do.
	}

	template<class RealType>
	inline uniform_real_distribution<RealType>::uniform_real_distribution(const param_type& params) : mParam(params)
	{
		// Nothing more to do.
	}

	template<class RealType>
	void uniform_real_distribution<RealType>::reset()
	{
		// Nothing to do.
	}

	template<class RealType>
	template<class Generator>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::operator()(Generator& g) const
	{
		return operator()(g, mParam);
	}

	template<class RealType>
	template<class Generator>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::operator()(Generator& g, const param_type& params) const
	{
		static_assert(sizeof(typename Generator::result_type) == 8 , "Generator result type should be at least 8 bytes.");
		//TODO:https://prng.di.unimi.it/#remarks#Generating uniform doubles in the unit interval
		return params.a() + (g() >> 11) * 0x1.0p-53 * (params.b() - params.a());
	}

	template<class RealType>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::a() const
	{
		return mParam.mA;
	}

	template<class RealType>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::b() const
	{
		return mParam.mB;
	}

	template<class RealType>
	inline typename uniform_real_distribution<RealType>::param_type
		uniform_real_distribution<RealType>::param() const
	{
		return mParam;
	}

	template<class RealType>
	inline void
		uniform_real_distribution<RealType>::param(const param_type& params)
	{
		mParam = params;
	}

	template<class RealType>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::min() const
	{
		return mParam.mA;
	}

	template<class RealType>
	inline typename uniform_real_distribution<RealType>::result_type
		uniform_real_distribution<RealType>::max() const
	{
		return mParam.mB;
	}

	template<class ResultType>
	inline bool operator==(const uniform_real_distribution<ResultType>& lhs, const uniform_real_distribution<ResultType>& rhs)
	{
		return (lhs.param() == rhs.param());
	}

	template<class ResultType>
	inline bool operator!=(const uniform_real_distribution<ResultType>& lhs, const uniform_real_distribution<ResultType>& rhs)
	{
		return (lhs.param() != rhs.param());
	}

	// EASTL doesn't currently implement IO stream-related functionality.
	// It may be useful to forward declare these templates and let the user implement them in the meantime.
	//
	// template<class CharT, class Traits, class ResultType>
	// eastl::basic_ostream<CharT, Traits>& operator<<(eastl::basic_ostream<CharT, Traits>& os, const uniform_real_distribution& uid);
	//
	// template<class CharT, class Traits, class ResultType>
	// eastl::basic_istream<CharT, Traits>& operator>>(eastl::basic_istream<CharT, Traits>& is, uniform_real_distribution& uid);

	
	

} // namespace eastl

#include <EASTL/internal/random/discrete-distribution.hpp>

#endif // Header include guard
```

- New <EASTL/internal/random/discrete-distribution.hpp>
```c++
// Copyright snsinfu 2020.
// Distributed under the Boost Software License, Version 1.0.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef INCLUDED_SNSINFU_DISCRETE_DISTRIBUTION_HPP
#define INCLUDED_SNSINFU_DISCRETE_DISTRIBUTION_HPP

// This is a single header-only library for eastl and later, providing:
//
// - class eastl::discrete_weights
//   Holds probability weights of a disctete distribution.
//
// - class eastl::discrete_distribution
//   A random number distribution of integers with given weights. This class
//   allows efficient modification of the weights.
//
// See: https://github.com/snsinfu/cxx-distr/


//TODO:I wonder what's algorithm is for since it's already in eastl vector...
#include <EASTL/algorithm.h>
//#include <cstddef>
#include <EASTL/initializer_list.h>
//#include <istream>
//#include <ostream>
#include <EASTL/random.h>
#include <EASTL/vector.h>

namespace eastl
{
    // WEIGHTS ---------------------------------------------------------------

    /*
     * Class holding the weights of a discrete distribution.
     * It allowsefficient reweighting and searching.
     */
    class discrete_weights
    {
    public:

        using pointer = double const*;
        using iterator = double const*;


        //Default constructor creates an empty object.
        discrete_weights() = default;

        /*
         * Sets weight values from a vector.
         *
         * Params:
         *   weights = Weight values. The weights must be non-negative finite numbers.
         *
         * Time complexity:
         *   O(N) where N is the number of events (= `weights.size()`).
         */
        explicit discrete_weights(eastl::vector<double> const& weights)
        {
            // Construct a complete binary tree in which the leaves contain
            // the weights and internal nodes contain the sums of the weights
            // of children.

            // Determine the number of leaves.
            std::size_t leaves = 1;

            for (;;)
            {
                if (leaves >= weights.size())
                    break;
                leaves *= 2;
            }

            // We store the binary tree as an array using the usual scheme:
            //
            //   root = 0 ,
            //   parent(node) = (node - 1) / 2 .
            //
            _sumtree.resize(leaves * 2 - 1);
            _leaves = leaves;
            _events = weights.size();
            EASTL_ASSERT(_leaves >= _events);

            // Fill the leaves.
            for (std::size_t i = 0; i < _events; i++)
                _sumtree[_leaves + i - 1] = weights[i];

            // Then, fill internal nodes from leaves to the root.
            // Recall that each node contains the sum of the weights of its children.
            for (std::size_t layer = 1; ; layer++)
            {
                auto const layer_size = leaves >> layer;
                if (layer_size == 0)
                    break;

                auto const start = layer_size - 1;
                auto const end = start + layer_size;

                EASTL_ASSERT(end < _leaves);
                EASTL_ASSERT(start < end);

                for (std::size_t node = start; node < end; node++)
                {
                    auto const lchild = 2 * node + 1;
                    auto const rchild = 2 * node + 2;
                    _sumtree[node] = _sumtree[lchild] + _sumtree[rchild];
                }
            }
        }


        /*
         * Sets weight values from an initializer list.
         *
         * Params:
         *   weights = Weight values. The weights must be non-negative finite numbers.
         */
        discrete_weights(std::initializer_list<double> const& weights): discrete_weights{ eastl::vector<double>{weights} }
        {
        }

        //Returns the number of events.
        inline std::size_t size() const noexcept
        {
            return _events;
        }

        //Returns a pointer to the array containing weight values.
        inline pointer data() const noexcept
        {
            return _sumtree.data() + _leaves - 1;
        }

        //Returns an iterator pointing to the beginning of the array containing weight values.
        inline iterator begin() const noexcept
        {
            return data();
        }

        //Returns an iterator pointing to the past the end of the array containing weight values.
        inline iterator end() const noexcept
        {
            return data() + size();
        }

        //Returns the weight of the i-th event.
        inline double operator[](std::size_t i) const
        {
            return *(data() + i);
        }

        /*
         * Returns the sum of the weights.
         *
         * Time complexity:
         *   O(1).
         */
        inline double sum() const
        {
            return _sumtree[0];
        }

        /*
         * Updates the weight of the i-th event.
         *
         * Behavior is undefined if `i` is out of range or `weight` is
         * negative or not finite. It is also undefined that the sum of
         * weights overflow due to the update.
         *
         * Params:
         *   i      = Index of the event to update weight.
         *   weight = New weight value.
         *
         * Time complexity:
         *   O(log N) where N is the number of events.
         */
        void update(std::size_t i, double weight)
        {
            EASTL_ASSERT(i < _events);
            EASTL_ASSERT(weight >= 0);

            auto node = _leaves + i - 1;
            _sumtree[node] = weight;

            do {
                node = (node - 1) / 2;
                auto const lchild = 2 * node + 1;
                auto const rchild = 2 * node + 2;
                _sumtree[node] = _sumtree[lchild] + _sumtree[rchild];
            } while (node > 0);
        }

        /*
         * Finds the event whose cumulative weight interval covers given probevalue.
         *
         * Let `w[i]` be the weight of the i-th event, and let `s[i]` be the
         * cumulative sum until the i-th event. That is,
         *
         *    s[0] = 0 ,
         *    s[i] = w[0] + ... + w[i-1] .
         *
         * Then, this function finds the event i such that
         *
         *    s[i] <= probe < s[i+1] .
         *
         * Params:
         *   probe = Probe weight used to find an event.
         *
         * Returns:
         *   The index of the event found.
         *
         * Time complexity:
         *   O(log N) where N is the number of events.
         */
        std::size_t find(double probe) const
        {
            std::size_t node = 0;

            for (;;)
            {
                auto const lchild = 2 * node + 1;
                auto const rchild = 2 * node + 2;

                if (lchild >= _sumtree.size())
                    break;

                if (probe < _sumtree[lchild])
                    node = lchild;
                else
                {
                    probe -= _sumtree[lchild];
                    node = rchild;
                }
            }

            EASTL_ASSERT(node >= _leaves - 1);
            EASTL_ASSERT(node < _sumtree.size());
            auto index = node - (_leaves - 1);

            // Search may overshoot due to numerical errors.
            if (index >= _events)
                index = _events - 1;

            return index;
        }
    private:
        eastl::vector<double> _sumtree;
        std::size_t _leaves = 0;
        std::size_t _events = 0;
    };


    inline bool operator==(eastl::discrete_weights const& w1, eastl::discrete_weights const& w2)
    {
        if (w1.size() != w2.size())
            return false;

        return eastl::equal(w1.begin(), w1.end(), w2.begin());
    }

    inline bool operator!=(eastl::discrete_weights const& w1, eastl::discrete_weights const& w2)
    {
        return !(w1 == w2);
    }

    //EASTL doesn't currently implement IO stream-related functionality.
    //template<typename Char, typename Tr>
    //std::basic_istream<Char, Tr>& operator>>(std::basic_istream<Char, Tr>& is, eastl::discrete_weights& weights)
    //{
    //    using sentry_type = typename std::basic_istream<Char, Tr>::sentry;
    //
    //    if (sentry_type sentry{ is })
    //    {
    //        std::size_t size;
    //        if (!(is >> size))
    //            return is;
    //
    //        eastl::vector<double> values(size);
    //        for (auto& value : values)
    //            if (!(is >> value))
    //                return is;
    //
    //        weights = eastl::discrete_weights{ values };
    //    }
    //
    //    return is;
    //}
    //
    //template<typename Char, typename Tr>
    //std::basic_ostream<Char, Tr>& operator<<( std::basic_ostream<Char, Tr>& os, eastl::discrete_weights const& weights)
    //{
    //    using sentry_type = typename std::basic_ostream<Char, Tr>::sentry;
    //
    //    if (sentry_type sentry{ os })
    //    {
    //        os << weights.size();
    //
    //        auto const sep = os.widen(' ');
    //
    //        for (auto weight : weights)
    //        {
    //            os << sep;
    //            os << weight;
    //        }
    //    }
    //
    //    return os;
    //}


    // DISTRIBUTION ----------------------------------------------------------

    //Distribution of random integers with given weights.
    template<typename T = int>
    class discrete_distribution
    {
    public:

        //Type of generated integer.
        using result_type = T;


        //Class holding distribution parameter set, namely, the weights.
        class param_type : public eastl::discrete_weights
        {
        public:

            using distribution_type = eastl::discrete_distribution<T>;

            // Inherit constructors from discrete_weights.
            using eastl::discrete_weights::discrete_weights;

            // Allow conversion from discrete_weights.
            param_type(eastl::discrete_weights const& weights) : discrete_weights{ weights }
            {
            }
        };


        //Default constructor creates an empty distribution.
        discrete_distribution() = default;


        /*
         * Creates a discrete distribution over `[0, N)` with given weights
         * where `N = weights.size()`.
         *
         * Params:
         *   weights = Weight values. The weights must be non-negative finite numbers.
         */
        explicit discrete_distribution(eastl::vector<double> const& weights) : _weights{ weights }
        {
        }

        /*
         * Creates a discrete distribution with given weights.
         *
         * Params:
         *   weights = Weight values. The weights must be non-negative finite numbers.
         */
        discrete_distribution(std::initializer_list<double> const& weights) : _weights{ weights }
        {
        }

        /*
         * Creates a discrete distribution with given parameter set.
         *
         * Params:
         *   param = A `eastl::discrete_weights` object.
         */
        explicit discrete_distribution(param_type const& param) : _weights{ param }
        {
        }

        //Resets the distribution state. This function does nothing.
        void reset()
        {
        }

        //Returns the parameter set of this distribution. It is convertible to `eastl::discrete_weights`.
        param_type const& param() const noexcept
        {
            return _weights;
        }

        //Reconfigures the distribution using given parameter set.
        //It is convertible from `eastl::discrete_weights`.
        void param(param_type const& new_param)
        {
            _weights = new_param;
        }

        //Returns the minimum possible integer generated from this distribution, namely, zero.
        result_type min() const
        {
            return 0;
        }

        //Returns the maximum possible integer generated from this distribution.
        result_type max() const
        {
            return result_type(_weights.size() - 1);
        }

        /*
         * Returns the sum of the weights.
         *
         * Time complexity:
         *   O(1).
         */
        double sum() const
        {
            return _weights.sum();
        }

        /*
         * Updates the weight of the number `i`.
         *
         * Params:
         *   i      = Number to change weight. Must be in the valid interval `[min(), max()]`.
         *   weight = New weight. Must be non-negative finite number.
         *
         * Time complexity:
         *   O(log N) where N is the upper bound.
         */
        void update(result_type i, double weight)
        {
            return _weights.update(std::size_t(i), weight);
        }

        /*
         * Generates an integer randomly from the weighted distribution.
         *
         * Params:
         *   random = Random number generator to use.
         *
         * Time complexity:
         *   O(log N) where N is the upper bound.
         */
        template<typename RNG>
        result_type operator()(RNG& random) const
        {
            eastl::uniform_real_distribution<double> uniform{ 0.0, _weights.sum() };
            return result_type(_weights.find(uniform(random)));
        }


    private:
        param_type _weights;
    };


    template<typename T>
    inline bool operator==(eastl::discrete_distribution<T> const& d1, eastl::discrete_distribution<T> const& d2)
    {
        return d1.param() == d2.param();
    }


    template<typename T>
    inline bool operator!=( eastl::discrete_distribution<T> const& d1, eastl::discrete_distribution<T> const& d2)
    {
        return !(d1 == d2);
    }

    // EASTL doesn't currently implement IO stream-related functionality.
    //template<typename Char, typename Tr, typename T>
    //std::basic_istream<Char, Tr>& operator>>(std::basic_istream<Char, Tr>& is, eastl::discrete_distribution<T>& distr)
    //{
    //    using param_type = typename eastl::discrete_distribution<T>::param_type;
    //    param_type param;
    //    is >> param;
    //    distr.param(param);
    //    return is;
    //}
    //
    //
    //template<typename Char, typename Tr, typename T>
    //std::basic_ostream<Char, Tr>&  operator<<(std::basic_ostream<Char, Tr>& os, eastl::discrete_distribution<T> const& distr)
    //{
    //    return os << distr.param();
    //}
}

#endif
```