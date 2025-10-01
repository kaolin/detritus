/** BTW, it is covered by the LGPL  ** Bob Pendleton **/
/** http://www.devolution.com/pipermail/sdl/2004-December/066119.html **/

/** 

  +--------------------------------------+
  + Bob Pendleton: writer and programmer +
  + email: Bob@xxxxxxxxxxxxx             +
  + blog:  www.Stonewolf.net             +
  + web:   www.GameProgrammer.com        +
  +--------------------------------------+

	http://www.oreillynet.com/pub/au/1205

	Edits by Kaolin Fire ( kfire@xxxxxxxx; web: erif.org )
		now takes into account glyph minx so glyphs can overlap as intended
		now takes into account newline character

**/

#include "font.h"

#include <iostream>
using namespace std;

  void Font::loadChar(char c)
  {
    GLfloat texcoord[4];
    char letter[2] = {0, 0};

    if ((minGlyph <= c) && 
        (c <= maxGlyph) &&
        (NULL == glyphs[((int)c)].pic))
    {
      SDL_Surface *g0 = NULL;
      SDL_Surface *g1 = NULL;

      letter[0] = c;
      TTF_GlyphMetrics(ttfFont, 
                       (Uint16)c, 
                       &glyphs[((int)c)].minx, 
                       &glyphs[((int)c)].maxx, 
                       &glyphs[((int)c)].miny, 
                       &glyphs[((int)c)].maxy, 
                       &glyphs[((int)c)].advance);

      g0 = TTF_RenderText_Blended(ttfFont, 
                                 letter, 
                                 foreground);

			/*
      if (NULL != g0)
      {
        g1 = SDL_DisplayFormatAlpha(g0);
        SDL_FreeSurface(g0);
      }
			*/

			g1 = g0;

      if (NULL != g1)
      {
        glyphs[((int)c)].pic = g1;
        glyphs[((int)c)].tex = SDL_GL_LoadTexture(g1, texcoord);
        glyphs[((int)c)].texMinX = texcoord[0];
        glyphs[((int)c)].texMinY = texcoord[1];
        glyphs[((int)c)].texMaxX = texcoord[2];
        glyphs[((int)c)].texMaxY = texcoord[3];
      }
    }
  }

  Font::Font(const char *fontName,
       int pointSize,
       float fgRed, float fgGreen, float fgBlue):
    fontName(fontName),
    pointSize(pointSize),
    fgRed(fgRed), fgGreen(fgGreen), fgBlue(fgBlue),
    ttfFont(NULL)
  {
    if (0 == initCounter)
    {
      if (TTF_Init() < 0)
      {
        //TODO :: errorExit("Can't init SDL_ttf");
				printf("Can't init SDL_ttf\n");
      }
    }
    initCounter++;
    initFont();
  }

  Font::~Font()
  {
    initCounter--;
    if (0 == initCounter)
    {
      TTF_Quit();
    }
  }

  void Font::initFont()
  {
    //SDL_RWops *src = NULL;
    int i;

    //src = SDL_RWFromMem(address, length);

    ttfFont = TTF_OpenFont(fontName, pointSize);
    if (NULL == ttfFont)
    {
      //TODO :: errorExit("Can't open font file");
			printf("Can't open font file\n");
    }

    //TTF_SetFontStyle(ttfFont, style);

    foreground.r = (Uint8)(255 * fgRed);
    foreground.g  = (Uint8)(255 * fgGreen);
    foreground.b = (Uint8)(255 * fgBlue);

    height = TTF_FontHeight(ttfFont);
    ascent = TTF_FontAscent(ttfFont);
    descent = TTF_FontDescent(ttfFont);
    lineSkip = TTF_FontLineSkip(ttfFont);

    for (i = minGlyph; i <= maxGlyph; i++)
    {
      glyphs[i].pic = NULL;
      glyphs[i].tex = 0;
    }
  }

  int Font::getLineSkip()
  {
    return lineSkip;
  }

  int Font::getHeight()
  {
    return height;
  }

  void Font::textSize(const char *text, 
                SDL_Rect *r)
  {
    int maxx = 0;
    int advance = 0;
    //int minx = 0; // unused
		int w_largest = 0;
		char lastchar = 0;

    r->x = 0;
    r->y = 0;
    r->w = 0;
    r->h = height;

    while (0 != *text)
    {
      if ((minGlyph <= *text) && (*text <= maxGlyph))
      {
				lastchar = *text;
				if (*text == '\n') {
					r->h += lineSkip;
    			r->w = r->w - advance + maxx;
					if (r->w > w_largest) w_largest = r->w;
					r->w = 0;
				} else {
        	loadChar(*text);

        	maxx = glyphs[((int)*text)].maxx;
        	advance = glyphs[((int)*text)].advance;
        	r->w += advance;
				}
      }

      text++;
    }
		if (lastchar != '\n') {
    	r->w = r->w - advance + maxx;
			if (r->w > w_largest) w_largest = r->w;
		} else {
			r->h -= lineSkip;
		}

		if (w_largest > r->w) r->w = w_largest;

  }

  void Font::drawText(const char *text, int x, int y)
  {
    GLfloat left, right;
    GLfloat top, bottom;
    GLfloat texMinX, texMinY;
    GLfloat texMaxX, texMaxY;
    GLfloat minx;
    GLfloat baseleft = x;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    while (0 != *text) {
			if (*text == '\n') {
				x = (int) baseleft;
				y += lineSkip;
			} else if ((minGlyph <= *text) && (*text <= maxGlyph)) {
        loadChar(*text);

        texMinX = glyphs[((int)*text)].texMinX;
        texMinY = glyphs[((int)*text)].texMinY;
        texMaxX = glyphs[((int)*text)].texMaxX;
        texMaxY = glyphs[((int)*text)].texMaxY;

        minx = glyphs[((int)*text)].minx;

        left   = x + minx;
        right  = x + glyphs[((int)*text)].pic->w + minx;
        top    = y;
        bottom = y + glyphs[((int)*text)].pic->h;

        glBindTexture(GL_TEXTURE_2D, glyphs[((int)*text)].tex);

        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(texMinX, texMinY); glVertex2f( left,    top);
        glTexCoord2f(texMaxX, texMinY); glVertex2f(right,    top);
        glTexCoord2f(texMinX, texMaxY); glVertex2f( left, bottom);
        glTexCoord2f(texMaxX, texMaxY); glVertex2f(right, bottom);

        glEnd();

        x += glyphs[((int)*text)].advance;
      }

      text++;
    }

    glPopAttrib();
  }

	int Font::initCounter = 0;
