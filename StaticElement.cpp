#include "stdafx.h"
#include "StaticElement.h"

/*Najpierw dopasowac pozycje i d³ugoœæ bounding boxa a pozniej dopiero obracac*/
StaticElement::StaticElement(float x, float y, float z, float length_x, float length_y, float length_z, float scale_x, float scale_y, float scale_z, float angle, std::string path_to_model, std::string path_to_texture)
{
	position.x = x;
	position.y = y;
	position.z = z;

	length.x = length_x;
	length.y = length_y;
	length.z = length_z;

	this->angle = angle;

	model = new StaticModel(x, y, z, scale_x, scale_y, scale_z, angle, path_to_model, path_to_texture);

	btCollisionShape* shape = new btBoxShape(btVector3(length.x / 2, length.y / 2, length.z / 2));
	rigidBody = new btRigidBody(0, nullptr, shape);
	rigidBody->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(angle)), btScalar(0), btScalar(0)), btVector3(x, y, z)));
}

void StaticElement::draw()
{
	model->draw();
}