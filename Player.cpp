#include "stdafx.h"
#include "Player.h"


Player::Player(float x, float y, float z, float length_x, float length_y, float length_z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	direction.x = 0.0f;
	direction.y = 0.0f;
	direction.z = -1.0f;

	length.x = length_x;
	length.y = length_y;
	length.z = length_z;

	model = new DynamicModel(position.x, position.y, position.z, 1.5f, 1.5f, 1.5f, "Models//Human//Human.md5mesh", "Models//Human//Human_Idle.md5anim");

	loadingAnimations();

	/*inicjowanie fizycznego rigidbody)*/
	btTransform rigidBodyTransform;
	rigidBodyTransform.setIdentity();
	rigidBodyTransform.setOrigin(btVector3(position.x, position.y, position.z));
	btDefaultMotionState* myMotionState = new btDefaultMotionState(rigidBodyTransform);
	btCollisionShape* shape = new btCapsuleShape(length_x / 2, length_y);
	rigidBody = new btRigidBody(1, myMotionState, shape);
}

void Player::update()
{
	angle = -(atan2(direction.z, direction.x) * 180 / 3.14f);

	if (state == HOLD_WEAPON)
	{
		model->verticalAngle = radiansToDegrees(asinf(direction.y));
	}
	if (isStabbing == true)
	{
		if (stab_anim_frames_count == 0)
		{
			rigidBody->setLinearVelocity(btVector3(0, 0, 0));
			model->setAnimation(stab);
			stab_anim_frames_count++;
		}
		else if (stab_anim_frames_count >= stab_anim_frames)
		{
			isStabbing = false;
			rigidBody->activate(true);
			stab_anim_frames_count = 0;
			if (isMoving)
			{
				if (isSprinting)
					model->setAnimation(run);
				else if (isSneaking)
					model->setAnimation(sneak);
				else
					model->setAnimation(walk);
			}
			else
				model->setAnimation(idle);
		}
		else
			stab_anim_frames_count++;
	}
	/*Wyciagniecie pozycji rigidbody z macierzy*/
	btScalar openGlMatrix[16];
	rigidBody->getWorldTransform().getOpenGLMatrix(openGlMatrix);
	position.x = openGlMatrix[12];
	position.y = openGlMatrix[13];
	position.z = openGlMatrix[14];

	float poprawka_wysokosci = -1.4f;
	model->updatePositon(position.x, position.y + poprawka_wysokosci, position.z);
	model->updateRotation(angle);
	rigidBody->setWorldTransform(btTransform(btQuaternion(btScalar(degreesToRadians(angle)), btScalar(0), btScalar(0)), btVector3(position.x, position.y, position.z)));
}

void Player::control(bool *keystate, char &keyUp, char &keyDown, bool endOfGame)
{
	if (isDead == false && isStabbing == false && endOfGame == false)
	{
		btVector3 velocity(0, 0, 0);

		if (keystate['w'])
		{
			rigidBody->activate(true);
			isMoving = true;
			if (state == IDLE)
			{
				if (isSprinting)
					velocity += btVector3(direction.x * sprint_speed, 0, direction.z * sprint_speed);
				else if (isSneaking)
					velocity += btVector3(direction.x * slower_speed, 0, direction.z * slower_speed);
				else
					velocity += btVector3(direction.x * speed, 0, direction.z * speed);
			}
			else
				velocity += btVector3(direction.x * pointing_speed, 0, direction.z * pointing_speed);
		}
		if (keystate['s'])
		{
			rigidBody->activate(true);
			isMoving = true;
			if (state == IDLE)
				velocity += btVector3(-direction.x * slower_speed, 0, -direction.z * slower_speed);
			else
				velocity += btVector3(-direction.x * pointing_speed, 0, -direction.z * pointing_speed);
		}
		if (keystate['a'])
		{
			rigidBody->activate(true);
			isMoving = true;
			if (state == IDLE)
			{
					velocity += btVector3(direction.z * slower_speed, 0, -direction.x * slower_speed);
			}
			else
				velocity += btVector3(direction.z * pointing_speed, 0, -direction.x * pointing_speed);
		}
		if (keystate['d'])
		{
			rigidBody->activate(true);
			isMoving = true;
			if (state == IDLE)
			{
				velocity += btVector3(-direction.z * slower_speed, 0, direction.x * slower_speed);
			}
			else
				velocity += btVector3(-direction.z * pointing_speed, 0, direction.x * pointing_speed);
		}

		/*Spowolnienie w razie wcisniecia dwoch przyciskow*/
		if (keystate['w'] && keystate['a'] || keystate['w'] && keystate['d'] || keystate['s'] && keystate['a'] || keystate['s'] && keystate['d'])
		{
			velocity *= btVector3(0.6f, 0.6f, 0.6f);
		}

		btVector3 tmpVelocity = rigidBody->getLinearVelocity();
		rigidBody->setLinearVelocity(btVector3(velocity.getX(), tmpVelocity.getY(), velocity.getZ()));

		if (keyDown == 'a')
		{
			if (state == IDLE && keystate['w'] == false)
			{
					model->setAnimation(sneak);
			}
		}
		else if (keyDown == 'd')
		{
			if (state == IDLE && keystate['w'] == false)
			{
					model->setAnimation(sneak);
			}
		}
		if (keyDown == 'w')
		{
			if (state == IDLE)
			{
				if (isSprinting)
					model->setAnimation(run);
				else if (isSneaking)
					model->setAnimation(sneak);
				else
					model->setAnimation(walk);
			}
		}

		if (keyDown == 's')
		{
			if (state == IDLE)
			{
				if (isSneaking)
					model->setAnimation(sneak);
				else
					model->setAnimation(walk);
			}
		}
		if (keyUp == 'w' || keyUp == 's')
		{
			keystate['W'] = false;
			keystate['w'] = false;
			if (state == IDLE)
			{
				model->setAnimation(idle);
			}
		}
		if (keyUp == 'w' || keyUp == 'a' || keyUp == 'd' || keyUp == 's')
		{
			if (state == IDLE && keystate['w'] == false && keystate['s'] == false && keystate['a'] == false && keystate['d'] == false)
			{
				model->setAnimation(idle);
				isMoving = false;
			}
		}

		if (keyDown == 'c')
		{
			isSneaking = !isSneaking;
			if (keystate['w'])
			{
				if (isSneaking)
					model->setAnimation(sneak);
				else
					model->setAnimation(walk);
			}
			isSprinting = false;
		}
		if (keyDown == 'x')
		{
			isSneaking = false;
			isSprinting = !isSprinting;
			if (keystate['w'])
			{
				if (isSprinting)
					model->setAnimation(run);
				else
					model->setAnimation(walk);
			}
			if (keystate['s'])
			{
				model->setAnimation(walk);
			}
		}

		/*wyzerowanie przyciskow*/
		keyDown = 1;
		keyUp = 1;
	}
	else
	{
		if (death_anim_frames_count == 0 && isStabbing == false)
			model->setAnimation(death);
		death_anim_frames_count++;
	}
}

