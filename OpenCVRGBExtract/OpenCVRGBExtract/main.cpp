#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"

using namespace std;
using namespace cv;

Mat src;
Mat srccopy; //用于拷贝出的源图像
string showsrc = "图像";

//鼠标回调函数
void onMouse(int event, int x, int y, int flags, void* ustc); 

//RGB颜色转换为HEX
string rgb2hex(int r, int g, int b, bool ishead = false);


int main(int argc, char** argv) {

	src = imread("E:/DCIM/6.jpg");   
	CvUtils::MatResize(src);
	CvUtils::SetShowWindow(src, showsrc, 500, 20);
	imshow(showsrc, src);

	//设置鼠标响影事件
	setMouseCallback(showsrc, onMouse);


	waitKey(0);
	return 0;

}


void onMouse(int event, int x, int y, int flags, void* ustc)
{
	//鼠标左键按下
	if (event == EVENT_LBUTTONUP)
	{
		srccopy = src.clone();

		//获取点击位置的颜色，此处注意y和x的位置顺序
		Scalar color = srccopy.at<Vec3b>(y, x);
		int b = color[0];
		int g = color[1];
		int r = color[2];


		string hexstr = rgb2hex(r, g, b, true);

		string rgbtext = "RGB(" + to_string(r) + "," + to_string(g) + "," + to_string(b) + ")";
		string hextext = "HEX:" + hexstr;


		//在图像上画出点击位置
		circle(srccopy, Point(x, y), 2, Scalar(0, 255, 255));

		//输出显示文字
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
