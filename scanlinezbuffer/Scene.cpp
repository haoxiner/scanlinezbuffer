#include "Scene.h"

void Scene::Load(const char * filepath)
{
	std::vector<tinyobj::material_t> mtls;
	std::string err;
	tinyobj::LoadObj(shapes, mtls, err, filepath);
	int faceCount = 0;
	for (auto shape : shapes)
	{
		for (auto idx : shape.mesh.indices)
		{
			++faceCount;
			auto v = idx * 3;
			float x = shape.mesh.positions[v];
			++v;
			float y = shape.mesh.positions[v];
			++v;
			float z = shape.mesh.positions[v];
			if (boxMax.x < x)
			{
				boxMax.x = x;
			}
			if (boxMax.y < y)
			{
				boxMax.y = y;
			}
			if (boxMax.z < z)
			{
				boxMax.z = z;
			}

			if (boxMin.x > x)
			{
				boxMin.x = x;
			}
			if (boxMin.y > y)
			{
				boxMin.y = y;
			}
			if (boxMin.z > z)
			{
				boxMin.z = z;
			}
		}
	}
	center.x = (boxMax.x + boxMin.x) / 2;
	center.y = (boxMax.y + boxMin.y) / 2;
	center.z = (boxMax.z + boxMin.z) / 2;

	colorTable.resize(faceCount / 3);
	for (auto iter = colorTable.begin();iter != colorTable.end();++iter)
	{
		int r = static_cast<int>(static_cast<float>(rand()) / RAND_MAX * 255);
		int g = static_cast<int>(static_cast<float>(rand()) / RAND_MAX * 255);
		int b = static_cast<int>(static_cast<float>(rand()) / RAND_MAX * 255);
		r = r > 10 ? r : 10;
		g = g > 10 ? g : 10;
		b = b > 10 ? b : 10;
		*iter = (r << 16 | g << 8 | b);
	}
	/*int intensity = 0;
	for (auto iter = colorTable.begin();iter != colorTable.end();++iter)
	{
		intensity = intensity % (256 - 50) + 50;
		*iter = (intensity << 16 | intensity << 8 | intensity);
		++intensity;
	}*/
}

