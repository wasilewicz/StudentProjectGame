#pragma once
#include "vec3.h"

class Environment
{
public:
	vec3 position;
	vec3 length;
	float angle;

	virtual void draw() = 0;
	virtual void loadResources(std::string path_to_model, std::string path_to_texture) = 0;
};