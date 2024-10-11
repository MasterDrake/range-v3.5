// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_DETAIL_CONFIG_HPP
#define EARANGES_DETAIL_CONFIG_HPP

#include <EASTL/internal/config.h>

// Grab some version information.
#ifndef __has_include
#include <iosfwd>
#elif __has_include(<version>)
#include <version>
#else
#include <iosfwd>
#endif

#if(defined(NDEBUG) && !defined(EARANGES_ENSURE_MSG)) || \
    (!defined(NDEBUG) && !defined(EARANGES_ASSERT) &&    \
     ((defined(__GNUC__) && !defined(__clang__) &&     \
       (__GNUC__ < 5 || defined(__MINGW32__))) ||      \
      defined(_MSVC_STL_VERSION)))
#include <cstdio>
#include <cstdlib>

namespace ranges
{
    namespace detail
    {
        template<typename = void>
        [[noreturn]] void assert_failure(char const * file, int line, char const * msg)
        {
            std::fprintf(stderr, "%s(%d): %s\n", file, line, msg);
            std::abort();
        }
    } // namespace detail
} // namespace ranges

#endif

#ifndef EARANGES_ASSERT
// Always use our hand-rolled assert implementation on older GCCs, which do
// not allow assert to be used in a constant expression, and on MSVC whose
// assert is not marked [[noreturn]].
#if !defined(NDEBUG) && ((defined(__GNUC__) && !defined(__clang__) && \
                          (__GNUC__ < 5 || defined(__MINGW32__))) ||  \
                         defined(_MSVC_STL_VERSION))
