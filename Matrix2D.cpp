#include "Matrix2D.h"


template <typename T>
Matrix2D<T>::Matrix2D() {}

template <typename T>
Matrix2D<T>::Matrix2D(unsigned int rows, unsigned int cols) 
				: rows(rows), cols(cols), data(rows, std::vector<T>(cols)) { }

template <typename T>
Matrix2D<T>::Matrix2D(unsigned int rows, unsigned int cols, const T &def)
				: rows(rows), cols(cols), data(rows, std::vector<T>(cols, def)) { }

template <typename T>
inline T& Matrix2D<T>::operator()(unsigned int row, unsigned int col)
{
	return data[row][col];
}

template <typename T>
inline const T& Matrix2D<T>::operator()(unsigned int row, unsigned int col) const
{
	return data[row][col];
}

