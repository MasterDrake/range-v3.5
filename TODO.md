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
  - Sostituire EARANGES_ASSERT con EASTL_ASSERT o EASTL_ASSERT_MSG o EASTL_CT_ASSERT
  - Contorllare se ranges ha un abort o un fail e usare EASTL_FAIL_MSG
  - Sostituire tutte le chiamate a throw e exceptions con EASTL_EXCEPTIONS_ENABLED
  #if EASTL_EXCEPTIONS_ENABLED
	#include <stdexcept> // std::out_of_range, std::length_error.
#endif
  - Controllare se serve in config o dove altro, usare EASTL_CPP17_INLINE_VARIABLE invece di chissà che diavoleria
  - Creare un namespace per EASTL_SIZE_T e usare quello invece che std::size_t, ma occhio a uintptr_t che mi sembra unsigned al contrario di EASTL_SSIZE_T
  - Rimuovere tutte le reference a std usando grepWin e la seguente regex.
  - \bstd\b(?!(?:::cout|::initializer_list|::size_t|::uintmax_t|::stringstream|::ptrdiff_t|::nullptr_t|::uint32_t|::intmax_t|::boolalpha|::cerr)\b)
  - ^(out|doc|extra|perf|include\EARanges\experimental)$
  - *.cpp|*.hpp

 ## STRING
 - For ranges, eastl::string does not conform the 'container' concept because it can't be initialized with an iterator Pair.
 - To solve this we need to add another constructor:
 ```c++
	   template<typename Iterator>
        basic_string(Iterator first, Iterator last, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR): mPair(allocator)
		{
			RangeInitialize(first, last);
		}
 ```

 ## ADDRESSOF CONSTEXPR
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
	template<typename T>
    constexpr T * addressof(T & value) EA_NOEXCEPT
	{
        if constexpr(true)
            return __builtin_addressof(value);
		else
			return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(value)));
	}

} // namespace eastl

#endif // EASTL_INTERNAL_MEMORY_BASE_H
```

# to_string()
- To use to_string, you need to implement vsprintf or use EA::StdC::Vsprintf
- or define it yourself like this:
```c++
int __cdecl EA::StdC::Vsnprintf(char * __restrict pDestination, unsigned __int64 n, char const * __restrict pFormat, char * arguments)
{
    return vsnprintf(pDestination, n, pFormat, arguments);
}
```
- needs #include cstdio and apparently it's not very portable.

# TUPLE CONSTEXPR
- Apparently the other side of the problem is that tuple and everything about it must be constexpr as well.
- Yes, that's the case. Infact now stuff works.
- So the changes needed are: add constexpr to some constructors and get member functions.
- The changes are in two files, obviously: tuple and tuple_fwd_decls.h

- tuple_fwd_decls.h
- The changes are the added EA_CONSTEXPR in the get methods.
```c++
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLE_FWD_DECLS_H
#define EASTL_TUPLE_FWD_DECLS_H

#include <EASTL/internal/config.h>

#if EASTL_TUPLE_ENABLED

namespace eastl
{
	template <typename... T>
	class tuple;

	template <typename Tuple>
	class tuple_size;

	template <size_t I, typename Tuple>
	class tuple_element;

	template <size_t I, typename Tuple>
	using tuple_element_t = typename tuple_element<I, Tuple>::type;

	// const typename for tuple_element_t, for when tuple or TupleImpl cannot itself be const
	template <size_t I, typename Tuple>
	using const_tuple_element_t = typename conditional<
						is_lvalue_reference<tuple_element_t<I, Tuple>>::value,
							 add_lvalue_reference_t<const remove_reference_t<tuple_element_t<I, Tuple>>>,
							 const tuple_element_t<I, Tuple>
						>::type;

	// get
	template <size_t I, typename... Ts_>
    EA_CONSTEXPR tuple_element_t<I, tuple<Ts_...>> & get(tuple<Ts_...> & t);

	template <size_t I, typename... Ts_>
    EA_CONSTEXPR const_tuple_element_t<I, tuple<Ts_...>> & get(const tuple<Ts_...> & t);

	template <size_t I, typename... Ts_>
    EA_CONSTEXPR tuple_element_t<I, tuple<Ts_...>> && get(tuple<Ts_...> && t);

	template <typename T, typename... ts_>
    EA_CONSTEXPR T & get(tuple<ts_...> & t);

	template <typename T, typename... ts_>
    EA_CONSTEXPR const T & get(const tuple<ts_...> & t);

	template <typename T, typename... ts_>
    EA_CONSTEXPR T && get(tuple<ts_...> && t);
}

#endif  // EASTL_VARIADIC_TEMPLATES_ENABLED

#endif  // EASTL_TUPLE_FWD_DECLS_H

```

- tuple.h is big enough
```c++
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLE_H
#define EASTL_TUPLE_H