#define EARANGES_ASSERT(...)                                    \
    static_cast<void>((__VA_ARGS__)                           \
                          ? void(0)                           \
                          : ::ranges::detail::assert_failure( \
                                __FILE__, __LINE__, "assertion failed: " #__VA_ARGS__))
#else
#include <cassert>
#define EARANGES_ASSERT assert
#endif
#endif

#include <EARanges/meta/meta_fwd.hpp>

#ifndef EARANGES_ASSUME
#if defined(__clang__) || defined(__GNUC__)
#define EARANGES_ASSUME(COND) static_cast<void>((COND) ? void(0) : __builtin_unreachable())
#elif defined(_MSC_VER)
#define EARANGES_ASSUME(COND) static_cast<void>(__assume(COND))
#else
#define EARANGES_ASSUME(COND) static_cast<void>(COND)
#endif
#endif // EARANGES_ASSUME

#ifndef EARANGES_EXPECT
#ifdef NDEBUG
#define EARANGES_EXPECT(COND) EARANGES_ASSUME(COND)
#else // NDEBUG
#define EARANGES_EXPECT(COND) EARANGES_ASSERT(COND)
#endif // NDEBUG
#endif // EARANGES_EXPECT

#ifndef EARANGES_ENSURE_MSG
#if defined(NDEBUG)
#define EARANGES_ENSURE_MSG(COND, MSG)                             \
    static_cast<void>((COND) ? void(0)                           \
                             : ::ranges::detail::assert_failure( \
                                   __FILE__, __LINE__, "ensure failed: " MSG))
#else
#define EARANGES_ENSURE_MSG(COND, MSG) EARANGES_ASSERT((COND) && MSG)
#endif
#endif

#ifndef EARANGES_ENSURE
#define EARANGES_ENSURE(...) EARANGES_ENSURE_MSG((__VA_ARGS__), #__VA_ARGS__)
#endif

#define EARANGES_DECLTYPE_AUTO_RETURN(...) \
    ->decltype(__VA_ARGS__)              \
    {                                    \
        return (__VA_ARGS__);            \
    }                                    \
    /**/

#define EARANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(...)                                 \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__)))->decltype(__VA_ARGS__) \
    {                                                                             \
        return (__VA_ARGS__);                                                     \
    }                                                                             \
    /**/

#define EARANGES_AUTO_RETURN_NOEXCEPT(...)                   \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__))) \
    {                                                      \
        return (__VA_ARGS__);                              \
    }                                                      \
    /**/

#define EARANGES_DECLTYPE_NOEXCEPT(...) \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__)))->decltype(__VA_ARGS__) /**/

// Non-portable forward declarations of standard containers
#define EARANGES_BEGIN_NAMESPACE_STD META_BEGIN_NAMESPACE_STD
#define EARANGES_END_NAMESPACE_STD META_END_NAMESPACE_STD
#define EARANGES_BEGIN_NAMESPACE_VERSION META_BEGIN_NAMESPACE_VERSION
#define EARANGES_END_NAMESPACE_VERSION META_END_NAMESPACE_VERSION
#define EARANGES_BEGIN_NAMESPACE_CONTAINER META_BEGIN_NAMESPACE_CONTAINER
#define EARANGES_END_NAMESPACE_CONTAINER META_END_NAMESPACE_CONTAINER

// Database of feature versions
#define EARANGES_CXX_STATIC_ASSERT_11 200410L
#define EARANGES_CXX_STATIC_ASSERT_14 EARANGES_CXX_STATIC_ASSERT_11
#define EARANGES_CXX_STATIC_ASSERT_17 201411L
#define EARANGES_CXX_VARIABLE_TEMPLATES_11 0L
#define EARANGES_CXX_VARIABLE_TEMPLATES_14 201304L
#define EARANGES_CXX_VARIABLE_TEMPLATES_17 EARANGES_CXX_VARIABLE_TEMPLATES_14
#define EARANGES_CXX_ATTRIBUTE_DEPRECATED_11 0L
#define EARANGES_CXX_ATTRIBUTE_DEPRECATED_14 201309L
#define EARANGES_CXX_ATTRIBUTE_DEPRECATED_17 EARANGES_CXX_ATTRIBUTE_DEPRECATED_14
#define EARANGES_CXX_CONSTEXPR_11 200704L
#define EARANGES_CXX_CONSTEXPR_14 201304L
#define EARANGES_CXX_CONSTEXPR_17 201603L
#define EARANGES_CXX_CONSTEXPR_20 201907L
#define EARANGES_CXX_CONSTEXPR_LAMBDAS 201603L
#define EARANGES_CXX_RANGE_BASED_FOR_11 200907L
#define EARANGES_CXX_RANGE_BASED_FOR_14 EARANGES_CXX_RANGE_BASED_FOR_11
#define EARANGES_CXX_RANGE_BASED_FOR_17 201603L
#define EARANGES_CXX_LIB_IS_FINAL_11 0L
#define EARANGES_CXX_LIB_IS_FINAL_14 201402L
#define EARANGES_CXX_LIB_IS_FINAL_17 EARANGES_CXX_LIB_IS_FINAL_14
#define EARANGES_CXX_RETURN_TYPE_DEDUCTION_11 0L
#define EARANGES_CXX_RETURN_TYPE_DEDUCTION_14 201304L
#define EARANGES_CXX_RETURN_TYPE_DEDUCTION_17 EARANGES_CXX_RETURN_TYPE_DEDUCTION_14
#define EARANGES_CXX_GENERIC_LAMBDAS_11 0L
#define EARANGES_CXX_GENERIC_LAMBDAS_14 201304L
#define EARANGES_CXX_GENERIC_LAMBDAS_17 EARANGES_CXX_GENERIC_LAMBDAS_14
#define EARANGES_CXX_STD_11 201103L
#define EARANGES_CXX_STD_14 201402L
#define EARANGES_CXX_STD_17 201703L
#define EARANGES_CXX_THREAD_LOCAL_PRE_STANDARD \
    200000L // Arbitrary number between 0 and C++11
#define EARANGES_CXX_THREAD_LOCAL_11 EARANGES_CXX_STD_11
#define EARANGES_CXX_THREAD_LOCAL_14 EARANGES_CXX_THREAD_LOCAL_11
#define EARANGES_CXX_THREAD_LOCAL_17 EARANGES_CXX_THREAD_LOCAL_14
#define EARANGES_CXX_INLINE_VARIABLES_11 0L
#define EARANGES_CXX_INLINE_VARIABLES_14 0L
#define EARANGES_CXX_INLINE_VARIABLES_17 201606L
#define EARANGES_CXX_COROUTINES_11 0L
#define EARANGES_CXX_COROUTINES_14 0L
#define EARANGES_CXX_COROUTINES_17 0L
#define EARANGES_CXX_COROUTINES_TS1 201703L
#define EARANGES_CXX_DEDUCTION_GUIDES_11 0L
#define EARANGES_CXX_DEDUCTION_GUIDES_14 0L
#define EARANGES_CXX_DEDUCTION_GUIDES_17 201606L
#define EARANGES_CXX_IF_CONSTEXPR_11 0L
#define EARANGES_CXX_IF_CONSTEXPR_14 0L
#define EARANGES_CXX_IF_CONSTEXPR_17 201606L
#define EARANGES_CXX_ALIGNED_NEW_11 0L
#define EARANGES_CXX_ALIGNED_NEW_14 0L
#define EARANGES_CXX_ALIGNED_NEW_17 201606L

// Implementation-specific diagnostic control
#if defined(_MSC_VER) && !defined(__clang__)
#define EARANGES_DIAGNOSTIC_PUSH __pragma(warning(push))
#define EARANGES_DIAGNOSTIC_POP __pragma(warning(pop))
#define EARANGES_DIAGNOSTIC_IGNORE_PRAGMAS __pragma(warning(disable : 4068))
#define EARANGES_DIAGNOSTIC_IGNORE(X) \
    EARANGES_DIAGNOSTIC_IGNORE_PRAGMAS __pragma(warning(disable : X))
#define EARANGES_DIAGNOSTIC_IGNORE_SHADOWING EARANGES_DIAGNOSTIC_IGNORE(4456)
#define EARANGES_DIAGNOSTIC_IGNORE_INDENTATION
#define EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
#define EARANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS EARANGES_DIAGNOSTIC_IGNORE(4099)
#define EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
#define EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
#define EARANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL
#define EARANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER
#define EARANGES_DIAGNOSTIC_IGNORE_ZERO_LENGTH_ARRAY
#define EARANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
#define EARANGES_DIAGNOSTIC_IGNORE_CXX2A_COMPAT
#define EARANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
#define EARANGES_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION
#define EARANGES_DIAGNOSTIC_IGNORE_MISSING_BRACES
#define EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE
#define EARANGES_DIAGNOSTIC_IGNORE_INCONSISTENT_OVERRIDE
#define EARANGES_DIAGNOSTIC_IGNORE_RANGE_LOOP_ANALYSIS
#define EARANGES_DIAGNOSTIC_IGNORE_DEPRECATED_THIS_CAPTURE
#define EARANGES_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
// Ignores both "divide by zero" and "mod by zero":
#define EARANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO EARANGES_DIAGNOSTIC_IGNORE(4723 4724)
#define EARANGES_DIAGNOSTIC_IGNORE_UNSIGNED_MATH EARANGES_DIAGNOSTIC_IGNORE(4146)
#define EARANGES_DIAGNOSTIC_IGNORE_TRUNCATION EARANGES_DIAGNOSTIC_IGNORE(4244)
#define EARANGES_DIAGNOSTIC_IGNORE_MULTIPLE_ASSIGNMENT_OPERATORS \
    EARANGES_DIAGNOSTIC_IGNORE(4522)
#define EARANGES_DIAGNOSTIC_IGNORE_VOID_PTR_DEREFERENCE
#define EARANGES_DIAGNOSTIC_KEYWORD_MACRO
#define EARANGES_DIAGNOSTIC_SUGGEST_OVERRIDE

#define EARANGES_CXX_VER _MSVC_LANG

#if _MSC_VER < 1920 || _MSVC_LANG < 201703L
#error range-v3 requires Visual Studio 2019 with the /std:c++17 (or /std:c++latest) and /permissive- options.
#endif

#if _MSC_VER < 1927
#define EARANGES_WORKAROUND_MSVC_895622 // Error when phase 1 name binding finds only
                                      // deleted function

#if _MSC_VER < 1925
#define EARANGES_WORKAROUND_MSVC_779708 // ADL for operands of function type [No workaround]

#if _MSC_VER < 1923
#define EARANGES_WORKAROUND_MSVC_573728 // rvalues of array types bind to lvalue references
                                      // [no workaround]
#define EARANGES_WORKAROUND_MSVC_934330 // Deduction guide not correctly preferred to copy
                                      // deduction candidate [No workaround]

#if _MSC_VER < 1922
#define EARANGES_WORKAROUND_MSVC_756601 // constexpr friend non-template erroneously
                                      // rejected with C3615
#define EARANGES_WORKAROUND_MSVC_793042 // T[0] sometimes accepted as a valid type in SFINAE
                                      // context

#if _MSC_VER < 1921
#define EARANGES_WORKAROUND_MSVC_785522 // SFINAE failure for error in immediate context
#define EARANGES_WORKAROUND_MSVC_786376 // Assertion casting anonymous union member in
                                      // trailing-return-type
#define EARANGES_WORKAROUND_MSVC_787074 // Over-eager substitution of dependent type in
                                      // non-instantiated nested class template
#define EARANGES_WORKAROUND_MSVC_790554 // Assert for return type that uses dependent
                                      // default non-type template argument
#endif                                // _MSC_VER < 1921
#endif                                // _MSC_VER < 1922
#endif                                // _MSC_VER < 1923
#endif                                // _MSC_VER < 1925
#endif                                // _MSC_VER < 1926

#if 1 // Fixed in 1920, but more bugs hiding behind workaround
#define EARANGES_WORKAROUND_MSVC_701385 // Yet another alias expansion error
#endif

#define EARANGES_WORKAROUND_MSVC_249830 // constexpr and arguments that aren't subject to
                                      // lvalue-to-rvalue conversion
#define EARANGES_WORKAROUND_MSVC_677925 // Bogus C2676 "binary '++': '_Ty' does not define
                                      // this operator"
#define EARANGES_WORKAROUND_MSVC_683388 // decltype(*i) is incorrectly an rvalue reference
                                      // for pointer-to-array i
#define EARANGES_WORKAROUND_MSVC_688606 // SFINAE failing to account for access control
                                      // during specialization matching
#define EARANGES_WORKAROUND_MSVC_786312 // Yet another mixed-pack-expansion failure
#define EARANGES_WORKAROUND_MSVC_792338 // Failure to match specialization enabled via call
                                      // to constexpr function
#define EARANGES_WORKAROUND_MSVC_835948 // Silent bad codegen destroying sized_generator [No
                                      // workaround]
#define EARANGES_WORKAROUND_MSVC_934264 // Explicitly-defaulted inherited default
                                      // constructor is not correctly implicitly constexpr

#elif defined(__GNUC__) || defined(__clang__)
#define EARANGES_PRAGMA(X) _Pragma(#X)
#define EARANGES_DIAGNOSTIC_PUSH EARANGES_PRAGMA(GCC diagnostic push)
#define EARANGES_DIAGNOSTIC_POP EARANGES_PRAGMA(GCC diagnostic pop)
#define EARANGES_DIAGNOSTIC_IGNORE_PRAGMAS EARANGES_PRAGMA(GCC diagnostic ignored "-Wpragmas")
#define EARANGES_DIAGNOSTIC_IGNORE(X)                                  \
    EARANGES_DIAGNOSTIC_IGNORE_PRAGMAS                                 \
    EARANGES_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas")        \
    EARANGES_PRAGMA(GCC diagnostic ignored "-Wunknown-warning-option") \
    EARANGES_PRAGMA(GCC diagnostic ignored X)
#define EARANGES_DIAGNOSTIC_IGNORE_SHADOWING EARANGES_DIAGNOSTIC_IGNORE("-Wshadow")
#define EARANGES_DIAGNOSTIC_IGNORE_INDENTATION \
    EARANGES_DIAGNOSTIC_IGNORE("-Wmisleading-indentation")
#define EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL \
    EARANGES_DIAGNOSTIC_IGNORE("-Wundefined-internal")
#define EARANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS \
    EARANGES_DIAGNOSTIC_IGNORE("-Wmismatched-tags")
#define EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION \
    EARANGES_DIAGNOSTIC_IGNORE("-Wsign-conversion")
#define EARANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL EARANGES_DIAGNOSTIC_IGNORE("-Wfloat-equal")
#define EARANGES_DIAGNOSTIC_IGNORE_FLOAT_CONVERSION EARANGES_DIAGNOSTIC_IGNORE("-Wfloat-conversion")
#define EARANGES_DIAGNOSTIC_IGNORE_MISSING_BRACES \
    EARANGES_DIAGNOSTIC_IGNORE("-Wmissing-braces")
#define EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS \
    EARANGES_DIAGNOSTIC_IGNORE("-Wglobal-constructors")
#define EARANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL \
    EARANGES_DIAGNOSTIC_IGNORE("-Wunneeded-internal-declaration")
#define EARANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER \
    EARANGES_DIAGNOSTIC_IGNORE("-Wunneeded-member-function")
#define EARANGES_DIAGNOSTIC_IGNORE_ZERO_LENGTH_ARRAY \
    EARANGES_DIAGNOSTIC_IGNORE("-Wzero-length-array")
#define EARANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT EARANGES_DIAGNOSTIC_IGNORE("-Wc++1z-compat")
#define EARANGES_DIAGNOSTIC_IGNORE_CXX2A_COMPAT EARANGES_DIAGNOSTIC_IGNORE("-Wc++2a-compat")
#define EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE \
    EARANGES_DIAGNOSTIC_IGNORE("-Wundefined-func-template")
#define EARANGES_DIAGNOSTIC_IGNORE_INCONSISTENT_OVERRIDE \
    EARANGES_DIAGNOSTIC_IGNORE("-Winconsistent-missing-override")
#define EARANGES_DIAGNOSTIC_IGNORE_RANGE_LOOP_ANALYSIS \
    EARANGES_DIAGNOSTIC_IGNORE("-Wrange-loop-analysis")
#define EARANGES_DIAGNOSTIC_IGNORE_DEPRECATED_THIS_CAPTURE \
    EARANGES_DIAGNOSTIC_IGNORE("-Wdeprecated-this-capture")
#define EARANGES_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME \
    EARANGES_DIAGNOSTIC_IGNORE("-Winit-list-lifetime")
#define EARANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO
#define EARANGES_DIAGNOSTIC_IGNORE_UNSIGNED_MATH
#define EARANGES_DIAGNOSTIC_IGNORE_TRUNCATION
#define EARANGES_DIAGNOSTIC_IGNORE_MULTIPLE_ASSIGNMENT_OPERATORS
#define EARANGES_DIAGNOSTIC_IGNORE_VOID_PTR_DEREFERENCE \
    EARANGES_DIAGNOSTIC_IGNORE("-Wvoid-ptr-dereference")
#define EARANGES_DIAGNOSTIC_KEYWORD_MACRO EARANGES_DIAGNOSTIC_IGNORE("-Wkeyword-macro")
#define EARANGES_DIAGNOSTIC_SUGGEST_OVERRIDE EARANGES_DIAGNOSTIC_IGNORE("-Wsuggest-override")

#define EARANGES_WORKAROUND_CWG_1554
#ifdef __clang__
#if __clang_major__ < 4
#define EARANGES_WORKAROUND_CLANG_23135 // constexpr leads to premature instantiation on
                                      // clang-3.x
#endif
#if (__clang_major__ >= 7 && __clang_major__ <= 9) || defined(__apple_build_version__)
#define EARANGES_WORKAROUND_CLANG_43400 // template friend is redefinition of itself
#endif
#else                                 // __GNUC__
#if __GNUC__ == 7 || __GNUC__ == 8
#define EARANGES_WORKAROUND_GCC_91525 /* Workaround strange GCC ICE */
#endif
#if __GNUC__ >= 9
#if __GNUC__ == 9 && __GNUC_MINOR__ < 3 && __cplusplus == EARANGES_CXX_STD_17
#define EARANGES_WORKAROUND_GCC_91923 // Failure-to-SFINAE with class type NTTP in C++17
#endif
#endif
#endif

#else
#define EARANGES_DIAGNOSTIC_PUSH
#define EARANGES_DIAGNOSTIC_POP
#define EARANGES_DIAGNOSTIC_IGNORE_PRAGMAS
#define EARANGES_DIAGNOSTIC_IGNORE_SHADOWING
#define EARANGES_DIAGNOSTIC_IGNORE_INDENTATION
#define EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
#define EARANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
#define EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
#define EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
#define EARANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL
#define EARANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER
#define EARANGES_DIAGNOSTIC_IGNORE_ZERO_LENGTH_ARRAY
#define EARANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
#define EARANGES_DIAGNOSTIC_IGNORE_CXX2A_COMPAT
#define EARANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
#define EARANGES_DIAGNOSTIC_IGNORE_MISSING_BRACES
#define EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE
#define EARANGES_DIAGNOSTIC_IGNORE_INCONSISTENT_OVERRIDE
#define EARANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
#define EARANGES_DIAGNOSTIC_IGNORE_DEPRECATED_THIS_CAPTURE
#define EARANGES_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
#define EARANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO
#define EARANGES_DIAGNOSTIC_IGNORE_UNSIGNED_MATH
#define EARANGES_DIAGNOSTIC_IGNORE_TRUNCATION
#define EARANGES_DIAGNOSTIC_IGNORE_MULTIPLE_ASSIGNMENT_OPERATORS
#define EARANGES_DIAGNOSTIC_IGNORE_VOID_PTR_DEREFERENCE
#define EARANGES_DIAGNOSTIC_KEYWORD_MACRO
#define EARANGES_DIAGNOSTIC_SUGGEST_OVERRIDE
#endif

// Configuration via feature-test macros, with fallback to __cplusplus
#ifndef EARANGES_CXX_VER
#define EARANGES_CXX_VER __cplusplus
#endif

#define EARANGES_CXX_FEATURE_CONCAT2(y, z) EARANGES_CXX_##y##_##z
#define EARANGES_CXX_FEATURE_CONCAT(y, z) EARANGES_CXX_FEATURE_CONCAT2(y, z)

#if EARANGES_CXX_VER >= EARANGES_CXX_STD_17
#define EARANGES_CXX_STD EARANGES_CXX_STD_17
#define EARANGES_CXX_FEATURE(x) EARANGES_CXX_FEATURE_CONCAT(x, 17)
#elif EARANGES_CXX_VER >= EARANGES_CXX_STD_14
#define EARANGES_CXX_STD EARANGES_CXX_STD_14
#define EARANGES_CXX_FEATURE(x) EARANGES_CXX_FEATURE_CONCAT(x, 14)
#else
#define EARANGES_CXX_STD EARANGES_CXX_STD_11
#define EARANGES_CXX_FEATURE(x) EARANGES_CXX_FEATURE_CONCAT(x, 11)
#endif

#ifndef EARANGES_CXX_STATIC_ASSERT
#ifdef __cpp_static_assert
#define EARANGES_CXX_STATIC_ASSERT __cpp_static_assert
#else
#define EARANGES_CXX_STATIC_ASSERT EARANGES_CXX_FEATURE(STATIC_ASSERT)
#endif
#endif

#ifndef EARANGES_CXX_VARIABLE_TEMPLATES
//#ifdef __cpp_variable_templates
//#define EARANGES_CXX_VARIABLE_TEMPLATES __cpp_variable_templates
//#else
// #define EARANGES_CXX_VARIABLE_TEMPLATES EARANGES_CXX_FEATURE(VARIABLE_TEMPLATES)
#define EARANGES_CXX_VARIABLE_TEMPLATES 0
//#endif
#endif

#if(defined(__cpp_lib_type_trait_variable_templates) && \
    __cpp_lib_type_trait_variable_templates > 0) ||     \
    EARANGES_CXX_VER >= EARANGES_CXX_STD_17
#define EARANGES_CXX_TRAIT_VARIABLE_TEMPLATES 1
#else
#define EARANGES_CXX_TRAIT_VARIABLE_TEMPLATES 0
#endif

#ifndef EARANGES_CXX_ATTRIBUTE_DEPRECATED
#ifdef __has_cpp_attribute
#define EARANGES_CXX_ATTRIBUTE_DEPRECATED __has_cpp_attribute(deprecated)
#elif defined(__cpp_attribute_deprecated)
#define EARANGES_CXX_ATTRIBUTE_DEPRECATED __cpp_attribute_deprecated
#else
#define EARANGES_CXX_ATTRIBUTE_DEPRECATED EARANGES_CXX_FEATURE(ATTRIBUTE_DEPRECATED)
#endif
#endif

#ifndef EARANGES_CXX_CONSTEXPR
#ifdef __cpp_constexpr
#define EARANGES_CXX_CONSTEXPR __cpp_constexpr
#else
#define EARANGES_CXX_CONSTEXPR EARANGES_CXX_FEATURE(CONSTEXPR)
#endif
#endif

#ifndef EARANGES_CXX_RANGE_BASED_FOR
#ifdef __cpp_range_based_for
#define EARANGES_CXX_RANGE_BASED_FOR __cpp_range_based_for
#else
#define EARANGES_CXX_RANGE_BASED_FOR EARANGES_CXX_FEATURE(RANGE_BASED_FOR)
#endif
#endif

#ifndef EARANGES_CXX_LIB_IS_FINAL
#include <type_traits>
#ifdef __cpp_lib_is_final
#define EARANGES_CXX_LIB_IS_FINAL __cpp_lib_is_final
#else
#define EARANGES_CXX_LIB_IS_FINAL EARANGES_CXX_FEATURE(LIB_IS_FINAL)
#endif
#endif

#ifndef EARANGES_CXX_RETURN_TYPE_DEDUCTION
#ifdef __cpp_return_type_deduction
#define EARANGES_CXX_RETURN_TYPE_DEDUCTION __cpp_return_type_deduction
#else
#define EARANGES_CXX_RETURN_TYPE_DEDUCTION EARANGES_CXX_FEATURE(RETURN_TYPE_DEDUCTION)
#endif
#endif

#ifndef EARANGES_CXX_GENERIC_LAMBDAS
#ifdef __cpp_generic_lambdas
#define EARANGES_CXX_GENERIC_LAMBDAS __cpp_generic_lambdas
#else
#define EARANGES_CXX_GENERIC_LAMBDAS EARANGES_CXX_FEATURE(GENERIC_LAMBDAS)
#endif
#endif

#ifndef EARANGES_CXX_THREAD_LOCAL
#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED <= 70100
#define EARANGES_CXX_THREAD_LOCAL 0
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED) || \
    (defined(__clang__) && (defined(__CYGWIN__) || defined(__apple_build_version__)))
