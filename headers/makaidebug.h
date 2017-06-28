#ifndef MAKAIDEBUG_H
#define MAKAIDEBUG_H

#include <GL/glew.h>
#include <QDebug>

namespace makai
{
    void CheckOpenGLError(const char* stmt, const char* fname, int line);
}

#ifdef _DEBUG
    #define GL_CHECK(stmt) do { \
            stmt; \
            makai::CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
    #define GL_CHECK(stmt) stmt
#endif

#endif // MAKAIDEBUG_H
