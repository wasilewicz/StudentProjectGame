#include "stdafx.h"
#include "Skybox.h"

Skybox::Skybox(std::string down, std::string top, std::string south, std::string north, std::string west, std::string east)
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	down = "Skybox\\" + down;
	top = "Skybox\\" + top;
	south = "Skybox\\" + south;
	north = "Skybox\\" + north;
	west = "Skybox\\" + west;
	east = "Skybox\\" + east;
	this->down = LoadTexture((char*)down.c_str(), GL_LINEAR, GL_LINEAR);
	this->top = LoadTexture((char*)top.c_str(), GL_LINEAR, GL_LINEAR);
	this->south = LoadTexture((char*)south.c_str(), GL_LINEAR, GL_LINEAR);
	this->north = LoadTexture((char*)north.c_str(), GL_LINEAR, GL_LINEAR);
	this->west = LoadTexture((char*)west.c_str(), GL_LINEAR, GL_LINEAR);
	this->east = LoadTexture((char*)east.c_str(), GL_LINEAR, GL_LINEAR);
}

GLuint Skybox::LoadTexture(char * file, int magFilter, int minFilter) {

	// Odczytanie bitmapy
	Bitmap *tex = new Bitmap();
	if (!tex->loadBMP(file)) {
		printf("ERROR: Cannot read texture file \"%s\".\n", file);
		return -1;
	}

	// Utworzenie nowego id wolnej tekstury
	GLuint texId;
	glGenTextures(1, &texId);

	// "Bindowanie" tekstury o nowoutworzonym id
	glBindTexture(GL_TEXTURE_2D, texId);

	// Okreœlenie parametrów filtracji dla tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter); // Filtracja, gdy tekstura jest powiêkszana
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter); // Filtracja, gdy tekstura jest pomniejszana

	// Wys³anie tekstury do pamiêci karty graficznej zale¿nie od tego, czy chcemy korzystaæ z mipmap czy nie
	if (minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST) {
		// Automatyczne zbudowanie mipmap i wys³anie tekstury do pamiêci karty graficznej
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex->width, tex->height, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
	}
	else {
		// Wys³anie tekstury do pamiêci karty graficznej 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
	}

	// Zwolnienie pamiêci, usuniêcie bitmapy z pamiêci - bitmapa jest ju¿ w pamiêci karty graficznej
	delete tex;

	// Zwrócenie id tekstury
	return texId;
}

void Skybox::draw(float size)
{
	//glDisable(GL_DEPTH_TEST);
	/* Sides */
	float D = size / 2;
	glBindTexture(GL_TEXTURE_2D, east);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-D, -D, -D);
	glTexCoord2f(1, 0); glVertex3f(+D, -D, -D);
	glTexCoord2f(1, 1); glVertex3f(+D, +D, -D);
	glTexCoord2f(0, 1); glVertex3f(-D, +D, -D);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, north);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(+D, -D, -D);
	glTexCoord2f(1, 0); glVertex3f(+D, -D, +D);
	glTexCoord2f(1, 1); glVertex3f(+D, +D, +D);
	glTexCoord2f(0, 1); glVertex3f(+D, +D, -D);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, west);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(+D, -D, +D);
	glTexCoord2f(1, 0); glVertex3f(-D, -D, +D);
	glTexCoord2f(1, 1); glVertex3f(-D, +D, +D);
	glTexCoord2f(0, 1); glVertex3f(+D, +D, +D);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, south);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-D, -D, +D);
	glTexCoord2f(1, 0); glVertex3f(-D, -D, -D);
	glTexCoord2f(1, 1); glVertex3f(-D, +D, -D);
	glTexCoord2f(0, 1); glVertex3f(-D, +D, +D);
	glEnd();

	/* Top and Bottom */
	glBindTexture(GL_TEXTURE_2D, top);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-D, +D, -D);
	glTexCoord2f(1, 0); glVertex3f(+D, +D, -D);
	glTexCoord2f(1, 1); glVertex3f(+D, +D, +D);
	glTexCoord2f(0, 1); glVertex3f(-D, +D, +D);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, down);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(+D, -D, -D);
	glTexCoord2f(1, 0); glVertex3f(-D, -D, -D);
	glTexCoord2f(1, 1); glVertex3f(-D, -D, +D);
	glTexCoord2f(0, 1); glVertex3f(+D, -D, +D);
	glEnd();
	//glEnable(GL_DEPTH_TEST);

}