// BUGBUG avoid unresolved __cxa_thread_atexit
#define EARANGES_CXX_THREAD_LOCAL EARANGES_CXX_THREAD_LOCAL_PRE_STANDARD
#else
#define EARANGES_CXX_THREAD_LOCAL EARANGES_CXX_FEATURE(THREAD_LOCAL)
#endif
#endif

#ifndef EARANGES_CXX_COROUTINES
#if defined(__cpp_coroutines) && defined(__has_include)
#if __has_include(<coroutine>)
#define EARANGES_CXX_COROUTINES __cpp_coroutines
#define EARANGES_COROUTINES_HEADER <coroutine>
#define EARANGES_COROUTINES_NS std
#elif __has_include(<experimental/coroutine>)
#define EARANGES_CXX_COROUTINES __cpp_coroutines
#define EARANGES_COROUTINES_HEADER <experimental/coroutine>
#define EARANGES_COROUTINES_NS std::experimental
#endif
#endif
#ifndef EARANGES_CXX_COROUTINES
#define EARANGES_CXX_COROUTINES EARANGES_CXX_FEATURE(COROUTINES)
#endif
#endif

#ifdef NDEBUG
#define EARANGES_NDEBUG_CONSTEXPR constexpr
#else
#define EARANGES_NDEBUG_CONSTEXPR inline
#endif

