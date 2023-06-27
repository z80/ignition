
#include "ref_frame_non_inertial_node.h"
#include "scene/3d/node_3d.h"
#include "save_load.h"

#include "ref_frame_motion_node.h"
#include "ref_frame_rotation_node.h"
#include "ref_frame_body_node.h"
#include "ref_frame_assembly_node.h"


namespace Ign
{

void RefFrameNonInertialNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_physics_integration", "en" ), &RefFrameNonInertialNode::set_physics_integration );
	ClassDB::bind_method( D_METHOD( "get_physics_integration" ),       &RefFrameNonInertialNode::get_physics_integration );

	ClassDB::bind_method( D_METHOD( "set_physics_mode", "en" ), &RefFrameNonInertialNode::set_physics_mode );
	ClassDB::bind_method( D_METHOD( "get_physics_mode" ),       &RefFrameNonInertialNode::get_physics_mode );

	ClassDB::bind_method( D_METHOD( "set_time_step", "en" ), &RefFrameNonInertialNode::set_time_step );
	ClassDB::bind_method( D_METHOD( "get_time_step" ),       &RefFrameNonInertialNode::get_time_step );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,  "physics_integration" ), "set_physics_integration", "get_physics_integration" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,  "physics_mode" ),        "set_physics_mode",        "get_physics_mode" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "time_step" ),           "set_time_step",           "get_time_step" );
}

void RefFrameNonInertialNode::_notification( int p_notification )
{
	switch (p_notification)
	{
		case NOTIFICATION_READY:
		{
			_refresh_force_source_nodes();
			break;
		}
	}
}

RefFrameNonInertialNode::RefFrameNonInertialNode()
	: RefFrameMotionNode()
{
	physics_integration = false;
	physics_mode = true;
}

RefFrameNonInertialNode::~RefFrameNonInertialNode()
{
}

void RefFrameNonInertialNode::set_physics_integration( bool en )
{
	physics_integration = en;
}

bool RefFrameNonInertialNode::get_physics_integration() const
{
	return physics_integration;
}

void RefFrameNonInertialNode::set_physics_mode( bool en )
{
	physics_mode = en;
}

bool RefFrameNonInertialNode::get_physics_mode() const
{
	return physics_mode;
}

void RefFrameNonInertialNode::set_time_step( real_t dt )
{
	nm.time_step = dt;
}

real_t RefFrameNonInertialNode::get_time_step() const
{
	return nm.time_step;
}

Dictionary RefFrameNonInertialNode::serialize()
{
	Dictionary ret_data = RefFrameMotionNode::serialize();

	ret_data["physics_mode"]     = physics_mode;
	ret_data["numerical_motion"] = nm.serialize();

	return ret_data;
}

bool RefFrameNonInertialNode::deserialize( const Dictionary & new_data )
{
	{
		const bool ok = RefFrameMotionNode::deserialize( new_data );
		if ( !ok )
			return false;
	}

	physics_mode = new_data["physics_mode"];

	{
		const Dictionary & nm_data = new_data["numerical_motion"];
		const bool ok = nm.deserialize( nm_data );
		if ( !ok )
			return false;
	}

	return true;
}

void RefFrameNonInertialNode::ign_pre_process( real_t delta )
{
	RefFrameMotionNode::ign_pre_process( delta );
}

void RefFrameNonInertialNode::ign_process( real_t delta )
{
	RefFrameMotionNode::ign_process( delta );
}

void RefFrameNonInertialNode::ign_post_process( real_t delta )
{
	RefFrameMotionNode::ign_post_process( delta );
}

void RefFrameNonInertialNode::ign_physics_pre_process( real_t delta )
{
	// For now it happens every single time as in the beginning GMs are not assigned.
	_refresh_force_source_nodes();

	if ( physics_mode )
		_refresh_body_nodes();
	else
	{
		if ( physics_integration )
			_refresh_super_body_nodes();
	}

	_compute_relative_se3s();
	const Vector3d combined_centrifugal_acc = _compute_combined_acc();
	_compute_all_accelerations( combined_centrifugal_acc );

	if ( !physics_mode )
		_integrate_super_bodies( delta );

	RefFrameMotionNode::ign_physics_pre_process( delta );
}

void RefFrameNonInertialNode::ign_physics_process( real_t delta )
{
	RefFrameMotionNode::ign_physics_process( delta );
}

void RefFrameNonInertialNode::ign_physics_post_process( real_t delta )
{
	RefFrameMotionNode::ign_physics_post_process( delta );
}



void RefFrameNonInertialNode::_refresh_force_source_nodes()
{
	parent_gms.clear();
	parent_rots.clear();
	all_gms.clear();

	// All GM's.
	{
		const String name = RefFrameMotionNode::get_class_static();
		SceneTree * tree = get_tree();
		nodes.clear();
		tree->get_nodes_in_group( name, &nodes );
		for ( const List<Node *>::Element * E=nodes.front(); E; E=E->next() )
		{
			Node * n = E->get();
			RefFrameMotionNode * m = Object::cast_to<RefFrameMotionNode>( n );
			const Float gm = m->cm.own_gm;
			if ( gm > 0.0 )
			{
				MotionPair mp;
				mp.node = m;
				all_gms.push_back( mp );
			}
		}
	}

	// Parent GM's which are currently orbiting.
	{
		RefFrameNode * n = this;
		RefFrameNode * p = Object::cast_to<RefFrameNode>( this->get_parent() );

		{
			// Check if the exact parent is rotation node.
			RefFrameRotationNode * rot = Object::cast_to<RefFrameRotationNode>( p );
			if ( rot != nullptr )
			{
				// If yes, write it down and shift down.
				RotationPair rp;
				rp.node = rot;
				parent_rots.push_back( rp );
				n = Object::cast_to<RefFrameNode>( p->get_parent() );
				if ( n != nullptr )
				{
					p = Object::cast_to<RefFrameNode>( n->get_parent() );
				}
			}
		}

		while ( (n != nullptr) || (p != nullptr) )
		{
			RefFrameMotionNode * gm_n = Object::cast_to<RefFrameMotionNode>( n );
			if (gm_n != nullptr)
			{
				const bool orbiting = gm_n->is_orbiting();
				if ( orbiting )
				{
					MotionPair mp;
					mp.node = gm_n;
					parent_gms.push_back( mp );
				}
			}
			n = p;
			if ( p != nullptr )
				p = Object::cast_to<RefFrameNode>( p->get_parent() );
		}
	}
}

