
#include "marching_cubes_dual_node.h"
#include "marching_cubes_dual.h"

namespace Ign
{

MarchingCubesDualNode::MarchingCubesDualNode()
{
    self_index      = -1;
    index_in_parent = -1;
    parent_node     = nullptr;
    for ( int i=0; i<8; i++ )
        child_nodes[i] = nullptr;
    size   = -1;

    for ( int i=0; i<8; i++ )
        values[i] = 0.0;

	face_base_index = -1;
	faces_qty       = 0;
}

MarchingCubesDualNode::~MarchingCubesDualNode()
{
}

MarchingCubesDualNode::MarchingCubesDualNode( const MarchingCubesDualNode & inst )
{
    *this = inst;
}

const MarchingCubesDualNode & MarchingCubesDualNode::operator=( const MarchingCubesDualNode & inst )
{
    if ( this != &inst )
    {
        self_index      = inst.self_index;
        index_in_parent = inst.index_in_parent;
        parent_node     = inst.parent_node;
        for ( int i=0; i<8; i++ )
            child_nodes[i] = inst.child_nodes[i];
        size   = inst.size;
        at     = inst.at;

        for ( int i=0; i<8; i++ )
        {
            vertices_int[i] = inst.vertices_int[i];
            vertices_scaled[i]     = inst.vertices_scaled[i];
            vertices[i] = inst.vertices[i];
            values[i]       = inst.values[i];
        }

		hash = inst.hash;

		face_base_index = inst.face_base_index;
		faces_qty       = inst.faces_qty;
		aabb            = inst.aabb;
    }

    return *this;
}

void MarchingCubesDualNode::query_nodes( const MarchingCubesDualNode & node, int sz, std::vector<MarchingCubesDualNode *> & ret )
{
	const bool intersects = this->intersects( node );
	if ( !intersects )
		return;

	if ( size == sz )
	{
		// Also make sure node is not empty.
		const bool not_empty = this->has_children() || (this->faces_qty > 0);
		if ( not_empty )
			ret.push_back( this );
		return;
	}

	const bool has_ch = this->has_children();
	if ( has_ch )
	{
		for ( int i=0; i<8; i++ )
		{
			MarchingCubesDualNode * ch = child_nodes[i];
			ch->query_nodes( node, sz, ret );
		}
	}
}

bool MarchingCubesDualNode::intersect_with_segment( MarchingCubesDual * tree, const Vector3d & start, const Vector3d & end, bool in_source, Vector3d & at, Vector3d & norm )
{
	const bool has_ch = this->has_children();
	if ( has_ch )
	{
		for ( int i=0; i<8; i++ )
		{
			MarchingCubesDualNode * ch = child_nodes[i];
			const bool ret = ch->intersect_with_segment( tree, start, end, in_source, at, norm );
			if ( ret )
				return true;
		}
		return false;
	}

	const Vector3 a( start.x_, start.y_, start.z_ );
	const Vector3 b( end.x_, end.y_, end.z_ );

	const bool intersects = this->aabb.intersects_segment( a, b );
	if ( !intersects )
		return false;

	const int qty = faces_qty;
	for ( int i=0; i<qty; i++ )
	{
		const int ind = face_base_index + i;
		const NodeFace & f = tree->get_face_by_index( ind );

		Vector3 pt;
		const bool ret = f.face.intersects_segment( a, b, &pt );
		if ( ret )
		{
			// Check if intersection point is inside the node.
			const bool inside = aabb.has_point( pt );
			if ( !inside )
				return false;

			at = Vector3d( pt.x, pt.y, pt.z );
			const Vector3 n = f.face.get_plane().get_normal();
			norm = Vector3d( n.x, n.y, n.z );

			if ( !in_source )
			{
				const SE3 & inv_se3 = tree->inverted_source_se3;
				at = inv_se3.q_ * at + inv_se3.r_;
				norm = inv_se3.q_ * norm;
			}

			return true;
		}
	}

	return false;
}

bool MarchingCubesDualNode::intersect_with_ray( MarchingCubesDual * tree, const Vector3d & start, const Vector3d & dir, bool in_source, Vector3d & at, Vector3d & norm )
{
	const bool has_ch = this->has_children();
	if ( has_ch )
	{
		for ( int i=0; i<8; i++ )
		{
			MarchingCubesDualNode * ch = child_nodes[i];
			const bool ret = ch->intersect_with_ray( tree, start, dir, in_source, at, norm );
			if ( ret )
				return true;
		}
		return false;
	}

	const Vector3 a( start.x_, start.y_, start.z_ );
	const Vector3 b( dir.x_, dir.y_, dir.z_ );

	const bool intersects = this->aabb.intersects_ray( a, b );
	if ( !intersects )
		return false;

	const int qty = faces_qty;
	for ( int i=0; i<qty; i++ )
	{
		const int ind = face_base_index + i;
		const NodeFace & f = tree->get_face_by_index( ind );

		Vector3 pt;
		const bool ret = f.face.intersects_ray( a, b, &pt );
		if ( ret )
		{
			// Check if intersection point is inside the node.
			const bool inside = aabb.has_point( pt );
			if ( !inside )
				return false;

			at = Vector3d( pt.x, pt.y, pt.z );
			const Vector3 n = f.face.get_plane().get_normal();
			norm = Vector3d( n.x, n.y, n.z );

			const SE3 & inv_se3 = tree->inverted_source_se3;
			at = inv_se3.q_ * at + inv_se3.r_;
			norm = inv_se3.q_ * norm;

			if ( !in_source )
			{
				const SE3 & inv_se3 = tree->inverted_source_se3;
				at = inv_se3.q_ * at + inv_se3.r_;
				norm = inv_se3.q_ * norm;
			}

			return true;
		}
	}

	return false;
}

bool MarchingCubesDualNode::has_children() const
{
    for ( int i=0; i<8; i++ )
    {
        const bool has = (child_nodes[i] != nullptr);
        if ( has )
            return true;
    }

    return false;
}

bool MarchingCubesDualNode::subdivide( MarchingCubesDual * tree, VolumeSource * source, const DistanceScalerBase * scaler )
{
	// Initialize AABB. It is for computing intersections.
	this->init_aabb( tree );

    if ( size <= 2 )
	{
		//const bool has_surface = this->has_surface( tree->iso_level );
		//MarchingCubesDualNode * parent = this->parent_node;
		//const bool parent_split = tree->should_split( parent, source, scaler );
        return false;
	}

    const bool should_split = tree->should_split( this, source, scaler );
    if ( !should_split )
        return false;

    const int size_2 = size / 2;

    for ( int i=0; i<8; i++ )
    {
        child_nodes[i]                  = tree->create_node();
        child_nodes[i]->size            = size_2;
        child_nodes[i]->parent_node     = this;
        child_nodes[i]->index_in_parent = i;
    }

    child_nodes[0]->at = this->at;
    child_nodes[1]->at = this->at + VectorInt( size_2,      0,      0 );
    child_nodes[2]->at = this->at + VectorInt( size_2,      0, size_2 );
    child_nodes[3]->at = this->at + VectorInt(      0,      0, size_2 );
    child_nodes[4]->at = this->at + VectorInt(      0, size_2,      0 );
    child_nodes[5]->at = this->at + VectorInt( size_2, size_2,      0 );
    child_nodes[6]->at = this->at + VectorInt( size_2, size_2, size_2 );
    child_nodes[7]->at = this->at + VectorInt(      0, size_2, size_2 );

    for ( int i=0; i<8; i++ )
		tree->compute_node_values( *(child_nodes[i]), source, scaler );

	for ( int i=0; i<8; i++ )
		child_nodes[i]->subdivide( tree, source, scaler );

    return true;
}

void MarchingCubesDualNode::compute_hashes()
{
	if ( parent_node != nullptr )
		hash = parent_node->hash;

	else
		hash.reset();

	hash << at.x;
	hash << at.y;
	hash << at.z;

	const bool has_children = this->has_children();
	if ( has_children )
	{
		for ( int i=0; i<8; i++ )
		{
			MarchingCubesDualNode * ch = child_nodes[i];
			ch->compute_hashes();
		}
	}
}

void MarchingCubesDualNode::init_aabb( MarchingCubesDual * tree )
{
	const Vector3d v_at = tree->at_in_source( at );
	const Vector3d v_to = tree->at_in_source( VectorInt( at.x+size, at.y+size, at.z+size ) );
	const Vector3d sz = v_to - v_at;
	aabb = AABB( Vector3(v_at.x_, v_at.y_, v_at.z_), Vector3(sz.x_, sz.y_, sz.z_) );
}


bool MarchingCubesDualNode::intersects( const MarchingCubesDualNode & other ) const
{
	{
		const int a = at.x;
		const int b = other.at.x + other.size;
		if ( a > b )
			return false;
	}
	{
		const int a = at.x + size;
		const int b = other.at.x;
		if ( a < b )
			return false;
	}

	{
		const int a = at.y;
		const int b = other.at.y + other.size;
		if ( a > b )
			return false;
	}
	{
		const int a = at.y + size;
		const int b = other.at.y;
		if ( a < b )
			return false;
	}

	{
		const int a = at.z;
		const int b = other.at.z + other.size;
		if ( a > b )
			return false;
	}
	{
		const int a = at.z + size;
		const int b = other.at.z;
		if ( a < b )
			return false;
	}

	return true;
}

bool MarchingCubesDualNode::contains_point( MarchingCubesDual * tree, const Vector3d & at ) const
{
	const SE3 & se3 = tree->source_se3;
	const Vector3d at_s = se3.q_ * at + se3.r_;
	const bool ret = aabb.has_point( Vector3( at_s.x_, at_s.y_, at_s.z_ ) );
	return ret;
}

Vector3d MarchingCubesDualNode::center_vector( MarchingCubesDual * tree, bool in_source ) const
{
	const VectorInt c = center();
	const Vector3d cs = tree->at_in_source( c );
	const SE3 & se3 = tree->inverted_source_se3;
	const Vector3d cw = in_source ? cs : (se3.q_ * cs + se3.r_);
	return cw;
}

SE3 MarchingCubesDualNode::se3_in_point( MarchingCubesDual * tree, const Vector3d & at, bool in_source ) const
{
	const SE3 & se3     = tree->source_se3;
	const SE3 & inv_se3 = tree->inverted_source_se3;

	const Vector3d at_s = se3.q_ * at + se3.r_;
	const Vector3d up = in_source ? ( at_s / at_s.Length() ) : inv_se3.q_ * ( at_s / at_s.Length() );
	const Vector3d up_default = in_source ? Vector3d( 0.0, 1.0, 0.0 ) : inv_se3.q_ * Vector3d( 0.0, 1.0, 0.0 );
	const Quaterniond q = Quaterniond( up_default, up );

	SE3 ret;
	ret.q_ = q;
	ret.r_ = at;

	return ret;
}


const VectorInt MarchingCubesDualNode::center() const
{
    const int size_2 = size / 2;
    const VectorInt c = at + VectorInt( size_2, size_2, size_2 );
    return c;
}

bool MarchingCubesDualNode::has_surface( Float iso_level ) const
{
    int above_qty = 0;
    int below_qty = 0;
    for ( int i=0; i<8; i++ )
    {
        const Float v = values[i];
        if ( v > iso_level )
            above_qty += 1;
        else if ( v < iso_level )
            below_qty += 1;

        if ( (above_qty > 0) && (below_qty > 0) )
            return true;
    }

    return false;
}

bool MarchingCubesDualNode::at_left( const MarchingCubesDualNode * root ) const
{
    const bool ret = (at.x <= root->at.x);
    return ret;
}

bool MarchingCubesDualNode::at_right( const MarchingCubesDualNode * root ) const
{
    const bool ret = ((at.x+size) >= (root->at.x+root->size));
    return ret;
}

bool MarchingCubesDualNode::at_bottom( const MarchingCubesDualNode * root ) const
{
    const bool ret = (at.y <= root->at.y);
    return ret;
}

bool MarchingCubesDualNode::at_top( const MarchingCubesDualNode * root ) const
{
    const bool ret = ((at.y+size) >= (root->at.y+root->size));
    return ret;
}

bool MarchingCubesDualNode::at_back( const MarchingCubesDualNode * root ) const
{
    const bool ret = (at.z <= root->at.z);
    return ret;
}

bool MarchingCubesDualNode::at_front( const MarchingCubesDualNode * root ) const
{
    const bool ret = ((at.z+size) >= (root->at.z+root->size));
    return ret;
}














}



