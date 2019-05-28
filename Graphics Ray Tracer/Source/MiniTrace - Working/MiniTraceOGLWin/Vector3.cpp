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
#include "Vector3.h"

double Vector3::operator [] (const int i) const
{
	return m_element[i];
}

double& Vector3::operator [] (const int i)
{
	return m_element[i];
}

Vector3 Vector3::operator + (const Vector3& rhs) const
{
	return Vector3(
		(*this)[0] + rhs[0],
		(*this)[1] + rhs[1],
		(*this)[2] + rhs[2]);
}

Vector3 Vector3::operator - (const Vector3& rhs) const
{
	return Vector3(
		(*this)[0] - rhs[0],
		(*this)[1] - rhs[1],
		(*this)[2] - rhs[2]);
}

Vector3& Vector3::operator = (const Vector3& rhs)
{
	m_element[0] = rhs[0];
	m_element[1] = rhs[1];
	m_element[2] = rhs[2];
	return *this;
}

Vector3 Vector3::operator * (const Vector3& rhs) const
{
	return Vector3(
		m_element[0] * rhs[0],
		m_element[1] * rhs[1],
		m_element[2] * rhs[2]);
}

Vector3 Vector3::operator * (double scale) const
{
	return Vector3(
		m_element[0] * scale,
		m_element[1] * scale,
		m_element[2] * scale
		);
}

double Vector3::Norm() const
{
	return sqrtf(m_element[0] * m_element[0] + m_element[1] * m_element[1] + m_element[2] * m_element[2]);
}

double Vector3::Norm_Sqr() const
{
	return m_element[0] * m_element[0] + m_element[1] * m_element[1] + m_element[2] * m_element[2];
}

double Vector3::DotProduct(const Vector3& rhs) const
{
	return m_element[0] * rhs[0] + m_element[1] * rhs[1] + m_element[2] * rhs[2];
}

Vector3 Vector3::Normalise()
{
	double length = this->Norm();

	if (length > 1.0e-8f)
	{
		double invLen = 1.0f / length;

		m_element[0] *= invLen;
		m_element[1] *= invLen;
		m_element[2] *= invLen;
	}

	return *this;
}

Vector3 Vector3::CrossProduct(const Vector3& rhs) const
{
	return Vector3(
		(m_element[1] * rhs[2] - m_element[2] * rhs[1]),
		(m_element[2] * rhs[0] - m_element[0] * rhs[2]),
		(m_element[0] * rhs[1] - m_element[1] * rhs[0])
		);
}

Vector3 Vector3::Reflect(const Vector3& n) const
{
	Vector3 result;
	
	//TODO: Calculate the reflection of this vector given the input normal n
	//Store the result in result
	
	// result = 2(normal dot this) * normal - this
	double thisNormDot = n.DotProduct(*this);

	result[0] = -(2 * thisNormDot * n[0] - (*this)[0]);
	result[1] = -(2 * thisNormDot * n[1] - (*this)[1]);
	result[2] = -(2 * thisNormDot * n[2] - (*this)[2]);

	return result;
}

Vector3 Vector3::Refract(const Vector3& n, double r_coeff) const
{
	Vector3 result;
	
	//TODO: Calculate the refraction of this vector given the input normal n and the refractive coefficient r_index
	//Store the result in result
	//Refraction is governed by the Snell's law
	
	// r_coeff = index 1 / index 2;
	// angle1 = angle of this vector from normal
	// angle2 = angle of refracted vector from it's normal
	// coeff1 / coeff2 = (sin angle2) / (sin angle1)
	// r_coeff = (sin angle1) / (sin angle2)
	// (sin angle 2) = (sin angle 1)/coeff

	//float angle1 = this->DotProduct(n);
	//float angle1 = sin(acos(this->DotProduct(n)));
	//float angle2 = angle1 / r_coeff;
	// dot prod = a.b = cos angle
	// a = cos angle / b
	//result[0] = angle2 / n[0];
	//result[1] = angle2 / n[1];
	//result[2] = angle2 / n[2];

	// n1/n2 = sinRefracted / sinIncident
	// refracted = asin( n1/n2 * sinIncident )
	//double incidentAngle = acos(this->DotProduct(n));
	//double refractedAngle = asin(r_coeff * sin(incidentAngle));

	//result[0] = cos(-refractedAngle) / n[0];
	//result[1] = cos(-refractedAngle) / n[1];
	//result[2] = cos(-refractedAngle) / n[2];

	//Method from this link: http://www.flipcode.com/archives/reflection_transmission.pdf

	double incidentAngle = this->DotProduct(n);
	//double refractedAngle = asin(r_coeff * sin(incidentAngle));

	//double sinRefractedSqr = pow(sin(refractedAngle), 2);
	double sinRefractedSqr2 = pow(r_coeff, 2) * (1 - pow(incidentAngle, 2));

	if (sinRefractedSqr2 <= 1)
		return result = (*this * r_coeff) - (n * (r_coeff * incidentAngle + sqrt(1 - sinRefractedSqr2)));

	/*if (sinRefractedSqr == sinRefractedSqr2)
	{
		result = (*this * r_coeff) - (n * (r_coeff * cos(incidentAngle) + sqrt(1 - sinRefractedSqr2)));
	}*/

	//return result;
}

void Vector3::SetZero()
{
	m_element[0] = m_element[1] = m_element[2] = 0.0f;
}
