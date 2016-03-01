#pragma once
#include <cstdint>
#include <vector>
#include "Triangle.h"

class Scene;
class Camera;
class Image;

class Rasterizer
{
public:
	Rasterizer();
	void Render(const Scene &scene, const Camera &camera, int32_t *pData);
private:
	std::vector<Triangle> m_mesh;
	float *m_zbuffer;
};