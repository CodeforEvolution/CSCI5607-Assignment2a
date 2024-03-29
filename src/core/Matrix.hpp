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

public:
	Matrix()
		:
		fArray()
	{
		Reset();
	}

	constexpr void
	ApplyTransforms()
	{
		Identity();

		// Apply Scaling
		doScaleX(fScaleX);
		doScaleY(fScaleY);

		// Apply Rotation
		doRotate2D(fRotation);

		// Apply Translation
		doTranslateX(fTranslateX);
		doTranslateY(fTranslateY);
	}

	// Access like raw array
	constexpr operator const T*() const { return fArray.data(); }

	// Unchecked Access
	constexpr T& operator[](size_t index) { return fArray[index]; }

	constexpr const T& operator[](size_t index) const { return fArray[index]; }

	constexpr T& operator()(size_t row, size_t column) { return fArray[rowAndColToIndex(row, column)]; }

	constexpr const T& operator()(size_t row, size_t column) const { return fArray[rowAndColToIndex(row, column)]; }

	// Checked Access
	constexpr std::optional<T &>
	At(size_t row, size_t column)
	{
		if (row > Rows || column > Columns)
			return {};

		return fArray[rowAndColToIndex(row, column)];
	}

	constexpr std::optional<T &>
	At(size_t index)
	{
		if (index >= Size)
			return {};

		return fArray[index];
	}

	/** Identity Matrix */
	constexpr void
	Identity()
	{
		fArray.fill(0);
		for (size_t index = 0; index < Size; index += (Dimensions + 1))
			fArray[index] = 1;
	}


	constexpr void
	Reset()
	{
		fScaleX = 1.f;
		fScaleY = 1.f;
		fTranslateX = 0.f;
		fTranslateY = 0.f;
		fRotation = 0.f;

		Identity();
	}

	/** Scale */

	constexpr void
	ScaleUniformBy(const T& factor)
	{
		fScaleX += factor;
		fScaleY += factor;

		ApplyTransforms();
	}

	constexpr void
	ScaleXBy(const T& factor)
	{
		fScaleX += factor;
		ApplyTransforms();
	}

	constexpr void
	ScaleYBy(const T& factor)
	requires(Dimensions >= 2)
	{
		fScaleY += factor;
		ApplyTransforms();
	}

//	void
//	ScaleZBy(const T &factor)
//	requires(Dimensions >= 3)
//	{
//		Matrix<T, Dimensions> scaleMatrix;
//		scaleMatrix.Reset();
//		scaleMatrix[(Dimensions * 2) + 2] = factor;
//
//		this->MultiplyBy(scaleMatrix);
//	}


	/** Rotate */
	void
	Rotate2DBy(const T& radians)
	requires(Dimensions >= 2)
	{
		fRotation += radians;
		ApplyTransforms();
	}

	/** Translate */
	void
	TranslateUniformBy(const T& offset)
	{
		fTranslateX += offset;
		fTranslateY += offset;
		ApplyTransforms();
	}

	void
	TranslateXBy(const T& offset)
	requires(Dimensions >= 3)
	{
		fTranslateX += offset;
		ApplyTransforms();
	}

	void
	TranslateYBy(const T& offset)
	requires(Dimensions >= 3)
	{
		fTranslateY += offset;
		ApplyTransforms();
	}

//	void
//	TranslateZBy(const T &factor) requires(Dimensions >= 3) {
//		Matrix<T, Dimensions> translateMatrix;
//		translateMatrix.Reset();
//		translateMatrix[kTranslateRowBegin + 2] = factor;
//
//		this->MultiplyBy(translateMatrix);
//	}


	// Matrix Multiplication

	void
	MultiplyBy(const std::array<T, Size>& other)
	{
		Matrix<T, Dimensions> matrixC;
		for (size_t row = 0; row < Dimensions; row++) {
			for (size_t column = 0; column < Dimensions; column++) {
				matrixC(row, column) = 0;
				for (size_t calcOffset = 0; calcOffset < Dimensions; calcOffset++) {
					matrixC(row, column) += (this->operator()(row, calcOffset) *
											 other[rowAndColToIndex(calcOffset, column)]);
				}
			}
		}

		fArray = matrixC.fArray;
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
					matrixC(row, column) += (this->operator()(row, calcOffset) *
											 other[rowAndColToIndex(calcOffset, column)]);
				}
			}
		}

		fArray = matrixC.fArray;
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
					matrixC(row, column) += (this->operator()(row, calcOffset) *
											 other[rowAndColToIndex(calcOffset, column)]);
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
		fArray[0] *= other.dx;
		fArray[1] *= other.dy;
		fArray[2] *= other.dz;

		fArray[Dimensions] *= other.dx;
		fArray[Dimensions + 1] *= other.dy;
		fArray[Dimensions + 2] *= other.dz;

		fArray[(Dimensions * 2)] *= other.dx;
		fArray[(Dimensions * 2) + 1] *= other.dy;
		fArray[(Dimensions * 2) + 2] *= other.dz;

		return *this;
	}

private:
	constexpr size_t rowAndColToIndex(size_t row, size_t column) { return (row * Columns) + column; }

	void
	doScaleX(T factor)
	{
		if (factor == 0)
			return;

		Matrix<T, Dimensions> scaleMatrix;
		scaleMatrix[0] = factor;

		this->MultiplyBy(scaleMatrix);
	}

	void
	doScaleY(T factor)
	{
		if (factor == 0)
			return;

		Matrix<T, Dimensions> scaleMatrix;
		scaleMatrix[Dimensions + 1] = factor;

		this->MultiplyBy(scaleMatrix);
	}


	static constexpr const size_t kTranslateX = 12;

	void
	doTranslateX(T offset)
	{
		if (offset == 0)
			return;

		Matrix<T, Dimensions> translateMatrix;
		translateMatrix[kTranslateX] = offset;

		this->MultiplyBy(translateMatrix);
	}


	static constexpr const size_t kTranslateY = 13;

	void
	doTranslateY(T offset)
	{
		if (offset == 0)
			return;

		Matrix<T, Dimensions> translateMatrix;
		translateMatrix[kTranslateY] = offset;

		this->MultiplyBy(translateMatrix);
	}

	void
	doRotate2D(T radians)
	{
		if (radians == 0)
			return;

		double cosRads = std::cos(radians);
		double sinRads = std::sin(radians);

		// Rotate around Z-axis
		std::array<T, Size> rotationArray = {
			cosRads,	-1 * sinRads,	0,	0,
			sinRads,	cosRads,		0,	0,
			0,			0,				1,	0,
			0,			0,				0,	1
		};

		this->MultiplyBy(rotationArray);
	}

private:
	// Underlying Array
	std::array<T, Size> fArray;

	float fScaleX = 1.f;
	float fScaleY = 1.f;
	float fTranslateX = 0.f;
	float fTranslateY = 0.f;
	float fRotation = 0.f;
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
