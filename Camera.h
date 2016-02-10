#pragma once
#include "vec3.h"
// Struktura pozwalaj¹ca na przechowanie aktualnego stanu kamery.
class Camera
{
private:
	/*Mno¿nik zmian pozycji - "szybkoœæ" ruchów kamery*/
	float speed = 1;
public:
	/*Pozycja kamery*/
	vec3 position;
	/*Kierunek patrzenia (wektor jednostkowy)*/
	vec3 direction;
	
	/*K¹ty potrzebne przy reagowaniu na ruchy myszki*/
	float vertical_angle = 0;
	float horizontal_angle = 0;
	
	/*Wartosci potrzebne przy ustawianiu kamery*/
	/*Dla kamery TPS*/
	float distanceWhenHoldingWeapon = -1.5;
	float distanceWhenIdle = -4;
	float TPSdistanceXZ = distanceWhenIdle;
	float TPSdistanceY = 1;
	/*Dla kamery orthographic*/
	float OrthographicDistanceXZ = -8;
	float OrthographicDistanceY = 13;

	bool isCameraFree;
	enum NotFreeCameraState{TPS, ORTHOGRAPHIC};
	NotFreeCameraState notFreeCameraState = ORTHOGRAPHIC;

	Camera();
	void update(vec3 player_position, vec3 player_direction);
	void control(bool *keystate);
	void mouseControl(float windowWidth, float windowHeight, float mouseX, float mouseY);

	void setIsCameraFree();
	void changeNotFreeCameraType();
};