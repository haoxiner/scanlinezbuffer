#pragma once
#include "tinyobjloader.h"
#include "Point.h"
class Scene
{
public:
	void Load(const char* filepath);
public:
	std::vector<tinyobj::shape_t> shapes;
	std::vector<float> colorTable;
	Point center;
	Point boxMax;
	Point boxMin;
};