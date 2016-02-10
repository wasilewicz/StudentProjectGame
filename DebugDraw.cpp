#include "stdafx.h"
#include "DebugDraw.h"

void DebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	glBegin(GL_LINES);
		glColor3f(color.getX(), color.getY(), color.getZ());
		glVertex3f(from.getX(), from.getY(), from.getZ());
		glVertex3f(to.getX(), to.getY(), to.getZ());
	glEnd();
}

void DebugDraw::drawBox(const btVector3 &bbMin, const btVector3 &bbMax, const btTransform &trans, const btVector3 &color)
{
	float openGlMatrix[16];
	trans.getOpenGLMatrix(openGlMatrix);
	btScalar transX = openGlMatrix[12];
	btScalar transY = openGlMatrix[13];
	btScalar transZ = openGlMatrix[14];
	btVector3 translateVector(transX, transY, transZ);
	glPushMatrix();
		glMultMatrixf(openGlMatrix);
		drawLine(btVector3(bbMin.getX(), bbMin.getY(), bbMin.getZ()), btVector3(bbMax.getX(), bbMin.getY(), bbMin.getZ()), color);
		drawLine(btVector3(bbMin.getX(), bbMin.getY(), bbMin.getZ()), btVector3(bbMin.getX(), bbMax.getY(), bbMin.getZ()), color);
		drawLine(btVector3(bbMin.getX(), bbMax.getY(), bbMin.getZ()), btVector3(bbMax.getX(), bbMax.getY(), bbMin.getZ()), color);
		drawLine(btVector3(bbMax.getX(), bbMax.getY(), bbMin.getZ()), btVector3(bbMax.getX(), bbMin.getY(), bbMin.getZ()), color);
		drawLine(btVector3(bbMin.getX(), bbMin.getY(), bbMax.getZ()), btVector3(bbMin.getX(), bbMin.getY(), bbMin.getZ()), color);
		drawLine(btVector3(bbMin.getX(), bbMin.getY(), bbMax.getZ()), btVector3(bbMin.getX(), bbMax.getY(), bbMax.getZ()), color);
		drawLine(btVector3(bbMin.getX(), bbMax.getY(), bbMax.getZ()), btVector3(bbMin.getX(), bbMax.getY(), bbMin.getZ()), color);
		drawLine(btVector3(bbMin.getX(), bbMax.getY(), bbMax.getZ()), btVector3(bbMax.getX(), bbMax.getY(), bbMax.getZ()), color);
		drawLine(btVector3(bbMax.getX(), bbMax.getY(), bbMax.getZ()), btVector3(bbMax.getX(), bbMax.getY(), bbMin.getZ()), color);
		drawLine(btVector3(bbMax.getX(), bbMax.getY(), bbMax.getZ()), btVector3(bbMax.getX(), bbMin.getY(), bbMax.getZ()), color);
		drawLine(btVector3(bbMax.getX(), bbMin.getY(), bbMax.getZ()), btVector3(bbMin.getX(), bbMin.getY(), bbMax.getZ()), color);
		drawLine(btVector3(bbMax.getX(), bbMin.getY(), bbMax.getZ()), btVector3(bbMax.getX(), bbMin.getY(), bbMin.getZ()), color);
	glPopMatrix();
}

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{}

void DebugDraw::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
{}

void DebugDraw::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
{}

void DebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{}

void DebugDraw::reportErrorWarning(const char* warningString)
{}

void DebugDraw::draw3dText(const btVector3& location, const char* textString)
{}

void DebugDraw::setDebugMode(int debugMode)
{
	this->debugMode = debugMode;
}