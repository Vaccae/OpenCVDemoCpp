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

		try
		{
			cv::Mat resultMat = PaddleOcrApi::GetPerspectiveMat(src, 2);
			CvUtils::SetShowWindow(resultMat, "resultMat", 700, 20);
			imshow("resultMat", resultMat);

			//获取分割后的图像
			//std::vector<cv::Mat> vts = PaddleOcrApi::GetNumMat(resultMat);

			cv::Mat tmpsrc;
			resultMat.copyTo(tmpsrc);
			//使用拉普拉斯算子实现图像对比度提高
			cv::Mat Laplancekernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
			cv::Mat imgLaplance, resimg;
			cv::filter2D(tmpsrc, imgLaplance, CV_32F, Laplancekernel);
			tmpsrc.convertTo(resimg, CV_32F);
			resimg = resimg - imgLaplance;
			resimg.convertTo(tmpsrc, CV_8UC3);
			CvUtils::SetShowWindow(tmpsrc, "resimg", 700, 20);
			cv::imshow("resimg", tmpsrc);

			//使用PaddleOCR识别
			//std::string resstr = PaddleOcrApi::GetPaddleOCRText(resultMat);
			//std::cout << "OCR:" << resstr << std::endl;

			std::vector<std::pair<std::string, cv::Rect>> vtsocrs;
			PaddleOcrApi::GetPaddleOCRTextRect(resultMat, vtsocrs);

			//输出识别文字
			//if (!resultMat.empty()) {
			//	putText::putTextZH(resultMat, resstr.data(), cv::Point(20, 20), cv::Scalar(0, 0, 255), 1);
			//	cv::putText(resultMat, resstr, cv::Point(20, 50), 1, 1, cv::Scalar(0, 0, 255));
			//}
			std::cout << "输出：" << std::endl;
			for (int i = 0; i < vtsocrs.size(); ++i) {
				int B = cv::theRNG().uniform(0, 255);
				int G = cv::theRNG().uniform(0, 255);
				int R = cv::theRNG().uniform(0, 255);

				cv::Rect tmprect = vtsocrs[i].second;
				std::string tmptext = "N" + std::to_string(i) + ":" + vtsocrs[i].first;
				cv::Point pt = cv::Point(tmprect.x,	tmprect.y);
				cv::rectangle(resultMat, tmprect, cv::Scalar(B, G, R));
				cv::putText(resultMat, tmptext, pt, 1, 1.2, cv::Scalar(B, G, R));

				std::cout << tmptext << std::endl;
			}


			CvUtils::SetShowWindow(resultMat, "cutMat", 600, 20);
			cv::imshow("cutMat", resultMat);
		}
		catch (const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}

		cv::waitKey(0);
	}


	return 0;
}