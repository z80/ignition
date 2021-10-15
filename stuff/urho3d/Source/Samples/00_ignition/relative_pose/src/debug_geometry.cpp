
#include "debug_geometry.h"
#include "ref_frame.h"

namespace Ign
{

void DrawDebugGeometry( Node * n, DebugRenderer * debug, bool depthTest, bool recursive )
{
    {
        Vector<SharedPtr<Component> > comps = n->GetComponents();
        const unsigned qty = comps.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<Component> & c = comps[i];
            if ( c )
            {
                c->DrawDebugGeometry( debug, depthTest );
            }
        }
    }

    if ( recursive )
    {
        const Vector<SharedPtr<Node> > nodes = n->GetChildren();
        const unsigned qty = nodes.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<Node> cn = nodes[i];
            if ( cn )
                DrawDebugGeometry( cn, debug, depthTest, recursive );
        }
    }
}

void DrawDebugGeometryRefFrames( Scene * s, DebugRenderer * debug, bool depthTest )
{
    {
        Vector<SharedPtr<Component> > comps = s->GetComponents();
        const unsigned qty = comps.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<Component> & c = comps[i];
            if ( !c )
                continue;
            RefFrame * rf = c->Cast<RefFrame>();
            if ( !rf )
                continue;
            rf->DrawDebugGeometry( debug, depthTest );
        }
    }
}

}

