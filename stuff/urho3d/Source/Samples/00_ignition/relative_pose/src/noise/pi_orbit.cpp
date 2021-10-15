// Copyright © 2008-2020 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_orbit.h"
#include "pi_consts.h"
#include "pi_float_comparison.h"

namespace Ign
{

Float PiOrbit::OrbitalPeriod(Float semiMajorAxis, Float centralMass)
{
	return 2.0 * M_PI * sqrt((semiMajorAxis * semiMajorAxis * semiMajorAxis) / (G * centralMass));
}

Float PiOrbit::OrbitalPeriodTwoBody(Float semiMajorAxis, Float totalMass, Float bodyMass)
{
	// variable names according to the formula in:
	// http://en.wikipedia.org/wiki/Barycentric_coordinates_(astronomy)#Two-body_problem
	//
	// We have a 2-body orbital system, represented as a gravpoint (at the barycentre),
	// plus two bodies, each orbiting that gravpoint.
	// We need to compute the orbital period, given the semi-major axis of one body's orbit
	// around the gravpoint, the total mass of the system, and the mass of the body.
	//
	// According to Kepler, the orbital period P is defined by:
	//
	// P = 2*pi * sqrt( a**3 / G*(M1 + M2) )
	//
	// where a is the semi-major axis of the orbit, M1 is the mass of the primary and M2 is
	// the mass of the secondary. But we don't have that semi-major axis value, we have the
	// the semi-major axis for the orbit of the secondary around the gravpoint, instead.
	//
	// So, this first computes the semi-major axis of the secondary's orbit around the primary,
	// and then uses the above formula to compute the orbital period.
	const Float r1 = semiMajorAxis;
	const Float m2 = (totalMass - bodyMass);
	const Float a = r1 * totalMass / m2;
	const Float a3 = a * a * a;
	return 2.0 * M_PI * sqrt(a3 / (G * totalMass));
}

static Float calc_velocity_area_per_sec(Float semiMajorAxis, Float centralMass, Float eccentricity)
{
	const Float a2 = semiMajorAxis * semiMajorAxis;
	const Float e2 = eccentricity * eccentricity;
	return M_PI * a2 * sqrt((eccentricity < 1.0) ? (1 - e2) : (e2 - 1.0)) / PiOrbit::OrbitalPeriod(semiMajorAxis, centralMass);
}

static Float calc_velocity_area_per_sec_gravpoint(Float semiMajorAxis, Float totalMass, Float bodyMass, Float eccentricity)
{
	const Float a2 = semiMajorAxis * semiMajorAxis;
	const Float e2 = eccentricity * eccentricity;
	return M_PI * a2 * sqrt((eccentricity < 1.0) ? (1 - e2) : (e2 - 1.0)) / PiOrbit::OrbitalPeriodTwoBody(semiMajorAxis, totalMass, bodyMass);
}

static void calc_position_from_mean_anomaly(const Float M, const Float e, const Float a, Float &cos_v, Float &sin_v, Float *r)
{
	// M is mean anomaly
	// e is eccentricity
	// a is semi-major axis

	cos_v = 0.0;
	sin_v = 0.0;
	if (r) {
		*r = 0.0;
	}

	if (e < 1.0) { // elliptic orbit
		// eccentric anomaly
		// NR method to solve for E: M = E-e*sin(E)  {Kepler's equation}
		Float E = M;
		int iter;
		for (iter = 0; iter < 10; iter++) {
			Float dE = (E - e * (sin(E)) - M) / (1.0 - e * cos(E));
			E = E - dE;
			if (fabs(dE) < 0.0001) break;
		}
		// method above sometimes can't find the solution
		// especially when e approaches 1
		if (iter == 10) { // most likely no solution found
			//failsafe to bisection method
			//max(E - M) == 1, so safe interval is M+-1.1
			Float Emin = M - 1.1;
			Float Emax = M + 1.1;
			Float Ymin = Emin - e * sin(Emin) - M;
			Float Ymax = Emax - e * sin(Emax) - M;
			Float Y;
			for (int i = 0; i < 14; i++) { // 14 iterations for precision 0.00006
				E = (Emin + Emax) / 2;
				Y = E - e * sin(E) - M;
				if ((Ymin * Y) < 0) {
					Ymax = Y;
					Emax = E;
				} else {
					Ymin = Y;
					Emin = E;
				}
			}
		}

		// true anomaly (angle of orbit position)
		cos_v = (cos(E) - e) / (1.0 - e * cos(E));
		sin_v = (sqrt(1.0 - e * e) * sin(E)) / (1.0 - e * cos(E));

		// heliocentric distance
		if (r) {
			*r = a * (1.0 - e * cos(E));
		}

	} else { // parabolic or hyperbolic orbit
		// eccentric anomaly
		// NR method to solve for E: M = E-sinh(E)
		// sinh E and cosh E are solved directly, because of inherent numerical instability of tanh(k arctanh x)
		Float sh = 2.0;
		for (int iter = 50; iter > 0; --iter) {
			Float d_sh = (M + e * sh - asinh(sh)) / (e - 1 / sqrt(1 + (sh * sh)));
			sh = sh - d_sh;
			if (fabs(d_sh) < 0.0001) break;
		}

		Float ch = sqrt(1 + sh * sh);

		// true anomaly (angle of orbit position)
		cos_v = (ch - e) / (1.0 - e * ch);
		sin_v = (sqrt(e * e - 1.0) * sh) / (e * ch - 1.0);

		if (r) { // heliocentric distance
			*r = a * (e * ch - 1.0);
		}
	}
}

Float PiOrbit::TrueAnomalyFromMeanAnomaly(Float MeanAnomaly) const
{
	Float cos_v, sin_v;
	calc_position_from_mean_anomaly(MeanAnomaly, m_eccentricity, m_semiMajorAxis, cos_v, sin_v, 0);
	return atan2(sin_v, cos_v);
}

Float PiOrbit::MeanAnomalyFromTrueAnomaly(Float trueAnomaly) const
{
	Float M_t0;
	const Float e = m_eccentricity;
	if (e < 1.0) {
		M_t0 = 2.0 * atan(tan(trueAnomaly / 2.0) * sqrt((1.0 - e) / (1.0 + e)));
		M_t0 = M_t0 - e * sin(M_t0);
	} else {
		// For hyperbolic trajectories, mean anomaly has opposite sign to true anomaly, therefore trajectories which go forward
		// in time decrease their true anomaly. Yes, it is confusing.
		M_t0 = 2.0 * atanh(tan(trueAnomaly / 2.0) * sqrt((e - 1.0) / (1.0 + e)));
		M_t0 = M_t0 - e * sinh(M_t0);
	}

	return M_t0;
}

Float PiOrbit::MeanAnomalyAtTime(Float time) const
{
	const Float e = m_eccentricity;
	if (e < 1.0) { // elliptic orbit
		return 2.0 * M_PI * time / Period() + m_orbitalPhaseAtStart;
	} else {
		return -2.0 * time * m_velocityAreaPerSecond / (m_semiMajorAxis * m_semiMajorAxis * sqrt(e * e - 1)) + m_orbitalPhaseAtStart;
	}
}

Vector3d PiOrbit::OrbitalPosAtTime(Float t) const
{
	Float cos_v, sin_v, r;
	calc_position_from_mean_anomaly(MeanAnomalyAtTime(t), m_eccentricity, m_semiMajorAxis, cos_v, sin_v, &r);
	return m_orient * Vector3d(-cos_v * r, sin_v * r, 0);
}

Float PiOrbit::OrbitalTimeAtPos(const Vector3d &pos, Float centralMass) const
{
	Float c = m_eccentricity * m_semiMajorAxis;
	Matrix3d matrixInv = m_orient.Inverse();
	Vector3d approx3dPos = (matrixInv * pos - Vector3d(c, 0., 0.)).Normalized();

	Float cos_v = -Vector3d(1., 0., 0.).DotProduct(approx3dPos);
	Float sin_v = std::copysign(Vector3d(1., 0., 0.).CrossProduct(approx3dPos).Length(), approx3dPos.y_);

	Float cos_E = (cos_v + m_eccentricity) / (1. + m_eccentricity * cos_v);
	Float E;
	Float meanAnomaly;
	if (m_eccentricity <= 1.) {
		E = std::acos(cos_E);
		if (sin_v < 0)
			E *= -1.;
		meanAnomaly = E - m_eccentricity * std::sin(E);
	} else {
		E = std::acosh(cos_E);
		if (sin_v < 0)
			E *= -1.;
		meanAnomaly = E - m_eccentricity * std::sinh(E);
	}

	if (m_eccentricity <= 1.) {
		meanAnomaly -= m_orbitalPhaseAtStart;
		while (meanAnomaly < 0)
			meanAnomaly += 2. * M_PI;
	} else if (meanAnomaly < 0.)
		meanAnomaly += m_orbitalPhaseAtStart;

	if (m_eccentricity <= 1.)
		return meanAnomaly * Period() / (2. * M_PI);
	else if (meanAnomaly < 0.)
		return -meanAnomaly * std::sqrt(std::pow(m_semiMajorAxis, 3) / (G * centralMass));
	else
		return -std::fabs(meanAnomaly + m_orbitalPhaseAtStart) * std::sqrt(std::pow(m_semiMajorAxis, 3) / (G * centralMass));
}

Vector3d PiOrbit::OrbitalVelocityAtTime(Float totalMass, Float t) const
{
	Float cos_v, sin_v, r;
	calc_position_from_mean_anomaly(MeanAnomalyAtTime(t), m_eccentricity, m_semiMajorAxis, cos_v, sin_v, &r);

	Float mi = G * totalMass;
	Float p;
	if (m_eccentricity <= 1.)
		p = (1. - m_eccentricity * m_eccentricity) * m_semiMajorAxis;
	else
		p = (m_eccentricity * m_eccentricity - 1.) * m_semiMajorAxis;

	Float h = std::sqrt(mi / p);

	return m_orient * Vector3d(h * sin_v, h * (m_eccentricity + cos_v), 0);
}

// used for stepping through the orbit in small fractions
// mean anomaly <-> true anomaly conversion doesn't have
// to be taken into account
Vector3d PiOrbit::EvenSpacedPosTrajectory(Float t, Float timeOffset) const
{
	const Float e = m_eccentricity;
	Float v = 2 * M_PI * t + TrueAnomalyFromMeanAnomaly(MeanAnomalyAtTime(timeOffset));
	Float r;

	if (e < 1.0) {
		r = m_semiMajorAxis * (1 - e * e) / (1 + e * cos(v));
	} else {
		r = m_semiMajorAxis * (e * e - 1) / (1 + e * cos(v));

		// planet is in infinity
		const Float ac = acos(-1 / e);
		if (v <= -ac) {
			v = -ac + 0.0001;
			r = 100.0 * AU;
		}
		if (v >= ac) {
			v = ac - 0.0001;
			r = 100.0 * AU;
		}
	}

	return m_orient * Vector3d(-cos(v) * r, sin(v) * r, 0);
}

Float PiOrbit::Period() const
{
	if (m_eccentricity < 1 && m_eccentricity >= 0) {
		return M_PI * m_semiMajorAxis * m_semiMajorAxis * sqrt(1 - m_eccentricity * m_eccentricity) / m_velocityAreaPerSecond;
	} else { // hyperbola.. period makes no sense, should not be used
		assert(0);
		return 0;
	}
}

Vector3d PiOrbit::Apogeum() const
{
	if (m_eccentricity < 1) {
		return m_semiMajorAxis * (1 + m_eccentricity) * (m_orient * Vector3d(1, 0, 0));
	} else {
		return Vector3d(0, 0, 0);
	}
}

Vector3d PiOrbit::Perigeum() const
{
	if (m_eccentricity < 1) {
		return m_semiMajorAxis * (1 - m_eccentricity) * (m_orient * Vector3d(-1, 0, 0));
	} else {
		return m_semiMajorAxis * (m_eccentricity - 1) * (m_orient * Vector3d(-1, 0, 0));
	}
}

void PiOrbit::SetShapeAroundBarycentre(Float semiMajorAxis, Float totalMass, Float bodyMass, Float eccentricity)
{
	m_semiMajorAxis = semiMajorAxis;
	m_eccentricity = eccentricity;
	m_velocityAreaPerSecond = calc_velocity_area_per_sec_gravpoint(semiMajorAxis, totalMass, bodyMass, eccentricity);
}

void PiOrbit::SetShapeAroundPrimary(Float semiMajorAxis, Float centralMass, Float eccentricity)
{
	m_semiMajorAxis = semiMajorAxis;
	m_eccentricity = eccentricity;
	m_velocityAreaPerSecond = calc_velocity_area_per_sec(semiMajorAxis, centralMass, eccentricity);
}

PiOrbit PiOrbit::FromBodyState(const Vector3d &pos, const Vector3d &vel, Float centralMass)
{
	PiOrbit ret;

	const Float r_now = pos.Length() + 1e-12;
	const Float v_now = vel.Length() + 1e-12;

	// standard gravitational parameter
	const Float u = centralMass * G;

	// angular momentum
	const Vector3d ang = pos.CrossProduct(vel);
	const Float LLSqr = ang.LengthSquared();
	const Float LL = ang.Length();

	// total energy
	const Float EE = vel.LengthSquared() / 2.0 - u / r_now;

	if (is_zero_general(centralMass) || is_zero_general(r_now) || is_zero_general(v_now) || is_zero_general(EE) || (ang.z_ * ang.z_ / LLSqr > 1.0))
	{
		ret.m_eccentricity = 0.0;
		ret.m_semiMajorAxis = 0.0;
		ret.m_velocityAreaPerSecond = 0.0;
		ret.m_orbitalPhaseAtStart = 0.0;
		ret.m_orient = Matrix3d::IDENTITY;
		return ret;
	}

	// http://en.wikipedia.org/wiki/Orbital_eccentricity
	ret.m_eccentricity = 1 + 2 * EE * LLSqr / (u * u);
	if (ret.m_eccentricity < 0.0) ret.m_eccentricity = 0.0;
	ret.m_eccentricity = sqrt(ret.m_eccentricity);
	//avoid parabola
	if (ret.m_eccentricity < 1.0001 && ret.m_eccentricity >= 1) ret.m_eccentricity = 1.0001;
	if (ret.m_eccentricity > 0.9999 && ret.m_eccentricity < 1) ret.m_eccentricity = 0.9999;

	// lines represent these quantities:
	// 		(e M G)^2
	// 		M G (e - 1) / 2 EE, always positive (EE and (e-1) change sign
	// 		M G / 2 EE,
	// which is a (http://en.wikipedia.org/wiki/Semi-major_axis); a of hyperbola is taken as positive here
	ret.m_semiMajorAxis = 2 * EE * LLSqr + u * u;
	if (ret.m_semiMajorAxis < 0) ret.m_semiMajorAxis = 0;
	ret.m_semiMajorAxis = (sqrt(ret.m_semiMajorAxis) - u) / (2 * EE);
	ret.m_semiMajorAxis = ret.m_semiMajorAxis / fabs(1.0 - ret.m_eccentricity);

	// clipping of the eccentricity leads to a strong decrease in the semimajor axis.
	// at low speed, since the ship is almost in the apocenter, semimajor axis should be
	// almost equal to half distance to the star (no less that's for sure)
	if (ret.m_eccentricity < 1 && ret.m_semiMajorAxis < r_now / 2) ret.m_semiMajorAxis = r_now / 2;

	// The formulas for rotation matrix were derived based on following assumptions:
	//	1. Trajectory follows Kepler's law and vector {-r cos(v), -r sin(v), 0}, r(t) and v(t) are parameters.
	//	2. Correct transformation must transform {0,0,LL} to ang and {-r_now cos(orbitalPhaseAtStart), -r_now sin(orbitalPhaseAtStart), 0} to pos.
	//  3. orbitalPhaseAtStart (=offset) is calculated from r = a ((e^2 - 1)/(1 + e cos(v) ))
	Float off = 0;

	if (ret.m_eccentricity < 1) {
		off = ret.m_semiMajorAxis * (1 - ret.m_eccentricity * ret.m_eccentricity) - r_now;
	} else {
		off = ret.m_semiMajorAxis * (-1 + ret.m_eccentricity * ret.m_eccentricity) - r_now;
	}

	// correct sign of offset is given by sign pos.Dot(vel) (heading towards apohelion or perihelion?]
	off = Clamp(off / (r_now * ret.m_eccentricity), -1.0, 1.0);
	off = -pos.DotProduct(vel) / fabs(pos.DotProduct(vel)) * acos(off);

	//much simpler and satisfies the specified conditions
	//and does not have unstable places (almost almost)
	Vector3d b1 = -pos.Normalized(); //x
	Vector3d b2 = -ang.Normalized(); //z
	//ret.m_orient = Matrix3d::FromVectors(b1, b2.Cross(b1), b2) * Matrix3d::RotateZ(-off).Transpose();

	ret.m_velocityAreaPerSecond = calc_velocity_area_per_sec(ret.m_semiMajorAxis, centralMass, ret.m_eccentricity);

	ret.m_orbitalPhaseAtStart = ret.MeanAnomalyFromTrueAnomaly(-off);

	return ret;
}



}



