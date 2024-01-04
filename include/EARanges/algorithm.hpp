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

#ifndef EARANGES_ALGORITHM_HPP
#define EARANGES_ALGORITHM_HPP

#include <EARanges/algorithm/adjacent_find.hpp>
#include <EARanges/algorithm/adjacent_remove_if.hpp>
#include <EARanges/algorithm/all_of.hpp>
#include <EARanges/algorithm/any_of.hpp>
#include <EARanges/algorithm/binary_search.hpp>
#include <EARanges/algorithm/contains.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/copy_backward.hpp>
#include <EARanges/algorithm/copy_if.hpp>
#include <EARanges/algorithm/copy_n.hpp>
#include <EARanges/algorithm/count.hpp>
#include <EARanges/algorithm/count_if.hpp>
#include <EARanges/algorithm/ends_with.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/algorithm/equal_range.hpp>
#include <EARanges/algorithm/fill.hpp>
#include <EARanges/algorithm/fill_n.hpp>
#include <EARanges/algorithm/find.hpp>
#include <EARanges/algorithm/find_end.hpp>
#include <EARanges/algorithm/find_first_of.hpp>
#include <EARanges/algorithm/find_if.hpp>
#include <EARanges/algorithm/find_if_not.hpp>
#include <EARanges/algorithm/fold.hpp>
#include <EARanges/algorithm/for_each.hpp>
#include <EARanges/algorithm/for_each_n.hpp>
#include <EARanges/algorithm/generate.hpp>
#include <EARanges/algorithm/generate_n.hpp>
#include <EARanges/algorithm/heap_algorithm.hpp>
#include <EARanges/algorithm/inplace_merge.hpp>
#include <EARanges/algorithm/is_partitioned.hpp>
#include <EARanges/algorithm/is_sorted.hpp>
#include <EARanges/algorithm/is_sorted_until.hpp>
#include <EARanges/algorithm/lexicographical_compare.hpp>
#include <EARanges/algorithm/lower_bound.hpp>
#include <EARanges/algorithm/max.hpp>
#include <EARanges/algorithm/max_element.hpp>
#include <EARanges/algorithm/merge.hpp>
#include <EARanges/algorithm/min.hpp>
#include <EARanges/algorithm/min_element.hpp>
#include <EARanges/algorithm/minmax.hpp>
#include <EARanges/algorithm/minmax_element.hpp>
#include <EARanges/algorithm/mismatch.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/algorithm/move_backward.hpp>
#include <EARanges/algorithm/none_of.hpp>
#include <EARanges/algorithm/nth_element.hpp>
#include <EARanges/algorithm/partial_sort.hpp>
#include <EARanges/algorithm/partial_sort_copy.hpp>
#include <EARanges/algorithm/partition.hpp>
#include <EARanges/algorithm/partition_copy.hpp>
#include <EARanges/algorithm/partition_point.hpp>
#include <EARanges/algorithm/permutation.hpp>
#include <EARanges/algorithm/remove.hpp>
#include <EARanges/algorithm/remove_copy.hpp>
#include <EARanges/algorithm/remove_copy_if.hpp>
#include <EARanges/algorithm/remove_if.hpp>
#include <EARanges/algorithm/replace.hpp>
#include <EARanges/algorithm/replace_copy.hpp>
#include <EARanges/algorithm/replace_copy_if.hpp>
#include <EARanges/algorithm/replace_if.hpp>
#include <EARanges/algorithm/reverse.hpp>
#include <EARanges/algorithm/reverse_copy.hpp>
#include <EARanges/algorithm/rotate.hpp>
#include <EARanges/algorithm/rotate_copy.hpp>
#include <EARanges/algorithm/sample.hpp>
#include <EARanges/algorithm/search.hpp>
#include <EARanges/algorithm/search_n.hpp>
#include <EARanges/algorithm/set_algorithm.hpp>
#include <EARanges/algorithm/shuffle.hpp>
#include <EARanges/algorithm/sort.hpp>
#include <EARanges/algorithm/stable_partition.hpp>
#include <EARanges/algorithm/stable_sort.hpp>
#include <EARanges/algorithm/starts_with.hpp>
#include <EARanges/algorithm/swap_ranges.hpp>
#include <EARanges/algorithm/transform.hpp>
#include <EARanges/algorithm/unique.hpp>
#include <EARanges/algorithm/unique_copy.hpp>
#include <EARanges/algorithm/unstable_remove_if.hpp>
#include <EARanges/algorithm/upper_bound.hpp>
#include <EARanges/detail/config.hpp>

// BUGBUG
#include <EARanges/algorithm/aux_/equal_range_n.hpp>
#include <EARanges/algorithm/aux_/lower_bound_n.hpp>
#include <EARanges/algorithm/aux_/merge_n.hpp>
#include <EARanges/algorithm/aux_/merge_n_with_buffer.hpp>
#include <EARanges/algorithm/aux_/sort_n_with_buffer.hpp>
#include <EARanges/algorithm/aux_/upper_bound_n.hpp>

#endif