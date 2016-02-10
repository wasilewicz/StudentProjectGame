#pragma once 
#include "stdafx.h"
#include "MD5Model.h"
#include "vec3.h"
#include "Camera.h"
#include "Player.h"
#include "ShaderCreator.h"
#include "Helpers.h"
#include "StaticModel.h"
#include "DynamicModel.h"
#include "Skybox.h"
#include "DebugDraw.h"
#include "Ground.h"
#include "StaticElement.h"
#include "Pistol.h"

/*Deklaracje funkcji u¿ywanych jako obs³uga zdarzeñ GLUT-a*/
void OnRender();
void OnReshape(int, int);
void OnKeyPress(unsigned char, int, int);
void OnKeyDown(unsigned char, int, int);
void OnKeyUp(unsigned char, int, int);
void OnMouseMove(int, int);
void OnMouseClicked(int button, int state, int x, int y);
void OnTimer(int);
void passive(int, int);

/*Funkcja rysuj¹ca wire kolizji, œrodek œwiata itd*/
void debugDraw();
/*Funkcja rysujaca celownik*/
void drawSight();

/*Przycisk który zosta³ w danej chwili wcisniety i puszczony*/
char keyUp = 1;
char keyDown = 1;

/*Zmienne przechowuj¹ce aktualny stan kamery i gracza*/
Camera camera;
Player player(0, 2, 80, 1, 1.8f, 1);

/*Rozmiary okna gry*/
int windowWidth = 1366;
int windowHeight = 768;

/*Obiekt klasy wczytujacej shadery*/
ShaderCreator shader;
/*Zmienna globalna, ktora przechowuje identyfikator programu cieniujacego zwrocony przez OpenGL*/
GLuint shadingProgram;

/*Struktura potrzebna do funkcji myszki*/
struct Mouse
{
	int x;
	int y;
};
Mouse mouse;

/*Tablica przechowuj¹ca stan klawiszy w formie flag, indeksowana wg kodów ASCII*/
bool keystate[256];

Skybox* skybox;

/*Rzeczy do konfiguracji fizycznego œwiata Bulleta*/
/*collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration*/
btDefaultCollisionConfiguration* collisionConfiguration;
/*use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)*/
btCollisionDispatcher* dispatcher;
/*btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep*/
btBroadphaseInterface* overlappingPairCache;
/*the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)*/
btSequentialImpulseConstraintSolver* solver;
/*Instancja fizycznego œwiata*/
btDiscreteDynamicsWorld* dynamicsWorld;
/*Obiekt klasy, która rysuje rzeczy potrzebne do debugowania, np. wireframe'y kolizji*/
DebugDraw* debugDrawer;
/*Funkcja do inicjalizacji wszystkich powyzszych wskaznikow*/
void initPhysicsWorld();

bool endOfGame = false;

/*Kontener na wszystkie elementy otoczenia*/
std::vector<StaticElement*> environmentElements;;
/*Inicjalizacja elementów otoczenia*/
void environmentInitialization();
void drawEnvironmentElements();
void deleteEnvironmentElements();
/*Elementy otoczenia*/


Ground* ground;
StaticElement* frontRightWall;
StaticElement* frontLeftWall;
StaticElement* frontRightGarage;
StaticElement* frontRightMetalHouse;
StaticElement* frontRightWatchTower;
StaticElement* frontLeftWatchTower;
StaticElement* firstRightWall;
StaticElement* secondRightWall;
StaticElement* thirdRightWall;
StaticElement* firstLeftWall;
StaticElement* secondLeftWall;
StaticElement* thirdLeftWall;
StaticElement* backRightWatchTower;
StaticElement* backLeftWatchTower;
StaticElement* backRightWall;
StaticElement* backLeftWall;
StaticElement* supplies1;
StaticElement* supplies2;
StaticElement* supplies3;
StaticElement* supplies4;
StaticElement* cargoContainer1;
StaticElement* backCargoContainer;
StaticElement* backCargoContainer1;
StaticElement* backCargoContainer2;
StaticElement* middleTower;
StaticElement* fence1;
StaticElement* fence2;
StaticElement* fence3;
StaticElement* fence4;
StaticElement* fence5;
StaticElement* fence6;
StaticElement* fence7;
StaticElement* fence8;
StaticElement* fence9;
StaticElement* fence10;
StaticElement* fence11;
StaticElement* fence12;
StaticElement* fence13;
StaticElement* fence14;
StaticElement* fence15;
StaticElement* fence16;
StaticElement* fence17;
StaticElement* fence18;
StaticElement* fence19;
StaticElement* fence20;
StaticElement* fence21;
StaticElement* leftGarage;
StaticElement* leftGarage2;
StaticElement* leftGarage3;
StaticElement* leftGarage4;
StaticElement* leftHangar1;
StaticElement* leftHangar2;
StaticElement* leftHangar3;
StaticElement* leftHangar4;
StaticElement* rightHangar1;
StaticElement* frontLeftHouse;
StaticElement* frontContainers;
StaticElement* rightGarages1;
StaticElement* rightGarages2;
StaticElement* rightGarages3;
StaticElement* rightGarages4;
StaticElement* onRightGarage1;
StaticElement* onRightGarage2;
StaticElement* midContainer1;
StaticElement* midContainer2;
StaticElement* leftContainer;
StaticElement* midContainer3;
StaticElement* supplies5;
StaticElement* supplies6;
StaticElement* bigHouse;
btRigidBody* bigHouseLeftWall;
btRigidBody* bigHouseRightWall;
btRigidBody* bigHouseLeftWall1;
btRigidBody* bigHouseRightWall1;
btRigidBody* bigHouseLeftWall2;
btRigidBody* bigHouseRightWall2;
btRigidBody* bigHouseLeftWall3;
btRigidBody* bigHouseRightWall3;
btRigidBody* bigHouseBackWall;
btRigidBody* bigHouseStairs1;

vector<Enemy*> enemies;
Enemy* enemy1;
Enemy* enemy2;
Enemy* enemy3;
Enemy* enemy4;
Enemy* enemy5;
void drawEnemiesModels(float fDeltaTime);
void updateEnemies();
void drawPlayerModel(float fDeltaTime);
Pistol* pistol;









