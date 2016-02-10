#pragma once
#include "stdafx.h"
#include "Enviroment.h"
#include "MD5Model.h"

class DynamicModel : public Environment
{
private:

	MD5Model model;
	vec3 scale;
	void loadResources(std::string path_to_model, std::string path_to_animation);
public:
	DynamicModel(float pos_x, float pos_y, float pos_z, float scale_x, float scale_y, float scale_z, std::string path_to_model, std::string path_to_first_animation);
	void draw();
	void update(float fDeltaTime);
	void updatePositon(float pos_x, float pos_y, float pos_z);
	void updateRotation(float angle);
	void setAnimation(MD5Model animation);

	float verticalAngle = 0;
};