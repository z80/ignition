
#ifndef __DEBUG_GEOMETRY_H_
#define __DEBUG_GEOMETRY_H_

#include "Urho3D/Urho3DAll.h"

using namespace Urho3D;

namespace Ign
{

void DrawDebugGeometry( Node * n, DebugRenderer * debug, bool depthTest, bool recursive );
void DrawDebugGeometryRefFrames( Scene * s, DebugRenderer * debug, bool depthTest );

}


#endif
