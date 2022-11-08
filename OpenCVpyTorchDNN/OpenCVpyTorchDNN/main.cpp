#pragma once
#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/dnn/dnn.hpp>

using namespace cv;
using namespace std;

dnn::Net net;

int main(int argc, char** argv) {
	cout << CV_VERSION << endl;
	//����onnx�ļ�
	string onnxfile = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVpyTorchDNN/OpenCVpyTorchDNN/test.onnx";

	net = dnn::readNetFromONNX(onnxfile);
	if (!net.empty()) {
		//��Mat������ֵ
		float value = 1024;
		Mat inputBlob = Mat(1, 1, CV_32F, Scalar(value));
		//�������ֵ
		net.setInput(inputBlob, "input");
		//Ԥ���� 
		Mat output = net.forward("output");

		cout << "����ֵ��" << value << endl;
		cout << "Ԥ��ֵ��" << output.at<float>(0, 0) << endl;
	}
	else
	{
		cout << "����Onnx�ļ�ʧ�ܣ�" << endl;
	}

	waitKey(0);
	return 0;
}