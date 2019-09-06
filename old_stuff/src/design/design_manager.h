
#ifndef __DESIGN_MANAGER_H_
#define __DESIGN_MANAGER_H_

#include "Urho3D/Urho3DAll.h"
#include "design.h"

namespace Osp
{

using namespace Urho3D;

class DesignManager: public Object
{
    URHO3D_OBJECT( DesignManager, Object );
public:
    struct DesignItem
    {
        /// Design name.
        String name;
        /// Design description.
        String desc;
        /// Identifier defining a file where
        /// the design is saved.
        int    id;
    };

    DesignManager( Context * c );
    ~DesignManager();

    bool saveDesign( const String & name,
                     const String & description,
                     const Design & design );
    Design loadDesign( int index );
    bool renameDesign( int index,
                       const String & nameNew );
    void setDescription( int index,
                         const String & description );
    void destroyDesign( int index );
    std::vector<String> designNames();
    const DesignItem & designItem( int ind );


public:
    String lastErr;

    std::vector<DesignItem> designs;
};


}

#endif



