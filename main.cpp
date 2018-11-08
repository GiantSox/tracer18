#include <iostream>
#define NOMINMAX
#include <Windows.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include "window.h"
#include "texture.h"

#define M_PI 3.141592653589793
#define INFINITY 1e8
#define MAX_RAY_DEPTH 5

struct color {
	float r, g, b;
};

template<typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T xx) : x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3& normalize()
	{
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
	Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
	T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec3<float> Vec3f;

class Sphere
{
public:
	Vec3f center;                           /// position of the sphere 
	float radius, radius2;                  /// sphere radius and radius^2 
	Vec3f surfaceColor, emissionColor;      /// surface color and emission (light) 
	float transparency, reflection;         /// surface transparency and reflectivity 
	Sphere(
		const Vec3f &c,
		const float &r,
		const Vec3f &sc,
		const float &refl = 0,
		const float &transp = 0,
		const Vec3f &ec = 0) :
		center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
		transparency(transp), reflection(refl)
	{ /* empty */
	}

	bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const
	{
		Vec3f l = center - rayorig;
		float tca = l.dot(raydir);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > radius2) return false;
		float thc = sqrt(radius2 - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};

float mix(const float &a, const float &b, const float &mix) {
	return b * mix + a * (1 - mix);
}


//Traces a given ray, and finds the color for it with recursion until reaching a light or max depth
Vec3f trace(const Vec3f &rayorig, const Vec3f &raydir, const std::vector<Sphere> &spheres, const int &depth)
{
	float tnear = INFINITY;

	const Sphere* hitSphere = NULL;

	//iterate over all the spheres
	for (int i = 0; i < spheres.size(); i++) {
		float t0 = INFINITY, t1 = INFINITY;
		//check if the current sphere intersects with the given ray
		if (spheres[i].intersect(rayorig, raydir, t0, t1)) {
			if (t0 < 0)
				t0 = t1;
			if (t0 < tnear) {
				tnear = t0;
				//not sure what all this t0/t1/tnear stuff is, but if these conditions 
				//are satisfied the ray has hit the sphere
				hitSphere = &spheres[i];
			}
		}
	}
	//if we don't hit anything return black
	if (!hitSphere)
		return Vec3f(0);
	
	Vec3f surfaceColor = 0;
	Vec3f hitPoint = rayorig + raydir * tnear; //maybe tnear is distance along ray where hit occured
	Vec3f hitPointNormal = hitPoint - hitSphere->center;
	hitPointNormal.normalize();
	//If normal and input ray aren't opposite to each other we are inside the ray and should 
	//flip the normal
	float bias = 1e-4;
	bool inside = false;
	if (raydir.dot(hitPointNormal) > 0) {
		hitPointNormal = -hitPointNormal;
		inside = true;
	}

	if ((hitSphere->transparency > 0 || hitSphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
		//reflections and refractions oh boy!

		//not sure why we're dotting the point and not the normal
		float facingratio = -raydir.dot(hitPoint);
		
		//not sure what this does but changing the mix value does fun things?
		float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);

		//not sure how this calculation works
		Vec3f reflectionDirection = raydir - hitPointNormal * 2 * raydir.dot(hitPointNormal);
		reflectionDirection.normalize();	//not neccessary since everything already normalized?

		//not sure what the bias is for
		//we're starting our trace from a point slightly further away 
		//from the surface, along the normal of hitPoint?
		Vec3f reflectionColor = trace(hitPoint + hitPointNormal * bias, reflectionDirection, spheres, depth + 1);
		Vec3f refractionColor = 0;
		//compute refraction
		if (hitSphere->transparency > 0) {
			
		}
		
		//Vec3f reflection = trace()

	}
	else
	{
		//diffuse object, don't need to trace further
		//go through all the spheres, and trace path to light
		//add up the color produced by the emissions from each light to surfaceColor
		for (int i = 0; i < spheres.size(); i++) {
			if (spheres[i].emissionColor.x > 0) {
				//this is a light
				Vec3f transmission = 1;
				//get ray from light (spheres[i]) to hitPoint on current object
				Vec3f lightDirection = spheres[i].center - hitPoint;
				lightDirection.normalize();
				//check if anything occluding the hitPoint from the light (shadow ray)
					//if so, set transmission = 0;
				for (int j = 0; j < spheres.size(); j++) {
					if (i != j) {	//don't want to cast a ray from the light to the light
						float t0, t1;
						//not sure why we're using a different startpoint with bias than hitpoint
						if (spheres[j].intersect(hitPoint + hitPointNormal * bias, lightDirection, t0, t1)) {
							//light is occluded
							transmission = 0;
							break;
						}
					}
				}

				//not sure why we're multiplying hitPoint and lightDirection
				//add the emission from the current light to surfaceColor
				surfaceColor += hitSphere->surfaceColor * transmission *
					std::max(float(0), hitPoint.dot(lightDirection)) * spheres[i].emissionColor;
			}
		}

	}

	//not sure why adding emissioncolor of hitsphere to surfaceColor of hitSphere
	return surfaceColor + hitSphere->emissionColor;
}

int main() {
	int width = 1280, height = 720;
	GLWindow window;
	Texture renderTex;

	unsigned char *pixelData = new unsigned char[4 * 1280 * 720];

	///Tracing
	//for every pixel
	for (int xi = 0; xi < width; xi++) {
		for (int yi = 0; yi < height; yi++) {
			//compute primary ray direction by getting gradient of line from pixel to projection center(eye)
			//for every object
				//check if primary ray for current pixel intersects with object
					//if yes, float currentObject.distance = distance from eye to object
						//if currentObject.distance is less than ray.distance
							//ray.distance = currentObject.distance
							//ray.object = currentObject
					//else
						//ray.object = null
							///I don't see many scenarios where this will happen, because it means
							///the primary ray hit *nothing*
			//if ray.object isn't null
				//shadowray.direction = the line from primary ray hit point to light position
				//for every object
					//check if it intersects with shadowray
						//if yes, ray.objectInShadow = true, no need to keep iterating through objects
				//if ray.objectInShadow = false,
					//pixels[xy] = object.color * light.brightness
						///My first thought was "but this doesn't account for objects getting 
						///darker as they get further away from the light", but I suppose that's 
						///already accounted for because fewer rays are hitting the object?
						///but the light is a point light that doesn't project like the camera?
						//calculate reflection/refraction
							//get reflection direction from normal at intersection point and primary ray direction
							//check if reflection ray hits any objects
								//if yes, cast shadow ray to find out if hitpoint has light/color
									//if yes, reflectionColor = hitpointColor;
									//if no, reflectionColor = black/0;
								//if no, reflectionColor = black/0;
							//get transmission ray from refractive index, normal at intersection point, and primary ray direction
							//if it's going through a ball, there's another refraction at other end
							//refraction occurs when the exit and enterance have a different refractive index
						//fresnel function combines reflection and refraction
							//pass in refractive index of object, and angle between primary ray and normal
				//else (object is in shadow)
					//pixels[xy] = 0
						///but what about reflections of light bouncing off other objects?
			//if intersections > 1, choose object with lowest distance from camera
		}
	}

	std::vector<Sphere> spheres;
	// position, radius, surface color, reflectivity, transparency, emission color
	/*spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
	spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), 1, 0.5));
	spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
	spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
	spheres.push_back(Sphere(Vec3f(-5.5, 0, -15), 3, Vec3f(0.90, 0.90, 0.90), 1, 0.0));*/

	// position, radius, surface color, reflectivity, transparency, emission color
	//spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
	spheres.push_back(Sphere(Vec3f(0.0, 0, -25), 4, Vec3f(1.00, 0.32, 0.36), 0, 0));
	spheres.push_back(Sphere(Vec3f(5.0, -2, -17.5), 2, Vec3f(0.90, 0.76, 0.46), 0, 0.0));
	spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 0, 0.0));
	spheres.push_back(Sphere(Vec3f(-5.75, 0, -17), 3, Vec3f(0.90, 0.90, 0.90), 0, 0.0));
	// light
	spheres.push_back(Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0.00, 0.00, 0.00), 0, 0.0, Vec3f(3)));

	

	//render
	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 35;
	float aspectRatio = width / float(height);
	float angle = tan(M_PI * 0.5 * fov / 180);

	int i = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++, i++) {
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vec3f raydir(xx, yy, -1);
			raydir.normalize();

			//compute intersections to all of the spheres from (0,0,0) along raydir (camera ray)
			Vec3f clr = trace(Vec3f(0), raydir, spheres, 0);
			/*pixelData[4 * i + 0] = clr.x * 255;
			pixelData[4 * i + 1] = clr.y * 255;
			pixelData[4 * i + 2] = clr.z * 255;*/
			pixelData[4 * i + 0] = clr.x;
			pixelData[4 * i + 1] = clr.y;
			pixelData[4 * i + 2] = clr.z;
			pixelData[4 * i + 3] = 255;
		}
	}


	renderTex.loadRGBA(pixelData, 1280, 720);

	while (!glfwWindowShouldClose(window.window))
	{
		float startTime = glfwGetTime();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		int i = 0;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++, i++) {
				float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
				float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
				Vec3f raydir(xx, yy, -1);
				raydir.normalize();

				//compute intersections to all of the spheres from (0,0,0) along raydir (camera ray)
				Vec3f clr = trace(Vec3f(0), raydir, spheres, 0);
				/*pixelData[4 * i + 0] = clr.x * 255;
				pixelData[4 * i + 1] = clr.y * 255;
				pixelData[4 * i + 2] = clr.z * 255;*/
				pixelData[4 * i + 0] = clr.x;
				pixelData[4 * i + 1] = clr.y;
				pixelData[4 * i + 2] = clr.z;
				pixelData[4 * i + 3] = 255;
			}
		}


		renderTex.loadRGBA(pixelData, 1280, 720);

		std::cout << (glfwGetTime() - startTime) *1000 << std::endl;
		window.drawFullscreenQuad(renderTex.texture);
		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}
	return 0;
}