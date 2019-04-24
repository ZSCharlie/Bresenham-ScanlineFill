/*���*/
#include "pch.h"
#include"draw.h"
#include <stdio.h> 
#include <math.h> 
#include <glut.h> 
#define maxHt 400
#define maxWd 400 
#define maxVer 10000 

FILE *fp;

// �����¿�ʼ���
typedef struct edgebucket
{
	int ymax;   //���������Y����
	float xofymin;  //��ͱߵ�X���� 
	float slopeinverse;
}EdgeBucket;

typedef struct edgetabletup
{
	// ɨ���߱��

	int countEdgeBucket;    //���
	EdgeBucket buckets[maxVer];
}EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;


//ɨ����
void initEdgeTable()
{
	int i;
	for (i = 0; i < maxHt; i++)
	{
		EdgeTable[i].countEdgeBucket = 0;
	}
	//��ʼ��
	ActiveEdgeTuple.countEdgeBucket = 0;
}

//���ڴ�ӡ�߱�ĺ��� 
/*void printTuple(EdgeTableTuple *tup)
{
	int j;

	if (tup->countEdgeBucket)
		printf("\nCount %d-----\n", tup->countEdgeBucket);

	for (j = 0; j < tup->countEdgeBucket; j++)
	{
		printf(" %d+%.2f+%.2f",
			tup->buckets[j].ymax, tup->buckets[j].xofymin, tup->buckets[j].slopeinverse);
	}
}*/

/*void printTable()
{
	int i, j;

	for (i = 0; i < maxHt; i++)
	{
		if (EdgeTable[i].countEdgeBucket)
			printf("\nScanline %d", i);

		printTuple(&EdgeTable[i]);
	}
}*/


/*��������*/
void insertionSort(EdgeTableTuple *ett)
{
	int i, j;
	EdgeBucket temp;

	for (i = 1; i < ett->countEdgeBucket; i++)//ett�����б� 
	{
		//ȡ��һ���� 
		temp.ymax = ett->buckets[i].ymax;
		temp.xofymin = ett->buckets[i].xofymin;
		temp.slopeinverse = ett->buckets[i].slopeinverse;
		j = i - 1;

		//��������
		while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0))
		{
			ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
			ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
			ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
			j = j - 1;
		}
		ett->buckets[j + 1].ymax = temp.ymax;
		ett->buckets[j + 1].xofymin = temp.xofymin;
		ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
	}
}

