
#include "pbd_rigid_body_node.h"
#include "pbd_server.h"
#include "pbd_rigid_body_rid.h"
#include "pbd_simulation_node.h"

#include "scene/3d/mesh_instance.h"

#include "Common.h"
#include "RigidBody.h"

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, PBD::VertexData & vs, Utilities::IndexedFaceMesh & ifm );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, PBD::VertexData & vs, Utilities::IndexedFaceMesh & ifm );

PbdRigidBodyNode::PbdRigidBodyNode()
	: Spatial()
{
	mass            = 1.0;
	inertia_tensor  = Vector3( 1.0, 1.0, 1.0 );
	friction        = 1.0;
	restitution     = 0.5;
}

PbdRigidBodyNode::~PbdRigidBodyNode()
{
	finit();
}

void PbdRigidBodyNode::set_shape_mesh_path( const NodePath & path )
{
	shape_mesh_path = path;
}

void PbdRigidBodyNode::set_mass( real_t m )
{
	mass = m;
	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rb->setMass( mass );
}

real_t PbdRigidBodyNode::get_mass() const
{
	return mass;
}

void PbdRigidBodyNode::set_inertia( const Vector3 & i )
{
	inertia_tensor = i;
	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rb->setInertiaTensor( Vector3r( inertia_tensor.x, inertia_tensor.y, inertia_tensor.z ) );
}

Vector3 PbdRigidBodyNode::get_inertia() const
{
	return inertia_tensor;
}

void PbdRigidBodyNode::set_friction( real_t k )
{
	friction = k;
	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rb->setFrictionCoeff( friction );
}

real_t PbdRigidBodyNode::get_friction() const
{
	return friction;
}

void PbdRigidBodyNode::set_restitution( real_t k )
{
	restitution = k;
	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rb->setRestitutionCoeff( restitution );
}

real_t PbdRigidBodyNode::get_restitution() const
{
	return restitution;
}


String PbdRigidBodyNode::get_configuration_warning() const
{
	String pre = Spatial::get_configuration_warning();

	PbdSimulationNode * sim = get_parent_simulation();
	if ( !sim )
	{
		const String post = "PbdRigidBody should be a direct parent of PbdSimulationNode";
		pre += "\n";
		pre += post;
	}

	MeshInstance * m = get_mesh_path_instance();
	if ( !m )
	{
		const String post = "PbdRigidBody should have a valid MeshInstance path in order to have collisions";
		pre += "\n";
		pre += post;
	}

	return pre;
}


void PbdRigidBodyNode::_notification( int p_what )
{
	switch ( p_what )
	{
	// One step after simulation initialization.
	case NOTIFICATION_POST_ENTER_TREE:
		ERR_FAIL_COND( init() );
		break;

	case NOTIFICATION_EXIT_TREE:
		finit();
		break;
	}

}

void PbdRigidBodyNode::_bind_methods()
{
}

bool PbdRigidBodyNode::init()
{
	const bool valid_ok = rid.is_valid();
	if ( valid_ok )
		return true;

	rid = PbdServer::get_singleton()->create_rigid_body();
	const bool new_is_valid = rid.is_valid();
	if ( !new_is_valid )
		rigid_body_rid = nullptr;

	rigid_body_rid = PbdServer::get_singleton()->get_rigid_body( rid );

	// Apply mass, inertia, etc.
	apply_all_body_props();
	// Initialize collision properties.
	init_collision_mesh( get_mesh_path_instance() );

	// Check if parent is simulation.
	PbdSimulationNode * sim = get_parent_simulation();
	if ( !sim )
		return false;



	return true;

}

void PbdRigidBodyNode::finit()
{
	const bool valid_ok = rid.is_valid();
	if ( !valid_ok )
		return;

	PbdServer::get_singleton()->destroy_rigid_body( rid );
	rid = RID();
	rigid_body_rid = nullptr;
}

PbdSimulationNode * PbdRigidBodyNode::get_parent_simulation() const
{
	Node * p = get_parent();
	PbdSimulationNode * sim = Node::cast_to<PbdSimulationNode>( p );
	return sim;
}

