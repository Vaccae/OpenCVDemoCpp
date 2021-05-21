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
    static vector<Point> Vecpt2fToVecpt(vector<Point2f>& vecpt2f);

    static vector<Point2f> VecptToVecpt2f(vector<Point>& vecpt);
};