void RefFrameNonInertialNode::_refresh_body_nodes()
{
	all_bodies.clear();
	const int qty = get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * node = get_child( i );
		// Only pick bodies.
		RefFrameBodyNode * body = Object::cast_to<RefFrameBodyNode>( node );
		if ( body != nullptr )
			all_bodies.push_back( body );
	}
}

void RefFrameNonInertialNode::_refresh_super_body_nodes()
{
	all_bodies.clear();
	const int qty = get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * node = get_child( i );
		// Only pick super bodies.
		RefFrameBodyNode * body = Object::cast_to<RefFrameAssemblyNode>( node );
		if ( body != nullptr )
			all_bodies.push_back( body );
	}
}

void RefFrameNonInertialNode::_compute_relative_se3s()
{
	{
		const int qty = parent_gms.size();
		for ( int i=0; i<qty; i++ )
		{
			MotionPair & mp = parent_gms.ptrw()[i];
			RefFrameMotionNode * node = mp.node;
			mp.se3 = node->relative_( this );
		}
	}

	{
		const int qty = all_gms.size();
		for ( int i=0; i<qty; i++ )
		{
			MotionPair & mp = all_gms.ptrw()[i];
			RefFrameMotionNode * node = mp.node;
			mp.se3 = node->relative_( this );
		}
	}

	{
		const int qty = parent_rots.size();
		for ( int i=0; i<qty; i++ )
		{
			RotationPair & mp = parent_rots.ptrw()[i];
			RefFrameRotationNode * node = mp.node;
			mp.se3 = node->relative_( this );
		}
	}
}

Vector3d RefFrameNonInertialNode::_compute_combined_acc()
{
	Vector3d combined_orbital_acc = Vector3d::ZERO;

	const int qty = parent_gms.size();
	for ( int i=0; i<qty; i++ )
	{
		MotionPair & mp = parent_gms.ptrw()[i];
		const Vector3d acc_in_parent = mp.node->cm.acceleration();
		const Quaterniond inv_q = mp.node->se3_.q_.Inverse();
		const Vector3d acc_local = inv_q * acc_in_parent;
		const Vector3d acc = mp.se3.q_ * acc_local;

		// '-' because converting centripital acceleration of orbiting body to
		// centrifugal accceleration of bodies in this ref. frame.
		combined_orbital_acc -= acc;
	}

	return combined_orbital_acc;
}

void RefFrameNonInertialNode::_compute_all_accelerations( const Vector3d & combined_orbital_acc )
{
	const int qty = all_bodies.size();
	for ( int i=0; i<qty; i++ )
	{
		RefFrameBodyNode * body = all_bodies.ptrw()[i];
		_compute_one_accelearation( combined_orbital_acc, body );
	}
}

void RefFrameNonInertialNode::_compute_one_accelearation(  const Vector3d & combined_orbital_acc, RefFrameBodyNode * body )
{
	const SE3 & rf_se3   = this->se3_;
	const SE3 & body_se3 = body->se3_;
	const Quaterniond inv_rf_q = rf_se3.q_.Inverse();

	Vector3d total_acc = combined_orbital_acc;

	{
		const int qty = parent_rots.size();
		for ( int i=0; i<qty; i++ )
		{
			RotationPair & p = parent_rots.ptrw()[i];
			RefFrameRotationNode * node = p.node;
			const SE3 & rot_se3 = p.se3;

			const Vector3d negative_r = rot_se3.r_ - body_se3.r_;

			const Vector3d w = inv_rf_q * node->se3_.w_;

			const Vector3d negative_v = rot_se3.v_ - body_se3.v_;

			const Vector3d centrifugal_acc = w.CrossProduct( w.CrossProduct( negative_r ) );
			const Vector3d coriolis_acc    = 2.0 * ( w.CrossProduct( negative_v ) );

			total_acc += centrifugal_acc;
			total_acc += coriolis_acc;
		}
	}

	{
		const int qty = all_gms.size();
		for ( int i=0; i<qty; i++ )
		{
			MotionPair & p = all_gms.ptrw()[i];
			RefFrameMotionNode * node = p.node;
			const SE3 & gm_se3 = p.se3;

			const Float gm = node->cm.own_gm;

			const Vector3d r = gm_se3.r_ - body_se3.r_;
			const Float abs_r = r.Length();

			const Vector3d acc = (gm/(abs_r*abs_r*abs_r)) * r;

			total_acc += acc;
		}
	}

	body->acc = total_acc;
}

void RefFrameNonInertialNode::_integrate_super_bodies( Float delta )
{
	const int qty = all_bodies.size();
	for ( int i=0; i<qty; i++ )
	{
		RefFrameBodyNode * body = all_bodies[i];

		nm.process( body->se3_, delta, body->acc );
	}
}



}

