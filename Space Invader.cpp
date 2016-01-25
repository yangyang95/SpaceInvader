// Space Invader.cpp : 定義主控台應用程式的進入點。
// E14023234 成大機械系 翁瑞陽
//
// Jerry Weng
// yangyang.Weng@gmail.com

#include "stdafx.h"

#include "shared/gltools.h"	// OpenGL toolkit

#include <ctype.h>		// library deal with char
#include "glPrintf.h"	// library to print chinese

/*
// uncomment if you want to print something to console
#include <iostream>		
using std::cout;
using std::endl;
using std::cin;
*/

#define ENEMY_ROW 10					// default  10
#define ENEMY_COL  3					// default   3 
#define ENEMY_INTERVAL 50				// default  50
#define ENEMY_INIT_HEIGHT 500			// default 500
#define ENEMY_DOWN_TIME_INTERVAL 250	// default 250  (unit: timer function interval) 

// Keep track of windows changing width and height
GLfloat windowWidth;
GLfloat windowHeight;

// load enemy picture
GLbyte *enemy = NULL;
GLint e_Width, e_Height, e_Components;
GLenum e_eFormat;

// load player picture
GLbyte *player = NULL;
GLint p_Width, p_Height, p_Components;
GLenum p_eFormat;

GLfloat player_pos = 480.0f;	// player x position
GLfloat enemy_pos = 200.0f;		// enemy  x position
GLfloat eSpeed = 3.0f;			// enemy  speed 3.0f
GLfloat enemy_down = 0.0f;		// how much enemy go down

GLint Timer = 0;

bool shoot = FALSE;
GLfloat xBullet;		// middle of player
GLfloat yBullet = 40;	// height of the bullet

int kill = 0;	// number of enemy killed
bool enemy_exsist[ENEMY_ROW * ENEMY_COL];	// left high corner is first, then go right.

bool win = FALSE;
bool lose = FALSE;

//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context. 
void SetupRC()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void draw_enemy()
{
	glPixelZoom(2.0f, 2.0f);

	for (int i = 0; i < ENEMY_ROW; i++)
	{
		for (int j = 0; j < ENEMY_COL; j++)
		{
			if (enemy_exsist[i + j*ENEMY_ROW])
			{
				// Use Window coordinates to set raster position
				glRasterPos2i(ENEMY_INTERVAL * i + enemy_pos, ENEMY_INIT_HEIGHT - ENEMY_INTERVAL * j + enemy_down);

				glDrawPixels(e_Width, e_Height, e_eFormat, GL_UNSIGNED_BYTE, enemy);
			}
		}
	}
}

void draw_player()
{
	glPixelZoom(0.15f, 0.15f);
	glRasterPos2i(player_pos, 20.0f);
	glDrawPixels(p_Width, p_Height, p_eFormat, GL_UNSIGNED_BYTE, player);
}

void draw_bullet()
{
	glBegin(GL_TRIANGLES);	
		glVertex2f(xBullet - 3.0f, yBullet);
		glVertex2f(xBullet + 3.0f, yBullet);
		glVertex2f(xBullet, yBullet + 10.0f);
	glEnd();

	// bullet goes up
	yBullet += 10.0f;
	if (yBullet >= windowHeight)
	{
		// when bullet goes out up window, stop this shot.
		shoot = FALSE;
		yBullet = 40;
	}
}

void shoot_enemy()
{	// The function is about whether enemy being shot 
	for (int j = 0; j < ENEMY_COL; j++)
	{
		if (abs(yBullet - (ENEMY_INIT_HEIGHT - ENEMY_INTERVAL * j + enemy_down)) <= 0.1f)
		{
			// y
			for (int i = 0; i < ENEMY_ROW; i++)
			{
				if (xBullet >(ENEMY_INTERVAL * i + enemy_pos) && (xBullet - (ENEMY_INTERVAL * i + enemy_pos) <= 30.0f))
				{
					// x
					if (enemy_exsist[i + j*ENEMY_ROW])
					{
						enemy_exsist[i + j*ENEMY_ROW] = FALSE;	// enemy shutdown
						kill += 1;
						break;
					}
				}
			}
		}
	}
}

