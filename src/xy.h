#pragma once
#if !defined(AFX_XY_H__EEF17F31_8D6E_4A2E_A42E_71220280A745__INCLUDED_)
#define AFX_XY_H__EEF17F31_8D6E_4A2E_A42E_71220280A745__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#define PI	3.14159265
#define PI2	PI*2
//当前绘制模式
enum MODE {modePENCIL, modePOINTS, modeLINES, modePOLYGON, modeCIRCLE};

struct Point
{
	Point(float vx, float vy)
	{
		pnt[0] = vx;
		pnt[1] = vy;
	}
	float pnt[2];
};

struct Color
{
	Color(){}
	Color(float r, float g, float b)
	{
		color[0] = r>1.0f ? 1.0f : r;
		color[1] = g>1.0f ? 1.0f : g;
		color[2] = b>1.0f ? 1.0f : b;
	}
	float color[3];
};

class PicElem
{
public:
	// PicElem() {};
	virtual void render() = 0;
	virtual void add(float, float, float, float, float, float) = 0;
	virtual void add(Point, Color, float) = 0;
	virtual void add(float, float, float, float, float, float, float, float) = 0;
	virtual void add(Point, Point, Color, float) = 0;
	virtual void replaceLast(float, float) = 0;
	virtual void clear() = 0;
	virtual void eraseLast() {}
//	virtual void add(float, float, Color, float);
//	virtual void 
	virtual ~PicElem() {};
};

class Points : public PicElem
{
public:
	Points(){}	
	void render()
	{
		for(size_t i = 0; i != m_points.size(); ++i)
		{
			glPointSize(m_size[i]);
			glColor3fv(m_colors[i].color);
			glBegin(GL_POINTS);
			glVertex2fv(m_points[i].pnt);
			glEnd();
		}
	}
	virtual void add(float, float, float, float, float, float, float, float) {};
	virtual void add(Point, Point, Color, float) {};
	void add(Point p, Color color, float size)
	{
		m_points.push_back(p);
		m_colors.push_back(color);
		m_size.push_back(size);
	}
	void add(float x, float y, float r, float g, float b, float size)
	{
		add(Point(x,y), Color(r,g,b), size);
	}

	void replaceLast(float x, float y)
	{
		m_points[m_points.size() - 1] = Point(x,y);
	}
	void clear()
	{
		m_points.clear();
		m_colors.clear();
		m_size.clear();
	}
	void eraseLast()
	{
		if(m_points.empty()) return;
		m_points.erase(m_points.end() -1);
		m_colors.erase(m_colors.end() - 1);
		m_size.erase(m_size.end() - 1);
	}
private:
	vector<Point> m_points;
	vector<Color> m_colors;
	vector<float> m_size;
};

class Lines : public PicElem
{
public:
	Lines(){}

	void render()
	{
		
		for(size_t i = 0; i != m_point1.size(); ++i)
		{
			glLineWidth(m_width[i]);
			glColor3fv(m_colors[i].color);
			glBegin(GL_LINES);
			glVertex2fv(m_point1[i].pnt);
			glVertex2fv(m_point2[i].pnt);
			glEnd();
		}
	}
	virtual void add(float, float, float, float, float, float) {}
	virtual void add(Point, Color, float) {};
	void add(Point p1, Point p2, Color color, float width)
	{
		m_point1.push_back(p1);
		m_point2.push_back(p2);
		m_colors.push_back(color);
		m_width.push_back(width);
	}
	void add(float x1, float y1, float x2, float y2, float r, float g, float b, float width)
	{
		add(Point(x1,y1),Point(x2,y2),Color(r,g,b),width);
	}

	void replaceLast(float x, float y)
	{
		m_point2[m_point2.size() - 1] = Point(x,y);
	}

	void clear()
	{
		m_point1.clear();
		m_point2.clear();
		m_colors.clear();
		m_width.clear();
	}
	void eraseLast()
	{
		if(m_point1.empty()) return;
		m_point1.erase(m_point1.end() -1);
		m_point2.erase(m_point2.end() -1);
		m_colors.erase(m_colors.end() - 1);
		m_width.erase(m_width.end() - 1);
	}
private:
	vector<Point> m_point1;
	vector<Point> m_point2;
	vector<Color> m_colors;
	vector<float> m_width;
};

class Polygons : public PicElem
{
public:
	Polygons(){}

