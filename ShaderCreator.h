#pragma once
#include "stdafx.h"
class ShaderCreator
{
private:
	GLuint CreateShader(GLenum, char[]);
public:
	GLuint CreateShadingProgram(char[], char[]);
};