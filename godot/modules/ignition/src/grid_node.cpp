
#include "grid_node.h"
#include "occupancy_grid.h"

GridNode::GridNode()
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

GridNode::~GridNode()
{

}

GridNode::GridNode( const GridNode & inst )
{
    *this = inst;
}

const GridNode & GridNode::operator=( const GridNode & inst )
{
    if ( this != &inst )
    {
        tree = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent = inst.indexInParent;
        level = inst.level;
		value = inst.value;
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

		verts_[0] = inst.verts_[0];
		verts_[1] = inst.verts_[1];
		verts_[2] = inst.verts_[2];
		verts_[3] = inst.verts_[3];
		verts_[4] = inst.verts_[4];
		verts_[5] = inst.verts_[5];
		verts_[6] = inst.verts_[6];
		verts_[7] = inst.verts_[7];

		planes_[0] = inst.planes_[0];
		planes_[1] = inst.planes_[1];
		planes_[2] = inst.planes_[2];
		planes_[3] = inst.planes_[3];
		planes_[4] = inst.planes_[4];
		planes_[5] = inst.planes_[5];

		aabb_ = inst.aabb_;

        ptInds = inst.ptInds;

		init();
    }

    return *this;
}

bool GridNode::hasChildren() const
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

bool GridNode::subdivide()
{
    // Subdrive means creating children.
    // It means node itself turns into not filled one.

    if ( level >= tree->max_depth_ )
        return false;

    bool ch = hasChildren();
    if ( ch )
        return false;

    if ( ptInds.empty() )
        return false;

    const int childLevel = this->level + 1;
    const real_t chSize2 = this->size2 * 0.5;

    GridNode nn[8];
    int    qtys[8];
    GridNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insertNode( n );
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
		GridNode & ch_n = nn[j];
		ch_n.value = 0;
		for ( int i=0; i<qty; i++ )
		{
			const int ind = ptInds[i];
			const Face3 & face = tree->faces_[ind];

			const bool inside = ch_n.inside( face );
            if ( inside )
            {
                ch_n.ptInds.push_back( ind );
                qtys[j] += 1;
                ch_n.value += 1;
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
					ch_n.value += 1;
				}
			}
		}
    }

	// Reset indices and value for a node with children.
    ptInds.clear();
	value = 0;

	// Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
		GridNode & ch_n = nn[i];
        if ( ( qtys[i] > 0 ) && ( childLevel < tree->max_depth_ ) )
            ch_n.subdivide();
        tree->nodes_.ptrw()[ ch_n.absIndex ] = ch_n;
    }

    return true;
}


bool GridNode::inside( const Face3 & face ) const
{
	const bool intersects = face.intersects_aabb( aabb_ );
	return intersects;
}

bool GridNode::inside( const Vector3 & pt ) const
{
	// If for at least one plane the point is above,
	// it is outside the node.
	for ( int i=0; i<6; i++ )
	{
		const Plane & p = planes_[i];
		const bool point_above = p.is_point_over( pt );
		if ( point_above )
			return false;
	}

	return true;
}

bool GridNode::inside( const GridNode & n ) const
{
	const bool intersects = aabb_.intersects( n.aabb_ );
	const bool has_ch = n.hasChildren();
	if ( !has_ch )
	{
		const bool is_filled = (n.value > 0);
		return is_filled;
	}

	for ( int i=0; i<8; i++ )
	{
		const int ind = n.children[i];
		const GridNode & ch_n = tree->nodes_.ptr()[ind];
		const bool ch_intersects = inside( ch_n );
		if ( ch_intersects )
			return true;
	}

	return false;
}

void GridNode::init()
{
	// Vertices.
	verts_[0].x = center.x - size2;
	verts_[0].y = center.y - size2;
	verts_[0].z = center.z - size2;

	verts_[1].x = center.x + size2;
	verts_[1].y = center.y - size2;
	verts_[1].z = center.z - size2;

	verts_[2].x = center.x + size2;
	verts_[2].y = center.y + size2;
	verts_[2].z = center.z - size2;

	verts_[3].x = center.x - size2;
	verts_[3].y = center.y + size2;
	verts_[3].z = center.z - size2;

	verts_[4].x = center.x - size2;
	verts_[4].y = center.y - size2;
	verts_[4].z = center.z + size2;

	verts_[5].x = center.x + size2;
	verts_[5].y = center.y - size2;
	verts_[5].z = center.z + size2;

	verts_[6].x = center.x + size2;
	verts_[6].y = center.y + size2;
	verts_[6].z = center.z + size2;

	verts_[7].x = center.x - size2;
	verts_[7].y = center.y + size2;
	verts_[7].z = center.z + size2;

	// Planes.
	// Front.
	Vector3 r0 = center;
	r0.y += size2;
	planes_[0] = Plane( r0, Vector3(0.0, 1.0, 0.0) );

	// Back
	r0 = center;
	r0.y -= size2;
	planes_[1] = Plane(r0, Vector3(0.0, -1.0, 0.0) );

	// Left
	r0 = center;
	r0.x -= size2;
	planes_[2] = Plane( r0, Vector3(-1.0, 0.0, 0.0) );

	// Right
	r0 = center;
	r0.x += size2;
	planes_[3] = Plane( r0, Vector3(1.0, 0.0, 0.0) );

	// Top
	r0 = center;
	r0.z += size2;
	planes_[4] = Plane( r0, Vector3(0.0, 0.0, 1.0) );

	// Bottom
	r0 = center;
	r0.z -= size2;
	planes_[5] = Plane( r0, Vector3( 0.0, 0.0, -1.0 ) );


	// AABB
	{
		const Vector3 sz2 = Vector3( size2, size2, size2 );
		const Vector3 sz  = sz2 * 2.0;
		const Vector3 origin = center - sz2;
		aabb_ = AABB( origin, sz );
	}
}



