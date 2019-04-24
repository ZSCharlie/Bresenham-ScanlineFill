/*����һ�������*/
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
void setPixel(int x, int y);//����
void ChangeSize(GLsizei w, GLsizei h);//�Զ�����
void BresenhamLine(int x0, int y0, int xEnd, int yEnd);

int prex = 0, prey = 0, curx = 0, cury = 0, stx = 0, sty = 0;
void InitEnvironment()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT );// ��ǰ��д����ɫ����
	glPointSize(6);
	glMatrixMode(GL_MODELVIEW);//��ǰ�����ľ���Ϊ��ģ����ͼ���󡱡�
	glLoadIdentity();
	gluOrtho2D(0, 400, 0, 400);//������ӳ�䣬ʵ�ʲ����Ŀռ��С
}

void setPixel(int x, int y) {//����
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	glFlush();
}

void ChangeSize(GLsizei w, GLsizei h) {
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);//���ݴ��ڵ�ʵʱ�仯�ػ洰�ڡ���ǰ��������
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//��ǰ������ָ���һ����λ����

	//����ͶӰ����������һ��
	if (w <= h)
		glOrtho(0.0f, 400.0f, 0.0f, 400.0f*h / w, 1.0, -1.0);
	else
		glOrtho(0.0f, 400.0f*w / h, 0.0f, 400.0f, 1.0, -1.0);//һ����ά�ռ��ͶӰ���������½� �� ����Զ�����꣬��ʾ����ͼ��
}

void swap(GLint &a, GLint &b) { GLint t = a; a = b; b = t; }
void BresenhamLine(GLint x1, GLint y1, GLint x2, GLint y2)
{
	//glClear(GL_COLOR_BUFFER_BIT);//���������ʾ����

	if (x2 < x1)
	{
		swap(x2, x1);
		swap(y2, y1);
	}
	//����һ����
	int x, y;
	x = x1;
	y = y1;
	setPixel(x, y);
	//���ȴ���ֱ��ƽ��������
	if (y1 == y2)
	{
		//ƽ��x��
		while (x < x2)
		{
			x++;
			setPixel(x, y);
		}
		return;
	}
	if (x1 == x2)
	{
		//ƽ��y��
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

	//0<k<1�����
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
	//k>=1�����
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
	//0>k>-1�����
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
	//k<-1�����
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
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)//�Ҽ����ö�������
	{
		prex = x, prey = 400 - y;
		//printf("��ʼ\n");
		stx = x, sty = 400 - y;
		printf("%d,%d\n", stx, sty);
		setPixel(x, 400 - y);
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)//���ּ��������յ�������
	{
		//printf("����\n");
		printf("%d,%d\n", stx, sty);
		BresenhamLine(stx, sty, curx, cury);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//�������
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			printf("%d,%d\n", x, 400 - y);
		}
		glColor3f(0, 0, 0);
		glBegin(GL_POINTS); //���ö������� ������ά���㼯 
		glVertex2f(x, 400 - y);//��֪��Ϊʲô��ȡ��Y�����Ǵ�ֱ�ԳƵ�
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
	//glutInit(&argc, argv);   //��ʼ��GLUT
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(400, 400);
	glutCreateWindow("���ƶ����");

	InitEnvironment();   //��ʼ��
	freopen("F:\\Coordinate.txt", "w", stdout); //��ȡ������������Զ��������ļ�d:\\Coordinate.txt��
	glutMouseFunc(&OnMouse);  //������
	glutDisplayFunc(&display);

	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE , GLUT_ACTION_CONTINUE_EXECUTION);
	//glutLeaveMainLoop();���������������ѭ�������ǲ�֧��...
	glutMainLoop();
	
	return 0;
}