#pragma once
#include "stdafx.h"
#include "vec3.h"
#include "Enviroment.h"
#include <vector>
#include <iostream>
#include "DynamicModel.h"
#include "Helpers.h"
#include <ctime>
class Enemy
{
private:
	vec3 length;
	int sightDistance = 30;
	/*Predkosc biegu do przodu i w inne strony*/
	float speed = 6;
	float slower_speed = 3;
	float sprint_speed = 20;
	float sightAngle = 110;
	float hearingRadius = 40;
	void movement();

public:
	vec3 position;
	vec3 direction;
	/*k¹t pod jakim obrócony jest model playera*/
	float angle;
	Enemy(float x, float y, float z, float length_x, float length_y, float length_z);
	/*Aktualizacja skladowych Playera*/
	void update(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, const string playerState, bool &isPlayerDead);
	void draw();

	/*Tutaj bedziemy podstawiac modele z animacjami*/
	DynamicModel* model;

	/*Rozne modele z animacjami*/
	MD5Model idle;
	MD5Model walk;
	MD5Model run;
	MD5Model holdWeapon;
	MD5Model death;

	void loadingAnimations();

	btRigidBody* rigidBody;

	enum State{ IDLE, WALKING, AGGRESSIVE, AWARE };
	State state = WALKING;


	/*AI*/
	/*Czynnosc wykonywana podczas state==WALKING*/
	void walking(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, const string playerState);
	/*mowi czy tuz przed nim jest sciana*/
	bool nearToCollision(btDiscreteDynamicsWorld* dynamicsWorld);
	/*zmiennna, ktora pokazuje jak dlugo idzie w jednym kierunku*/
	int walking_count = 0;
	/*zmienna ktora pokazuje jak duzo klatek minelo od zgubienia gracza*/
	int seeing_count = 0;
	bool seesPlayer(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition);
	bool hearsPlayer(vec3* playerPosition, const string playerState);
	vec3 lastSeenPlayerPosition;
	/*Czynnosc wykonywana podczas state==AGGRESSIVE*/
	void aggressive(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, bool &isPlayerDead);
	/*Czynnosc wykonywana podczas state==AWARE*/
	void aware(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, const string playerState);
	void goToPlace(vec3* position);
	void shot(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, bool &isPlayerDead);
	/*ilosc klatek ktore musza minac zeby moc strzelic kolejny raz*/
	int breakBetweenShots = 40;
	/*zmienna pomocnicza do liczenia tej ilosci klatek*/
	int shotCount = 0;
	/*ilosc klatek animacji strzalu*/
	int shot_anim_length = 20;
	/*szansza na trafienie  = 1/hit_chance*/
	int hit_chance = 3;
	/*zasieg strzelania*/
	int shot_distance = 20;
	/*czy celuje*/
	bool isPointing = false;
	bool isDead = false;
	int death_anim_frames = 45;
	int death_anim_frames_count = 0;
	
	vector<vec3*> bullets;
};

