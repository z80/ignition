
#include "system_generator.h"
#include "pi_system_generator.h"
#include "pi_consts.h"

namespace Ign
{

SystemGenerator::SystemGenerator()
{
}

SystemGenerator::~SystemGenerator()
{
}

void SystemGenerator::generate( Context * context )
{
	PiSystem s( 10, 2, 1 );
	generateSystem( s );
	createBodies( context, s );

	const unsigned qty = s.bodies_.Size();
	for ( unsigned i=0; i<qty; i++ )
	{
		createBody( context, s, i );
	}
}

void SystemGenerator::generateSystem( PiSystem & s )
{
	PiRandom rand;
	rand.seed( UNIVERSE_SEED );
	PiSystemGenerator generator;
	generator.apply( &s, rand );
}

void SystemGenerator::createBodies( Context * context, PiSystem & s )
{

}

void SystemGenerator::createBody( Context * context, PiSystem & s, int bodyIndex )
{
}

}






