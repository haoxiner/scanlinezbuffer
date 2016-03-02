#include "Rasterizer.h"
#include "Scene.h"
#include "Camera.h"
#include <memory>
#include <algorithm>

Rasterizer::Rasterizer(unsigned int xResolution, unsigned int yResolution) :
	m_xResolution(xResolution), m_yResolution(yResolution), m_zbuffer(new float[xResolution]),
	m_shapeTable(new std::vector<ShapeTableItem>[yResolution]),
	m_edgeTable(new std::vector<EdgeTableItem>[yResolution])
{
	Triangle t(1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, -1.0, 0.0);
	m_mesh.push_back(t);
}

Rasterizer::~Rasterizer()
{
	delete[] m_zbuffer;
	delete[] m_edgeTable;
	delete[] m_shapeTable;
}

void Rasterizer::Render(const Scene &scene, const Camera &camera, int32_t *pData)
{
	for (size_t i = 0; i < m_mesh.size(); ++i)
	{
		Point &p0 = m_mesh[i].p0;
		Point &p1 = m_mesh[i].p1;
		Point &p2 = m_mesh[i].p2;

		Vector v1 = p1 - p0;
		Vector v2 = p2 - p0;
		Vector n = Vector::Cross(v1, v2);

		// shape table
		ShapeTableItem shapeTableItem;
		shapeTableItem.a = n.x;
		shapeTableItem.b = n.y;
		shapeTableItem.c = n.z;
		shapeTableItem.d = -p0.x * n.x - p0.y*n.y - p0.z*n.z;
		shapeTableItem.id = i;
		shapeTableItem.dy = std::fmaxf(p1.y - p0.y, p2.y - p0.y);
		m_shapeTable[static_cast<int>(p0.y)].push_back(shapeTableItem);

		// edge table
		EdgeTableItem edgeTableItem;
		edgeTableItem.id = i;

		edgeTableItem.x = p0.x;
		edgeTableItem.dx = (p0.x - p1.x) / (p0.y - p1.y);
		edgeTableItem.dy = p1.y - p0.y;
		m_edgeTable[static_cast<int>(p0.y)].push_back(edgeTableItem);
		
		edgeTableItem.x = p0.x;
		edgeTableItem.dx = (p0.x - p2.x) / (p0.y - p2.y);
		edgeTableItem.dy = p2.y - p0.y;
		m_edgeTable[static_cast<int>(p0.y)].push_back(edgeTableItem);

		edgeTableItem.x = p1.x;
		edgeTableItem.dx = (p2.x - p1.x) / (p2.y - p1.y);
		edgeTableItem.dy = p2.y - p1.y;
		m_edgeTable[static_cast<int>(p1.y)].push_back(edgeTableItem);
	}

	// clear frame buffer
	memset(pData, 0, 1024 * 768 * 4);

	for (size_t i = 0; i < m_yResolution; ++i)
	{
		if (!m_shapeTable[i].empty())
		{
			// add new shape
			for (auto shapeItem : m_shapeTable[i])
			{
				ActiveShapeTableItem astItem;
				astItem.dy = shapeItem.dy;
				m_activeShapeTable.push_back(astItem);
			}
			for (auto edgeItem : m_edgeTable[i])
			{
				ActiveEdgeTableItem aetItem;
				aetItem.xl = edgeItem.x;
			}
		}
		
	}

	
	std::sort(m_mesh.begin(), m_mesh.end());
	for (size_t i = 0; i < 1024 * 768 / 2; i++)
	{
		pData[i] = (255 << 16);
	}
}