#ifndef __MATRIX_2D_H__
#define __MATRIX_2D_H__

#include <vector>
template <typename T>
class Matrix2D
{
	public:
		Matrix2D();
		Matrix2D(unsigned int rows, unsigned int cols);
		Matrix2D(unsigned int rows, unsigned int cols, const T& def);
		
		T& operator()(unsigned int row, unsigned int col);
		const T& operator()(unsigned int row, unsigned int col) const;
		
		unsigned int rows;
		unsigned int cols;
		
	private:
		
		// 用vector实现的二维数组
		std::vector<std::vector<T>> data;
};

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

#endif  //__MATRIX_2D_H__
