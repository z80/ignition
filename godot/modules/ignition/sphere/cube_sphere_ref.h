
#ifndef __CUBE_SPHERE_REF_H_
#define __CUBE_SPHERE_REF_H_

#include "cube_sphere.h"
#include "core/reference.h"

namespace Ign
{

class CubeSphereRef: public Reference
{
    GDCLASS(CubeSphereRef, Reference);
    OBJ_CATEGORY("Ignition");
protected:
    static void _bind_methods();
public:
    CubeSphereRef();
    ~CubeSphereRef();

    void set_height_source( const Ref<HeightSourceRef> & hs );
    Ref<HeightSourceRef> get_height_source() const;

    void set_r( real_t r );
    real_t get_r() const;

    void set_h( real_t h );
    real_t get_h() const;

    void set_distance_scaler( Ref<DistanceScalerRef> new_scaler );
    Ref<DistanceScalerRef> get_distance_scaler() const;

    void set_apply_scale( bool en );
    bool get_apply_scale() const;

    // How far "close"/"far" scaling mode shifts from one to another.
    void set_scale_mode_distance( real_t radie );
    real_t get_scale_mode_distance() const;

    void locate_mesh( Node * sphere_ref_frame, Node * ref_frame, Ref<Reference> subdivide_source_ref, Node * mesh_instance_node );
    void generate_mesh( Node * sphere_ref_frame, Node * ref_frame, Ref<Reference> subdivide_source_ref, Node * mesh_instance_node );

    const PoolVector3Array & collision_triangles( Node * sphere_ref_frame, Node * ref_frame, Ref<Reference> subdivide_source_ref );

public:
    CubeSphere sphere;

};




}






#endif



