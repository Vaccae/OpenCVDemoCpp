#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"


using namespace std;
using namespace cv;

//����ͼƬ����·��
String FilePaths = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVSplitImage/pic";
//��ȡĿ¼�µ������ļ�
vector<String> files;


//���ص�����
void onMouse(int event, int x, int y, int flags, void* ustc);

Mat src;
Mat srccopy; //���ڿ�������Դͼ��
string showsrc = "ͼ��";
int imgindex = 0;
//����͸�ӱ任�ĵ�
Point2f vertices[4];


//��͸�ӱ任����и�ֵ,����ֵΪ3ʱ,˵��4���㶼�Ѿ���ֵ�ˣ����Խ�����һ������
int setPerspectivePoint(Point2f* vts, int x, int y, bool isinit = false);


int main(int argc, char** argv) {
	glob(FilePaths, files);

	if (files.size() <= 0) {
		cout << "�Ҳ���ͼƬ�ļ�" << endl;
		waitKey(0);
		return -1;
	}

	//��ʼ��͸�ӱ任�ĵ�
	setPerspectivePoint(vertices, 0, 0, true);

	//�ر�������ʾ����
	destroyAllWindows();

	cout << "srcindex:" << imgindex << endl;
	String file = files[imgindex];
	src = imread(file);
	CvUtils::MatResize(src);
	CvUtils::SetShowWindow(src, showsrc, 50, 20);
	imshow(showsrc, src);
	//������Դͼ
	src.copyTo(srccopy);

	//���������Ӱ�¼�
	setMouseCallback(showsrc, onMouse);

	waitKey(0);

	return 0;
}


void onMouse(int event, int x, int y, int flags, void* ustc)
{
	//����������
	if (event == EVENT_LBUTTONUP)
	{
		//����ѡ��ĵ�
		int ptindex = setPerspectivePoint(vertices, x, y);

		//��ͼ���ϻ������λ��
		circle(src, Point(x, y), 3, Scalar(255, 0, 0), -1);
		//ѡ�еĵ���л���
		if (ptindex > 0 && ptindex <= 3) {
			line(src, vertices[ptindex], vertices[ptindex - 1], Scalar(255, 0, 0), 3);
			//�������һ����ʱ����ʼ����л�������
			if (ptindex == 3) {
				line(src, vertices[ptindex], vertices[0], Scalar(255, 0, 0), 3);
			}
		}

		imshow(showsrc, src);

		if (ptindex == 3) {
			//������С���κͶ������ϵ�����ĸ������͸�ӱ任����		
			Point2f rectPoint[4];
			//������ת���εĿ�͸�
			float rWidth = CvUtils::CalcPointDistance(vertices[0], vertices[1]);
			float rHeight = CvUtils::CalcPointDistance(vertices[1], vertices[2]);
			//����͸�ӱ任���ĸ�����
			rectPoint[0] = Point2f(0, 0);
			rectPoint[1] = rectPoint[0] + Point2f(rWidth, 0);
			rectPoint[2] = rectPoint[1] + Point2f(0, rHeight);
			rectPoint[3] = rectPoint[0] + Point2f(0, rHeight);


			//����͸�ӱ任����		
			Mat warpmatrix = getPerspectiveTransform(vertices, rectPoint);
			Mat resultimg;
			//͸�ӱ任
			warpPerspective(srccopy, resultimg, warpmatrix, resultimg.size(), INTER_LINEAR);

			//��ȡ͸�ӱ任���ͼ����ʾ����
			Rect cutrect = Rect(rectPoint[0], rectPoint[2]);
			Mat cutMat = resultimg(cutrect);

			CvUtils::SetShowWindow(cutMat, "cutMat", 600, 20);
			imshow("cutMat", cutMat);
		}

	}
	else if (event == EVENT_RBUTTONUP) {
		//��ʼ��͸�ӱ任�ĵ�
		setPerspectivePoint(vertices, 0, 0, true);

		imgindex++;
		if (imgindex < files.size()) {
			//�ر�������ʾ����
			destroyAllWindows();

			cout << "srcindex:" << imgindex << endl;
			String file = files[imgindex];
			src = imread(file);
			CvUtils::MatResize(src);
			CvUtils::SetShowWindow(src, showsrc, 50, 20);
			imshow(showsrc, src);

			//������Դͼ
			src.copyTo(srccopy);

			//���������Ӱ�¼�
			setMouseCallback(showsrc, onMouse);
		}
		waitKey(0);
	}
}


//��͸�ӱ任����и�ֵ,����ֵΪtrueʱ,˵��4���㶼�Ѿ���ֵ�ˣ����Խ�����һ������
int setPerspectivePoint(Point2f* vts, int x, int y, bool isinit)
{
	int res = 0;
	if (isinit) {
		for (int i = 0; i < 4; ++i) {
			vts[i].x = -1.0f;
			vts[i].y = -1.0f;
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			if (vts[i].x == -1.0f && vts[i].y == -1.0f) {
				res = i;
				vts[i].x = x;
				vts[i].y = y;
				break;
			}
		}
	}
	return res;
}
