
#ifndef __PBD_RIGID_BODY_NODE_H_
#define __PBD_RIGID_BODY_NODE_H_

#include "scene/3d/spatial.h"
#include "Common.h"



class MeshInstance;
class PbdRigidBodyRid;
class PbdSimulationNode;

namespace PBD
{
	class RigidBody;
}


class PbdRigidBodyNode: public Spatial
{
	GDCLASS( PbdRigidBodyNode, Spatial );

public:
	PbdRigidBodyNode();
	~PbdRigidBodyNode();

	void set_shape_mesh_path( const NodePath & path );
	const NodePath & get_shape_mesh_path() const;

	void set_mass( real_t mass );
	real_t get_mass() const;

	void set_inertia( const Vector3 & i );
	Vector3 get_inertia() const;

	void set_friction( real_t k );
	real_t get_friction() const;

	void set_restitution( real_t k );
	real_t get_restitution() const;


	String get_configuration_warning() const;
protected:
	void _notification( int p_what );
	static void _bind_methods();


private:
	// Crazy Godot containers.
	RID rid;
	PbdRigidBodyRid * rigid_body_rid;
	NodePath shape_mesh_path;

	// body properties.
	real_t  mass;
	Vector3 inertia_tensor;
	real_t  friction;
	real_t  restitution;

	bool init();
	void finit();

	PbdSimulationNode * get_parent_simulation() const;
	MeshInstance      * get_mesh_path_instance() const;
	bool init_collision_mesh( MeshInstance * mi );
	void apply_all_body_props();
	void apply_rigid_body_pose();
public:
	PBD::RigidBody * get_rigid_body();
};








#endif