#ifndef EARANGES_CXX_INLINE_VARIABLES
#ifdef __cpp_inline_variables
#define EARANGES_CXX_INLINE_VARIABLES __cpp_inline_variables
#elif defined(__clang__) && (__clang_major__ == 3 && __clang_minor__ == 9) && \
    EARANGES_CXX_VER > EARANGES_CXX_STD_14
// Clang 3.9 supports inline variables in C++17 mode, but doesn't define
// __cpp_inline_variables
#define EARANGES_CXX_INLINE_VARIABLES EARANGES_CXX_INLINE_VARIABLES_17
#else
#define EARANGES_CXX_INLINE_VARIABLES EARANGES_CXX_FEATURE(INLINE_VARIABLES)
#endif // __cpp_inline_variables
#endif // EARANGES_CXX_INLINE_VARIABLES

#if EARANGES_CXX_INLINE_VARIABLES < EARANGES_CXX_INLINE_VARIABLES_17 && \
    !defined(EARANGES_DOXYGEN_INVOKED)
#define EARANGES_INLINE_VAR
#define EARANGES_INLINE_VARIABLE(type, name)                           \
    namespace                                                        \
    {                                                                \
        constexpr auto & name = ::ranges::static_const<type>::value; \
    }
#else // EARANGES_CXX_INLINE_VARIABLES >= EARANGES_CXX_INLINE_VARIABLES_17
#define EARANGES_INLINE_VAR inline
#define EARANGES_INLINE_VARIABLE(type, name) \
    inline constexpr type name{};          \
    /**/
