
#include "pcg_random.h"

namespace Ign
{

static const uint64_t DEF_STATE = 0x853c49e6748fea9bULL;
static const uint64_t DEF_SEQ   = 0xda3e39cb94b95bdbULL;

PcgRandom::PcgRandom()
{ 
    const uint64_t state = DEF_STATE;
    const uint64_t seq   = DEF_SEQ;
    state_ = 0U;
    inc_ = (seq << 1u) | 1u;
    uint();
    state_ += state;
    uint();
}

PcgRandom::~PcgRandom()
{
}

PcgRandom::PcgRandom( const PcgRandom & inst )
{
    *this = inst;
}

const PcgRandom & PcgRandom::operator=( const PcgRandom & inst )
{
    if ( this != &inst )
    {
        state_ = inst.state_;
        inc_   = inst.inc_;
    }
    return *this;
}

void PcgRandom::seed( uint64_t s )
{
    const uint64_t def_state = DEF_STATE;
    state_ = (def_state << 16) ^ s;
}

uint32_t PcgRandom::uint()
{
    const uint64_t oldstate = state_;
    // Advance internal state
    state_ = oldstate * 6364136223846793005ULL + inc_;
    // Calculate output function (XSH RR), uses old state for max ILP
    const uint32_t xorshifted = static_cast<uint32_t>( ((oldstate >> 18u) ^ oldstate) >> 27u );
    const uint32_t rot = static_cast<uint32_t>( oldstate >> 59u );
    const uint32_t res = (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    return res;
}

uint32_t PcgRandom::uint( uint32_t bound )
{
    // The piece of shit below doesn't work as the fancy explanation says it should.
    // So I do the 'naive' approach instead.
    const uint32_t i = uint();
    const uint32_t res = i % bound;
    return res;

    // To avoid bias, we need to make the range of the RNG a multiple of
    // bound, which we do by dropping output less than a threshold.
    // A naive scheme to calculate the threshold would be to do
    //
    //     uint32_t threshold = 0x100000000ull % bound;
    //
    // but 64-bit div/mod is slower than 32-bit div/mod (especially on
    // 32-bit platforms).  In essence, we do
    //
    //     uint32_t threshold = (0x100000000ull-bound) % bound;
    //
    // because this version will calculate the same modulus, but the LHS
    // value is less than 2^32.

    /*
       uint32_t threshold = -bound % bound;
       */

    // Uniformity guarantees that this loop will terminate.  In practice, it
    // should usually terminate quickly; on average (assuming all bounds are
    // equally likely), 82.25% of the time, we can expect it to require just
    // one iteration.  In the worst case, someone passes a bound of 2^31 + 1
    // (i.e., 2147483649), which invalidates almost 50% of the range.  In
    // practice, bounds are typically small and only a tiny amount of the range
    // is eliminated.

    /*
       for (;;) {
       uint32_t r = uint();
       if (r >= threshold)
       return r % bound;
       }
       */
}


}



