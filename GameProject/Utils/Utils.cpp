#include "Utils.h"

#include "glm/gtc/constants.hpp"
#include <cmath>
#include <float.h>

Utils::CubicResult Utils::solveCubic(double a, double b, double c)
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
		result.twoEqual = true;
	}
	else // Only option left is that all roots are real and unequal (to get here, q < 0)
	{
		result.x3_imag = result.x2_imag = 0;
		q = -q;
		double dum1 = q * q*q;
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
	success = true;
	const double epsilon = 0.001;
	int n = A[0].size(); // Columns
	int m = A.size(); // Rows

	int h = 0; // Pivot row
	int k = 0; // Pivot column
	while (h < m && k < n)
	{
		// Find the k-th pivot
		int iMax = 0;
		for (int i = h; i < m; i++)
			if (abs(A[iMax][k]) < abs(A[i][k]))
				iMax = i;

		if (abs(A[iMax][k]) < epsilon)
		{
			// No pivot in this column, pass to next column.
			A[iMax][k] = 0.0;
			k++;
		}
		else
		{
			// Swap row h with iMax
			for (int j = 0; j < n; j++)
			{
				double tmp = A[h][j];
				A[h][j] = A[iMax][j];
				A[iMax][j] = tmp;
			}

			for (int i = h + 1; i < m; i++)
			{
				double f = A[i][k] / A[h][k];
				// Fill with zeros the lower part of pivot column
				A[i][k] = 0.0;
				// Do for all remaining elements in current row
				for (int j = k+1; j < n; j++)
				{
					A[i][j] -= A[h][j]*f;
				}
			}
			k++;
			h++;
		}
	}

	std::vector<double> x(m);
	for (int i = m - 1; i >= 0; i--)
	{
		double a = A[i][i];
		if (abs(a) <= epsilon) success = false;
		x[i] = A[i][m] / a;
		for (int k = i - 1; k >= 0; k--) {
			A[k][m - 1] -= A[k][i] * x[i];
		}

	}

	// Solve equation Ax=b for an upper triangular matrix A
	/*std::vector<double> x(m-1);
	for (int i = m - 1; i >= 0; i--) {
		double a = A[i][i];
		if (abs(a) <= epsilon) success = false;
		x[i] = A[i][m-1] / a;
		for (int k = i - 1; k >= 0; k--) {
			A[k][m-1] -= A[k][i] * x[i];
		}
	}*/

	return x;

	/*
	// https://martin-thoma.com/solving-linear-equations-with-gaussian-elimination/
	int n = A.size();
	success = true;

	const double epsilon = 0.0000001;

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
			if (abs(a) <= epsilon) success = false;
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
		if (abs(a) <= epsilon) success = false;
		x[i] = A[i][n] / a;
		for (int k = i - 1; k >= 0; k--) {
			A[k][n] -= A[k][i] * x[i];
		}
	}

	return x;*/
}

