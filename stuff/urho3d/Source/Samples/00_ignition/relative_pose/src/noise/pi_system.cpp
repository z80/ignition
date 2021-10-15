
#include "pi_system.h"

namespace Ign
{

void PiSystem::debugDump()
{
	const PiSourceDesc & sbody = bodies_[ root_body_ind_ ];
	const String spacing( "" );
	dumpBody( spacing, sbody, root_body_ind_ );
}

void PiSystem::dumpBody( const String & spacing, const PiSourceDesc & sbody, unsigned bodyInd )
{
	URHO3D_LOGINFO( "Body:" );
	switch (sbody.super_type_)
	{
	case SUPERTYPE_STAR:
		URHO3D_LOGINFO( "supertype: Star" );
		break;
	case SUPERTYPE_ROCKY_PLANET:
		URHO3D_LOGINFO( "supertype: Rocky Planet" );
		break;
	default:
		URHO3D_LOGINFO( "supertype: None" );
		break;
	}
	URHO3D_LOGINFOF( "type:  %i", sbody.type_ );
	URHO3D_LOGINFOF( "seed_: %i", sbody.seed_ );
	URHO3D_LOGINFOF( "GM_:   %i", sbody.mass_.ToInt32() );
	URHO3D_LOGINFOF( "radius_: %i", sbody.radius_.ToInt32() );
	URHO3D_LOGINFOF( "aspectRatio_: %f", sbody.aspectRatio_.ToDouble() );
	URHO3D_LOGINFOF( "life_: %f", sbody.life_.ToDouble() );
	URHO3D_LOGINFOF( "atm_oxidizing_: %f", sbody.atm_oxidizing_.ToDouble() );
	URHO3D_LOGINFOF( "gas_: %f", sbody.gas_.ToDouble() );
	URHO3D_LOGINFOF( "average_temp_: %i", sbody.average_temp_ );

	URHO3D_LOGINFOF( "liquid_: %f", sbody.liquid_.ToDouble() );
	URHO3D_LOGINFOF( "volcanic_: %f", sbody.volcanic_.ToDouble() );
	URHO3D_LOGINFOF( "ice_: %f", sbody.ice_.ToDouble() );
	URHO3D_LOGINFOF( "metal_: %f", sbody.metal_.ToDouble() );

	// Orbit and rotation parameters.
	URHO3D_LOGINFOF( "orb_max_: %f", sbody.orb_max_.ToDouble() );
	URHO3D_LOGINFOF( "orb_max_: %f", sbody.orb_min_.ToDouble() );
	URHO3D_LOGINFOF( "eccentricity_: %f", sbody.eccentricity_.ToDouble() );
	URHO3D_LOGINFOF( "axial_tilt_: %f", sbody.axial_tilt_.ToDouble() );
	URHO3D_LOGINFOF( "semimajor_axis_: %f", sbody.semimajor_axis_.ToDouble() );
	URHO3D_LOGINFOF( "rotation_period_: %f", sbody.rotation_period_.ToDouble() );
	URHO3D_LOGINFOF( "inclination_: %f", sbody.inclination_.ToDouble() ); // Orbit inclination.
	URHO3D_LOGINFOF( "start_eccentric_anomaly_: %f", sbody.start_eccentric_anomaly_.ToDouble() );
	URHO3D_LOGINFOF( "Y_: %f", sbody.Y_ );
	URHO3D_LOGINFOF( "X_: %f", sbody.X_ );

	URHO3D_LOGINFOF( "atmos_density_: %f", sbody.atmos_density_.ToDouble() );
	//Color atmos_color_;

	URHO3D_LOGINFO( "" );

	const unsigned qty = bodies_.Size();
	const String childSpacing = spacing + String( "    " );
	for ( unsigned i=0; i<qty; i++ )
	{
		const PiSourceDesc & childBody = bodies_[i];
		if ( childBody.parent_ind_ != bodyInd )
			continue;
		dumpBody( childSpacing, childBody, i );
	}
}



}








