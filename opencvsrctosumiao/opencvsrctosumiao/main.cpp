#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	Mat gray, gray_inverse,dst;

	Mat src = imread("E:/DCIM/grow.jpg");
	if (!src.data) {
		cout << "读取图像失败" << endl;
		return -1;
	}
	imshow("src", src);

	//1.转为灰度图
	cvtColor(src, gray, COLOR_BGRA2GRAY);

	//2.图像取反,三种取反的方法
	//2.1 遍历像素直接用255去减
	//gray_inverse = Scalar(255, 255, 255) - gray;
	//2.2 用subtract函数
	//subtract(Scalar(255, 255, 255), gray, gray_inverse);
	//2.3 位运算直接取反
	gray_inverse = ~gray;
	//imshow("gray_inverse", gray_inverse);

	//3 高斯模糊
	GaussianBlur(gray_inverse, gray_inverse, Size(15, 15), 50, 50);
	//imshow("GaussianBlur", gray_inverse);

	//4 颜色减淡混合
	divide(gray, 255 - gray_inverse, dst, 255);
	imshow("dst", dst);


	waitKey(0);
	return 0;
}