#include <EASTL/internal/config.h>
#include <EASTL/compare.h>
#include <EASTL/functional.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EASTL/internal/tuple_fwd_decls.h>

EA_DISABLE_VC_WARNING(4623) // warning C4623: default constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4625) // warning C4625: copy constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4510) // warning C4510: default constructor could not be generated

#if EASTL_TUPLE_ENABLED

namespace eastl
{
// non-recursive tuple implementation based on libc++ tuple implementation and description at
// http://mitchnull.blogspot.ca/2012/06/c11-tuple-implementation-details-part-1.html

// TupleTypes helper
template <typename... Ts> struct TupleTypes {};

// tuple_size helper
template <typename T> class tuple_size {};
template <typename T> class tuple_size<const T>          : public tuple_size<T> {};
template <typename T> class tuple_size<volatile T>       : public tuple_size<T> {};
template <typename T> class tuple_size<const volatile T> : public tuple_size<T> {};

template <typename... Ts> class tuple_size<TupleTypes<Ts...>> : public integral_constant<size_t, sizeof...(Ts)> {};
template <typename... Ts> class tuple_size<tuple<Ts...>>      : public integral_constant<size_t, sizeof...(Ts)> {};

#if EASTL_VARIABLE_TEMPLATES_ENABLED
	template <class T>
	EA_CONSTEXPR size_t tuple_size_v = tuple_size<T>::value;
#endif

namespace Internal
{
	template <typename TupleIndices, typename... Ts>
	struct TupleImpl;
} // namespace Internal

template <typename Indices, typename... Ts>
class tuple_size<Internal::TupleImpl<Indices, Ts...>> : public integral_constant<size_t, sizeof...(Ts)>
{
};

// tuple_element helper to be able to isolate a type given an index
template <size_t I, typename T>
class tuple_element
{
};

template <size_t I>
class tuple_element<I, TupleTypes<>>
{
public:
	static_assert(I != I, "tuple_element index out of range");
};

template <typename H, typename... Ts>
class tuple_element<0, TupleTypes<H, Ts...>>
{
public:
	typedef H type;
};

template <size_t I, typename H, typename... Ts>
class tuple_element<I, TupleTypes<H, Ts...>>
{
public:
	typedef tuple_element_t<I - 1, TupleTypes<Ts...>> type;
};

// specialization for tuple
template <size_t I, typename... Ts>
class tuple_element<I, tuple<Ts...>>
{
public:
	typedef tuple_element_t<I, TupleTypes<Ts...>> type;
};

template <size_t I, typename... Ts>
class tuple_element<I, const tuple<Ts...>>
{
public:
	typedef typename add_const<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

template <size_t I, typename... Ts>
class tuple_element<I, volatile tuple<Ts...>>
{
public:
	typedef typename add_volatile<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

template <size_t I, typename... Ts>
class tuple_element<I, const volatile tuple<Ts...>>
{
public:
	typedef typename add_cv<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

// specialization for TupleImpl
template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, const Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, const tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, volatile Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, volatile tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, const volatile Internal::TupleImpl<Indices, Ts...>> : public tuple_element<
																				 I, const volatile tuple<Ts...>>
{
};

// attempt to isolate index given a type
template <typename T, typename Tuple>
struct tuple_index
{
};

template <typename T>
struct tuple_index<T, TupleTypes<>>
{
	typedef void DuplicateTypeCheck;
	tuple_index() = delete; // tuple_index should only be used for compile-time assistance, and never be instantiated
	static const size_t index = 0;
};

template <typename T, typename... TsRest>
struct tuple_index<T, TupleTypes<T, TsRest...>>
{
	typedef int DuplicateTypeCheck;
	// after finding type T in the list of types, try to find type T in TsRest.
	// If we stumble back into this version of tuple_index, i.e. type T appears twice in the list of types, then DuplicateTypeCheck will be of type int, and the static_assert will fail.
	// If we don't, then we'll go through the version of tuple_index above, where all of the types have been exhausted, and DuplicateTypeCheck will be void.
	static_assert(is_void<typename tuple_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<size_t>()");

	static const size_t index = 0;
};

template <typename T, typename TsHead, typename... TsRest>
struct tuple_index<T, TupleTypes<TsHead, TsRest...>>
{
	typedef typename tuple_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck DuplicateTypeCheck;
	static const size_t index = tuple_index<T, TupleTypes<TsRest...>>::index + 1;
};

template <typename T, typename Indices, typename... Ts>
struct tuple_index<T, Internal::TupleImpl<Indices, Ts...>> : public tuple_index<T, TupleTypes<Ts...>>
{
};


namespace Internal
{
	// swallow
	//
	// Provides a vessel to expand variadic packs.
	//
	template <typename... Ts>
	void swallow(Ts&&...) {}


