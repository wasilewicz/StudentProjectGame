#include "stdafx.h"
#include "DynamicModel.h"

DynamicModel::DynamicModel(float pos_x, float pos_y, float pos_z, float scale_x, float scale_y, float scale_z, std::string path_to_model, std::string path_to_first_animation)
{
	position.x = pos_x;
	position.y = pos_y;
	position.z = pos_z;

	scale.x = scale_x;
	scale.y = scale_y;
	scale.z = scale_z;

	loadResources(path_to_model, path_to_first_animation);
}

void DynamicModel::loadResources(std::string path_to_model, std::string path_to_animation)
{
	model.LoadModel(path_to_model);
	model.LoadAnim(path_to_animation);
}

void DynamicModel::draw()
{
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glScalef(scale.x, scale.y, scale.z);
	glRotatef(angle, 0, 1, 0);
	glRotatef(verticalAngle, 0, 0, 1);
	glFrontFace(GL_CW);
	model.Render();
	glFrontFace(GL_CCW);
	glPopMatrix();
}

void DynamicModel::update(float fDeltaTime)
{
	model.Update(fDeltaTime);
}

void DynamicModel::updatePositon(float pos_x, float pos_y, float pos_z)
{
	position.x = pos_x;
	position.y = pos_y;
	position.z = pos_z;
}

void DynamicModel::updateRotation(float angle)
{
	this->angle = angle;
}

void DynamicModel::setAnimation(MD5Model animation)
{
	model = animation;
}
