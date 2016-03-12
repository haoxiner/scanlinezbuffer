#pragma once
#include <cstdint>
#include <vector>
#include <list>

#include "Triangle.h"

class Scene;
class Camera;
class Image;

struct ShapeTableItem
{
	float a, b, c, d;
	unsigned int id;
	unsigned int dy;
	float color;
};

struct EdgeTableItem
{
	float x;
	float dx;
	unsigned int dy;
	unsigned int id;
};

struct ActiveEdgeTableItem
{
	float xl;
	float dxl;
	unsigned int dyl;

	float xr;
	float dxr;
	unsigned int dyr;

	float zl;
	float dzx;
	float dzy;

	unsigned int id;
	float color;
};

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
	int32_t *m_screen;

	std::vector<std::vector<ShapeTableItem>> m_shapeTable;
	std::vector<std::list<EdgeTableItem>> m_edgeTable;
	std::list<ActiveEdgeTableItem> m_activeEdgeTable;

};