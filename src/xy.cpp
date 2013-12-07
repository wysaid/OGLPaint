#include "stdafx.h"
#include "resource.h"
#include "xy.h"
#define MAX_LOADSTRING 100

HINSTANCE hInst;					

HWND		g_hwnd;
HDC			g_hdc;
HGLRC		g_hRC=NULL;	
HINSTANCE	g_hInstance;

RECT		g_rect;
RECT		g_clientRect;

int			g_scrWidth = 800;
int			g_scrHeight = 600;
int			g_cliWidth;
int			g_cliHeight;

GLfloat		g_aspect;
vector<PicElem*> vec;
MODE		g_mode = modeLINES;

float		g_defSize = 1.0f;
// float		g_defWidth = 1.0f;
Color		g_defColor(1.0f,1.0f,1.0f);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	
	MSG msg;
	HACCEL hAccelTable;
	
	vec.push_back(new Points);
	vec.push_back(new Lines);
//	vec.push_back(new Lines);

	MyRegisterClass(hInstance);
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	GetClientRect(g_hwnd,&g_clientRect);
	g_cliWidth = g_clientRect.right - g_clientRect.left;
	g_cliHeight = g_clientRect.bottom - g_clientRect.top;
	EnableOpenGL();
	SceneInit(g_clientRect.right, g_clientRect.bottom);
	GetWindowRect(g_hwnd, &g_rect);
	g_scrWidth = g_rect.right - g_rect.left;
	g_scrHeight = g_rect.bottom - g_rect.top;
	if(g_cliWidth > 0 && g_cliHeight > 0)
		SceneResizeViewport(g_cliWidth, g_cliHeight);
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_XY);
	SceneShow();
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	DisableOpenGL();
	DestroyWindow(g_hwnd);
	for(size_t i = 0; i != vec.size(); ++i)
		delete vec[i];
	vec.clear();
	ChangeDisplaySettings(NULL,0);
	
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_XY);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= (LPCSTR)IDC_XY;
	wcex.lpszClassName	= "Name";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; 
	
	g_hwnd = CreateWindow("Name", "wysaid", WS_TILEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, 0, g_scrWidth, g_scrHeight, NULL, NULL, hInstance, NULL);
	
	if (!g_hwnd)
	{
		return FALSE;
	}

	FileInitialize (g_hwnd);
	ChangeDisplaySettings(NULL, 0);
	ShowWindow(g_hwnd, nCmdShow);
	UpdateWindow(g_hwnd);	
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static isKeydown = false;
	int wmId, wmEvent;