#endif // EARANGES_CXX_INLINE_VARIABLES

#if defined(EARANGES_DOXYGEN_INVOKED)
#define EARANGES_DEFINE_CPO(type, name) \
    inline constexpr type name{};     \
    /**/
#elif EARANGES_CXX_INLINE_VARIABLES < EARANGES_CXX_INLINE_VARIABLES_17
#define EARANGES_DEFINE_CPO(type, name)                                \
    namespace                                                        \
    {                                                                \
        constexpr auto & name = ::ranges::static_const<type>::value; \
    }                                                                \
    /**/
#else // EARANGES_CXX_INLINE_VARIABLES >= EARANGES_CXX_INLINE_VARIABLES_17
#define EARANGES_DEFINE_CPO(type, name) \
    namespace _                       \
    {                                 \
        inline constexpr type name{}; \
    }                                 \
    using namespace _;                \
    /**/
#endif // EARANGES_CXX_INLINE_VARIABLES

#ifndef EARANGES_DOXYGEN_INVOKED
#define EARANGES_HIDDEN_DETAIL(...) __VA_ARGS__
#else
#define EARANGES_HIDDEN_DETAIL(...)
#endif

#ifndef EARANGES_DOXYGEN_INVOKED
#define EARANGES_ADL_BARRIER_FOR(S) S##_ns
#define EARANGES_STRUCT_WITH_ADL_BARRIER(S) \
    _ranges_adl_barrier_noop_;            \
    namespace EARANGES_ADL_BARRIER_FOR(S)   \
    {                                     \
        struct S;                         \
    }                                     \
    using EARANGES_ADL_BARRIER_FOR(S)::S;   \
    struct EARANGES_ADL_BARRIER_FOR(S)::S /**/
