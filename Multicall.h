#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <queue>
using namespace std;
using namespace cv;
typedef struct { //for openset
	vector<unsigned char> sub_openset;
	vector<unsigned char> customer_on_board;
}openset_data;
typedef struct //for breath width search
{
	unsigned char node; //현재 위치
	double score;
	double time;
	vector<unsigned char> pre_nodes; //어디서 왔는지
}q_data;
class Multicall {
private:
	int n;
	Mat img;
	Point2i mypos;
	vector<Point2i> customer_s; // 1~n : 승객 출발지
	vector<Point2i> customer_d; // 1~n : 승객 목적지
public:
	Multicall(int n, int size);
	void plot(const vector<double>& score);
	vector<double> getscore();
	Mat getdistanceandscore();
	bool isfeasible(int n, int visit_node, const vector<unsigned char> customer_on_board, int capacity);
	void openset_update(vector<openset_data>& openset, const unsigned char& visit_node, const int& n, const int capacity, queue<q_data>& q);
	vector<unsigned char> fullsearch(const vector<double>& score, const Mat& D_time, const int& capacity, vector<unsigned char>& customer_on_board);
	void drawline(vector<unsigned char> storage); // result : 1 channel = 1 or 0 2 channel = visit order
	
};