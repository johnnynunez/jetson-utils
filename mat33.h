/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
 
#ifndef __MATRIX_33_H_
#define __MATRIX_33_H_


#include <stdio.h>
#include <unistd.h>


/**
 * Cast a 3x3 matrix from one type to another.
 */
template<typename T1, typename T2>
inline void mat33_cast( T1 dst[3][3], const T2 src[3][3] )
{
	for( uint32_t i=0; i < 3; i++ )
		for( uint32_t j=0; j < 3; j++ )
			dst[i][j] = (T1)src[i][j];
}


/**
 * Copy src input matrix to dst output.
 */
template<typename T>
inline void mat33_copy( T dst[3][3], const T src[3][3] )
{
	memcpy(dst, src, sizeof(T) * 9);
}


/**
 * Compute the determinant of a 3x3 matrix, returns |src|
 */
template <typename T>
inline T mat33_det( const T src[3][3] )
{
	return src[0][0] * (src[1][1] * src[2][2] - src[1][2] * src[2][1])
		- src[0][1] * (src[1][0] * src[2][2] - src[1][2] * src[2][0])
		+ src[0][2] * (src[1][0] * src[2][1] - src[1][1] * src[2][0]);
}


/**
 * Initialize a 3x3 identity matrix.
 */
template<typename T>
inline void mat33_identity( T dst[3][3] )
{
	dst[0][0] = 1; dst[0][1] = 0; dst[0][2] = 0;
	dst[1][0] = 0; dst[1][1] = 1; dst[1][2] = 0;
	dst[2][0] = 0; dst[2][1] = 0; dst[2][2] = 1;
}


/**
 * Compute the inverse of a 3x3 matrix, dst=src^-1
 * It is safe to have dst and src be the same memory.
 */
template<typename T>
inline void mat33_inverse( T dst[3][3], const T src[3][3] )
{
	T inv[3][3];

	// invert
	const T det = mat33_det(src);

	inv[0][0] = + (src[1][1] * src[2][2] - src[1][2] * src[2][1]);
	inv[0][1] = - (src[0][1] * src[2][2] - src[0][2] * src[2][1]);
	inv[0][2] = + (src[0][1] * src[1][2] - src[0][2] * src[1][1]);
	inv[1][0] = - (src[1][0] * src[2][2] - src[1][2] * src[2][0]);
	inv[1][1] = + (src[0][0] * src[2][2] - src[0][2] * src[2][0]);
	inv[1][2] = - (src[0][0] * src[1][2] - src[0][2] * src[1][0]);
	inv[2][0] = + (src[1][0] * src[2][1] - src[1][1] * src[2][0]);
	inv[2][1] = - (src[0][0] * src[2][1] - src[0][1] * src[2][0]);
	inv[2][2] = + (src[0][0] * src[1][1] - src[0][1] * src[1][0]);

	// scale by determinant
	for( uint32_t i=0; i < 3; i++ )
		for( uint32_t k=0; k < 3; k++ )
			dst[i][k] = inv[i][k] / det;
}


/**
 * Multiply two 3x3 matrices, dst=a*b
 */
template<typename T>
inline void mat33_mul( T dst[3][3], const T a[3][3], const T b[3][3] )
{
	for( int i=0; i < 3; i++ )
	{
		for( int j=0; j < 3; j++ )
		{
			dst[i][j] = 0;

			for( int k=0; k < 3; k++ )
				dst[i][j] = dst[i][j] + a[i][k] * b[k][j];
		}
	}
}


/**
 * Print out a 3x3 matrix to stdout.
 */
template<typename T>
inline void mat33_print( const T src[3][3], const char* name=NULL )
{
	if( name != NULL )
		printf("%s = \n", name);

	printf(" [ ");

	for( uint32_t i=0; i < 3; i++ )
	{
		for( uint32_t j=0; j < 3; j++ )
			printf("%f ", src[i][j]);

		if( i < 2 )
			printf("\n   ");
		else
			printf("]\n");
	}
}


/**
 * Determine the rank of a 3x3 matrix
 */
template<typename T>
inline int mat33_rank( const T src[3][3] )
{
	T mat[3][3];

	// reducing to row-echelon form alters the matrix
	mat33_copy(mat, src);

	// iteratively compute the rank
	int rank = 3;	// (= #columns)
  
	for( int row=0; row < rank; row++ ) 
	{ 
		if( mat[row][row] != 0 ) 
		{ 
			for( int col=0; col < 3; col++ ) 
			{ 
				if (col != row) 
				{ 
					const T mult = mat[col][row] / mat[row][row]; 

					for( int i = 0; i < rank; i++ ) 
						mat[col][i] -= mult * mat[row][i]; 
				} 
			} 
		} 
		else
		{ 
			bool reduce = true; 

			for( int i=row+1; i < 3; i++ ) 
			{ 
				if( mat[i][row] != 0 ) 
				{
					for( int k=0; k < rank; k++ )
					{
						const T tmp = mat[row][k];
						mat[row][k] = mat[i][k];
						mat[i][k] = tmp;
 					}

					reduce = false; 
					break ; 
				} 
			} 
  
			if( reduce ) 
			{ 
				rank--; 
  
				for( int i=0; i < 3; i++ ) 
					mat[i][row] = mat[i][rank]; 
			} 
  
			row--; 
		} 
	}

	return rank; 
}


/**
 * Swap two 3x3 matrices inline, a=b and b=a
 */
template<typename T>
inline void mat33_swap( T a[3][3], T b[3][3] )
{
	T c[3][3];

	mat33_copy(c, a);
	mat33_copy(a, b);
	mat33_copy(b, c);
}


/**
 * Compute the trace of a 3x3 matrix, returns tr(src)
 */
template<typename T>
inline T mat33_trace( const T src[3][3] )
{
	return src[0][0] + src[1][1] + src[2][2];
}


/**
 * Transform a 2D vector by a 3x3 matrix
 */
template<typename T>
inline void mat33_transform( T& x_out, T& y_out, const T x_in, const T y_in, const T mat[3][3] )
{
	const T x = mat[0][0] * x_in + mat[0][1] * y_in + mat[0][2];
	const T y = mat[1][0] * x_in + mat[1][1] * y_in + mat[1][2];
	const T z = mat[2][0] * x_in + mat[2][1] * y_in + mat[2][2];

	x_out = x; // / z;
	y_out = y; // / z;
}


/**
 * Transform a 2D vector by a 3x3 matrix, dst=src*mat
 */
template<typename T>
inline void mat33_transform( T dst[2], const T src[2], const T mat[3][3] )
{
	mat33_transform(dst[0], dst[1], src[0], src[1], mat);
}


/**
 * Transform an array of 2D vectors by a 3x3 matrix
 */
template<typename T>
inline void mat33_transform( T* dst, const T* src, const int N, const T mat[3][3] )
{
	for( uint32_t n=0; n < N; n++ )
		mat33_transform(dst[n*2], dst[n*2+1], src[n*2], src[n*2+1], mat); 
}

/**
 * Transpose a 3x3 matrix, dst=src^T
 */
template<typename T>
inline void mat33_transpose( T dst[3][3], const T src[3][3] )
{
	for( uint32_t i=0; i < 3; i++ )
		for( uint32_t j=0; j < 3; j++ )
			dst[i][j] = src[j][i];
}


/**
 * Set a 3x3 matrix to all zero's.
 */
template<typename T>
inline void mat33_zero( T dst[3][3] )
{
	memset(dst, 0, sizeof(T) * 9);
}

#endif
