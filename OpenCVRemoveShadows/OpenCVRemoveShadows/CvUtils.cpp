#include "CvUtils.h"

void CvUtils::SetShowWindow(Mat img, string winname, int pointx, int pointy)
{
	//设置显示窗口
	namedWindow(winname, WindowFlags::WINDOW_NORMAL);
	//设置图像显示大小
	resizeWindow(winname, img.size());
	//设置图像显示位置
	moveWindow(winname, pointx, pointy);
}

void CvUtils::MatResize(Mat& frame, int maxwidth, int maxheight)
{
	double scale;
	//判断图像是水平还是垂直
	bool isHorizontal = frame.cols > frame.rows ? true : false;

	//根据水平还是垂直计算缩放
	if (isHorizontal) {
		if (frame.cols > maxwidth) {
			scale = (double)maxwidth / frame.cols;
			resize(frame, frame, Size(0, 0), scale, scale);
		}
	}
	else {
		if (frame.rows > maxheight) {
			scale = (double)maxheight / frame.rows;
			resize(frame, frame, Size(0, 0), scale, scale);
		}
	}
}

vector<Point> CvUtils::Vecpt2fToVecpt(vector<Point2f>& vecpt2f)
{
	vector<Point> vecpt;
	for (Point2f item : vecpt2f) {
		Point pt = item;
		vecpt.push_back(pt);
	}
	return vecpt;
}

vector<Point2f> CvUtils::VecptToVecpt2f(vector<Point>& vecpt)
{
	vector<Point2f> vecpt2f;
	for (Point item : vecpt) {
		Point2f pt = item;
		vecpt2f.push_back(pt);
	}
	return vecpt2f;
}
