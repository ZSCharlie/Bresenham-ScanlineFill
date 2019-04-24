/*绘制一个多边形*/
#include "pch.h"
#include<iostream>
#include<fstream> 
#include<string>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<windows.h>
#include<glut.h>
#include<stdio.h>
#include <fstream>
#include<stdlib.h>

using namespace std;
void setPixel(int x, int y);//画点
void ChangeSize(GLsizei w, GLsizei h);//自动窗口
void BresenhamLine(int x0, int y0, int xEnd, int yEnd);

int prex = 0, prey = 0, curx = 0, cury = 0, stx = 0, sty = 0;
void InitEnvironment()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT );// 当前可写的颜色缓冲
	glPointSize(6);
	glMatrixMode(GL_MODELVIEW);//当前操作的矩阵为“模型视图矩阵”。
	glLoadIdentity();
	gluOrtho2D(0, 400, 0, 400);//类似于映射，实际操作的空间大小
}

void setPixel(int x, int y) {//画点
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	glFlush();
}

void ChangeSize(GLsizei w, GLsizei h) {
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);//根据窗口的实时变化重绘窗口。当前绘制区域
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//当前矩阵均恢复成一个单位矩阵

	//正交投影，分类讨论一下
	if (w <= h)
		glOrtho(0.0f, 400.0f, 0.0f, 400.0f*h / w, 1.0, -1.0);
	else
		glOrtho(0.0f, 400.0f*w / h, 0.0f, 400.0f, 1.0, -1.0);//一个三维空间的投影函数，左下近 和 右上远的坐标，显示正视图。
}

void swap(GLint &a, GLint &b) { GLint t = a; a = b; b = t; }
void BresenhamLine(GLint x1, GLint y1, GLint x2, GLint y2)
{
	//glClear(GL_COLOR_BUFFER_BIT);//清除窗口显示内容

	if (x2 < x1)
	{
		swap(x2, x1);
		swap(y2, y1);
	}
	//画第一个点
	int x, y;
	x = x1;
	y = y1;
	setPixel(x, y);
	//首先处理直线平行坐标轴
	if (y1 == y2)
	{
		//平行x轴
		while (x < x2)
		{
			x++;
			setPixel(x, y);
		}
		return;
	}
	if (x1 == x2)
	{
		//平行y轴
		while (y < y2)
		{
			y++;
			setPixel(x, y);
		}
		return;
	}
	int dx = x2 - x1, dy = y2 - y1;
	int p;
	int twoDy = 2 * dy, twoMinusDx = 2 * (dy - dx), twoDx = 2 * dx, twoMinusDy = 2 * (dx - dy);
	int twoSum = 2 * (dy + dx);
	double k = (double)dy / (double)dx;

	//0<k<1的情况
	if (k<1.0&&k>0.0)
	{
		p = 2 * dy - dx;
		while (x < x2)
		{
			x++;
			if (p < 0)
				p += twoDy;
			else
			{
				y++;
			    p += twoMinusDx;
				
			}
			setPixel(x, y);
		}
	}
	//k>=1的情况
	if (k >= 1.0)
	{
		p = dy;
		while (y < y2)
		{
			y++;
			if (p < 0)
				p += twoDx;
			else
			{
				x++;
				p += twoMinusDy;
			}
			setPixel(x, y);
		}
	}
	//0>k>-1的情况
	if (k > -1 && k < 0)
	{
		p = 2 * dy + dx;
		while (x < x2)
		{
			x++;
			if (p >= 0)
				p += twoDy;
			else
			{
				y--;
				p += twoSum;
				
			}
			setPixel(x, y);
		}
	}
	//k<-1的情况
	if (k <= -1)
	{
		p = 2 * dx - dy;
		while (y > y2)
		{
			y--;
			if (p >= 0)
				p -= twoDx;
			else
			{
				x++;
				p -= twoSum;
			}
			setPixel(x, y);
		}
	}
}


void OnMouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)//右键设置多边形起点
	{
		prex = x, prey = 400 - y;
		//printf("开始\n");
		stx = x, sty = 400 - y;
		printf("%d,%d\n", stx, sty);
		setPixel(x, 400 - y);
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)//滑轮键将起点和终点连起来
	{
		//printf("结束\n");
		printf("%d,%d\n", stx, sty);
		BresenhamLine(stx, sty, curx, cury);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//左键画线
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			printf("%d,%d\n", x, 400 - y);
		}
		glColor3f(0, 0, 0);
		glBegin(GL_POINTS); //设置顶点坐标 单个二维顶点集 
		glVertex2f(x, 400 - y);//不知道为什么获取的Y坐标是垂直对称点
		curx = x, cury = 400 - y;
		BresenhamLine(prex, prey, curx, cury);
		prex = curx, prey = cury;
		glEnd();
		glFlush();

	}
}

void display(void)
{

}

int draw()
{
	//glutInit(&argc, argv);   //初始化GLUT
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(400, 400);
	glutCreateWindow("绘制多边形");

	InitEnvironment();   //初始化
	freopen("F:\\Coordinate.txt", "w", stdout); //获取的坐标输出到自动创建的文件d:\\Coordinate.txt中
	glutMouseFunc(&OnMouse);  //鼠标监听
	glutDisplayFunc(&display);

	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE , GLUT_ACTION_CONTINUE_EXECUTION);
	//glutLeaveMainLoop();本来想用这个跳出循环，但是不支持...
	glutMainLoop();
	
	return 0;
}