#include <opencv2/opencv.hpp>
#include <iostream>
#include "../../Utils/CvUtils.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

	Mat src = imread("E:/DCIM/Test_image.jpg");
	//因为图片是纵向的，所以前面800的参数作用不大，主要是为了限制高度可以三张图片都显示出来
	CvUtils::MatResize(src, 800, 520);
	//imshow("src", src);

	//1.将图像转为灰度图
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	CvUtils::SetShowWindow(gray, "gray", 0, 30);
	imshow("gray", gray);

	//定义腐蚀和膨胀的结构化元素和迭代次数
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	int iteration = 9;

	//2.将灰度图进行膨胀操作
	Mat dilateMat;
	morphologyEx(gray, dilateMat, MORPH_DILATE, element, Point(-1, -1), iteration);
	//imshow("dilate", dilateMat);

	//3.将膨胀后的图再进行腐蚀
	Mat erodeMat;
	morphologyEx(dilateMat, erodeMat, MORPH_ERODE, element, Point(-1, -1), iteration);
	//imshow("erode", erodeMat);

	//4.膨胀再腐蚀后的图减去原灰度图再进行取反操作
	Mat calcMat = ~(erodeMat - gray);
	CvUtils::SetShowWindow(calcMat, "calc", gray.cols * 2, 30);
	imshow("calc", calcMat);

	//5.使用规一化将原来背景白色的改了和原来灰度图差不多的灰色
	Mat removeShadowMat;
	normalize(calcMat, removeShadowMat, 0, 200, NORM_MINMAX);
	CvUtils::SetShowWindow(removeShadowMat, "dst", 
		gray.cols, 30);
	imshow("dst", removeShadowMat);


	waitKey(0);
	return 0;
}