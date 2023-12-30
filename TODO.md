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
  - Aggiungere EARanges Extra per random e addressof :/: