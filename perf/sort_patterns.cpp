// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <EARanges/detail/config.hpp>

#if EARANGES_CXX_RETURN_TYPE_DEDUCTION >= EARANGES_CXX_RETURN_TYPE_DEDUCTION_14 && \
    EARANGES_CXX_GENERIC_LAMBDAS >= EARANGES_CXX_GENERIC_LAMBDAS_14

#include <iostream>
#include <iomanip>
#include <EASTL/vector.h>
#include <random>
#include <EASTL/functional.h>
#include <climits>
#include <EASTL/chrono.h>
//#include <EASTL/algorithm.h>
#include <EASTL/sort.h>
#include <EARanges/all.hpp>

EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

#include "../test/eastl_utils.h"

namespace
{
  /// Creates an geometric infinite sequence starting at 1 where the
  /// successor is multiplied by \p V
  auto geometric_sequence(eastl::size_t V)
  {
    eastl::size_t N = 1;
    return ranges::views::generate([N, V]() mutable
    {
      auto old = N;
      N *= V;
      return old;
    });
  }

  /// Creates an geometric infinite sequence starting at 1 where the
  /// successor is multiplied by \p V
  auto geometric_sequence_n(eastl::size_t V, eastl::size_t limit)
  {
    return geometric_sequence(V) | ranges::views::take_while([limit](eastl::size_t n) { return n <= limit; });
  }

  /// Random uniform integer sequence
  struct random_uniform_integer_sequence
  {
    std::default_random_engine gen;
    std::uniform_int_distribution<> dist;
    auto operator()(eastl::size_t)
    {
      return ranges::views::generate([&]{ return dist(gen); });
    }
    static eastl::string name() { return "random_uniform_integer_sequence"; }
  };

  struct ascending_integer_sequence
  {
    auto operator()(eastl::size_t) { return ranges::views::iota(1); }
    static eastl::string name() { return "ascending_integer_sequence"; }
  };

  struct descending_integer_sequence
  {
    auto operator()(eastl::size_t)
    {
      return ranges::views::iota(0ll, eastl::numeric_limits<long long>::max()) | ranges::views::reverse;
    }
    static eastl::string name() { return "descending_integer_sequence"; }
  };

  auto even = [](auto i) { return i % 2 == 0; };
  auto odd = [](auto i) { return !even(i); };

  struct even_odd_integer_sequence
  {
    static eastl::string name() { return "even_odd_integer_sequence"; }
    auto operator()(eastl::size_t n)
    {
      return ranges::views::concat(ranges::views::ints(eastl::size_t{0}, n) | ranges::views::filter(even),
                                  ranges::views::ints(eastl::size_t{0}, n) | ranges::views::filter(odd));
    }
  };

  struct organ_pipe_integer_sequence
  {
    static eastl::string name() { return "organ_pipe_integer_sequence"; }
    auto operator()(eastl::size_t n) 
    {
      return ranges::views::concat(ranges::views::ints(eastl::size_t{0}, n/2),
                                  ranges::views::ints(eastl::size_t{0}, n/2 + 1)
                                  | ranges::views::reverse);
    }
  };

  template<typename Seq>
  void print(Seq seq, eastl::size_t n)
  {
    std::cout << "sequence: " << seq.name().c_str() << '\n';
    EARANGES_FOR(auto i , seq(n) | ranges::views::take(n))
      std::cout << i << '\n';
  }

  /// Returns the duration of a computation
  using clock_t = eastl::chrono::high_resolution_clock;
  using duration_t = clock_t::duration;

  template<typename Computation>
  auto duration(Computation &&c)
  {
    auto time = []{ return clock_t::now(); };
    const auto start = time();
    c();
    return time() - start;
  }

  template<typename Duration>
  auto to_millis(Duration d)
  {
    return eastl::chrono::duration_cast<eastl::chrono::milliseconds>(d).count();
  }

  template<typename Durations> auto compute_mean(Durations &&durations)
  {
    using D = ranges::range_value_t<Durations>;
    D total = ranges::accumulate(durations, D{}, ranges::plus{}, ranges::convert_to<D>{});
    return total / ranges::size(durations);
  }

  template<typename Durations> auto compute_stddev(Durations &&durations)
  {
    using D = ranges::range_value_t<Durations>;
    using Rep = typename D::rep;
    const auto mean = compute_mean(durations);
    const auto stddev = ranges::accumulate(
      durations | ranges::views::transform([=](auto i)
      {
        auto const delta = (i - mean).count();
        return delta * delta;
      }), Rep{}, ranges::plus{}, ranges::convert_to<Rep>{});
    return D{static_cast<typename D::rep>(std::sqrt(stddev / ranges::size(durations)))};
  }

