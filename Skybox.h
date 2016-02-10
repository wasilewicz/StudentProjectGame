#pragma once
#include "stdafx.h"
#include "vec3.h"
#include "Bitmap.h"

class Skybox
{
private:
	vec3 position;
	GLuint down, top, south, north, west, east;
	GLuint LoadTexture(char * file, int magFilter, int minFilter);
public:
	Skybox(std::string down, std::string top, std::string south, std::string north, std::string west, std::string east);
	void draw(float size);
};