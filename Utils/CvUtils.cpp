#include "CvUtils.h"

void CvUtils::SetShowWindow(cv::Mat img, std::string winname, int pointx, int pointy)
{
	//设置显示窗口
	cv::namedWindow(winname, cv::WindowFlags::WINDOW_NORMAL);
	//设置图像显示大小
	resizeWindow(winname, img.size());
	//设置图像显示位置
	cv::moveWindow(winname, pointx, pointy);
}

void CvUtils::MatResize(cv::Mat& frame, int maxwidth, int maxheight)
{
	double scale;
	//判断图像是水平还是垂直
	bool isHorizontal = frame.cols > frame.rows ? true : false;

	//根据水平还是垂直计算缩放
	if (isHorizontal) {
		if (frame.cols > maxwidth) {
			scale = (double)maxwidth / frame.cols;
			resize(frame, frame, cv::Size(0, 0), scale, scale);
		}
	}
	else {
		if (frame.rows > maxheight) {
			scale = (double)maxheight / frame.rows;
			resize(frame, frame, cv::Size(0, 0), scale, scale);
		}
	}
}

std::vector<cv::Point> CvUtils::Pt2fsToPts(std::vector<cv::Point2f>& vecpt2f)
{
	std::vector<cv::Point> vecpt;
	for (cv::Point2f item : vecpt2f) {
		cv::Point pt = item;
		vecpt.push_back(pt);
	}
	return vecpt;
}

std::vector<cv::Point2f> CvUtils::PtsToPt2fs(std::vector<cv::Point>& vecpt)
{
	std::vector<cv::Point2f> vecpt2f;
	for (cv::Point item : vecpt) {
		cv::Point2f pt = item;
		vecpt2f.push_back(pt);
	}
	return vecpt2f;
}

//求Mat的中位数
int CvUtils::GetMatMidVal(cv::Mat& img)
{
	//判断如果不是单通道直接返回128
	if (img.channels() > 1) return 128;
	int rows = img.rows;
	int cols = img.cols;
	//定义数组
	float mathists[256] = { 0 };
	//遍历计算0-255的个数
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int val = img.at<uchar>(row, col);
			mathists[val]++;
		}
	}

	int calcval = rows * cols / 2;
	int tmpsum = 0;
	for (int i = 0; i < 256; ++i) {
		tmpsum += mathists[i];
		if (tmpsum > calcval) {
			return i;
		}
	}
	return 0;
}

//使用大津法Mat的阈值
int CvUtils::GetMatOTSU(cv::Mat& img)
{
	//判断如果不是单通道直接返回128
	if (img.channels() > 1) return 128;
	int rows = img.rows;
	int cols = img.cols;
	//定义数组
	float mathists[256] = { 0 };
	//遍历计算0-255的个数
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int val = img.at<uchar>(row, col);
			mathists[val]++;
		}
	}

	//定义灰度级像素在整个图像中的比例
	float grayPro[256] = { 0 };
	int matSize = rows * cols;
	for (int i = 0; i < 256; ++i) {
		grayPro[i] = (float)mathists[i] / (float)matSize;
	}

	//大津法OTSU，前景与背景分割，计算出方差最大的灰度值
	int calcval = -1;
	int calcMax = 0;
	for (int i = 0; i < 256; ++i) {
		float w0 = 0, w1 = 0, u0tmp = 0, u1tmp = 0, u0 = 0, u1 = 0, u = 0, calctmp = 0;

		for (int k = 0; k < 256; ++k) {
			float curGray = grayPro[k];
			//计算背景部分
			if (k <= i) {
				//以i为阈值分类，第一类总的概率
				w0 += curGray;
				u0tmp += curGray * k;
			}
			//计算前景部分
			else {
				//以i为阈值分类，第一类总的概率
				w1 += curGray;
				u1tmp += curGray * k;
			}
		}

		//求出第一类和第二类的平均灰度
		u0 = u0tmp / w0;
		u1 = u1tmp / w1;
		//求出整幅图像的平均灰度
		u = u0tmp + u1tmp;

		//计算类间方差
		calctmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);

		//更新最大类间方差，并设置阈值
		if (calctmp > calcMax) {
			calcMax = calctmp;
			calcval = i;
		}
	}

	return calcval;
}

