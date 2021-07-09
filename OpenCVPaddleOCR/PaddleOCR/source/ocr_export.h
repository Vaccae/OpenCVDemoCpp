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

#define DLLEXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

	DLLEXPORT char* PaddleOCRText(cv::Mat& img);

#ifdef __cplusplus
}
#endif

PaddleOCR::OCRConfig readOCRConfig();
