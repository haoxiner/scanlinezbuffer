#pragma once
#include "tinyobjloader.h"
#include "Point.h"
class Scene
{
public:
	Scene();
	void LoadConfig(const char* filepath);
	void Load(const char* filepath);
	void switchNext();
public:
	std::vector<tinyobj::shape_t> shapes;
	std::vector<float> colorTable;
	Point center;
	Point boxMax;
	Point boxMin;
private:
	std::vector<std::string> filepathList;
	int currentModelIndex;
};