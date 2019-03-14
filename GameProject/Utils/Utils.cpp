#include "Utils.h"

#include <cmath>
#include <Engine/Rendering/GLAbstraction/RenderingResources.h>
#include <glm/glm.hpp>

std::pair<glm::vec3, glm::mat3> Utils::jacobiMethod(const glm::mat3 & mat, unsigned int maxIterations)
{
	// Algorithm (Math) from Calculus++. Link to chapter 3: http://people.math.gatech.edu/~aleykin3/math2605fall10/CARLEN/chap3.pdf

	/*
	Find the absolute max off-diagonal element.
	Arguments:
		m: The matrix to search.
	Return:
		A vector where the x-component is the row index and the y-component is the column index.
	*/
	auto findMax = [](const glm::mat3 & m)->glm::ivec2 {
		glm::ivec2 index(0, 1);
		float max = fabs(m[index.y][index.x]);
		for (unsigned j = 0; j < 3; j++) {
			for (unsigned i = 0; i < 3; i++) {
				float c = fabs(m[j][i]);
				// Replace only if the max value is smaller the the current and if it is not a diagonal element.
				if (max < c && i != j) {
					index.x = i;
					index.y = j;
					max = fabs(m[index.y][index.x]);
				}
			}
		}
		return index;
	};

	/*
	Calculate the eigenvectors from a 2x2 matrix.
	Arguments:
		a: Top-left element.
		b: Top-right element.
		c: Bottom-left element.
		d: Bottom-right element.
	Return:
		A 2x2 matrix with its columns as its eignevectors.
	*/
	auto getEigenMat = [](float a, float b, float c, float d)->glm::mat2 {
		glm::mat2 result(0.f);

		float inside = b * b + ((a - d)*.5f)*((a - d)*.5f);
		float alpha = 0.f;
		// If 'inside' is 0 the eigenvalues are the same. 
		if (inside > 0.000001f)
			alpha = sqrt(inside);

		// If 'inside' is less than zero, then there is no solution to the problem.
		if (inside < 0.f)
			return result;

		float beta = (a + d)*.5f;

		// The first eignevector corresponding to eigenvalue beta+alpha.
		float vx = -b;
		float vy = a - beta - alpha;

		// Fix the sign, so that the top right element is the only negative element.
		if (vx < 0.f)
		{
			vx = -vx;
			vy = -vy;
		}

		// First eigenvector.
		result[0][0] = vx;
		result[0][1] = vy;

		// Second eigenvector.
		result[1][0] = -vy;
		result[1][1] = vx;

		// Normalize the vectors.
		result *= 1.f / sqrt(b*b + vy * vy);

		return result;
	};

	/*
	Get the 3x3 rotation matrix.
	The rotation will depend on the index in the lower triangle of the matrix and its angle will depend on the 2x2 matrix of eigenvectors.
	Arguments:
		m: A 2x2 matrix which its columns is its eigenvectors.
		i: Index of the row.
		j: Index of the column.
	Return:
		A 3x3 rotation matrix.
	*/
	auto G = [](const glm::mat2& m, unsigned i, unsigned j)->glm::mat3 {
		glm::mat3 result(1.f);
		if (i == 1 && j == 0) {
			result[0][0] = m[0][0];
			result[1][1] = m[1][1];
			result[1][0] = m[1][0];
			result[0][1] = m[0][1];
		}
		else if (i == 2 && j == 0) {
			result[0][0] = m[0][0];
			result[2][2] = m[1][1];
			result[2][0] = m[1][0];
			result[0][2] = m[0][1];
		}
		else {
			result[1][1] = m[0][0];
			result[2][2] = m[1][1];
			result[2][1] = m[1][0];
			result[1][2] = m[0][1];
		}
		return result;
	};

	glm::mat3 v(1.f);  // This will hold the eigenvectors.
	glm::mat3 e = mat; // This will hold the eigenvalues.

	unsigned int it = 0; // Iteration counter.
	const float epsilon = 0.00001f; // Threshold for when to quit.

	// Iterate if there are at least one off-diagonal element which has a absolute value greater then the threshold.
	// Stop iteration if all off-diagonal elements are close to zero or it has reached its max iteration requirement.
	glm::ivec2 index = findMax(e);
	float max = abs(e[index.y][index.x]);
	while (max > epsilon && it < maxIterations)
	{
		if (index.y >= index.x)
		{
			int tmp = index.x;
			index.x = index.y;
			index.y = tmp;
		}

		glm::mat2 smallEigen = getEigenMat(e[index.y][index.y], e[index.x][index.y], e[index.y][index.x], e[index.x][index.x]);

		// Calculate the rotation matrix.
		glm::mat3 r = G(smallEigen, index.x, index.y);

		// Rotate the accumulated matrix.
		e = glm::transpose(r)*e*r;

		// Save the rotation chain.
		v = v * r;

		// Get newest off-diagonal element.
		index = findMax(e);
		max = fabs(e[index.y][index.x]);

		it++;
	}
	return std::pair<glm::vec3, glm::mat3>(glm::vec3(e[0][0], e[1][1], e[2][2]), v);
}

