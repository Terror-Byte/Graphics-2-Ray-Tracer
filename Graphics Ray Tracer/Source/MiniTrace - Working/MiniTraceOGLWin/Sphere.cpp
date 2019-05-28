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
#include "Sphere.h"

Sphere::Sphere()
{
	//the default sphere is centred around the origin of the world coordinate
	//default radius is 2.0

	m_centre.SetZero();
	m_radius = 2.0;
	m_primtype = PRIMTYPE_Sphere;
}

Sphere::Sphere(double x, double y, double z, double r)
{
	m_centre.SetVector(x, y, z);
	m_radius = r;
	m_primtype = PRIMTYPE_Sphere;
}

Sphere::~Sphere()
{
}

RayHitResult Sphere::IntersectByRay(Ray& ray)
{
	RayHitResult result = Ray::s_defaultHitResult;

	double t = FARFAR_AWAY;
	Vector3 normal;
	Vector3 intersection_point;

	//TODO: Calculate the intersection between the input ray and this sphere
	// Store the parametric result in t
	// The algebraic form of a sphere is  (x - cx)^2 + (y - cy)^2 + (z - cz)^2 = r^2 where
	// (x,y,z) is a point on the sphere
	// (cx, cy, cz) is the centre of the sphere.,i.e. m_centre
	// r is the radius of the sphere
	//
	// Note, the solution to ray-sphere intersection is the root(s) of a quadratic equation
	// therefore, there will be three scenarios
	// 1. Two real roots, the ray intersects the sphere twice. In this case, we are only interested in the closest intersection
	// 2. One real root, the ray tangentially intersects the sphere
	// 3. No real root, no intersection
	
	// Dis fuckin website has quadratic equation shizzle https://www.rit.edu/~w-asc/documents/services/resources/handouts/9Roots%20of%20Quadratic%20Equations.pdf

	// Variables to make the quadratic equation code much cleaner and more readable
	Vector3 sMinusC = ray.GetRayStart().operator-(m_centre);
	Vector3 rayDir = ray.GetRay();
	double rayDirDot = ray.GetRay().DotProduct(ray.GetRay());

	// Works out the descriminant of the quadratic equation
	double discriminant = pow(rayDir.DotProduct(sMinusC), 2) - (rayDirDot * (sMinusC.DotProduct(sMinusC) - pow(m_radius, 2)));

	double tPlus;
	double tMinus;
	
	/* Checks how many real roots the result of the discriminant will have.
       if the discriminant is larger than 0, then there will be 2 real roots,
	   and thus both roots are worked out. After that, it sets t to the closest
	   root to the camera.
	   If the discriminant equals 0, then there is only 1 real root, which is then
	   worked out and is stored in t;
	   If the discriminant is less than 0, then there are no real roots and the 
	   function returns. */
	if (discriminant > 0)
	{
		tPlus = (-rayDir.DotProduct(sMinusC) + sqrt(discriminant)) / rayDirDot;
		tMinus = (-rayDir.DotProduct(sMinusC) - sqrt(discriminant)) / rayDirDot;

		if (tPlus < tMinus)
			t = tPlus;
		else if (tMinus < tPlus)
			t = tMinus;
	}
	else if (discriminant == 0)
	{
		t = (-rayDir.DotProduct(sMinusC) + sqrt(discriminant)) / rayDirDot;
	}
	else if (discriminant < 0)
	{
		return result;
	}

	//Calculate the exact location of the intersection using the result of t
	intersection_point = ray.GetRayStart() + ray.GetRay()*t;
	
	//TODO: Calculate normal (DONE).
	//Normals vary across the surface of a sphere
	//You need to calculate the normal based on the location of the intersection

	// Involves centre of circle and point of intersection. m_centre and intersection_point
	normal = intersection_point - m_centre;
	normal = normal.Normalise();

	if (t>0.0 && t < FARFAR_AWAY)
	{
		result.t = t;
		result.data = this;
		result.point = intersection_point;
		result.normal = normal;
	}

	return result;
}
