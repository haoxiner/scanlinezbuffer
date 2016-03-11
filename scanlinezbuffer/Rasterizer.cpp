#include "Rasterizer.h"
#include "Scene.h"
#include "Camera.h"
#include <memory>
#include <algorithm>

Rasterizer::Rasterizer(unsigned int xResolution, unsigned int yResolution) :
	m_xResolution(xResolution), m_yResolution(yResolution), m_zbuffer(new float[xResolution]),
	m_screen(new int32_t[xResolution*yResolution])
{
	Camera camera(xResolution,yResolution);
	camera.LookAt(Point(0, 0, 0), Vector(0, 0, -1), Vector(0, 1, 0));
	float ratio = static_cast<float>(xResolution) / static_cast<float>(yResolution);
	camera.Frustum(-ratio, ratio, 1, -1, 0.01f, 100.0f);
	Transform worldToNDC = camera.WorldToProjection();

	m_shapeTable.resize(yResolution);
	m_edgeTable.resize(yResolution);

	Point p0(0,0,-1), p1(150,0,-1), p2(15,50,-1);

	Triangle t0(worldToNDC(p0), worldToNDC(p1), worldToNDC(p2));
	for (size_t i = 0; i < 3; i++)
	{
		t0.p[i].x = (t0.p[i].x + 1) / 2 * xResolution;
		t0.p[i].y = (t0.p[i].y + 1) / 2 * yResolution;
		t0.p[i].z -= 1;
	}
	t0.SortVertex();
	t0.color = (255 << 16);
	m_mesh.push_back(t0);
	
	/*for (size_t j = 0; j < 10; j++)
	{
		float yTranslate = j * 70;
		for (size_t i = 0; i < 32; ++i)
		{
			float xTranslate = i * 30;
			Triangle t(
				20.0f + xTranslate, 60.f + yTranslate, -2.0f,
				40.0f + xTranslate, 80.0f + yTranslate, -2.0f,
				30.0f + xTranslate, 120.0f + yTranslate, -2.0f);
			t.SortVertex();
			t.color = (255 << 16);
			m_mesh.push_back(t);
		}
	}
	Triangle t(100.0f, 100.0f, -3.0f, 500.0f, 200.0f, -1.0f, 300.0f, 500.0f, -1.0f);
	t.SortVertex();
	t.color = 255;
	m_mesh.push_back(t);*/

}

Rasterizer::~Rasterizer()
{
	delete[] m_zbuffer;
	delete[] m_screen;

	//delete[] m_edgeTable;
	//delete[] m_shapeTable;
}

void Rasterizer::Render(const Scene &scene, const Camera &camera, int32_t *pData)
{
	for (size_t i = 0; i < m_mesh.size(); ++i)
	{
		Point &p0 = m_mesh[i].p[0];
		Point &p1 = m_mesh[i].p[1];
		Point &p2 = m_mesh[i].p[2];

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
		//color
		shapeTableItem.color = m_mesh[i].color;
		shapeTableItem.dy = static_cast<int>(std::fmaxf(p1.y - p0.y, p2.y - p0.y)) + 1;
		m_shapeTable[static_cast<int>(p0.y)].push_back(shapeTableItem);

		// edge table
		EdgeTableItem edgeTableItem;
		edgeTableItem.id = i;
		// p0p1
		if (p0.y != p1.y)
		{
			edgeTableItem.x = p0.x;
			edgeTableItem.dx = (p0.x - p1.x) / (p0.y - p1.y);
			edgeTableItem.dy = static_cast<int>(p1.y - p0.y) + 1;
			m_edgeTable[static_cast<int>(p0.y)].push_back(edgeTableItem);
		}
		// p0p2
		if (p0.y != p2.y)
		{
			edgeTableItem.x = p0.x;
			edgeTableItem.dx = (p0.x - p2.x) / (p0.y - p2.y);
			edgeTableItem.dy = static_cast<int>(p2.y - p0.y) + 1;
			m_edgeTable[static_cast<int>(p0.y)].push_back(edgeTableItem);
		}
		// p1p2
		if (p1.y != p2.y)
		{
			edgeTableItem.x = p1.x;
			edgeTableItem.dx = (p1.x - p2.x) / (p1.y - p2.y);
			edgeTableItem.dy = static_cast<int>(p2.y - p1.y) + 1;
			m_edgeTable[static_cast<int>(p1.y)].push_back(edgeTableItem);
		}
	}

	// clear frame buffer
	memset(m_screen, 0, m_xResolution*m_yResolution * sizeof(int32_t));

	for (size_t y = 0; y < m_yResolution; ++y)
	{
		// clear zbuffer
		memset(m_zbuffer, 0, m_xResolution*sizeof(float));

		if (!m_shapeTable[y].empty())
		{
			auto edgeItemIter = m_edgeTable[y].begin();
			// add new shape
			for (auto shapeItem : m_shapeTable[y])
			{
				/*ActiveShapeTableItem astItem;
				astItem.dy = shapeItem.dy;
				m_activeShapeTable.push_front(astItem);*/

				ActiveEdgeTableItem aetItem;
				aetItem.id = shapeItem.id;
				// color
				aetItem.color = shapeItem.color;

				aetItem.xl = edgeItemIter->x;
				aetItem.dxl = edgeItemIter->dx;
				aetItem.dyl = edgeItemIter->dy;

				++edgeItemIter;
				aetItem.xr = edgeItemIter->x;
				aetItem.dxr = edgeItemIter->dx;
				aetItem.dyr = edgeItemIter->dy;
				++edgeItemIter;

				if ((aetItem.xl > aetItem.xr) || (aetItem.xl == aetItem.xr && aetItem.dxl > aetItem.dxr))
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
		unsigned int baseIndex = y*m_xResolution;
		for (auto aetItemIter = m_activeEdgeTable.begin();aetItemIter != m_activeEdgeTable.end();)
		{
			float zValue = aetItemIter->zl;
			for (int x = static_cast<int>(aetItemIter->xl); x <= static_cast<int>(aetItemIter->xr) && x<m_xResolution; ++x)
			{
				// draw scanline
				
				// zValue > 0 must be cliped, zValue -> (-2,0)
				if (x > 0 && zValue < m_zbuffer[x])
				{
					m_zbuffer[x] = zValue;
					m_screen[baseIndex + x] = aetItemIter->color;
				}

				zValue += aetItemIter->dzx;
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
						aetItemIter->dyl = edgeItemIter->dy - 1;
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
						aetItemIter->dyr = edgeItemIter->dy - 1;
						break;
					}
				}
			}
			else if (aetItemIter->dyl == 0 && aetItemIter->dyr == 0)
			{
				aetItemIter = m_activeEdgeTable.erase(aetItemIter);
				continue;
			}
			aetItemIter->xl += aetItemIter->dxl;
			aetItemIter->xr += aetItemIter->dxr;
			aetItemIter->zl += (aetItemIter->dxl*aetItemIter->dzx + aetItemIter->dzy);

			++aetItemIter;
		}
	}

	memcpy(pData, m_screen, sizeof(int32_t)*m_xResolution*m_yResolution);

	m_shapeTable.clear();
	m_edgeTable.clear();

	m_shapeTable.resize(m_yResolution);
	m_edgeTable.resize(m_yResolution);
	
	//m_activeShapeTable.clear();
	m_activeEdgeTable.clear();
}