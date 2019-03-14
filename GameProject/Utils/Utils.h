#pragma once

#include <utility>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct Vertex;
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

	static glm::quat rotate(glm::quat& q, float yaw, float pitch, float roll);

	/*
	Converting from Euler angles to quaternion.
	Atrugments:
		yaw: Angle in radians of a rotation around the y-axis.
		pitch: Angle in radians of a rotation around the x-axis.
		roll: Angle in radians of a rotation around the z-axis.
	*/
	static glm::quat toQuaternion(float yaw, float pitch, float roll);

	/*
	Map one interval to another. Eg. [-100, 20.4] -> [0, 1]
	This will do a linear interpolation when mapping to the new interval.
	Arguments:
		min:	Old interval minimum.
		max:	Old interval maximum.
		x:		The value to map from the old interval to the new.
		newMin: New interval minimum.
		newMax: New interval maximum.
	Return:
		A value within the new interval.
	*/
	static float map(float min, float max, float x, float newMin, float newMax);

	struct AABB
	{
		glm::vec3 size;
		glm::vec3 pos;
	};

	/*
	Create an AABB from a set of vertices. The user can specify the axis of which the bounding box will use.
	Arguments:
		vertices:		The vertices used when computing.
		numVertices:	The number of vertices used.
		e1:				The x-axis.
		e2:				The y-axis.
		e3:				The z-axis.
	*/
	static AABB getAABB(Vertex* vertices, unsigned int numVertices, glm::vec3 e1 = { 1.f, 0.f, 0.f }, glm::vec3 e2 = { 0.f, 1.f, 0.f }, glm::vec3 e3 = { 0.f, 0.f, 1.f });
	static AABB getAABB(std::vector<glm::vec3> points, glm::vec3 e1 = { 1.f, 0.f, 0.f }, glm::vec3 e2 = { 0.f, 1.f, 0.f }, glm::vec3 e3 = { 0.f, 0.f, 1.f });
	
	struct OBB
	{
		glm::vec3 size;
		glm::vec3 pos;
		glm::quat rot;
	};

	/*
	Create a OBB from a set of vertices.
	Arguments:
		ptr:			A pointer to the model.
		vertices:		The vertices used when computing.
		numVertices:	The number of vertices used.
	*/
	static OBB getOBB(Vertex* vertices, unsigned int numVertices);
};