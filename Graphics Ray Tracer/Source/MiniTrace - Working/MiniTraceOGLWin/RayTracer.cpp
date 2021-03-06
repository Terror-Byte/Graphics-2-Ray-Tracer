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

#ifdef WIN32
#include <Windows.h>
#include <gl/GL.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include "Camera.h"

RayTracer::RayTracer()
{
	m_buffHeight = m_buffWidth = 0.0;
	m_renderCount = 0;
	SetTraceLevel(5);
	m_traceflag = (TraceFlag)(TRACE_AMBIENT | TRACE_DIFFUSE_AND_SPEC |
		TRACE_SHADOW | TRACE_REFLECTION | TRACE_REFRACTION);
}

RayTracer::RayTracer(int Width, int Height)
{
	m_buffWidth = Width;
	m_buffHeight = Height;
	m_renderCount = 0;
	SetTraceLevel(5);
	
	m_traceflag = (TraceFlag)(TRACE_AMBIENT | TRACE_DIFFUSE_AND_SPEC |
		TRACE_SHADOW | TRACE_REFLECTION | TRACE_REFRACTION);
}

RayTracer::~RayTracer()
{

}

void RayTracer::DoRayTrace( Scene* pScene )
{
	Camera* cam = pScene->GetSceneCamera();
	
	Vector3 camRightVector = cam->GetRightVector();
	Vector3 camUpVector = cam->GetUpVector();
	Vector3 camViewVector = cam->GetViewVector();
	Vector3 centre = cam->GetViewCentre();
	Vector3 camPosition = cam->GetPosition();

	double sceneWidth = pScene->GetSceneWidth();
	double sceneHeight = pScene->GetSceneHeight();

	// If the scene is orthographic, increase the screen width and height.
	if (m_traceflag & RayTracer::TRACE_ORTHO)
	{
		sceneWidth *= 20;
		sceneHeight *= 20;
	}

	double pixelDX = sceneWidth / m_buffWidth;
	double pixelDY = sceneHeight / m_buffHeight;
	
	int total = m_buffHeight*m_buffWidth;
	int done_count = 0;
	
	Vector3 start;

	start[0] = centre[0] - ((sceneWidth * camRightVector[0])
		+ (sceneHeight * camUpVector[0])) / 2.0;
	start[1] = centre[1] - ((sceneWidth * camRightVector[1])
		+ (sceneHeight * camUpVector[1])) / 2.0;
	start[2] = centre[2] - ((sceneWidth * camRightVector[2])
		+ (sceneHeight * camUpVector[2])) / 2.0;
	
	Colour scenebg = pScene->GetBackgroundColour();

	if (m_renderCount == 0)
	{
		fprintf(stdout, "Trace start.\n");

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < m_buffHeight; i++) 
		{
			for (int j = 0; j < m_buffWidth; j++) 
			{

				//calculate the metric size of a pixel in the view plane (e.g. framebuffer)
				Vector3 pixel;

				pixel[0] = start[0] + (i + 0.5) * camUpVector[0] * pixelDY
					+ (j + 0.5) * camRightVector[0] * pixelDX;
				pixel[1] = start[1] + (i + 0.5) * camUpVector[1] * pixelDY
					+ (j + 0.5) * camRightVector[1] * pixelDX;
				pixel[2] = start[2] + (i + 0.5) * camUpVector[2] * pixelDY
					+ (j + 0.5) * camRightVector[2] * pixelDX;

				/*
				* setup view ray
				* In perspective projection, each view ray originates from the eye (camera) position 
				* and pierces through a pixel in the view plane
				*
				* TODO: For a little extra credit, set up the view rays to produce orthographic projection
				*/
				// link: http://www.cs.cornell.edu/courses/cs4620/2011fa/lectures/06raytracingWeb.pdf
				Ray viewray;	

				//Orthographic Camera
				if (m_traceflag & RayTracer::TRACE_ORTHO)
				{
					viewray.SetRay(pixel, camViewVector);
				}
				//Perspective Camera
				else				
				{
					viewray.SetRay(camPosition, (pixel - camPosition).Normalise());
				}

				//trace the scene using the view ray
				//the default colour is the background colour, unless something is hit along the way
				Colour colour = this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				/*
				* The only OpenGL code we need
				* Draw the pixel as a coloured rectangle
				*/
				glColor3f(colour.red, colour.green, colour.blue);
				glRecti(j, i, j + 1, i + 1);
			}
			glFlush();
		}
		fprintf(stdout, "Done!!!\n");
		m_renderCount++;
	}
	glFlush();
}