MeshInstance * PbdRigidBodyNode::get_mesh_path_instance() const
{
	Node * n = get_node( shape_mesh_path );
	MeshInstance * m = Node::cast_to<MeshInstance>( n );
	return m;
}

bool PbdRigidBodyNode::init_collision_mesh( MeshInstance * mi )
{
	const Transform t    = mi->get_transform();
	const Ref<Mesh> mesh = mi->get_mesh();

	const PBD::VertexData            vd;
	const Utilities::IndexedFaceMesh ifm;

	const int qty = mesh->get_surface_count();
	for ( int i=0; i<qty; i++ )
	{
		const Mesh & m = **mesh;
		faces_from_surface( t, m, i, vd, ifm );
	}

	const Transform t = get_transform();
	const Vector3   x = t.get_origin();
	const Quat      q = t.basis.get_rotation_quat();

	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rb->initBody( mass, Vector3r( x.x, x.y, x.z ),
		          inertia_tensor, Quanterionr( q.x, q.y, q.z, q.w ),
		          vd, ifr );
}

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, PBD::VertexData & vs, Utilities::IndexedFaceMesh & ifm )
{
	// Don't add faces if doesn't consist of triangles.
	if (mesh.surface_get_primitive_type(surface_idx) != Mesh::PRIMITIVE_TRIANGLES)
		return;

	const bool indexed_array = (mesh.surface_get_format(surface_idx) & Mesh::ARRAY_FORMAT_INDEX );
	parse_mesh_arrays( t, mesh, surface_idx, indexed_array, vd, ifm );
}

static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, PBD::VertexData & vd, Utilities::IndexedFaceMesh & ifm )
{
	const int vert_count = is_index_array ? mesh.surface_get_array_index_len( surface_idx ) :
		                                    mesh.surface_get_array_len( surface_idx );
	if (vert_count == 0 || vert_count % 3 != 0)
		return;

	const int d_qty = vert_count / 3;
	ifm.release();

	Array arrays = mesh.surface_get_arrays( surface_idx );

	PoolVector<int> indices = arrays[Mesh::ARRAY_INDEX];
	PoolVector<int>::Read indices_reader = indices.read();

	PoolVector<Vector3> vertices = arrays[Mesh::ARRAY_VERTEX];
	PoolVector<Vector3>::Read vertices_reader = vertices.read();

	if ( is_index_array )
	{
		for (int i = 0; i < vert_count; i++)
		{
			const int face_idx   = i / 3;
			const int set_offset = i % 3;
			const int lookup_index = indices_reader[i];
			const Vector3 v = vertices_reader[lookup_index];
			const Vector3 vt = t.xform( v );
			vd.setPosition( i, Vector3r( vt.x, vt.y, vt.z ) );
			if ( set_offset == 0 )
			{
				const unsigned int inds[] = {i, i+1, i+2};
				ifm.addFace( inds );
			}
		}
	}
	else
	{
		for (int i = 0; i < vert_count; i++)
		{
			const int face_idx   = i / 3;
			const int set_offset = i % 3;
			const int lookup_index = i;
			const Vector3 v = vertices_reader[lookup_index];
			const Vector3 vt = t.xform( v );
			vd.setPosition( i, Vector3r( vt.x, vt.y, vt.z ) );
			if ( set_offset == 0 )
			{
				const unsigned int inds[] = {i, i+1, i+2};
				ifm.addFace( inds );
			}
		}
	}

	ifm.buildNeighbors();
	ifm.updateNormals( vd, 0 );
	ifm.updateVertexNormals( vd );
}

void PbdRigidBodyNode::apply_all_body_props()
{
	if ( !rigid_body_rid )
		return;
	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rb->setMass( mass );
	rb->setInertiaTensor( Vector3r( inertia_tensor.x, inertia_tensor.y, inertia_tensor.z ) );
	rb->setFrictionCoeff( friction );
	rb->setRestitutionCoeff( restitution );
}




