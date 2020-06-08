
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

        qtys[i] = 0;
    }

    const int expectedQty = ptInds.size() / 8;
    for ( int i=0; i<8; i++ )
    {
        nn[i].indexInParent = i;
        nn[i].parentAbsIndex = this->absIndex;
        nn[i].level = childLevel;
        nn[i].size2 = chSize2;

        children[i] = nn[i].absIndex;
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
    for ( int i=0; i<qty; i++ )
    {
        const int ind = ptInds[i];
        const Face3 & face = tree->faces_[ind];

        for ( int i=0; i<8; i++ )
        {
			nn[i].value = 0;
            if ( nn[i].inside( face ) )
            {
                nn[i].ptInds.push_back( ind );
                qtys[i] += 1;
				nn[i].value += 1;
                break;
            }
        }
    }

	// Reset indices and value for a node with children.
    ptInds.clear();
	value = 0;

	// Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        if ( ( qtys[i] > 0 ) && ( childLevel < tree->max_depth_ ) )
            nn[i].subdivide();
        tree->nodes_.ptrw()[ nn[i].absIndex ] = nn[i];
    }

    return true;
}


bool GridNode::inside( const Face3 & face ) const
{
	// If all 3 verts are above either node plane
	// it means node can't contain the face.
	for ( int i=0; i<6; i++ )
	{
		const Plane & p = planes_[i];
		bool all_above = true;
		for ( int j=0; j<3; j++ )
		{
			const Vector3 & v = face.vertex[i];
			const bool point_above = p.is_point_over( v );
			if ( !point_above )
			{
				all_above = false;
				break;
			}
			if ( all_above )
				return false;
		}
	}

	// If all node verts are on the same size of the face
	// the node can't contain this face.
	int qty = 0;
	const Plane p = face.get_plane();
	for ( int i=0; i<8; i++ )
	{
		const Vector3 & v = verts_[i];
		const bool is_above = p.is_point_over( v );
		if ( is_above )
			qty += 1;
		else
			qty -= 1;
	}
	if ( (qty == 8) || (qty == -8) )
		return false;

	// Now compute face "side planes". And do the same thing.
	const Vector3 e0( face.vertex[0] - face.vertex[1] );
	const Vector3 e1( face.vertex[1] - face.vertex[2] );
	const Vector3 e2( face.vertex[2] - face.vertex[0] );
	const Vector3 normal = vec3_cross( e0, e2 );
	const bool is_degenerate = (normal.length_squared() < CMP_EPSILON2);
	if ( is_degenerate )
		return false;

	const Vector3 ee[] = { e0, e1, e2 };
	for ( int i=0; i<3; i++ )
	{
		const Vector3 & e = ee[i];
		const Vector3 n0 = vec3_cross( normal, e );
		const Plane   p0 = Plane( n0, face.vertex[i] );
		qty = 0;
		for ( int j=0; j<8; j++ )
		{
			const Vector3 & v = verts_[j];
			const bool is_above = p0.is_point_over( v );
			if ( is_above )
				qty += 1;
		}
		if ( qty == 8 )
			return false;
	}

	return true;
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
	}

	return true;
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

	verts_[2].x = center.x - size2;
	verts_[2].y = center.y + size2;
	verts_[2].z = center.z - size2;

	verts_[3].x = center.x + size2;
	verts_[3].y = center.y + size2;
	verts_[3].z = center.z - size2;

	verts_[4].x = center.x - size2;
	verts_[4].y = center.y - size2;
	verts_[4].z = center.z + size2;

	verts_[5].x = center.x + size2;
	verts_[5].y = center.y - size2;
	verts_[5].z = center.z + size2;

	verts_[6].x = center.x - size2;
	verts_[6].y = center.y + size2;
	verts_[6].z = center.z + size2;

	verts_[7].x = center.x + size2;
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
}



