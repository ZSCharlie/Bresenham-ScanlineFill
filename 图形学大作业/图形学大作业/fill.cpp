/*填充*/
#include "pch.h"
#include"draw.h"
#include <stdio.h> 
#include <math.h> 
#include <glut.h> 
#define maxHt 400
#define maxWd 400 
#define maxVer 10000 

FILE *fp;

// 从左下开始填充
typedef struct edgebucket
{
	int ymax;   //这条边最大Y坐标
	float xofymin;  //最低边的X坐标 
	float slopeinverse;
}EdgeBucket;

typedef struct edgetabletup
{
	// 扫描线编号

	int countEdgeBucket;    //编号
	EdgeBucket buckets[maxVer];
}EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;


//扫描线
void initEdgeTable()
{
	int i;
	for (i = 0; i < maxHt; i++)
	{
		EdgeTable[i].countEdgeBucket = 0;
	}
	//初始化
	ActiveEdgeTuple.countEdgeBucket = 0;
}

//用于打印边表的函数 
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


/*插入排序*/
void insertionSort(EdgeTableTuple *ett)
{
	int i, j;
	EdgeBucket temp;

	for (i = 1; i < ett->countEdgeBucket; i++)//ett里所有边 
	{
		//取出一条边 
		temp.ymax = ett->buckets[i].ymax;
		temp.xofymin = ett->buckets[i].xofymin;
		temp.slopeinverse = ett->buckets[i].slopeinverse;
		j = i - 1;

		//插入排序
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

//将一条新边插入到活动边表的函数 
void storeEdgeInTuple(EdgeTableTuple *receiver, int ym, int xm, float slopInv)
{

	(receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
	(receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
	(receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;

	
	insertionSort(receiver);

	(receiver->countEdgeBucket)++;


}

//将坐标转化为边 插入到 活动边表
void storeEdgeInTable(int x1, int y1, int x2, int y2)
{
	float m, minv;
	int ymaxTS, xwithyminTS, scanline;  

	if (x2 == x1) //一条垂线 
	{
		minv = 0.000000;
	}
	else
	{
		m = ((float)(y2 - y1)) / ((float)(x2 - x1));	//斜率k

		if (y2 == y1)	//一条水平线
			return;

		minv = (float)1.0 / m;
		//printf("\nSlope string for %d %d & %d %d: %f", x1, y1, x2, y2, minv);
	}

	//将线的方向规定为  y值小→y值大 小的Y值作为扫描线编号 
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

// 在活动边表中移除一条非活动边  ymax == yy
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

//更新ymin  xofymin+=1/k  即向上扫描的过程 
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

		removeEdgeByYmax(&ActiveEdgeTuple, i);//移除 

		insertionSort(&ActiveEdgeTuple);//排序 

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
					//BresenhamLine(x1, i, x2, i);用bresenham太慢了
					glEnd();
					glFlush();

					// printf("\nLine drawn from %d,%d to %d,%d",x1,i,x2,i); 
				}

			}

			j++;
		}


		updatexbyslopeinv(&ActiveEdgeTuple);
	}


	printf("填充完成\n");

}


void myInit(void)
{

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(0, maxHt, 0, maxWd);
	glClear(GL_COLOR_BUFFER_BIT);
}

void drawPoly()//根据坐标画轮廓线
{
	
	glColor3f(1.0f, 1.0f, 0.0f);
	int count = 0, x1, y1, x2, y2;
	rewind(fp);
	while (!feof(fp))
	{
		count++;
		if (count > 2)//第一个点之后的点
		{
			x1 = x2;
			y1 = y2;
			count = 2;
		}
		if (count == 1)//第一个点
		{
			fscanf(fp, "%d,%d", &x1, &y1);
		}
		else
		{
			fscanf(fp, "%d,%d", &x2, &y2);
			//printf("\n%d,%d", x2, y2);
			BresenhamLine(x1, y1, x2, y2);

			glEnd();
			storeEdgeInTable(x1, y1, x2, y2);//存入边表


			glFlush();
		}
	}


}

void Draw(void)//填充
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
		printf("无法打开文件");
		return 0;
	}
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(maxHt, maxWd);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("扫面线填充图形");
	myInit();
	glutDisplayFunc(Draw);

	glutMainLoop();
	fclose(fp);
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