//求自适应阈值的最小和最大值
void CvUtils::GetMatMinMaxThreshold(cv::Mat& img, int& minval, int& maxval, int calctype, float sigma)
{
	int midval;
	switch (calctype)
	{
	case 1: {
		midval = GetMatOTSU(img);
		break;
	}
	default:
		midval = GetMatMidVal(img);
		break;
	}
	std::cout << "midval:" << midval << std::endl;
	// 计算低阈值
	minval = cv::saturate_cast<uchar>((1.0 - sigma) * midval);
	minval = cv::saturate_cast<uchar>(midval / 2);
	//计算高阈值
	maxval = cv::saturate_cast<uchar>((1.0 + sigma) * midval);
	maxval = cv::saturate_cast<uchar>(midval);
}


//根据中心点找最远的四个点
void CvUtils::GetRectPoints(cv::Point2f vetPoints[], cv::Point2f center, std::vector<cv::Point> convex)
{
	//定义最远的4个点，0--左上， 1--右上， 2--右下  3--左下
	float ltdist = 0.0f;  //左上的最大距离 
	float rtdist = 0.0f;  //右上的最大距离 
	float rbdist = 0.0f;  //右下的最大距离 
	float lbdist = 0.0f;  //左下的最大距离

	for (auto curpoint : convex) {
		//计算点的距离 
		float curdist = CalcPointDistance(center, curpoint);

		if (curpoint.x < center.x && curpoint.y < center.y)
		{
			//判断是否在左上
			if (curdist > ltdist) {
				ltdist = curdist;
				vetPoints[0] = curpoint;
			}
		}
		else if (curpoint.x > center.x && curpoint.y < center.y) {
			//判断在右上
			if (curdist > rtdist) {
				rtdist = curdist;
				vetPoints[1] = curpoint;
			}
		}
		else if (curpoint.x > center.x && curpoint.y > center.y) {
			//判断在右下
			if (curdist > rbdist) {
				rbdist = curdist;
				vetPoints[2] = curpoint;
			}
		}
		else if (curpoint.x < center.x && curpoint.y > center.y) {
			//判断在左下
			if (curdist > lbdist) {
				lbdist = curdist;
				vetPoints[3] = curpoint;
			}
		}

	}

}

//根据最小矩形点找最近的四边形点
//第一参数为输出的点，第二个参数为矩形的4个点，第三个为多边形拟合的点 
void CvUtils::GetPointsFromRect(cv::Point2f vetPoints[], cv::Point2f rectPoints[], std::vector<cv::Point> convex)
{
	//定义最远的4个点，0--左上， 1--右上， 2--右下  3--左下
	float ltdist = 99999999.9f;  //左上的最大距离 
	float rtdist = 99999999.9f;  //右上的最大距离 
	float rbdist = 99999999.9f;  //右下的最大距离 
	float lbdist = 99999999.9f;  //左下的最大距离
	float curdist = 0.0f; //当前点的计算距离


	for (auto curpoint : convex) {
		//计算左上点的距离 
		curdist = CalcPointDistance(rectPoints[0], curpoint);
		if (curdist < ltdist) {
			ltdist = curdist;
			vetPoints[0] = curpoint;
		}
		//计算右上角的点距离
		curdist = CalcPointDistance(rectPoints[1], curpoint);
		if (curdist < rtdist) {
			rtdist = curdist;
			vetPoints[1] = curpoint;
		}
		//计算右下角点的距离
		curdist = CalcPointDistance(rectPoints[2], curpoint);
		if (curdist < rbdist) {
			rbdist = curdist;
			vetPoints[2] = curpoint;
		}
		//计算左下角点的距离
		curdist = CalcPointDistance(rectPoints[3], curpoint);
		if (curdist < lbdist) {
			lbdist = curdist;
			vetPoints[3] = curpoint;
		}
	}
}

