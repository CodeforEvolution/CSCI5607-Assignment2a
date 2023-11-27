// Assignment 2a - Programming Interactive 2D Graphics with OpenGL
// Work by Jacob Secunda
#ifndef HW2A_MATRIXUTILITIES_HPP
#define HW2A_MATRIXUTILITIES_HPP

#include "glad/glad.h"

#include <array>
#include <optional>

#include "Point.hpp"
#include "Vector3D.hpp"

//typedef GLfloat GLmatrix[16];
// 0	1	2	3	// X
// 4	5	6	7	// Y
// 8	9	10	11	// Z
// 12	13	14	15	// Translation

// Numbering starts from 0!

template<typename T, size_t Dimensions>
requires(Dimensions > 0)
struct Matrix {
	// Constants
	static constexpr size_t Rows = Dimensions;
	static constexpr size_t Columns = Dimensions;
	static constexpr size_t Size = Rows * Columns;

	// Underlying Array
	std::array<T, Size> m;

public:
	// Access like raw array
	operator const T*() const { return m.data(); }

	// Unchecked Access
	constexpr T& operator[](size_t index) { return m[index]; }
	constexpr const T& operator[](size_t index) const { return m[index]; }

	constexpr T&
	operator()(size_t row, size_t column)
	{
		return m[rowAndColToIndex(row, column)];
	}

	constexpr const T&
	operator()(size_t row, size_t column) const
	{
		return m[rowAndColToIndex(row, column)];
	}

	// Checked Access
	constexpr std::optional<T&>
	At(size_t row, size_t column)
	{
		if (row > Rows || column > Columns)
			return {};

		return m[rowAndColToIndex(row, column)];
	}

	constexpr std::optional<T&>
	At(size_t index)
	{
		if (index >= Size)
			return {};

		return m[index];
	}

	void
	Reset()
	{
		m.fill(0);
		for (size_t index = 0; index < Size; index += (Dimensions + 1))
			m[index] = 1;
	}

	/** Scale */

	void
	ScaleUniformBy(const T& factor)
	{
		for (size_t index = 0; index < Size; index += (Dimensions + 1))
			m[index] *= factor;
	}

	void
	ScaleXBy(const T& factor)
	{
		Matrix<T, Dimensions> scaleMatrix;
		scaleMatrix.Reset();
		scaleMatrix[0] = factor;

		this->MultiplyBy(scaleMatrix);
	}

	void
	ScaleYBy(const T& factor)
	requires(Dimensions >= 2)
	{
		Matrix<T, Dimensions> scaleMatrix;
		scaleMatrix.Reset();
		scaleMatrix[Dimensions + 1] = factor;

		this->MultiplyBy(scaleMatrix);
	}

	void
	ScaleZBy(const T& factor)
	requires(Dimensions >= 3)
	{
		Matrix<T, Dimensions> scaleMatrix;
		scaleMatrix.Reset();
		scaleMatrix[(Dimensions * 2) + 2] = factor;

		this->MultiplyBy(scaleMatrix);
	}


	/** Rotate */
	void
	Rotate2DBy(const T& radians)
	requires(Dimensions >= 2)
	{
		if (radians == 0)
			return;

		T cosRads = std::cos(radians);
		T sinRads = std::sin(radians);

		// Rotate around Z-axis
		Matrix<T, 4> rotationMatrix = {
			cosRads,	-1 * sinRads,	0,	0,
			sinRads,	cosRads,		0,	0,
			0,			0,				1,	0,
			0,			0,				0,	1
		};

		this->MultiplyBy(rotationMatrix);
	}

	/** Translate */
	void
	TranslateUniformBy(const T& factor)
	{
		for (size_t index = Dimensions - 1; index < Size; index += Dimensions)
			m[index] += factor;
	}

	static const size_t kTranslateRowBegin = Rows * (Columns - 1);

	void
	TranslateXBy(const T& factor)
	requires(Dimensions >= 3)
	{
		Matrix<T, Dimensions> translateMatrix;
		translateMatrix.Reset();
		translateMatrix[kTranslateRowBegin] = factor;

		this->MultiplyBy(translateMatrix);
	}

	void
	TranslateYBy(const T& factor)
	requires(Dimensions >= 3)
	{
		Matrix<T, Dimensions> translateMatrix;
		translateMatrix.Reset();
		translateMatrix[kTranslateRowBegin + 1] = factor;

		this->MultiplyBy(translateMatrix);
	}