void draw_text()
{
	// print chinese text
	glPrintf(">>glut");
	glColor3f(1, 1, 1);
	glPrintf("\n\'A\', \'D\'鍵左右移動，按下空白鍵發射子彈！");	// use A, D to move, and shoot with space.
	
	glColor3f(0.5, 0.8, 0.6);
	glPrintf("\n\n你現在得%i分", kill * 100);		// how many point you got now.

	glColor3f(1.0f, 0.0f, 0.0f);
	if (lose)
		glPrintf("\n\n你輸了!!!!!!!!  按下'Q'離開  按下'R'重來");	// you lose, press Q to leave, R to rest.
	glColor3f(0.7f, 0.6f, 0.9f);
	if (win)
		glPrintf("\n\n你贏了!!!!!!!!  按下'Q'離開  按下'R'重來");	// you win, press Q to leave, R to rest. 
}


///////////////////////////////////////////////////////////////////////        
// Called to draw scene
void RenderScene(void)
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);

	// Targa's are 1 byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load the TGA file, get width, height, and component/format information
	enemy = gltLoadTGA("images/ship2.tga", &e_Width, &e_Height, &e_Components, &e_eFormat);
	player = gltLoadTGA("images/player.tga", &p_Width, &p_Height, &p_Components, &p_eFormat);

	// Draw the pixmap
	if (enemy != NULL && player != NULL)
	{
		draw_enemy();
		draw_player();
	}

	// draw bullet
	if (!shoot)
	{
		xBullet = player_pos + 50;	// shoot from middle of player
	}
	else
	{
		// draw bullet
		draw_bullet();

		// shoot enemy
		shoot_enemy();
	}

	// Don't need the image data anymore
	free(enemy);
	free(player);

	// draw chinese text
	draw_text();

	// Do the buffer Swap
	glutSwapBuffers();
}


//////////////////////////////////////////////////////////////
// For this example, it really doesn't matter what the 
// projection is since we are using glWindowPos
void ChangeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the clipping volume
	gluOrtho2D(0.0f, (GLfloat)w, 0.0, (GLfloat)h);

	windowWidth  = w;
	windowHeight = h;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TimerFunction(int value)
{ 
	// enemy moving 
	if (ENEMY_INTERVAL * ENEMY_ROW + enemy_pos > windowWidth)
		eSpeed = -eSpeed;
	if (enemy_pos < 5.0f)
		eSpeed = -eSpeed;

	enemy_pos += eSpeed;

	// Timer
	Timer += 1;	
	// enemy descend
	if (Timer > ENEMY_DOWN_TIME_INTERVAL && !lose)
	{
		enemy_down -= 10.f;
		Timer = 0;
	}
	
	// win
	if (!win && kill == ENEMY_ROW * ENEMY_COL)		// kill every enemy
	{
		win = TRUE;
	}

	// lose
	if (!lose && (ENEMY_INIT_HEIGHT - ENEMY_INTERVAL * ENEMY_COL + enemy_down <= 40))
	{
		lose = TRUE;
	}

	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 0);
}

void initial_enemy()
{
	for (int i = 0; i < ENEMY_ROW * ENEMY_COL; i++)
	{
		enemy_exsist[i] = TRUE;
	}
}

void restart(){

	// player
	player_pos = 480.0f;	// player position
	
	// bullet
	yBullet = 40;

	// enemy
	initial_enemy();
	kill = 0;	// number of enemy killed
	
	Timer = 0;
	enemy_pos = 200.0f;		// enemy  position
	enemy_down = 0.0f;		// how much enemy go down

	// bool
	shoot = FALSE;
	win = FALSE;
	lose = FALSE;
}

#define SPACEBAR 32
void Keyboard(unsigned char key, int x, int y)
{
	key = tolower(key);

	switch (key)
	{
		case 'd':
			player_pos += 10.0f;
			break;
		case 'a':
			player_pos -= 10.0f;
			break;
		case SPACEBAR:
			shoot = TRUE;
			break;
		case 'q':
			if (win || lose)
				exit(0);
			break;
		case 'r':
			if (win || lose)
				restart();
			break;
	}
}

/////////////////////////////////////////////////////////////
// Main program entrypoint
int main(int argc, char* argv[])
{
	initial_enemy();	

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GL_DOUBLE);
	glutInitWindowSize(1024, 1080);
	glutCreateWindow("Space Invader");
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(33, TimerFunction, 0);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();

	return 0;
}


