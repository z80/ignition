
#include "ref_frame_non_inertial_node.h"
#include "scene/3d/spatial.h"
#include "save_load.h"

#include "ref_frame_motion_node.h"
#include "ref_frame_rotation_node.h"


namespace Ign
{

void RefFrameNonInertialNode::_bind_methods()
{
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
	physics_mode = true;
}

RefFrameNonInertialNode::~RefFrameNonInertialNode()
{
}

void RefFrameNonInertialNode::set_physics_mode( bool en )
{
	physics_mode = en;
}

bool RefFrameNonInertialNode::get_physics_node() const
{
	return physics_mode;
}

void RefFrameNonInertialNode::_ign_pre_process( real_t delta )
{
	RefFrameNode::_ign_pre_process( delta );
}

void RefFrameNonInertialNode::_ign_process( real_t delta )
{
	RefFrameNode::_ign_process( delta );
}

void RefFrameNonInertialNode::_ign_post_process( real_t delta )
{
	RefFrameNode::_ign_post_process( delta );
}

void RefFrameNonInertialNode::_ign_physics_pre_process( real_t delta )
{
}

void RefFrameNonInertialNode::_ign_physics_process( real_t delta )
{
}

void RefFrameNonInertialNode::_ign_physics_post_process( real_t delta )
{
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
			p = Object::cast_to<RefFrameNode>( p->get_parent() );
		}
	}
}


void RefFrameNonInertialNode::_compute_relative_se3s()
{
	{
		const int qty = parent_gms.size();
		for ( int i=0; i<qty; i++ )
		{
			MotionPair & mp = parent_gms.get(i);
			RefFrameMotionNode * node = mp.node;
			mp.se3 = node->relative_( this );
		}
	}

	{
		const int qty = all_gms.size();
		for ( int i=0; i<qty; i++ )
		{
			MotionPair & mp = all_gms.get(i);
			RefFrameMotionNode * node = mp.node;
			mp.se3 = node->relative_( this );
		}
	}

	{
		const int qty = parent_rots.size();
		for ( int i=0; i<qty; i++ )
		{
			RotationPair & mp = parent_rots.get(i);
			RefFrameRotationNode * node = mp.node;
			mp.se3 = node->relative_( this );
		}
	}
}

void RefFrameNonInertialNode::_compute_combined_acc()
{
	_combined_orbital_acc = Vector3d::ZERO;

	const int qty = parent_gms.size();
	for ( int i=0; i<qty; i++ )
	{
		MotionPair & mp = parent_gms.get(i);
		const Vector3d acc_in_parent = mp.node->cm.acceleration();
		const Quaterniond inv_q = mp.node->se3_.q_.Inverse();
		const Vector3d acc_local = inv_q * acc_in_parent;
		const Vector3d acc = mp.se3.q_ * acc_local;

		// '-' because converting centripital acceleration of orbiting body to
		// centrifugal accceleration of bodies in this ref. frame.
		_combined_orbital_acc -= acc;
	}
}

void RefFrameNonInertialNode::_compute_all_accelerations()
{
}

void RefFrameNonInertialNode::_compute_one_accelearation( RefFrameBodyNode * body )
{
}



}

