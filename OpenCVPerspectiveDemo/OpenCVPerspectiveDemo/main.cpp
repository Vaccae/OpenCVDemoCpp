#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"


using namespace std;
using namespace cv;

//设置图片所以路径
String FilePaths = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVSplitImage/pic";
//获取目录下的所有文件
vector<String> files;


//鼠标回调函数
void onMouse(int event, int x, int y, int flags, void* ustc);

Mat src;
Mat srccopy; //用于拷贝出的源图像
string showsrc = "图像";
int imgindex = 0;
//设置透视变换的点
Point2f vertices[4];


//给透视变换点进行赋值,返回值为3时,说明4个点都已经赋值了，可以进行下一步操作
int setPerspectivePoint(Point2f* vts, int x, int y, bool isinit = false);


int main(int argc, char** argv) {
	glob(FilePaths, files);

	if (files.size() <= 0) {
		cout << "找不到图片文件" << endl;
		waitKey(0);
		return -1;
	}

	//初始化透视变换的点
	setPerspectivePoint(vertices, 0, 0, true);

	//关闭所有显示窗口
	destroyAllWindows();

	cout << "srcindex:" << imgindex << endl;
	String file = files[imgindex];
	src = imread(file);
	CvUtils::MatResize(src);
	CvUtils::SetShowWindow(src, showsrc, 50, 20);
	imshow(showsrc, src);
	//复制下源图
	src.copyTo(srccopy);

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
		//设置选择的点
		int ptindex = setPerspectivePoint(vertices, x, y);

		//在图像上画出点击位置
		circle(src, Point(x, y), 3, Scalar(255, 0, 0), -1);
		//选中的点进行画线
		if (ptindex > 0 && ptindex <= 3) {
			line(src, vertices[ptindex], vertices[ptindex - 1], Scalar(255, 0, 0), 3);
			//当是最后一个点时和起始点进行画线连接
			if (ptindex == 3) {
				line(src, vertices[ptindex], vertices[0], Scalar(255, 0, 0), 3);
			}
		}

		imshow(showsrc, src);

		if (ptindex == 3) {
			//根据最小矩形和多边形拟合的最大四个点计算透视变换矩阵		
			Point2f rectPoint[4];
			//计算旋转矩形的宽和高
			float rWidth = CvUtils::CalcPointDistance(vertices[0], vertices[1]);
			float rHeight = CvUtils::CalcPointDistance(vertices[1], vertices[2]);
			//计算透视变换的四个顶点
			rectPoint[0] = Point2f(0, 0);
			rectPoint[1] = rectPoint[0] + Point2f(rWidth, 0);
			rectPoint[2] = rectPoint[1] + Point2f(0, rHeight);
			rectPoint[3] = rectPoint[0] + Point2f(0, rHeight);


			//计算透视变换矩阵		
			Mat warpmatrix = getPerspectiveTransform(vertices, rectPoint);
			Mat resultimg;
			//透视变换
			warpPerspective(srccopy, resultimg, warpmatrix, resultimg.size(), INTER_LINEAR);

			//载取透视变换后的图像显示出来
			Rect cutrect = Rect(rectPoint[0], rectPoint[2]);
			Mat cutMat = resultimg(cutrect);

			CvUtils::SetShowWindow(cutMat, "cutMat", 600, 20);
			imshow("cutMat", cutMat);
		}

	}
	else if (event == EVENT_RBUTTONUP) {
		//初始化透视变换的点
		setPerspectivePoint(vertices, 0, 0, true);

		imgindex++;
		if (imgindex < files.size()) {
			//关闭所有显示窗口
			destroyAllWindows();

			cout << "srcindex:" << imgindex << endl;
			String file = files[imgindex];
			src = imread(file);
			CvUtils::MatResize(src);
			CvUtils::SetShowWindow(src, showsrc, 50, 20);
			imshow(showsrc, src);

			//复制下源图
			src.copyTo(srccopy);

			//设置鼠标响影事件
			setMouseCallback(showsrc, onMouse);
		}
		waitKey(0);
	}
}


//给透视变换点进行赋值,返回值为true时,说明4个点都已经赋值了，可以进行下一步操作
int setPerspectivePoint(Point2f* vts, int x, int y, bool isinit)
{
	int res = 0;
	if (isinit) {
		for (int i = 0; i < 4; ++i) {
			vts[i].x = -1.0f;
			vts[i].y = -1.0f;
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			if (vts[i].x == -1.0f && vts[i].y == -1.0f) {
				res = i;
				vts[i].x = x;
				vts[i].y = y;
				break;
			}
		}
	}
	return res;
}
