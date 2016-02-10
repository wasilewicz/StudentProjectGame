#pragma once
#include "stdafx.h"
#include "StaticModel.h"
#include "Helpers.h"

class StaticElement
{
private:
	vec3 position;
	vec3 direction;
	vec3 length;

	/*k¹t pod jakim obrócony jest model*/
	float angle;

	StaticModel* model;
	

public:
	btRigidBody* rigidBody;
	StaticElement(float x, float y, float z, float length_x, float length_y, float length_z, float scale_x, float scale_y, float scale_z, float angle, std::string path_to_model, std::string path_to_texture);
	void draw();
};