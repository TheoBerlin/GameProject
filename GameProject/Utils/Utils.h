#pragma once

#include <utility>
#include "glm/glm.hpp"

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

};