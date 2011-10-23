#include "precompiled.h"

#include "UtilityConstants.h"

void reportError() {
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error: %s\n", errString);
	}
}

void clearError() {
	//while( glGetError() != GL_NO_ERROR );
}