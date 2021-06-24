#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class CvUtils
{
public:
	//��ʾ��������  
    //����  img ��ʾ��ͼ��Դ��
    //      winname ��ʾ�Ĵ�������,
    //      pointx  ��ʾ������x
    //      pointy  ��ʾ������y
	static void SetShowWindow(Mat img, string winname, int pointx, int pointy);

    //����ͼ����ʾ��С���������õ�Ĭ������߶ȣ������Ļ��Զ���������
    //����  frame  Դͼ��
    //      maxwidth Ĭ�ϵ������
    //      maxheight Ĭ�ϵ����߶�
    static void MatResize(Mat& frame, int maxwidth = 800, int maxheight = 600);

    //vecpt��vecpt2fת��
    static vector<Point> Pt2fsToPts(vector<Point2f>& vecpt2f);
    static vector<Point2f> PtsToPt2fs(vector<Point>& vecpt);


    //��ͼ������ֵ����λ��
    static int GetMatMidVal(Mat& img);

    //��򷨷�����ֵ
    static int GetMatOTSU(Mat& img);

    //��ͼ���Զ���ֵ����С�����
    //calctype 0-��λ��  1-�������ֵ
    static void GetMatMinMaxThreshold(Mat& img, int& minval, int& maxval, 
        int calctype = 0, float sigma = 0.5);


    //�������ĵ����Ľ���Զ�ĵ�
    static void GetRectPoints(Point2f vetPoints[], Point2f center, vector<Point> convex);
    //������С���ε���������ı��ε�
    static void GetPointsFromRect(Point2f vetPoints[], Point2f rectPoints[], vector<Point> convex);

    //�������¶������ֱ����Ϻ��ҵ��Ķ�Ӧ��
    static void GetPointsFromFitline(Point2f newPoints[], Point2f vetPoints[], Point2f rectPoints[], float dist = 15.0f);

    //������ֱ�ߵĽ���
    static Point2f GetCrossPoint(Vec4f Line1, Vec4f Line2);

    //�����ת�����Ƿ񳬳�ͼ��߽�
    static bool CheckRectBorder(Mat& img, RotatedRect rect);

    //������ת���������
    static void SortRotatedRectPoints(Point2f vetPoints[], RotatedRect rect, int flag = 0);

    //�������
    static float CalcPointDistance(Point2f point1, Point2f point2);
};

