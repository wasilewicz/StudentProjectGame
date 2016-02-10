#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	isCameraFree = false;
}

void Camera::update(vec3 player_position, vec3 player_direction)
{
	/*Kamera lekko z góry*/
	if (notFreeCameraState == NotFreeCameraState::ORTHOGRAPHIC)
	{
		position.x = player_position.x + (OrthographicDistanceXZ * player_direction.x);
		position.y = player_position.y + OrthographicDistanceY;
		position.z = player_position.z + (OrthographicDistanceXZ * player_direction.z);
	}
	else if (notFreeCameraState == NotFreeCameraState::TPS)
	{
		/*TPS Kamera*/
		position.x = player_position.x + (TPSdistanceXZ * player_direction.x) + ((-player_direction.z));
		position.y = player_position.y + TPSdistanceY - player_direction.y;
		position.z = player_position.z + (TPSdistanceXZ * player_direction.z) + ((player_direction.x));
	}
}

void Camera::control(bool *keystate)
{
	if (keystate['w'])
	{
		position.x += direction.x * speed;
		position.y += direction.y * speed;
		position.z += direction.z * speed;
	}
	if (keystate['s'])
	{
		position.x -= direction.x * speed;
		position.y -= direction.y * speed;
		position.z -= direction.z * speed;
	}
	if (keystate['a'])
	{
		position.x -= -direction.z * speed;
		position.y -= direction.y * speed;
		position.z -= direction.x * speed;
	}
	if (keystate['d'])
	{
		position.x -= direction.z * speed;
		position.y -= direction.y * speed;
		position.z -= -direction.x * speed;
	}

}

void Camera::setIsCameraFree()
{
	isCameraFree = !isCameraFree;
}

void Camera::changeNotFreeCameraType()
{
	if (notFreeCameraState == ORTHOGRAPHIC)
	{
		notFreeCameraState = TPS;
	}
	else
	{
		notFreeCameraState = ORTHOGRAPHIC;
	}
}

void Camera::mouseControl(float windowWidth, float windowHeight, float mouseX, float mouseY)
{
	horizontal_angle += 0.01f * (windowWidth / 2 - mouseX);
	vertical_angle += 0.01f * (windowHeight / 2 - mouseY);

	direction.x = cos(vertical_angle) * sin(horizontal_angle);
	direction.y = sin(vertical_angle);
	direction.z = cos(vertical_angle) * cos(horizontal_angle);
}