/*	int tempX,tempY;*/
	switch (message) 
	{
	case WM_COMMAND:
	{
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_PENCIL:
			g_mode = modePENCIL;
			break;
		case IDM_POINTS:
			g_mode = modePOINTS;
			break;
		case IDM_LINES:
			g_mode = modeLINES;
			break;
		case IDM_Polygon:
			g_mode = modePOLYGON;
			break;
		case IDM_CIRCLE:
			g_mode = modeCIRCLE;
			break;
		case IDM_COLOR:
			DialogBox(hInst, (LPCTSTR)IDD_COLORBOX, hWnd, (DLGPROC)colorBox);
			break;
		case IDM_SIZEBOX:
			DialogBox(hInst, (LPCTSTR)IDD_DEFAULTWIDTH, hWnd, (DLGPROC)sizeBox);
			break;
		case IDM_CLEAR:
			{
				vec[0]->clear();
				vec[1]->clear();
				for(size_t i=2; i < vec.size(); ++i)
					delete vec[i];
				vec.erase(vec.begin() + 2, vec.end());
				InvalidateRect(hWnd, NULL, false);
				break;
			}
		case IDM_ADDPOINT:
			DialogBox(hInst, (LPCTSTR)IDD_ADDPOINT, hWnd, (DLGPROC)addPointBox);
			break;
		case IDM_ADDLINE:
			DialogBox(hInst, (LPCTSTR)IDD_ADDLINES, hWnd, (DLGPROC)addLineBox);
			break;
		case IDM_ADDPOLYGON:
			DialogBox(hInst, (LPCTSTR)IDD_ADDPOLYGON, hWnd, (DLGPROC)addPolygonBox);
			break;
		case IDM_ADDCIRCLE:
			DialogBox(hInst, (LPCTSTR)IDD_ADDCIRCLE, hWnd, (DLGPROC)addCircleBox);
			break;
		case IDM_SAVE: save(hWnd); break;
		case IDM_HELP:
			DialogBox(hInst, (LPCTSTR)IDD_HELPBOX, hWnd, (DLGPROC)Help);
			break;
		case IDM_DELETEPOINT:
			vec[0]->eraseLast();
			break;
		case IDM_DELETELINE:
			vec[1]->eraseLast();
			break;
		case IDM_DELETEOTHER:
			if(vec.size() >2)
				vec.erase(vec.end() -1);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}

	case WM_RBUTTONDOWN:
		if(!isKeydown || g_mode != modePOLYGON) break;
		addLastPoint(lParam);
		break;
	case WM_MOUSEMOVE:
		if(!isKeydown) break;
		switch(g_mode)
		{
		case modePENCIL: addLastPoint(lParam); break;
		case modePOINTS: addPoints(lParam); break;
		case modeLINES: modifyLastLine(lParam); break;
		case modePOLYGON: 
		case modeCIRCLE: modifyLastPoint(lParam); break;
		}
		InvalidateRect(hWnd,NULL,false);
		break;
	case WM_LBUTTONDOWN:
		isKeydown = true;
		switch(g_mode)
		{
		case modePENCIL:
			{
				Point pnt(LOWORD(lParam),g_clientRect.bottom - HIWORD(lParam));
				PicElem* p = new Pencil;
				p->add(pnt, g_defColor,g_defSize);
				vec.push_back(p);
			}
			break;
		case modePOINTS: addPoints(lParam); break;
		case modeLINES:
			addLines(lParam,lParam);
			break;
		case modePOLYGON:
			{
				Point pnt(LOWORD(lParam),g_clientRect.bottom - HIWORD(lParam));
				PicElem* p = new Polygons;
				p->add(pnt, pnt, g_defColor,g_defSize);
				p->add(pnt, g_defColor,g_defSize);
				vec.push_back(p);
			}
			break;
		case modeCIRCLE:
			{
				Point pnt(LOWORD(lParam),g_clientRect.bottom - HIWORD(lParam));
				PicElem* p = new Circles;
				p->add(pnt, g_defColor, g_defSize);
				vec.push_back(p);
			}			
		}
		InvalidateRect(hWnd,NULL,false);
		break;
	case WM_LBUTTONUP:
		isKeydown = false;
		break;
	case WM_SIZE:
		{
			GetClientRect(g_hwnd, &g_clientRect);
			g_cliWidth = g_clientRect.right - g_clientRect.left;
			g_cliHeight = g_clientRect.bottom - g_clientRect.top;
			GetWindowRect(g_hwnd, &g_rect);
			g_scrWidth = g_rect.right - g_rect.left;
			g_scrHeight = g_rect.bottom - g_rect.top;
			if(g_cliWidth > 0 && g_cliHeight > 0)
				SceneResizeViewport(g_cliWidth, g_cliHeight);
		}
		break;
	case WM_CHAR:
		switch(wParam)
		{
		case '\n': case '\r':
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case ' ':
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;				
		}
	case WM_PAINT:
		SceneShow();
		ValidateRect(hWnd,NULL);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK Help(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}


LRESULT CALLBACK inputBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}

LRESULT CALLBACK colorBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		else if(LOWORD(wParam) == IDOK )
		{
			char r[128],g[128],b[128];
			int ir,ig,ib;
			ir= GetDlgItemText(hDlg, IDC_COLOR_R, r, 127);
			ig = GetDlgItemText(hDlg, IDC_COLOR_G, g, 127);
			ib = GetDlgItemText(hDlg, IDC_COLOR_B, b, 127);
			r[ir] = 0;
			g[ig] = 0;
			b[ib] = 0;
			g_defColor = Color(atof(r), atof(g), atof(b));
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}