	void
	TranslateZBy(const T& factor)
	requires(Dimensions >= 3)
	{
		Matrix<T, Dimensions> translateMatrix;
		translateMatrix.Reset();
		translateMatrix[kTranslateRowBegin + 2] = factor;

		this->MultiplyBy(translateMatrix);
	}


	// Matrix Multiplication

	void
	MultiplyBy(const std::array<T, Size>& other)
	{
		Matrix<T, Dimensions> matrixC;
		for (size_t row = 0; row < Dimensions; row++) {
			for (size_t column = 0; column < Dimensions; column++) {
				matrixC(row, column) = 0;
				for (size_t calcOffset = 0; calcOffset < Dimensions; calcOffset++) {
					matrixC(row, column) += (this->operator()(row, calcOffset) * other[rowAndColToIndex(calcOffset, column)]);
				}
			}
		}

		m = matrixC.m;
	}


	void
	MultiplyBy(const Matrix& other)
	requires(Rows == other.Columns)
	{
		Matrix<T, Dimensions> matrixC;
		for (size_t row = 0; row < Dimensions; row++) {
			for (size_t column = 0; column < Dimensions; column++) {
				matrixC(row, column) = 0;
				for (size_t calcOffset = 0; calcOffset < Dimensions; calcOffset++) {
					matrixC(row, column) += (this->operator()(row, calcOffset) * other[rowAndColToIndex(calcOffset, column)]);
				}
			}
		}

		m = matrixC.m;
	}


	Matrix&
	operator*=(const Matrix& other)
	requires(Columns == other.Rows)
	{
		Matrix<T, Dimensions> matrixC;
		for (size_t row = 0; row < Dimensions; row++) {
			for (size_t column = 0; column < Dimensions; column++) {
				matrixC(row, column) = 0;
				for (size_t calcOffset = 0; calcOffset < Dimensions; calcOffset++) {
					matrixC(row, column) += (this->operator()(row, calcOffset) * other[rowAndColToIndex(calcOffset, column)]);
				}
			}
		}

		return std::move(matrixC);
	}


	// When dealing with a 4x4 matrix, we assume w is implicitly 1
	// when multiplying against a 3-coordinate vector (x, y, z).
	Matrix&
	operator*=(const Vector3D<T>& other)
	requires(Dimensions == 3 || Dimensions == 4)
	{
		m[0] *= other.dx;
		m[1] *= other.dy;
		m[2] *= other.dz;

		m[Dimensions] *= other.dx;
		m[Dimensions + 1] *= other.dy;
		m[Dimensions + 2] *= other.dz;

		m[(Dimensions * 2)] *= other.dx;
		m[(Dimensions * 2) + 1] *= other.dy;
		m[(Dimensions * 2) + 2] *= other.dz;

		return *this;
	}

private:
	constexpr size_t rowAndColToIndex(size_t row, size_t column) { return (row * Columns) + column; }
};

/** Stream Operator */
template<typename T, size_t Dimensions>
static std::ostream&
operator<<(std::ostream& out, const Matrix<T, Dimensions>& matrix)
{
	for (size_t row = 0; row < matrix.Rows; row++) {
		out << "| ";
		for (size_t column = 0; column < matrix.Columns; column++) {
			out << matrix(row, column) << ' ';
		}
		out << "|\n";
	}

	return out;
}

// This only works with matrices of the same size for now...
template<typename T, size_t Dimensions>
static Matrix<T, Dimensions>
operator*(const Matrix<T, Dimensions>& matrixA, const Matrix<T, Dimensions>& matrixB)
{
	Matrix<T, Dimensions> matrixC;
	for (size_t row = 0; row < Dimensions; row++) {
		for (size_t column = 0; column < Dimensions; column++) {
			matrixC(row, column) = 0;
			for (size_t calcOffset = 0; calcOffset < Dimensions; calcOffset++) {
				matrixC(row, column) += (matrixA(row, calcOffset) * matrixB(calcOffset, column));
			}
		}
	}

	return matrixC;
}


/** Types */
template<typename T>
using Matrix2D = Matrix<T, 2>;

template<typename T>
using Matrix3D = Matrix<T, 3>;

template<typename T>
using Matrix4D = Matrix<T, 4>;

using GLmatrix = Matrix4D<float>;


#endif //HW2A_MATRIXUTILITIES_HPP