	virtual void render()
	{
		glLineWidth(m_width);
		glBegin(GL_POLYGON);
		glColor3fv(m_color.color);
		for(int i=0; i != m_points.size(); ++i)
		{
			glVertex2fv(m_points[i].pnt);
		}
		glEnd();
	}
	virtual void add(float, float, float, float, float, float) {}
	
	virtual void add(float x1, float y1, float x2, float y2, float r, float g, float b, float width) {}

	void add(Point p1, Point p2, Color color, float width) 
	{
		m_points.push_back(p1);
		m_points.push_back(p2);
		m_color = color;
		m_width = width;
	}

	void add(Point p, Color color, float width) 
	{
		m_points.push_back(p);
		m_color = color;
		m_width = width;
	};

	void replaceLast(float x, float y)
	{
		m_points[m_points.size() - 1] = Point(x,y);
	}
	virtual void clear(){}
protected:
	vector<Point> m_points;
	Color m_color;
	float m_width;
};

class Pencil : public Polygons
{
public:
	void render()
	{
		glLineWidth(m_width);
		glBegin(GL_LINE_STRIP);
		glColor3fv(m_color.color);
		for(int i=0; i != m_points.size(); ++i)
		{
			glVertex2fv(m_points[i].pnt);
		}
		glEnd();
	}
};

class Circles : public PicElem
{
public:
	Circles(){}

	void render()
	{
		glLineWidth(m_width);
		glColor3fv(m_color.color);
		glBegin(GL_LINE_STRIP);		
		for(int i=1; i < m_points.size(); ++i)
		{
			glVertex2fv(m_points[i].pnt);
		}
		glEnd();
	}

	virtual void add(float, float, float, float, float, float) {}
	
	virtual void add(float x1, float y1, float x2, float y2, float r, float g, float b, float width) {}

	void add(Point p1, Point p2, Color color, float width) 
	{
		m_points.clear();
		float centerX = (p1.pnt[0]+p2.pnt[0]) / 2;
		float centerY = (p1.pnt[1]+p2.pnt[1]) / 2;
		float radiusX = fabs(p1.pnt[0]-p2.pnt[0]) / 2;
		float radiusY = fabs(p1.pnt[1]-p2.pnt[1]) / 2;
		m_points.push_back(p1);
		for(float f = .0f; f < PI2; f += 0.01f)
		{
			m_points.push_back(Point(radiusX*cos(f) + centerX, radiusY*sin(f) + centerY));
		}
		m_points.push_back(Point(radiusX + centerX, centerY));
		m_color = color;
		m_width = width;
	}

	void add(Point p, Color color, float width) 
	{
//		m_points.clear();
		m_points.push_back(p);
		m_color = color;
		m_width = width;
	};

	void replaceLast(float x, float y)
	{
		if(m_points.size() == 0) return;
		m_points.erase(m_points.begin()+1, m_points.end());
		float centerX = (m_points[0].pnt[0]+x) / 2;
		float centerY = (m_points[0].pnt[1]+y) / 2;
		float radiusX = fabs(m_points[0].pnt[0]-x) / 2;
		float radiusY = fabs(m_points[0].pnt[1]-y) / 2;
		for(float f = .0f; f < PI2; f += 0.01f)
		{
			m_points.push_back(Point(radiusX*cos(f) + centerX, radiusY*sin(f) + centerY));
		}
		m_points.push_back(Point(radiusX + centerX, centerY));
	}
	virtual void clear() {}
private:
	vector<Point> m_points;
	Color m_color;
	float m_width;
};

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Help(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	colorBox(HWND , UINT , WPARAM , LPARAM );
LRESULT CALLBACK	sizeBox(HWND , UINT , WPARAM , LPARAM );
LRESULT CALLBACK	addPointBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	addLineBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	addPolygonBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	addCircleBox(HWND, UINT, WPARAM, LPARAM);

void SceneInit(int w, int h);
void SceneResizeViewport(GLsizei , GLsizei );
void SceneShow(GLvoid);
void EnableOpenGL();
void DisableOpenGL();
bool ChangeResolution(int , int , int );
void addPoints(LPARAM);
void addLines(LPARAM, LPARAM);
void modifyLastLine(LPARAM);
// void addPolygon(LPARAM);
void modifyLastPoint(LPARAM);
void addLastPoint(LPARAM);
#endif // !defined(AFX_XY_H__EEF17F31_8D6E_4A2E_A42E_71220280A745__INCLUDED_)
