#include "stdafx.h"
#include "theGame.h"

using namespace std;

int main(int argc, char* argv[])
{
	srand(time(NULL));
	initPhysicsWorld();
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Infiltration");
	glutFullScreenToggle();
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnReshape);
	glutKeyboardFunc(OnKeyPress);
	glutKeyboardUpFunc(OnKeyUp);
	glutTimerFunc(17, OnTimer, 0);
	glutPassiveMotionFunc(passive);
	glutMotionFunc(passive);
	glutMouseFunc(OnMouseClicked);
	glewInit();
	shadingProgram = shader.CreateShadingProgram("Shaders//PHONG.VERT.SHADER.txt", "Shaders//PHONG.FRAG.SHADER.txt");
	glUseProgram(shadingProgram);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	/*Lightning*/
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	/*ustawienie myszki na srodek okna, wyłączenie kursora i podłączenie funkcji passive*/
	glutWarpPointer(windowWidth / 2, windowHeight / 2);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutPassiveMotionFunc(passive);

	/*TWORZENIE OBIEKTÓW*/
	ground = new Ground("Models//Ground_Dirt.obj", "Textures//concrete.bmp", 225, 1, 225);
	dynamicsWorld->addRigidBody(ground->rigidBody);
	glPushMatrix();
	glTranslatef(0, -25, 0);
	skybox = new Skybox("grimmnight_dn.bmp", "grimmnight_up.bmp", "grimmnight_lf.bmp", "grimmnight_ft.bmp", "grimmnight_bk.bmp", "grimmnight_rt.bmp");
	glPopMatrix();

	/*Inicjalizacja Playera:*/
	player.position = vec3(0.0f, 1.75f, 0.0f);
	player.direction = vec3(0.0f, 0.0f, -1.0f);
	dynamicsWorld->addRigidBody(player.rigidBody);
	/*Inicjalizacja kamery*/
	camera.update(player.position, player.direction);
	/*Inicjalizacja elementów otoczenia*/
	environmentInitialization();

	glutMainLoop();

	deleteEnvironmentElements();
	return 0;
}

// Obsługa zdarzenia, gdy zostanie wciśnięty klawisz - zdarzenie nieoodporne na repetycję klawiszy.
void OnKeyPress(unsigned char key, int x, int y)
{
	//printf("KeyPress: %c\n", key);
	if (!keystate[key]) {
		OnKeyDown(key, x, y); // Emulacja zdarzenia związanego z pojedynczym wciśnięciem klawisza
	}
	keystate[key] = true;
}

// Obsługa naszego własnego zdarzenia, gdy zostanie po raz pierwszy wciśnięty klawisz - zdarzenie odporne na repetycję.
void OnKeyDown(unsigned char key, int x, int y)
{
	keyDown = key;
	//printf("KeyDown: %c\n", key);
	if (key == 27) { // ESC - wyjście
		glutLeaveMainLoop();
	}
	/*p - zmiana kamery z podążającej za playerem na wolną i odwrotnie*/
	if (key == 'p')
	{
		camera.setIsCameraFree();
	}
	if (key == 'o')
	{
		//camera.changeNotFreeCameraType();
	}
}

// Obsługa zdarzenia puszczenia klawisza.
void OnKeyUp(unsigned char key, int x, int y)
{
	keyUp = key;
	keystate[key] = false;
}

void passive(int x1, int y1)
{
	mouse.x = x1;
	mouse.y = y1;
}

