
#ifndef __PI_SYSTEM_H_
#define __PI_SYSTEM_H_

#include "data_types.h"
#include "vector3d.h"
#include "pi_fixed.h"
#include "pi_source.h"

namespace Ign
{

class PiSystem
{
public:
        PiSystem( int x, int y, int z, Uint32 si )
            : sx(x),
              sy(y),
              sz(z),
              idx(si),
              m_numStars(0),
              m_seed(0),
              m_population(-1)
        {}

        // Check that we've had our habitation status set
        const String & GetName() const { return m_name; }
        const Vector<String> & GetOtherNames() const { return m_other_names; }
        unsigned GetNumStars() const { return m_numStars; }
        const PiSourceDesc & GetStarType( unsigned i ) const
        {
                assert( i < m_numStars );
                return m_starType[i];
        }



        const int sx, sy, sz;
        const Uint32 idx;

        String m_name;
        Vector<String> m_other_names;
        Vector3d m_pos;
        unsigned m_numStars;
        PiSourceDesc m_starType[4];
        Uint32 m_seed;
        fixed m_population;
};




}






#endif






