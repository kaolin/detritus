/** http://osdl.sourceforge.net/OSDL/OSDL-0.3/src/doc/web/main/documentation/rendering/SDL-openGL-examples.html **/

#ifndef __GLSDL_H
#define __GLSDL_H

#include "os.h"
#include "geometry.h"

#include <string>

using namespace std;

/**
 * Returns the first power of 2 greater or equal to the specified value.
 *
 * @param value that the  
 *
 *
 */

int powerOfTwo( int value );

/**
 * Create a texture from a surface. Leaves alpha as is
 *
 * The source surface can come from example from a PNG file.
 *
 */

GLuint uploadTextureFromSurface( const SDL_Surface & sourceSurface, GLfloat * textureCoordinates);



/** http://www.devolution.com/pipermail/sdl/2004-December/066130.html **/
/** Bob Pendleton **/
//  Create a texture from a surface. Set the alpha according
//  to the color key. Pixels that match the color key get an
//  alpha of zero while all other pixels get an alpha of
//  one.

GLuint loadTextureColorKey(SDL_Surface *surface, 
                           GLfloat *texcoord,
                           int ckr, 
                           int ckg, 
                           int ckb);

/** 	Sam Lantinga, Software Engineer, Blizzard Entertainment **/
GLuint SDL_GL_LoadTexture(SDL_Surface *surface);
GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord);
GLuint SDL_GL_Build2DMipmaps(SDL_Surface *surface,int width, int height);
GLuint SDL_GL_Build2DMipmaps(SDL_Surface *surface, int width, int height, GLfloat *texcoord);

SDL_Surface *load_image(string filename);
SDL_Surface *load_image(string filename, bool optimized );

void glTranslatef(const XYZ &v);
void glRotatef(const float theta, const XYZ &v);

/** FROM http://www.lighthouse3d.com/opengl/billboarding/index.php3?billCheat */
void billboardCheatSphericalBegin(bool push=true);
void billboardEnd(bool pop=true);
/** END FROM http://www.lighthouse3d.com/opengl/billboarding/index.php3?billCheat */


#endif