void OnMouseClicked(int button, int state, int x, int y)
{
	/*Przejscie i wyjscie z trybu celowania*/
	if (button == GLUT_RIGHT_BUTTON && player.isDead == false)
	{
		camera.vertical_angle = 0;
		if (state == GLUT_DOWN )
		{
				player.state = player.HOLD_WEAPON;
				player.model->setAnimation(player.holdWeapon);
				camera.TPSdistanceXZ = camera.distanceWhenHoldingWeapon;
				camera.notFreeCameraState = camera.TPS;
		}
		else if (state == GLUT_UP)
		{
			if (camera.notFreeCameraState == camera.TPS)
			{
				player.model->verticalAngle = 0;
				player.state = player.IDLE;
				if (keystate['w'])
				{
					if (player.isSneaking)
						player.model->setAnimation(player.sneak);
					else if (player.isSprinting)
						player.model->setAnimation(player.run);
					else
						player.model->setAnimation(player.walk);
				}
				else if (keystate['a'] || keystate['d'])
				{
					player.model->setAnimation(player.sneak);
				}
				else if (keystate['s'])
				{
					if (player.isSneaking)
						player.model->setAnimation(player.sneak);
					else
						player.model->setAnimation(player.walk);
				}
				else
					player.model->setAnimation(player.idle);

				
				camera.TPSdistanceXZ = camera.distanceWhenIdle;

				camera.notFreeCameraState = camera.ORTHOGRAPHIC;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			/*strzelanie*/
			if (camera.notFreeCameraState == camera.TPS && player.ammo > 0)
			{
				vec3* vec = new vec3(player.position.x, player.position.y, player.position.z);
				player.bullets.push_back(vec);
				player.ammo--;
				btVector3 start;
				start.setX(camera.position.x);
				start.setZ(camera.position.z);
				start.setY(camera.position.y);
				btVector3 end;
				end.setX(camera.position.x + camera.direction.x * 40);
				end.setZ(camera.position.z + camera.direction.z * 40);
				end.setY(camera.position.y + camera.direction.y * 40);
				
				btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
				dynamicsWorld->rayTest(start, end, RayCallback);
				if (RayCallback.hasHit())
				{
					const btCollisionObject * body = RayCallback.m_collisionObject;
					for each (Enemy* enemy in enemies)
					{
						if (body->getWorldTransform().getOrigin() == enemy->rigidBody->getCenterOfMassTransform().getOrigin() && player.isSeeingEnemy(dynamicsWorld, &enemy->position))
						{
							dynamicsWorld->removeRigidBody(enemy->rigidBody);
							enemy->isDead = true;
						}
					}
				}
			}
			/*uzycie noza*/
			else if (camera.notFreeCameraState == camera.ORTHOGRAPHIC)
			{
				for each (Enemy* enemy in enemies)
				{
					if (abs(player.position.x - enemy->position.x) <= 1.5f && abs(player.position.z - enemy->position.z) <= 1.5f && player.isStabbing == false)
					{
						player.ammo += 2;
						player.rigidBody->activate(false);
						dynamicsWorld->removeRigidBody(enemy->rigidBody);
						player.isStabbing = true;
						enemy->isDead = true;
					}
				}
			}
		}
	}
}

void OnTimer(int id)
{
	if (enemy1->isDead == true && enemy2->isDead == true && enemy3->isDead == true && enemy4->isDead == true && enemy5->isDead == true)
		endOfGame = true;
	dynamicsWorld->stepSimulation(1.f / 60.f, 10);

	if (camera.isCameraFree == false)
	{
		if (player.isStabbing == false)
		{
			if (camera.notFreeCameraState == camera.NotFreeCameraState::ORTHOGRAPHIC)
			{
				player.control(keystate, keyUp, keyDown, endOfGame);
				player.mouseControlOrthographic(windowWidth, windowHeight, mouse.x, mouse.y);
				camera.horizontal_angle += 0.01f * (windowWidth / 2 - mouse.x);
				camera.direction.x = cos(camera.vertical_angle) * sin(camera.horizontal_angle);
				camera.direction.y = sin(camera.vertical_angle);
				camera.direction.z = cos(camera.vertical_angle) * cos(camera.horizontal_angle);
				glutWarpPointer(windowWidth / 2, windowHeight / 2);
			}
			else if (camera.notFreeCameraState == camera.NotFreeCameraState::TPS)
			{
				player.control(keystate, keyUp, keyDown, endOfGame);
				player.mouseControl(windowWidth, windowHeight, mouse.x, mouse.y, endOfGame);
				camera.horizontal_angle += 0.01f * (windowWidth / 2 - mouse.x);
				camera.direction.x = cos(camera.vertical_angle) * sin(camera.horizontal_angle);
				camera.direction.y = sin(camera.vertical_angle);
				camera.direction.z = cos(camera.vertical_angle) * cos(camera.horizontal_angle);
				glutWarpPointer(windowWidth / 2, windowHeight / 2);
			}
		}
		camera.update(player.position, player.direction);
	}
	else
	{
		camera.control(keystate);
		camera.mouseControl(windowWidth, windowHeight, mouse.x, mouse.y);
		glutWarpPointer(windowWidth / 2, windowHeight / 2);
	}
	player.update();

	updateEnemies();
	glutTimerFunc(17, OnTimer, 0);
}

void OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (player.isDead == true && player.death_anim_frames_count == 0)
		camera.notFreeCameraState = camera.ORTHOGRAPHIC;

	/*Ustawienie kamery na podstawie jej typu*/
	if (camera.isCameraFree == false)
	{
		if (camera.notFreeCameraState == camera.NotFreeCameraState::ORTHOGRAPHIC)
		{
			gluLookAt(
				camera.position.x, camera.position.y, camera.position.z, 
				player.position.x + player.direction.x * 3, player.position.y, player.position.z + player.direction.z * 3,
				0.0f, 1.0f, 0.0f 
				);
		}
		else if (camera.notFreeCameraState == camera.NotFreeCameraState::TPS)
		{
			gluLookAt(
				camera.position.x, camera.position.y, camera.position.z, 
				camera.position.x + player.direction.x, camera.position.y + player.direction.y, camera.position.z + player.direction.z,
				0.0f, 1.0f, 0.0f 
				);
		}
	}
	else
	{
		gluLookAt(
			camera.position.x, camera.position.y, camera.position.z,
			camera.position.x + camera.direction.x, camera.position.y + camera.direction.y, camera.position.z + camera.direction.z,
			0.0f, 1.0f, 0.0f
			);
	}

	/*RYSOWANIE POCISKOW*/
	for each (Enemy* enemy in enemies)
	{
		for (int i = 0; i < enemy->bullets.size(); i++)
		{
			glPushMatrix();
			glTranslatef(enemy->bullets[i]->x, enemy->bullets[i]->y*1.5f, enemy->bullets[i]->z);
			glutSolidSphere(0.1f, 8, 8);
			glPopMatrix();
			enemy->bullets[i]->x = enemy->bullets[i]->x + enemy->direction.x * 4;
			enemy->bullets[i]->z = enemy->bullets[i]->z + enemy->direction.z * 4;
			if (enemy->bullets[i]->x > 200 || enemy->bullets[i]->x < -200 || enemy->bullets[i]->z > 200 || enemy->bullets[i]->z < -200)
				enemy->bullets.erase(enemy->bullets.begin() + i);
		}
	}
	for (int i = 0; i < player.bullets.size(); i++)
	{
		glPushMatrix();
		glTranslatef(player.bullets[i]->x, player.bullets[i]->y*1.5f, player.bullets[i]->z);
		glutSolidSphere(0.1f, 8, 8);
		glPopMatrix();
		player.bullets[i]->x = player.bullets[i]->x + player.direction.x * 4;
		player.bullets[i]->z = player.bullets[i]->z + player.direction.z * 4;
		if (player.bullets[i]->x > 200 || player.bullets[i]->x < -200 || player.bullets[i]->z > 200 || player.bullets[i]->z < -200)
			player.bullets.erase(player.bullets.begin() + i);
	}

	skybox->draw(225);

	GLfloat l0_pos[] = { 0.0f, -1.0f, 0.0f, 0.0f };
	GLfloat l0_amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l0_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l0_spe[] = { 0.3f, 0.3f, 0.3f, 0.3f };

	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00001f);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spe);
	glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);


	/*Rysowanie statycznych modeli*/
	ground->draw();
	drawEnvironmentElements();
	
	/*Rysowanie modeli z animacjami*/
	static ElapsedTime elapsedTime;
	float fDeltaTime = elapsedTime.GetElapsedTime();

	drawPlayerModel(fDeltaTime);
	drawEnemiesModels(fDeltaTime);

	//debugDraw();
	if (camera.notFreeCameraState == camera.TPS)
	{
		glPushMatrix();
		pistol->draw(player.position.x + player.direction.x * 1, player.position.y*1.5f + player.direction.y * 1, player.position.z + player.direction.z * 1, player.angle);
		glPopMatrix();
	}

	drawSight();

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void OnReshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(60.0f, (float)width / height, .01f, 250.0f);
}