Colour RayTracer::TraceScene(Scene* pScene, Ray& ray, Colour incolour, int tracelevel, bool shadowray)
{
	RayHitResult result;
	Colour outcolour = incolour;
	std::vector<Light*>* light_list = pScene->GetLightList();

	if (tracelevel <= 0) // reach the MAX depth of the recursion.
	{
		return outcolour;
	}

	result = pScene->IntersectByRay(ray, shadowray);

	if (result.data) //the ray has hit something
	{

		Vector3 start = ray.GetRayStart();

		if (!shadowray)
		{
			outcolour = CalculateLighting(light_list,
				&start,
				&result);
		}
		else // If the ray is a shadow ray, stops the shadow rays from being reflected/refracted.
		{
			outcolour.red /= 10;
			outcolour.blue /= 10;
			outcolour.green /= 10;
			return outcolour;
		}

		if (m_traceflag & TRACE_REFLECTION)
		{
			//Only consider reflection for spheres and boxes
			if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere ||
				((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
			{
				//TODO: Calculate reflection ray based on the current intersection result
				//Recursively call TraceScene with the reflection ray
				//Combine the returned colour with the current surface colour 
				// https://asalga.wordpress.com/2012/09/23/understanding-vector-reflection-visually/

				Ray newRay;
				newRay.SetRay(result.point, ray.GetRay().Reflect(result.normal));
				Colour reflection = TraceScene(pScene, newRay, incolour, tracelevel - 1, shadowray);
				Colour lights = CalculateLighting(light_list, &pScene->GetSceneCamera()->GetPosition(), &result);

				outcolour.red *= reflection.red;
				outcolour.green *= reflection.green;
				outcolour.blue *= reflection.blue;
			}
		}

		if (m_traceflag & TRACE_REFRACTION)
		{
			//Only consider refraction for spheres and boxes
			if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere ||
				((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
			{
				//TODO: Calculate refraction ray based on the current intersection result
				//Recursively call TraceScene with the reflection ray
				//Combine the returned colour with the current surface colour
				//FUCKIN SNELLS LAW http://hyperphysics.phy-astr.gsu.edu/hbase/geoopt/imggo/snell2.gif

				float coeff = 1 / 1.5;
				Vector3 refractedVector = ray.GetRay().Refract(result.normal, coeff);
				Ray newRay1;
				newRay1.SetRay(result.point + (refractedVector * 0.1), refractedVector);

				Colour refraction = TraceScene(pScene, newRay1, incolour, tracelevel - 1, shadowray);
				Colour lights = CalculateLighting(light_list, &start, &result);

				outcolour.red *= refraction.red;
				outcolour.green *= refraction.green;
				outcolour.blue *= refraction.blue;
			}
		}
		
		//////Check if this is in shadow
		if ( m_traceflag & TRACE_SHADOW )
		{
			std::vector<Light*>::iterator lit_iter = light_list->begin();
			while (lit_iter != light_list->end())
			{
				//TODO: Calculate the shadow ray using the current intersection result and the light position
				//Recursively call TraceScene with the shadow ray
				
				Vector3 lightPos = (*lit_iter)->GetLightPosition();
				Vector3 shadowDir = /*result.point - lightPos*/ lightPos - result.point;
				shadowDir = shadowDir.Normalise();

				Ray newRay;
				newRay.SetRay(result.point + (shadowDir * 0.1), shadowDir);
				outcolour = TraceScene(pScene, newRay, outcolour, tracelevel - 1, true);
				lit_iter++;
			}
		}
	}		
	return outcolour;
}

Colour RayTracer::CalculateLighting(std::vector<Light*>* lights, Vector3* campos, RayHitResult* hitresult)
{
	Colour outcolour;
	std::vector<Light*>::iterator lit_iter = lights->begin();

	//Retrive the material for the intersected primitive
	Primitive* prim = (Primitive*)hitresult->data;
	Material* mat = prim->GetMaterial();

	//the default output colour is the ambient colour
	outcolour = mat->GetAmbientColour();
	
	//This is a hack to set a checker pattern on the planes
	//Do not modify it
	if (((Primitive*)hitresult->data)->m_primtype == Primitive::PRIMTYPE_Plane)
	{
		int dx = (hitresult->point[0]/2.0);
		int dy = (hitresult->point[1]/2.0);
		int dz = (hitresult->point[2]/2.0);

		if (dx % 2 || dy % 2 || dz % 2)
		{
			outcolour.red = 1.0;
			outcolour.green = 1.0;
			outcolour.blue = 1.0;

		}
		else
		{
			outcolour.red = 0.0;
			outcolour.green = 0.0;
			outcolour.blue = 0.0;
		}
	}

	//Go through all the light sources in the scene
	//and calculate the lighting at the intersection point
	if (m_traceflag & TRACE_DIFFUSE_AND_SPEC)
	{
		while (lit_iter != lights->end())
		{
			Vector3 light_pos = (*lit_iter)->GetLightPosition();  //position of the light source
			Vector3 normal = hitresult->normal; //surface normal at intersection
			Vector3 surface_point = hitresult->point; //location of the intersection on the surface
			
			//TODO: Calculate the surface colour using the illumination model from the lecture notes
			// 1. Compute the diffuse term
			// 2. Compute the specular term using either the Phong model or the Blinn-Phong model
			// 3. store the result in outcolour

			// Vectors used in light calculation
			Vector3 lightDir = light_pos.operator-(surface_point).Normalise(); /* Denotes the direction of the point light source in relation to the surface point. */

			Vector3 viewDir = campos->operator-(surface_point).Normalise(); /* Denotes the direction of the view position in relation to the surface point. */

			Vector3 halfVec = (lightDir + viewDir).Normalise(); /* Bisects the angle between the light vector and the view vector. */

			// Caculate diffuse
			// rd = kd * ld * cosTheta where kd = surface diffuse colour and ld = light diffuse colour.
			Colour matDiff = mat->GetDiffuseColour();
			Colour lightColour= (*lit_iter)->GetLightColour();
			Colour diffuse;

			double dotProdLight = lightDir.DotProduct(normal);
			dotProdLight = min(max(dotProdLight, 0.0), 1.0); /* Anchors dotProdLight to between 0 and 1. */

			diffuse.red = matDiff.red * lightColour.red * dotProdLight;
			diffuse.blue = matDiff.blue * lightColour.blue * dotProdLight;
			diffuse.green = matDiff.green * lightColour.green * dotProdLight;

			// Calculate specular (blinn-phong model)
			// rs = ks * ls * cos^nPhi where ks = surface specular colour, ls = light specular colour and phi is the angle between h and the normal.
			Colour matSpec = mat->GetSpecularColour();
			Colour specular;
			double intensity = mat->GetSpecPower();
			double dotProdHalf = halfVec.DotProduct(normal);
			dotProdHalf = min(max(dotProdHalf, 0.0), 1.0); /* Anchors dotProdHalf to between 0 and 1. */

			specular.red = matSpec.red * lightColour.red * pow(dotProdHalf, intensity);
			specular.blue = matSpec.blue * lightColour.blue * pow(dotProdHalf, intensity);
			specular.green = matSpec.green * lightColour.green * pow(dotProdHalf, intensity);

			// Add to outcolour.
			outcolour.red += (diffuse.red + specular.red);
			outcolour.blue += (diffuse.blue + specular.blue);
			outcolour.green += (diffuse.green + specular.green);

			lit_iter++;
		}
	}

	return outcolour;
}
