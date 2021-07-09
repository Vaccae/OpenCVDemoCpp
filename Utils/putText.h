#pragma once
#ifndef PUTTEXT_H_
#define PUTTEXT_H_

#include <Windows.h>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;

class putText
{
private:
	static void GetStringSize(HDC hdc, const char* str, int* w, int* h);

public:
	static void putTextZH(Mat& dst, const char* str, Point org, Scalar color, int fontsize,
		const char* fn = "Arial", bool italic = false, bool underline = false);
};

#endif // !PUTTEXT_H_
