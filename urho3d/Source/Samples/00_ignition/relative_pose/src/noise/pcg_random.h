
#ifndef __PCG_RANDOM_H_
#define __PCG_RANDOM_H_

#include <cstdint>

namespace Ign
{

/*
typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng)
{
	uint64_t oldstate = rng->state;
	// Advance internal state
	rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
	// Calculate output function (XSH RR), uses old state for max ILP
	uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	uint32_t rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
*/

class PcgRandom
{
public:
	PcgRandom( uint64_t state=0x853c49e6748fea9bULL, uint64_t seq=0xda3e39cb94b95bdbULL );
	~PcgRandom();

	PcgRandom( const PcgRandom & inst );
	const PcgRandom & operator=( const PcgRandom & inst );

	void seed( uint64_t s=0 );

	uint32_t uint();
	uint32_t uint( uint32_t bound );

	uint64_t state_, inc_;
};

}



#endif



