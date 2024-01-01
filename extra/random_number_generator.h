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