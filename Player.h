#pragma once
#include "stdafx.h"
#include "vec3.h"
#include "Enviroment.h"
#include <vector>
#include <iostream>
#include "DynamicModel.h"
#include "Helpers.h"
#include "Enemy.h"
#include "Pistol.h"

class Player
{
private:
	/*Predkosc biegu do przodu i w inne strony*/
	float speed = 10;
	float slower_speed = 8;
	float pointing_speed = 3;
	float sprint_speed = 20;

public:
	vec3 position;
	vec3 direction;
	vec3 length;


	/*k¹t pod jakim obrócony jest model playera*/
	float angle;

	/*K¹ty potrzebne do reagowania na ruchy myszki*/
	float vertical_angle = 0;
	float horizontal_angle = 0;

	Player(float x, float y, float z, float length_x, float length_y, float length_z);

	/*Aktualizacja skladowych Playera*/
	void update();
	void control(bool *keystate, char &keyUp, char &keyDown, bool endOfGame);//sterowanie
	void mouseControl(float windowWidth, float windowHeight, float mouseX, float mouseY, bool endOfGame);
	void mouseControlOrthographic(float windowWidth, float windowHeight, float mouseX, float mouseY);
	void draw();
	bool Player::isSeeingEnemy(btDiscreteDynamicsWorld* dynamicsWorld, vec3* enemyPosition);
	/*Tutaj bedziemy podstawiac modele z animacjami*/
	DynamicModel* model;

	/*Rozne modele z animacjami*/
	MD5Model idle;
	MD5Model walk;
	MD5Model run;
	MD5Model holdWeapon;
	MD5Model sneak;
	MD5Model death;
	MD5Model stab;

	void loadingAnimations();

	btRigidBody* rigidBody;

	bool isSneaking = false;
	bool isSprinting = false;
	bool isMoving = false;
	bool isDead = false;
	bool isStabbing = false;
	int stab_anim_frames = 35;
	int stab_anim_frames_count = 0;


	enum State{HOLD_WEAPON, IDLE};
	State state = IDLE;
	string getMovementStateString();

	int hitPoints = 3;
	int death_anim_frames = 50;
	int death_anim_frames_count = 0;

	vector<vec3*> bullets;
	

	int ammo = 1;

};
