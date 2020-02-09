// grand.h
// VERSION 1.1.5
// Glenn G. Chappell
// 5 May 2015
//
// Header for class GRand
// Allows easy pseudorandom number generation
// There is no associated source file.
// Requires C++11 or later.
//
// Convenience wrapper around C++ Standard Library RNG.
// Intended for simple pseudorandom number generation, or as sample
// code. Not for cryptographic use.

// Copyright (c) 2014-2015 Glenn G. Chappell
// License: MIT. See LICENSE or <http://opensource.org/licenses/MIT>.

/*

EXAMPLE USAGE

  GRand r;      // RNG object; seeded with unpredictable value
  GRand r2(7);  // Separate RNG; fixed seed gives predictable sequence
  r2.seed(2);   // Another way to seed

  // Print 5 random values, each in { 0, 1, 2, ..., 99 }
  for (int k = 0; k < 5; ++k)
      cout << r.i(100) << endl;

  // Flip a coin
  if (r.b())
     cout << "HEADS" << endl;
  else
     cout << "tails" << endl;

  bool b75 = r.b(0.75);  // bool with 75% chance of being true
  double d1 = r.d();     // double in [0.0,1.0), uniformly distributed
  double d3 = r.d(3.0);  // double in [0.0,3.0), uniformly distributed

  // Randomly shuffle a dataset (v is a vector, deque, or array)
  std::shuffle(v.begin(), v.end(), r);

*/

#ifndef FILE_GRAND_H_INCLUDED
#define FILE_GRAND_H_INCLUDED

#include <random>
// For:
//   std::mt19937
//   std::random_device
//   std::uniform_int_distribution
//   std::uniform_real_distribution
//   std::bernoulli_distribution
#include <type_traits>
// For:
//   std::common_type
#include <limits>
// For:
//   std::numeric_limits

// Version number
// Guaranteed to increase with with each release.
#define GRAND_PACKAGE_VERSION (10105)  // 1 01 05 means 1.1.5

// Handle varying support for constexpr
// As of Visual C++ 2013, no version of VC++ supports constexpr.
// Preprocessor constant GRAND_CONSTEXPR is defined to be "" in Visual
// C++ and "constexpr" in other compilers. This constant is for internal
// use only; it is #undef'd at the end of this file.
#ifndef _MSC_VER
# define GRAND_CONSTEXPR constexpr
#else
# define GRAND_CONSTEXPR
#endif

// class GRand
// Good random number generation
// Convenience wrapper around C++ Standard Library RNG.
// Exceptions:
//     Public member functions that take arguments of arbitrary types
//      (ctor from seed, seed [one parameter], operator() [one
//      parameter]) will throw what & when an operation on the argument
//      type throws. No such exceptions will be thrown if the argument
//      is of a fundamental type.
//     If the GRand object has been seeded unpredictably (default ctor,
//      seed [no parameters]), then public member functions that
//      generate random values (i, d, b, operator()) may throw the
//      exception thrown by std::random_device on failure. No such
//      exceptions will be thrown if the GRand object has not been
//      seeded unpredictably.
//     In no other cases will any member function throw.
//     Whenever an exception is thrown, the Strong Guarantee holds, as
//      long as it holds for the type that originated the exception.
class GRand {

// ***** GRand: internal-use types *****
private:

    typedef std::mt19937 rng_type;  // Type of our RNG object

// ***** GRand: ctors, dctor, op= *****
public:

    // Default ctor
    // Seed RNG with unpredictable value.
    GRand()
        :_seed_needed(true),
         _rng()
    {}

    // Ctor from seed (number)
    // Seed RNG with given value.
    // Template so that any reasonable argument type gives no warnings.
    // Requirements on Types:
    //     SeedType must be copy constructible.
    //     SeedType must be convertible to rng_type::result_type
    //      (guaranteed to be an unsigned integral type).
    template <typename SeedType>
    explicit GRand(SeedType s)
        :_seed_needed(false),
         _rng(rng_type::result_type(s))
    {}

    // Compiler-generated copy/move ctors, copy/move op=, dctor used.

// ***** GRand: general public functions *****
public:

    // seed - no parameters
    // Seed RNG with unpredictable value.
    void seed()
    { _seed_needed = true; }

    // seed from number
    // Seed RNG with given value.
    // Template so that any reasonable argument type gives no warnings.
    // Requirements on Types:
    //     SeedType must be copy constructible.
    //     SeedType must be convertible to rng_type::result_type
    //      (guaranteed to be an unsigned integral type).
    template <typename SeedType>
    void seed(SeedType s)
    {
        _seed_needed = false;
        _rng.seed(rng_type::result_type(s));
    }

    // i
    // Return uniformly distributed random integer in range [0, n-1], or
    //  0 if n <= 0. Range is {0, 1} if no parameter given.
    int i(int n=2)
    { return (n <= 1) ? 0 : rand_integer(n); }

    // d
    // Return uniformly distributed random double in range [0.0, d) if
    //  d > 0.0, in range (d, 0.0] if d < 0.0, or 0.0 if d == 0.0. Range
    //  is [0.0, 1.0) if no parameter given.
    double d(double d=1.0)
    {
        ck_seed();
        if (d > 0.0)
            return std::uniform_real_distribution<double>(0.0, d)(_rng);
        if (d < 0.0)
            return
                -std::uniform_real_distribution<double>(0.0, -d)(_rng);
        return 0.0;
    }

