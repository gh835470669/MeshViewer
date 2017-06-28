#include "makaidebug.h"

void makai::CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        qDebug("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
    }
}