	// TupleLeaf
	//
	template <size_t I, typename ValueType, bool IsEmpty = is_empty_v<ValueType>>
	class TupleLeaf;

	template <size_t I, typename ValueType, bool IsEmpty>
	inline void swap(TupleLeaf<I, ValueType, IsEmpty>& a, TupleLeaf<I, ValueType, IsEmpty>& b)
	{
		eastl::swap(a.getInternal(), b.getInternal());
	}

	template <size_t I, typename ValueType, bool IsEmpty>
	class TupleLeaf
	{
	public:
		EA_CONSTEXPR TupleLeaf() : mValue() {}
		EA_CONSTEXPR TupleLeaf(const TupleLeaf&) = default;
		EA_CONSTEXPR TupleLeaf& operator=(const TupleLeaf&) = delete;

		// We shouldn't need this explicit constructor as it should be handled by the template below but OSX clang
		// is_constructible type trait incorrectly gives false for is_constructible<T&&, T&&>::value
        EA_CONSTEXPR explicit TupleLeaf(ValueType && v)
          : mValue(eastl::forward<ValueType>(v))
        {}

		template <typename T, typename = typename enable_if<is_constructible<ValueType, T&&>::value>::type>
        EA_CONSTEXPR explicit TupleLeaf(T && t)
			: mValue(eastl::forward<T>(t))
		{
		}

		template <typename T>
        EA_CONSTEXPR explicit TupleLeaf(const TupleLeaf<I, T> & t)
			: mValue(t.getInternal())
		{
		}

		template <typename T>
        EA_CONSTEXPR TupleLeaf & operator=(T && t)
		{
			mValue = eastl::forward<T>(t);
			return *this;
		}

		int swap(TupleLeaf& t)
		{
			eastl::Internal::swap(*this, t);
			return 0;
		}

		ValueType& getInternal() { return mValue; }
        EA_CONSTEXPR const ValueType & getInternal() const
        {
            return mValue;
        }

	private:
		ValueType mValue;  
	};

	// TupleLeaf: partial specialization for when we can use the Empty Base Class Optimization
	template <size_t I, typename ValueType>
	class TupleLeaf<I, ValueType, true> : private ValueType
	{
	public:
		// true_type / false_type constructors for case where ValueType is default constructible and should be value
		// initialized and case where it is not
		TupleLeaf(const TupleLeaf&) = default;

		template <typename T, typename = typename enable_if<is_constructible<ValueType, T&&>::value>::type>
		explicit TupleLeaf(T&& t)
			: ValueType(eastl::forward<T>(t))
		{
		}

		template <typename T>
		explicit TupleLeaf(const TupleLeaf<I, T>& t)
			: ValueType(t.getInternal())
		{
		}

		template <typename T>
		TupleLeaf& operator=(T&& t)
		{
			ValueType::operator=(eastl::forward<T>(t));
			return *this;
		}

		int swap(TupleLeaf& t)
		{
			eastl::Internal::swap(*this, t);
			return 0;
		}

		ValueType& getInternal() { return static_cast<ValueType&>(*this); }
		const ValueType& getInternal() const { return static_cast<const ValueType&>(*this); }

	private:
		TupleLeaf& operator=(const TupleLeaf&) = delete;
	};



	// MakeTupleTypes
	//
	//
	template <typename TupleTypes, typename Tuple, size_t Start, size_t End>
	struct MakeTupleTypesImpl;

	template <typename... Types, typename Tuple, size_t Start, size_t End>
	struct MakeTupleTypesImpl<TupleTypes<Types...>, Tuple, Start, End>
	{
		typedef typename remove_reference<Tuple>::type TupleType;
		typedef typename MakeTupleTypesImpl<
			TupleTypes<Types..., typename conditional<is_lvalue_reference<Tuple>::value,
													  // append ref if Tuple is ref
													  tuple_element_t<Start, TupleType>&,
													  // append non-ref otherwise
													  tuple_element_t<Start, TupleType>>::type>,
			Tuple, Start + 1, End>::type type;
	};

	template <typename... Types, typename Tuple, size_t End>
	struct MakeTupleTypesImpl<TupleTypes<Types...>, Tuple, End, End>
	{
		typedef TupleTypes<Types...> type;
	};

	template <typename Tuple>
	using MakeTupleTypes_t = typename MakeTupleTypesImpl<TupleTypes<>, Tuple, 0,
														 tuple_size<typename remove_reference<Tuple>::type>::value>::type;


	// TupleImpl
	//
	//
	template <size_t I, typename Indices, typename... Ts>
	EA_CONSTEXPR tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t);

	template <size_t I, typename Indices, typename... Ts>
	EA_CONSTEXPR const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t);

	template <size_t I, typename Indices, typename... Ts>
	EA_CONSTEXPR tuple_element_t<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t);

