#include "precompiled.h"

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

#include "DrawingConstants.h"
#include "UtilityConstants.h"

#include <cstdio>

void drawString( int x, int y, void *font, char *string) {  
  char *c;
  glRasterPos2i( x, y );
  for( c=string; *c != '\0'; c++ ) {
    glutBitmapCharacter( font, *c );
  }
}

GLuint loadTexture( const char *filename, int width, int height, int wrap ) {
	FILE * file  = fopen( filename, "rb" );
	if ( file == NULL ) {
		return 0;
	}

    unsigned char *data = new unsigned char[width * height * 3 ]; //malloc( width * height * 3 );

    fread( data, width * height * 3, 1, file );
    fclose( file );

	GLuint texture;

	//reportError();
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
	printf("%d", texture );

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

    delete[] data;

    return texture;
}