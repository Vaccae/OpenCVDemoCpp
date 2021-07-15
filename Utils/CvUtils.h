#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>


class CvUtils
{
public:
	//��ʾ��������  
    //����  img ��ʾ��ͼ��Դ��
    //      winname ��ʾ�Ĵ�������,
    //      pointx  ��ʾ������x
    //      pointy  ��ʾ������y
	static void SetShowWindow(cv::Mat img, std::string winname, int pointx, int pointy);

    //����ͼ����ʾ��С���������õ�Ĭ������߶ȣ������Ļ��Զ���������
    //����  frame  Դͼ��
    //      maxwidth Ĭ�ϵ������
    //      maxheight Ĭ�ϵ����߶�
    static void MatResize(cv::Mat& frame, int maxwidth = 800, int maxheight = 600);

    //vecpt��vecpt2fת��
    static std::vector<cv::Point> Pt2fsToPts(std::vector<cv::Point2f>& vecpt2f);
    static std::vector<cv::Point2f> PtsToPt2fs(std::vector<cv::Point>& vecpt);


    //��ͼ������ֵ����λ��
    static int GetMatMidVal(cv::Mat& img);

    //��򷨷�����ֵ
    static int GetMatOTSU(cv::Mat& img);

    //��ͼ���Զ���ֵ����С�����
    //calctype 0-��λ��  1-�������ֵ
    static void GetMatMinMaxThreshold(cv::Mat& img, int& minval, int& maxval, 
        int calctype = 0, float sigma = 0.5);


    //�������ĵ����Ľ���Զ�ĵ�
    static void GetRectPoints(cv::Point2f vetPoints[], cv::Point2f center, std::vector<cv::Point> convex);
    //������С���ε���������ı��ε�
    static void GetPointsFromRect(cv::Point2f vetPoints[], cv::Point2f rectPoints[], std::vector<cv::Point> convex);

    //�������¶������ֱ����Ϻ��ҵ��Ķ�Ӧ��
    static void GetPointsFromFitline(cv::Point2f newPoints[], cv::Point2f vetPoints[], cv::Point2f rectPoints[], float dist = 15.0f);

    //������ֱ�ߵĽ���
    static cv::Point2f GetCrossPoint(cv::Vec4f Line1, cv::Vec4f Line2);

    //�����ת�����Ƿ񳬳�ͼ��߽�
    static bool CheckRectBorder(cv::Mat& img, cv::RotatedRect rect);

    //������ת���������
    static void SortRotatedRectPoints(cv::Point2f vetPoints[], cv::RotatedRect rect, int flag = 0);

    //�������
    static float CalcPointDistance(cv::Point2f point1, cv::Point2f point2);

    //����н�
    static double CalcAngle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
};

