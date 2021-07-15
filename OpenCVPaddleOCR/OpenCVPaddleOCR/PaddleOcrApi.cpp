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
			resstr = "���ز���PaddleOCRExport.dll��̬�⣡";
			FreeLibrary(hdll);
			return resstr;
		}

		funName = (DllFun)GetProcAddress(hdll, "PaddleOCRText");
		if (funName == NULL)
		{
			resstr = "�Ҳ���PaddleOCRText������";
			FreeLibrary(hdll);
			return resstr;
		}

		resstr = funName(src);
		// ��utf-8��stringת��Ϊwstring
		std::wstring wtxt = utf8str2wstr(resstr);
		// �ٽ�wstringת��Ϊgbk��string
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

	//��˹�˲�
	cv::GaussianBlur(tmpsrc, tmpsrc, cv::Size(5, 5), 0.5, 0.5);

	int srcArea = tmpsrc.size().area();
	float maxArea = 0;
	int maxAreaidx = -1;

	std::vector<cv::Mat> channels;
	cv::Mat B_src, G_src, R_src, dstmat;
	cv::split(tmpsrc, channels);

	int minthreshold = 120, maxthreshold = 200;

	//B����Canny
	//�������ֵ
	CvUtils::GetMatMinMaxThreshold(channels[0], minthreshold, maxthreshold, 1);
	std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
	//Canny��Ե��ȡ
	cv::Canny(channels[0], B_src, minthreshold, maxthreshold);

	//�������ֵ
	CvUtils::GetMatMinMaxThreshold(channels[1], minthreshold, maxthreshold, 1);
	std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
	//Canny��Ե��ȡ
	Canny(channels[1], G_src, minthreshold, maxthreshold);

	//�������ֵ
	CvUtils::GetMatMinMaxThreshold(channels[2], minthreshold, maxthreshold, 1);
	std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
	//Canny��Ե��ȡ
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

	//������Ϻ�Ķ��������
	std::vector<std::vector<cv::Point>> vtshulls(contours.size());

	for (int i = 0; i < contours.size(); ++i) {
		//�ж�������״�������ı��εĺ��Ե�
		double lensval = 0.01 * arcLength(contours[i], true);
		std::vector<cv::Point> convexhull;
		approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);

		//��ϵĶ���δ�ŵ������������
		vtshulls[i] = convexhull;

		//�����ı��εĹ��˵�
		if (convexhull.size() != 4) continue;

		//�����С��ת����
		cv::RotatedRect rRect = minAreaRect(contours[i]);
		//������С��ת�������������ֵ
		if (rRect.size.height == 0) continue;

		if (rRect.size.area() > maxArea && rRect.size.area() > srcArea * 0.1
			&& !CvUtils::CheckRectBorder(src, rRect)) {
			maxArea = rRect.size.area();
			maxAreaidx = i;
		}
	}

	//�ҵ�������������������������д���
	if (maxAreaidx >= 0) {
		std::cout << "iterations:" << iterations << "  maxAreaidx:" << maxAreaidx << std::endl;
		//��ȡ��С��ת����
		cv::RotatedRect rRect = minAreaRect(contours[maxAreaidx]);
		cv::Point2f vertices[4];
		//���������������㣬�����ϣ����ϣ����£�����˳��
		CvUtils::SortRotatedRectPoints(vertices, rRect);

		std::cout << "Rect:" << vertices[0] << vertices[1] << vertices[2] << vertices[3] << std::endl;

		//���ݻ�õ�4���㻭��
		for (int k = 0; k < 4; ++k) {
			line(dstcontour, vertices[k], vertices[(k + 1) % 4], cv::Scalar(255, 0, 0));
		}

		//�����ı��ε��ĵ�����
		cv::Point2f rPoints[4];
		CvUtils::GetPointsFromRect(rPoints, vertices, vtshulls[maxAreaidx]);
		for (int k = 0; k < 4; ++k) {
			line(dstcontour, rPoints[k], rPoints[(k + 1) % 4], cv::Scalar(255, 255, 255));
		}


		//��������С�����ĸ���������������÷�Χ������������ĵ���ֱ������ٿ������
		cv::Point2f newPoints[4];
		CvUtils::GetPointsFromFitline(newPoints, rPoints, vertices);
		for (int k = 0; k < 4; ++k) {
			line(dstcontour, newPoints[k], newPoints[(k + 1) % 4], cv::Scalar(255, 100, 255));
		}


		//������С���κͶ������ϵ�����ĸ������͸�ӱ任����		
		cv::Point2f rectPoint[4];
		//������ת���εĿ�͸�
		float rWidth = CvUtils::CalcPointDistance(vertices[0], vertices[1]);
		float rHeight = CvUtils::CalcPointDistance(vertices[1], vertices[2]);
		//����͸�ӱ任�����Ͻ���ʼ��
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


		//����͸�ӱ任����		
		cv::Mat warpmatrix = getPerspectiveTransform(rPoints, rectPoint);
		cv::Mat resultimg;
		//͸�ӱ任
		warpPerspective(src, resultimg, warpmatrix, resultimg.size(), cv::INTER_LINEAR);

		/*CvUtils::SetShowWindow(resultimg, "resultimg", 200, 20);
		imshow("resultimg", resultimg);*/

		//��ȡ͸�ӱ任���ͼ����ʾ����
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

	//ʹ��������˹����ʵ��ͼ��Աȶ����
	cv::Mat Laplancekernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
	cv::Mat imgLaplance, resimg;
	cv::filter2D(tmpsrc, imgLaplance, CV_32F, Laplancekernel);
	tmpsrc.convertTo(resimg, CV_32F);
	resimg = resimg - imgLaplance;
	resimg.convertTo(tmpsrc, CV_8UC3);
	CvUtils::SetShowWindow(tmpsrc, "resimg", 700, 20);
	cv::imshow("resimg", tmpsrc);


	cv::cvtColor(tmpsrc, tmpgray, cv::COLOR_BGR2GRAY);

	//��ֵ��
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

	//������
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
	////������Ϻ�Ķ��������
	std::vector<std::vector<cv::Point>> vtshulls;

	//for (int i = 0; i < contours.size(); ++i) {
	//	//cv::drawContours(tmpsrc, contours, i, cv::Scalar(0, 0, 255));
	//	//�ж�������״�������ı��εĺ��Ե�
	//	double lensval = 0.01 * arcLength(contours[i], true);
	//	std::vector<cv::Point> convexhull;
	//	approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);

	//	//�����ı��εĹ��˵�
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
//    //��ֵ��
//    //cv::threshold(tmpgray, threshsrc, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
//
//    //int minthreshold = 120, maxthreshold = 200;
//    ////�������ֵ
//    //CvUtils::GetMatMinMaxThreshold(tmpgray, minthreshold, maxthreshold, 1);
//    //std::cout << "OTSUmin:" << minthreshold << "  OTSUmax:" << maxthreshold << std::endl;
//    ////Canny��Ե��ȡ
//    Canny(tmpgray, threshsrc, 0, 255);
//
//    CvUtils::SetShowWindow(threshsrc, "threshsrc", 700, 20);
//    cv::imshow("threshsrc", threshsrc);
//
//
//    //std::vector<std::vector<cv::Point>> contours;
//    //std::vector<cv::Vec4i> hierarchy;
//    //findContours(threshsrc, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
//    ////������Ϻ�Ķ��������
//    //std::vector<std::vector<cv::Point>> vtshulls;
//
//    //for (int i = 0; i < contours.size(); ++i) {
//    //    //�ж�������״�������ı��εĺ��Ե�
//    //    double lensval = 0.01 * arcLength(contours[i], true);
//    //    std::vector<cv::Point> convexhull;
//    //    approxPolyDP(cv::Mat(contours[i]), convexhull, lensval, true);
//
//    //    //�����ı��εĹ��˵�
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
//    //������
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