	template <typename T, typename Indices, typename... Ts>
    EA_CONSTEXPR T & get(TupleImpl<Indices, Ts...> & t);

	template <typename T, typename Indices, typename... Ts>
    EA_CONSTEXPR const T & get(const TupleImpl<Indices, Ts...> & t);

	template <typename T, typename Indices, typename... Ts>
    EA_CONSTEXPR T && get(TupleImpl<Indices, Ts...> && t);

	template <size_t... Indices, typename... Ts>
	struct TupleImpl<integer_sequence<size_t, Indices...>, Ts...> : public TupleLeaf<Indices, Ts>...
	{
		EA_CONSTEXPR TupleImpl() = default;

		// index_sequence changed to integer_sequence due to issues described below in VS2015 CTP 6. 
		// https://connect.microsoft.com/VisualStudio/feedback/details/1126958/error-in-template-parameter-pack-expansion-of-std-index-sequence
		// 
		template <typename... Us, typename... ValueTypes>
        EA_CONSTEXPR explicit TupleImpl(integer_sequence<size_t, Indices...>,
                                       TupleTypes<Us...>, ValueTypes &&... values)
			: TupleLeaf<Indices, Ts>(eastl::forward<ValueTypes>(values))...
		{
		}

		template <typename OtherTuple>
        EA_CONSTEXPR TupleImpl(OtherTuple && t)
			: TupleLeaf<Indices, Ts>(eastl::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...
		{
		}

		template <typename OtherTuple>
        EA_CONSTEXPR TupleImpl & operator=(OtherTuple && t)
		{
			swallow(TupleLeaf<Indices, Ts>::operator=(
						eastl::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...);
			return *this;
		}

		EA_CONSTEXPR TupleImpl & operator=(const TupleImpl & t)
		{
			swallow(TupleLeaf<Indices, Ts>::operator=(static_cast<const TupleLeaf<Indices, Ts>&>(t).getInternal())...);
			return *this;
		}

		void swap(TupleImpl& t) { swallow(TupleLeaf<Indices, Ts>::swap(static_cast<TupleLeaf<Indices, Ts>&>(t))...); }
	};

	template <size_t I, typename Indices, typename... Ts>
	EA_CONSTEXPR inline tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t)
	{
		typedef tuple_element_t<I, TupleImpl<Indices, Ts...>> Type;
		return static_cast<Internal::TupleLeaf<I, Type>&>(t).getInternal();
	}

	template <size_t I, typename Indices, typename... Ts>
	EA_CONSTEXPR inline const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t)
	{
		typedef tuple_element_t<I, TupleImpl<Indices, Ts...>> Type;
		return static_cast<const Internal::TupleLeaf<I, Type>&>(t).getInternal();
	}

	template <size_t I, typename Indices, typename... Ts>
	EA_CONSTEXPR inline tuple_element_t<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t)
	{
		typedef tuple_element_t<I, TupleImpl<Indices, Ts...>> Type;
		return static_cast<Type&&>(static_cast<Internal::TupleLeaf<I, Type>&>(t).getInternal());
	}

	template <typename T, typename Indices, typename... Ts>
	EA_CONSTEXPR inline T& get(TupleImpl<Indices, Ts...>& t)
	{
		typedef tuple_index<T, TupleImpl<Indices, Ts...>> Index;
		return static_cast<Internal::TupleLeaf<Index::index, T>&>(t).getInternal();
	}

	template <typename T, typename Indices, typename... Ts>
    EA_CONSTEXPR inline const T & get(const TupleImpl<Indices, Ts...> & t)
	{
		typedef tuple_index<T, TupleImpl<Indices, Ts...>> Index;
		return static_cast<const Internal::TupleLeaf<Index::index, T>&>(t).getInternal();
	}

	template <typename T, typename Indices, typename... Ts>
    EA_CONSTEXPR inline T && get(TupleImpl<Indices, Ts...> && t)
	{
		typedef tuple_index<T, TupleImpl<Indices, Ts...>> Index;
		return static_cast<T&&>(static_cast<Internal::TupleLeaf<Index::index, T>&>(t).getInternal());
	}


	// TupleLike
	//
	// type-trait that determines if a type is an eastl::tuple or an eastl::pair.
	//
	template <typename T> struct TupleLike                   : public false_type {};
	template <typename T> struct TupleLike<const T>          : public TupleLike<T> {};
	template <typename T> struct TupleLike<volatile T>       : public TupleLike<T> {};
	template <typename T> struct TupleLike<const volatile T> : public TupleLike<T> {};

	template <typename... Ts>
	struct TupleLike<tuple<Ts...>> : public true_type {};

	template <typename First, typename Second>
	struct TupleLike<eastl::pair<First, Second>> : public true_type {};


