
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/algorithm/aux_/partition_point_n.hpp>

namespace eastl
{
	template <typename ForwardIterator, typename ForwardSentinel, typename Compare, typename Projection>
	EA_CONSTEXPR ForwardIterator min_element(ForwardIterator first, ForwardSentinel last, Compare compare, Projection proj)
	{
		if (first != last)
			for (auto tmp = ranges::next(first); tmp != last; ++tmp)
				if (ranges::invoke(compare, ranges::invoke(proj, *tmp), ranges::invoke(proj, *first)))
					first = tmp;
		return first;
	}
	
	template <typename ForwardIterator, typename ForwardSentinel, typename Compare, typename Projection>
	EA_CONSTEXPR ForwardIterator max_element(ForwardIterator first, ForwardSentinel last, Compare compare, Projection proj)
	{
		if (first != last)
			for (auto tmp = ranges::next(first); tmp != last; ++tmp)
				if (ranges::invoke(compare, ranges::invoke(proj, *first), ranges::invoke(proj, *tmp)))
					first = tmp;
		return first;
	}
	template <typename InputIterator, typename Function, typename Projection>
	EA_CPP14_CONSTEXPR inline InputIterator
	for_each_n(InputIterator first, ranges::iter_difference_t<InputIterator> n, Function fun, Projection proj)
	{
		EARANGES_EXPECT(0 <= n);
		auto norig = n;
		auto b = ranges::uncounted(first);
		for (; 0 < n; ++b, --n)
			ranges::invoke(fun, ranges::invoke(proj, *b));
		return ranges::recounted(first, b, norig);
	}
	
	template <typename OutputIterator, typename Generator>
	EA_CONSTEXPR inline ranges::detail::out_fun_result<OutputIterator, Generator>
	generate_n(OutputIterator first, ranges::iter_difference_t<OutputIterator> n, Generator gen)
	{
		EARANGES_EXPECT(n >= 0);
		auto norig = n;
		auto b = ranges::uncounted(first);
		for (; 0 != n; ++b, --n)
			*b = ranges::invoke(gen);
		return {ranges::recounted(first, b, norig), ranges::detail::move(gen)};
	}
	
	template <typename BidirectionalIterator, typename BidirectionalSentinel>
	EA_CONSTEXPR inline BidirectionalIterator reverse(BidirectionalIterator first, BidirectionalSentinel end_)
	{
		BidirectionalIterator last = ranges::next(first, end_);
		typedef typename eastl::iterator_traits<BidirectionalIterator>::iterator_category IC;
		//TODO:It should work ... eastl::reverse_impl(first, last, ranges::iterator_tag_of<BidirectionalIterator>{});

		eastl::reverse_impl(first, last, IC{});
		return last;
	}
	
	template <typename BidirectionalIterator, typename BidirectionalSentinel, typename OutputIterator>
	EA_CONSTEXPR inline ranges::detail::in_out_result<BidirectionalIterator, OutputIterator>
	reverse_copy(BidirectionalIterator first, BidirectionalSentinel end, OutputIterator out)
	{
		BidirectionalIterator last = ranges::next(first, end), res = last;
		for (; first != last; ++out)
			*out = *--last;
		return {res, out};
	}
	
	template <typename ForwardIterator, typename ForwardSentinel, typename OutputIterator, typename Projection>
	EA_CONSTEXPR ranges::detail::in_out_result<ForwardIterator, OutputIterator> rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardSentinel last, OutputIterator out)
	{
		auto res = ranges::copy(middle, eastl::move(last), eastl::move(out));
		return {eastl::move(res.in), ranges::copy(eastl::move(first), middle, eastl::move(res.out)).out};
	}
	
	template <typename ForwardIterator, typename ForwardSentinel, typename Predicate, typename Projection>
	EA_CONSTEXPR ForwardIterator unique(ForwardIterator first, ForwardSentinel last, Predicate pred, Projection proj)
	{
		first = adjacent_find(eastl::move(first), last, ranges::ref(pred), ranges::ref(proj));

		if (first != last)
		{
			for (ForwardIterator i = ranges::next(first); ++i != last;)
				if (!ranges::invoke(pred, ranges::invoke(proj, *first), ranges::invoke(proj, *i)))
					*++first = ranges::iter_move(i);
			++first;
		}
		return first;
	}
	template <class ForwardIterator, class ForwardSentinel, class UnaryPredicate, class Projection>
	EA_CONSTEXPR ForwardIterator partition_point(ForwardIterator first, ForwardSentinel last, UnaryPredicate pred, Projection proj)
	{
		if (EARANGES_CONSTEXPR_IF(ranges::sized_sentinel_for<ForwardSentinel, ForwardIterator>))
		{
			auto len = ranges::distance(first, eastl::move(last));
			return ranges::aux::partition_point_n(eastl::move(first), len, eastl::move(pred), eastl::move(proj));
		}

		// Probe exponentially for either last-of-range or an iterator that is past the partition point (i.e., does not satisfy pred).
		auto len = ranges::iter_difference_t<ForwardIterator>{1};
		while (true)
		{
			auto mid = first;
			auto d = ranges::advance(mid, len, last);
			if (mid == last || !ranges::invoke(pred, ranges::invoke(proj, *mid)))
			{
				len -= d;
				return ranges::aux::partition_point_n(eastl::move(first), len, ranges::ref(pred), ranges::ref(proj));
			}
			first = eastl::move(mid);
			len *= 2;
		}	
	}
	
}