#else
#define EARANGES_ADL_BARRIER_FOR(S)
#define EARANGES_STRUCT_WITH_ADL_BARRIER(S) S
#endif

#ifndef EARANGES_DOXYGEN_INVOKED
#define EARANGES_FUNC_BEGIN(NAME) \
    struct NAME##_fn            \
    {
#define EARANGES_FUNC_END(NAME) \
    }                         \
    ;                         \
    EARANGES_INLINE_VARIABLE(NAME##_fn, NAME)
#define EARANGES_FUNC(NAME) operator() EARANGES_FUNC_CONST_ /**/
#define EARANGES_FUNC_CONST_(...) (__VA_ARGS__) const
#else
#define EARANGES_FUNC_BEGIN(NAME)
#define EARANGES_FUNC_END(NAME)
#define EARANGES_FUNC(NAME) NAME
#endif

#ifndef EARANGES_CXX_DEDUCTION_GUIDES
#if defined(__clang__) && defined(__apple_build_version__)
// Apple's clang version doesn't do deduction guides very well.
#define EARANGES_CXX_DEDUCTION_GUIDES 0
#elif defined(__cpp_deduction_guides)
#define EARANGES_CXX_DEDUCTION_GUIDES __cpp_deduction_guides
#else
#define EARANGES_CXX_DEDUCTION_GUIDES EARANGES_CXX_FEATURE(DEDUCTION_GUIDES)
#endif // __cpp_deduction_guides
#endif // EARANGES_CXX_DEDUCTION_GUIDES

// __VA_OPT__
#ifndef EARANGES_CXX_VA_OPT
//#if __cplusplus > 201703L
//#define EARANGES_CXX_THIRD_ARG_(A, B, C, ...) C
//#define EARANGES_CXX_VA_OPT_I_(...) EARANGES_CXX_THIRD_ARG_(__VA_OPT__(, ), 1, 0, ?)
//#define EARANGES_CXX_VA_OPT EARANGES_CXX_VA_OPT_I_(?)
//#else
#define EARANGES_CXX_VA_OPT 0
//#endif
#endif // EARANGES_CXX_VA_OPT

#ifndef EARANGES_CXX_IF_CONSTEXPR
#ifdef __cpp_if_constexpr
#define EARANGES_CXX_IF_CONSTEXPR __cpp_if_constexpr
#else
#define EARANGES_CXX_IF_CONSTEXPR EARANGES_CXX_FEATURE(IF_CONSTEXPR)
#endif
#endif // EARANGES_CXX_IF_CONSTEXPR

// Its not enough for the compiler to support this; the stdlib must support it too.
#ifndef EARANGES_CXX_ALIGNED_NEW
#if(!defined(_LIBCPP_VERSION) ||                                                 \
    (_LIBCPP_VERSION >= 4000 && !defined(_LIBCPP_HAS_NO_ALIGNED_ALLOCATION))) && \
    (!defined(__GLIBCXX__) || (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE >= 7))
