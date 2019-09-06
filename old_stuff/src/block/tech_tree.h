
#ifndef __TECH_TREE_H_
#define __TECH_TREE_H_

#include <Urho3D/Urho3DAll.h>
#include "vector3d.h"

using namespace Urho3D;

namespace Osp
{

struct ConnectionDesc
{
    /// Where on a part connection point is.
    Vector3d   r;
    /// Alignment vector.
    Vector3d   a;
    /// Allow connections anywhere or only to another connection
    /// point.
    bool       attachAnywhere;
};

struct PartDesc
{
    /// Part name. It should correspond to a
    /// part PartsManager is able to create.
    String name;
    /// Category part goes to in the panel.
    String category;
    /// Needed tech tree node to allow part
    /// to be visible.
    String neededNode;

    String tooltip;
    String description;

    String icon;

    std::vector<ConnectionDesc> connections;
};


// Group container.
struct CategoryDesc
{
    String name;
    String tooltip;
    String description;

    String         icon;

    /// PartDesc indices in the array of all PartDescs.
    std::vector<int> items;
};

class TechTree: public Object
{
    URHO3D_OBJECT( TechTree, Object )
public:
    TechTree( Context * c );
    ~TechTree();

    const PartDesc & partDesc( const String & name );

    std::vector<CategoryDesc> & getPanelContent();
    std::vector<PartDesc>     & getPartDescs();
public:
    std::vector<PartDesc>     partDescs;
    std::vector<CategoryDesc> panelContent;

    void debugInit();
};


}





#endif

