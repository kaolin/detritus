/** http://osdl.sourceforge.net/OSDL/OSDL-0.3/src/doc/web/main/documentation/rendering/SDL-openGL-examples.html **/
/**
 * Returns the first power of 2 greater or equal to the specified value.
 *
 * @param value that the  
 *
 *
 */

#include "glsdl.h"

int powerOfTwo( int value )
{

	int result = 1 ;
	
	while ( result < value )
		result *= 2 ;
	
	return result ;		
}

//  Create a texture from a surface. Set the alpha according
//  to the color key. Pixels that match the color key get an
//  alpha of zero while all other pixels get an alpha of
//  one.

GLuint loadTextureColorKey(SDL_Surface *surface, 
                           GLfloat *texcoord,
                           int ckr, 
                           int ckg, 
                           int ckb)
{
  GLuint texture;
  int w, h;
  SDL_Surface *image;
  SDL_Rect area;
  Uint32 colorkey;

  // Use the surface width and height expanded to powers of 2 

  w = powerOfTwo(surface->w);
  h = powerOfTwo(surface->h);
  texcoord[0] = 0.0f;                    // Min X 
  texcoord[1] = 0.0f;                    // Min Y 
  texcoord[2] = (GLfloat)surface->w / w; // Max X 
  texcoord[3] = (GLfloat)surface->h / h; // Max Y 

  image = SDL_CreateRGBSurface(
                               SDL_SWSURFACE,
                               w, h,
                               32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks 
                               0x000000FF, 
                               0x0000FF00, 
                               0x00FF0000, 
                               0xFF000000
#else
                               0xFF000000,
                               0x00FF0000, 
                               0x0000FF00, 
                               0x000000FF
#endif
                               );
  if (image == NULL)
  {
    return 0;
  }

  // Set up so that colorkey pixels become transparent 

  colorkey = SDL_MapRGBA(image->format, ckr, ckg, ckb, 0);
  SDL_FillRect(image, NULL, colorkey);

  colorkey = SDL_MapRGBA(surface->format, ckr, ckg, ckb, 0);
  SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);

  // Copy the surface into the GL texture image 
  area.x = 0;
  area.y = 0;
  area.w = surface->w;
  area.h = surface->h;
  SDL_BlitSurface(surface, &area, image, &area);

  // Create an OpenGL texture for the image 

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               w, h,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               image->pixels);

  SDL_FreeSurface(image); // No longer needed 

  return texture;
}

