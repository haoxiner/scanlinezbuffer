#pragma once
#include "tinyobjloader.h"

class Scene
{
public:
	void Load(const char* filepath);
public:
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> mtls;
};