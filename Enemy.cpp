#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy(float x, float y, float z, float length_x, float length_y, float length_z)
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

	/*inicjowanie fizycznego rigidbody*/
	btTransform rigidBodyTransform;
	rigidBodyTransform.setIdentity();
	rigidBodyTransform.setOrigin(btVector3(position.x, position.y, position.z));
	btDefaultMotionState* myMotionState = new btDefaultMotionState(rigidBodyTransform);
	btCollisionShape* shape = new btCapsuleShape(length_x / 2, length_y);
	rigidBody = new btRigidBody(10, myMotionState, shape);
}

void Enemy::loadingAnimations()
{
	idle.LoadModel("Models//Human//Human.md5mesh");
	idle.LoadAnim("Models//Human//Human_Idle.md5anim");

	run.LoadModel("Models//Human//Human.md5mesh");
	run.LoadAnim("Models//Human//Human_Run.md5anim");

	walk.LoadModel("Models//Human//Human.md5mesh");
	walk.LoadAnim("Models//Human//Human_Walk.md5anim");

	holdWeapon.LoadModel("Models//Human//Human.md5mesh");
	holdWeapon.LoadAnim("Models//Human//Human_HoldWeapon.md5anim");

	death.LoadModel("Models//Human//Human.md5mesh");
	death.LoadAnim("Models//Human//Human_Death.md5anim");
}

