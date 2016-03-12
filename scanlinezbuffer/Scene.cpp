#include "Scene.h"

void Scene::Load(const char * filepath)
{
	std::string err;
	tinyobj::LoadObj(shapes, mtls, err, filepath);
}

