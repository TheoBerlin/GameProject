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

	static glm::mat3 calcEigenVectorsFromSymmetricMat(const glm::mat3& mat);

	static void tred2(float **a, int n, float d[], float e[]);

};