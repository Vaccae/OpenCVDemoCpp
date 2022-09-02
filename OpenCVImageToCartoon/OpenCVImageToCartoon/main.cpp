#include<iostream>
#include<opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"

using namespace std;
using namespace cv;

void EdgesToCartoon(Mat frame);
void StylizationToCartoon(Mat frame);
void PencilSketchToCartoon(Mat frame);

int main(int agrc, char** argv) {

	Mat src = imread("E:/Family/me/2021����1.jpg");

	//�Զ�����ͼ����ʾ��С
	CvUtils::MatResize(src);
	//��ʾͼ��
	CvUtils::SetShowWindow(src, "src", 10, 50);
	imshow("src", src);

	//ʹ��˫���˲�ʵ��ͼƬ��ͨ��
	EdgesToCartoon(src);

	//ʹ�÷��ʵ��ͼƬ��ͨ��
	StylizationToCartoon(src);

	//ʹ��pencilSketch�˲�ʵ��ͼƬ��ͨ��
	PencilSketchToCartoon(src);

	waitKey(0);
	return 0;
}

void EdgesToCartoon(Mat frame)
{
	double t1 = (double)getTickCount();

	Mat gray;
	//1.�Ҷ�ͼ
	cvtColor(frame, gray, COLOR_BGR2GRAY);
	//2.��ֵ�˲�����
	medianBlur(gray, gray, 5);

	//2.Canny��ȡ��Ե��ԭ��Ҳ��Laplacian�����Թ�����������ͼ��Ч������Canny��
	//Laplacian(gray, gray, CV_8U, 3);
	Canny(gray, gray, 120, 240);

	//3.��ֵ����ȡ��ı�Եͼ��
	Mat mask(frame.size(), CV_8U);
	threshold(gray, mask, 120, 255, THRESH_BINARY_INV);

	//4.��ԭʼͼ��˫���˲�
	//4.1��Сԭʼͼ�����ڼӿ촦���ٶ�
	Size tmpdst(frame.cols / 2, frame.rows / 2);
	Mat srctmp = Mat(tmpdst, frame.type());
	resize(frame, srctmp, tmpdst, 0, 0, INTER_LINEAR);
	Mat tmp = Mat(tmpdst, CV_8UC3);

	//4.2ʹ��˫���˲�����ͼ�������˵�������Ϊ2
	//���������ֱ����Χ
	int dsize = 5;
	double sColor = 30;
	double sSpace = 10;
	//��������
	int iterator = 2;
	for (int i = 0; i < iterator; i++) {
		bilateralFilter(srctmp, tmp, dsize, sColor, sSpace);
		bilateralFilter(tmp, srctmp, dsize, sColor, sSpace);
	}

	//4.3���������ͼ��������ԭͼ��С�ٽ�����Ĥ����
	Mat srcbak;
	resize(srctmp, srcbak, frame.size(), 0, 0, INTER_LINEAR);
	//��Ĥ����
	Mat dst = Mat(frame.size(), frame.type(), Scalar::all(0)); //��ʼ��
	srcbak.copyTo(dst, mask);

	//��ʾͼ��
	CvUtils::SetShowWindow(dst, "dst", 330, 50);
	imshow("dst", dst);

	t1 = (double)getTickCount() - t1;
	cout << "EdgesToCartoon��ʱ:" << t1 * 1000.0 / cv::getTickFrequency() << " ms \n";

}

void StylizationToCartoon(Mat frame)
{
	double t1 = (double)getTickCount();

	Mat dststyle;
	stylization(frame, dststyle, 100, 0.7);
	//��ʾͼ��
	CvUtils::SetShowWindow(dststyle, "dststyle", 630, 50);
	imshow("dststyle", dststyle);

	t1 = (double)getTickCount() - t1;
	cout << "StylizationToCartoon��ʱ:" << t1 * 1000.0 / cv::getTickFrequency() << " ms \n";

}

void PencilSketchToCartoon(Mat frame)
{
	double t1 = (double)getTickCount();

	Mat dstpencil1, dstpencil2;
	pencilSketch(frame, dstpencil1, dstpencil2, 60, 0.2);

	//��ʾͼ��
	CvUtils::SetShowWindow(dstpencil2, "dstpencil", 930, 50);
	imshow("dstpencil", dstpencil2);

	t1 = (double)getTickCount() - t1;
	cout << "PencilSketchToCartoon��ʱ:" << t1 * 1000.0 / cv::getTickFrequency() << " ms \n";

}
