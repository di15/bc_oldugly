

#ifndef MATRIX_H
#define MATRIX_H

class Quaternion;
class Vec4f;

class Matrix
{
	public:
		//	Matrix data, stored in column-major order
		float m_matrix[16];

		Matrix();
		Matrix(Vec4f a, Vec4f b, Vec4f c, Vec4f d);
		~Matrix();
		void loadIdentity();
		void set( const float *matrix );

		void postMultiply( const Matrix& matrix );

		//	Set the translation of the current matrix. Will erase any previous values.
		void setTranslation( const float *translation );

		//	Set the inverse translation of the current matrix. Will erase any previous values.
		void setInverseTranslation( const float *translation );

		void setScale( const float *scale );

		//	Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
		void setRotationRadians( const float *angles );

		//	Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
		void setRotationDegrees( const float *angles );

		//	Make a rotation matrix from a quaternion. The 4th row and column are unmodified.
		void setRotationQuaternion( const Quaternion& quat );

		//	Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
		void setInverseRotationRadians( const float *angles );

		//	Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
		void setInverseRotationDegrees( const float *angles );

		float* getComponent(int row, int col) { return &m_matrix[ row + col*4 ]; }

		//	Translate a vector by the inverse of the translation part of this matrix.
		void inverseTranslateVect( float *pVect );

		//	Rotate a vector by the inverse of the rotation part of this matrix.
		void inverseRotateVect( float *pVect );
};


void LogMat(const float* m);

#endif