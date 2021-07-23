#pragma once
//ͨ������windowsAPI �����غ�ж��DLL  
#include <Windows.h>  
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include "..\..\Utils\CvUtils.h"

struct OCRTextRect {
public:
	char* OCRText;  //ʶ�����Ϣ
	int ptx, pty;       //Rect����ʼ����
	int width, height;  //Rect�Ŀ�͸�

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

	//���ֲ���
	static int binarySearch(std::vector<std::pair<std::string, cv::Rect>>& vtsrect, const OCRTextRect rect);

public:
	static std::string GetPaddleOCRText(cv::Mat& src);

	static std::string GetPaddleOCRTextRect(cv::Mat& src, std::vector<std::pair<std::string, cv::Rect>>& vtsocr);

	//����OCRTextRectתΪvector����
	static std::vector<std::pair<std::string, cv::Rect>> SortRectPair(const OCRTextRect* vtsrect, const int count);
	//͸�ӱ任��ȡͼ��
	static cv::Mat GetPerspectiveMat(cv::Mat& src, int iterations = 1);

	//�ָ����ݻ��ݵ�ͼ��
	static std::vector<cv::Mat> GetNumMat(cv::Mat& src);

	// string�ı��뷽ʽΪutf8������ã�
	static std::string wstr2utf8str(const std::wstring& str);
	static std::wstring utf8str2wstr(const std::string& str);

	// string�ı��뷽ʽΪ��utf8����������뷽ʽ���ɲ��ã�
	static std::string wstr2str(const std::wstring& str, const std::string& locale);
	static std::wstring str2wstr(const std::string& str, const std::string& locale);

};

