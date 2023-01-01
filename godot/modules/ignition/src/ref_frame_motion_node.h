
#ifndef __REF_FRAME_MOTION_NODE_H_
#define __REF_FRAME_MOTION_NODE_H_

#include "ref_frame_node.h"
#include "celestial_motion.h"
#include "distance_scaler_base_ref.h"

namespace Ign
{

class RefFrameMotionNode: public RefFrameNode
{
	GDCLASS(RefFrameMotionNode, RefFrameNode);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();
	void _notification( int p_notification );

public:
	RefFrameMotionNode();
	virtual ~RefFrameMotionNode();

	virtual void set_se3_raw( const SE3 & se3 ) override;

	void set_allow_orbiting( bool en );
	bool get_allow_orbiting() const;

	void stop();

	bool is_orbiting() const;
	String movement_type() const;
	real_t specific_angular_momentum() const;
	real_t eccentricity() const;
	real_t period() const;
	real_t time_after_periapsis() const;
	real_t closest_approach() const;
	real_t perigee() const;
	real_t apogee() const;
	real_t min_velocity() const;
	real_t max_velocity() const;
	real_t excess_velocity() const;
	real_t deflection_angle() const;
	Vector3 acceleration() const;
	// Orbit orientation.
	// From focus towards perigee.
	Vector3 ex() const;
	// Along velocity at perigee.
	Vector3 ey() const;

	void set_own_gm( real_t gm );
	real_t get_own_gm() const;

	real_t get_gm() const;

	bool launch();
	real_t compute_gm_by_speed( real_t radius_km, real_t wanted_surface_orbit_velocity_kms ) const;
	real_t compute_gm_by_period( real_t radius_km, real_t wanted_period_hrs ) const;
	bool launch_elliptic( const Vector3 & unit_r, const Vector3 & unit_v, real_t period_hrs, real_t eccentricity );

	virtual Dictionary serialize() override;
	virtual bool deserialize( const Dictionary & data ) override;

	PoolVector3Array orbit_points( Node * orbiting_center, Node * player_viewpoint, Node * camera_node, Ref<DistanceScalerBaseRef> scaler, int qty );

	void set_force_numerical( bool en );
	bool get_force_numerical() const;


	virtual void set_debug( bool en ) override;
	virtual bool get_debug() const override;

	// Compute forces, integrate dynamics.
	virtual void _ign_physics_pre_process( real_t delta ) override;
	// Set positions, place visuals.
	virtual void _ign_physics_process( real_t delta ) override;
	// Place camera.
	virtual void _ign_physics_post_process( real_t delta ) override;

public:
	CelestialMotion cm;
	// This one is for trajectory visualization buffering.
	Vector<Vector3d> pts;
};


}




#endif