    // b
    // Return random bool value. Probability of true is given value, or
    //  0.5 if none given.
    bool b(double p=0.5)
    {
        ck_seed();
        if (p <= 0.0) return false;
        if (p >= 1.0) return true;
        return std::bernoulli_distribution(p)(_rng);
    }

// ***** GRand: support for C++ Standard concepts *****
public:

    // An object of type GRand meets the requirements for Uniform Random
    // Number Generator (C++11 26.5.1.3). An object of type GRand may be
    // passed as the third argument to std::shuffle (C++11 25.3.12) --
    // as long as result_type is convertible to
    // iterator_traits<RandomAccessIterator>::difference_type, where
    // RandomAccessIterator is the type of the first two arguments of
    // std::shuffle (i.e., pretty much always).

    // Type result_type
    // Return type of operator() - no parameters.
    typedef rng_type::result_type result_type;

    // min & max: The C++ Standard Library shipped with Xcode 6.3
    // requires these member functions to be constexpr in a Uniform
    // Random Number Generator object. The C++11 Standard does not
    // require this, but it does not hurt -- except that Visual C++
    // currently does not support constexpr at all. So we do a bit of
    // system-dependent preprocessor stuff to make it all work.

    // min
    // Minimum return value of operator() - no parameters.
    //static GRAND_CONSTEXPR result_type min()
    //{ return rng_type::min(); }

    // max
    // Maximum return value of operator() - no parameters.
    //static GRAND_CONSTEXPR result_type max()
    //{ return rng_type::max(); }

    // operator() - no arguments
    // Return uniformly distributed random value of type result_type, in
    //  range [min(), max()].
    result_type operator()()
    {
        ck_seed();
        return _rng();
    }

    // An object of type GRand may be passed as the third argument to
    // std::random_shuffle (C++11 25.3.12) -- as long as
    // iterator_traits<RandomAccessIterator>::difference_type and
    // unsigned long long are each convertible to the other, where
    // RandomAccessIterator is the type of the first two arguments of
    // std::random_shuffle (i.e., pretty much always).

    // operator() - one argument
    // Return random integer in range [0, n-1], or 0 if n <= 0. Result
    //  is uniformly distributed if n <= ULLONG_MAX.
    // Requirements on Types:
    //     IntType must be copy constructible.
    //     IntType must be convertible to unsigned long long.
    //     unsigned long long must be convertible to IntType.
    template <typename IntType>
    IntType operator()(IntType n)
    {
        if (n <= IntType(1ULL)) return IntType(0ULL);
        // "ULL" in above line because of our requirements on types.
        typedef typename
            std::common_type<IntType, unsigned long long>::type TT;
        // Explicit type promotion avoids signed/unsigned warnings.
        if (TT(n) <= TT(std::numeric_limits<unsigned long long>::max()))
            // Above is correct for signed or unsigned n, since n > 0.
            return IntType(rand_integer((unsigned long long)(n)));
        else
            // If n is too big for (unsigned long long), we do the best
            //  we can.
            return IntType(rand_integer(
                std::numeric_limits<unsigned long long>::max()));
    }
    // Simpler non-template versions of operator()(n) for the 8
    // fundamental types that rand_integer can take.
#define GRAND_OP_PAREN(T) T operator()(T n) \
    { return (n <= (T)(1)) ? (T)(0) : rand_integer(n); }
        // GRAND_OP_PAREN is internal-use only; #undef'd at end of file
    GRAND_OP_PAREN(short)
    GRAND_OP_PAREN(int)
    GRAND_OP_PAREN(long)
    GRAND_OP_PAREN(long long)
    GRAND_OP_PAREN(unsigned short)
    GRAND_OP_PAREN(unsigned int)
    GRAND_OP_PAREN(unsigned long)
    GRAND_OP_PAREN(unsigned long long)

    // NOTE: An object of type GRand does NOT meet the requirements for
    // Random Number Engine (C++11 26.5.1.4).

// ***** GRand: internal-use functions *****
private:

    // rand_integer
    // Return uniformly distributed random integer in range [0, n-1].
    // Pre:
    //     n >= 2.
    // Requirements on Types:
    //     IntType must be short, int, long, long long, or the unsigned
    //      version of one of these (to avoid undefined behavior from
    //      std::uniform_int_distribution).
    // This function is used in both i(n) and operator()(n). We avoid
    // the n <= 1 check here (thus the precondition) so that we can
    // handle negative n correctly.
    template <typename IntType>
    IntType rand_integer(IntType n)
    {
        ck_seed();
#ifdef __GNUC__
// g++ 4.7.* incorrectly gives conversion warnings when IntType is short
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
        return std::uniform_int_distribution<IntType>(
            IntType(0),
            n-IntType(1))
                (_rng);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    }

    // ck_seed
    // If _seed_needed is true, then do nondeterministic seed and set
    // _seed_needed to false; otherwise do nothing.
    //
    // This function exists to allow a default-constructed GRand object
    // to be given a specified seed later, without nondeterministic
    // seeding being done in the meantime. Thus we avoid crashes in
    // environments where nondeterministic seeding is not available
    // (e.g., O. Lawlor's NetRun).
    void ck_seed()
    {
        if (!_seed_needed)
            return;
        _rng.seed(rng_type::result_type(std::random_device()()));
        _seed_needed = false;
    }

// ***** GRand: data members *****
private:

    bool _seed_needed;
                    // True if seed needed; nondeterministic seeding is
                    //  done if no seed given before output requested
    rng_type _rng;  // Our random-number generator object

};  // End class GRand

// Undefine internal-use-only preprocessor symbols
#undef GRAND_OP_PAREN
#undef GRAND_CONSTEXPR

#endif //#ifndef FILE_GRAND_H_INCLUDED

