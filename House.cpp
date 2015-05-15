#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include <FreeImage.h>

#include <assimp\cimport.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Wall.h"
#include "ExternalModel.h"

namespace Light {
	GLfloat position[] = {0, 3.4, 0, 1};
	
	void init() {
		GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat no_light[] = {0, 0, 0, 0};
		GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};

		glShadeModel (GL_SMOOTH);
		
		glLightfv (GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv (GL_LIGHT0, GL_SPECULAR, white_light);
		glLightModelfv (GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
		glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		
		glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.3);
		glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
		glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);

		glEnable (GL_LIGHTING);
		glEnable (GL_LIGHT0);
		glEnable (GL_NORMALIZE);
	}
	
	void place() {
		glLightfv (GL_LIGHT0, GL_POSITION, position);
	}
}

#define PI 3.1415926535897932384626433832795
#include "Viewer.h"

// Global variables
SDL_Window* mainwindow; /* Our window handle */
bool quit = false;
bool movingObject = false;
ExternalModels::ExternalModel* currentObject = NULL;
Viewer viewer;

/* ------------------------------------------------------------------------- */
//	Event handling
/* ------------------------------------------------------------------------- */

float step = 0.05;
float angle = 0;

const Uint8 *state = SDL_GetKeyboardState(NULL);
void handleKeypress() {
	float a = 0,b = 0,c = 0;
	if ( state[SDL_SCANCODE_ESCAPE] ) {
		quit = true;
	}
	if ( state[SDL_SCANCODE_W] ) {
		viewer.getDirection(a,b,c);
		viewer.translatePosition (a*step, 0, c*step); //go ahead
	}
	if ( state[SDL_SCANCODE_S] ) {
		viewer.getDirection(a,b,c);
		viewer.translatePosition (-a*step, 0, -c*step); //go back		
	}
	// rotate camera
	if ( state[SDL_SCANCODE_A] ) { // left
		viewer.spin += 0.02;
	}	
	if ( state[SDL_SCANCODE_D] ) { // right
		viewer.spin -= 0.02;
	}	
	if ( state[SDL_SCANCODE_Q] ) { //up
		viewer.spinX -= 0.08;
	}	
	if ( state[SDL_SCANCODE_E] ) { // down
		viewer.spinX += 0.08;
	}	
	// up - down camera
	if ( state[SDL_SCANCODE_Z] ) { 
		viewer.setUpDirection(angle -= 0.05);
	}	
	if ( state[SDL_SCANCODE_C] ) {
		viewer.setUpDirection(angle += 0.05);
	}
	if(state[SDL_SCANCODE_SPACE]){
		for (int i = 0; i < ExternalModels::modelNum; i ++) {
			GLfloat x1_, y1_, z1_, x2_, y2_, z2_;
			currentObject = ExternalModels::modelList[i];
			currentObject->getBoundingBox (x1_, y1_, z1_, x2_, y2_, z2_);
			if (viewer.tracing (x1_, y1_, z1_, x2_, y2_, z2_)) {
				movingObject = true;
				break;
			}
		}
	}
	if (state[SDL_SCANCODE_P]) {
		SDL_Surface * surf = SDL_CreateRGBSurface(SDL_SWSURFACE, 1366, 768, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
		glReadPixels (0, 0, 1366, 768, GL_RGB, GL_UNSIGNED_BYTE, surf->pixels);
		//SDL_Surface * flip = flipVert(surf);
		SDL_SaveBMP(surf, "screen.bmp");
		SDL_FreeSurface(surf);
		//SDL_SaveBMP(flip, filename.c_str());
		//SDL_FreeSurface(flip);
	}
}

void handleMouse() {
	int x = 0, y = 0;
	SDL_GetRelativeMouseState(&x, &y);
	
	viewer.spin += -x*abs(x)/49000.0; 
	viewer.spinX -= y*abs(y)/49000.0;
	if (viewer.spinX > 1) viewer.spinX = 1;
	if (viewer.spinX < -1) viewer.spinX = -1;
	viewer.setDirection(sin(viewer.spin), sin(viewer.spinX), cos(viewer.spin));	
	
	if(SDL_GetMouseState(NULL, NULL) &SDL_BUTTON(1)){
		for (int i = 0; i < ExternalModels::modelNum; i ++) {
			GLfloat x1_, y1_, z1_, x2_, y2_, z2_;
			currentObject = ExternalModels::modelList[i];
			currentObject->getBoundingBox (x1_, y1_, z1_, x2_, y2_, z2_);
			if (viewer.tracing (x1_, y1_, z1_, x2_, y2_, z2_)) {
				movingObject = true;
				break;
			}
		}
	}
}
 // Handle keyboard2
void handleKeypress2() {
	if ( state[SDL_SCANCODE_ESCAPE] ) {
		quit = true;
	}
	if ( state[SDL_SCANCODE_W] ) {
		currentObject->translatePosition (0, 0, -step);
	}
	if ( state[SDL_SCANCODE_S] ) {
		currentObject->translatePosition (0, 0, step);
	}
	
	if ( state[SDL_SCANCODE_A] ) { // left
		currentObject->translatePosition (-step, 0, 0);
	}	
	if ( state[SDL_SCANCODE_D] ) { // right
		currentObject->translatePosition (step, 0, 0);
	}
	
	if ( state[SDL_SCANCODE_Q] ) { // down
		currentObject->translatePosition (0, -step, 0);
	}	
	if ( state[SDL_SCANCODE_E] ) { // up
		currentObject->translatePosition (0, step, 0);
	}
	
	if(state[SDL_SCANCODE_SPACE]){
		movingObject = false;
	}
}
//
void handleMouse2() {
	int x = 0, y = 0;
	SDL_GetRelativeMouseState(&x, &y);
	
	viewer.spin += -x*abs(x)/49000.0; 
	viewer.spinX -= y*abs(y)/49000.0;
	if (viewer.spinX > 1) viewer.spinX = 1;
	if (viewer.spinX < -1) viewer.spinX = -1;
	viewer.setDirection(sin(viewer.spin), sin(viewer.spinX), cos(viewer.spin));
	if(SDL_GetMouseState(NULL, NULL) &SDL_BUTTON(1)){
		movingObject = false;
	}
}
//
// ------------------------------------------------------------------------- //

void display() {
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	viewer.applyCamera();
	Light::place();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Walls::render();
	ExternalModels::render();
	viewer.render();
}

int lastTime, currentTime, frameCount;
void calculateFPS() {
	currentTime = SDL_GetTicks();
	if (currentTime - lastTime > 1000) {
		lastTime = currentTime;
		printf ("%d ", frameCount);
		frameCount = 0;
	}
	frameCount ++;
}

void mainLoop() {
	calculateFPS();
	
	SDL_PumpEvents();
	if (!movingObject) {
		handleMouse();
		handleKeypress();
	}
	else {
		handleMouse2();
		handleKeypress2();
	}
	
	display();
	
	SDL_GL_SwapWindow (mainwindow);
}

void myInit() {
	float myNear = 0.2;
	float myFar = 10000.0;

	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glEnable(GL_DEPTH_TEST);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (50, 16 / 9.0, myNear, myFar);
	
	glEnable (GL_TEXTURE_2D);
	//glEnable (GL_COLOR_MATERIAL);
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	Light::init();
	
	Walls::init();
	
	ExternalModels::init();
	
	frameCount = 0; lastTime = SDL_GetTicks();
}

void myCleanup() {
	ExternalModels::cleanUp();
	Walls::cleanUp();
}

int main(int argc, char *argv[])
{
	SDL_GLContext maincontext; /* Our opengl context handle */
	
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 2);

	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);

	/* Create our window centered at 800x600 resolution */
	mainwindow = SDL_CreateWindow("3dhouse", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1366, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);

	/* Create our opengl context and attach it to our window */
	maincontext = SDL_GL_CreateContext (mainwindow);

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval (1);
	
	SDL_SetRelativeMouseMode(SDL_TRUE);

	myInit();
	while(!quit){
		mainLoop();
	}
	myCleanup();
	
	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext (maincontext);
	SDL_DestroyWindow (mainwindow);
	SDL_Quit();

	return 0;
}
