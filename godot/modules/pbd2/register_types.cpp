
#include "register_types.h"

#include "core/engine.h"
#include "core/class_db.h"

#include "src/pbd_server.h"
#include "src/_pbd_server.h"

#include "src/pbd2_simulation_node.h"
#include "src/pbd2_rigid_body_node.h"
#include "src/pbd2_joint_hinge_node.h"
#include "src/pbd2_joint_ball_node.h"

void register_pbd_types()
{
	ClassDB::register_class<Pbd2SimulationNode>();
	ClassDB::register_class<Pbd2RigidBodyNode>();
	ClassDB::register_class<Pbd2ContactPtNode>();
}

void unregister_pbd_types()
{
	if ( pbd_server )
		memdelete( pbd_server );

	if ( _pbd_server )
		memdelete( _pbd_server );
}
