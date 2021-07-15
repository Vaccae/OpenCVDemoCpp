#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>


class CvUtils
{
public:
	//显示窗口设置  
    //参数  img 显示的图像源，
    //      winname 显示的窗口名称,
    //      pointx  显示的坐标x
    //      pointy  显示的坐标y
	static void SetShowWindow(cv::Mat img, std::string winname, int pointx, int pointy);

    //设置图像显示大小，根据设置的默认最大宽高度，超出的话自动进行缩放
    //参数  frame  源图像
    //      maxwidth 默认的最大宽度
    //      maxheight 默认的最大高度
    static void MatResize(cv::Mat& frame, int maxwidth = 800, int maxheight = 600);

    //vecpt和vecpt2f转换
    static std::vector<cv::Point> Pt2fsToPts(std::vector<cv::Point2f>& vecpt2f);
    static std::vector<cv::Point2f> PtsToPt2fs(std::vector<cv::Point>& vecpt);


    //求图像像素值的中位数
    static int GetMatMidVal(cv::Mat& img);

    //大津法返回阈值
    static int GetMatOTSU(cv::Mat& img);

    //求图像自动阈值的最小最大数
    //calctype 0-中位数  1-大津法求阈值
    static void GetMatMinMaxThreshold(cv::Mat& img, int& minval, int& maxval, 
        int calctype = 0, float sigma = 0.5);


    //根据中心点找四角最远的点
    static void GetRectPoints(cv::Point2f vetPoints[], cv::Point2f center, std::vector<cv::Point> convex);
    //根据最小矩形点找最近的四边形点
    static void GetPointsFromRect(cv::Point2f vetPoints[], cv::Point2f rectPoints[], std::vector<cv::Point> convex);

    //采用重新定义点做直线拟合后找到的对应点
    static void GetPointsFromFitline(cv::Point2f newPoints[], cv::Point2f vetPoints[], cv::Point2f rectPoints[], float dist = 15.0f);

    //求两条直线的交点
    static cv::Point2f GetCrossPoint(cv::Vec4f Line1, cv::Vec4f Line2);

    //检测旋转矩形是否超出图像边界
    static bool CheckRectBorder(cv::Mat& img, cv::RotatedRect rect);

    //排序旋转矩形坐标点
    static void SortRotatedRectPoints(cv::Point2f vetPoints[], cv::RotatedRect rect, int flag = 0);

    //计算距离
    static float CalcPointDistance(cv::Point2f point1, cv::Point2f point2);

    //计算夹角
    static double CalcAngle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
};

