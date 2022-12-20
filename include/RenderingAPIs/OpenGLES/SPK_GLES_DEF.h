//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
//////////////////////////////////////////////////////////////////////////////////


#ifndef H_SPK_GLES_DEF
#define H_SPK_GLES_DEF

//#define USE_WINDOWS

#ifdef _MSC_VER
#pragma warning(disable : 4275) // disables the warning about exporting DLL classes children of non DLL classes
#endif

// 1.02.02 Compatibility with older versions
#ifdef SPK_DLL
#define SPK_GL_IMPORT
#endif

#ifdef SPK_GL_EXPORT
#define SPK_GL_PREFIX __declspec(dllexport)
#elif defined(SPK_IMPORT) || defined(SPK_GL_IMPORT)
#define SPK_GL_PREFIX __declspec(dllimport) 
#else
#define SPK_GL_PREFIX
#endif

#ifndef SPK_NO_GL_INC

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#elif defined(macintosh)
#include <gl.h>
#else
#ifndef USE_WINDOWS
#include <GLES2/gl2.h>
#else
#include "glad/glad.h"
#endif
#endif

#if defined(linux) || defined(__linux)
// #define GLX_GLXEXT_PROTOTYPES
// #include <GL/glx.h>
#endif

#endif

// Defines the APIENTRY if not already done
#ifndef APIENTRY
#define APIENTRY
#endif

/**
* @namespace SPK::GLES
* @brief the namespace for openGLES dependent SPARK code
* @since 12.07.22
*/

#endif
