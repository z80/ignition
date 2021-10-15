// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#define NOMINMAX

#include "pi_source.h"
#include <algorithm> // for std::max

namespace Ign
{

template <>
const char *TerrainHeightFractal<TerrainHeightEllipsoid>::GetHeightFractalName() const { return "Ellipsoid"; }

template <>
TerrainHeightFractal<TerrainHeightEllipsoid>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	//const Float rad = m_minBody.radius_.ToDouble();
	//m_maxHeight = m_minBody.aspectRatio_.ToDouble() - 1.0;
	//m_maxHeightInMeters = m_maxHeight * rad;
	//m_invMaxHeight = 1.0 / m_maxHeight;
}

// This returns the height assuming the body is an ellipsoid (oblate spheroid) with an equatorial bulge
// Where the equator lies on the plane perpendicular to the axis of rotation which contains the center of mass (body center)

// Equatorial bulge
// Due to revolution the centrifugal force at the equator of a body is much greater (centrifugal force = angular velocity in rad/s ^2 * radius)
// This causes the equator to bulge more with increasing angular velocity (revolutions/second)
// 1. The equatorial radius is the distance from the center at the equator
// 2. The polar radius is the distance from the center to a point at the pole
// 3. The crossection involving the two poles and the equator forms an _ellipse_ whose center is at the planet center
//       Coord system: x_ along the major axis (along the equatorial radius (er) line) with x_ axis touching two points at the equator
//                     y_ along the minor axis (along the polar radius (pr) line) with the y_axis touching the two poles
//                     Origin (x_=y_=0)at the planet center
//       Aspect ratio of ellipse (ar) = major axis/minor axis = er/pr (eqn. 1) (http://en.wikipedia.org/wiki/Aspect_ratio#Ellipses)
//       Equation of ellipse: Polar form : R(t) = er*pr/sqrt((er*cos(t))^2+(pr*sin(t))^2) (eqn. 2) where t is the angle with the +x_ axis direction and R is the distance from the center.
//       For the terrain code GetHeight() returns the displacement (along the center to surface line) at each point of a unit sphere the terrain has.
//          * This geometry is resized by the 'radius' of the body. The radius value used for bodies with flattening is the short Polar radius which fits inside the elipsoid.
//          * Therefore for ellipsoid bodies the idea is that GetHeight returns the difference at any point between a sphere of size Polar radius and the ellipsoid surface.
// 4. Pioneer's coordinate system defines a body with the y axis (p.y_) as the up direction (perpendicular to the plane of rotation in which the equator lies).
//       * x (p.x_) and z (p.z_) axes lie on the plane of the equator
//       * GetHeight sees coordinates scaled such that the unit sphere is the Polar radius. That is an input vector p = Vector3d(x,y,z) on the unit sphere.
//       * In GetHeight coordinate system: pr = 1.0 (eqn. 4)
//           Ellipse coord system:y_ axis = p.y_ axis
//                                x_ axis is the axis perpendicular to y_ (p.y_) i.e. lies on the p.x_/p.z_ plane
//                                x^2 = size x_ axis coord^2 = size(p.x_,p.z_)^2 = sqrt(p.x_^2+p.y_^2)^2 = (p.x_^2+p.z_^2) (eqn. 5)
//			 Polar form equation: R(t) = er*pr/sqrt((pr*cos(t))^2+(er*sin(t))^2) (eqn. 2)
//                                R(t) = (ar*1.0)*(1.0)/sqrt(((1.0)*cos(t))^2+((ar*1.0)*sin(t))^2) (substituting using eqn. 1 and eqn. 4)
//                                R(t) = ar/sqrt(cos(t)^2+ar^2*sin(t)^2) (eqn. 6)
//
//                                cos(t)^2 = (x_/size(x_,y_))^2 = size(x_)^2/size(x_,y_)^2 = x_^2/1.0^2
//                                cos(t)^2 = x_^2 (eqn. 7)
//                                sin(t)^2 = (y_/size(x_,y_))^2 = size(y_)^2/size(x_,y_)^2 = y_^2/1.0^2
//                                sin(t)^2 = y_^2 (eqn . 8)
//
//                                R(t) = ar/sqrt(x_^2+ar^2*y_^2) (eqn. 9) (substituting using eqn. 7 and eqn. 8)

template <>
Float TerrainHeightFractal<TerrainHeightEllipsoid>::GetHeight(const Vector3d &p) const
{
	const Float ar = m_minBody.aspectRatio_.ToDouble();
	// x_^2 = (p.z_^2+p.x_^2) (eqn. 5)
	const Float x_squared = (p.x_ * p.x_ + p.z_ * p.z_);
	// y_ = p.y_
	const Float y_squared = p.y_ * p.y_;
	const Float distFromCenter_R = ar / sqrt(x_squared + ar * ar * y_squared); // (eqn. 9)
	// GetHeight must return the difference in the distance from center between a point in a sphere of
	// Polar radius (in coords scaled to a unit sphere) and the point on the ellipsoid surface.
	return std::max(distFromCenter_R - 1.0, 0.0);
}

}
