#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "..\..\Utils\CvUtils.h"
#include "PaddleOcrApi.h"
#include "..\..\Utils\putText.h"


//����ͼƬ����·��
cv::String FilePaths = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVSplitImage/pic";


int main(int argc, char** argv) {
	//��ȡĿ¼�µ������ļ�
	std::vector<cv::String> files;
	cv::glob(FilePaths, files);


	if (files.size() <= 0) {
		std::cout << "�Ҳ���ͼƬ�ļ�" << std::endl;
		cv::waitKey(0);
		return -1;
	}

	for (int index = 0; index < files.size(); ++index) {
		//�ر�������ʾ����
		cv::destroyAllWindows();

		std::cout << "srcindex:" << index << std::endl;
		cv::String file = files[index];
		cv::Mat src = cv::imread(file);
		CvUtils::MatResize(src);
		CvUtils::SetShowWindow(src, "src", 20, 20);
		cv::imshow("src", src);

		//��˹�˲�
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

			//B����Canny
			//�������ֵ
			CvUtils::GetMatMinMaxThreshold(channels[0], minthreshold, maxthreshold, 1);
			std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
			//Canny��Ե��ȡ
			cv::Canny(channels[0], B_src, minthreshold, maxthreshold);

			//�������ֵ
			CvUtils::GetMatMinMaxThreshold(channels[1], minthreshold, maxthreshold, 1);
			std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
			//Canny��Ե��ȡ
			Canny(channels[1], G_src, minthreshold, maxthreshold);

			//�������ֵ
			CvUtils::GetMatMinMaxThreshold(channels[2], minthreshold, maxthreshold, 1);
			std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
			//Canny��Ե��ȡ
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

			//������Ϻ�Ķ��������
			std::vector<std::vector<cv::Point>> vtshulls(contours.size());

			for (int i = 0; i < contours.size(); ++i) {
				//�ж�������״�������ı��εĺ��Ե�
				double lensval = 0.01 * arcLength(contours[i], true);
				std::vector<cv::Point> convexhull;
				approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);

				//��ϵĶ���δ�ŵ������������
				vtshulls[i] = convexhull;

				//�����ı��εĹ��˵�
				if (convexhull.size() != 4) continue;

				//�����С��ת����
				cv::RotatedRect rRect = minAreaRect(contours[i]);
				//������С��ת�������������ֵ
				if (rRect.size.height == 0) continue;

				if (rRect.size.area() > maxArea && rRect.size.area() > srcArea * 0.1
					&& !CvUtils::CheckRectBorder(src, rRect)) {
					maxArea = rRect.size.area();
					maxAreaidx = i;
				}
			}

			//�ҵ�������������������������д���
			if (maxAreaidx >= 0) {
				//��ȡ��С��ת����
				cv::RotatedRect rRect = minAreaRect(contours[maxAreaidx]);
				cv::Point2f vertices[4];
				//���������������㣬�����ϣ����ϣ����£�����˳��
				CvUtils::SortRotatedRectPoints(vertices, rRect);

				std::cout << "Rect:" << vertices[0] << vertices[1] << vertices[2] << vertices[3] << std::endl;

				//���ݻ�õ�4���㻭��
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, vertices[k], vertices[(k + 1) % 4], cv::Scalar(255, 0, 0));
				}

				//�����ı��ε��ĵ�����
				cv::Point2f rPoints[4];
				CvUtils::GetPointsFromRect(rPoints, vertices, vtshulls[maxAreaidx]);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, rPoints[k], rPoints[(k + 1) % 4], cv::Scalar(255, 255, 255));
				}


				//��������С�����ĸ���������������÷�Χ������������ĵ���ֱ������ٿ������
				cv::Point2f newPoints[4];
				CvUtils::GetPointsFromFitline(newPoints, rPoints, vertices);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, newPoints[k], newPoints[(k + 1) % 4], cv::Scalar(255, 100, 255));
				}


				//������С���κͶ������ϵ�����ĸ������͸�ӱ任����		
				cv::Point2f rectPoint[4];
				//������ת���εĿ�͸�
				float rWidth = CvUtils::CalcPointDistance(vertices[0], vertices[1]);
				float rHeight = CvUtils::CalcPointDistance(vertices[1], vertices[2]);
				//����͸�ӱ任�����Ͻ���ʼ��
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


				//����͸�ӱ任����		
				cv::Mat warpmatrix = getPerspectiveTransform(rPoints, rectPoint);
				cv::Mat resultimg;
				//͸�ӱ任
				warpPerspective(src, resultimg, warpmatrix, resultimg.size(), cv::INTER_LINEAR);

				/*CvUtils::SetShowWindow(resultimg, "resultimg", 200, 20);
				imshow("resultimg", resultimg);*/

				//��ȡ͸�ӱ任���ͼ����ʾ����
				cv::Rect cutrect = cv::Rect(rectPoint[0], rectPoint[2]);
				cv::Mat cutMat = resultimg(cutrect);

				//ʹ��PaddleOCRʶ��
				std::string resstr = PaddleOcrApi::GetPaddleOCRText(cutMat);
				std::cout << "OCR:" << resstr << std::endl;

				//���ʶ������
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