//��һ���±߲��뵽��߱�ĺ��� 
void storeEdgeInTuple(EdgeTableTuple *receiver, int ym, int xm, float slopInv)
{

	(receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
	(receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
	(receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;

	
	insertionSort(receiver);

	(receiver->countEdgeBucket)++;


}

//������ת��Ϊ�� ���뵽 ��߱�
void storeEdgeInTable(int x1, int y1, int x2, int y2)
{
	float m, minv;
	int ymaxTS, xwithyminTS, scanline;  

	if (x2 == x1) //һ������ 
	{
		minv = 0.000000;
	}
	else
	{
		m = ((float)(y2 - y1)) / ((float)(x2 - x1));	//б��k

		if (y2 == y1)	//һ��ˮƽ��
			return;

		minv = (float)1.0 / m;
		//printf("\nSlope string for %d %d & %d %d: %f", x1, y1, x2, y2, minv);
	}

	//���ߵķ���涨Ϊ  yֵС��yֵ�� С��Yֵ��Ϊɨ���߱�� 
	if (y1 > y2)
	{
		scanline = y2;
		ymaxTS = y1;
		xwithyminTS = x2;
	}
	else
	{
		scanline = y1;
		ymaxTS = y2;
		xwithyminTS = x1;
	}
	storeEdgeInTuple(&EdgeTable[scanline], ymaxTS, xwithyminTS, minv);


}

// �ڻ�߱����Ƴ�һ���ǻ��  ymax == yy
void removeEdgeByYmax(EdgeTableTuple *Tup, int yy)
{
	int i, j;
	for (i = 0; i < Tup->countEdgeBucket; i++)
	{
		if (Tup->buckets[i].ymax == yy)
		{
			//printf("\nRemoved at %d", yy);

			for (j = i; j < Tup->countEdgeBucket - 1; j++)
			{
				Tup->buckets[j].ymax = Tup->buckets[j + 1].ymax;
				Tup->buckets[j].xofymin = Tup->buckets[j + 1].xofymin;
				Tup->buckets[j].slopeinverse = Tup->buckets[j + 1].slopeinverse;
			}
			Tup->countEdgeBucket--;
			i--;
		}
	}
}

//����ymin  xofymin+=1/k  ������ɨ��Ĺ��� 
void updatexbyslopeinv(EdgeTableTuple *Tup)
{
	int i;

	for (i = 0; i < Tup->countEdgeBucket; i++)
	{
		(Tup->buckets[i]).xofymin = (Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
	}
}


void ScanlineFill()
{


	int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;

	for (i = 0; i < maxHt; i++)
	{

		for (j = 0; j < EdgeTable[i].countEdgeBucket; j++)
		{
			storeEdgeInTuple(&ActiveEdgeTuple, EdgeTable[i].buckets[j].
				ymax, EdgeTable[i].buckets[j].xofymin,
				EdgeTable[i].buckets[j].slopeinverse);
		}
//		printTuple(&ActiveEdgeTuple);

		removeEdgeByYmax(&ActiveEdgeTuple, i);//�Ƴ� 

		insertionSort(&ActiveEdgeTuple);//���� 

//		printTuple(&ActiveEdgeTuple);

		j = 0;
		FillFlag = 0;
		coordCount = 0;
		x1 = 0;
		x2 = 0;
		ymax1 = 0;
		ymax2 = 0;
		while (j < ActiveEdgeTuple.countEdgeBucket)
		{
			if (coordCount % 2 == 0)
			{
				x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
				ymax1 = ActiveEdgeTuple.buckets[j].ymax;
				if (x1 == x2)
				{
					if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
					{
						x2 = x1;
						ymax2 = ymax1;
					}

					else
					{
						coordCount++;
					}
				}

				else
				{
					coordCount++;
				}
			}
			else
			{
				x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
				ymax2 = ActiveEdgeTuple.buckets[j].ymax;

				FillFlag = 0;

				if (x1 == x2)
				{
					if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
					{
						x1 = x2;
						ymax1 = ymax2;
					}
					else
					{
						coordCount++;
						FillFlag = 1;
					}
				}
				else
				{
					coordCount++;
					FillFlag = 1;
				}


				if (FillFlag)
				{
					glColor3f(1.0f, 1.0f, 0.0f);

					glBegin(GL_LINES);
					glVertex2i(x1, i);
					glVertex2i(x2, i);
					//BresenhamLine(x1, i, x2, i);��bresenham̫����
					glEnd();
					glFlush();

					// printf("\nLine drawn from %d,%d to %d,%d",x1,i,x2,i); 
				}

			}

			j++;
		}


		updatexbyslopeinv(&ActiveEdgeTuple);
	}


	printf("������\n");

}


void myInit(void)
{

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(0, maxHt, 0, maxWd);
	glClear(GL_COLOR_BUFFER_BIT);
}

void drawPoly()//�������껭������
{
	
	glColor3f(1.0f, 1.0f, 0.0f);
	int count = 0, x1, y1, x2, y2;
	rewind(fp);
	while (!feof(fp))
	{
		count++;
		if (count > 2)//��һ����֮��ĵ�
		{
			x1 = x2;
			y1 = y2;
			count = 2;
		}
		if (count == 1)//��һ����
		{
			fscanf(fp, "%d,%d", &x1, &y1);
		}
		else
		{
			fscanf(fp, "%d,%d", &x2, &y2);
			//printf("\n%d,%d", x2, y2);
			BresenhamLine(x1, y1, x2, y2);

			glEnd();
			storeEdgeInTable(x1, y1, x2, y2);//����߱�


			glFlush();
		}
	}


}

void Draw(void)//���
{
	initEdgeTable();
	drawPoly();
//	printf("\nTable");
	//printTable();

	ScanlineFill();
}
int fill()
{
	fp = fopen("F:\\Coordinate.txt", "r");
	if (fp == NULL)
	{
		printf("�޷����ļ�");
		return 0;
	}
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(maxHt, maxWd);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("ɨ�������ͼ��");
	myInit();
	glutDisplayFunc(Draw);

	glutMainLoop();
	fclose(fp);
}


// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ������ʾ: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
