#pragma once
#include <iostream>
#include <direct.h>  
#include <stdio.h> 
#include <codecvt> 
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <include/config.h>
#include <include/ocr_det.h>
#include <include/ocr_rec.h>

struct OCRTextRect {
public:
	char* OCRText;  //ʶ�����Ϣ
	int ptx, pty;       //Rect����ʼ����
	int width, height;  //Rect�Ŀ�͸�

	OCRTextRect() : OCRText(""), ptx(0), pty(0), width(0), height(0)
	{
	}
};

#define DLLEXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

	DLLEXPORT char* PaddleOCRText(cv::Mat& img);

	DLLEXPORT int PaddleOCRTextRect(cv::Mat& img, OCRTextRect *resptr);

#ifdef __cplusplus
}
#endif



PaddleOCR::OCRConfig readOCRConfig();
