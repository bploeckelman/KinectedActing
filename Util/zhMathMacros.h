/******************************************************************************
Copyright (C) 2013 Tomislav Pejsa

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __zhMathMacros_h__
#define __zhMathMacros_h__

#include "zhPrereq.h"

namespace zh
{

#define zhEqualf( a, b ) ( ( fabs( ( a ) - ( b ) ) ) < ( 0.00001f ) ) // returns true if 2 float numbers are equal
#define zhEqualf_t( a, b, t ) ( ( fabs( ( a ) - ( b ) ) ) < ( t ) ) // returns true if 2 float numbers are equal with specified tolerance
#define zhRoundf(x) ( floor( (x) + 0.5f ) ) // rounds a float number
#define zhRoundi(x) ( int( (x) + 0.5f ) ) // rounds a float number and returns an integer
#define zhRoundi64(x) ( Int64( (x) + 0.5f ) ) // rounds a float number and returns a 64-bit integer
#define zhClamp01(x) ( (x) < 0 ? 0 : ( (x) > 1 ? 1 : (x) ) )
#define zhClamp(x,l,r) ( (x) < l ? l : ( (x) > r ? r : (x) ) )

#define zhE 2.71828182f
#define zhPI 3.14159265f
#define zh2PI 6.28318531f
#define zhPI2 1.57079633f
#define zhPI4 0.78539816f
#define zhDeg(x) ( (x) * 180.f / zhPI ) // converts radians to degrees
#define zhRad(x) ( (x) * zhPI / 180.f ) // converts degrees to radians

}

#endif // __zhMathMacros_h__
