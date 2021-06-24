#include <opencv2/opencv.hpp>
#include <iostream>
#include "../../Utils/CvUtils.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

	Mat src = imread("E:/DCIM/Test_image.jpg");
	//��ΪͼƬ������ģ�����ǰ��800�Ĳ������ò�����Ҫ��Ϊ�����Ƹ߶ȿ�������ͼƬ����ʾ����
	CvUtils::MatResize(src, 800, 520);
	//imshow("src", src);

	//1.��ͼ��תΪ�Ҷ�ͼ
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	CvUtils::SetShowWindow(gray, "gray", 0, 30);
	imshow("gray", gray);

	//���帯ʴ�����͵Ľṹ��Ԫ�غ͵�������
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	int iteration = 9;

	//2.���Ҷ�ͼ�������Ͳ���
	Mat dilateMat;
	morphologyEx(gray, dilateMat, MORPH_DILATE, element, Point(-1, -1), iteration);
	//imshow("dilate", dilateMat);

	//3.�����ͺ��ͼ�ٽ��и�ʴ
	Mat erodeMat;
	morphologyEx(dilateMat, erodeMat, MORPH_ERODE, element, Point(-1, -1), iteration);
	//imshow("erode", erodeMat);

	//4.�����ٸ�ʴ���ͼ��ȥԭ�Ҷ�ͼ�ٽ���ȡ������
	Mat calcMat = ~(erodeMat - gray);
	CvUtils::SetShowWindow(calcMat, "calc", gray.cols * 2, 30);
	imshow("calc", calcMat);

	//5.ʹ�ù�һ����ԭ��������ɫ�ĸ��˺�ԭ���Ҷ�ͼ���Ļ�ɫ
	Mat removeShadowMat;
	normalize(calcMat, removeShadowMat, 0, 200, NORM_MINMAX);
	CvUtils::SetShowWindow(removeShadowMat, "dst", 
		gray.cols, 30);
	imshow("dst", removeShadowMat);


	waitKey(0);
	return 0;
}