#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"

using namespace std;
using namespace cv;

Mat src;
Mat srccopy; //���ڿ�������Դͼ��
string showsrc = "ͼ��";

//���ص�����
void onMouse(int event, int x, int y, int flags, void* ustc); 

//RGB��ɫת��ΪHEX
string rgb2hex(int r, int g, int b, bool ishead = false);


int main(int argc, char** argv) {

	src = imread("E:/DCIM/6.jpg");   
	CvUtils::MatResize(src);
	CvUtils::SetShowWindow(src, showsrc, 500, 20);
	imshow(showsrc, src);

	//���������Ӱ�¼�
	setMouseCallback(showsrc, onMouse);


	waitKey(0);
	return 0;

}


void onMouse(int event, int x, int y, int flags, void* ustc)
{
	//����������
	if (event == EVENT_LBUTTONUP)
	{
		srccopy = src.clone();

		//��ȡ���λ�õ���ɫ���˴�ע��y��x��λ��˳��
		Scalar color = srccopy.at<Vec3b>(y, x);
		int b = color[0];
		int g = color[1];
		int r = color[2];


		string hexstr = rgb2hex(r, g, b, true);

		string rgbtext = "RGB(" + to_string(r) + "," + to_string(g) + "," + to_string(b) + ")";
		string hextext = "HEX:" + hexstr;


		//��ͼ���ϻ������λ��
		circle(srccopy, Point(x, y), 2, Scalar(0, 255, 255));

		//�����ʾ����
		putText(srccopy, rgbtext, Point(x + 5, y + 5), 1, 1.2, Scalar(0, 255, 255));
		putText(srccopy, hextext, Point(x + 5, y + 35), 1, 1.2, Scalar(0, 255, 255));

		imshow(showsrc, srccopy);
	}
}

string rgb2hex(int r, int g, int b, bool ishead)
{
	stringstream hexstr;
	if (ishead) hexstr << "#";
	hexstr << hex << (r << 16 | g << 8 | b);

	cout << "rgb:" << r << "," << g << "," << b << "  hex:" << hexstr.str() << endl;
	return hexstr.str();
}
