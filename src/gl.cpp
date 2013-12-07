#include "stdafx.h"
#include "OGLPaint.h"
extern HWND		g_hwnd;
extern HDC			g_hdc;
extern HGLRC		g_hRC;	
extern HINSTANCE	g_hInstance;

extern RECT		g_rect;
extern RECT		g_clientRect;
extern int			g_scrWidth;
extern int			g_scrHeight;
extern int			g_cliWidth;
extern int			g_cliHeight;
extern GLfloat		g_aspect;
extern vector<PicElem*> vec;
extern MODE g_mode;

extern Color g_defColor;
extern float g_defSize;

void SceneInit(int w, int h)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glShadeModel(GL_SMOOTH);				
//	glClearColor( 0.0, 0.0, 0.0, 0.5 );
//	glClearDepth(1.0f);				
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void SceneResizeViewport(GLsizei w, GLsizei h)
{
	if(h==0) 
	{	
		h=1;
	}
	g_aspect = (GLfloat)w/(GLfloat)h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,g_clientRect.right,0,g_clientRect.bottom,10.0f,-10.0f);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();	
	gluLookAt(0,0,8.0f,0,0,0,0,1,0);
}

void SceneShow(GLvoid)								
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glPushMatrix();

	for(size_t i = 0; i != vec.size(); ++i)
	{
		vec[i]->render();
	}
	
	glFlush();
	glPopMatrix();
	SwapBuffers(g_hdc);
}

void EnableOpenGL()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;

	g_hdc = GetDC( g_hwnd );

	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags =   PFD_DRAW_TO_WINDOW |
					PFD_SUPPORT_OPENGL |
					PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat( g_hdc, &pfd );

	SetPixelFormat( g_hdc, iFormat, &pfd );

	g_hRC = wglCreateContext( g_hdc );
	wglMakeCurrent( g_hdc, g_hRC );
}

void DisableOpenGL()
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( g_hRC );
	ReleaseDC( g_hwnd, g_hdc );
}

bool ChangeResolution(int w, int h, int bitdepth)
{
	DEVMODE devMode;
	int   modeSwitch;
	int   closeMode = 0;

	EnumDisplaySettings(NULL, closeMode, &devMode);

	devMode.dmBitsPerPel = bitdepth;
	devMode.dmPelsWidth  = w;
	devMode.dmPelsHeight = h;
	devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	modeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

	if(modeSwitch == DISP_CHANGE_SUCCESSFUL)
	{
		return true;
	}
	else
	{
		ChangeDisplaySettings(NULL, 0);
		return false;
	}
}

void addPoints(LPARAM param)
{
	vec[0]->add(Point(LOWORD(param), g_cliHeight - HIWORD(param)),g_defColor,g_defSize);
}

void addLines(LPARAM param1, LPARAM param2)
{
	vec[1]->add(Point(LOWORD(param1), g_cliHeight - HIWORD(param1)), 
		Point(LOWORD(param2), g_cliHeight - HIWORD(param2)), g_defColor, g_defSize);
}

void modifyLastLine(LPARAM param)
{
	vec[1]->replaceLast(LOWORD(param), g_cliHeight - HIWORD(param));
}

// void addPolygon(LPARAM param)
// {
// 	PicElem* p = new Polygon;
// 	p->add(Point(LOWORD(param), g_clientRect.bottom - HIWORD(param)),g_defColor,g_defSize);
// 	vec.push_back(p);
// }

void addLastPoint(LPARAM param)
{
	vec[vec.size() - 1]->add(Point(LOWORD(param), g_cliHeight - HIWORD(param)),g_defColor,g_defSize);
}

void modifyLastPoint(LPARAM param)
{
	vec[vec.size() - 1]->replaceLast(LOWORD(param), g_cliHeight - HIWORD(param));
}