#if defined(__cpp_aligned_new)
#define EARANGES_CXX_ALIGNED_NEW __cpp_aligned_new
#else
#define EARANGES_CXX_ALIGNED_NEW EARANGES_CXX_FEATURE(ALIGNED_NEW)
#endif
#else // _LIBCPP_VERSION < 4000 || __GLIBCXX__ < 20170502
#define EARANGES_CXX_ALIGNED_NEW 0L
#endif
#endif // EARANGES_CXX_ALIGNED_NEW

//#if defined(__clang__)
//#define EARANGES_IS_SAME(...) __is_same(__VA_ARGS__)
//#elif defined(__GNUC__) && __GNUC__ >= 6
//#define EARANGES_IS_SAME(...) __is_same_as(__VA_ARGS__)
//#elif EARANGES_CXX_TRAIT_VARIABLE_TEMPLATES
//#define EARANGES_IS_SAME(...) eastl::is_same_v<__VA_ARGS__>
//#else
#define EARANGES_IS_SAME(...) eastl::is_same<__VA_ARGS__>::value
//#endif
//TODO: Check alternatives in eastl for this.
//#if defined(__has_cpp_attribute) && __has_cpp_attribute(no_unique_address)
//#define EARANGES_NO_UNIQUE_ADDRESS [[no_unique_address]]
//#else
#define EARANGES_NO_UNIQUE_ADDRESS
//#endif