glm::quat Utils::rotateTo(const glm::vec3 & v1, const glm::vec3 & v2)
{
	glm::quat q;
	glm::vec3 n = glm::normalize(glm::cross(v1, v2));
	float angle = acosf(glm::dot(v1, v2)) * .5f;
	q.x = n.x*sinf(angle);
	q.y = n.y*sinf(angle);
	q.z = n.z*sinf(angle);
	q.w = cosf(angle);
	return q;
}

glm::quat Utils::rotate(glm::quat & q, float yaw, float pitch, float roll)
{
	glm::quat rot = glm::quat({ yaw, pitch, roll });
	return rot * q;
}

glm::quat Utils::toQuaternion(float yaw, float pitch, float roll)
{
	float cy = cos(roll * .5f);
	float sy = sin(roll * .5f);
	float cp = cos(yaw * .5f);
	float sp = sin(yaw * .5f);
	float cr = cos(pitch * .5f);
	float sr = sin(pitch * .5f);

	glm::quat q;
	q.w = cy * cp * cr + sy * sp * sr;
	q.x = cy * cp * sr - sy * sp * cr;
	q.y = sy * cp * sr + cy * sp * cr;
	q.z = sy * cp * cr - cy * sp * sr;
	return q;
}

float Utils::map(float min, float max, float x, float newMin, float newMax)
{
	if (x < min) return newMin;
	if (x > max) return newMax;
	float t = (x - min) / (max - min);
	return newMin + t*(newMax-newMin);
}

Utils::AABB Utils::getAABB(Vertex * vertices, unsigned int numVertices, glm::vec3 e1, glm::vec3 e2, glm::vec3 e3)
{
	glm::vec3 min = glm::vec3(10000.0f);
	glm::vec3 max = glm::vec3(-10000.0f);
	for (unsigned int i = 0; i < numVertices; i++)
	{
		Vertex& vert = vertices[i];
		float e1dot = glm::dot(e1, vert.Position);
		float e2dot = glm::dot(e2, vert.Position);
		float e3dot = glm::dot(e3, vert.Position);
		min.x = glm::min(min.x, e1dot);
		min.y = glm::min(min.y, e2dot);
		min.z = glm::min(min.z, e3dot);

		max.x = glm::max(max.x, e1dot);
		max.y = glm::max(max.y, e2dot);
		max.z = glm::max(max.z, e3dot);
	}

	glm::vec3 pos = e1 * (min.x + (max.x - min.x)*.5f) + e2 * (min.y + (max.y - min.y)*.5f) + e3 * (min.z + (max.z - min.z)*.5f);

	return { glm::max(max - min, 0.01f), pos };
}

