#include<iostream>
#include<opencv2/opencv.hpp>
#include "../../Utils/CvUtils.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {

	Mat src = imread("E:/DCIM/hrd/h1.jpg");
	CvUtils::MatResize(src);
	CvUtils::SetShowWindow(src, "src", 10, 20);
	imshow("src", src);

	Mat src2 = imread("E:/DCIM/hrd/h3.jpg");
	CvUtils::MatResize(src2);
	CvUtils::SetShowWindow(src2, "src2", 300, 20);
	imshow("src2", src2);

	//����Sift�Ļ�������
	int numFeatures = 500;
	//����detector��ŵ�KeyPoints��
	Ptr<SIFT> detector = SIFT::create(numFeatures);
	vector<KeyPoint> keypoints, keypoints2;
	detector->detect(src, keypoints);
	detector->detect(src2, keypoints2);
	//��ӡKeypoints
	cout << "Keypoints:" << keypoints.size() << endl;
	cout << "Keypoints2:" << keypoints2.size() << endl;

	Mat drawsrc, drawsrc2;
	drawKeypoints(src, keypoints, drawsrc);
	CvUtils::SetShowWindow(drawsrc, "drawsrc", 10, 20);
	imshow("drawsrc", drawsrc);
	drawKeypoints(src2, keypoints2, drawsrc2);
	CvUtils::SetShowWindow(drawsrc2, "drawsrc2", 300, 20);
	imshow("drawsrc2", drawsrc2);

	//����������������,����������ȡ
	Mat dstSIFT, dstSIFT2;
	Ptr<SiftDescriptorExtractor> descriptor = SiftDescriptorExtractor::create();
	descriptor->compute(src, keypoints, dstSIFT);
	descriptor->compute(src2, keypoints2, dstSIFT2);
	cout << dstSIFT.cols << endl;
	cout << dstSIFT2.rows << endl;

	//����BFMatch����ƥ��
	BFMatcher matcher(NORM_L2);
	//����ƥ��������
	vector<DMatch> matches;
	//ʵ��������֮���ƥ��
	matcher.match(dstSIFT, dstSIFT2, matches);

	//����������������ֵ����Сֵ
	double max_dist = 0;
	double min_dist = 1000;
	for (int i = 1; i < dstSIFT.rows; ++i)
	{
		//ͨ��ѭ�����¾��룬����ԽСԽƥ��
		double dist = matches[i].distance;
		if (dist > max_dist)
			max_dist = dist;
		if (dist < min_dist)
			min_dist = dist;
	}
	cout << "min_dist=" << min_dist << endl;
	cout << "max_dist=" << max_dist << endl;
	//ƥ����ɸѡ    
	vector<DMatch> goodMatches;
	for (int i = 0; i < matches.size(); ++i)
	{
		double dist = matches[i].distance;
		if (dist < 2 * min_dist)
			goodMatches.push_back(matches[i]);
	}
	cout << "goodMatches:" << goodMatches.size() << endl;

	Mat result;
	//ƥ������������ɫ����һ��������ɫ���
	drawMatches(src, keypoints, src2, keypoints2, goodMatches, result, 
		Scalar(255, 255, 0), Scalar::all(-1));
	CvUtils::SetShowWindow(result, "Result", 100, 20);
	imshow("Result", result);


	waitKey(0);
	return 0;
}