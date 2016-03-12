#include "Rasterizer.h"
#include "Scene.h"
#include "Camera.h"
#include "tinyobjloader.h"
#include <random>
#include <memory>
#include <algorithm>

Rasterizer::Rasterizer(unsigned int xResolution, unsigned int yResolution) :
	m_xResolution(xResolution), m_yResolution(yResolution), m_zbuffer(new float[xResolution]),
	m_screen(new int32_t[xResolution*yResolution])
{
	m_shapeTable.resize(yResolution);
	m_edgeTable.resize(yResolution);
}

Rasterizer::~Rasterizer()
{
	delete[] m_zbuffer;
	delete[] m_screen;
}

void Rasterizer::Render(const Scene &scene, const Camera &camera, int32_t *pData)
{
	Transform worldToNDC = camera.WorldToProjection();
	m_mesh.clear();
	for (auto shape : scene.shapes)
	{
		auto mesh = shape.mesh;
		for (size_t i = 0; i < mesh.indices.size();)
		{
			auto v = 3*mesh.indices[i];
			++i;
			Point p0(mesh.positions[v], mesh.positions[v + 1], mesh.positions[v + 2]);
			v = 3*mesh.indices[i];
			++i;
			Point p1(mesh.positions[v], mesh.positions[v + 1], mesh.positions[v + 2]);
			v = 3*mesh.indices[i];
			++i;
			Point p2(mesh.positions[v], mesh.positions[v + 1], mesh.positions[v + 2]);
			Triangle t0(worldToNDC(p0), worldToNDC(p1), worldToNDC(p2));
			bool accepted = true;
			for (size_t j = 0; j < 3; j++)
			{
				t0.p[j].x = static_cast<int>((t0.p[j].x + 1) / 2 * m_xResolution);
				if (t0.p[j].x < 0 || t0.p[j].x >= m_xResolution)
				{
					accepted = false;
					break;
				}
				t0.p[j].y = static_cast<int>((t0.p[j].y + 1) / 2 * m_yResolution);
				if (t0.p[j].y < 0 || t0.p[j].y >= m_yResolution)
				{
					accepted = false;
					break;
				}
				t0.p[j].z -= 1;
				if (t0.p[j].z < -2 || t0.p[j].z > 0)
				{
					accepted = false;
					break;
				}
			}
			if (accepted && t0.Check())
			{
				t0.SortVertex();
				int intensity = static_cast<int>(static_cast<float>(rand()) / RAND_MAX * 255);
				intensity = intensity > 10 ? intensity : 10;
				t0.color = (intensity << 16 | intensity << 8 | intensity);
				m_mesh.push_back(t0);
			}
		}
	}
	std::sort(m_mesh.begin(), m_mesh.end());
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
			// add new shape
			for (auto shapeItem : m_shapeTable[y])
			{
				ActiveEdgeTableItem aetItem;
				aetItem.id = shapeItem.id;
				// color
				aetItem.color = shapeItem.color;
				auto edgeItemIter = m_edgeTable[y].begin();
				while (edgeItemIter->id != shapeItem.id)
				{
					++edgeItemIter;
				}
				aetItem.xl = edgeItemIter->x;
				aetItem.dxl = edgeItemIter->dx;
				aetItem.dyl = edgeItemIter->dy;
				edgeItemIter = m_edgeTable[y].erase(edgeItemIter);
				while (edgeItemIter->id != shapeItem.id)
				{
					++edgeItemIter;
				}
				aetItem.xr = edgeItemIter->x;
				aetItem.dxr = edgeItemIter->dx;
				aetItem.dyr = edgeItemIter->dy;
				m_edgeTable[y].erase(edgeItemIter);
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
		unsigned int lineIndex = y*m_xResolution;
		for (auto aetItemIter = m_activeEdgeTable.begin();aetItemIter != m_activeEdgeTable.end();)
		{
			float zValue = aetItemIter->zl;
			for (int x = static_cast<int>(aetItemIter->xl); x <= static_cast<int>(aetItemIter->xr)/* && x < m_xResolution*/; ++x)
			{
				// draw scanline
				// zValue -> (-2,0)
				if (/*x > 0 && */zValue < m_zbuffer[x])
				{
					m_zbuffer[x] = zValue;
					m_screen[lineIndex + x] = aetItemIter->color;
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
	m_activeEdgeTable.clear();
}