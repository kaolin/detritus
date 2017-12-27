#include "skybox.h"
#include "game.h"

using std::cout;
using std::endl;

Skybox::Skybox() {
	//cout << "CREATING SKYBOX" << endl;
	position = XYZ(0,0,0);
	size = XYZ(30,30,30);
	displayList = 0;
	//cout << "SKYBOX GETTING STATE" << endl;
	state = &State::Get();
	//cout << "SKYBOX CREATED" << endl;
	skybox_tex = state->skybox_tex;
}

Skybox::~Skybox() {
	glDeleteLists(displayList,1);
}

void Skybox::render() {
	//Timer *thing = &state->thing;
	glDepthMask(0);
	glPushMatrix();
	//turn off fog!
	//...
	//rotate the cube
	//float theta = 360*sin((thing.get_ticks()+tickoffset)/20000.0f); // reset thing when ... thing is some nice clean multiple? TODO
	//float theta = thing->get_ticks()/500.0f; //360*sin((thing.get_ticks()+tickoffset)/20000.0f); // reset thing when ... thing is some nice clean multiple? TODO
  //glRotatef (theta, 0.0f, 1.0f, 0.0f); // default lazy spin


	if (displayList == 0) {
		displayList = glGenLists(1);
		glNewList(displayList,GL_COMPILE);
			glColor4f(1,1,1,1);
			glTranslatef(position.r3[0],position.r3[1],position.r3[2]);
			glScalef(size.r3[0],size.r3[1],size.r3[2]);

			float cz = -0.0f,cx = 1.0f;
			float r = 1.0f; // If you have border issues change this to 1.005f
			// Common Axis Z - FRONT Side
			glBindTexture(GL_TEXTURE_2D,skybox_tex[4]);
			glBegin(GL_QUADS);	
				glTexCoord2f(cx, cz); glVertex3f(-r ,1.0f,-r);
				glTexCoord2f(cx,  cx); glVertex3f(-r,1.0f,r);
				glTexCoord2f(cz,  cx); glVertex3f( r,1.0f,r); 
				glTexCoord2f(cz, cz); glVertex3f( r ,1.0f,-r);
			glEnd();
 
			// Common Axis Z - BACK side
			glBindTexture(GL_TEXTURE_2D,skybox_tex[5]);
			glBegin(GL_QUADS);		
				glTexCoord2f(cx,cz);  glVertex3f(-r,-1.0f,-r);
				glTexCoord2f(cx,cx);  glVertex3f(-r,-1.0f, r);
				glTexCoord2f(cz,cx);  glVertex3f( r,-1.0f, r); 
				glTexCoord2f(cz,cz);  glVertex3f( r,-1.0f,-r);
			glEnd();
 
			// Common Axis X - Left side
			glBindTexture(GL_TEXTURE_2D,skybox_tex[3]);
			glBegin(GL_QUADS);		
				glTexCoord2f(cx,cx); glVertex3f(-1.0f, -r, r);	
				glTexCoord2f(cz,cx); glVertex3f(-1.0f,  r, r); 
				glTexCoord2f(cz,cz); glVertex3f(-1.0f,  r,-r);
				glTexCoord2f(cx,cz); glVertex3f(-1.0f, -r,-r);		
			glEnd();
 
			// Common Axis X - Right side
			glBindTexture(GL_TEXTURE_2D,skybox_tex[2]);
			glBegin(GL_QUADS);		
				glTexCoord2f( cx,cx); glVertex3f(1.0f, -r, r);	
				glTexCoord2f(cz, cx); glVertex3f(1.0f,  r, r); 
				glTexCoord2f(cz, cz); glVertex3f(1.0f,  r,-r);
				glTexCoord2f(cx, cz); glVertex3f(1.0f, -r,-r);
			glEnd();
 
			// Common Axis Y - Draw Up side
			glBindTexture(GL_TEXTURE_2D,skybox_tex[0]);
			glBegin(GL_QUADS);		
				glTexCoord2f(cz, cz); glVertex3f( r, -r,1.0f);
				glTexCoord2f(cx, cz); glVertex3f( r,  r,1.0f); 
				glTexCoord2f(cx, cx); glVertex3f(-r,  r,1.0f);
				glTexCoord2f(cz, cx); glVertex3f(-r, -r,1.0f);
			glEnd();
 
			// Common Axis Y - Down side
			glBindTexture(GL_TEXTURE_2D,skybox_tex[1]);
			glBegin(GL_QUADS);		
				glTexCoord2f(cz,cz);  glVertex3f( r, -r,-1.0f);
				glTexCoord2f( cx,cz); glVertex3f( r,  r,-1.0f); 
				glTexCoord2f( cx,cx); glVertex3f(-r,  r,-1.0f);
				glTexCoord2f(cz, cx); glVertex3f(-r, -r,-1.0f);
			glEnd();

		glEndList();
	}
 glCallList(displayList);
	// Load Saved Matrix
	glPopMatrix();

	glDepthMask(1);
 
};
