/**
 * This file defines all helper functions. Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _OTV_HELPER_H_
#define _OTV_HELPER_H_

#include "otv_common.h"

//! @name error check helper from EPFL ICG class
static inline const char* ErrorString(GLenum error) {
    const char* msg;
    switch (error) {
#define Case(Token)  case Token: msg = #Token; break;
	Case(GL_INVALID_ENUM);
	Case(GL_INVALID_VALUE);
	Case(GL_INVALID_OPERATION);
	Case(GL_INVALID_FRAMEBUFFER_OPERATION);
	Case(GL_NO_ERROR);
	Case(GL_OUT_OF_MEMORY);
#undef Case
    }
    return msg;
}

//! @name check error
static inline void _glCheckError(const char* file, int line, const char* comment) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
	fprintf(stderr, "ERROR: %s (file %s, line %i: %s).\n", comment, file, line, ErrorString(error));
    }
}

#ifndef NDEBUG
#	define check_error_gl(x) _glCheckError(FILE, LINE, x)
#else
#	define check_error_gl() ((void)0)
#endif

namespace otv {
	
    struct ImageData {
	std::vector<unsigned char> data;
	unsigned int width = 0, height = 0;
	bool IsEmpty() { return (width * height <= 0); }
	cy::Point2<int> Size() { return cy::Point2<int>(width, height); }
    };

    namespace helper {

	//! @name mouse2screen: convert mouse coordinate to [-1,1] * [-1,1]
	inline void mouse2screen(int x, int y, float width, float height, cy::Point2f& p)
	{
	    p = cy::Point2f(2.0f * (float)x / width - 1.0f, 1.0f - 2.0f * (float)y / height);
	}

	//! @name writePPM Helper function to write the rendered image as PPM file
	void writePPM(const char *fileName, const ospcommon::vec2i &size, const uint32_t *pixel) {
	    using namespace ospcommon;
	    FILE *file = fopen(fileName, "wb");
	    fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
	    unsigned char *out = (unsigned char *)alloca(3 * size.x);
	    for (int y = 0; y < size.y; y++) {
		const unsigned char *in = (const unsigned char *)&pixel[(size.y - 1 - y)*size.x];
		for (int x = 0; x < size.x; x++) {
		    out[3 * x + 0] = in[4 * x + 0];
		    out[3 * x + 1] = in[4 * x + 1];
		    out[3 * x + 2] = in[4 * x + 2];
		}
		fwrite(out, 3 * size.x, sizeof(char), file);
	    }
	    fprintf(file, "\n");
	    fclose(file);
	}

	//! @name debug functions
	inline void debug(const cy::Matrix3f& m) {
#ifndef NDEBUG
	    std::cout << std::endl;
	    std::cout << "\t" << m(0, 0) << "\t" << m(0, 1) << "\t" << m(0, 2) << std::endl;
	    std::cout << "\t" << m(1, 0) << "\t" << m(1, 1) << "\t" << m(1, 2) << std::endl;
	    std::cout << "\t" << m(2, 0) << "\t" << m(2, 1) << "\t" << m(2, 2) << std::endl;
#endif
	}
	inline void debug(const cy::Matrix4f& m) {
#ifndef NDEBUG
	    std::cout << std::endl;
	    std::cout << "\t" << m(0, 0) << "\t" << m(0, 1) << "\t" << m(0, 2) << "\t" << m(0, 3) << std::endl;
	    std::cout << "\t" << m(1, 0) << "\t" << m(1, 1) << "\t" << m(1, 2) << "\t" << m(1, 3) << std::endl;
	    std::cout << "\t" << m(2, 0) << "\t" << m(2, 1) << "\t" << m(2, 2) << "\t" << m(2, 3) << std::endl;
	    std::cout << "\t" << m(3, 0) << "\t" << m(3, 1) << "\t" << m(3, 2) << "\t" << m(3, 3) << std::endl;
#endif
	}
	inline void debug(const cy::Point2f& m) {
#ifndef NDEBUG
	    std::cout << std::endl;
	    std::cout << "\t" << m[0] << "\t" << m[1] << std::endl;
#endif
	}
	inline void debug(const cy::Point3f& m) {
#ifndef NDEBUG
	    std::cout << std::endl;
	    std::cout << "\t" << m[0] << "\t" << m[1] << "\t" << m[2] << std::endl;
#endif
	}
	inline void debug(const cy::Point4f& m) {
#ifndef NDEBUG
	    std::cout << std::endl;
	    std::cout << "\t" << m[0] << "\t" << m[1] << "\t" << m[2] << "\t" << m[3] << std::endl;
#endif
	}
    };

};

#endif//_OTV_HELPER_H_