void Enemy::update(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, const string playerState, bool &isPlayerDead)
{

	rigidBody->activate(true);
	if (isDead == true)
	{
		if (death_anim_frames_count == 0)
		{
			rigidBody->activate(false);
			model->setAnimation(death);
		}
		death_anim_frames_count++;
	}
	else if (state == State::WALKING)
	{
		walking(dynamicsWorld, playerPosition, playerState);
	}
	else if (state == State::AGGRESSIVE)
	{
		aggressive(dynamicsWorld, playerPosition, isPlayerDead);
	}
	else if (state == State::AWARE)
	{
		aware(dynamicsWorld, playerPosition, playerState);
	}

	angle = -(atan2(direction.z, direction.x) * 180 / 3.14f);

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

void Enemy::draw()
{
	model->draw();
}

/*AI*/
void Enemy::walking(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, const string playerState)
{
	
	if (nearToCollision(dynamicsWorld))
	{
		walking_count = 0;
	}

	if (walking_count == 0)
	{
		model->setAnimation(walk);
		direction.x = ((float)(rand() % 1000 + 1)) / 1000;
		if (rand() % 2 + 1 == 1)
			direction.x *= -1;
		direction.z = ((float)(rand() % 1000 + 1)) / 1000;
		if (rand() % 2 + 1 == 1)
			direction.z *= -1;
		walking_count++;
	}
	else
	{
		if (walking_count == 500)
			walking_count = 0;
		else
			walking_count++;
	}
	movement();
	
	if (seesPlayer(dynamicsWorld, playerPosition))
	{
		lastSeenPlayerPosition.x = playerPosition->x;
		lastSeenPlayerPosition.y = playerPosition->y;
		lastSeenPlayerPosition.z = playerPosition->z;
		model->setAnimation(run);
		state = AGGRESSIVE;
	}
	else if (hearsPlayer(playerPosition, playerState))
	{
		lastSeenPlayerPosition.x = playerPosition->x;
		lastSeenPlayerPosition.y = playerPosition->y;
		lastSeenPlayerPosition.z = playerPosition->z;
		state = AWARE;
	}
}

bool Enemy::nearToCollision(btDiscreteDynamicsWorld* dynamicsWorld)
{
	btVector3 start = btVector3(position.x, position.y, position.z);
	btVector3 end = btVector3(position.x + direction.x * 10, position.y, position.z + direction.z * 10);
	btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
	dynamicsWorld->rayTest(start, end, RayCallback);
	if (RayCallback.hasHit())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Enemy::seesPlayer(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition)
{
	float endX, endZ;
	btVector3 start = btVector3(position.x, position.y, position.z);
	if (playerPosition->x > position.x)
		endX = playerPosition->x - 0.5f;
	else
		endX = playerPosition->x + 0.5f;

	if (playerPosition->z > position.z)
		endZ = playerPosition->z - 0.5f;
	else
		endZ = playerPosition->z + 0.5f;

	btVector3 end = btVector3(endX, position.y, endZ);
	btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
	dynamicsWorld->rayTest(start, end, RayCallback);
	if (RayCallback.hasHit() || distanceBetween2Points(position.x, position.z, endX, endZ) > sightDistance )
	{
		return false;
	}
	else
	{
		float vectorToPlayerX = playerPosition->x - position.x;
		float vectorToPlayerZ = playerPosition->z - position.z;
		float angleBetween = radiansToDegrees(acosf((vectorToPlayerX * direction.x + vectorToPlayerZ * direction.z) / (sqrt(pow(vectorToPlayerX, 2) + pow(vectorToPlayerZ, 2)) * sqrt(pow(direction.x, 2) + pow(direction.z, 2)))));
		if (angleBetween < sightAngle / 2)
		{
			seeing_count = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Enemy::hearsPlayer(vec3* playerPosition, const string playerState)
{
	if (playerState == "Sneaking" || playerState == "Standing")
		return false;
	if (playerState == "Sprinting" && distanceBetween2Points(position.x, position.z, playerPosition->x, playerPosition->z) < hearingRadius)
		return true;
	if (playerState == "Walking" && distanceBetween2Points(position.x, position.z, playerPosition->x, playerPosition->z) < hearingRadius / 2)
		return true;
	else
		return false;
}

void Enemy::movement()
{
	float _speed;
	if (state == State::WALKING)
	{
		_speed = speed;
	}
	else if (state == State::AGGRESSIVE)
	{
		_speed = sprint_speed;
	}
	else
	{
		_speed = speed;
	}

	float tmpY = rigidBody->getLinearVelocity().getY();
	float directionProportionX = abs(direction.x) / (abs(direction.x) + abs(direction.z));
	float directionProportionZ = abs(direction.z) / (abs(direction.x) + abs(direction.z));
	float velX, velZ;
	if (direction.x > 0)
	{
		velX = directionProportionX * _speed;
	}
	else
	{
		velX = -(directionProportionX * _speed);
	}
	if (direction.z > 0)
	{
		velZ = directionProportionZ * _speed;
	}
	else
	{
		velZ = -(directionProportionZ * _speed);
	}
	rigidBody->setLinearVelocity(btVector3(velX, tmpY, velZ));
}

void Enemy::aggressive(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, bool &isPlayerDead)
{
	if (seesPlayer(dynamicsWorld, playerPosition))
	{
		lastSeenPlayerPosition.x = playerPosition->x;
		lastSeenPlayerPosition.y = playerPosition->y;
		lastSeenPlayerPosition.z = playerPosition->z;
		seeing_count = 0;
	}
	goToPlace(&lastSeenPlayerPosition);

	if (abs(position.x - lastSeenPlayerPosition.x) <= 0.3f && abs(position.z - lastSeenPlayerPosition.z) <= 0.3f && seesPlayer(dynamicsWorld, playerPosition) == false || seeing_count >= 200)
	{
		seeing_count = 0;
		model->setAnimation(walk);
		state = State::WALKING;
		return;
	}

	seeing_count++;


	if (distanceBetween2Points(position.x, position.z, playerPosition->x, playerPosition->z) < shot_distance && seesPlayer(dynamicsWorld, playerPosition) && shotCount == 0)
	{
		rigidBody->setLinearVelocity(btVector3(0, 0, 0));
		if (isPointing == false)
		{
			isPointing = true;
			model->setAnimation(holdWeapon);
		}
		shot(dynamicsWorld, playerPosition, isPlayerDead);
		shotCount++;
	}
	else
	{
		if (shotCount == shot_anim_length && (distanceBetween2Points(position.x, position.z, playerPosition->x, playerPosition->z) > shot_distance || seesPlayer(dynamicsWorld, playerPosition) == false))
		{
			if (isPointing == true)
			{
				isPointing = false;
				model->setAnimation(run);
			}
			movement();
		}

		shotCount++;
		if (shotCount == breakBetweenShots)
			shotCount = 0;
	}
}

void Enemy::goToPlace(vec3* placePosition)
{
	float z, x;
	z = placePosition->z - this->position.z;
	x = placePosition->x - this->position.x;

	direction.x = x / (abs(x) + abs(z));
	direction.z = z / (abs(x) + abs(z));
}

void Enemy::aware(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, const string playerState)
{
	if (seesPlayer(dynamicsWorld, playerPosition))
	{
		lastSeenPlayerPosition.x = playerPosition->x;
		lastSeenPlayerPosition.y = playerPosition->y;
		lastSeenPlayerPosition.z = playerPosition->z;
		model->setAnimation(run);
		state = State::AGGRESSIVE;
		return;
	}
	else if (hearsPlayer(playerPosition, playerState))
	{
		lastSeenPlayerPosition.x = playerPosition->x;
		lastSeenPlayerPosition.y = playerPosition->y;
		lastSeenPlayerPosition.z = playerPosition->z;
		seeing_count = 0;
	}
	goToPlace(&lastSeenPlayerPosition);

	if (abs(position.x - lastSeenPlayerPosition.x) <= 0.3f && abs(position.z - lastSeenPlayerPosition.z) <= 0.3f && seesPlayer(dynamicsWorld, playerPosition) == false || seeing_count == 200)
	{
		model->setAnimation(walk);
		state = State::WALKING;
	}

	if (nearToCollision(dynamicsWorld))
	{
		direction.x += 0.3f;
		direction.z -= 0.3f;
	}

	seeing_count++;
	movement();

}

void Enemy::shot(btDiscreteDynamicsWorld* dynamicsWorld, vec3* playerPosition, bool &isPlayerDead)
{
	vec3* vec = new vec3(position.x, position.y, position.z);
	bullets.push_back(vec);
		if (rand() % hit_chance + 1 == hit_chance)
		{	
			isPlayerDead = true;
		}
}

