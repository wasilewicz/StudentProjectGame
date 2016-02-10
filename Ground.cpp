#include "stdafx.h"
#include "Ground.h"

Ground::Ground(std::string path_to_model, std::string path_to_texture, float width, float height, float depth)
{
	model = new StaticModel(0, 0, 0, 1, 1, 1, 0, path_to_model, path_to_texture);

	btCollisionShape* shape = new btBoxShape(btVector3(width / 2, height / 2, depth / 2));
	rigidBody = new btRigidBody(0, nullptr, shape);
	rigidBody->translate(btVector3(0, 0, 0));
}

void Ground::draw()
{
	glPushMatrix();
	glTranslatef(0, -0.5f, 0);
	glScalef(3, 1, 3);
	model->draw();
	glPopMatrix();
}