  struct benchmark
  {
    struct result_t
    {
      duration_t mean_t;
      duration_t max_t;
      duration_t min_t;
      eastl::size_t size;
      duration_t deviation;
    };
    eastl::vector<result_t> results;

    template<typename Computation, typename Sizes>
    benchmark(Computation &&c, Sizes &&sizes, double target_deviation = 0.25, eastl::size_t max_iters = 100, eastl::size_t min_iters = 5)
    {
      EARANGES_FOR(auto size , sizes)
      {
        eastl::vector<duration_t> durations;
        duration_t deviation;
        duration_t mean_duration;
        eastl::size_t iter;

        for (iter = 0; iter < max_iters; ++iter)
        {
          c.init(size);
          durations.emplace_back(duration(c));
          mean_duration = compute_mean(durations);
          if (++iter == max_iters)
            break;
          if (iter >= min_iters)
          {
            deviation = compute_stddev(durations);
            if (deviation < target_deviation * mean_duration)
              break;
          }
        }
        auto minmax = ranges::minmax(durations);
        results.emplace_back(result_t{mean_duration, minmax.max, minmax.min, size, deviation});
        std::cerr << "size: " << size << " iter: " << iter
                  << " dev: " << to_millis(deviation)
                  << " mean: " << to_millis(mean_duration)
                  << " max: " << to_millis(minmax.max)
                  << " min: " << to_millis(minmax.min) << '\n';
      }
    }
  };

  template<typename Seq, typename Comp>
  struct computation_on_sequence
  {
    Seq seq;
    Comp comp;
    eastl::vector<ranges::range_value_t<decltype(seq(eastl::size_t{}))>> data;
    computation_on_sequence(Seq s, Comp c, eastl::size_t max_size) : seq(eastl::move(s)), comp(eastl::move(c))
    {
      data.reserve(max_size);
    }
    void init(eastl::size_t size)
    {
      data.resize(size);
      ranges::copy(seq(size) | ranges::views::take(size), ranges::begin(data));
    }
    void operator()() { comp(data); }
  };

  template<typename Seq, typename Comp>
  auto make_computation_on_sequence(Seq s, Comp c, eastl::size_t max_size)
  {
    return computation_on_sequence<Seq, Comp>(eastl::move(s), eastl::move(c), max_size);
  }

  template<typename Seq> void benchmark_sort(Seq &&seq, eastl::size_t max_size)
  {
    auto ranges_sort_comp = make_computation_on_sequence(seq, ranges::sort, max_size);

    auto eastl_sort_comp = make_computation_on_sequence(seq, [](auto &&v) { eastl::sort(eastl::begin(v), eastl::end(v)); }, max_size);

    auto ranges_sort_benchmark = benchmark(ranges_sort_comp, geometric_sequence_n(2, max_size));

    auto eastl_sort_benchmark = benchmark(eastl_sort_comp, geometric_sequence_n(2, max_size));

    using std::setw;
    std::cout << '#'
              << "pattern: " << seq.name().c_str() << '\n';
    std::cout << '#' << setw(19) << 'N' << setw(20) << "ranges::sort" << setw(20)
              << "eastl::sort"
              << '\n';

    for(auto p : ranges::views::zip(ranges_sort_benchmark.results, eastl_sort_benchmark.results))
    {
      auto rs = p.first;
      auto ss = p.second;

      std::cout << setw(20) << rs.size << setw(20) << to_millis(rs.mean_t) << setw(20) << to_millis(ss.mean_t) << '\n';
    }
  }
} // unnamed namespace

int main()
{
  constexpr eastl::size_t max_size = 2000000;

  print(random_uniform_integer_sequence(), 20);
  print(ascending_integer_sequence(), 20);
  print(descending_integer_sequence(), 20);
  print(even_odd_integer_sequence(), 20);
  print(organ_pipe_integer_sequence(), 20);

  benchmark_sort(random_uniform_integer_sequence(), max_size);
  benchmark_sort(ascending_integer_sequence(), max_size);
  benchmark_sort(descending_integer_sequence(), max_size);
  benchmark_sort(organ_pipe_integer_sequence(), max_size);
}

#else

#pragma message("sort_patterns requires C++14 return type deduction and generic lambdas")

int main() {}

#endif
