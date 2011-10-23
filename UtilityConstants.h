/*
   Constants related to math etc.
   Some of these are of course in other header files.

   by Michael Welsman-Dinelle, April 2009
*/

#pragma once

#include <cstdio>

#include <GL/glut.h>
#include "Point2D.h"

const static double M_PI   =      3.14159265358979323846;
const static double M_PI_2 =      1.57079632679489661923;
const static double M_PI_4 =      0.785398163397448309616;

const static double EPSILON =     0.00000000001;

const static double DEG_PER_RAD = 57.29578f;

void reportError();
void clearError();