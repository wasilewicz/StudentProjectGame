#pragma once
#include "stdafx.h"
#include "Enviroment.h"
#include "vec3.h"
#include "Bitmap.h"

class StaticModel : public Environment
{
protected:
	vec3 scale;
	//GLuint model;
	GLuint texture;
	float angle;

	/*Funkcja wczytuj¹ca model z .obj*/
	GLuint LoadObj(char * file);
	/*Funkcja odczytuj¹ca bitmapê i tworz¹ca na jej podstawie teksturê z zadanym rodzajem filtracji*/
	GLuint LoadTexture(char * file, int magFilter, int minFilter);
	void loadResources(std::string path_to_model, std::string path_to_texture);
public:
	StaticModel(float pos_x, float pos_y, float pos_z, float scale_x, float scale_y, float scale_z, float angle, std::string path_to_model, std::string path_to_texture);
	void draw();
	GLuint model;
	struct SFace
	{
		int v[3];
		int n[3];
		int t[3];
	};
};