	// TupleConvertible
	//
	//
	//
	template <bool IsSameSize, typename From, typename To>
	struct TupleConvertibleImpl : public false_type
	{
	};

	template <typename... FromTypes, typename... ToTypes>
	struct TupleConvertibleImpl<true, TupleTypes<FromTypes...>,	TupleTypes<ToTypes...>>
		: public integral_constant<bool, conjunction<is_convertible<FromTypes, ToTypes>...>::value>
	{
	};

	template <typename From, typename To,
			  bool = TupleLike<typename remove_reference<From>::type>::value,
			  bool = TupleLike<typename remove_reference<To>::type>::value>
	struct TupleConvertible : public false_type
	{
	};

	template <typename From, typename To>
	struct TupleConvertible<From, To, true, true>
		: public TupleConvertibleImpl<tuple_size<typename remove_reference<From>::type>::value ==
				tuple_size<typename remove_reference<To>::type>::value,
				MakeTupleTypes_t<From>, MakeTupleTypes_t<To>>
	{
	};


	// TupleAssignable
	//
	//
	//
	template <bool IsSameSize, typename Target, typename From>
	struct TupleAssignableImpl : public false_type
	{
	};

	template <typename... TargetTypes, typename... FromTypes>
	struct TupleAssignableImpl<true, TupleTypes<TargetTypes...>, TupleTypes<FromTypes...>>
		: public bool_constant<conjunction<is_assignable<TargetTypes, FromTypes>...>::value>
	{
	};

	template <typename Target, typename From,
			  bool = TupleLike<typename remove_reference<Target>::type>::value,
			  bool = TupleLike<typename remove_reference<From>::type>::value>
	struct TupleAssignable : public false_type
	{
	};

	template <typename Target, typename From>
	struct TupleAssignable<Target, From, true, true>
		: public TupleAssignableImpl<
			tuple_size<typename remove_reference<Target>::type>::value ==
			tuple_size<typename remove_reference<From>::type>::value,
			MakeTupleTypes_t<Target>, MakeTupleTypes_t<From>>
	{
	};


	// TupleImplicitlyConvertible and TupleExplicitlyConvertible 
	//
	// helpers for constraining conditionally-explicit ctors
	//
	template <bool IsSameSize, typename TargetType, typename... FromTypes>
	struct TupleImplicitlyConvertibleImpl : public false_type
	{
	};


	template <typename... TargetTypes, typename... FromTypes>
	struct TupleImplicitlyConvertibleImpl<true, TupleTypes<TargetTypes...>, FromTypes...>
		: public conjunction<
		is_constructible<TargetTypes, FromTypes>...,
		is_convertible<FromTypes, TargetTypes>...>
	{
	};

	template <typename TargetTupleType, typename... FromTypes>
	struct TupleImplicitlyConvertible
		: public TupleImplicitlyConvertibleImpl<
		tuple_size<TargetTupleType>::value == sizeof...(FromTypes),
		MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
	{
	};

	template<typename TargetTupleType, typename... FromTypes>
	using TupleImplicitlyConvertible_t = enable_if_t<TupleImplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;

	template <bool IsSameSize, typename TargetType, typename... FromTypes>
	struct TupleExplicitlyConvertibleImpl : public false_type
	{
	};

	template <typename... TargetTypes, typename... FromTypes>
	struct TupleExplicitlyConvertibleImpl<true, TupleTypes<TargetTypes...>, FromTypes...>
		: public conjunction<
			is_constructible<TargetTypes, FromTypes>...,
			negation<conjunction<is_convertible<FromTypes, TargetTypes>...>>>
	{
	};

	template <typename TargetTupleType, typename... FromTypes>
	struct TupleExplicitlyConvertible
		: public TupleExplicitlyConvertibleImpl<
		tuple_size<TargetTupleType>::value == sizeof...(FromTypes),
		MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
	{
	};

	template<typename TargetTupleType, typename... FromTypes>
	using TupleExplicitlyConvertible_t = enable_if_t<TupleExplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;


