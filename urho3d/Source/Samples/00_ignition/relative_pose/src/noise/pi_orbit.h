// Copyright © 2008-2020 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef ORBIT_H
#define ORBIT_H

#include "matrix3d.h"
#include "vector3d.h"

namespace Ign
{

class Orbit {
public:
	// utility functions for simple calculations
	static Float OrbitalPeriod(Float semiMajorAxis, Float centralMass);
	static Float OrbitalPeriodTwoBody(Float semiMajorAxis, Float totalMass, Float bodyMass);

	// note: the resulting Orbit is at the given position at t=0
	static Orbit FromBodyState(const Vector3d &position, const Vector3d &velocity, Float central_mass);

	Orbit() :
		m_eccentricity(0.0),
		m_semiMajorAxis(0.0),
		m_orbitalPhaseAtStart(0.0),
		m_velocityAreaPerSecond(0.0),
		m_orient(Matrix3d::IDENTITY)
	{}

	void SetShapeAroundBarycentre(Float semiMajorAxis, Float totalMass, Float bodyMass, Float eccentricity);
	void SetShapeAroundPrimary(Float semiMajorAxis, Float totalMass, Float eccentricity);
	void SetPlane(const Matrix3d &orient)
	{
		m_orient = orient;
		assert(!std::isnan(m_orient.m00_) && !std::isnan(m_orient.m01_) && !std::isnan(m_orient.m02_));
		assert(!std::isnan(m_orient.m10_) && !std::isnan(m_orient.m11_) && !std::isnan(m_orient.m12_));
		assert(!std::isnan(m_orient.m20_) && !std::isnan(m_orient.m21_) && !std::isnan(m_orient.m22_));
	}
	void SetPhase(Float orbitalPhaseAtStart) { m_orbitalPhaseAtStart = orbitalPhaseAtStart; }

	Vector3d OrbitalPosAtTime(Float t) const;
	Float OrbitalTimeAtPos(const Vector3d &pos, Float centralMass) const;
	Vector3d OrbitalVelocityAtTime(Float totalMass, Float t) const;

	// 0.0 <= t <= 1.0. Not for finding orbital pos
	Vector3d EvenSpacedPosTrajectory(Float t, Float timeOffset = 0) const;

	Float Period() const;
	Vector3d Apogeum() const;
	Vector3d Perigeum() const;

	// basic accessors
	Float GetEccentricity() const { return m_eccentricity; }
	Float GetSemiMajorAxis() const { return m_semiMajorAxis; }
	Float GetOrbitalPhaseAtStart() const { return m_orbitalPhaseAtStart; }
	const Matrix3d &GetPlane() const { return m_orient; }

private:
	Float TrueAnomalyFromMeanAnomaly(Float MeanAnomaly) const;
	Float MeanAnomalyFromTrueAnomaly(Float trueAnomaly) const;
	Float MeanAnomalyAtTime(Float time) const;

	Float m_eccentricity;
	Float m_semiMajorAxis;
	Float m_orbitalPhaseAtStart; // 0 to 2 pi radians
	/* dup " " --------------------------------------- */
	Float m_velocityAreaPerSecond; // seconds
	Matrix3d m_orient;
};

}

#endif
