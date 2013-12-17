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

/**
* @file zhString.h
* @brief Helper macros and functions for working with strings.
*/

#ifndef __zhString_h__
#define __zhString_h__

#include "zhPrereq.h"

// C++
#include <string>
#include <sstream>

namespace zh
{

/**
* Converts a specific type to a string using ostringstream.
*/
template <typename T>
std::string toString( const T& t )
{
	std::ostringstream oss;
	oss << t;
	return oss.str();
}

/**
* Parses a specific type from string using istringstream.
*/
template <typename T>
T fromString( const std::string& tstr )
{
	T t;
	std::istringstream iss(tstr);

	iss >> t;

	return t;
}

}

#endif // __zhString_h__