	// TupleEqual
	//
	//
	//
	template <size_t I>
	struct TupleEqual
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1& t1, const Tuple2& t2)
		{
			static_assert(tuple_size<Tuple1>::value == tuple_size<Tuple2>::value, "comparing tuples of different sizes.");
			return TupleEqual<I - 1>()(t1, t2) && get<I - 1>(t1) == get<I - 1>(t2);
		}
	};

	template <>
	struct TupleEqual<0>
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1&, const Tuple2&)
		{
			return true;
		}
	};

	// TupleLess
	//
	//
	//
	template <size_t I>
	struct TupleLess
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1& t1, const Tuple2& t2)
		{
			static_assert(tuple_size<Tuple1>::value == tuple_size<Tuple2>::value, "comparing tuples of different sizes.");
			return TupleLess<I - 1>()(t1, t2) || (!TupleLess<I - 1>()(t2, t1) && get<I - 1>(t1) < get<I - 1>(t2));
		}
	};

	template <>
	struct TupleLess<0>
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1&, const Tuple2&)
		{
			return false;
		}
	};


	// MakeTupleReturnImpl
	//
	//
	//
	template <typename T> struct MakeTupleReturnImpl                       { typedef T type; };
	template <typename T> struct MakeTupleReturnImpl<reference_wrapper<T>> { typedef T& type; };

	template <typename T>
	using MakeTupleReturn_t = typename MakeTupleReturnImpl<decay_t<T>>::type;


	// tuple_cat helpers
	//
	//
	//

	// TupleCat2Impl
	template <typename Tuple1, typename Is1, typename Tuple2, typename Is2>
	struct TupleCat2Impl;

	template <typename... T1s, size_t... I1s, typename... T2s, size_t... I2s>
	struct TupleCat2Impl<tuple<T1s...>, index_sequence<I1s...>, tuple<T2s...>, index_sequence<I2s...>>
	{
		using ResultType = tuple<T1s..., T2s...>;

		template <typename Tuple1, typename Tuple2>
		static inline ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
		{
			return ResultType(get<I1s>(eastl::forward<Tuple1>(t1))..., get<I2s>(eastl::forward<Tuple2>(t2))...);
		}
	};

	// TupleCat2
	template <typename Tuple1, typename Tuple2>
	struct TupleCat2;

	template <typename... T1s, typename... T2s>
	struct TupleCat2<tuple<T1s...>, tuple<T2s...>>
	{
		using Is1        = make_index_sequence<sizeof...(T1s)>;
		using Is2        = make_index_sequence<sizeof...(T2s)>;
		using TCI        = TupleCat2Impl<tuple<T1s...>, Is1, tuple<T2s...>, Is2>;
		using ResultType = typename TCI::ResultType;

		template <typename Tuple1, typename Tuple2>
		static inline ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
		{
			return TCI::DoCat2(eastl::forward<Tuple1>(t1), eastl::forward<Tuple2>(t2));
		}
	};

	// TupleCat
	template <typename... Tuples>
	struct TupleCat;

	template <typename Tuple1, typename Tuple2, typename... TuplesRest>
	struct TupleCat<Tuple1, Tuple2, TuplesRest...>
	{
		using FirstResultType = typename TupleCat2<Tuple1, Tuple2>::ResultType;
		using ResultType      = typename TupleCat<FirstResultType, TuplesRest...>::ResultType;

		template <typename TupleArg1, typename TupleArg2, typename... TupleArgsRest>
		static inline ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2, TupleArgsRest&&... ts)
		{
			return TupleCat<FirstResultType, TuplesRest...>::DoCat(
				TupleCat2<TupleArg1, TupleArg2>::DoCat2(eastl::forward<TupleArg1>(t1), eastl::forward<TupleArg2>(t2)),
				eastl::forward<TupleArgsRest>(ts)...);
		}
	};

	template <typename Tuple1, typename Tuple2>
	struct TupleCat<Tuple1, Tuple2>
	{
		using TC2 = TupleCat2<Tuple1, remove_reference_t<Tuple2>>;
		using ResultType = typename TC2::ResultType;

		template <typename TupleArg1, typename TupleArg2>
		static inline ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2)
		{
			return TC2::DoCat2(eastl::forward<TupleArg1>(t1), eastl::forward<TupleArg2>(t2));
		}
	};

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <typename... T1s, typename... T2s, size_t... Is>
	constexpr auto TupleThreeWay(const tuple<T1s...>& t1, const tuple<T2s...>& t2, index_sequence<Is...> is)
	{
		std::common_comparison_category_t<synth_three_way_result<T1s, T2s>...> result = std::strong_ordering::equal;
		((result = synth_three_way{}(get<Is>(t1), get<Is>(t2)), result != 0) || ...);
		return result;
	}
#endif
}  // namespace Internal



// tuple
//
// eastl::tuple is a fixed-size container of heterogeneous values. It is a
// generalization of eastl::pair which hold only two heterogeneous values.
//
// https://en.cppreference.com/w/cpp/utility/tuple
//
template <typename... Ts>
class tuple;

template <typename T, typename... Ts>
class tuple<T, Ts...>
{
public:
	EA_CONSTEXPR tuple() = default;
	
