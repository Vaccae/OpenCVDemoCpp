#pragma once
//通过调用windowsAPI 来加载和卸载DLL  
#include <Windows.h>  
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include "..\..\Utils\CvUtils.h"

struct OCRTextRect {
public:
	char* OCRText;  //识别的信息
	int ptx, pty;       //Rect的起始坐标
	int width, height;  //Rect的宽和高

	OCRTextRect() {
		OCRText = nullptr;
		ptx = 0;
		pty = 0;
		width = 0;
		height = 0;
	}
};

class PaddleOcrApi
{
private:
	typedef char*(*DllFun)(cv::Mat&);

	typedef int (*DllFunOCRTextRect)(cv::Mat&, OCRTextRect*);

	//二分查找
	static int binarySearch(std::vector<std::pair<std::string, cv::Rect>>& vtsrect, const OCRTextRect rect);

public:
	static std::string GetPaddleOCRText(cv::Mat& src);

	static std::string GetPaddleOCRTextRect(cv::Mat& src, std::vector<std::pair<std::string, cv::Rect>>& vtsocr);

	//排序OCRTextRect转为vector容器
	static std::vector<std::pair<std::string, cv::Rect>> SortRectPair(const OCRTextRect* vtsrect, const int count);
	//透视变换获取图像
	static cv::Mat GetPerspectiveMat(cv::Mat& src, int iterations = 1);

	//分割数据华容道图像
	static std::vector<cv::Mat> GetNumMat(cv::Mat& src);

	// string的编码方式为utf8，则采用：
	static std::string wstr2utf8str(const std::wstring& str);
	static std::wstring utf8str2wstr(const std::string& str);

	// string的编码方式为除utf8外的其它编码方式，可采用：
	static std::string wstr2str(const std::wstring& str, const std::string& locale);
	static std::wstring str2wstr(const std::string& str, const std::string& locale);

};

