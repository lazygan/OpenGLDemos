

#include "CLightObject.h"

CLightObject::CLightObject() : position(glm::vec3(0.0, 0.0, 0.0)), direction(glm::vec3(0, 2, 2)), up(glm::vec3(0, 1, 0))
{
	//computeMatrixes();
}

CLightObject::CLightObject(glm::vec3 pos, glm::vec3 dir,glm::vec3 color) : up(glm::vec3(0, 1, 0)), horizontalAngle(0.0f), verticalAngle(3.14f / 4.0f), fov(90.0)
{
	this->position = pos;
	this->direction = dir;
	this->color = color;
	this->aspect = (float)(SHADOWMAPSIZE) / (float)(SHADOWMAPSIZE);
	//computeMatrixes();
}


CLightObject::~CLightObject()
{
}


void CLightObject::computeMatrixes() {
	//float aspec = (float)(SHADOWMAPSIZE) / (float)(SHADOWMAPSIZE);
#ifndef ORTHO_PROJECTION
	this->ProjectionMatrix = glm::perspective<float>(fov, aspect, 0.1f, 1000.0f);
#else
	this->ProjectionMatrix = glm::ortho<float>(-40,40,-40,40,-100,100);
#endif

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	//Viz http://www.lighthouse3d.com/wp-content/uploads/2011/04/vfpoints2.gif
	this->direction = glm::vec3(
		cos(this->verticalAngle) * sin(this->horizontalAngle),
		sin(this->verticalAngle),
		cos(this->verticalAngle) * cos(this->horizontalAngle)
		);

	// Right vector
	this->right = glm::vec3(
		sin(this->horizontalAngle - 3.14f / 2.0f),
		0,
		cos(this->horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector - cross produktem dostanu kolm?vektor na tyto dva
	this->up = glm::cross(right, direction);

	// Camera matrix
	this->ViewMatrix = glm::lookAt(
		this->position,
		this->position + this->direction /*- this->position*/,
		this->up
		);

#ifdef DEBUG
	std::cout << "Light POSITION vector: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	//std::cout << "Light UP vector: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
	//std::cout << "Light RIGHT vector: (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
	std::cout << "Light DIRECTION vector: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
	std::cout << "Light horizotnal angle: " << horizontalAngle << std::endl;
	std::cout << "Light vertical angle: " << verticalAngle << std::endl;
#endif
}

glm::vec3 CLightObject::getPosition() {
	return this->position;
}

void CLightObject::setPosition(glm::vec3 p) {
	this->position = p;
}

glm::vec3 CLightObject::getDirection() {
	return this->direction;
}
glm::vec3 CLightObject::getRight() {
	return this->right;
}


void CLightObject::setDirection(glm::vec3 dir) {
	this->direction = dir;
}

void CLightObject::setUp(glm::vec3 up) {
	this->up = up;
}

glm::mat4 CLightObject::getProjMatrix() {
	return this->ProjectionMatrix;
}
glm::mat4 CLightObject::getViewMatrix() {
	return this->ViewMatrix;
}

void CLightObject::setHorAngle(float ha) {
	this->horizontalAngle = ha;
}
void CLightObject::setVerAngle(float va) {
	this->verticalAngle = va;
}

float CLightObject::getHorAngle() {
	return this->horizontalAngle;
}

float CLightObject::getVerAngle() {
	return this->verticalAngle;
}

float CLightObject::getAspectRatio(){
	return this->aspect;
}
float CLightObject::getFov() {
	return this->fov;
}

void CLightObject::visualize() {

}

glm::vec3 CLightObject::getColor() {
    return color;
}