void initPhysicsWorld()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -50, 0));
	debugDrawer = new DebugDraw;
	dynamicsWorld->setDebugDrawer(debugDrawer);
}

void debugDraw()
{
	/*Rysowanie środka świata i funkcji Debug(shape'y kolizji itd*/
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glutSolidCube(0.5f);
	glPopMatrix();
	dynamicsWorld->debugDrawWorld();
	glEnable(GL_LIGHTING);
}

void drawSight()
{
	/*Rysowanie celownika*/
	glPushMatrix();
	glTranslatef(camera.position.x + player.direction.x, camera.position.y + player.direction.y, camera.position.z + player.direction.z);
	glutSolidSphere(0.003f, 16, 16);
	glPopMatrix();
}




void drawEnvironmentElements()
{
	for (int i = 0; i < environmentElements.size(); i++)
	{
		environmentElements[i]->draw();
	}
}

void drawEnemiesModels(float fDeltaTime)
{
	for each (Enemy* enemy in enemies)
	{
		if (enemy->death_anim_frames_count <= enemy->death_anim_frames + 10 && (player.isSeeingEnemy(dynamicsWorld, &enemy->position) == true || distanceBetween2Points(player.position.x, player.position.z, enemy->position.x, enemy->position.z) < 50))
			enemy->model->update(fDeltaTime);
		enemy->draw();

		if (enemy->isPointing && enemy->isDead == false)
			pistol->draw(enemy->position.x + enemy->direction.x * 1, enemy->position.y*1.5f + enemy->direction.y * 1, enemy->position.z + enemy->direction.z * 1, enemy->angle);

	}
}

