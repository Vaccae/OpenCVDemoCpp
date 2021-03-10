#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	Mat gray, gray_inverse,dst;

	Mat src = imread("E:/DCIM/grow.jpg");
	if (!src.data) {
		cout << "��ȡͼ��ʧ��" << endl;
		return -1;
	}
	imshow("src", src);

	//1.תΪ�Ҷ�ͼ
	cvtColor(src, gray, COLOR_BGRA2GRAY);

	//2.ͼ��ȡ��,����ȡ���ķ���
	//2.1 ��������ֱ����255ȥ��
	//gray_inverse = Scalar(255, 255, 255) - gray;
	//2.2 ��subtract����
	//subtract(Scalar(255, 255, 255), gray, gray_inverse);
	//2.3 λ����ֱ��ȡ��
	gray_inverse = ~gray;
	//imshow("gray_inverse", gray_inverse);

	//3 ��˹ģ��
	GaussianBlur(gray_inverse, gray_inverse, Size(15, 15), 50, 50);
	//imshow("GaussianBlur", gray_inverse);

	//4 ��ɫ�������
	divide(gray, 255 - gray_inverse, dst, 255);
	imshow("dst", dst);


	waitKey(0);
	return 0;
}