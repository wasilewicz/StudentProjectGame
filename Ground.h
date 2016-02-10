#include "stdafx.h"
#include "StaticModel.h"


class Ground
{
private:
	StaticModel* model;
public:
	btRigidBody* rigidBody;
	Ground(std::string path_to_model, std::string path_to_texture, float width, float height, float depth);
	void draw();
};