Utils::AABB Utils::getAABB(std::vector<glm::vec3> points, glm::vec3 e1, glm::vec3 e2, glm::vec3 e3)
{
	glm::vec3 min = glm::vec3(10000.0f);
	glm::vec3 max = glm::vec3(-10000.0f);
	for (unsigned int i = 0; i < points.size(); i++)
	{
		glm::vec3& p = points[i];
		float e1dot = glm::dot(e1, p);
		float e2dot = glm::dot(e2, p);
		float e3dot = glm::dot(e3, p);
		min.x = glm::min(min.x, e1dot);
		min.y = glm::min(min.y, e2dot);
		min.z = glm::min(min.z, e3dot);

		max.x = glm::max(max.x, e1dot);
		max.y = glm::max(max.y, e2dot);
		max.z = glm::max(max.z, e3dot);
	}

	glm::vec3 pos = e1 * (min.x + (max.x - min.x)*.5f) + e2 * (min.y + (max.y - min.y)*.5f) + e3 * (min.z + (max.z - min.z)*.5f);

	return { glm::max(max - min, 0.01f), pos };
}

Utils::OBB Utils::getOBB(Vertex * vertices, unsigned int numVertices)
{
	std::vector<Vertex> verts(vertices, vertices + numVertices);

	// Calculate centroid.
	glm::vec3 centroid(0.0f);
	for (Vertex& v : verts)
		centroid += v.Position;
	centroid /= (float)numVertices;

	// Distance from centroid to vertex.
	std::vector<glm::vec3> changeInPos;
	for (Vertex& v : verts)
		changeInPos.push_back(v.Position - centroid);

	/* Matrix multiplication for a single element.
		c[i][j] = (a*b)[i][j]
	Arguments:
		m1: matrix a
		m2: matrix b
		i: row index
		j: col index
	*/
	auto getElem = [](std::vector<glm::vec3>& m1, std::vector<glm::vec3>& m2, int i, int j)->float {
		float a = 0.0f;
		for (unsigned k = 0; k < m1.size(); k++)
			a += m1[k][i] * m2[k][j];
		return a;
	};

	// Calculate the covariance matrix. By multiplying the Nx3 matrix with its transpose from the left we get a 3x3 matrix which will be the covariance matrix after normalization. 
	glm::mat3 cov;
	for (unsigned i = 0; i < 3; i++)
		for (unsigned j = 0; j < 3; j++)
			cov[j][i] = getElem(changeInPos, changeInPos, i, j) / 3.f; // Multiply by its transpose and divide by the dimension size to normalize it.

	// Calculate the eigenvectors from the covariance matrix. This will also give us the eigenvalues as a byproduct but we will not use them.
	std::pair<glm::vec3, glm::mat3> jacobiResult = Utils::jacobiMethod(cov);

	glm::vec3 e1 = glm::normalize(jacobiResult.second[0]);
	glm::vec3 e2 = glm::normalize(jacobiResult.second[1]);
	glm::vec3 e3 = glm::normalize(jacobiResult.second[2]);

	// Get the AABB in the base [e1 e2 e3].
	AABB aabb = getAABB(vertices, numVertices, e1, e2, e3);

	// Rotate the x-axis to the first eigenvector.
	glm::vec3 vx = glm::vec3{ 1.f, 0.f, 0.f };
	glm::quat rotX = Utils::rotateTo(vx, e1);

	// If the first eigenvector is parallel to the x-axis, do not rotate.
	float d = glm::dot(vx, e1);
	if (abs(d) > 0.999f)
		rotX = glm::quat(1.f, 0.f, 0.f, 0.f);

	// Rotate the z-axis by the previous rotation and then rotate that vector to the second eigenvector.
	// This is equivalent to make a roll around the previously calculated axis to match the second and third eigenvector..
	glm::vec3 vz = glm::normalize(rotX * glm::vec3(0.f, 0.f, 1.f));
	glm::quat roll = Utils::rotateTo(vz, e3);

	// If the second eigenvector is parallel to the new z-axis, do not rotate.
	d = glm::dot(e3, vz);
	if (abs(d) > 0.999f)
		roll = glm::quat(1.f, 0.f, 0.f, 0.f);

	return { aabb.size, aabb.pos, roll * rotX };
}
