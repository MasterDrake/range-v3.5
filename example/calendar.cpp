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

#include <EARanges/detail/config.hpp>

#if EARANGES_CXX_RETURN_TYPE_DEDUCTION >= EARANGES_CXX_RETURN_TYPE_DEDUCTION_14 && \
    EARANGES_CXX_GENERIC_LAMBDAS >= EARANGES_CXX_GENERIC_LAMBDAS_14

///[calendar]

// Usage:
//     calendar 2015
//
// Output:
/*
        January              February                March
              1  2  3   1  2  3  4  5  6  7   1  2  3  4  5  6  7
  4  5  6  7  8  9 10   8  9 10 11 12 13 14   8  9 10 11 12 13 14
 11 12 13 14 15 16 17  15 16 17 18 19 20 21  15 16 17 18 19 20 21
 18 19 20 21 22 23 24  22 23 24 25 26 27 28  22 23 24 25 26 27 28
 25 26 27 28 29 30 31                        29 30 31

         April                  May                  June
           1  2  3  4                  1  2      1  2  3  4  5  6
  5  6  7  8  9 10 11   3  4  5  6  7  8  9   7  8  9 10 11 12 13
 12 13 14 15 16 17 18  10 11 12 13 14 15 16  14 15 16 17 18 19 20
 19 20 21 22 23 24 25  17 18 19 20 21 22 23  21 22 23 24 25 26 27
 26 27 28 29 30        24 25 26 27 28 29 30  28 29 30
                       31
         July                 August               September
           1  2  3  4                     1         1  2  3  4  5
  5  6  7  8  9 10 11   2  3  4  5  6  7  8   6  7  8  9 10 11 12
 12 13 14 15 16 17 18   9 10 11 12 13 14 15  13 14 15 16 17 18 19
 19 20 21 22 23 24 25  16 17 18 19 20 21 22  20 21 22 23 24 25 26
 26 27 28 29 30 31     23 24 25 26 27 28 29  27 28 29 30
                       30 31
        October              November              December
              1  2  3   1  2  3  4  5  6  7         1  2  3  4  5
  4  5  6  7  8  9 10   8  9 10 11 12 13 14   6  7  8  9 10 11 12
 11 12 13 14 15 16 17  15 16 17 18 19 20 21  13 14 15 16 17 18 19
 18 19 20 21 22 23 24  22 23 24 25 26 27 28  20 21 22 23 24 25 26
 25 26 27 28 29 30 31  29 30                 27 28 29 30 31
// */

// Credits:
//   Thanks to H. S. Teoh for the article that served as the
//     inspiration for this example:
//     <http://wiki.dlang.org/Component_programming_with_ranges>
//   Thanks to github's Arzar for bringing date::week_number
//     to my attention.

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <EASTL/algorithm.h>
#include <cstddef>
#include <EASTL/functional.h>
#include <iostream>
#include <EARanges/action/join.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/for_each.hpp>
#include <EARanges/algorithm/mismatch.hpp>
#include <EARanges/core.hpp>
#include <EARanges/iterator/stream_iterators.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/chunk.hpp>
#include <EARanges/view/chunk_by.hpp>
#include <EARanges/view/concat.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/join.hpp>
#include <EARanges/view/repeat_n.hpp>
#include <EARanges/view/single.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/view/transform.hpp>
#include <stdexcept>
#include <EASTL/string.h>
#include <EASTL/utility.h>
#include <EASTL/vector.h>

#if _WIN32
#include "Windows.h"
#endif

namespace po = boost::program_options;
namespace greg = boost::gregorian;
using date = greg::date;
using day = greg::date_duration;
using namespace ranges;

#include "../test/eastl_utils.h"

namespace std
{
    std::ostream & operator<<(std::ostream & os, eastl::string const & str)
    {
        return os << str.c_str();
    }

    std::istream& operator>>(std::istream & is, eastl::string & str)
    {
        str.clear();

        char ch;
        while(is.get(ch) && !std::isspace(ch))
            str.push_back(ch);

        if(is)
            is.unget();

        return is;
    }

    template<typename T>
    std::ostream & operator<<(std::ostream & os, eastl::vector<T> const & vec)
    {
        for(const auto & elem : vec)
            os << elem;
        return os;
    }
} // namespace std

namespace boost
{
    namespace gregorian
    {
        date& operator++(date &d)
        {
            return d = d + day(1);
        }
        date operator++(date &d, int)
        {
            return ++d - day(1);
        }
    }
}
namespace ranges
{
    template<>
    struct incrementable_traits<date>
    {
        using difference_type = date::duration_type::duration_rep::int_type;
    };
}
CPP_assert(incrementable<date>);

auto dates(unsigned short start, unsigned short stop)
{
    return views::iota(date{start, greg::Jan, 1}, date{stop, greg::Jan, 1});
}

auto dates_from(unsigned short year)
{
    return views::iota(date{year, greg::Jan, 1});
}

auto by_month()
{
    return views::chunk_by([](date a, date b) { return a.month() == b.month(); });
}

auto by_week()
{
    return views::chunk_by([](date a, date b) {
        // ++a because week_number is Mon-Sun and we want Sun-Sat
        return (++a).week_number() == (++b).week_number();
    });
}

eastl::string format_day(date d)
{
    return boost::str(boost::format("%|3|") % d.day()).c_str();
}

// In:  range<range<date>>: month grouped by weeks.
// Out: range<eastl::string>: month with formatted weeks.
auto format_weeks()
{
    return views::transform([](/*range<date>*/ auto week)
    {
        return boost::str(boost::format("%1%%2%%|22t|") %
                          eastl::string(front(week).day_of_week() * 3u, ' ') %
                          (week | views::transform(format_day) | actions::join)).c_str();
    });
}

