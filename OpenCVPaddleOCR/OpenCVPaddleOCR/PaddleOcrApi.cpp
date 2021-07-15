#include "PaddleOcrApi.h"


std::string PaddleOcrApi::GetPaddleOCRText(cv::Mat& src)
{
	std::string resstr;
	DllFun funName;
	HINSTANCE hdll;

	try
	{
		hdll = LoadLibrary(L"PaddleOCRExport.dll");
		if (hdll == NULL)
		{
			resstr = "加载不到PaddleOCRExport.dll动态库！";
			FreeLibrary(hdll);
			return resstr;
		}

		funName = (DllFun)GetProcAddress(hdll, "PaddleOCRText");
		if (funName == NULL)
		{
			resstr = "找不到PaddleOCRText函数！";
			FreeLibrary(hdll);
			return resstr;
		}

		resstr = funName(src);
		// 将utf-8的string转换为wstring
		std::wstring wtxt = utf8str2wstr(resstr);
		// 再将wstring转换为gbk的string
		resstr = wstr2str(wtxt, "Chinese");

		FreeLibrary(hdll);
	}
	catch (const std::exception& ex)
	{
		resstr = ex.what();
		return "Error:" + resstr;
		FreeLibrary(hdll);
	}

	return resstr;
}

cv::Mat PaddleOcrApi::GetPerspectiveMat(cv::Mat& src, int iterations)
{
	cv::Mat tmpsrc, cannysrc, resultMat;
	src.copyTo(tmpsrc);

	//高斯滤波
	cv::GaussianBlur(tmpsrc, tmpsrc, cv::Size(5, 5), 0.5, 0.5);

	int srcArea = tmpsrc.size().area();
	float maxArea = 0;
	int maxAreaidx = -1;

	std::vector<cv::Mat> channels;
	cv::Mat B_src, G_src, R_src, dstmat;
	cv::split(tmpsrc, channels);

	int minthreshold = 120, maxthreshold = 200;

	//B进行Canny
	//大津法求阈值
	CvUtils::GetMatMinMaxThreshold(channels[0], minthreshold, maxthreshold, 1);
	std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
	//Canny边缘提取
	cv::Canny(channels[0], B_src, minthreshold, maxthreshold);

	//大津法求阈值
	CvUtils::GetMatMinMaxThreshold(channels[1], minthreshold, maxthreshold, 1);
	std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
	//Canny边缘提取
	Canny(channels[1], G_src, minthreshold, maxthreshold);

	//大津法求阈值
	CvUtils::GetMatMinMaxThreshold(channels[2], minthreshold, maxthreshold, 1);
	std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
	//Canny边缘提取
	Canny(channels[2], R_src, minthreshold, maxthreshold);


	bitwise_or(B_src, G_src, dstmat);
	bitwise_or(R_src, dstmat, dstmat);
	//CvUtils::SetShowWindow(dstmat, "dstmat", 700, 20);
	//imshow("dstmat", dstmat);


	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	findContours(dstmat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	cv::Mat dstcontour = cv::Mat::zeros(cannysrc.size(), CV_8SC3);
	cv::Mat tmpcontour;
	dstcontour.copyTo(tmpcontour);

	//定义拟合后的多边形数组
	std::vector<std::vector<cv::Point>> vtshulls(contours.size());

	for (int i = 0; i < contours.size(); ++i) {
		//判断轮廓形状，不是四边形的忽略掉
		double lensval = 0.01 * arcLength(contours[i], true);
		std::vector<cv::Point> convexhull;
		approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);

		//拟合的多边形存放到定义的数组中
		vtshulls[i] = convexhull;

		//不是四边形的过滤掉
		if (convexhull.size() != 4) continue;

		//求出最小旋转矩形
		cv::RotatedRect rRect = minAreaRect(contours[i]);
		//更新最小旋转矩形中面积最大的值
		if (rRect.size.height == 0) continue;

		if (rRect.size.area() > maxArea && rRect.size.area() > srcArea * 0.1
			&& !CvUtils::CheckRectBorder(src, rRect)) {
			maxArea = rRect.size.area();
			maxAreaidx = i;
		}
	}

	//找到符合条码的最大面积的轮廓进行处理
	if (maxAreaidx >= 0) {
		std::cout << "iterations:" << iterations << "  maxAreaidx:" << maxAreaidx << std::endl;
		//获取最小旋转矩形
		cv::RotatedRect rRect = minAreaRect(contours[maxAreaidx]);
		cv::Point2f vertices[4];
		//重新排序矩形坐标点，按左上，右上，右下，左下顺序
		CvUtils::SortRotatedRectPoints(vertices, rRect);

		std::cout << "Rect:" << vertices[0] << vertices[1] << vertices[2] << vertices[3] << std::endl;

		//根据获得的4个点画线
		for (int k = 0; k < 4; ++k) {
			line(dstcontour, vertices[k], vertices[(k + 1) % 4], cv::Scalar(255, 0, 0));
		}

		//计算四边形的四点坐标
		cv::Point2f rPoints[4];
		CvUtils::GetPointsFromRect(rPoints, vertices, vtshulls[maxAreaidx]);
		for (int k = 0; k < 4; ++k) {
			line(dstcontour, rPoints[k], rPoints[(k + 1) % 4], cv::Scalar(255, 255, 255));
		}


		//采用离最小矩形四个点最近的重新设置范围，将所在区域的点做直线拟合再看看结果
		cv::Point2f newPoints[4];
		CvUtils::GetPointsFromFitline(newPoints, rPoints, vertices);
		for (int k = 0; k < 4; ++k) {
			line(dstcontour, newPoints[k], newPoints[(k + 1) % 4], cv::Scalar(255, 100, 255));
		}


		//根据最小矩形和多边形拟合的最大四个点计算透视变换矩阵		
		cv::Point2f rectPoint[4];
		//计算旋转矩形的宽和高
		float rWidth = CvUtils::CalcPointDistance(vertices[0], vertices[1]);
		float rHeight = CvUtils::CalcPointDistance(vertices[1], vertices[2]);
		//计算透视变换的左上角起始点
		float left = dstcontour.cols;
		float top = dstcontour.rows;
		for (int i = 0; i < 4; ++i) {
			if (left > newPoints[i].x) left = newPoints[i].x;
			if (top > newPoints[i].y) top = newPoints[i].y;
		}

		rectPoint[0] = cv::Point2f(left, top);
		rectPoint[1] = rectPoint[0] + cv::Point2f(rWidth, 0);
		rectPoint[2] = rectPoint[1] + cv::Point2f(0, rHeight);
		rectPoint[3] = rectPoint[0] + cv::Point2f(0, rHeight);


		//计算透视变换矩阵		
		cv::Mat warpmatrix = getPerspectiveTransform(rPoints, rectPoint);
		cv::Mat resultimg;
		//透视变换
		warpPerspective(src, resultimg, warpmatrix, resultimg.size(), cv::INTER_LINEAR);

		/*CvUtils::SetShowWindow(resultimg, "resultimg", 200, 20);
		imshow("resultimg", resultimg);*/

		//载取透视变换后的图像显示出来
		cv::Rect cutrect = cv::Rect(rectPoint[0], rectPoint[2]);
		resultMat = resultimg(cutrect);

		//CvUtils::SetShowWindow(resultMat, "resultMat", 600, 20);
		//cv::imshow("resultMat", resultMat);

		iterations--;
		if (iterations > 0) {
			resultMat = GetPerspectiveMat(resultMat, iterations);
		}
	}
	else {
		src.copyTo(resultMat);
	}
	return resultMat;
}

