#pragma once

#include <utility>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct Utils
{
	/*
	With the Jacobi's algorithm, the eigenvalues and eigenvectors are computed iteratively for a real symmetric matrix.
	Arguments:
		mat: The real symmetric matrix which the eigenvalues and eigenvectors will be calculated from.
		maxIterations: The max number of iterations.
	Return:
		first: Vector with eigenvalues.
		second: Matrix with eigenvectors as its columns. Where each column corresponds to an element in the vector of eigenvalues.
	*/
	static std::pair<glm::vec3, glm::mat3> jacobiMethod(const glm::mat3& mat, unsigned int maxIterations = 100);

	/*
	Return a quaternion which describes the rotation from vector v1 to v2.
	*/
	static glm::quat rotateTo(const glm::vec3& v1, const glm::vec3& v2);

	/*
	Converting from Euler angles to quaternion.
	Atrugments:
		yaw: Angle in radians of a rotation around the y-axis. 
		pitch: Angle in radians of a rotation around the x-axis.
		roll: Angle in radians of a rotation around the z-axis.
	*/
	static glm::quat toQuaternion(float yaw, float pitch, float roll);
};