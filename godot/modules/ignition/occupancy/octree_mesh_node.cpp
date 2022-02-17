
#include "octree_mesh_node.h"
#include "octree_mesh.h"


namespace Ign
{

OctreeMeshNode::OctreeMeshNode()
{
    this->tree = nullptr;
    parentAbsIndex = -1;
    indexInParent = -1;
    level = -1;
    absIndex = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    size2 = 1.0;
    center = Vector3( 0.0, 0.0, 0.0 );

    init();
}

OctreeMeshNode::~OctreeMeshNode()
{

}

OctreeMeshNode::OctreeMeshNode( const OctreeMeshNode & inst )
{
    *this = inst;
}

const OctreeMeshNode & OctreeMeshNode::operator=( const OctreeMeshNode & inst )
{
    if ( this != &inst )
    {
        tree = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent = inst.indexInParent;
        level = inst.level;
        absIndex = inst.absIndex;

        children[0] = inst.children[0];
        children[1] = inst.children[1];
        children[2] = inst.children[2];
        children[3] = inst.children[3];
        children[4] = inst.children[4];
        children[5] = inst.children[5];
        children[6] = inst.children[6];
        children[7] = inst.children[7];

        size2 = inst.size2;
        center = inst.center;

        aabb_ = inst.aabb_;

        ptInds = inst.ptInds;

	init();
    }

    return *this;
}

bool OctreeMeshNode::hasChildren() const
{
    // Filled can't have children.
    if ( !ptInds.empty() )
        return false;

    for ( int i=0; i<8; i++ )
    {
        if ( children[i] >= 0 )
            return true;
    }

    return false;
}

bool OctreeMeshNode::subdivide()
{
    // Subdrive means creating children.
    // It means node itself turns into not filled one.

    if ( level >= tree->max_depth_ )
        return false;

    bool ch = hasChildren();
    if ( ch )
        return false;

	const int pts_qty = ptInds.size();
    if ( pts_qty <= tree->min_faces_ )
        return false;

    const int childLevel = this->level + 1;
    const real_t chSize2 = this->size2 * 0.5;

    OctreeMeshNode nn[8];
    int            qtys[8];
    OctreeMeshNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insert_node( n );
        nn[i] = tree->nodes_[ n.absIndex ];

        nn[i].indexInParent = i;
        nn[i].parentAbsIndex = this->absIndex;
        nn[i].level = childLevel;
        nn[i].size2 = chSize2;

        children[i] = nn[i].absIndex;

        qtys[i] = 0;
    }
    nn[0].center = this->center;
    nn[0].center.x -= chSize2;
    nn[0].center.y -= chSize2;
    nn[0].center.z -= chSize2;

    nn[1].center = this->center;
    nn[1].center.x += chSize2;
    nn[1].center.y -= chSize2;
    nn[1].center.z -= chSize2;

    nn[2].center = this->center;
    nn[2].center.x -= chSize2;
    nn[2].center.y += chSize2;
    nn[2].center.z -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x += chSize2;
    nn[3].center.y += chSize2;
    nn[3].center.z -= chSize2;

    nn[4].center = this->center;
    nn[4].center.x -= chSize2;
    nn[4].center.y -= chSize2;
    nn[4].center.z += chSize2;

    nn[5].center = this->center;
    nn[5].center.x += chSize2;
    nn[5].center.y -= chSize2;
    nn[5].center.z += chSize2;

    nn[6].center = this->center;
    nn[6].center.x -= chSize2;
    nn[6].center.y += chSize2;
    nn[6].center.z += chSize2;

    nn[7].center = this->center;
    nn[7].center.x += chSize2;
    nn[7].center.y += chSize2;
    nn[7].center.z += chSize2;

    for ( int i=0; i<8; i++ )
        nn[i].init();

    // Need to assign center and size
    const int qty = ptInds.size();
    for ( int j=0; j<8; j++ )
    {
        OctreeMeshNode & ch_n = nn[j];
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds[i];
            const Face3 & face = tree->faces_[ind];

            const bool inside = ch_n.inside( face );
            if ( inside )
            {
                ch_n.ptInds.push_back( ind );
                qtys[j] += 1;
            }
        }
        if ( qtys[j] == 0 )
        {
            for ( int i=0; i<qty; i++ )
            {
                const int ind = ptInds[i];
                const Face3 & face = tree->faces_[ind];

                const bool inside = ch_n.inside( face );
                if ( inside )
                {
                    ch_n.ptInds.push_back( ind );
                    qtys[j] += 1;
                }
            }
        }
    }

    // Reset indices and value for a node with children.
    ptInds.clear();

    // Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        OctreeMeshNode & ch_n = nn[i];
        if ( ( qtys[i] > 0 ) && ( childLevel < tree->max_depth_ ) )
            ch_n.subdivide();
        tree->nodes_.ptrw()[ ch_n.absIndex ] = ch_n;
    }

    return true;
}


