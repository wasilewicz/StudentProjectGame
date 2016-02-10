#include "stdafx.h"
#include "Helpers.h"

void RemoveQuotes(std::string& str)
{
	size_t n;
	while ((n = str.find('\"')) != std::string::npos) str.erase(n, 1);
}

int GetFileLength(std::istream& file)
{
	int pos = file.tellg();
	file.seekg(0, std::ios::end);
	int length = file.tellg();
	file.seekg(pos);

	return length;
}

void IgnoreLine(std::istream& file, int length)
{
	file.ignore(length, '\n');
}

void ComputeQuatW(glm::quat& quat)
{
	float t = 1.0f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
	if (t < 0.0f)
	{
		quat.w = 0.0f;
	}
	else
	{
		quat.w = -sqrtf(t);
	}
}

ElapsedTime::ElapsedTime(float maxTimeStep)
	: m_fMaxTimeStep(maxTimeStep)
	, m_fPrevious(std::clock() / (float)CLOCKS_PER_SEC)
{}

float ElapsedTime::GetElapsedTime() const
{
	float fCurrentTime = std::clock() / (float)CLOCKS_PER_SEC;
	float fDeltaTime = fCurrentTime - m_fPrevious;
	m_fPrevious = fCurrentTime;

	if (fDeltaTime < m_fMaxTimeStep)
		fDeltaTime = fDeltaTime;
	else
		fDeltaTime = m_fMaxTimeStep;

	return fDeltaTime;
}

float degreesToRadians(float degrees)
{
	return degrees * 0.0174;
}

float radiansToDegrees(float radians)
{
	return radians / 0.0174;
}

float distanceBetween2Points(float x1, float z1, float x2, float z2)
{
	return sqrtf(pow(x2 - x1, 2) + pow(z2 - z1, 2));
}
