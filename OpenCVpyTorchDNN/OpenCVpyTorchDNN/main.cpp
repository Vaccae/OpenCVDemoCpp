#pragma once
#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/dnn/dnn.hpp>

using namespace cv;
using namespace std;

dnn::Net net;

int main(int argc, char** argv) {
	cout << CV_VERSION << endl;
	//定义onnx文件
	string onnxfile = "D:/Business/DemoTEST/CPP/OpenCVDemoCpp/OpenCVpyTorchDNN/OpenCVpyTorchDNN/test.onnx";

	net = dnn::readNetFromONNX(onnxfile);
	if (!net.empty()) {
		//给Mat赋测试值
		float value = 1024;
		Mat inputBlob = Mat(1, 1, CV_32F, Scalar(value));
		//输入参数值
		net.setInput(inputBlob, "input");
		//预测结果 
		Mat output = net.forward("output");

		cout << "输入值：" << value << endl;
		cout << "预测值：" << output.at<float>(0, 0) << endl;
	}
	else
	{
		cout << "加载Onnx文件失败！" << endl;
	}

	waitKey(0);
	return 0;
}