void updateEnemies()
{
	for each (Enemy* enemy in enemies)
	{
		enemy->update(dynamicsWorld, &player.position, player.getMovementStateString(), player.isDead);
	}
}

void drawPlayerModel(float fDeltaTime)
{
	if (player.death_anim_frames_count <= player.death_anim_frames + 10)
		player.model->update(fDeltaTime);
	player.draw();
}


void environmentInitialization()
{
	frontRightWall = new StaticElement(28, 2, 73, 41, 9, 5, 1.4f, 1.4f, 1.4f, 0, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(frontRightWall->rigidBody);
	environmentElements.push_back(frontRightWall);

	frontLeftWall = new StaticElement(-28, 2, 73, 41, 9, 5, 1.4f, 1.4f, 1.4f, 0, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(frontLeftWall->rigidBody);
	environmentElements.push_back(frontLeftWall);

	frontRightGarage = new StaticElement(15, 2, 67, 14, 6, 13, 1.4f, 1.4f, 1.4f, 0, "Models//Garage.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(frontRightGarage->rigidBody);
	environmentElements.push_back(frontRightGarage);

	frontRightMetalHouse = new StaticElement(15, 9, 65, 7, 6, 5, 1.4f, 1.4f, 1.4f, 0, "Models//MetalHouse.obj", "Textures//rusty_metal_texture.bmp");
	dynamicsWorld->addRigidBody(frontRightMetalHouse->rigidBody);
	environmentElements.push_back(frontRightMetalHouse);

	frontRightWatchTower = new StaticElement(49, 3, 73, 5.5f, 20, 5.5f, 2, 2, 2, 0, "Models//WatchTower//WatchTower.obj", "Textures//rusty_aircraft_metal_texture.bmp");
	dynamicsWorld->addRigidBody(frontRightWatchTower->rigidBody);
	environmentElements.push_back(frontRightWatchTower);

	frontLeftWatchTower = new StaticElement(-49, 3, 73, 5.5f, 20, 5.5f, 2, 2, 2, 0, "Models//WatchTower//WatchTower.obj", "Textures//rusty_aircraft_metal_texture.bmp");
	dynamicsWorld->addRigidBody(frontLeftWatchTower->rigidBody);
	environmentElements.push_back(frontLeftWatchTower);

	firstRightWall = new StaticElement(49, 2, 50, 41, 9, 5, 1.4f, 1.4f, 1.4f, 90, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(firstRightWall->rigidBody);
	environmentElements.push_back(firstRightWall);

	secondRightWall = new StaticElement(49, 2, 9, 41, 9, 5, 1.4f, 1.4f, 1.4f, 90, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(secondRightWall->rigidBody);
	environmentElements.push_back(secondRightWall);

	thirdRightWall = new StaticElement(49, 2, -32, 41, 9, 5, 1.4f, 1.4f, 1.4f, 90, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(thirdRightWall->rigidBody);
	environmentElements.push_back(thirdRightWall);

	firstLeftWall = new StaticElement(-49, 2, 50, 41, 9, 5, 1.4f, 1.4f, 1.4f, 270, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(firstLeftWall->rigidBody);
	environmentElements.push_back(firstLeftWall);

	secondLeftWall = new StaticElement(-48.5f, 2, 9, 41, 9, 5, 1.4f, 1.4f, 1.4f, 270, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(secondLeftWall->rigidBody);
	environmentElements.push_back(secondLeftWall);

	thirdLeftWall = new StaticElement(-48, 2, -32, 41, 9, 5, 1.4f, 1.4f, 1.4f, 270, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(thirdLeftWall->rigidBody);
	environmentElements.push_back(thirdLeftWall);

	backRightWatchTower = new StaticElement(49, 3, -51, 5.5f, 20, 5.5f, 2, 2, 2, 0, "Models//WatchTower//WatchTower.obj", "Textures//rusty_aircraft_metal_texture.bmp");
	dynamicsWorld->addRigidBody(backRightWatchTower->rigidBody);
	environmentElements.push_back(backRightWatchTower);

	backLeftWatchTower = new StaticElement(-48, 3, -51, 5.5f, 20, 5.5f, 2, 2, 2, 0, "Models//WatchTower//WatchTower.obj", "Textures//rusty_aircraft_metal_texture.bmp");
	dynamicsWorld->addRigidBody(backLeftWatchTower->rigidBody);
	environmentElements.push_back(backLeftWatchTower);

	backRightWall = new StaticElement(28, 2, -51, 41, 9.0f, 5, 1.4f, 1.4f, 1.4f, 180, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(backRightWall->rigidBody);
	environmentElements.push_back(backRightWall);

	backLeftWall = new StaticElement(-28, 2, -51, 41, 9, 5, 1.4f, 1.4f, 1.4f, 180, "Models//Walls//Wall3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(backLeftWall->rigidBody);
	environmentElements.push_back(backLeftWall);

	supplies1 = new StaticElement(-10, 2, 40, 4, 4, 2, 0.08f, 0.08f, 0.08f, 30, "Models//Supplies//Supplies2x.obj", "Models//Supplies//Supplies_tex2.bmp");
	dynamicsWorld->addRigidBody(supplies1->rigidBody);
	environmentElements.push_back(supplies1);

	supplies2 = new StaticElement(-14.5f, 2, 42, 4, 4, 2, 0.08f, 0.08f, 0.08f, 30, "Models//Supplies//Supplies2x.obj", "Models//Supplies//Supplies_tex1.bmp");
	dynamicsWorld->addRigidBody(supplies2->rigidBody);
	environmentElements.push_back(supplies2);

	supplies3 = new StaticElement(-9, 2, 37, 4, 4, 2, 0.08f, 0.08f, 0.08f, 30, "Models//Supplies//Supplies2x.obj", "Models//Supplies//Supplies_tex1.bmp");
	dynamicsWorld->addRigidBody(supplies3->rigidBody);
	environmentElements.push_back(supplies3);

	supplies4 = new StaticElement(-13.5f, 2, 39, 4, 4, 2, 0.08f, 0.08f, 0.08f, 30, "Models//Supplies//Supplies2x.obj", "Models//Supplies//Supplies_tex2.bmp");
	dynamicsWorld->addRigidBody(supplies4->rigidBody);
	environmentElements.push_back(supplies4);

	cargoContainer1 = new StaticElement(20, 2, 0, 9.5f, 6, 4, 0.4f, 0.4f, 0.4f, 60, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(cargoContainer1->rigidBody);
	environmentElements.push_back(cargoContainer1);

	middleTower = new StaticElement(0, 7.5f, 0, 7.2f, 20, 7.2f, 0.8f, 0.9f, 0.95f, 90, "Models//WatchTower//BigWatchTower.obj", "Textures//rusty_metal_texture.bmp");
	dynamicsWorld->addRigidBody(middleTower->rigidBody);
	environmentElements.push_back(middleTower);

	fence1 = new StaticElement(22, 2, 56.5f, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, 0, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence1->rigidBody);
	environmentElements.push_back(fence1);

	fence2 = new StaticElement(24, 2, 49.5f, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, -31, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence2->rigidBody);
	environmentElements.push_back(fence2);

	fence3 = new StaticElement(24, 2, 37.5f, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, 31, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence3->rigidBody);
	environmentElements.push_back(fence3);

	fence4 = new StaticElement(21, 2, 30.5f, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, 15, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence4->rigidBody);
	environmentElements.push_back(fence4);

	fence5 = new StaticElement(22.5f, 2, 24, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, -42, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence5->rigidBody);
	environmentElements.push_back(fence5);

	fence6 = new StaticElement(43, 2, 20.8f, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, 69, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence6->rigidBody);
	environmentElements.push_back(fence6);

	fence7 = new StaticElement(37.2, 2, 16.2, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, 35, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence7->rigidBody);
	environmentElements.push_back(fence7);

	fence8 = new StaticElement(-7.6f, 2, 66.8f, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, 0, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence8->rigidBody);
	environmentElements.push_back(fence8);

	fence9 = new StaticElement(-9.2f, 2, 59.4f, 0.5f, 3, 8, 0.03f, 0.03f, 0.03f, 25, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence9->rigidBody);
	environmentElements.push_back(fence9);

	fence10 = new StaticElement(-2.6f, 2, 33.8f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, -57, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence10->rigidBody);
	environmentElements.push_back(fence10);

	fence11 = new StaticElement(21.8f, 2, -8.8f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, -17, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence11->rigidBody);
	environmentElements.push_back(fence11);

	fence12 = new StaticElement(24.2f, 2, -17.6f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, -14, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence12->rigidBody);
	environmentElements.push_back(fence12);

	fence13 = new StaticElement(24.8f, 2, -26.2f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, 7, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence13->rigidBody);
	environmentElements.push_back(fence13);

	fence14 = new StaticElement(-2.4f, 2, 54.4f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, -87, "Models//Fence//Fence2x.obj", "Textures//TELHA3.bmp");
	dynamicsWorld->addRigidBody(fence14->rigidBody);
	environmentElements.push_back(fence14);

	fence15 = new StaticElement(5.2f, 2, 54.2f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, -87, "Models//Fence//Fence2x.obj", "Textures//TELHA3.bmp");
	dynamicsWorld->addRigidBody(fence15->rigidBody);
	environmentElements.push_back(fence15);

	fence16 = new StaticElement(-7.4f, 2, 79.4f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, 0, "Models//Fence//Fence2x.obj", "Textures//TELHA3.bmp");
	dynamicsWorld->addRigidBody(fence16->rigidBody);
	environmentElements.push_back(fence16);

	fence17 = new StaticElement(7.4f, 2, 79.4f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, 0, "Models//Fence//Fence2x.obj", "Textures//TELHA3.bmp");
	dynamicsWorld->addRigidBody(fence17->rigidBody);
	environmentElements.push_back(fence17);

	fence18 = new StaticElement(3.4f, 2, 84.5, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, -90, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence18->rigidBody);
	environmentElements.push_back(fence18);

	fence19 = new StaticElement(-4.8f, 2, 84.5, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, -90, "Models//Fence//Fence2x.obj", "Models//House//JANELA.bmp");
	dynamicsWorld->addRigidBody(fence19->rigidBody);
	environmentElements.push_back(fence19);

	fence20 = new StaticElement(2.2f, 2, -72.0f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, 19, "Models//Fence//Fence2x.obj", "Textures//TELHA3.bmp");
	dynamicsWorld->addRigidBody(fence20->rigidBody);
	environmentElements.push_back(fence20);

	fence21 = new StaticElement(0.6f, 2, -14.4f, 0.5f, 3, 9, 0.03f, 0.03f, 0.04f, 19, "Models//Fence//Fence2x.obj", "Textures//TELHA3.bmp");
	dynamicsWorld->addRigidBody(fence21->rigidBody);
	environmentElements.push_back(fence21);

	leftGarage = new StaticElement(-40.8, 1.8f, 1, 14, 6, 13, 1.4f, 1.4f, 1.4f, 270, "Models//Garage.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(leftGarage->rigidBody);
	environmentElements.push_back(leftGarage);

	leftGarage2 = new StaticElement(-27.8, 1.8f, 1, 14, 6, 13, 1.4f, 1.4f, 1.4f, 90, "Models//Garage.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(leftGarage2->rigidBody);
	environmentElements.push_back(leftGarage2);

	leftGarage3 = new StaticElement(-40.8, 1.8f, -12.8f, 14, 6, 13, 1.4f, 1.4f, 1.4f, 270, "Models//Garage.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(leftGarage3->rigidBody);
	environmentElements.push_back(leftGarage3);

	leftGarage4 = new StaticElement(-27.8, 1.8f, -12.8f, 14, 6, 13, 1.4f, 1.4f, 1.4f, 90, "Models//Garage.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(leftGarage4->rigidBody);
	environmentElements.push_back(leftGarage4);

	leftHangar1 = new StaticElement(-40.2f, 4.4f, 14.4f, 12.5f, 6, 16, 1.3f, 1.3f, 1.3f, -180, "Models//Elements//Hangar.obj", "Textures//doomMetal.bmp");
	dynamicsWorld->addRigidBody(leftHangar1->rigidBody);
	environmentElements.push_back(leftHangar1);

	leftHangar2 = new StaticElement(-27.5f, 4.4f, 14.4f, 12.5f, 6, 16, 1.3f, 1.3f, 1.3f, -180, "Models//Elements//Hangar.obj", "Textures//doomMetal.bmp");
	dynamicsWorld->addRigidBody(leftHangar2->rigidBody);
	environmentElements.push_back(leftHangar2);

	leftHangar3 = new StaticElement(-40.2f, 4.4f, -27.4f, 12.5f, 6, 16, 1.3f, 1.3f, 1.3f, 0, "Models//Elements//Hangar.obj", "Textures//doomMetal.bmp");
	dynamicsWorld->addRigidBody(leftHangar3->rigidBody);
	environmentElements.push_back(leftHangar3);

	leftHangar4 = new StaticElement(-27.5f, 4.4f, -27.4f, 12.5f, 6, 16, 1.3f, 1.3f, 1.3f, 0, "Models//Elements//Hangar.obj", "Textures//doomMetal.bmp");
	dynamicsWorld->addRigidBody(leftHangar4->rigidBody);
	environmentElements.push_back(leftHangar4);

	rightHangar1 = new StaticElement(36.6f, 4.4f, 4.2f, 15.5f, 6, 19.5f, 1.09f, 1.09f, 1.09f, -90, "Models//Elements//BigHangar.obj", "Textures//rustyHangar.bmp");
	dynamicsWorld->addRigidBody(rightHangar1->rigidBody);
	environmentElements.push_back(rightHangar1);

	frontLeftHouse = new StaticElement(-40.3f, 5, 59.2f, 11.6f, 10.2f, 21, 0.6f, 0.6f, 0.6f, 360, "Models//FarmHouse//FarmHouse.obj", "Models//FarmHouse//Farmhouse_Texture.bmp");
	dynamicsWorld->addRigidBody(frontLeftHouse->rigidBody);
	environmentElements.push_back(frontLeftHouse);

	frontContainers = new StaticElement(-19.5f, 2.4, 68, 4, 5, 12, 0.83f, 0.83f, 0.83f, -90, "Models//Container//Container3x.obj", "Models//Container//01.bmp");
	dynamicsWorld->addRigidBody(frontContainers->rigidBody);
	environmentElements.push_back(frontContainers);

	rightGarages1 = new StaticElement(43, 2.6, 58.2, 25, 5, 8, 0.84f, 0.84f, 0.84f, 90, "Models//Walls//WallWithGarage3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(rightGarages1->rigidBody);
	environmentElements.push_back(rightGarages1);

	rightGarages2 = new StaticElement(43, 2.6, 34, 25, 5, 8, 0.84f, 0.84f, 0.84f, 90, "Models//Walls//WallWithGarage3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(rightGarages2->rigidBody);
	environmentElements.push_back(rightGarages2);

	rightGarages3 = new StaticElement(43, 2.6, -36, 25, 5, 8, 0.84f, 0.84f, 0.84f, 90, "Models//Walls//WallWithGarage3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(rightGarages3->rigidBody);
	environmentElements.push_back(rightGarages3);

	rightGarages4 = new StaticElement(43, 2.6, -20, 25, 5, 8, 0.84f, 0.84f, 0.84f, 90, "Models//Walls//WallWithGarage3x.obj", "Textures//scratch_metal.bmp");
	dynamicsWorld->addRigidBody(rightGarages4->rigidBody);
	environmentElements.push_back(rightGarages4);

	onRightGarage1 = new StaticElement(43, 6, 44, 6, 6, 5, 0.9f, 0.7f, 6.5f, 180, "Models//MetalHouse.obj", "Textures//Military-Texture-grey-1024x1024.bmp");
	dynamicsWorld->addRigidBody(onRightGarage1->rigidBody);
	environmentElements.push_back(onRightGarage1);

	onRightGarage2 = new StaticElement(43, 6, -26, 6, 6, 5, 0.9f, 0.7f, 6.3f, 180, "Models//MetalHouse.obj", "Textures//Military-Texture-grey-1024x1024.bmp");
	dynamicsWorld->addRigidBody(onRightGarage2->rigidBody);
	environmentElements.push_back(onRightGarage2);

	midContainer1 = new StaticElement(0, 2, 25.2, 7.5f, 4, 3, 0.33f, 0.33f, 0.33, -45, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(midContainer1->rigidBody);
	environmentElements.push_back(midContainer1);

	midContainer2 = new StaticElement(4, 2, 28, 7.5f, 4, 3, 0.33f, 0.33f, 0.33, 60, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(midContainer2->rigidBody);
	environmentElements.push_back(midContainer2);

	midContainer3 = new StaticElement(15, 1.8f, -28.2, 7.5f, 4, 3, 0.33f, 0.33f, 0.33, 41, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(midContainer3->rigidBody);
	environmentElements.push_back(midContainer3);

	leftContainer = new StaticElement(-46.4f, 2.8f, 32.8f, 16, 5, 4, 0.70f, 0.47f, 0.47f, 90, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(leftContainer->rigidBody);
	environmentElements.push_back(leftContainer);

	bigHouse = new StaticElement(0, 5.8f, -45.4, 0, 0, 0, 0.37f, 0.37f, 0.37f, 0, "Models//BigHouse//BigHouse.obj", "Textures//vintage_wood_texture.bmp");
	dynamicsWorld->addRigidBody(bigHouse->rigidBody);
	environmentElements.push_back(bigHouse);

	bigHouseLeftWall = new btRigidBody(0, nullptr, new btBoxShape(btVector3(0.1f, 5, 3.5f)));
	bigHouseLeftWall->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(-9.3f, 5.8, -47.3f)));
	dynamicsWorld->addRigidBody(bigHouseLeftWall);

	bigHouseRightWall = new btRigidBody(0, nullptr, new btBoxShape(btVector3(0.1f, 5, 3.5f)));
	bigHouseRightWall->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(9.9f, 5.8, -47.3f)));
	dynamicsWorld->addRigidBody(bigHouseRightWall);

	bigHouseLeftWall1 = new btRigidBody(0, nullptr, new btBoxShape(btVector3(2.3f, 5, 0.1f)));
	bigHouseLeftWall1->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(-6.9f, 5.8, -43.3f)));
	dynamicsWorld->addRigidBody(bigHouseLeftWall1);

	bigHouseRightWall1 = new btRigidBody(0, nullptr, new btBoxShape(btVector3(2.3f, 5, 0.1f)));
	bigHouseRightWall1->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(7.5f, 5.8, -43.3f)));
	dynamicsWorld->addRigidBody(bigHouseRightWall1);

	bigHouseLeftWall2 = new btRigidBody(0, nullptr, new btBoxShape(btVector3(0.1f, 5, 2.3f)));
	bigHouseLeftWall2->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(-4.8f, 5.8, -41.3f)));
	dynamicsWorld->addRigidBody(bigHouseLeftWall2);

	bigHouseRightWall2 = new btRigidBody(0, nullptr, new btBoxShape(btVector3(0.1f, 5, 2.3f)));
	bigHouseRightWall2->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(5.4f, 5.8, -41.1f)));
	dynamicsWorld->addRigidBody(bigHouseRightWall2);

	bigHouseLeftWall3 = new btRigidBody(0, nullptr, new btBoxShape(btVector3(1.5f, 5, 0.1f)));
	bigHouseLeftWall3->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(-3.2f, 5.8, -38.9f)));
	dynamicsWorld->addRigidBody(bigHouseLeftWall3);

	bigHouseRightWall3 = new btRigidBody(0, nullptr, new btBoxShape(btVector3(1.5f, 5, 0.1f)));
	bigHouseRightWall3->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(4.0f, 5.8, -38.9f)));
	dynamicsWorld->addRigidBody(bigHouseRightWall3);

	bigHouseBackWall = new btRigidBody(0, nullptr, new btBoxShape(btVector3(8.0f, 3, 0.1f)));
	bigHouseBackWall->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(0, 5.8, -51.5f)));
	dynamicsWorld->addRigidBody(bigHouseBackWall);

	for (float i = 0; i < 40; i++)
	{
		bigHouseStairs1 = new btRigidBody(0, nullptr, new btBoxShape(btVector3(1.2f, 0.1f, 4.3f)));
		bigHouseStairs1->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(0)), btScalar(0), btScalar(0)), btVector3(11.5f, 0.7 + i* 0.1f, -45.5f - 0.141f*i)));
		dynamicsWorld->addRigidBody(bigHouseStairs1);
	}

	backCargoContainer = new StaticElement(25, 2.6f, -46.4f, 10, 3, 4, 0.44f, 0.44f, 0.44f, 0, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(backCargoContainer->rigidBody);
	environmentElements.push_back(backCargoContainer);

	backCargoContainer1 = new StaticElement(-15.2f, 2.6f, -46.4f, 10, 3, 4, 0.44f, 0.44f, 0.44f, 0, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(backCargoContainer1->rigidBody);
	environmentElements.push_back(backCargoContainer1);

	backCargoContainer2 = new StaticElement(-26.2f, 2.6f, -46.4f, 10, 3, 4, 0.44f, 0.44f, 0.44f, 0, "Models//CargoContainer//CargoContainer.obj", "Models//CargoContainer//cargoContainer_texture.bmp");
	dynamicsWorld->addRigidBody(backCargoContainer2->rigidBody);
	environmentElements.push_back(backCargoContainer2);

	supplies5 = new StaticElement(-4.4f, 2, -21.6, 4, 4, 2, 0.08f, 0.08f, 0.08f, -15, "Models//Supplies//Supplies2x.obj", "Models//Supplies//Supplies_tex2.bmp");
	dynamicsWorld->addRigidBody(supplies5->rigidBody);
	environmentElements.push_back(supplies5);

	supplies6 = new StaticElement(-6.8f, 2, -22.6, 4, 4, 2, 0.08f, 0.08f, 0.08f, -15, "Models//Supplies//Supplies2x.obj", "Models//Supplies//Supplies_tex2.bmp");
	dynamicsWorld->addRigidBody(supplies6->rigidBody);
	environmentElements.push_back(supplies6);


	enemy1 = new Enemy(0, 2, -30, 1, 1.8f, 1);
	enemies.push_back(enemy1);
	dynamicsWorld->addRigidBody(enemy1->rigidBody);

	enemy2 = new Enemy(4, 2, -20, 1, 1.8f, 1);
	enemies.push_back(enemy2);
	dynamicsWorld->addRigidBody(enemy2->rigidBody);

	enemy3 = new Enemy(-20, 2, 40, 1, 1.8f, 1);
	enemies.push_back(enemy3);
	dynamicsWorld->addRigidBody(enemy3->rigidBody);

	enemy4 = new Enemy(40, 2, 60, 1, 1.8f, 1);
	enemies.push_back(enemy4);
	dynamicsWorld->addRigidBody(enemy4->rigidBody);

	enemy5 = new Enemy(20, 2, -20, 1, 1.8f, 1);
	enemies.push_back(enemy5);
	dynamicsWorld->addRigidBody(enemy5->rigidBody);


	pistol = new Pistol("Models//Handgun//Handgun1.obj", "Models//Handgun//Handgun_texture.bmp");
}

void deleteEnvironmentElements()
{
	environmentElements.clear();
}