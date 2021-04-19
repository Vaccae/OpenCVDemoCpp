#include <iostream>
#include <opencv2/opencv.hpp>
#include "CvUtils.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	Mat src = imread("E:/DCIM/test9.jpg");
	CvUtils::MatResize(src);
	CvUtils::SetShowWindow(src, "src", 50, 50);
	imshow("src", src);

	//fastNlMeansDenoisingColored
	double time = getTickCount();
	Mat denoisingdst;
	fastNlMeansDenoisingColored(src, denoisingdst);
	time = getTickCount() - time;
	cout << "运行时间：" << time / getTickFrequency() * 1000 << endl;

	CvUtils::SetShowWindow(denoisingdst, "denoising", src.cols+70, 50);
	imshow("denoising", denoisingdst);

	waitKey(0);
	return 0;
}