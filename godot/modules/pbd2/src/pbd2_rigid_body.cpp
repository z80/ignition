

namespace Pbd
{


RigidBody::RigidBody()
{
    mass = 1.0;
    inertia     = Matrix3d::IDENTITY;
    inv_inertia = Matrix3d::IDENTITY;
}

RigidBody::~RigidBody()
{
}

RigidBody::RigidBody( const RigidBody & inst )
{
    *this = inst;
}

const RigidBody & RigidBody::operator=( const RigidBody & inst )
{
    if ( this != &inst )
    {
        pose      = inst.pose;
        prev_pose = inst.prev_pose;
        orig_pose = inst.orig_pose;
        vel       = inst.vel;
        omega     = inst.omega;

        mass        = inst.mass;
        inertia     = inst.inertia;
        inv_inertia = inst.inv_inertia;

        force       = inst.force;
        torque      = inst.torque;
        
        contact_points = inst.contact_points;
    }
    return *this;
}

void RigidBody::set_inertia( const Matrix3d & I )
{
    inertia = I;
    inv_inertia = I.Inverese();
}

Matrix33 RigidBody::I() const
{
    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d i = A.Transpose() * inertia * A;
    return i;
}

Matrix33 RigidBody::inv_I() const
{
    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d inv_i = A * inv_inertia * A.Transpose();
    return i;
}


void RigidBody::integrate_dynamics( Float h )
{
    // Save previous pose.
    prev_pose = pose;

    // Translation.
    vel += force * h;
    pose.r += vel * h;

    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d I = A.Transpose() * inertia * A;
    const Matrix3d inv_I = A * inv_inertia * A.Transpose();

    const Vector3d w_x_Iw = omega.Cross( I * omega );
    const Vector3d dw = ( inv_I * ( torque - w_x_Iw ) ) * h;
    omega += dw;

    const Float k = 0.5 * h;
    const Quaterniond dq( w.x_*k, w.y_*k, w.z_*k, 0.0 );
    pose.q += dq;
    pose.q.Normalize();
}

void RigidBody::update_velocities( Float h )
{
    vel = ( pose.r - prev_pose.r ) / h;
    Quaterniond dq = pose.q * prev_pose.q.Conjugate();
    Vector3 w = Vector3( dq.x_, dq.y_, dq.z_ );
    if ( dq.w_ < 0.0 )
        w = -w;

    w *= 2.0 / h;
}

void RigidBody::init_contact_lambdas()
{
    const int qty = contact_points.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPont & pt = contact_points.wptr()[i];
        pt.init_lambdas();
    }
}

void RigidBody::solve_contacts( Float h )
{
    const int qty = contact_points.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPont & pt = contact_points.wptr()[i];
        pt.solve( this, h );
    }
}

void RigidBody::update_contact_velocities( Float h )
{
    const int qty = contact_points.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPont & pt = contact_points.wptr()[i];
        pt->solve_dynamic_friction( this, h );
    }
}


// "r" local, "n" world.
Float RigidBody::specific_inv_mass_pos( const Vector3d & r, const Vector3d & n )
{
    if ( mass <= 0.0 )
        return 0.0;

    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d inv_I = A * inv_inertia * A.Transpose();
    const Vector3d r_world = pose.q * r;
    const Vector3d r_x_n = r_world.Cross( n );
    const Float mu = 1.0/mass + r.Dot( inv_I * r_x_n );

    return mu;
}

// "n" world.
Float RigidBody::specific_inv_mass_rot( const Vector3d & n )
{
    if ( mass <= 0.0 )
        return 0.0;

    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d inv_I = A * inv_inertia * A.Transpose();
    const Float mu = n.Dot( inv_I * n );

    return mu;
}


}

