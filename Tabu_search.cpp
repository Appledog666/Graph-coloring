// Map1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <ctime>
#include<string.h>
#define TRUE 1
/*全局变量定义*/
int k; //颜色种数
int iter = 0; //步数
int noimprove = 0;//没有更优解的步数
int f = 0;   //目标函数值
int best_f = 0;  //历史最小冲突值
int c_node;//要修改的结点
int c_color;//要修改的编号
int Sol[600]; //当前解
int Best_Sol[600];  //历史最优解
int Adjacent_Color_Table[600][600]; //邻接表
int solution[2000][2];  //邻域解
int tsolution[2000][2];//禁忌的邻域解
int TabuTenure[600][600];//禁忌表
int delt;//δ值
int flag = 0;
int Adjacent_graph_Table[600][600]; //图的邻接表
int arc[600];   //每个点的边数
int N, edge;//图的当前顶点数和弧数
int MAXiter;  //最大连续无历史最优解更新次数
char file[20]; //文件名
			   /*------函数定义-------*/
int LoadGraph(void);
void Initialization(void);
void Findmove(void);
void Makemove(void);
/*---------*/
int main(void) {
	int i;
	clock_t start, finish;
	double total_time;
	if (!LoadGraph())
		return 0;
	if (!strcmp(file, "DSJC125.1.col")) {
		k = 20;
		MAXiter = 20000;
	}
	if (!strcmp(file, "DSJC250.1.col")) {
		k = 20;
		MAXiter = 50000;
	}
	if (!strcmp(file, "DSJC250.5.col")) {
		k = 50;
		MAXiter = 8000000;
	}
	if (!strcmp(file, "DSJC250.9.col")) {
		k = 90;
		MAXiter = 500000;
	}
	if (!strcmp(file, "DSJC500.1.col")) {
		k = 20;
		MAXiter = 80000000;
	}
	if (!strcmp(file, "DSJC500.5.col")) {
		k = 60;
		MAXiter = 100000000;
	}
	while (k) {
		start = clock();
		Initialization();  //对解进行初始化
		while (best_f) {
			iter++;
			Findmove();
			if (!flag)
				Makemove();
			if (best_f <= f) {   //连续无最优解更新MAXiter次则跳出循环
				noimprove++;
				if (MAXiter<noimprove)
					break;
			}
			else
				noimprove = 0;
		}
		if (MAXiter<noimprove || flag == 1) {
			printf("Cannot find better solution!");
			printf("solution K: %d", k + 1);
			break;
		}
		finish = clock();
		total_time = (double)(finish - start) / CLOCKS_PER_SEC;
		printf("K:% d,Iter: %d,time :%f s ", k, iter, total_time);
		for (i = 0; i < N; i++)
			printf("%d ", Sol[i]);
		printf("\n");
		k--;
		iter = 0;  //步数归零重新进行迭代
	}
	return 0;

}
/*------图的读取函数-------*/
int LoadGraph() {   //读取的为无向图
	FILE *fp;
	int i = 0;
	int j = 0;
	int data1, data2;
	char c;
	printf("Please enter the file name\n");
	scanf("%s", &file);
	getchar();
	if ((fp = fopen(file, "rb")) == NULL) {
		printf("Fail to open the file!\n");
		return 0;
	}
	else
		printf("Success to open the file!\n");
	fscanf(fp, "%d ", &N);    //读取点的个数
	fscanf(fp, "%d ", &edge); //读取边的条数
	for (i = 0; i<N; i++) {
		arc[i] = 0;
	}
	i = 0;
	fscanf(fp, "%c ""%d ", &c, &data1);
	while (data1) {
		i = data1 - 1;
		fscanf(fp, "%d ", &data2);
		Adjacent_graph_Table[i][arc[i]] = data2;
		arc[i]++;
		j = data2 - 1;
		Adjacent_graph_Table[j][arc[j]] = data1;
		arc[j]++;
		fscanf(fp, "%c ""%d ", &c, &data1);
	}
	return TRUE;
}
void Initialization(void) {
	srand(time(NULL));
	int i;
	int j;
	int color;
	int color2;
	int *ad_node;
	for (i = 0; i<N; i++) {
		Sol[i] = rand() % k;     //初始化随机解
		Best_Sol[i] = Sol[i];
		for (j = 0; j<k; j++) {
			Adjacent_Color_Table[i][j] = 0;
			TabuTenure[i][j] = 0;
		}
	}
	for (i = 0; i<N; i++) {
		ad_node = Adjacent_graph_Table[i];
		color2 = Sol[i];
		for (j = 0; j<arc[i]; j++) {
			color = Sol[ad_node[j] - 1];
			Adjacent_Color_Table[i][color]++;
		}
	}
	for (i = 0; i<N; i++) {   //计算目标函数的值
		color = Sol[i];
		f = f + Adjacent_Color_Table[i][color];
	}
	f = f / 2;
	best_f = f;
}
void Findmove()
{
	flag = 0;
	delt = 500000;  //随机设定一个很大的值
	int tdelt = 500000;//禁忌解的δ值
	int color; //当前颜色
	int improve;//冲突结点改变的值
	int step = 0;
	int tstep = 0;
	int i, j;
	int *ad_node; //邻接表指针
	int *tabu_node;   //禁忌表指针
	for (i = 0; i<N; i++) {
		ad_node = Adjacent_Color_Table[i];
		tabu_node = TabuTenure[i];
		color = Sol[i];
		if (ad_node[color]>0) {
			for (j = 0; j<k; j++) {
				if (j != color) {        //颜色不与当前结点相同
					improve = ad_node[j] - ad_node[color];
					if (tabu_node[j] <= iter) {   //若为非禁忌解
						if (improve<delt) {
							step = 0;     //若找到更优的解，更新
							delt = improve;
							solution[step][0] = i;
							solution[step][1] = j;
							step++;
						}
						if (improve == delt) {
							solution[step][0] = i;
							solution[step][1] = j;
							step++;
						}
					}
					else {               //若为禁忌解
						if (improve<tdelt) {
							tstep = 0;  //若找到更优的禁忌解，更新
							tdelt = improve;
							tsolution[tstep][0] = i;
							tsolution[tstep][1] = j;
							tstep++;
						}
						if (improve == tdelt) {
							tsolution[tstep][0] = i;
							tsolution[tstep][1] = j;
							tstep++;
						}
					}
				}
			}
		}
	}
	if (tdelt<delt && (f + tdelt)<best_f) {     //满足aspiration，解禁
		delt = tdelt;
		improve = rand() % tstep;//相等的delt中随机选择一个
		c_node = tsolution[improve][0]; //要改变的结点
		c_color = tsolution[improve][1];  //要改变的颜色
	}
	else if (step) {  //若找到了非禁忌的解
		improve = rand() % step;
		c_node = solution[improve][0];
		c_color = solution[improve][1];
	}
	else {
		flag = 1;     //若出现找不到最优解或者禁忌的解的情况，返回
		return;
	}
}
void Makemove() {
	int i;
	int tt;  //禁忌步数
	int b_color;//更换之前的颜色
	int *ad_node;
	b_color = Sol[c_node];
	f = f + delt;  //更新目标函数值
	if (f<best_f) {
		best_f = f;   //更新历史最优解
		Best_Sol[c_node] = c_color;   //修改当前历史最优解
	}
	Sol[c_node] = c_color;   //修改当前解
	ad_node = Adjacent_graph_Table[c_node];
	for (i = 0; i<arc[c_node]; i++) {
		Adjacent_Color_Table[ad_node[i] - 1][b_color]--;
		Adjacent_Color_Table[ad_node[i] - 1][c_color]++;
	}
	tt = iter + f + rand() % 10 + 1;  //设置禁忌步数(后面为1-10的随机值)
	TabuTenure[c_node][b_color] = tt;  //更新禁忌表
}
