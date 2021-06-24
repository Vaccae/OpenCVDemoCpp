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
    static vector<Point> Pt2fsToPts(vector<Point2f>& vecpt2f);
    static vector<Point2f> PtsToPt2fs(vector<Point>& vecpt);


    //求图像像素值的中位数
    static int GetMatMidVal(Mat& img);

    //大津法返回阈值
    static int GetMatOTSU(Mat& img);

    //求图像自动阈值的最小最大数
    //calctype 0-中位数  1-大津法求阈值
    static void GetMatMinMaxThreshold(Mat& img, int& minval, int& maxval, 
        int calctype = 0, float sigma = 0.5);


    //根据中心点找四角最远的点
    static void GetRectPoints(Point2f vetPoints[], Point2f center, vector<Point> convex);
    //根据最小矩形点找最近的四边形点
    static void GetPointsFromRect(Point2f vetPoints[], Point2f rectPoints[], vector<Point> convex);

    //采用重新定义点做直线拟合后找到的对应点
    static void GetPointsFromFitline(Point2f newPoints[], Point2f vetPoints[], Point2f rectPoints[], float dist = 15.0f);

    //求两条直线的交点
    static Point2f GetCrossPoint(Vec4f Line1, Vec4f Line2);

    //检测旋转矩形是否超出图像边界
    static bool CheckRectBorder(Mat& img, RotatedRect rect);

    //排序旋转矩形坐标点
    static void SortRotatedRectPoints(Point2f vetPoints[], RotatedRect rect, int flag = 0);

    //计算距离
    static float CalcPointDistance(Point2f point1, Point2f point2);
};

