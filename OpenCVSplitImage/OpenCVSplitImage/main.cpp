#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"

using namespace cv;
using namespace std;
//设置图片所以路径
String FilePaths = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVSplitImage/pic";


int main(int argc, char** argv) {
	//获取目录下的所有文件
	vector<String> files;
	glob(FilePaths, files);


	if (files.size() <= 0) {
		cout << "找不到图片文件" << endl;
		waitKey(0);
		return -1;
	}

	for (int index = 0; index < files.size(); ++index) {
		//关闭所有显示窗口
		destroyAllWindows();

		cout << "srcindex:" << index << endl;
		String file = files[index];
		Mat src = imread(file);
		CvUtils::MatResize(src);
		CvUtils::SetShowWindow(src, "src", 20, 20);
		imshow("src", src);

		//高斯滤波
		GaussianBlur(src, src, Size(5, 5), 0.5, 0.5);

		int srcArea = src.size().area();
		float maxArea = 0;
		int maxAreaidx = -1;

		try
		{
			vector<Mat> channels;
			Mat B_src, G_src, R_src, dstmat;
			split(src, channels);
			
			int minthreshold = 120, maxthreshold = 200;
		
			//B进行Canny
			//大津法求阈值
			CvUtils::GetMatMinMaxThreshold(channels[0], minthreshold, maxthreshold, 1);
			cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << endl;
			//Canny边缘提取
			Canny(channels[0], B_src, minthreshold, maxthreshold);

			//大津法求阈值
			CvUtils::GetMatMinMaxThreshold(channels[1], minthreshold, maxthreshold, 1);
			cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << endl;
			//Canny边缘提取
			Canny(channels[1], G_src, minthreshold, maxthreshold);

			//大津法求阈值
			CvUtils::GetMatMinMaxThreshold(channels[2], minthreshold, maxthreshold, 1);
			cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << endl;
			//Canny边缘提取
			Canny(channels[2], R_src, minthreshold, maxthreshold);

			
			bitwise_or(B_src, G_src, dstmat);
			bitwise_or(R_src, dstmat, dstmat);
			CvUtils::SetShowWindow(dstmat, "dstmat", 700, 20);
			imshow("dstmat", dstmat);

			//Mat channelmat;
			//resize(B_src, B_src, Size(0, 0), 0.4, 0.4);
			//resize(G_src, G_src, Size(0, 0), 0.4, 0.4);
			//resize(R_src, R_src, Size(0, 0), 0.4, 0.4);
			//channelmat.push_back(B_src);
			//channelmat.push_back(G_src);
			//channelmat.push_back(R_src);
			//CvUtils::SetShowWindow(channelmat, "channelmat", 600, 0);
			//imshow("channelmat", channelmat);


			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;
			findContours(dstmat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

			Mat dstcontour = Mat::zeros(dstmat.size(), CV_8SC3);
			Mat tmpcontour;
			dstcontour.copyTo(tmpcontour);

			//定义拟合后的多边形数组
			vector<vector<Point>> vtshulls(contours.size());

			for (int i = 0; i < contours.size(); ++i) {
				//判断轮廓形状，不是四边形的忽略掉
				double lensval = 0.01 * arcLength(contours[i], true);
				vector<Point> convexhull;
				approxPolyDP(Mat(contours[i]), convexhull, lensval, true);

				//拟合的多边形存放到定义的数组中
				vtshulls[i] = convexhull;

				//不是四边形的过滤掉
				if (convexhull.size() != 4) continue;

				//求出最小旋转矩形
				RotatedRect rRect = minAreaRect(contours[i]);
				//更新最小旋转矩形中面积最大的值
				if (rRect.size.height == 0) continue;
				
				if (rRect.size.area() > maxArea && rRect.size.area() > srcArea * 0.1
					&& !CvUtils::CheckRectBorder(src, rRect)) {
					maxArea = rRect.size.area();
					maxAreaidx = i;
				}
			}

			//找到符合条码的最大面积的轮廓进行处理
			if (maxAreaidx >= 0) {
				//获取最小旋转矩形
				RotatedRect rRect = minAreaRect(contours[maxAreaidx]);
				Point2f vertices[4];
				//重新排序矩形坐标点，按左上，右上，右下，左下顺序
				CvUtils::SortRotatedRectPoints(vertices, rRect);

				cout <<"Rect:" << vertices[0] << vertices[1] << vertices[2] << vertices[3] << endl;

				//根据获得的4个点画线
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, vertices[k], vertices[(k + 1) % 4], Scalar(255, 0, 0));
				}

				//计算四边形的四点坐标
				Point2f rPoints[4];
				CvUtils::GetPointsFromRect(rPoints, vertices, vtshulls[maxAreaidx]);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, rPoints[k], rPoints[(k + 1) % 4], Scalar(255, 255, 255));
				}


				//采用离最小矩形四个点最近的重新设置范围，将所在区域的点做直线拟合再看看结果
				Point2f newPoints[4];
				CvUtils::GetPointsFromFitline(newPoints, rPoints, vertices);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, newPoints[k], newPoints[(k + 1) % 4], Scalar(255, 100, 255));
				}


				//根据最小矩形和多边形拟合的最大四个点计算透视变换矩阵		
				Point2f rectPoint[4];
				//计算旋转矩形的宽和高
				float rWidth = CvUtils::CalcPointDistance(vertices[0], vertices[1]);
				float rHeight = CvUtils::CalcPointDistance(vertices[1], vertices[2]);
				//计算透视变换的左上角起始点
				float left = dstcontour.cols;
				float top = dstcontour.rows;
				for (int i = 0; i < 4; ++i) {
					if (left > newPoints[i].x) left = newPoints[i].x;
					if (top > newPoints[i].y) top = newPoints[i].y;
				}

				rectPoint[0] = Point2f(left, top);
				rectPoint[1] = rectPoint[0] + Point2f(rWidth, 0);
				rectPoint[2] = rectPoint[1] + Point2f(0, rHeight);
				rectPoint[3] = rectPoint[0] + Point2f(0, rHeight);


				//计算透视变换矩阵		
				Mat warpmatrix = getPerspectiveTransform(rPoints, rectPoint);
				Mat resultimg;
				//透视变换
				warpPerspective(src, resultimg, warpmatrix, resultimg.size(), INTER_LINEAR);

				/*CvUtils::SetShowWindow(resultimg, "resultimg", 200, 20);
				imshow("resultimg", resultimg);*/

				//载取透视变换后的图像显示出来
				Rect cutrect = Rect(rectPoint[0], rectPoint[2]);
				Mat cutMat = resultimg(cutrect);
				
				CvUtils::SetShowWindow(cutMat, "cutMat", 600, 20);
				imshow("cutMat", cutMat);
			}

			/*CvUtils::SetShowWindow(dstcontour, "dstcontour", 600, 20);
			imshow("dstcontour", dstcontour);*/

		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}

		waitKey(0);
	}


	return 0;
}