#if defined(__clang__)
#if __has_attribute(no_sanitize)
#define EARANGES_INTENDED_MODULAR_ARITHMETIC \
    __attribute__((__no_sanitize__("unsigned-integer-overflow")))
#else
#define EARANGES_INTENDED_MODULAR_ARITHMETIC
#endif
#else
#define EARANGES_INTENDED_MODULAR_ARITHMETIC
#endif

#ifndef EARANGES_CONSTEXPR_IF
#if EARANGES_CXX_IF_CONSTEXPR >= EARANGES_CXX_IF_CONSTEXPR_17
#define EARANGES_CONSTEXPR_IF(...) false) \
    {} else if constexpr(__VA_ARGS__
#else
#define EARANGES_CONSTEXPR_IF(...) __VA_ARGS__
#endif
#endif // EARANGES_CONSTEXPR_IF

#if !defined(EARANGES_BROKEN_CPO_LOOKUP) && \
    !defined(EARANGES_DOXYGEN_INVOKED) && \
    defined(EARANGES_WORKAROUND_MSVC_895622)
#define EARANGES_BROKEN_CPO_LOOKUP 1
#endif
#ifndef EARANGES_BROKEN_CPO_LOOKUP
#define EARANGES_BROKEN_CPO_LOOKUP 0
#endif

#ifndef EARANGES_NODISCARD
#if defined(__has_cpp_attribute) && __has_cpp_attribute(nodiscard)
#if defined(__clang__) && __cplusplus < 201703L
// clang complains about using nodiscard in C++14 mode.
#define EARANGES_NODISCARD                           \
    EARANGES_DIAGNOSTIC_PUSH                         \
    EARANGES_DIAGNOSTIC_IGNORE("-Wc++1z-extensions") \
    [[nodiscard]] EARANGES_DIAGNOSTIC_POP /**/
#else
#define EARANGES_NODISCARD [[nodiscard]]
#endif
#else
#define EARANGES_NODISCARD
#endif
#endif

#ifndef EARANGES_EMPTY_BASES
#ifdef _MSC_VER
#define EARANGES_EMPTY_BASES __declspec(empty_bases)
#else
#define EARANGES_EMPTY_BASES
#endif
#endif

#endif
