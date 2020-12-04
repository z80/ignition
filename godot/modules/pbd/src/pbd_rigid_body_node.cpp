
#include "pbd_rigid_body_node.h"
#include "pbd_server.h"
#include "pbd_rigid_body_rid.h"
#include "pbd_simulation_node.h"

#include "scene/3d/mesh_instance.h"

#include "Common.h"
#include "RigidBody.h"
#include "Simulation.h"

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

const NodePath & PbdRigidBodyNode::get_shape_mesh_path() const
{
	return shape_mesh_path;
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
	case NOTIFICATION_READY:
		ERR_FAIL_COND( init() );
		break;

	case NOTIFICATION_EXIT_TREE:
		finit();
		break;
	}

}

void PbdRigidBodyNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_shape_mesh_path", "node_path"), &PbdRigidBodyNode::set_shape_mesh_path );
	ClassDB::bind_method( D_METHOD("get_shape_mesh_path"), &PbdRigidBodyNode::get_shape_mesh_path, Variant::NODE_PATH );

	ClassDB::bind_method( D_METHOD("set_mass", "m"), &PbdRigidBodyNode::set_mass );
	ClassDB::bind_method( D_METHOD("get_mass"), &PbdRigidBodyNode::get_mass, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_inertia", "i3"), &PbdRigidBodyNode::set_inertia );
	ClassDB::bind_method( D_METHOD("get_inertia"), &PbdRigidBodyNode::get_inertia, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_friction", "k"), &PbdRigidBodyNode::set_friction );
	ClassDB::bind_method( D_METHOD("get_friction"), &PbdRigidBodyNode::get_friction, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_restitution", "k"), &PbdRigidBodyNode::set_restitution );
	ClassDB::bind_method( D_METHOD("get_restitution"), &PbdRigidBodyNode::get_restitution, Variant::REAL );

	ADD_PROPERTY( PropertyInfo(Variant::NODE_PATH, "shape_mesh_path"), "set_shape_mesh_path", "get_shape_mesh_path" );
	ADD_PROPERTY( PropertyInfo(Variant::REAL, "mass"), "set_mass", "get_mass" );
	ADD_PROPERTY( PropertyInfo(Variant::VECTOR3, "inertia"), "set_inertia", "get_inertia" );
	ADD_PROPERTY( PropertyInfo(Variant::REAL, "friction"), "set_friction", "get_friction" );
	ADD_PROPERTY( PropertyInfo(Variant::REAL, "restitution"), "set_restitution", "get_restitution" );
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

	// Initialize collision properties.
	MeshInstance * mi = get_mesh_path_instance();
	if ( !mi )
		return false;
	const bool initialized_collistions_ok = init_collision_mesh( mi );
	if ( !initialized_collistions_ok )
		return false;

	// Apply mass, inertia, etc.
	// And this should be after initializing collision mesh. It is because
	// there is sets friction, restitution and other params to default values.
	apply_all_body_props();

	// Check if parent is simulation.
	PbdSimulationNode * sim = get_parent_simulation();
	if ( !sim )
		return false;

	// Insert rigid body into simulation.
	PBD::Simulation * s = sim->sim->get_simulation();
	PBD::SimulationModel * sm = s->getModel();
	PBD::SimulationModel::RigidBodyVector & rbs = sm->getRigidBodies();

	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rbs.push_back( rb );

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
	if ( !mi )
		return false;

	const Transform t    = mi->get_transform();
	const Ref<Mesh> mesh = mi->get_mesh();

	PBD::VertexData            vd;
	Utilities::IndexedFaceMesh ifm;

	const int qty = mesh->get_surface_count();
	// This is a little bit strange.
	// Count overall number of vertices for Utilities::IndexedFaceMesh initialization.
	// Yes. It is required. Otherwise it fails.
	int total_qty = 0;
	for ( int i=0; i<qty; i++ )
	{
		const Mesh & m = **mesh;
		const uint32_t fmt = m.surface_get_format( i );
		const bool is_index_array = ( fmt & Mesh::ARRAY_FORMAT_INDEX );
		const int vert_count = is_index_array ? m.surface_get_array_index_len( i ) :
						                        m.surface_get_array_len( i );
		total_qty += vert_count;
	}
	ifm.initMesh( total_qty, total_qty, total_qty );

	// And now normal mesh parsing.
	for ( int i=0; i<qty; i++ )
	{
		const Mesh & m = **mesh;
		faces_from_surface( t, m, i, vd, ifm );
	}

	// Some magic after inserting vertices and faces.
	ifm.buildNeighbors();
	ifm.updateNormals( vd, 0 );
	ifm.updateVertexNormals( vd );


	const Transform own_t = get_transform();
	const Vector3   x = own_t.get_origin();
	const Quat      q = own_t.basis.get_rotation_quat();

	PBD::RigidBody * rb = rigid_body_rid->get_rigid_body();
	rb->initBody( mass, Vector3r( x.x, x.y, x.z ),
		          Vector3r( inertia_tensor.x, inertia_tensor.y, inertia_tensor.z ),
		          Quaternionr( q.w, q.x, q.y, q.z ),
		          vd, ifm );

	return true;
}

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, PBD::VertexData & vd, Utilities::IndexedFaceMesh & ifm )
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
			vd.addVertex( Vector3r( vt.x, vt.y, vt.z ) );
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




