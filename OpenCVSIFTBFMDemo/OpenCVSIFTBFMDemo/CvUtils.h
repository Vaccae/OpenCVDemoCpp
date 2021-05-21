#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class CvUtils
{
public:
	//显示窗口设置  
    //参数  img 显示的图像源，
    //      winname 显示的窗口名称,
    //      pointx  显示的坐标x
    //      pointy  显示的坐标y
	static void SetShowWindow(Mat img, string winname, int pointx, int pointy);

    //设置图像显示大小，根据设置的默认最大宽高度，超出的话自动进行缩放
    //参数  frame  源图像
    //      maxwidth 默认的最大宽度
    //      maxheight 默认的最大高度
    static void MatResize(Mat& frame, int maxwidth = 800, int maxheight = 600);

    //vecpt和vecpt2f转换
    static vector<Point> Vecpt2fToVecpt(vector<Point2f>& vecpt2f);

    static vector<Point2f> VecptToVecpt2f(vector<Point>& vecpt);
};

