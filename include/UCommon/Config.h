/*
MIT License

Copyright (c) 2024 Ubpa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

/** You can define your assert and no entry macros. */
//#define UBPA_UCOMMON_ASSERT(expression) <Your-Assert>
//#define UBPA_UCOMMON_NO_ENTRY() <Your-No-Entry>
//#define UBPA_UCOMMON_API <Your-API>
//#define UBPA_UCOMMON_MALLOC(size) <Your-Malloc>
//#define UBPA_UCOMMON_FREE(ptr) <Your-Free>

/** Uncomment this code for forward compatibility. */
//#define UBPA_UCOMMON_ELEMENT_TYPE_TEX_C_DEFAULT = EElementType::Uint8

#define UBPA_UCOMMON_DELTA 0.000001f

#define UBPA_UCOMMON_TVECTOR2_DEFINE
#define UBPA_UCOMMON_TVECTOR_DEFINE
#define UBPA_UCOMMON_TVECTOR4_DEFINE

//----------------------------------------------------------

//#define UCOMMON_COMPATIBLE_CPP14

#ifndef UBPA_UCOMMON_ASSERT

/**
* If you want to use other assertion functions,
* comment out the following line of code and define UBPA_UCOMMON_ASSERT(expression)
*/
#define UBPA_UCOMMON_USING_CASSERT

#ifdef UBPA_UCOMMON_USING_CASSERT
#include <cassert>
#define UBPA_UCOMMON_ASSERT(expression) assert(expression)
#else
#define UBPA_UCOMMON_ASSERT(expression) ((void)0)
#endif // UBPA_UCOMMON_USING_CASSERT
#endif // UBPA_UCOMMON_ASSERT

#ifndef UBPA_UCOMMON_NO_ENTRY
#ifdef UBPA_UCOMMON_ASSERT
#define UBPA_UCOMMON_NO_ENTRY() UBPA_UCOMMON_ASSERT(false)
#else
#define UBPA_UCOMMON_NO_ENTRY() ((void)0)
#endif
#endif

#if !defined(UBPA_UCOMMON_API)
 #if defined(UBPA_UCOMMON_USING_SHARED_LIBRARY)
  #if defined(_WIN32) || defined(__CYGWIN__) /** Windows */
   #ifdef UBPA_UCOMMON_BUILDING_SHARED_LIBRARY
    #if defined(__GNUC__)
     #define __attribute__((dllexport))
    #else
     #define UBPA_UCOMMON_API __declspec(dllexport)
    #endif // __GNUC__
   #else
    #if defined(__GNUC__)
     #define __attribute__((dllimport))
    #else
     #define UBPA_UCOMMON_API __declspec(dllimport)
    #endif // __GNUC__
   #endif // UBPA_UCOMMON_BUILDING_SHARED_LIBRARY
  #elif defined(__GNUC__)
   #if __GNUC__ >= 4 /** GCC 4.x has support for visibility options */
    #define UBPA_UCOMMON_API __attribute__((visibility("default")))
   #else
    #define UBPA_UCOMMON_API
   #endif // __GNUC__
  #else
   #error "Unknown case"
  #endif
 #else
  #define UBPA_UCOMMON_API
 #endif // defined(UBPA_UCOMMON_USING_SHARED_LIBRARY)
#endif // !defined(UBPA_UCOMMON_API)

#if !defined(UBPA_UCOMMON_API)
#error "UBPA_UCOMMON_API is not defined."
#endif // !UBPA_UCOMMON_API

#if (defined(UBPA_UCOMMON_MALLOC) && !defined(UBPA_UCOMMON_FREE)) || (!defined(UBPA_UCOMMON_MALLOC) && defined(UBPA_UCOMMON_FREE))
#error "You must define malloc and free in the same time."
#endif

#if !defined(UBPA_UCOMMON_MALLOC) && !defined(UBPA_UCOMMON_FREE)
#include <cstdlib>
#define UBPA_UCOMMON_MALLOC(size) (malloc(size))
#define UBPA_UCOMMON_FREE(ptr) (free(ptr))
#endif