//重新计算距离变换的4个坐标点
//思路：旋转矩形的点和上一步获取的临近点判断距离，如果小于阈值都列入，大于阈值按最近距离的阈值处理
void CvUtils::GetPointsFromFitline(cv::Point2f newPoints[], cv::Point2f vetPoints[], cv::Point2f rectPoints[], float dist)
{
	//1.重新规划区域点
	float curdist = CalcPointDistance(rectPoints[0], vetPoints[0]);
	newPoints[0] = curdist <= dist ? rectPoints[0] : vetPoints[0] + cv::Point2f(-dist, -dist);

	curdist = CalcPointDistance(rectPoints[1], vetPoints[1]);
	newPoints[1] = curdist <= dist ? rectPoints[1] : vetPoints[1] + cv::Point2f(dist, -dist);

	curdist = CalcPointDistance(rectPoints[2], vetPoints[2]);
	newPoints[2] = curdist <= dist ? rectPoints[2] : vetPoints[2] + cv::Point2f(dist, dist);

	curdist = CalcPointDistance(rectPoints[3], vetPoints[3]);
	newPoints[3] = curdist <= dist ? rectPoints[3] : vetPoints[3] + cv::Point2f(-dist, dist);

	//左侧区域点为最小旋转矩形的左上左下和离的最近的左上左下组成
	std::vector<cv::Point2f> lArea;
	lArea.push_back(newPoints[0]);
	lArea.push_back(vetPoints[0]);
	lArea.push_back(vetPoints[3]);
	lArea.push_back(newPoints[3]);

	//顶部区域点为最小外接矩形左上右上和最近点的左上右上组成
	std::vector<cv::Point2f> tArea;
	tArea.push_back(newPoints[0]);
	tArea.push_back(newPoints[1]);
	tArea.push_back(vetPoints[1]);
	tArea.push_back(vetPoints[0]);

	//右侧区域点为最近点的右上右下和最小外接矩形的右上和右下组成
	std::vector<cv::Point2f> rArea;
	rArea.push_back(vetPoints[1]);
	rArea.push_back(newPoints[1]);
	rArea.push_back(newPoints[2]);
	rArea.push_back(vetPoints[2]);

	//底部区域点为最近点的左下右下和最小外接矩形的左下右下组成
	std::vector<cv::Point2f> bArea;
	bArea.push_back(vetPoints[3]);
	bArea.push_back(vetPoints[2]);
	bArea.push_back(newPoints[2]);
	bArea.push_back(newPoints[3]);

	//2.根据区域做直线拟合
	//左边区域
	cv::Vec4f lLine;
	fitLine(lArea, lLine, cv::DIST_L2, 0, 0.01, 0.01);
	//顶部区域
	cv::Vec4f tLine;
	fitLine(tArea, tLine, cv::DIST_L2, 0, 0.01, 0.01);
	//右边区域
	cv::Vec4f rLine;
	fitLine(rArea, rLine, cv::DIST_L2, 0, 0.01, 0.01);
	//顶部区域
	cv::Vec4f bLine;
	fitLine(bArea, bLine, cv::DIST_L2, 0, 0.01, 0.01);

	//3.根据直线拟合的求每两条直线的交叉点为我们的多边形顶点
	//左上顶点
	newPoints[0] = GetCrossPoint(lLine, tLine);
	//右上顶点
	newPoints[1] = GetCrossPoint(tLine, rLine);
	//右下顶点
	newPoints[2] = GetCrossPoint(rLine, bLine);
	//左下顶点
	newPoints[3] = GetCrossPoint(bLine, lLine);
}

