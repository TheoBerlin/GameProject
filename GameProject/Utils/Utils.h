#pragma once

#include <vector>
#include "glm/glm.hpp"

struct Utils
{
	struct CubicResult
	{
		double x1_real = 0.0f;
		double x2_real = 0.0f;
		double x3_real = 0.0f;

		double x1_imag = 0.0f;
		double x2_imag = 0.0f;
		double x3_imag = 0.0f;

		bool twoEqual = false;
	};
	static CubicResult solveCubic(double a, double b, double c);

	static std::vector<double> solveSystem(std::vector< std::vector<double>> A, bool& success);

	static glm::dvec3 calcEigenvector(double eigenValue, const glm::dmat3& mat);

	/*
	With the Jacobi's algorithm, the eigenvalues and eigenvectors are computed iteratively of a real symmetric matrix.
	Arguments:
		mat: The real symmetric matrix which the eigenvalues and eigenvectors will be calculated from.
		maxIterations: The max number of iterations.
	Return:
		first: Matrix with its diagonal holding the eigenvalues.
		second: Matrix with eigenvectors as its columns.
	*/
	static std::pair<glm::mat3, glm::mat3> jacobiMethod(const glm::mat3& mat, unsigned int maxIterations = 100);
	static std::pair<glm::mat3, glm::mat3> jacobiMethod2(const glm::mat3& mat, unsigned int maxIterations = 100);

	static glm::vec3 powerMethod(const glm::mat3& mat, unsigned int maxIterations = 20);
	static glm::vec3 powerMethodInv(const glm::mat3& mat, unsigned int maxIterations = 20);

	static void tred2(float **a, int n, float d[], float e[]);

};