LRESULT CALLBACK sizeBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		else if(LOWORD(wParam) == IDOK)
		{
			char size[128];
			size[GetDlgItemText(hDlg, IDC_DEFAULTWIDTH, size, 127)] = 0;
			g_defSize = atof(size);
			if(g_defSize == 0.0f) g_defSize = 1.0f;
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}


LRESULT CALLBACK addPointBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		else if(LOWORD(wParam) == IDOK)
		{
			char x[128],y[128];
			x[GetDlgItemText(hDlg, IDC_X, x, 127)] = 0;
			y[GetDlgItemText(hDlg, IDC_Y, y, 127)] = 0;
			vec[0]->add(Point(atof(x), g_cliHeight - atof(y)),g_defColor,g_defSize);
			InvalidateRect(g_hwnd, NULL, false);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}

LRESULT CALLBACK addLineBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		else if(LOWORD(wParam) == IDOK)
		{
			char x1[128],y1[128],x2[128],y2[128];
			x1[GetDlgItemText(hDlg, IDC_X1, x1, 127)] = 0;
			y1[GetDlgItemText(hDlg, IDC_Y1, y1, 127)] = 0;
			x2[GetDlgItemText(hDlg, IDC_X2, x2, 127)] = 0;
			y2[GetDlgItemText(hDlg, IDC_Y2, y2, 127)] = 0;
			vec[1]->add(Point(atof(x1), g_cliHeight - atof(y1)),Point(atof(x2), g_cliHeight - atof(y2)),g_defColor,g_defSize);
			InvalidateRect(g_hwnd, NULL, false);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}

LRESULT CALLBACK addPolygonBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool first = false;
	static PicElem* p = NULL;
	static int cnt = 0;
	switch (message)
	{
	case WM_INITDIALOG:
		first = true;
		p = new Polygons;
		cnt = 0;
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) 
		{
			delete p;
			p = NULL;
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		else if(LOWORD(wParam) == IDC_CONTINUE)
		{
			char x[128],y[128];
			x[GetDlgItemText(hDlg, IDC_X, x, 127)] = 0;
			y[GetDlgItemText(hDlg, IDC_Y, y, 127)] = 0;
			if(x[0] && y[0])
			{
				Point pnt(atof(x),g_clientRect.bottom - atof(y));
				p->add(pnt, g_defColor,g_defSize);
				TextOut(GetDC(hDlg),10,90,x,sprintf(x,"第%d个点添加成功!",++cnt));
			}
			else
				TextOut(GetDC(hDlg),10,90,x,sprintf(x,"输入内容不能为空!"));
		}
		else if(LOWORD(wParam) == IDOK)
		{
			char x[128],y[128];
			x[GetDlgItemText(hDlg, IDC_X, x, 127)] = 0;
			y[GetDlgItemText(hDlg, IDC_Y, y, 127)] = 0;
			if(x[0] && y[0])
			{
				Point pnt(atof(x),g_clientRect.bottom - atof(y));
				p->add(pnt, g_defColor,g_defSize);
				vec.push_back(p);
			}
			else
			{
				TextOut(GetDC(hDlg),10,90,x,sprintf(x,"输入内容不能为空!"));
				break;
			}
			InvalidateRect(g_hwnd, NULL, false);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
	}
    return FALSE;
}

LRESULT CALLBACK addCircleBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			char x1[128],y1[128],x2[128],y2[128];
			x1[GetDlgItemText(hDlg, IDC_X1, x1, 127)] = 0;
			y1[GetDlgItemText(hDlg, IDC_Y1, y1, 127)] = 0;
			x2[GetDlgItemText(hDlg, IDC_X2, x2, 127)] = 0;
			y2[GetDlgItemText(hDlg, IDC_Y2, y2, 127)] = 0;
			PicElem* p = new Circles;
			p->add(Point(atof(x1),g_clientRect.bottom - atof(y1)), Point(atof(x2),g_clientRect.bottom - atof(y2)), g_defColor, g_defSize);
			vec.push_back(p);
			InvalidateRect(g_hwnd, NULL, false);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}