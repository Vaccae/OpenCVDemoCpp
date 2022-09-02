#include<iostream>
#include<opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"

using namespace std;
using namespace cv;

void EdgesToCartoon(Mat frame);
void StylizationToCartoon(Mat frame);
void PencilSketchToCartoon(Mat frame);

int main(int agrc, char** argv) {

	Mat src = imread("E:/Family/me/2021过年1.jpg");

	//自动设置图像显示大小
	CvUtils::MatResize(src);
	//显示图像
	CvUtils::SetShowWindow(src, "src", 10, 50);
	imshow("src", src);

	//使用双边滤波实现图片卡通化
	EdgesToCartoon(src);

	//使用风格化实现图片卡通化
	StylizationToCartoon(src);

	//使用pencilSketch滤波实现图片卡通化
	PencilSketchToCartoon(src);

	waitKey(0);
	return 0;
}

void EdgesToCartoon(Mat frame)
{
	double t1 = (double)getTickCount();

	Mat gray;
	//1.灰度图
	cvtColor(frame, gray, COLOR_BGR2GRAY);
	//2.中值滤波降噪
	medianBlur(gray, gray, 5);

	//2.Canny提取边缘，原来也用Laplacian算子试过，不过本张图的效果还是Canny好
	//Laplacian(gray, gray, CV_8U, 3);
	Canny(gray, gray, 120, 240);

	//3.二值化提取后的边缘图像
	Mat mask(frame.size(), CV_8U);
	threshold(gray, mask, 120, 255, THRESH_BINARY_INV);

	//4.对原始图像双边滤波
	//4.1缩小原始图像，用于加快处理速度
	Size tmpdst(frame.cols / 2, frame.rows / 2);
	Mat srctmp = Mat(tmpdst, frame.type());
	resize(frame, srctmp, tmpdst, 0, 0, INTER_LINEAR);
	Mat tmp = Mat(tmpdst, CV_8UC3);

	//4.2使用双边滤波处理图像，设置了迭代次数为2
	//像素邻域的直径范围
	int dsize = 5;
	double sColor = 30;
	double sSpace = 10;
	//迭代次数
	int iterator = 2;
	for (int i = 0; i < iterator; i++) {
		bilateralFilter(srctmp, tmp, dsize, sColor, sSpace);
		bilateralFilter(tmp, srctmp, dsize, sColor, sSpace);
	}

	//4.3将处理完的图像缩放至原图大小再进行掩膜叠加
	Mat srcbak;
	resize(srctmp, srcbak, frame.size(), 0, 0, INTER_LINEAR);
	//掩膜叠加
	Mat dst = Mat(frame.size(), frame.type(), Scalar::all(0)); //初始化
	srcbak.copyTo(dst, mask);

	//显示图像
	CvUtils::SetShowWindow(dst, "dst", 330, 50);
	imshow("dst", dst);

	t1 = (double)getTickCount() - t1;
	cout << "EdgesToCartoon用时:" << t1 * 1000.0 / cv::getTickFrequency() << " ms \n";

}

void StylizationToCartoon(Mat frame)
{
	double t1 = (double)getTickCount();

	Mat dststyle;
	stylization(frame, dststyle, 100, 0.7);
	//显示图像
	CvUtils::SetShowWindow(dststyle, "dststyle", 630, 50);
	imshow("dststyle", dststyle);

	t1 = (double)getTickCount() - t1;
	cout << "StylizationToCartoon用时:" << t1 * 1000.0 / cv::getTickFrequency() << " ms \n";

}

void PencilSketchToCartoon(Mat frame)
{
	double t1 = (double)getTickCount();

	Mat dstpencil1, dstpencil2;
	pencilSketch(frame, dstpencil1, dstpencil2, 60, 0.2);

	//显示图像
	CvUtils::SetShowWindow(dstpencil2, "dstpencil", 930, 50);
	imshow("dstpencil", dstpencil2);

	t1 = (double)getTickCount() - t1;
	cout << "PencilSketchToCartoon用时:" << t1 * 1000.0 / cv::getTickFrequency() << " ms \n";

}