bool OctreeMeshNode::inside( const Face3 & face ) const
{
    const bool intersects = face.intersects_aabb( aabb_ );
    return intersects;
}


bool OctreeMeshNode::intersects_ray( const Vector3 & origin, const Vector3 & dir ) const
{
    const bool intersects_aabb = aabb_.intersects_ray( origin, dir );
    if ( !intersects_aabb )
        return false;

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int face_ind = ptInds.ptr()[i];
            const Face3 & f = tree->faces_[face_ind];
            const bool ok = f.intersects_ray( origin, dir );
            if ( ok )
                return true;
        }
        return false;
    }

    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const OctreeMeshNode & ch_n = tree->nodes_.ptr()[ind];
        const bool ch_intersects = ch_n.intersects_ray( origin, dir );
        if ( ch_intersects )
            return true;
    }

    return false;
}

bool OctreeMeshNode::intersects_ray_face( const Vector3 & origin, const Vector3 & dir, int & face_ind, real_t & face_dist ) const
{
    const bool intersects_aabb = aabb_.intersects_ray( origin, dir );
    if ( !intersects_aabb )
        return false;

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        bool intersects = false;
        for ( int i=0; i<qty; i++ )
        {
            const int child_face_ind = ptInds.ptr()[i];
            const Face3 & f = tree->faces_[child_face_ind];
            Vector3 at;
            const bool ok = f.intersects_ray( origin, dir, &at );
            if ( ok )
            {
                const Vector3 dr = at - origin;
                const real_t d   = dr.length();
                if ( (face_ind < 0) || (d < face_dist) )
                {
                    face_ind   = child_face_ind;
                    face_dist  = d;
                    intersects = true;
                    // Do not interrupt here.
                    // Need to check all triangles.
                }
            }
        }
        return intersects;
    }

    bool intersects = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const OctreeMeshNode & ch_n = tree->nodes_.ptr()[ind];
		
		int f_ind;
		real_t f_dist;
		const bool ch_intersects = ch_n.intersects_ray_face( origin, dir, f_ind, f_dist );
		if ( ch_intersects )
		{
			if ( (!intersects) || (f_dist < face_dist) )
			{
				face_ind  = f_ind;
				face_dist = f_dist;
				intersects = true;
			}
			// Again, don't interrupt it here. Need to check all the children.
		}    }

    return intersects;
}



bool OctreeMeshNode::intersects_segment( const Vector3 & start, const Vector3 & end ) const
{
    const bool intersects_aabb = aabb_.intersects_ray( start, end );
    if ( !intersects_aabb )
        return false;

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int face_ind = ptInds.ptr()[i];
            const Face3 & f = tree->faces_[face_ind];
            const bool ok = f.intersects_segment( start, end );
            if ( ok )
                return true;
        }
        return false;
    }

    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const OctreeMeshNode & ch_n = tree->nodes_.ptr()[ind];
        const bool ch_intersects = ch_n.intersects_ray( start, end );
        if ( ch_intersects )
            return true;
    }

    return false;
}

bool OctreeMeshNode::intersects_segment_face( const Vector3 & start, const Vector3 & end, int & face_ind, real_t & face_dist, Vector3 & face_at ) const
{
    const bool intersects_aabb = aabb_.intersects_segment( start, end );
    if ( !intersects_aabb )
        return false;

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        bool intersects = false;
        for ( int i=0; i<qty; i++ )
        {
            const int child_face_ind = ptInds.ptr()[i];
            const Face3 & f = tree->faces_[child_face_ind];
            Vector3 at;
            const bool ok = f.intersects_segment( start, end, &at );
            if ( ok )
            {
                const Vector3 dr = at - start;
                const real_t  d  = dr.length();
                if ( (face_ind < 0) || (d < face_dist) )
                {
                    face_ind   = child_face_ind;
                    face_dist  = d;
					face_at    = at;
                    intersects = true;
                    // Do not interrupt here.
                    // Need to check all triangles in order to find the closest one.
                }
            }
        }
        return intersects;
    }

    bool intersects = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const OctreeMeshNode & ch_n = tree->nodes_.ptr()[ind];
		int     f_ind  = -1;
		real_t  f_dist = -1.0;
		Vector3 f_at;
        const bool ch_intersects = ch_n.intersects_segment_face( start, end, f_ind, f_dist, f_at );
        if ( ch_intersects )
		{
			if ( (!intersects) || (f_dist < face_dist) )
			{
				face_ind   = f_ind;
				face_dist  = f_dist;
				face_at    = f_at;
				intersects = true;
			}
			// Again, don't interrupt it here. Need to check all the children.
		}
    }

    return intersects;
}


void OctreeMeshNode::init()
{
    // AABB
    const Vector3 sz2 = Vector3( size2, size2, size2 );
    const Vector3 sz  = sz2 * 2.0;
    const Vector3 origin = center - sz2;
    aabb_ = AABB( origin, sz );
}








}



