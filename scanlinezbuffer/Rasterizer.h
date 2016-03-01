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
	Rasterizer(unsigned int xResolution, unsigned int yResolution);
	~Rasterizer();
	void Render(const Scene &scene, const Camera &camera, int32_t *pData);
private:
	std::vector<Triangle> m_mesh;
	float *m_zbuffer;
	unsigned int m_xResolution, m_yResolution;
};