//求两条直线的交叉点
cv::Point2f CvUtils::GetCrossPoint(cv::Vec4f Line1, cv::Vec4f Line2)
{
	double k1, k2;
	//Line1的斜率
	k1 = Line1[1] / Line1[0];
	//Line2的斜率
	k2 = Line2[1] / Line2[0];

	cv::Point2f crossPoint;
	crossPoint.x = (k1 * Line1[2] - Line1[3] - k2 * Line2[2] + Line2[3]) / (k1 - k2);
	crossPoint.y = (k1 * k2 * (Line1[2] - Line2[2]) + k1 * Line2[3] - k2 * Line1[3]) / (k1 - k2);
	return crossPoint;
}

bool CvUtils::CheckRectBorder(cv::Mat& img, cv::RotatedRect rect)
{
	bool res = false;
	cv::Point2f pts[4];
	rect.points(pts);

	for (auto item : pts) {
		if (item.x<0 || item.y<0 || item.x> img.cols || item.y>img.rows) {
			res = true;
			break;
		}
	}
	return res;
}

//重新排序旋转矩形坐标点
void CvUtils::SortRotatedRectPoints(cv::Point2f vetPoints[], cv::RotatedRect rect, int flag)
{
	rect.points(vetPoints);

	std::cout << vetPoints[0] << vetPoints[1] << vetPoints[2] << vetPoints[3] << std::endl;
	std::cout << rect.angle << std::endl;

	cv::Point2f curpoint;

	if (flag == 0) {
		//按X轴排序
		for (int i = 0; i < 4; ++i) {
			for (int k = i + 1; k < 4; ++k) {
				if (vetPoints[i].x > vetPoints[k].x) {
					curpoint = vetPoints[i];
					vetPoints[i] = vetPoints[k];
					vetPoints[k] = curpoint;
				}
			}
		}

		//判断X坐标前两个定义左上左下角
		if (vetPoints[0].y > vetPoints[1].y) {
			curpoint = vetPoints[0];
			vetPoints[0] = vetPoints[1];
			vetPoints[1] = vetPoints[3];
			vetPoints[3] = curpoint;
		}
		else {
			curpoint = vetPoints[3];
			vetPoints[3] = vetPoints[1];
			vetPoints[1] = curpoint;
		}

		//判断X坐标后两个定义右上右下角
		if (vetPoints[1].y > vetPoints[2].y) {
			curpoint = vetPoints[1];
			vetPoints[1] = vetPoints[2];
			vetPoints[2] = curpoint;
		}
	}
	else {
		//根据Rect的坐标点，Y轴最大的为P[0]，p[0]围着center顺时针旋转, 
		//旋转角度为负的话即是P[0]在左下角，为正P[0]是右下角
		//重新排序坐标点
		if (rect.angle < 0) {
			curpoint = vetPoints[0];
			vetPoints[0] = vetPoints[2];
			vetPoints[2] = curpoint;
			curpoint = vetPoints[1];
			vetPoints[1] = vetPoints[3];
			vetPoints[3] = curpoint;
		}
		else if (rect.angle > 0) {
			curpoint = vetPoints[0];
			vetPoints[0] = vetPoints[1];
			vetPoints[1] = vetPoints[2];
			vetPoints[2] = vetPoints[3];
			vetPoints[3] = curpoint;
		}
	}
}


//计算两点间的距离
float CvUtils::CalcPointDistance(cv::Point2f point1, cv::Point2f point2)
{
	//计算两个点的Point差值
	cv::Point2f tmppoint = point1 - point2;
	//利用欧几里德距离计算H
	return sqrt(pow(tmppoint.x, 2) + pow(tmppoint.y, 2));
}

//根据三个点计算夹角
double CvUtils::CalcAngle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;

	double cosine = (dx1 * dx2 + dy1 * dy2) / (sqrt(dx1 * dx1 + dy1 * dy1) * sqrt(dx2 * dx2 + dy2 * dy2) + 1e-10);

	return cosine;
}
