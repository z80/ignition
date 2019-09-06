
#include "pivot_marker.h"
#include "name_generator.h"
#include "block.h"

using namespace Urho3D;

namespace Osp
{

PivotMarker::PivotMarker( Context * c )
    : ItemBase( c )
{
}

void PivotMarker::createContent( Node * node )
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();

    // Separate node to attach object to. It is because scale might change.
    // And it is necessary to have a node tree with no scales envolved.
    modelNode = node->CreateChild( NameGenerator::Next( "PivotMarker" ) );
    model     = modelNode->CreateComponent<StaticModel>();
    model->SetModel( cache->GetResource<Model>("Models/Sphere.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Materials/Stone.xml") );
    model->SetCastShadows( true );
}

PivotMarker::~PivotMarker()
{

}

void PivotMarker::setSize( float sz )
{
    modelNode->SetScale( sz );
}

Block * PivotMarker::block()
{
    Node * n = GetNode();
    Node * pn = n->GetParent();
    Vector<SharedPtr<Component> > comps = pn->GetComponents();
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        Block * b = c->Cast<Block>();
        if ( b )
            return b;
    }
    return nullptr;
}

PivotMarker * PivotMarker::markerConnectedTo()
{
    return connectedTo.Get();
}

Block * PivotMarker::blockConnectedTo()
{
    PivotMarker * m = markerConnectedTo();
    if ( !m )
        return nullptr;
    Block * b = m->block();
    return b;
}

void PivotMarker::OnNodeSet( Node * node )
{
    Component::OnNodeSet( node );
    if ( node )
        createContent( node );
}


}