/** 	Sam Lantinga, Software Engineer, Blizzard Entertainment **/
GLuint SDL_GL_LoadTexture(SDL_Surface *surface) {
	GLfloat texcoord[4]; // dummy data because we don't care about the size.
	return  SDL_GL_LoadTexture(surface,texcoord);
}
GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord) {
    GLuint texture;
    int w, h;
    SDL_Surface *image;
    SDL_Rect area;
    Uint32 saved_flags;
    Uint8  saved_alpha;

    /* Use the surface width and height expanded to powers of 2 */
    w = powerOfTwo(surface->w);
    h = powerOfTwo(surface->h);
    texcoord[0] = 0.0f;         /* Min X */
    texcoord[1] = 0.0f;         /* Min Y */
    texcoord[2] = (GLfloat)surface->w / w;  /* Max X */
    texcoord[3] = (GLfloat)surface->h / h;  /* Max Y */

    image = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
            0x000000FF,
            0x0000FF00,
            0x00FF0000,
            0xFF000000
#else
            0xFF000000,
            0x00FF0000,
            0x0000FF00,
            0x000000FF
#endif
               );
    if ( image == NULL ) {
        return 0;
    }

    /* Save the alpha blending attributes */
    saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
    saved_alpha = surface->format->alpha;
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
        SDL_SetAlpha(surface, 0, 0);
    }

    /* Copy the surface into the GL texture image */
    area.x = 0;
    area.y = 0;
    area.w = surface->w;
    area.h = surface->h;
    SDL_BlitSurface(surface, &area, image, &area);

    /* Restore the alpha blending attributes */
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
        SDL_SetAlpha(surface, saved_flags, saved_alpha);
    }

    /* Create an OpenGL texture for the image */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GAME_GLTEXPARAM); // GL_LINEAR / GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GAME_GLTEXPARAM); // GL_LINEAR / GL_NEAREST
    glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA,
             w, h,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             image->pixels);
    SDL_FreeSurface(image); /* No longer needed */

    return texture;
}
GLuint SDL_GL_Build2DMipmaps(SDL_Surface *surface,int width, int height) {
	GLfloat texcoord[4]; // dummy data because we don't care about the size.
	return  SDL_GL_Build2DMipmaps(surface,width,height,texcoord);
}
GLuint SDL_GL_Build2DMipmaps(SDL_Surface *surface, int width, int height, GLfloat *texcoord) {
    GLuint texture;
    int w, h;
    SDL_Surface *image;
    SDL_Rect area;
    Uint32 saved_flags;
    Uint8  saved_alpha;

    /* Use the surface width and height expanded to powers of 2 */
    w = powerOfTwo(surface->w);
    h = powerOfTwo(surface->h);
    texcoord[0] = 0.0f;         /* Min X */
    texcoord[1] = 0.0f;         /* Min Y */
    texcoord[2] = (GLfloat)surface->w / w;  /* Max X */
    texcoord[3] = (GLfloat)surface->h / h;  /* Max Y */

    image = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
            0x000000FF,
            0x0000FF00,
            0x00FF0000,
            0xFF000000
#else
            0xFF000000,
            0x00FF0000,
            0x0000FF00,
            0x000000FF
#endif
               );
    if ( image == NULL ) {
        return 0;
    }

    /* Save the alpha blending attributes */
    saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
    saved_alpha = surface->format->alpha;
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
        SDL_SetAlpha(surface, 0, 0);
    }

    /* Copy the surface into the GL texture image */
    area.x = 0;
    area.y = 0;
    area.w = surface->w;
    area.h = surface->h;
    SDL_BlitSurface(surface, &area, image, &area);

    /* Restore the alpha blending attributes */
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
        SDL_SetAlpha(surface, saved_flags, saved_alpha);
    }

    /* Create an OpenGL texture for the image */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D,
             GL_RGBA,
             width, height,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             image->pixels);
    SDL_FreeSurface(image); /* No longer needed */

    return texture;
}

SDL_Surface *load_image( string filename) { return load_image(filename,true); }
SDL_Surface *load_image( string filename, bool optimized ) { 
	//The image that's loaded 
	SDL_Surface* loadedImage = NULL; 

	//The optimized image that will be used 
	SDL_Surface* optimizedImage = NULL; 

	//Load the image using 
	loadedImage = IMG_Load( filename.c_str() ); 

	//If the image loaded 
	if( optimized && loadedImage != NULL ) { 

		//Create an optimized image 
		optimizedImage = SDL_DisplayFormatAlpha( loadedImage ); 

		//Free the old image 
		SDL_FreeSurface( loadedImage ); 
	} else {
		optimizedImage = loadedImage;
	}

	//Return the optimized image 
	return optimizedImage; 
}

void glTranslatef(const XYZ &v) {
	glTranslatef(v.getX(),v.getY(),v.getZ());
}
void glRotatef(const float theta, const XYZ &v) {
	glRotatef(theta,v.getX(),v.getY(),v.getZ());
}

/** FROM http://www.lighthouse3d.com/opengl/billboarding/index.php3?billCheat */
void billboardCheatSphericalBegin(bool push) {
	
	float modelview[16];
	int i,j;

	// save the current modelview matrix
	if (push) glPushMatrix();

	// get the current modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	// undo all rotations
	// beware all scaling is lost as well 
	for( i=0; i<3; i++ ) 
	    for( j=0; j<3; j++ ) {
		if ( i==j )
		    modelview[i*4+j] = 1.0;
		else
		    modelview[i*4+j] = 0.0;
	    }

	// set the modelview with no rotations
	glLoadMatrixf(modelview);
}



void billboardEnd(bool pop) {

	// restore the previously 
	// stored modelview matrix
	if (pop) glPopMatrix();
}

/** END FROM http://www.lighthouse3d.com/opengl/billboarding/index.php3?billCheat */