// Return a formatted string with the title of the month
// corresponding to a date.
eastl::string month_title(date d)
{
    return boost::str(boost::format("%|=22|") % d.month().as_long_string()).c_str();
}

// In:  range<range<date>>: year of months of days
// Out: range<range<eastl::string>>: year of months of formatted wks
auto layout_months()
{
    return views::transform([](/*range<date>*/ auto month)
    {
        const auto week_count = static_cast<std::ptrdiff_t>(distance(month | by_week()));
        return views::concat(
            views::single(month_title(front(month))),
            month | by_week() | format_weeks(),
            views::repeat_n(eastl::string(22, ' '), 6 - week_count));
    });
}

// Flattens a range of ranges by iterating the inner
// ranges in round-robin fashion.
template<class Rngs>
class interleave_view : public view_facade<interleave_view<Rngs>>
{
    friend range_access;
    eastl::vector<range_value_t<Rngs>> rngs_;
    struct cursor;
    cursor begin_cursor()
    {
        return {0, &rngs_, views::transform(rngs_, ranges::begin) | to<eastl::vector>};
    }

public:
    interleave_view() = default;
    explicit interleave_view(Rngs rngs) : rngs_(eastl::move(rngs) | to<eastl::vector>)
    {}
};

template<class Rngs>
struct interleave_view<Rngs>::cursor
{
    std::size_t n_;
    eastl::vector<range_value_t<Rngs>> *rngs_;
    eastl::vector<iterator_t<range_value_t<Rngs>>> its_;

    decltype(auto) read() const
    {
        return *its_[n_];
    }

    void next()
    {
        if(0 == ((++n_) %= its_.size()))
            for_each(its_, [](auto &it) { ++it; });
    }

    bool equal(default_sentinel_t) const
    {
        if(n_ != 0)
            return false;
        auto ends = *rngs_ | views::transform(ranges::end);
        return its_.end() != eastl::mismatch(its_.begin(), its_.end(), ends.begin(), eastl::not_equal_to<>{}).first;
    }

    CPP_member
    auto equal(cursor const& that) const -> CPP_ret(bool)(requires forward_range<range_value_t<Rngs>>)
    {
        return n_ == that.n_ && its_ == that.its_;
    }
};

// In:  range<range<T>>
// Out: range<T>, flattened by walking the ranges round-robin fashion.
auto interleave()
{
    return make_view_closure([](auto &&rngs)
    {
        using Rngs = decltype(rngs);
        return interleave_view<views::all_t<Rngs>>(views::all(eastl::forward<Rngs>(rngs)));
    });
}

// In:  range<range<T>>
// Out: range<range<T>>, transposing the rows and columns.
auto transpose()
{
    return make_view_closure([](auto &&rngs)
    {
        using Rngs = decltype(rngs);
        CPP_assert(forward_range<Rngs>);
        return eastl::forward<Rngs>(rngs)
            | interleave()
            | views::chunk(static_cast<std::size_t>(distance(rngs)));
    });
}

// In:  range<range<range<string>>>
// Out: range<range<range<string>>>, transposing months.
auto transpose_months()
{
    return views::transform([](/*range<range<string>>*/ auto rng) { return rng | transpose(); });
}

// In:  range<range<string>>
// Out: range<string>, joining the strings of the inner ranges
auto join_months()
{
    return views::transform([](/*range<string>*/ auto rng) { return actions::join(rng); });
}

// In:  range<date>
// Out: range<string>, lines of formatted output
auto format_calendar(std::size_t months_per_line)
{
    return
        // Group the dates by month:
        by_month()
        // Format the month into a range of strings:
      | layout_months()
        // Group the months that belong side-by-side:
      | views::chunk(months_per_line)
        // Transpose the rows and columns of the size-by-side months:
      | transpose_months()
        // Ungroup the side-by-side months:
      | views::join
        // Join the strings of the transposed months:
      | join_months();
}

int main(int argc, char *argv[]) try
{
#if _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
        "start", po::value<unsigned short>(), "Year to start")(
        "stop", po::value<eastl::string>(), "Year to stop")(
        "per-line",
        po::value<std::size_t>()->default_value(3u),
        "Nbr of months per line");

    po::positional_options_description p;
    p.add("start", 1).add("stop", 1);

    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv).options(desc).positional(p).run(),
        vm);
    po::notify(vm);

    if(vm.count("help") || 1 != vm.count("start"))
    {
        std::cerr << desc << '\n';
        return 1;
    }

    auto const start = vm["start"].as<unsigned short>();
    auto const stop = 0 == vm.count("stop")
                          ? (unsigned short)(start + 1)
                          : vm["stop"].as<eastl::string>() == "never"
                                ? (unsigned short)-1
                                : boost::lexical_cast<unsigned short>(vm["stop"].as<eastl::string>());
    auto const months_per_line = vm["per-line"].as<std::size_t>();

    if(stop != (unsigned short)-1 && stop <= start)
    {
        std::cerr << "ERROR: The stop year must be larger than the start" << '\n';
        return 1;
    }

    if((unsigned short)-1 != stop)
        copy(dates(start, stop) | format_calendar(months_per_line), ostream_iterator<>(std::cout, "\n"));
    else
        copy(dates_from(start) | format_calendar(months_per_line), ostream_iterator<>(std::cout, "\n"));
}
catch(std::exception &e)
{
    std::cerr << "ERROR: Unhandled exception\n";
    std::cerr << "  what(): " << e.what();
    return 1;
}
///[calendar]

#else
#pragma message( \
    "calendar requires C++14 return type deduction and generic lambdas")
int
main()
{}
#endif
