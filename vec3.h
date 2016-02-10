#pragma once
struct vec3
{
	float x, y, z;

	vec3 operator+(vec3 &a)
	{
		vec3 vec;
		vec.x = this->x + a.x;
		vec.y = this->y + a.y;
		vec.z = this->z + a.z;
		return vec;
	}

	vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	vec3()
	{}
};

struct vec2
{
	float x, y;
};