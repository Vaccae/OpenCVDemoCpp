#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat matresize(Mat& bigsrc, Mat& smallsrc, int step=10);

int main(int argc, char** argv) {
	//读取图像一
	Mat src1 = imread("E:/DCIM/Resize/src1.png");
	imshow("src1", src1);
	//读取图像二
	Mat src2 = imread("E:/DCIM/Resize/src2.png");
	imshow("src2", src2);

	//根据两个图像生成新的Mat
	Mat dst = matresize(src1, src2);
	//将图像保存到本地
	imwrite("E:/DCIM/Resize/resize.png", dst);

	imshow("resize", dst);
	waitKey(0);
	return 0;
}

//第二张图为第一张图的10倍缩小
Mat matresize(Mat& bigsrc, Mat& smallsrc, int step)
{
	//如果倍数小于等于0直接退出
	if (step <= 0) return Mat();

	Mat dst, tmpsrc;
	//复制输出图像
	bigsrc.copyTo(dst);

	//计算第二张图要缩放的大小
	int calccol = bigsrc.cols / step;
	int calcrow = bigsrc.rows / step;
	resize(smallsrc, tmpsrc, Size(calccol, calcrow));


	for (int col = 0; col < calccol; ++col) {
		for (int row = 0; row < calcrow; ++row) {
			int mapcol = col * step + step * 0.5;
			int maprow = row * step + step * 0.5;

			//改变输出图的值
			if (mapcol < dst.cols && maprow < dst.rows) {
				dst.at<Vec3b>(maprow, mapcol) = tmpsrc.at<Vec3b>(row, col);
			}
		}
	}

	return dst;
}
