#pragma once
//通过调用windowsAPI 来加载和卸载DLL  
#include <Windows.h>  
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

class PaddleOcrApi
{
private:
	typedef char*(*DllFun)(cv::Mat&);

public:
	static std::string GetPaddleOCRText(cv::Mat& src);

	// string的编码方式为utf8，则采用：
	static std::string wstr2utf8str(const std::wstring& str);
	static std::wstring utf8str2wstr(const std::string& str);

	// string的编码方式为除utf8外的其它编码方式，可采用：
	static std::string wstr2str(const std::wstring& str, const std::string& locale);
	static std::wstring str2wstr(const std::string& str, const std::string& locale);

};