	template <typename T2 = T, 
		Internal::TupleImplicitlyConvertible_t<tuple, const T2&, const Ts&...> = 0>
	EA_CONSTEXPR tuple(const T& t, const Ts&... ts)
		: mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, t, ts...)
	{
	}

	template <typename T2 = T, 
		Internal::TupleExplicitlyConvertible_t<tuple, const T2&, const Ts&...> = 0>
	explicit EA_CONSTEXPR tuple(const T& t, const Ts&... ts)
		: mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, t, ts...)
	{
	}

	template <typename U, typename... Us,
		Internal::TupleImplicitlyConvertible_t<tuple, U, Us...> = 0>
		EA_CONSTEXPR tuple(U&& u, Us&&... us)
			: mImpl(make_index_sequence<sizeof...(Us) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, eastl::forward<U>(u),
					eastl::forward<Us>(us)...)
	{
	}

	template <typename U, typename... Us,
		Internal::TupleExplicitlyConvertible_t<tuple, U, Us...> = 0>
		explicit EA_CONSTEXPR tuple(U&& u, Us&&... us)
			: mImpl(make_index_sequence<sizeof...(Us) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, eastl::forward<U>(u),
					eastl::forward<Us>(us)...)
	{
	}

	template <typename OtherTuple,
			  typename enable_if<Internal::TupleConvertible<OtherTuple, tuple>::value, bool>::type = false>
	tuple(OtherTuple&& t)
		: mImpl(eastl::forward<OtherTuple>(t))
	{
	}

	template <typename OtherTuple,
			  typename enable_if<Internal::TupleAssignable<tuple, OtherTuple>::value, bool>::type = false>
	tuple& operator=(OtherTuple&& t)
	{
		mImpl.operator=(eastl::forward<OtherTuple>(t));
		return *this;
	}

	void swap(tuple& t) { mImpl.swap(t.mImpl); }

private:
	typedef Internal::TupleImpl<make_index_sequence<sizeof...(Ts) + 1>, T, Ts...> Impl;
	Impl mImpl;

	template <size_t I, typename... Ts_>
    EA_CONSTEXPR friend tuple_element_t<I, tuple<Ts_...>> & get(tuple<Ts_...> & t);

	template <size_t I, typename... Ts_>
	EA_CONSTEXPR friend const_tuple_element_t<I, tuple<Ts_...>>& get(const tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
    EA_CONSTEXPR friend tuple_element_t<I, tuple<Ts_...>> && get(tuple<Ts_...> && t);

	template <typename T_, typename... ts_>
    EA_CONSTEXPR friend T_ & get(tuple<ts_...> & t);

	template <typename T_, typename... ts_>
    EA_CONSTEXPR friend const T_ & get(const tuple<ts_...> & t);

	template <typename T_, typename... ts_>
    EA_CONSTEXPR friend T_ && get(tuple<ts_...> && t);
};

// template specialization for an empty tuple
template <>
class tuple<>
{
public:
	void swap(tuple&) {}
};

template <size_t I, typename... Ts>
EA_CONSTEXPR inline tuple_element_t<I, tuple<Ts...>> & get(tuple<Ts...> & t)
{
	return get<I>(t.mImpl);
}

template <size_t I, typename... Ts>
EA_CONSTEXPR inline const_tuple_element_t<I, tuple<Ts...>> & get(const tuple<Ts...> & t)
{
	return get<I>(t.mImpl);
}

template <size_t I, typename... Ts>
EA_CONSTEXPR inline tuple_element_t<I, tuple<Ts...>> && get(tuple<Ts...> && t)
{
	return get<I>(eastl::move(t.mImpl));
}

template <typename T, typename... Ts>
EA_CONSTEXPR inline T & get(tuple<Ts...> & t)
{
	return get<T>(t.mImpl);
}

template <typename T, typename... Ts>
EA_CONSTEXPR inline const T & get(const tuple<Ts...> & t)
{
	return get<T>(t.mImpl);
}

template <typename T, typename... Ts>
EA_CONSTEXPR inline T && get(tuple<Ts...> && t)
{
	return get<T>(eastl::move(t.mImpl));
}

template <typename... Ts>
inline void swap(tuple<Ts...>& a, tuple<Ts...>& b)
{
	a.swap(b);
}


// tuple operators
//
//
template <typename... T1s, typename... T2s>
inline bool operator==(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleEqual<sizeof...(T1s)>()(t1, t2);
}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
template <typename... T1s, typename... T2s>
inline constexpr std::common_comparison_category_t<synth_three_way_result<T1s, T2s>...> operator<=>(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleThreeWay(t1, t2, make_index_sequence<sizeof...(T1s)>{});
}
#else
template <typename... T1s, typename... T2s>
inline bool operator<(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleLess<sizeof...(T1s)>()(t1, t2);
}

template <typename... T1s, typename... T2s> inline bool operator!=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t1 == t2); }
template <typename... T1s, typename... T2s> inline bool operator> (const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return t2 < t1; }
template <typename... T1s, typename... T2s> inline bool operator<=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t2 < t1); }
template <typename... T1s, typename... T2s> inline bool operator>=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t1 < t2); }
#endif

