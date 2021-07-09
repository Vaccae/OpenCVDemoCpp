#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "..\..\Utils\CvUtils.h"
#include "PaddleOcrApi.h"
#include "..\..\Utils\putText.h"


//设置图片所以路径
cv::String FilePaths = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVSplitImage/pic";


int main(int argc, char** argv) {
	//获取目录下的所有文件
	std::vector<cv::String> files;
	cv::glob(FilePaths, files);


	if (files.size() <= 0) {
		std::cout << "找不到图片文件" << std::endl;
		cv::waitKey(0);
		return -1;
	}

	for (int index = 0; index < files.size(); ++index) {
		//关闭所有显示窗口
		cv::destroyAllWindows();

		std::cout << "srcindex:" << index << std::endl;
		cv::String file = files[index];
		cv::Mat src = cv::imread(file);
		CvUtils::MatResize(src);
		CvUtils::SetShowWindow(src, "src", 20, 20);
		cv::imshow("src", src);

		//高斯滤波
		GaussianBlur(src, src, cv::Size(5, 5), 0.5, 0.5);

		int srcArea = src.size().area();
		float maxArea = 0;
		int maxAreaidx = -1;

		try
		{
			std::vector<cv::Mat> channels;
			cv::Mat B_src, G_src, R_src, dstmat;
			cv::split(src, channels);

			int minthreshold = 120, maxthreshold = 200;

			//B进行Canny
			//大津法求阈值
			CvUtils::GetMatMinMaxThreshold(channels[0], minthreshold, maxthreshold, 1);
			std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
			//Canny边缘提取
			cv::Canny(channels[0], B_src, minthreshold, maxthreshold);

			//大津法求阈值
			CvUtils::GetMatMinMaxThreshold(channels[1], minthreshold, maxthreshold, 1);
			std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
			//Canny边缘提取
			Canny(channels[1], G_src, minthreshold, maxthreshold);

			//大津法求阈值
			CvUtils::GetMatMinMaxThreshold(channels[2], minthreshold, maxthreshold, 1);
			std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
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


			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i> hierarchy;
			findContours(dstmat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

			cv::Mat dstcontour = cv::Mat::zeros(dstmat.size(), CV_8SC3);
			cv::Mat tmpcontour;
			dstcontour.copyTo(tmpcontour);

			//定义拟合后的多边形数组
			std::vector<std::vector<cv::Point>> vtshulls(contours.size());

			for (int i = 0; i < contours.size(); ++i) {
				//判断轮廓形状，不是四边形的忽略掉
				double lensval = 0.01 * arcLength(contours[i], true);
				std::vector<cv::Point> convexhull;
				approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);

				//拟合的多边形存放到定义的数组中
				vtshulls[i] = convexhull;

				//不是四边形的过滤掉
				if (convexhull.size() != 4) continue;

				//求出最小旋转矩形
				cv::RotatedRect rRect = minAreaRect(contours[i]);
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
				cv::RotatedRect rRect = minAreaRect(contours[maxAreaidx]);
				cv::Point2f vertices[4];
				//重新排序矩形坐标点，按左上，右上，右下，左下顺序
				CvUtils::SortRotatedRectPoints(vertices, rRect);

				std::cout << "Rect:" << vertices[0] << vertices[1] << vertices[2] << vertices[3] << std::endl;

				//根据获得的4个点画线
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, vertices[k], vertices[(k + 1) % 4], cv::Scalar(255, 0, 0));
				}

				//计算四边形的四点坐标
				cv::Point2f rPoints[4];
				CvUtils::GetPointsFromRect(rPoints, vertices, vtshulls[maxAreaidx]);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, rPoints[k], rPoints[(k + 1) % 4], cv::Scalar(255, 255, 255));
				}


				//采用离最小矩形四个点最近的重新设置范围，将所在区域的点做直线拟合再看看结果
				cv::Point2f newPoints[4];
				CvUtils::GetPointsFromFitline(newPoints, rPoints, vertices);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, newPoints[k], newPoints[(k + 1) % 4], cv::Scalar(255, 100, 255));
				}


				//根据最小矩形和多边形拟合的最大四个点计算透视变换矩阵		
				cv::Point2f rectPoint[4];
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

				rectPoint[0] = cv::Point2f(left, top);
				rectPoint[1] = rectPoint[0] + cv::Point2f(rWidth, 0);
				rectPoint[2] = rectPoint[1] + cv::Point2f(0, rHeight);
				rectPoint[3] = rectPoint[0] + cv::Point2f(0, rHeight);


				//计算透视变换矩阵		
				cv::Mat warpmatrix = getPerspectiveTransform(rPoints, rectPoint);
				cv::Mat resultimg;
				//透视变换
				warpPerspective(src, resultimg, warpmatrix, resultimg.size(), cv::INTER_LINEAR);

				/*CvUtils::SetShowWindow(resultimg, "resultimg", 200, 20);
				imshow("resultimg", resultimg);*/

				//载取透视变换后的图像显示出来
				cv::Rect cutrect = cv::Rect(rectPoint[0], rectPoint[2]);
				cv::Mat cutMat = resultimg(cutrect);

				//使用PaddleOCR识别
				std::string resstr = PaddleOcrApi::GetPaddleOCRText(cutMat);
				std::cout << "OCR:" << resstr << std::endl;

				//输出识别文字
				putText::putTextZH(cutMat, resstr.data(), cv::Point(20, 20), cv::Scalar(0, 0, 255), 1);
				cv::putText(cutMat, resstr, cv::Point(20, 50), 1, 1, cv::Scalar(0, 0, 255));

				CvUtils::SetShowWindow(cutMat, "cutMat", 600, 20);
				cv::imshow("cutMat", cutMat);
			}

			/*CvUtils::SetShowWindow(dstcontour, "dstcontour", 600, 20);
			imshow("dstcontour", dstcontour);*/

		}
		catch (const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}

		cv::waitKey(0);
	}


	return 0;
}