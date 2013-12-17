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
* @file zhPrereq.h
* @brief Prerequisite header file, included by all other header files.
*/

#ifndef __zhPrereq_h__
#define __zhPrereq_h__

// version info
#define zhVersion_Major 0
#define zhVersion_Minor 0
#define zhVersion_Patch 0
#define zhVersion_Suffix ""

// compile-time configuration:
#define zhLog_Enabled 1
#define zhLogFile_Path "zh.log"
#define zhMultiThreading_Enabled 0
#define zhMemoryPool_ChunkSize 4096
#define zhMemoryPool_MaxObjSize 128
#define zhAnimationParam_SampleInterpK 10 // k-value used for kNN interpolation of parameter samples in param. animations
#define zhBlend_TWTimeStep 0.1f // maximum Euler step used in blending with timewarping
#define zhAnimation_SampleRate 30 // animation sample rate (in frames per second)
#define zhAnimationParam_SampleDensity 10 // dense sampling factor for parametric spaces (the total number of dense samples is this factor X number of base samples)
#define zhBlend_DegeneracyLimit 3 // maximum number of consecutive frames in one animation
// that may be blended with the same frame of another animation
#define zhDTW_KernelSize 15 // size of the kernel used for dynamic timewarping;
// greater kernel size yields a better time alignment at the expense of performance
#define zhARFSS_NumClusters 20//2000 // number of clusters for adaptive representative frame set selection (ARFSS)

// compilers
#define zhCompiler_MSVC 1

// identify compiler
#if defined(_MSC_VER)
	#define zhCompiler zhCompiler_MSVC
#else
	#pragma error "Unknown compiler. Compilation failed."
#endif

// platforms
#define zhPlatform_Win 1

// identify platform
#if defined(__WIN32__) || defined(_WIN32)
	#define zhPlatform zhPlatform_Win
#else
	#pragma error "Unknown platform. Compilation failed."
#endif

// architectures
#define zhArchType_32 1
#define zhArchType_64 2

// identify architecture type
#if defined(_M_X64) || defined(_M_AMD64) || defined(__ia64__) || defined(__x86_64__)
	#define zhArchType zhArchType_64
#else
	#define zhArchType zhArchType_32
#endif

// 64-bit int type
#if zhCompiler == zhCompiler_MSVC
	typedef __int64 Int64;
	typedef unsigned __int64 UInt64;
#else
	typedef long long Int64;
	typedef unsigned long long UInt64;
#endif

// suppress some annoying warnings
#if zhCompiler == zhCompiler_MSVC
	#pragma warning( disable : 4251 4267 )
#endif

#ifdef _DEBUG
	#include <assert.h>
	#define zhAssert assert
#else
	#define zhAssert(x)
#endif

#if zhPlatform == zhPlatform_Win
	#ifdef zhNonClientBuild
		//#define zhDeclSpec __declspec( dllexport )
	#else
		//#define zhDeclSpec __declspec( dllimport )
	#endif
#endif

#if zhMultiThreading_Enabled
	#define zhDeclare_Mutex boost::recursive_mutex mMtx;
	#define zhLock_Mutex mMtx.lock()
	#define zhTryLock_Mutex mMtx.try_lock()
	#define zhUnlock_Mutex mMtx.unlock()
	#define zhDeclare_SharedMutex boost::recursive_mutex* mMtx;
	#define zhCreate_SharedMutex mMtx = new boost::recursive_mutex()
	#define zhDelete_SharedMutex delete mMtx
	#define zhLock_SharedMutex(x) (x).mMtx->lock()
	#define zhUnlock_SharedMutex(x) (x).mMtx->unlock()
	#define zhLockCopy_SharedMutex(x) (x).mMtx->lock(); mMtx = (x).mMtx
#else
	#define zhDeclare_Mutex
	#define zhLock_Mutex
	#define zhTryLock_Mutex
	#define zhUnlock_Mutex
	#define zhDeclare_SharedMutex
	#define zhCreate_SharedMutex
	#define zhDelete_SharedMutex
	#define zhLock_SharedMutex(x)
	#define zhUnlock_SharedMutex(x)
	#define zhLockCopy_SharedMutex(x)
#endif

// C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#if zhCompiler == zhCompiler_MSVC
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif
#include <algorithm>

// Boost
#if zhMultiThreading_Enabled
#include <boost/thread.hpp>
#endif

#endif // __zhPrereq_h__
