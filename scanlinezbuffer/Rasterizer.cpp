#include "Rasterizer.h"
#include "Scene.h"
#include "Camera.h"
#include <memory>
#include <algorithm>

Rasterizer::Rasterizer(unsigned int xResolution, unsigned int yResolution) :
	m_xResolution(xResolution), m_yResolution(yResolution), m_zbuffer(new float[xResolution])
{
	Triangle t(1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, -1.0, 0.0);
	m_mesh.push_back(t);
}

Rasterizer::~Rasterizer()
{
	delete [] m_zbuffer;
}

void Rasterizer::Render(const Scene &scene, const Camera &camera, int32_t *pData)
{
	memset(pData, 0, 1024 * 768 * 4);
	std::sort(m_mesh.begin(), m_mesh.end());
	for (size_t i = 0; i < 1024 * 768 / 2; i++)
	{
		pData[i] = (255 << 16);
	}
}