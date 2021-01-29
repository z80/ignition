
#include "pbd2_collisions.h"
#include "pbd2_collision_sphere.h"
#include "pbd2_collision_box.h"
#include "pbd2_collision_plane.h"

#include "pbd2_collision_utils.h"

#include "core/print_string.h"


namespace Pbd
{

static const Float EPS = 0.0001;

void collision_sphere_sphere( CollisionSphere * obj_a, CollisionSphere * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    const Pose pose_a = obj_a->pose_w();
    const Pose pose_b = obj_b->pose_w();
    const Vector3d dr = pose_a.r - pose_b.r;
    const Float dist = dr.Length();
    const Float max_dist = obj_a->radius + obj_b->radius;
    if ( dist >= max_dist )
        return;

    if ( dist < EPS )
    {
        const Vector3d at = pose_a.r;
        const Vector3d depth = Vector3d::UP * max_dist;
        ats.push_back( at );
        depths.push_back( depth );
        return;
    }

    const Vector3d a = dr / dist;
    const Vector3d at = ( (pose_b.r + a*obj_b->radius) + (pose_a.r - a*obj_a->radius) ) * 0.5;
    const Vector3d depth = pose_b.r - pose_a.r + a*(obj_b->radius + obj_a->radius);
    ats.push_back( at );
    depths.push_back( depth );
}


void collision_sphere_plane( CollisionSphere * obj_a, CollisionPlane * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    const Pose pose_sphere = obj_a->pose_w();
    const Pose pose_plane = obj_b->pose_w();
    const Float radius = obj_a->radius;
    // Plane normal in world space.
    const Vector3d n = pose_plane.q * Vector3d( 0.0, 1.0, 0.0 );

    // Compute the height.
    const Vector3d dr = pose_sphere.r - pose_plane.r;
    const Float h = n.DotProduct( dr );
	const Float dh = h - radius;
    if ( dh > -EPS )
        return;

    // The lowest sphere point.
    const Vector3d rl = pose_sphere.r - (n*radius);
    // The projection on the plane.
    const Vector3d rh = pose_sphere.r - (n*h);
    // Collision point.
    const Vector3d at = (rl + rh) * 0.5;
    // Displacement.
    const Vector3d depth = rh - rl;

    ats.push_back( at );
    depths.push_back( depth );
}

void collision_sphere_box( CollisionSphere * obj_a, CollisionBox * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // Sphere in Box ref. frame.
    const Pose pose_sphere = obj_a->pose_w();
    const Pose pose_box    = obj_b->pose_w();
    const Pose pose_rel = pose_sphere / pose_box;
    const Float radius = obj_a->radius;
    // Copy-pase from ODE.
    // Clip sphere center onto the box surface and check the distance.

    bool on_border = false;
    Vector3d p = pose_rel.r;
    const Vector3d sz2 = obj_b->get_size2();
    if (p.x_ < -sz2.x_)
    {
        p.x_ = -sz2.x_;
        on_border = true;
    }
    else if (p.x_ > sz2.x_)
    {
        p.x_ = sz2.x_;
        on_border = true;
    }

    if (p.y_ < -sz2.y_)
    {
        p.y_ = -sz2.y_;
        on_border = true;
    }
    else if (p.y_ > sz2.y_)
    {
        p.y_ = sz2.y_;
        on_border = true;
    }

    if (p.z_ < -sz2.z_)
    {
        p.z_ = -sz2.z_;
        on_border = true;
    }
    else if (p.z_ > sz2.z_)
    {
        p.z_ = sz2.z_;
        on_border = true;
    }

    if ( !on_border )
    {
        // Sphere center is inside the box.
        // Find the closest face and push towards it.
        Float min_dist = std::abs( p.x_ - sz2.x_ );
        int min_ind = 0;
        const Float dist_left = std::abs( p.x_ + sz2.x_ );
        if ( dist_left < min_dist )
        {
            min_dist = dist_left;
            min_ind = 1;
        }
        const Float dist_up = std::abs( p.y_ - sz2.y_ );
        if ( dist_up < min_dist )
        {
            min_dist = dist_up;
            min_ind = 2;
        }
        const Float dist_down = std::abs( p.y_ + sz2.y_ );
        if ( min_dist = dist_down )
        {
            min_dist = dist_down;
            min_ind = 3;
        }
        const Float dist_back = std::abs( p.z_ - sz2.z_ );
        if ( dist_back < min_dist )
        {
            min_dist = dist_back;
            min_ind = 4;
        }
        const Float dist_forward = std::abs( p.z_ + sz2.z_ );
        if ( dist_forward < min_dist )
        {
            min_dist = dist_forward;
            min_ind = 5;
        }
        // Two points. One on the surface, the other is the deepest point on the sphere.
        Vector3d v_surf, v_deep;
        if ( min_ind == 0 )
        {
            v_surf = Vector3d( sz2.x_, p.y_, p.z_ );
            v_deep = Vector3d( p.x_-radius, p.y_, p.z_ );
        }
        else if ( min_ind == 1 )
        {
            v_surf = Vector3d( -sz2.x_, p.y_, p.z_ );
            v_deep = Vector3d( p.x_+radius, p.y_, p.z_ );
        }
        else if ( min_ind == 2 )
        {
            v_surf = Vector3d( p.x_, sz2.y_, p.z_ );
            v_deep = Vector3d( p.x_, p.y_-radius, p.z_ );
        }
        else if ( min_ind == 3 )
        {
            v_surf = Vector3d( p.x_, -sz2.y_, p.z_ );
            v_deep = Vector3d( p.x_, p.y_+radius, p.z_ );
        }
        else if ( min_ind == 4 )
        {
            v_surf = Vector3d( p.x_, p.y_, sz2.z_ );
            v_deep = Vector3d( p.x_, p.y_, p.z_-radius );
        }
        else
        {
            v_surf = Vector3d( p.x_, p.y_, -sz2.z_ );
            v_deep = Vector3d( p.x_, p.y_, p.z_+radius );
        }
        Vector3d at = (v_surf + v_deep) * 0.5;
        Vector3d depth = (v_surf - v_deep);
        // Convert to world ref. frame.
        at = pose_box.r + (pose_box.q * at);
        depth = pose_box.q * depth;
        ats.push_back( at );
        depths.push_back( depth );
        return;
    }

    // Sphere may touch the surface point or not.
    const Vector3d dr = pose_rel.r - p;
    const Float dist = dr.Length();
    const Float depth = radius - dist;
    // If far or not deep enough, no contact.
    if ( depth < EPS )
        return;
    const Vector3d v_surf = p; 
    // Unit vector away from cube surface.
	Vector3d a = pose_rel.r - v_surf;
    a.Normalize();
    const Vector3d v_deep = pose_rel.r - (radius * a);
    Vector3d v_at = (v_surf + v_deep) * 0.5;
    Vector3d v_depth = v_surf - v_deep;
    v_at = pose_box.r + (pose_box.q * v_at);
    v_depth = pose_box.q * v_depth;
    ats.push_back( v_at );
    depths.push_back( v_depth );
}

void collision_box_plane( CollisionBox * obj_a, CollisionPlane * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // General idea.
    // Convert box to plane ref. frame.
    // Check box vertices below (y=0) plane.
    const Pose pose_box   = obj_a->pose_w();
    const Pose pose_plane = obj_b->pose_w();
    const Pose pose_rel = pose_box / pose_plane;
	Box box_a;
	box_a.init( obj_a->get_size2() );

	for ( int i=0; i<8; i++ )
    {
		const BoxVertex & bv = box_a.verts[i];
        const Vector3d v = pose_rel.r + (pose_rel.q * bv.v);
        // Check Y component. It should be below surface.
        if ( v.y_ > -EPS )
            continue;
        const Vector3d v_surf = Vector3d( v.x_, 0.0, v.z_ );
        const Vector3d v_deep = v;
        Vector3d at = (v_surf + v_deep) * 0.5;
        Vector3d depth = (v_surf - v_deep);
        at = pose_plane.r + (pose_plane.q * at);
        depth = pose_plane.q * depth;
        ats.push_back( at );
        depths.push_back( depth );

		{
			//print_line( String("box-plane depth: " ) + rtos( depth.Length() ) );
		}
    }
}

void collision_box_box( CollisionBox * obj_a, CollisionBox * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    // Ok. Box A is converted to box B ref. frame.
    // Vertices of box A are tested if they are inside of box B.
    // It is saved towards what face the displacement is.
    // All edges of A are tested agains all faces of B.
    // In the case of intersection the smallest fix is computed.
    // If at the same time a vertex of this edge is inside the box B 
    // and its fix face is the same it is checked if fix distance is smaller.
    // If vertex distance is smaller, edge-face intersection is rejected.
    // If not smaller that iti s also added to toe intersections list.
    // Also, box need to be intersected symmetrically a->b and b->a. It naturally happens 
    // in collision processing loop.
    const Pose pose_a   = obj_a->pose_w();
    const Pose pose_b = obj_b->pose_w();
    const Pose pose_rel = pose_a / pose_b;

    Box box_a;
    box_a.init( obj_a->get_size2() );
    Box box_b;
    box_b.init( obj_b->get_size2() );

    box_a.apply( pose_rel );

    for ( int i=0; i<8; i++ )
    {
        BoxVertex & v = box_a.verts[i];
        const bool inside = box_b.inside( v );
        if ( inside )
        {
            const BoxFace & f = box_b.faces[v.face_id];
            const Vector3d depth = f.n * v.depth;
            const Vector3d at = v.v + depth * 0.5;
			// Convert to world ref frame.
			const Vector3d at_w = pose_b.r + (pose_b.q * at);
			const Vector3d depth_w = pose_b.q * depth;
            ats.push_back( at_w );
            depths.push_back( depth_w );

			print_line( String("vertex id: " ) + itos(i) + String(", face id: ") + itos(v.face_id) + String(", depth: ") + rtos( depth.Length() ) );
        }
    }

    for ( int i=0; i<12; i++ )
    {
        const int ind_a = Box::vert_inds[i][0];
        const int ind_b = Box::vert_inds[i][1];
        BoxVertex a = box_a.verts[ind_a];
        BoxVertex b = box_a.verts[ind_b];
		// Cropping the edge to only box "b" internal part.
		int intersections_qty = 0;
		int intersecting_face_inds[2] = {-1, -1};
		for ( int face_b=0; face_b<6; face_b++ )
		{
			// Check if edge vertices inder the surface. And if yes,
			// if their face id is the same is for the perpendicular.
			// And if yes, reject it.
			if ( a.inside )
			{
				if ( a.face_id == face_b )
					continue;
			}
			if ( b.inside )
			{
				if ( b.face_id == face_b )
					continue;
			}

			const BoxFace & f = box_b.faces[face_b];
			Float t_at;
			Vector3d v_at;
			const bool intersects = f.intersects( a, b, t_at, v_at );
			if ( !intersects )
				continue;
			/*// Check which point to reject. Either "a" or "b" should be above.
			const Float d = f.n.DotProduct(a.v - f.center);
			if ( d >= 0.0 )
				a.v = v_at;
			else
				b.v = v_at;/**/
			intersecting_face_inds[intersections_qty] = face_b;
			intersections_qty += 1;
			// Line cannot cross cube in more than 2 points.
			// So stop checking if 2 intersections were detected.
			if ( intersections_qty >= 2 )
				break;
		}
		// At this point "a"-"b" is inside the cube.
		// Search for the shortest common perpendicular.
		int last_closest_edge_ind = -1;
		Vector3d min_va, min_vb;
		bool closest_edge_found = false;
		Float min_d2 = -1.0;
		for ( int j=0; j<intersections_qty; j++ )
		{
			const int face_ind = intersecting_face_inds[j];
			const BoxFace & f = box_b.faces[face_ind];
			for ( int face_edge_ind=0; face_edge_ind<4; face_edge_ind++ )
			{
				const int edge_ind = f.edge_inds[face_edge_ind];
				// Check if it was already tested.
				if ( last_closest_edge_ind == edge_ind )
					continue;
				const int ind_1 = Box::vert_inds[edge_ind][0];
				const int ind_2 = Box::vert_inds[edge_ind][1];
				const BoxVertex & b1 = box_b.verts[ind_1];
				const BoxVertex & b2 = box_b.verts[ind_2];
				Vector3d pa, pb;
				const bool ok = common_perp( a.v, b.v, b1.v, b2.v, pa, pb );
				if ( !ok )
					continue;
				// Point on the edge should be below face surface.
				const bool inside_a = box_a.inside_const( pb );
				if ( !inside_a )
					continue;
				// Looking for the smallest one.
				const Float len = (pb - pa).LengthSquared();
				if ( (!closest_edge_found) || (len < min_d2) )
				{
					min_d2 = len;
					min_va = pa;
					min_vb = pb;
					last_closest_edge_ind = edge_ind;
					closest_edge_found = true;
				}
			}
		}

		if ( closest_edge_found )
		{
			// Add position and depth.
			// For "a" depth is towards edge "b".
			const Vector3d at = (min_va + min_vb) * 0.5;
			const Vector3d depth = (min_vb - min_va);
			// Convert to world ref frame.
			const Vector3d at_w = pose_b.r + (pose_b.q * at);
			const Vector3d depth_w = pose_b.q * depth;
			ats.push_back( at_w );
			depths.push_back( depth_w );

			{
				print_line( String("edge-edge depth: " ) + rtos( depth.Length() ) );

				const Float d = depth.Length();
				if ( d > 0.1 )
				{
					Vector<Vector3d> ats2, depths2;
					collision_box_box( obj_a, obj_b, ats2, depths2 );
				}
			}
		}
    }
}











}







