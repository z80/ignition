
#include "openvr_capture.h"

// This one is for timestamp in exactly the same way as for SLAM.
#include <chrono>

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
	poses.resize( qty );

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

	/*vr::VRInput()->SetActionManifestPath( Path_MakeAbsolute( "./json/tracker.json", Path_StripFilename( Path_GetExecutablePath() ) ).c_str() );

	vr::VRInput()->GetActionSetHandle( "/actions/demo", &action_set );

	const int qty_n = trackers.size();
	for ( int i=0; i<qty_n; i++ )
	{
		Tracker & t = trackers.ptrw()[i];
		String stri = String( "/actions/demo/in/" );
		if (i == 0)
			stri += String( "Hand_Left" );
		else if ( i == 1 )
			stri += String( "Hand_Right" );
		else if ( i == 2 )
			stri += String( "Foot_Left" );
		else if ( i == 3 )
			stri += String( "Foot_Right" );
		else if ( i == 4 )
			stri += String( "Chest" );
		else if ( i == 5 )
			stri += String( "Head" );
		vr::VRInput()->GetActionHandle( stri.ascii().ptr(), &t.action_pose );
	}*/

	//vr::IVRS

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
		poses.clear();
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

	const int qty = trackers.size();

	tracker->GetDeviceToAbsoluteTrackingPose( vr::TrackingUniverseStanding, 0.0f, poses.ptrw(), qty );
	for ( int i=0; i<qty; i++ )
	{
		const vr::TrackedDevicePose_t & p = poses.ptr()[i];
		Tracker & t = trackers.ptrw()[i];

		t.valid = p.bPoseIsValid;

		const vr::HmdMatrix34_t & m = p.mDeviceToAbsoluteTracking;
		Basis   & b = t.t.basis;
		Vector3 & o = t.t.origin;
		o.x = m.m[0][3];
		o.y = m.m[1][3];
		o.z = m.m[2][3];

		b.elements[0].x = m.m[0][0];
		b.elements[0].y = m.m[0][1];
		b.elements[0].z = m.m[0][2];

		b.elements[1].x = m.m[1][0];
		b.elements[1].y = m.m[1][1];
		b.elements[1].z = m.m[1][2];

		b.elements[2].x = m.m[2][0];
		b.elements[2].y = m.m[2][1];
		b.elements[2].z = m.m[2][2];
	}

	return true;

	/*
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

	bool result = true;

	for ( int i=0; i<qty; i++ )
	{
		Tracker & t = trackers.ptrw()[i];
		vr::InputPoseActionData_t poseData;
		if ( vr::VRInput()->GetPoseActionDataForNextFrame( t.action_pose, vr::TrackingUniverseStanding, &poseData, sizeof( poseData ), vr::k_ulInvalidInputValueHandle ) != vr::VRInputError_None
			|| !poseData.bActive || !poseData.pose.bPoseIsValid )
		{
			t.valid = false;
			//printf( "fail\n" );
			result = false;
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
			b.elements[0].y = m.m[0][1];
			b.elements[0].z = m.m[0][2];

			b.elements[1].x = m.m[1][0];
			b.elements[1].y = m.m[1][1];
			b.elements[1].z = m.m[1][2];

			b.elements[2].x = m.m[2][0];
			b.elements[2].y = m.m[2][1];
			b.elements[2].z = m.m[2][2];


			//printf( "ok\n" );
			//continue;

			//m_rHand[eHand].m_rmat4Pose = ConvertSteamVRMatrixToMatrix4( poseData.pose.mDeviceToAbsoluteTracking );

		}
	}

	//return bRet;

	return result;*/
}


bool OpenvrCapture::valid( int i ) const
{
	const int sz = trackers.size();
	if ( i >= sz )
		return false;
	const bool ok = trackers.ptr()[i].valid;
	return ok;
}

Transform OpenvrCapture::pose( int i ) const
{
	const int sz = trackers.size();
	if ( i >= sz )
		return Transform();
	const Transform & t = trackers.ptr()[i].t;
	return t;
}

unsigned long long OpenvrCapture::timestamp()
{
	typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8> >::type > Days;

	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();

	const Days days = std::chrono::duration_cast<Days>(duration);
	duration -= days;

	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

	return nanoseconds.count();
}


