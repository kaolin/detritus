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

**/

#ifndef __FONT_H
#define __FONT_H

#include "os.h"

class Font
{
private:
  static const int minGlyph = ' ';
  static const int maxGlyph = 126;

  static int initCounter;

  typedef struct
  {
    int minx, maxx;
    int miny, maxy;
    int advance;
    SDL_Surface *pic;
    GLuint tex;
    GLfloat texMinX, texMinY;
    GLfloat texMaxX, texMaxY;
  } glyph;

  int height;
  int ascent;
  int descent;
  int lineSkip;
  glyph glyphs[maxGlyph + 1];

  const char *fontName;
  //int length;
  int pointSize;
  //int style;
  float fgRed, fgGreen, fgBlue;
  //float bgRed, bgGreen, bgBlue;

  TTF_Font *ttfFont;

  SDL_Color foreground;
  //SDL_Color background;

  void loadChar(char c);

public:

  Font(const char *fontName,
       int pointSize, 
       float fgRed, float fgGreen, float fgBlue);

  ~Font();

  void initFont();

  int getLineSkip();

  int getHeight();

  void textSize(const char *text, SDL_Rect *r);

  void drawText(const char *text, int x, int y);

};
#endif