std::vector<cv::Mat> PaddleOcrApi::GetNumMat(cv::Mat& src)
{
	std::vector<cv::Mat> vts;
	cv::Mat tmpsrc, tmpgray, threshsrc;
	src.copyTo(tmpsrc);

	//使用拉普拉斯算子实现图像对比度提高
	cv::Mat Laplancekernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
	cv::Mat imgLaplance, resimg;
	cv::filter2D(tmpsrc, imgLaplance, CV_32F, Laplancekernel);
	tmpsrc.convertTo(resimg, CV_32F);
	resimg = resimg - imgLaplance;
	resimg.convertTo(tmpsrc, CV_8UC3);
	CvUtils::SetShowWindow(tmpsrc, "resimg", 700, 20);
	cv::imshow("resimg", tmpsrc);


	cv::cvtColor(tmpsrc, tmpgray, cv::COLOR_BGR2GRAY);

	//二值化
	cv::threshold(tmpgray, threshsrc, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	CvUtils::SetShowWindow(threshsrc, "threshsrc", 700, 20);
	cv::imshow("threshsrc", threshsrc);

	cv::Mat dst;
	cv::distanceTransform(threshsrc, dst, cv::DIST_L1, 3, 5);
	CvUtils::SetShowWindow(dst, "dst1", 700, 20);
	cv::imshow("dst1", dst);

	cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
	CvUtils::SetShowWindow(dst, "dst2", 500, 20);
	cv::imshow("dst2", dst);

	cv::threshold(dst, dst, 0.1, 1, cv::THRESH_BINARY);
	CvUtils::SetShowWindow(dst, "dst3", 500, 20);
	cv::imshow("dst3", dst);


	//std::vector<cv::Vec4f> lines;
	//cv::HoughLinesP(dst_8u, lines, 1, CV_PI / 180.0, 200, 50, 40);

	//cv::Scalar color = cv::Scalar(0, 0, 255);
	//for (int i = 0; i < lines.size(); ++i) {
	//	cv::Vec4f line = lines[i];
	//	cv::putText(tmpsrc, std::to_string(i), cv::Point(line[0], line[1]), 1, 1, color);
	//	cv::line(tmpsrc, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color);
	//}
	//CvUtils::SetShowWindow(tmpsrc, "tmpsrc", 300, 20);
	//cv::imshow("tmpsrc", tmpsrc);

	//开运算
	cv::Mat morph1, morph2, morphcalc;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1));
	cv::morphologyEx(dst, morph1, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 1);
	CvUtils::SetShowWindow(morph1, "morph1", 500, 20);
	cv::imshow("morph1", morph1);

	//cv::morphologyEx(threshsrc, morph2, cv::MORPH_TOPHAT, kernel);
	//CvUtils::SetShowWindow(morph2, "morph2", 500, 20);
	//cv::imshow("morph2", morph2);

	//morphcalc = threshsrc - morph2;
	//CvUtils::SetShowWindow(morphcalc, "morphcalc", 500, 20);
	//cv::imshow("morphcalc", morphcalc);


	cv::Mat dst_8u;
	morph1.convertTo(dst_8u, CV_8U);
	CvUtils::SetShowWindow(dst_8u, "dst_8u", 300, 20);
	cv::imshow("dst_8u", dst_8u);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	findContours(dst_8u, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	////定义拟合后的多边形数组
	std::vector<std::vector<cv::Point>> vtshulls;

	//for (int i = 0; i < contours.size(); ++i) {
	//	//cv::drawContours(tmpsrc, contours, i, cv::Scalar(0, 0, 255));
	//	//判断轮廓形状，不是四边形的忽略掉
	//	double lensval = 0.01 * arcLength(contours[i], true);
	//	std::vector<cv::Point> convexhull;
	//	approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);

	//	//不是四边形的过滤掉
	//	if (convexhull.size() != 4) continue;
	//	vtshulls.push_back(convexhull);
	//}

	std::cout << "contourssize:" << contours.size() << std::endl;
	cv::Mat dstimg = cv::Mat::zeros(src.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); ++i) {
		cv::drawContours(dstimg, contours, static_cast<int>(i), cv::Scalar::all(255), -1);
	}

	CvUtils::SetShowWindow(dstimg, "dstimg", 300, 20);
	cv::imshow("dstimg", dstimg);


	return vts;
}
//std::vector<cv::Mat> PaddleOcrApi::GetNumMat(cv::Mat& src)
//{
//    std::vector<cv::Mat> vts;
//    cv::Mat tmpsrc,tmpgray,threshsrc;
//    src.copyTo(tmpsrc);
//
//    cv::GaussianBlur(tmpsrc, tmpsrc, cv::Size(3, 3), 0.5, 0.5);
//
//    cv::cvtColor(tmpsrc, tmpgray, cv::COLOR_BGR2GRAY);
//
//    cv::equalizeHist(tmpgray, tmpgray);
//
//    //二值化
//    //cv::threshold(tmpgray, threshsrc, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
//
//    //int minthreshold = 120, maxthreshold = 200;
//    ////大津法求阈值
//    //CvUtils::GetMatMinMaxThreshold(tmpgray, minthreshold, maxthreshold, 1);
//    //std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
//    ////Canny边缘提取
//    Canny(tmpgray, threshsrc, 0, 255);
//
//    CvUtils::SetShowWindow(threshsrc, "threshsrc", 700, 20);
//    cv::imshow("threshsrc", threshsrc);
//
//
//    //std::vector<std::vector<cv::Point>> contours;
//    //std::vector<cv::Vec4i> hierarchy;
//    //findContours(threshsrc, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
//    ////定义拟合后的多边形数组
//    //std::vector<std::vector<cv::Point>> vtshulls;
//
//    //for (int i = 0; i < contours.size(); ++i) {
//    //    //判断轮廓形状，不是四边形的忽略掉
//    //    double lensval = 0.01 * arcLength(contours[i], true);
//    //    std::vector<cv::Point> convexhull;
//    //    approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);
//
//    //    //不是四边形的过滤掉
//    //    if (convexhull.size() != 4) continue;
//
//    //    vtshulls.push_back(convexhull);
//    //}
//
//    //for (int i = 0; i < vtshulls.size(); ++i) {
//    //    cv::drawContours(tmpsrc, vtshulls, i, cv::Scalar(0, 0, 255));
//    //}
//
//
//    std::vector<cv::Vec4f> lines;
//    cv::HoughLinesP(threshsrc, lines, 1, CV_PI / 180.0, 200, 100, 40);
//
//    cv::Scalar color = cv::Scalar(0, 0, 255);
//    for (int i = 0; i < lines.size(); ++i) {
//        cv::Vec4f line = lines[i];
//        cv::putText(tmpsrc, std::to_string(i), cv::Point(line[0], line[1]), 1, 1, color);
//        cv::line(tmpsrc, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color);
//    }
//    CvUtils::SetShowWindow(tmpsrc, "tmpsrc", 700, 20);
//    cv::imshow("tmpsrc", tmpsrc);
//
//    //开运算
//    //cv::Mat morph1, morph2, morphcalc;
//    //cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
//    //cv::morphologyEx(threshsrc, morph1, cv::MORPH_ELLIPSE, kernel);
//    //CvUtils::SetShowWindow(morph1, "morph1", 500, 20);
//    //cv::imshow("morph1", morph1);
//
//    //cv::morphologyEx(threshsrc, morph2, cv::MORPH_TOPHAT, kernel);
//    //CvUtils::SetShowWindow(morph2, "morph2", 500, 20);
//    //cv::imshow("morph2", morph2);
//
//    //morphcalc = threshsrc - morph2;
//    //CvUtils::SetShowWindow(morphcalc, "morphcalc", 500, 20);
//    //cv::imshow("morphcalc", morphcalc);
//
//    return vts;
//}

std::string PaddleOcrApi::wstr2utf8str(const std::wstring& str)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;
	return strCnv.to_bytes(str);
}

std::wstring PaddleOcrApi::utf8str2wstr(const std::string& str)
{
	static std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
	return strCnv.from_bytes(str);
}

std::string PaddleOcrApi::wstr2str(const std::wstring& str, const std::string& locale)
{
	typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
	static std::wstring_convert<F> strCnv(new F(locale));
	return strCnv.to_bytes(str);
}

std::wstring PaddleOcrApi::str2wstr(const std::string& str, const std::string& locale)
{
	typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
	static std::wstring_convert<F> strCnv(new F(locale));
	return strCnv.from_bytes(str);
}
