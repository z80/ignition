
#ifndef __OPENVR_CAPTURE_H_
#define __OPENVR_CAPTURE_H_

#include "core/vector.h"
#include "core/math/transform.h"

#include "openvr.h"
#include "pathtools_public.h"


class OpenvrCapture
{
public:
	OpenvrCapture();
	virtual ~OpenvrCapture();

	bool init( int qty=1 );
	void finit();
	bool process();

	bool valid( int i ) const;
	Transform pose( int i ) const;
	static signed long long timestamp();
private: 
	vr::IVRSystem * tracker;

	struct Tracker
	{
		//vr::VRActionHandle_t action_pose = vr::k_ulInvalidActionHandle;
		bool valid;
		Transform t;
	};

	Vector<vr::TrackedDevicePose_t> poses;
	Vector<Tracker> trackers;
	//vr::VRActionSetHandle_t action_set = vr::k_ulInvalidActionSetHandle;
};




#endif



