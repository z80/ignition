
#ifndef __DESIGN_H_
#define __DESIGN_H_

#include <Urho3D/Urho3DAll.h>
#include "vector3d.h"
#include "quaterniond.h"

namespace Osp
{

using namespace Urho3D;


class Design: public Urho3D::RefCounted
{
public:
    struct Joint
    {
        // Additional axis for rotation is not needed.
        // It is supposed to be fixed by part design.
        // So really need only "r" and "q".
        int blockA, slotA;
        int blockB, slotB;
        // May describe additional parameters later.
    };

    struct Block
    {
        // Block type name to be able to create one.
        String typeName;
        // Position and orientation with respect to
        // the Site design was made in.
        Vector3d    r;
        Quaterniond q;
    };

    Design();
    ~Design();

    Design( const Design & inst );
    const Design & operator=( const Design & inst );

    bool save( const String & fname, bool overwrite=false ) const;
    bool load( const String & fname );
    /// 1) Each part is in joints at least once.
    /// 2) Joints connect existing parts.
    /// 3) No isolated components.
    bool valid() const;

public:
    std::vector<Block> blocks;
    std::vector<Joint> joints;
};



}


#endif



