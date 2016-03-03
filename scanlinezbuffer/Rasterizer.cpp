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
	Triangle t(100.0f, 100.f, -1.0f, 800.0f, 100.0f, -1.0f, 800.0f, 500.0f, -1.0f);
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

		if (n.z == 0.0f)
		{
			continue;
		}

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
		// p0p1
		if (p0.y != p1.y)
		{
			edgeTableItem.x = p0.x;
			edgeTableItem.dx = (p0.x - p1.x) / (p0.y - p1.y);
			edgeTableItem.dy = p1.y - p0.y;
			m_edgeTable[static_cast<int>(p0.y)].push_back(edgeTableItem);
		}
		// p0p2
		if (p0.y != p2.y)
		{
			edgeTableItem.dx = (p0.x - p2.x) / (p0.y - p2.y);
			edgeTableItem.dy = p2.y - p0.y;
			m_edgeTable[static_cast<int>(p0.y)].push_back(edgeTableItem);
		}
		// p1p2
		if (p1.y != p2.y)
		{
			edgeTableItem.dx = (p0.x - p2.x) / (p0.y - p2.y);
			edgeTableItem.dy = p2.y - p0.y;
			m_edgeTable[static_cast<int>(p0.y)].push_back(edgeTableItem);
		}
	}

	// clear frame buffer
	memset(pData, 0, m_xResolution*m_yResolution * sizeof(int32_t));
	// clear zbuffer
	memset(m_zbuffer, 0, m_xResolution*sizeof(float));

	for (size_t y = 0; y < m_yResolution; ++y)
	{
		if (!m_shapeTable[y].empty())
		{
			auto edgeItemIter = m_edgeTable[y].begin();
			// add new shape
			for (auto shapeItem : m_shapeTable[y])
			{
				ActiveShapeTableItem astItem;
				astItem.dy = shapeItem.dy;
				m_activeShapeTable.push_front(astItem);

				ActiveEdgeTableItem aetItem;
				aetItem.id = shapeItem.id;

				aetItem.xl = edgeItemIter->x;
				aetItem.dxl = edgeItemIter->dx;
				aetItem.dyl = edgeItemIter->dy;

				++edgeItemIter;
				aetItem.xr = edgeItemIter->x;
				aetItem.dxr = edgeItemIter->dx;
				aetItem.dyr = edgeItemIter->dy;
				++edgeItemIter;

				if (aetItem.xl > aetItem.xr)
				{
					std::swap(aetItem.xl, aetItem.xr);
					std::swap(aetItem.dxl, aetItem.dxr);
					std::swap(aetItem.dyl, aetItem.dyr);
				}

				aetItem.dzx = -shapeItem.a / shapeItem.c;
				aetItem.dzy = -shapeItem.b / shapeItem.c;
				aetItem.zl = aetItem.dzx * aetItem.xl + y*aetItem.dzy - shapeItem.d / shapeItem.c;

				m_activeEdgeTable.push_front(aetItem);
			}
		}
		unsigned int baseIndex = y*m_yResolution;
		for (auto aetItemIter = m_activeEdgeTable.begin();aetItemIter != m_activeEdgeTable.end();++aetItemIter)
		{
			float zValue = aetItemIter->zl;
			for (int x = static_cast<int>(aetItemIter->xl); x <= static_cast<int>(aetItemIter->xr); ++x)
			{
				// draw scanline
				zValue += aetItemIter->dzx;
				// zValue > 0 must be cliped 
				if (zValue < 0 && zValue > m_zbuffer[x])
				{
					m_zbuffer[x] = zValue;
					pData[baseIndex + x] = (255 << 16);
				}
			}
			--aetItemIter->dyl;
			--aetItemIter->dyr;
			if (aetItemIter->dyl == 0 && aetItemIter->dyr != 0)
			{
				for (auto edgeItemIter = m_edgeTable[y].begin();edgeItemIter != m_edgeTable[y].end();++edgeItemIter)
				{
					if (edgeItemIter->id == aetItemIter->id)
					{
						aetItemIter->xl = edgeItemIter->x;
						aetItemIter->dxl = edgeItemIter->dx;
						aetItemIter->dyl = edgeItemIter->dy;
						break;
					}
				}
			}
			else if (aetItemIter->dyr == 0 && aetItemIter->dyl != 0)
			{
				for (auto edgeItemIter = m_edgeTable[y].begin();edgeItemIter != m_edgeTable[y].end();++edgeItemIter)
				{
					if (edgeItemIter->id == aetItemIter->id)
					{
						aetItemIter->xr = edgeItemIter->x;
						aetItemIter->dxr = edgeItemIter->dx;
						aetItemIter->dyr = edgeItemIter->dy;
						break;
					}
				}
			}
			else if (aetItemIter->dyl == 0 && aetItemIter->dyr == 0)
			{
				m_activeEdgeTable.erase(aetItemIter);
				continue;
			}
			aetItemIter->xl += aetItemIter->dxl;
			aetItemIter->xr += aetItemIter->dxr;
			aetItemIter->zl += (aetItemIter->dxl*aetItemIter->dzx + aetItemIter->dzy);
		}
	}
}