// tuple_cat 
//
//
template <typename... Tuples>
inline typename Internal::TupleCat<Tuples...>::ResultType tuple_cat(Tuples&&... ts)
{
	return Internal::TupleCat<Tuples...>::DoCat(eastl::forward<Tuples>(ts)...);
}


// make_tuple 
//
//
template <typename... Ts>
inline EA_CONSTEXPR tuple<Internal::MakeTupleReturn_t<Ts>...> make_tuple(Ts&&... values)
{
	return tuple<Internal::MakeTupleReturn_t<Ts>...>(eastl::forward<Ts>(values)...);
}


// forward_as_tuple 
//
//
template <typename... Ts>
inline EA_CONSTEXPR tuple<Ts&&...> forward_as_tuple(Ts&&... ts) EA_NOEXCEPT
{
	return tuple<Ts&&...>(eastl::forward<Ts&&>(ts)...);
}


// ignore 
//
// An object of unspecified type such that any value can be assigned to it with no effect.
//
// https://en.cppreference.com/w/cpp/utility/tuple/ignore
//
namespace Internal 
{
	struct ignore_t
	{
		ignore_t() = default;

		template <typename T>
		const ignore_t& operator=(const T&) const
		{
			return *this;
		}
	};
}// namespace Internal

static const Internal::ignore_t ignore;


// tie 
//
// Creates a tuple of lvalue references to its arguments or instances of eastl::ignore.
//
// https://en.cppreference.com/w/cpp/utility/tuple/tie
//
template <typename... Ts>
inline EA_CONSTEXPR tuple<Ts&...> tie(Ts&... ts) EA_NOEXCEPT
{
	return tuple<Ts&...>(ts...);
}


// apply
//
// Invoke a callable object using a tuple to supply the arguments. 
//
// http://en.cppreference.com/w/cpp/utility/apply
//
namespace detail
{
	template <class F, class Tuple, size_t... I>
	EA_CONSTEXPR decltype(auto) apply_impl(F&& f, Tuple&& t, index_sequence<I...>)
	{
		return invoke(eastl::forward<F>(f), get<I>(eastl::forward<Tuple>(t))...);
	}
} // namespace detail

template <class F, class Tuple>
EA_CONSTEXPR decltype(auto) apply(F&& f, Tuple&& t)
{
	return detail::apply_impl(eastl::forward<F>(f), eastl::forward<Tuple>(t),
		                      make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
}

}  // namespace eastl


///////////////////////////////////////////////////////////////
// C++17 structured bindings support for eastl::tuple
//
#ifndef EA_COMPILER_NO_STRUCTURED_BINDING
	#include <tuple>
	namespace std
	{
		// NOTE(rparolin): Some platform implementations didn't check the standard specification and implemented the
		// "tuple_size" and "tuple_element" primary template with as a struct.  The standard specifies they are
		// implemented with the class keyword so we provide the template specializations as a class and disable the
		// generated warning.
		EA_DISABLE_CLANG_WARNING(-Wmismatched-tags)

		template <class... Ts>
		class tuple_size<::eastl::tuple<Ts...>> : public ::eastl::integral_constant<size_t, sizeof...(Ts)>
		{
		};

		template <size_t I, class... Ts>
		class tuple_element<I, ::eastl::tuple<Ts...>> : public ::eastl::tuple_element<I, ::eastl::tuple<Ts...>>
		{
		};

		EA_RESTORE_CLANG_WARNING()
	}
#endif


#endif  // EASTL_TUPLE_ENABLED
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
#endif  // EASTL_TUPLE_H

```

## RANDOM
- Q: Why it's not an option??
- A: Even tho random_number_generator and discrete_distribution are optional, the original <EASTL/random.h> implementation is very very scarce, with a broken int_distribution that doesn't take ints also it doesn't provide a real_distribution, for discrete_distribution for algorithms using weights, like loot drops.
- So you can skip the includes of rng and dd but the int_distribution static assert will fail with RNGs like std::mt19973 since its result_type should be an uint, not int.
- Also, it's kinda a big change, so you have to want it.
- https://en.cppreference.com/w/cpp/header/random
- TODO: 2) Creare un eastl::random_device, somehow :(
- TODO: 3) Figure out how to include discrete-distribution in random.h without breaking everything.

- <EASTL/random_number_generator.h>
- Needs pcg files in EASTL/internal/random/
```c++
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
        // TODO: if removecvref is used, everything falls apart :( But shhh
        template<typename SeedSeq, typename = eastl::enable_if_t<!eastl::is_same_v<
                                       eastl::remove_reference_t<SeedSeq>, RNG>>>
        RNG(SeedSeq && seedSeq)
          : rng(seedSeq)
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
		//static_assert(sizeof(typename Generator::result_type) == 4, "Int distribution generator result_type should be at least 4 bytes.");
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