#pragma once
#include "stdafx.h"
#include "vec3.h"
#include "Helpers.h"
class Pistol
{
private:
	GLuint texture;
	GLuint model;
	/*Funkcja wczytuj�ca model z .obj*/
	GLuint LoadObj(char * file);
	/*Funkcja odczytuj�ca bitmap� i tworz�ca na jej podstawie tekstur� z zadanym rodzajem filtracji*/
	GLuint LoadTexture(char * file, int magFilter, int minFilter);

	struct SFace
	{
		int v[3];
		int n[3];
		int t[3];
	};
public:
	Pistol(std::string path_to_model, std::string path_to_texture);
	void draw(float x, float y, float z, float angle);
};