#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"

using namespace cv;
using namespace std;
//����ͼƬ����·��
String FilePaths = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVSplitImage/pic";


int main(int argc, char** argv) {
	//��ȡĿ¼�µ������ļ�
	vector<String> files;
	glob(FilePaths, files);


	if (files.size() <= 0) {
		cout << "�Ҳ���ͼƬ�ļ�" << endl;
		waitKey(0);
		return -1;
	}

	for (int index = 0; index < files.size(); ++index) {
		//�ر�������ʾ����
		destroyAllWindows();

		cout << "srcindex:" << index << endl;
		String file = files[index];
		Mat src = imread(file);
		CvUtils::MatResize(src);
		CvUtils::SetShowWindow(src, "src", 20, 20);
		imshow("src", src);

		//��˹�˲�
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
		
			//B����Canny
			//�������ֵ
			CvUtils::GetMatMinMaxThreshold(channels[0], minthreshold, maxthreshold, 1);
			cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << endl;
			//Canny��Ե��ȡ
			Canny(channels[0], B_src, minthreshold, maxthreshold);

			//�������ֵ
			CvUtils::GetMatMinMaxThreshold(channels[1], minthreshold, maxthreshold, 1);
			cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << endl;
			//Canny��Ե��ȡ
			Canny(channels[1], G_src, minthreshold, maxthreshold);

			//�������ֵ
			CvUtils::GetMatMinMaxThreshold(channels[2], minthreshold, maxthreshold, 1);
			cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << endl;
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


			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;
			findContours(dstmat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

			Mat dstcontour = Mat::zeros(dstmat.size(), CV_8SC3);
			Mat tmpcontour;
			dstcontour.copyTo(tmpcontour);

			//������Ϻ�Ķ��������
			vector<vector<Point>> vtshulls(contours.size());

			for (int i = 0; i < contours.size(); ++i) {
				//�ж�������״�������ı��εĺ��Ե�
				double lensval = 0.01 * arcLength(contours[i], true);
				vector<Point> convexhull;
				approxPolyDP(Mat(contours[i]), convexhull, lensval, true);

				//��ϵĶ���δ�ŵ������������
				vtshulls[i] = convexhull;

				//�����ı��εĹ��˵�
				if (convexhull.size() != 4) continue;

				//�����С��ת����
				RotatedRect rRect = minAreaRect(contours[i]);
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
				RotatedRect rRect = minAreaRect(contours[maxAreaidx]);
				Point2f vertices[4];
				//���������������㣬�����ϣ����ϣ����£�����˳��
				CvUtils::SortRotatedRectPoints(vertices, rRect);

				cout <<"Rect:" << vertices[0] << vertices[1] << vertices[2] << vertices[3] << endl;

				//���ݻ�õ�4���㻭��
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, vertices[k], vertices[(k + 1) % 4], Scalar(255, 0, 0));
				}

				//�����ı��ε��ĵ�����
				Point2f rPoints[4];
				CvUtils::GetPointsFromRect(rPoints, vertices, vtshulls[maxAreaidx]);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, rPoints[k], rPoints[(k + 1) % 4], Scalar(255, 255, 255));
				}


				//��������С�����ĸ���������������÷�Χ������������ĵ���ֱ������ٿ������
				Point2f newPoints[4];
				CvUtils::GetPointsFromFitline(newPoints, rPoints, vertices);
				for (int k = 0; k < 4; ++k) {
					line(dstcontour, newPoints[k], newPoints[(k + 1) % 4], Scalar(255, 100, 255));
				}


				//������С���κͶ������ϵ�����ĸ������͸�ӱ任����		
				Point2f rectPoint[4];
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

				rectPoint[0] = Point2f(left, top);
				rectPoint[1] = rectPoint[0] + Point2f(rWidth, 0);
				rectPoint[2] = rectPoint[1] + Point2f(0, rHeight);
				rectPoint[3] = rectPoint[0] + Point2f(0, rHeight);


				//����͸�ӱ任����		
				Mat warpmatrix = getPerspectiveTransform(rPoints, rectPoint);
				Mat resultimg;
				//͸�ӱ任
				warpPerspective(src, resultimg, warpmatrix, resultimg.size(), INTER_LINEAR);

				/*CvUtils::SetShowWindow(resultimg, "resultimg", 200, 20);
				imshow("resultimg", resultimg);*/

				//��ȡ͸�ӱ任���ͼ����ʾ����
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