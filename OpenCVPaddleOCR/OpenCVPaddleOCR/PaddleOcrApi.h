#pragma once
//ͨ������windowsAPI �����غ�ж��DLL  
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

	// string�ı��뷽ʽΪutf8������ã�
	static std::string wstr2utf8str(const std::wstring& str);
	static std::wstring utf8str2wstr(const std::string& str);

	// string�ı��뷽ʽΪ��utf8����������뷽ʽ���ɲ��ã�
	static std::string wstr2str(const std::wstring& str, const std::string& locale);
	static std::wstring str2wstr(const std::string& str, const std::string& locale);

};

