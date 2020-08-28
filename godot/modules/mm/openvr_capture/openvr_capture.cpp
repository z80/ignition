
#include "openvr_capture.h"



//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
OpenvrCapture::OpenvrCapture()
	: tracker( nullptr )
{
};



OpenvrCapture::~OpenvrCapture()
{
	finit();
}

bool OpenvrCapture::init( int qty )
{
	finit();

	trackers.resize( qty );

	// Loading the SteamVR Runtime
	vr::EVRInitError eError = vr::VRInitError_None;
	tracker = vr::VR_Init( &eError, vr::VRApplication_Scene );

	if ( eError != vr::VRInitError_None )
	{
		tracker = nullptr;
		//char buf[1024];
		//sprintf_s( buf, sizeof( buf ), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		return false;
	}


	if ( !vr::VRCompositor() )
	{
		//printf( "Compositor initialization failed. See log file for details\n" );
		return false;
	}

	vr::VRInput()->SetActionManifestPath( Path_MakeAbsolute( "./json/tracker.json", Path_StripFilename( Path_GetExecutablePath() ) ).c_str() );

	vr::VRInput()->GetActionSetHandle( "/actions/demo", &action_set );

	const int qty_n = trackers.size();
	for ( int i=0; i<qty_n; i++ )
	{
		Tracker & t = trackers.ptrw()[i];
		String stri = String( "/actions/demo/in/Hand_" ) + itos( i );
		vr::VRInput()->GetActionHandle( stri.ascii().ptr(), &t.action_pose );
	}

	return true;
}



//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void OpenvrCapture::finit()
{
	if( tracker )
	{
		vr::VR_Shutdown();
		tracker = nullptr;
		trackers.clear();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool OpenvrCapture::process()
{
	if (!tracker)
		return false;
	if (trackers.empty())
		return false;

	// Process SteamVR events
	vr::VREvent_t event;
	while( tracker->PollNextEvent( &event, sizeof( event ) ) )
	{
		//ProcessVREvent( event );
	}

	// Process SteamVR action state
	// UpdateActionState is called each frame to update the state of the actions themselves. The application
	// controls which action sets are active with the provided array of VRActiveActionSet_t structs.
	vr::VRActiveActionSet_t actionSet = { 0 };
	actionSet.ulActionSet = action_set;
	vr::VRInput()->UpdateActionState( &actionSet, sizeof(actionSet), 1 );

	const int qty = trackers.size();
	for ( int i=0; i<qty; i++ )
	{
		Tracker & t = trackers.ptrw()[i];
		t.valid = true;
	}

	for ( int i=0; i<qty; i++ )
	{
		Tracker & t = trackers.ptrw()[i];
		vr::InputPoseActionData_t poseData;
		if ( vr::VRInput()->GetPoseActionDataForNextFrame( t.action_pose, vr::TrackingUniverseStanding, &poseData, sizeof( poseData ), vr::k_ulInvalidInputValueHandle ) != vr::VRInputError_None
			|| !poseData.bActive || !poseData.pose.bPoseIsValid )
		{
			t.valid = false;
			//printf( "fail\n" );
		}
		else
		{
			const vr::HmdMatrix34_t & m = poseData.pose.mDeviceToAbsoluteTracking;
			Basis   & b = t.t.basis;
			Vector3 & o = t.t.origin;
			o.x = m.m[0][3];
			o.y = m.m[1][3];
			o.z = m.m[2][3];

			b.elements[0].x = m.m[0][0];
			b.elements[0].y = m.m[1][0];
			b.elements[0].z = m.m[2][0];

			b.elements[1].x = m.m[0][1];
			b.elements[1].y = m.m[1][1];
			b.elements[1].z = m.m[2][1];

			b.elements[2].x = m.m[0][2];
			b.elements[2].y = m.m[1][2];
			b.elements[2].z = m.m[2][2];


			//printf( "ok\n" );
			//continue;

			//m_rHand[eHand].m_rmat4Pose = ConvertSteamVRMatrixToMatrix4( poseData.pose.mDeviceToAbsoluteTracking );

		}
	}

	//return bRet;

	return true;
}


const Transform & OpenvrCapture::pose( int i ) const
{
	const Transform & t = trackers.ptr()[i].t;
	return t;
}


