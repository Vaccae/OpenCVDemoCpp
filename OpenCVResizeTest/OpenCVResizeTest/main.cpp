#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat matresize(Mat& bigsrc, Mat& smallsrc, int step=10);

int main(int argc, char** argv) {
	//��ȡͼ��һ
	Mat src1 = imread("E:/DCIM/Resize/src1.png");
	imshow("src1", src1);
	//��ȡͼ���
	Mat src2 = imread("E:/DCIM/Resize/src2.png");
	imshow("src2", src2);

	//��������ͼ�������µ�Mat
	Mat dst = matresize(src1, src2);
	//��ͼ�񱣴浽����
	imwrite("E:/DCIM/Resize/resize.png", dst);

	imshow("resize", dst);
	waitKey(0);
	return 0;
}

//�ڶ���ͼΪ��һ��ͼ��10����С
Mat matresize(Mat& bigsrc, Mat& smallsrc, int step)
{
	//�������С�ڵ���0ֱ���˳�
	if (step <= 0) return Mat();

	Mat dst, tmpsrc;
	//�������ͼ��
	bigsrc.copyTo(dst);

	//����ڶ���ͼҪ���ŵĴ�С
	int calccol = bigsrc.cols / step;
	int calcrow = bigsrc.rows / step;
	resize(smallsrc, tmpsrc, Size(calccol, calcrow));


	for (int col = 0; col < calccol; ++col) {
		for (int row = 0; row < calcrow; ++row) {
			int mapcol = col * step + step * 0.5;
			int maprow = row * step + step * 0.5;

			//�ı����ͼ��ֵ
			if (mapcol < dst.cols && maprow < dst.rows) {
				dst.at<Vec3b>(maprow, mapcol) = tmpsrc.at<Vec3b>(row, col);
			}
		}
	}

	return dst;
}
