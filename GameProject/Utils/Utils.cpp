#include "Utils.h"

#include "glm/gtc/constants.hpp"
#include <cmath>

Utils::CubicResult Utils::solveCubic(float a, float b, float c)
{
	CubicResult result;
	// Code from icy1: http://www.cplusplus.com/forum/beginner/234717/
	double q = (3.*b - a * a) / 9.;
	double r = (-27.*c + a * (9.*b - 2.*a*a)) / 54.;
	double disc = q * q*q + r * r;
	double r13;
	double term1 = a / 3.;

	if (disc > 0.00000001) // One root real, two are complex.
	{
		double sqDisc = sqrt(disc);
		double s = r + sqDisc;
		s = s < 0. ? -cbrt(-s) : cbrt(s);
		double t = r - sqDisc;
		t = t < 0. ? -cbrt(-t) : cbrt(t);
		result.x1_real = -term1 + s + t;
		term1 += (s + t) / 2.f;
		result.x3_real = result.x2_real = -term1;
		term1 = sqrt(3.)*((s - t) / 2.);
		result.x2_imag = term1;
		result.x3_imag = -term1;
	}
	else if (disc >= 0.0 && disc <= 0.00000001) // All roots real, at least two are equal.
	{
		result.x3_imag = result.x2_imag = 0.;
		r13 = r < 0. ? -cbrt(-r) : cbrt(r);
		result.x1_real = -term1 + 2.*r13;
		result.x2_real = result.x3_real = -(r13 + term1);
		result.towEqual = true;
	}
	else // Only option left is that all roots are real and unequal (to get here, q < 0)
	{
		result.x3_imag = result.x2_imag = 0;
		q = -q;
		float dum1 = q * q*q;
		dum1 = acos(r / sqrt(dum1));
		r13 = 2.0*sqrt(q);
		result.x1_real = -term1 + r13 * cos(dum1 / 3.0);
		result.x2_real = -term1 + r13 * cos((dum1 + 2.0*glm::pi<double>()) / 3.0);
		result.x3_real = -term1 + r13 * cos((dum1 + 4.0*glm::pi<double>()) / 3.0);
	}
	return result;
}

std::vector<double> Utils::solveSystem(std::vector<std::vector<double>> A, bool& success)
{
	// https://martin-thoma.com/solving-linear-equations-with-gaussian-elimination/
	int n = A.size();
	success = true;

	for (int i = 0; i < n; i++) {
		// Search for maximum in this column
		double maxEl = abs(A[i][i]);
		int maxRow = i;
		for (int k = i + 1; k < n; k++) {
			if (abs(A[k][i]) > maxEl) {
				maxEl = abs(A[k][i]);
				maxRow = k;
			}
		}

		// Swap maximum row with current row (column by column)
		for (int k = i; k < n + 1; k++) {
			double tmp = A[maxRow][k];
			A[maxRow][k] = A[i][k];
			A[i][k] = tmp;
		}
		// Make all rows below this one 0 in current column
		for (int k = i + 1; k < n; k++) {
			double a = A[i][i];
			if (a >= -0.00000001 && a <= 0.00000001) success = false;
			double c = -A[k][i] / a;
			for (int j = i; j < n + 1; j++) {
				if (i == j) {
					A[k][j] = 0;
				}
				else {
					A[k][j] += c * A[i][j];
				}
			}
		}
	}

	// Solve equation Ax=b for an upper triangular matrix A
	std::vector<double> x(n);
	for (int i = n - 1; i >= 0; i--) {
		double a = A[i][i];
		if (a >= -0.00000001 && a <= 0.00000001) success = false;
		x[i] = A[i][n] / a;
		for (int k = i - 1; k >= 0; k--) {
			A[k][n] -= A[k][i] * x[i];
		}
	}

	return x;
}

glm::vec3 Utils::calcEigenvector(float eigenValue, const glm::mat3 & m)
{
	std::vector<double> x(2);

	glm::mat3 mat = m - eigenValue*glm::mat3(1.f);

	bool success = false;
	int index = 0;
	while (!success && index <= 2)
	{
		std::vector<double> line(3);
		std::vector<std::vector<double>> A(2, line);
		for (unsigned int i = 0; i < 2; i++)
			for (unsigned int j = 0; j < 2; j++)
			{
				if (index == 0) // X = 1, Will miss one col.
					A[i][j] = mat[i][j + 1];
				else if (index == 1) // Y = 1, Will miss one col.
					A[i][j] = mat[i][(j == 0) ? 0 : 2];
				else if (index == 2) // Y = 1, Will miss one col.
					A[i][j] = mat[i][j];
			}

		for (unsigned int i = 0; i < 2; i++)
			A[i][2] = -mat[i][index];
		x = Utils::solveSystem(A, success);

		if (success == false) index++;
	}

	if (success == false) // All equations is a multiple of another. Set X = 1, Y = 1 and solve for Z. This will result in two eigenvectors. => two eigenvalues are the same.
	{
		// TODO: Make this.
	}

	if(index == 0)
		return glm::vec3(1.f, x[0], x[1]);
	else if (index == 1)
		return glm::vec3(x[0], 1.f, x[1]);
	return glm::vec3(x[0], x[1], 1.f);
}
