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
	static CubicResult solveCubic(float a, float b, float c);

	static std::vector<double> solveSystem(std::vector< std::vector<double>> A, bool& success);

	static glm::vec3 calcEigenvector(float eigenValue, const glm::mat3& mat);

};