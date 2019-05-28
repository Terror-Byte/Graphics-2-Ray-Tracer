/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
* 
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include "Triangle.h"

Triangle::Triangle()
{
	m_vertices[0] = Vector3(-1.0, 0.0, -5.0);
	m_vertices[1] = Vector3(0.0, 1.0, -5.0);
	m_vertices[2] = Vector3(1.0, 0.0, -5.0);
	m_normal = Vector3(0.0, 0.0, 1.0);
	m_primtype = PRIMTYPE_Triangle;
}

Triangle::Triangle(Vector3 pos1, Vector3 pos2, Vector3 pos3)
{
	SetTriangle(pos1, pos2, pos3);

	m_primtype = PRIMTYPE_Triangle;
}


Triangle::~Triangle()
{
}

void Triangle::SetTriangle(Vector3 v0, Vector3 v1, Vector3 v2)
{
	m_vertices[0] = v0;
	m_vertices[1] = v1;
	m_vertices[2] = v2;

	//Calculate Normal
	Vector3 NormalA = m_vertices[1] - m_vertices[0];
	Vector3 NormalB = m_vertices[2] - m_vertices[0];
	Vector3 Norm = NormalA.CrossProduct(NormalB);
	Norm.Normalise();
	m_normal = Norm;
}

RayHitResult Triangle::IntersectByRay(Ray& ray)
{
	RayHitResult result = Ray::s_defaultHitResult;
	double t = FARFAR_AWAY;
	Vector3 intersection_point;
	
	// TODO: Calculate the intersection between in the input ray and this triangle
	// If you have implemented ray-plane intersection, you have done half the work for ray-triangle intersection.
	// The remaining test is to check if the intersection point is inside the triangle
	//
	// Similar to the plane case, you should check if the ray is parallel to the triangle
	// and check if the ray intersects the triangle from the front or the back

	// If the dot product of the ray and the triangle's normal is larger than 0, they are travelling in a smiliar direction and therefore the ray is intersecting from the back of the triangle.
	// If the dot product is equal to 0, then the ray and the normal are perpendicular, meaning the ray is parallel to the surface of the triangle.
	// If the dot product is less than 0, then the ray and the normal are travelling in opposite directions, meaning the ray is intersecting from the front of the triangle, and thus we should
	// calculate the exact location of intersection.
	if (ray.GetRay().DotProduct(m_normal) < 0)
	{
		double d = -m_vertices[0].DotProduct(m_normal);
		double top = ray.GetRayStart().DotProduct(m_normal) + d;
		double bottom = ray.GetRay().DotProduct(m_normal);
		t = -((top) / (bottom));

		//Calculate the exact location of the intersection using the result of t
		intersection_point = ray.GetRayStart() + ray.GetRay()*t;

		/* Barycentric Links: - http://www.blackpawn.com/texts/pointinpoly/
		- http://facultyfp.salisbury.edu/despickler/personal/Resources/Graphics/Resources/barycentric.pdf
		(this implementation is from the first link)
		vector0 is the vector of m_vertices[0] to m_vertices[2]
		vector1 is the vector of m_vertices[0] to m_vertices[1]
		vector2 is the vector of m_vertices[0] to the intersection point of the ray
		*/
		Vector3 vector0 = m_vertices[2].operator-(m_vertices[0]); // point 0 to point 2
		Vector3 vector1 = m_vertices[1].operator-(m_vertices[0]); // point 0 to point 1
		Vector3 vector2 = intersection_point.operator-(m_vertices[0]); // point 0 to intersection point

		// Computes Dot products
		double dot00 = vector0.DotProduct(vector0);
		double dot01 = vector0.DotProduct(vector1);
		double dot02 = vector0.DotProduct(vector2);
		double dot11 = vector1.DotProduct(vector1);
		double dot12 = vector1.DotProduct(vector2);

		// Computes Barycentric coordinates
		double invDenom = 1 / (dot00 * dot11 - dot01 * dot01); // Inverse Denominator
		double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// If point is outside zero, reset t
		if (u < 0 || v < 0 || u + v >= 1)
		{
			t = FARFAR_AWAY;
			intersection_point = ray.GetRayStart() + ray.GetRay() * t;
		}
	}

	if (t > 0 && t < FARFAR_AWAY) { //ray intersection
		result.t = t;
		result.normal = this->m_normal;
		result.point = intersection_point;
		result.data = this;
		return result;
	}
	
	return result;
}