glm::dvec3 Utils::calcEigenvector(double eigenValue, const glm::dmat3 & m)
{
	const double epsilon = 0.000001;//DBL_EPSILON;
	glm::dmat3 mat = m - eigenValue*glm::dmat3(1.);
	
	/*
	// x = 1
	double e = mat[1][1];
	if (abs(e) > epsilon)
	{
		double alpha = mat[1][0] / e;
		double down = mat[2][0] - alpha * mat[2][1];
		if (abs(down) > epsilon)
		{
			double z = (alpha*mat[0][1] - mat[0][0]) / down;
			double y = (-mat[0][1] - mat[2][1] * z) / e;
			return glm::dvec3(1., y, z);
		}
	}

	// y = 1
	double d = mat[0][1];
	if (abs(d) > epsilon)
	{
		double alpha = mat[0][0] / d;
		double down = mat[2][0] - alpha * mat[2][1];
		if (abs(down) > epsilon)
		{
			double z = (alpha*e - mat[1][0]) / down;
			double x = (-e - mat[2][1] * z) / d;
			return glm::dvec3(x, 1., z);
		}
	}

	// z = 1
	if (abs(d) > epsilon)
	{
		double alpha = mat[0][0] / d;
		double down = mat[1][0] - alpha * e;
		if (abs(down) > epsilon)
		{
			double y = (alpha*mat[2][1] - mat[2][0]) / down;
			double x = (-mat[2][1] - e * y) / d;
			return glm::dvec3(x, y, 1.);
		}
	}

	return glm::dvec3(NAN, NAN, NAN);
	*/
	// All equations is a multiple of another. Set X = 1, Y = 1 and solve for Z. This will result in two eigenvectors. => two eigenvalues are the same.
	// This function will not calculate two eigenvectors.

	
	std::vector<double> x(2);

	bool success = false;
	int index = 0;
	while (!success && index <= 3)
	{
		std::vector<double> line(3);
		std::vector<std::vector<double>> A(3, line);
		for (unsigned int i = 0; i < 3; i++)
			for (unsigned int j = 0; j < 2; j++)
			{
				if (index == 0) // X = 1, Will miss one col.
					A[i][j] = mat[j + 1][i];
				else if (index == 1) // Y = 1, Will miss one col.
					A[i][j] = mat[(j == 0) ? 0 : 2][i];
				else if (index == 2) // Y = 1, Will miss one col.
					A[i][j] = mat[j][i];
			}

		for (unsigned int i = 0; i < 3; i++)
			A[i][2] = -mat[index][i];
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

std::pair<glm::mat3, glm::mat3> Utils::jacobiMethod(const glm::mat3 & mat, unsigned int maxIterations)
{
	// https://en.wikipedia.org/wiki/Jacobi_eigenvalue_algorithm
	// http://fourier.eng.hmc.edu/e176/lectures/NM/node13.html
	// Use the Jacobi´s method
	// http://fptchlx02.tu-graz.ac.at/cgi-bin/access.com?c1=0000&c2=0000&c3=0000&file=0638

	auto findMax = [](const glm::mat3 & m)->glm::ivec2 {
		// Find absolute max off-diagonal element.
		glm::ivec2 index(0, 0);
		float max = fabs(m[index.y][index.x]);
		for (unsigned j = 0; j < 3; j++) {
			for (unsigned i = 0; i < 3; i++) {
				float c = fabs(m[j][i]);
				if (max < c && i != j) {
					index.x = i;
					index.y = j;
					max = fabs(m[index.y][index.x]);
				}
			}
		}
		return index;
	};

	glm::mat3 v(1.f);  // This will hold the eigenvectors.
	glm::mat3 e = mat; // This will hold the eigenvalues.

	unsigned int it = 0;
	const float epsilon = 0.0000001f;

	// Iterate there are at least one off-diagonal element which has a absolute value greater then the threshold.
	// Stop iteration if all off-diagonal elements are close to zero or it has reached its max iteration requirement.
	glm::ivec2 index = findMax(e);
	float max = abs(e[index.y][index.x]);
	while (max > epsilon && it < maxIterations)
	{
		// Set the off-diagonal element to zero results in a scalar w.
		float w = (e[index.y][index.y] - e[index.x][index.x]) / (2.f*e[index.y][index.x]);

		// Calculate tan(angle)
		float t = 0.f;
		if (w >= 0.f)
			t = -w + sqrtf(w*w + 1.f);
		else
			t = -w - sqrtf(w*w + 1.f);

		// Calculate sin(angle) and cos(angle)
		float s = t / sqrtf(1.f + t * t);
		float c = 1.f / sqrt(1.f + t * t);
		
		float angle = .5f*atanf(2.f*e[index.y][index.x] / (e[index.y][index.y] - e[index.x][index.x]));
		s = sinf(angle);
		c = cosf(angle);
		
		// Calculate the rotation matrix.
		glm::mat3 r(1.f);
		r[index.x][index.x] = r[index.y][index.y] = c;
		r[index.y][index.x] = s;
		r[index.x][index.y] = -s;

		// Rotate the accumulated matrix.
		e = glm::transpose(r)*e*r;

		// Save the rotation chain.
		v = v * r;

		// Find the newest greatest absolute value and try to set it to zero with the next iteration.
		index = findMax(e);
		max = abs(e[index.y][index.x]);

		// Update iteration step.
		it++;
	}

	printf("it: %d\n", it);
	return std::pair<glm::mat3, glm::mat3>(e, v);
}

std::pair<glm::mat3, glm::mat3> Utils::jacobiMethod2(const glm::mat3 & mat, unsigned int maxIterations)
{
	auto findMax = [](const glm::mat3 & m)->glm::ivec2 {
		// Find absolute max off-diagonal element.
		glm::ivec2 index(0, 1);
		float max = fabs(m[index.y][index.x]);
		for (unsigned j = 0; j < 3; j++) {
			for (unsigned i = 0; i < 3; i++) {
				float c = fabs(m[j][i]);
				if (max < c && i != j) {
					index.x = i;
					index.y = j;
					max = fabs(m[index.y][index.x]);
				}
			}
		}
		return index;
	};

	// http://people.math.gatech.edu/~aleykin3/math2605fall10/CARLEN/chap3.pdf

	auto getEigenMat = [](float a, float b, float c, float d)->glm::mat2 {
		float inside = b * b + ((a - d)*.5f)*((a - d)*.5f);
		float alpha = 0.f;
		if(inside > 0.000001f)
			alpha = sqrt(inside);
		float beta = (a + d)*.5f;
		glm::mat2 result;

		float vx = -b;
		float vy = a - beta - alpha;

		if (vx < 0.f)
		{
			vx = -vx;
			vy = -vy;
		}

		result[0][0] = vx;
		result[0][1] = vy;
		result[1][0] = -vy;
		result[1][1] = vx;

		result *= 1.f / sqrt(b*b + vy*vy);

		return result;
	};

	auto G = [](const glm::mat2& m, unsigned i, unsigned j)->glm::mat3 {
		glm::mat3 result(1.f);
		if (i == 1 && j == 0) {
			result[0][0] = m[0][0];
			result[1][1] = m[1][1];
			result[1][0] = m[1][0];
			result[0][1] = m[0][1];
		} else if (i == 2 && j == 0) {
			result[0][0] = m[0][0];
			result[2][2] = m[1][1];
			result[2][0] = m[1][0];
			result[0][2] = m[0][1];
		} else {
			result[1][1] = m[0][0];
			result[2][2] = m[1][1];
			result[2][1] = m[1][0];
			result[1][2] = m[0][1];
		}
		return result;
	};

	glm::mat3 v(1.f);  // This will hold the eigenvectors.
	glm::mat3 e = mat; // This will hold the eigenvalues.

	unsigned int it = 0;
	const float epsilon = 0.000000001f;

	// Iterate there are at least one off-diagonal element which has a absolute value greater then the threshold.
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
		v = v*r;

		index = findMax(e);
		max = abs(e[index.y][index.x]);

		it++;
	}
	return std::pair<glm::mat3, glm::mat3>(e, v);
}

glm::vec3 Utils::powerMethod(const glm::mat3 & mat, unsigned int maxIterations)
{
	glm::vec3 b = glm::normalize(mat[2]); // Pick a random vector, wher b != 0.

	float diff = 0.f;
	const float epsilon = 0.9999999f;
	glm::mat3 a = mat;
	unsigned int it = 0;
	while (diff < epsilon && it < maxIterations)
	{
		glm::vec3 b2 = a*b;

		float norm = fmaxf(fmaxf(b2.x, b2.y), b2.z);
		b2 /= norm;

		diff = fabs(glm::dot(b, glm::normalize(b2)));

		b = glm::normalize(b2);
		it++;
	}

	return b;
}

glm::vec3 Utils::powerMethodInv(const glm::mat3 & mat, unsigned int maxIterations)
{
	glm::vec3 b = glm::normalize(mat[0]); // Pick a random vector, wher b != 0.

	float diff = 0.f;
	const float epsilon = 0.9999999f;
	glm::mat3 a = glm::inverse(mat);
	unsigned int it = 0;
	while (diff < epsilon && it < maxIterations)
	{
		glm::vec3 b2 = a * b;

		float norm = fmaxf(fmaxf(b2.x, b2.y), b2.z);
		b2 /= norm;

		diff = fabs(glm::dot(b, glm::normalize(b2)));

		b = glm::normalize(b2);
		it++;
	}

	return b;
}

void Utils::tred2(float ** a, int n, float d[], float e[])
{
	int l, k, j, i;
	float scale, hh, h, g, f;

	for (i = n; i >= 2; i--)
	{
		l = i - 1;
		h = scale = 0.f;
		if (l > 1)
		{
			for (k = 1; k <= l; k++)
				scale += fabs(a[i][k]);
			if (scale == 0.f)
				e[i] = a[i][l];
			else {
				for (k = 1; k <= l; k++) {
					a[i][k] /= scale;
					h += a[i][k] * a[i][k];
				}
				f = a[i][l];
				g = (f >= 0.f ? -sqrt(h) : sqrt(h));
				e[i] = scale * g;
				h -= f * g;
				a[i][l] = f - g;
				f = 0.f;
				for (j = 1; j <= l; j++) {
					a[j][i] = a[i][j] / h;
					g = 0.f;
					for (k = 1; k <= j; k++)
						g += a[j][k] * a[i][k];
					for (k = j + 1; k <= l; k++)
						g += a[k][j] * a[i][k];
					e[j] = g / h;
					f += e[j] * a[i][j];
				}
				hh = f / (h+h);
				for (j = 1; j <= l; j++) {
					f = a[i][j];
					e[j] = g = e[j] - hh * f;
					for (k = 1; k <= j; k++)
						a[j][k] -= (f*e[k] + g*a[i][k]);
				}
			}
		}
		else {
			e[i] = a[i][l];
		}
		d[i] = h;
	}
	d[1] = 0.f;
	e[1] = 0.f;
	for (i = 1; i <= n; i++)
	{
		l = i - 1;
		if (d[i]) {
			for (j = 1; j < l; j++) {
				g = 0.f;
				for (k = 1; k <= l; k++)
					g += a[i][k] * a[k][j];
				for (k = 1; k <= l; k++)
					a[k][j] -= g * a[k][i];
			}
		}
		d[i] = a[i][i];
		a[i][i] = 1.f;
		for (j = 1; j <= l; j++)
			a[j][i] = a[i][j] = 0.f;
	}
}
