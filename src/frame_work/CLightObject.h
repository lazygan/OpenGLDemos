/*
* File:   CLightObject.h
* Desc:		Trida reprezentujici svetlo ve scene
*
*/
#ifndef CLIGHTOBJECT_H
#define CLIGHTOBJECT_H
#pragma once


class CLightObject
{
public:
	CLightObject();
	CLightObject(glm::vec3 pos, glm::vec3 dir,glm::vec3 color);
	~CLightObject();

	void visualize();
	void computeMatrixes();
	glm::vec3 getPosition();
	glm::vec3 getDirection();
    glm::vec3 getColor();
	glm::mat4 getProjMatrix();
	glm::mat4 getViewMatrix();
	glm::vec3 getRight();
	float getHorAngle();
	float getVerAngle();
	float getAspectRatio();
	float getFov();

	void setPosition(glm::vec3);
	void setDirection(glm::vec3);
	void setUp(glm::vec3);
	void setHorAngle(float);
	void setVerAngle(float);
private:
    glm::vec3 color;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat4 ViewMatrix, ProjectionMatrix;
	float verticalAngle, horizontalAngle, aspect, fov;
};

#endif

