/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <math.h>
#include "Plane.h"


Plane::Plane()
{
	//The default plane is the xz plane with normal pointing towards positive y
	// the offset is zero,
	m_normal.SetVector(0.0, 1.0, 0.0);
	m_offset = 0.0;
	m_primtype = PRIMTYPE_Plane;
}


Plane::~Plane()
{
}

RayHitResult Plane::IntersectByRay(Ray& ray)
{
	RayHitResult result = Ray::s_defaultHitResult;
	double t = FARFAR_AWAY;
	Vector3 intersection_point;
	
	//TODO: Calculate the intersection the input ray and this plane
	// Store the parametric result in t
	// The plane equation is a*x + a*y + a*z + d = 0, where
	// a = m_normal[0], b = m_normal[1], c = m_normal[2] and d = m_offset
	//
	// Note: 
	// Some additional checks can make the test more robust
	// 1. You should check if the ray is parallel to plane
	// 2. Check if the ray intersects the plane from the front or the back

	// If the dot product of the ray and the plane's normal is larger than 0, they are travelling in a smiliar direction and therefore the ray is intersecting from the back of the plane.
	// If the dot product is equal to 0, then the ray and the normal are perpendicular, meaning the ray is parallel to the surface of the plane.
	// If the dot product is less than 0, then the ray and the normal are travelling in opposite directions, meaning the ray is intersecting from the front of the plane, and thus we should
	// calculate the exact location of intersection.
	if (ray.GetRay().DotProduct(m_normal) < 0)
	{
		double top = ray.GetRayStart().DotProduct(m_normal) + m_offset;
		double bottom = ray.GetRay().DotProduct(m_normal);
		t = -((top) / (bottom));

		//Calculate the exact location of the intersection using the result of t
		intersection_point = ray.GetRayStart() + ray.GetRay()*t;
	}
	
	if (t>0.0 && t < FARFAR_AWAY)
	{
		result.normal = m_normal;
		result.t = t;
		result.data = this;
		result.point = intersection_point;
	}

	return result;
}

void Plane::SetPlane(const Vector3& normal, double offset)
{
	m_normal = normal;
	m_offset = -offset;
}