void Player::draw()
{
	model->draw();
}


void Player::loadingAnimations()
{
	idle.LoadModel("Models//Human//Human.md5mesh");
	idle.LoadAnim("Models//Human//Human_Idle.md5anim");

	run.LoadModel("Models//Human//Human.md5mesh");
	run.LoadAnim("Models//Human//Human_Run.md5anim");

	walk.LoadModel("Models//Human//Human.md5mesh");
	walk.LoadAnim("Models//Human//Human_Walk.md5anim");

	holdWeapon.LoadModel("Models//Human//Human.md5mesh");
	holdWeapon.LoadAnim("Models//Human//Human_HoldWeapon.md5anim");

	sneak.LoadModel("Models//Human//Human.md5mesh");
	sneak.LoadAnim("Models//Human//Human_Sneak.md5anim");

	death.LoadModel("Models//Human//Human.md5mesh");
	death.LoadAnim("Models//Human//Human_Death.md5anim");

	stab.LoadModel("Models//Human//Human.md5mesh");
	stab.LoadAnim("Models//Human//Human_Stab.md5anim");
}

void Player::mouseControl(float windowWidth, float windowHeight, float mouseX, float mouseY, bool endOfGame)
{
	if (isDead == false && isStabbing == false && endOfGame == false)
	{
		horizontal_angle += 0.01f * (windowWidth / 2 - mouseX);
		direction.x = cos(vertical_angle) * sin(horizontal_angle);
		direction.y = sin(vertical_angle);
		direction.z = cos(vertical_angle) * cos(horizontal_angle);
	}
}

void Player::mouseControlOrthographic(float windowWidth, float windowHeight, float mouseX, float mouseY)
{
	if (isDead == false && isStabbing == false)
	{
		horizontal_angle += 0.01f * (windowWidth / 2 - mouseX);
		vertical_angle = 0;
		direction.x = cos(vertical_angle) * sin(horizontal_angle);
		direction.z = cos(vertical_angle) * cos(horizontal_angle);
	}
}

string Player::getMovementStateString()
{
	if (isSneaking)
		return "Sneaking";
	else if (isSprinting && isMoving)
		return "Sprinting";
	else if (isMoving)
		return "Walking";
	else
		return "Standing";
}

bool Player::isSeeingEnemy(btDiscreteDynamicsWorld* dynamicsWorld, vec3* enemyPosition)
{
	float endX, endZ;
	btVector3 start = btVector3(position.x, position.y, position.z);
	if (enemyPosition->x > position.x)
		endX = enemyPosition->x - 0.5f;
	else
		endX = enemyPosition->x + 0.5f;

	if (enemyPosition->z > position.z)
		endZ = enemyPosition->z - 0.5f;
	else
		endZ = enemyPosition->z + 0.5f;

	btVector3 end = btVector3(endX, position.y, endZ);
	btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
	dynamicsWorld->rayTest(start, end, RayCallback);
	if (RayCallback.hasHit() || distanceBetween2Points(position.x, position.z, endX, endZ) > 40)
	{
		return false;
	}
	else
	{
		float vectorToPlayerX = enemyPosition->x - position.x;
		float vectorToPlayerZ = enemyPosition->z - position.z;
		float angleBetween = radiansToDegrees(acosf((vectorToPlayerX * direction.x + vectorToPlayerZ * direction.z) / (sqrt(pow(vectorToPlayerX, 2) + pow(vectorToPlayerZ, 2)) * sqrt(pow(direction.x, 2) + pow(direction.z, 2)))));
		if (angleBetween < 110 / 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}