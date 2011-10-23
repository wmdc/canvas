// Note that all of these methods are for OpenGL style (column-major) matrices.

#pragma once

#include "precompiled.h"

#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.1415926535897932385f
#endif

inline float deg2rad(float x)
{
	return x * M_PI / 180.0f;
}

inline float rad2deg(float x)
{
	return x * (180.0f / M_PI);
}


inline void multMatrixColumnMajor(float result[16], float m[16], float n[16])
{
	result[0]  = m[0]*n[0] + m[4]*n[1] + m[8]*n[2] + m[12]*n[3];
	result[1]  = m[1]*n[0] + m[5]*n[1] + m[9]*n[2] + m[13]*n[3];
	result[2]  = m[2]*n[0] + m[6]*n[1] + m[10]*n[2] + m[14]*n[3];
	result[3]  = m[3]*n[0] + m[7]*n[1] + m[11]*n[2] + m[15]*n[3];

	result[4]  = m[0]*n[4] + m[4]*n[5] + m[8]*n[6] + m[12]*n[7];
	result[5]  = m[1]*n[4] + m[5]*n[5] + m[9]*n[6] + m[13]*n[7];
	result[6]  = m[2]*n[4] + m[6]*n[5] + m[10]*n[6] + m[14]*n[7];
	result[7]  = m[3]*n[4] + m[7]*n[5] + m[11]*n[6] + m[15]*n[7];

	result[8]  = m[0]*n[8] + m[4]*n[9] + m[8]*n[10] + m[12]*n[11];
	result[9]  = m[1]*n[8] + m[5]*n[9] + m[9]*n[10] + m[13]*n[11];
	result[10] = m[2]*n[8] + m[6]*n[9] + m[10]*n[10] + m[14]*n[11];
	result[11] = m[3]*n[8] + m[7]*n[9] + m[11]*n[10] + m[15]*n[11];

	result[12] = m[0]*n[12] + m[4]*n[13] + m[8]*n[14] + m[12]*n[15];
	result[13] = m[1]*n[12] + m[5]*n[13] + m[9]*n[14] + m[13]*n[15];
	result[14] = m[2]*n[12] + m[6]*n[13] + m[10]*n[14] + m[14]*n[15];
	result[15] = m[3]*n[12] + m[7]*n[13] + m[11]*n[14] + m[15]*n[15];
}

/*
 * Convert x * y * z euler angles to a 4x4 rotation matrix (row major).
 * You can derive this by creating x, y, z rotation matrices and multiplying
 * them together.
 *
 * m: an array of size 16 for output to be assigned to.
 *
 * See: http://web.archive.org/web/20041029003853/http:/www.j3d.org/matrix_faq/matrfaq_latest.html#Q36
 */
inline void eulerTo4x4ColumnMajor(float m[16], float x, float y, float z)
{
	float a = cos(x);
    float b = sin(x);
    float c = cos(y);
    float d = sin(y);
    float e = cos(z);
    float f = sin(z);
    float ad =   a * d;
    float bd =   b * d;

    m[0]  =   c * e;
    m[4]  =  -c * f;
    m[8]  =   d;
	m[12]  =  0;
    m[1]  =  bd * e + a * f;
    m[5]  = -bd * f + a * e;
    m[9]  =  -b * c;
	m[13] = 0;
    m[2]  = -ad * e + b * f;
    m[6]  =  ad * f + b * e;
    m[10] =   a * c;
	m[14] = 0;
	m[3] = 0;
	m[7] = 0;
	m[11] = 0;
    m[15] =  1;
}

//http://www.gregslabaugh.name/publications/euler.pdf
inline void columnMajorMatrixToEuler(float *m, float *x, float *y, float *z)
{
	if(m[2] != 1 && m[2] != -1)
	{

		*y = -asin(m[2]); //one of two possible solutions -> other is y + M_PI

		float cosy = cos(*y);
		*x = atan2(m[6]/cosy, m[10]/cosy);
		*z = atan2(m[1]/cosy, m[0]/cosy);
	}
	else
	{
		*z = 0; //anything is valid -- this is "gimbal lock"
		if(m[2] == -1)
		{
			*y = M_PI/2;
			*x = *z + atan2(m[4], m[8]);
		}
		else
		{
			*y = -M_PI/2;
			*x = *z + atan2(-m[4], -m[8]);
		}
	}


}

void eulerLeftMultiplyByColumnMajorMatrix(float m[16], float *x, float *y, float *z)
{
	float mc[16];
	for( int i = 0; i < 16; i++ )
	{
		mc[i] = m[i];
	}

	float m2[16];
	eulerTo4x4ColumnMajor(m2, *x, *y, *z);

	multMatrixColumnMajor(m, mc, m2);

	columnMajorMatrixToEuler(m, x, y, z);
}

void rotateEuler(float angle, float axis1, float axis2, float axis3, float *z, float *y, float *x)
{
	//Convert to rotation matrix representation
	//to apply the rotation.
	float m[16];
		
	glPushMatrix();
	{
	    glLoadIdentity();

		glRotatef(angle + 180, axis1, axis2, axis3);

		glRotatef(*z, 0.0f, 0.0f, 1.0f);
		glRotatef(*y, 0.0f, 1.0f, 0.0f);
		glRotatef(*x, 1.0f, 0.0f, 0.0f);

		glGetFloatv(GL_MODELVIEW_MATRIX, m);
	}
	glPopMatrix();

	columnMajorMatrixToEuler(m, x, y, z);

	*z = rad2deg(*z);
	*y = rad2deg(*y);
	*x = rad2deg(*x);
}

const int N_ORTHO_PASSES = 10;

inline void cross3d(float v1[3], float v2[3], float result[3])
{
	result[0] = v1[1]*v2[2] - v1[2]*v2[1];
	result[1] = v1[2]*v2[0] - v1[0]*v2[2];
	result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

// Spatial Keyframing, Igarashi 2007, p. 110
void applyOrtho(float *m)
{
	float u[3], v[3], w[3];

	cross3d(&m[4], &m[8], u); //y*z
	cross3d(&m[12], &m[0], v); //z*x
	cross3d(&m[0], &m[4], w); //x*y

	m[0] = (m[0] + u[0])/2; //new x -> avg x and u
	m[1] = (m[1] + u[1])/2;
	m[2] = (m[2] + u[2])/2;

	m[4] = (m[4] + v[0])/2; //new y -> avg y and v
	m[5] = (m[5] + v[1])/2;
	m[6] = (m[6] + v[2])/2;

	m[8] = (m[8] + w[0])/2; //new z -> avg z and w
	m[9] = (m[9] + w[1])/2;
	m[10] = (m[10] + w[2])/2;
}

void makeOrtho(float *m)
{
	for(int i = 0; i < N_ORTHO_PASSES; i++)
	{
		applyOrtho(m);
	}
}