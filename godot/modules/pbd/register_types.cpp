
#include "register_types.h"

#include "core/engine.h"
#include "core/class_db.h"

#include "src/pbd_server.h"
#include "src/_pbd_server.h"

#include "src/pbd_simulation_node.h"
#include "src/pbd_rigid_body_node.h"


#include "Common.h"
#include "Logger.h"
#include "Timing.h"
INIT_LOGGING;
INIT_TIMING;

static PbdServer * pbd_server   = nullptr;
static _PbdServer * _pbd_server = NULL;

void register_pbd_types()
{
	pbd_server = memnew(PbdServer);
	pbd_server->init();
	_pbd_server = memnew(_PbdServer);
	ClassDB::register_class<_PbdServer>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("PbdServer", _PbdServer::get_singleton()));

	ClassDB::register_class<PbdSimulationNode>();
	ClassDB::register_class<PbdRigidBodyNode>();
}

void unregister_pbd_types()
{
	if ( pbd_server )
		memdelete( pbd_server );

	if ( _pbd_server )
		memdelete( _pbd_server );
}
