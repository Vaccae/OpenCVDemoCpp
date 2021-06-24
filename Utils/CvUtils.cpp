#include "CvUtils.h"

void CvUtils::SetShowWindow(Mat img, string winname, int pointx, int pointy)
{
	//������ʾ����
	namedWindow(winname, WindowFlags::WINDOW_NORMAL);
	//����ͼ����ʾ��С
	resizeWindow(winname, img.size());
	//����ͼ����ʾλ��
	moveWindow(winname, pointx, pointy);
}

void CvUtils::MatResize(Mat& frame, int maxwidth, int maxheight)
{
	double scale;
	//�ж�ͼ����ˮƽ���Ǵ�ֱ
	bool isHorizontal = frame.cols > frame.rows ? true : false;

	//����ˮƽ���Ǵ�ֱ��������
	if (isHorizontal) {
		if (frame.cols > maxwidth) {
			scale = (double)maxwidth / frame.cols;
			resize(frame, frame, Size(0, 0), scale, scale);
		}
	}
	else {
		if (frame.rows > maxheight) {
			scale = (double)maxheight / frame.rows;
			resize(frame, frame, Size(0, 0), scale, scale);
		}
	}
}

vector<Point> CvUtils::Pt2fsToPts(vector<Point2f>& vecpt2f)
{
	vector<Point> vecpt;
	for (Point2f item : vecpt2f) {
		Point pt = item;
		vecpt.push_back(pt);
	}
	return vecpt;
}

vector<Point2f> CvUtils::PtsToPt2fs(vector<Point>& vecpt)
{
	vector<Point2f> vecpt2f;
	for (Point item : vecpt) {
		Point2f pt = item;
		vecpt2f.push_back(pt);
	}
	return vecpt2f;
}

//��Mat����λ��
int CvUtils::GetMatMidVal(Mat& img)
{
	//�ж�������ǵ�ͨ��ֱ�ӷ���128
	if (img.channels() > 1) return 128;
	int rows = img.rows;
	int cols = img.cols;
	//��������
	float mathists[256] = { 0 };
	//��������0-255�ĸ���
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int val = img.at<uchar>(row, col);
			mathists[val]++;
		}
	}

	int calcval = rows * cols / 2;
	int tmpsum = 0;
	for (int i = 0; i < 256; ++i) {
		tmpsum += mathists[i];
		if (tmpsum > calcval) {
			return i;
		}
	}
	return 0;
}

//ʹ�ô��Mat����ֵ
int CvUtils::GetMatOTSU(Mat& img)
{
	//�ж�������ǵ�ͨ��ֱ�ӷ���128
	if (img.channels() > 1) return 128;
	int rows = img.rows;
	int cols = img.cols;
	//��������
	float mathists[256] = { 0 };
	//��������0-255�ĸ���
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int val = img.at<uchar>(row, col);
			mathists[val]++;
		}
	}

	//����Ҷȼ�����������ͼ���еı���
	float grayPro[256] = { 0 };
	int matSize = rows * cols;
	for (int i = 0; i < 256; ++i) {
		grayPro[i] = (float)mathists[i] / (float)matSize;
	}

	//���OTSU��ǰ���뱳���ָ������������ĻҶ�ֵ
	int calcval;
	int calcMax = 0;
	for (int i = 0; i < 256; ++i) {
		float w0 = 0, w1 = 0, u0tmp = 0, u1tmp = 0, u0 = 0, u1 = 0, u = 0, calctmp = 0;

		for (int k = 0; k < 256; ++k) {
			float curGray = grayPro[k];
			//���㱳������
			if (k <= i) {
				//��iΪ��ֵ���࣬��һ���ܵĸ���
				w0 += curGray;
				u0tmp += curGray * k;
			}
			//����ǰ������
			else {
				//��iΪ��ֵ���࣬��һ���ܵĸ���
				w1 += curGray;
				u1tmp += curGray * k;
			}
		}

		//�����һ��͵ڶ����ƽ���Ҷ�
		u0 = u0tmp / w0;
		u1 = u1tmp / w1;
		//�������ͼ���ƽ���Ҷ�
		u = u0tmp + u1tmp;

		//������䷽��
		calctmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);

		//���������䷽���������ֵ
		if (calctmp > calcMax) {
			calcMax = calctmp;
			calcval = i;
		}
	}

	return calcval;
}

//������Ӧ��ֵ����С�����ֵ
void CvUtils::GetMatMinMaxThreshold(Mat& img, int& minval, int& maxval, int calctype, float sigma)
{
	int midval;
	switch (calctype)
	{
	case 1: {
		midval = GetMatOTSU(img);
		break;
	}
	default:
		midval = GetMatMidVal(img);
		break;
	}
	cout << "midval:" << midval << endl;
	// �������ֵ
	minval = saturate_cast<uchar>((1.0 - sigma) * midval);
	minval = saturate_cast<uchar>(midval / 2);
	//�������ֵ
	maxval = saturate_cast<uchar>((1.0 + sigma) * midval);
	maxval = saturate_cast<uchar>(midval);
}


//�������ĵ�����Զ���ĸ���
void CvUtils::GetRectPoints(Point2f vetPoints[], Point2f center, vector<Point> convex)
{
	//������Զ��4���㣬0--���ϣ� 1--���ϣ� 2--����  3--����
	float ltdist = 0.0f;  //���ϵ������� 
	float rtdist = 0.0f;  //���ϵ������� 
	float rbdist = 0.0f;  //���µ������� 
	float lbdist = 0.0f;  //���µ�������

	for (auto curpoint : convex) {
		//�����ľ��� 
		float curdist = CalcPointDistance(center, curpoint);

		if (curpoint.x < center.x && curpoint.y < center.y)
		{
			//�ж��Ƿ�������
			if (curdist > ltdist) {
				ltdist = curdist;
				vetPoints[0] = curpoint;
			}
		}
		else if (curpoint.x > center.x && curpoint.y < center.y) {
			//�ж�������
			if (curdist > rtdist) {
				rtdist = curdist;
				vetPoints[1] = curpoint;
			}
		}
		else if (curpoint.x > center.x && curpoint.y > center.y) {
			//�ж�������
			if (curdist > rbdist) {
				rbdist = curdist;
				vetPoints[2] = curpoint;
			}
		}
		else if (curpoint.x < center.x && curpoint.y > center.y) {
			//�ж�������
			if (curdist > lbdist) {
				lbdist = curdist;
				vetPoints[3] = curpoint;
			}
		}

	}

}

//������С���ε���������ı��ε�
//��һ����Ϊ����ĵ㣬�ڶ�������Ϊ���ε�4���㣬������Ϊ�������ϵĵ� 
void CvUtils::GetPointsFromRect(Point2f vetPoints[], Point2f rectPoints[], vector<Point> convex)
{
	//������Զ��4���㣬0--���ϣ� 1--���ϣ� 2--����  3--����
	float ltdist = 99999999.9f;  //���ϵ������� 
	float rtdist = 99999999.9f;  //���ϵ������� 
	float rbdist = 99999999.9f;  //���µ������� 
	float lbdist = 99999999.9f;  //���µ�������
	float curdist = 0.0f; //��ǰ��ļ������


	for (auto curpoint : convex) {
		//�������ϵ�ľ��� 
		curdist = CalcPointDistance(rectPoints[0], curpoint);
		if (curdist < ltdist) {
			ltdist = curdist;
			vetPoints[0] = curpoint;
		}
		//�������Ͻǵĵ����
		curdist = CalcPointDistance(rectPoints[1], curpoint);
		if (curdist < rtdist) {
			rtdist = curdist;
			vetPoints[1] = curpoint;
		}
		//�������½ǵ�ľ���
		curdist = CalcPointDistance(rectPoints[2], curpoint);
		if (curdist < rbdist) {
			rbdist = curdist;
			vetPoints[2] = curpoint;
		}
		//�������½ǵ�ľ���
		curdist = CalcPointDistance(rectPoints[3], curpoint);
		if (curdist < lbdist) {
			lbdist = curdist;
			vetPoints[3] = curpoint;
		}
	}
}

//���¼������任��4�������
//˼·����ת���εĵ����һ����ȡ���ٽ����жϾ��룬���С����ֵ�����룬������ֵ������������ֵ����
void CvUtils::GetPointsFromFitline(Point2f newPoints[], Point2f vetPoints[], Point2f rectPoints[], float dist)
{
	//1.���¹滮�����
	float curdist = CalcPointDistance(rectPoints[0], vetPoints[0]);
	newPoints[0] = curdist <= dist ? rectPoints[0] : vetPoints[0] + Point2f(-dist, -dist);

	curdist = CalcPointDistance(rectPoints[1], vetPoints[1]);
	newPoints[1] = curdist <= dist ? rectPoints[1] : vetPoints[1] + Point2f(dist, -dist);

	curdist = CalcPointDistance(rectPoints[2], vetPoints[2]);
	newPoints[2] = curdist <= dist ? rectPoints[2] : vetPoints[2] + Point2f(dist, dist);

	curdist = CalcPointDistance(rectPoints[3], vetPoints[3]);
	newPoints[3] = curdist <= dist ? rectPoints[3] : vetPoints[3] + Point2f(-dist, dist);

	//��������Ϊ��С��ת���ε��������º��������������������
	vector<Point2f> lArea;
	lArea.push_back(newPoints[0]);
	lArea.push_back(vetPoints[0]);
	lArea.push_back(vetPoints[3]);
	lArea.push_back(newPoints[3]);

	//���������Ϊ��С��Ӿ����������Ϻ������������������
	vector<Point2f> tArea;
	tArea.push_back(newPoints[0]);
	tArea.push_back(newPoints[1]);
	tArea.push_back(vetPoints[1]);
	tArea.push_back(vetPoints[0]);

	//�Ҳ������Ϊ�������������º���С��Ӿ��ε����Ϻ��������
	vector<Point2f> rArea;
	rArea.push_back(vetPoints[1]);
	rArea.push_back(newPoints[1]);
	rArea.push_back(newPoints[2]);
	rArea.push_back(vetPoints[2]);

	//�ײ������Ϊ�������������º���С��Ӿ��ε������������
	vector<Point2f> bArea;
	bArea.push_back(vetPoints[3]);
	bArea.push_back(vetPoints[2]);
	bArea.push_back(newPoints[2]);
	bArea.push_back(newPoints[3]);

	//2.����������ֱ�����
	//�������
	Vec4f lLine;
	fitLine(lArea, lLine, DIST_L2, 0, 0.01, 0.01);
	//��������
	Vec4f tLine;
	fitLine(tArea, tLine, DIST_L2, 0, 0.01, 0.01);
	//�ұ�����
	Vec4f rLine;
	fitLine(rArea, rLine, DIST_L2, 0, 0.01, 0.01);
	//��������
	Vec4f bLine;
	fitLine(bArea, bLine, DIST_L2, 0, 0.01, 0.01);

	//3.����ֱ����ϵ���ÿ����ֱ�ߵĽ����Ϊ���ǵĶ���ζ���
	//���϶���
	newPoints[0] = GetCrossPoint(lLine, tLine);
	//���϶���
	newPoints[1] = GetCrossPoint(tLine, rLine);
	//���¶���
	newPoints[2] = GetCrossPoint(rLine, bLine);
	//���¶���
	newPoints[3] = GetCrossPoint(bLine, lLine);
}

//������ֱ�ߵĽ����
Point2f CvUtils::GetCrossPoint(Vec4f Line1, Vec4f Line2)
{
	double k1, k2;
	//Line1��б��
	k1 = Line1[1] / Line1[0];
	//Line2��б��
	k2 = Line2[1] / Line2[0];

	Point2f crossPoint;
	crossPoint.x = (k1 * Line1[2] - Line1[3] - k2 * Line2[2] + Line2[3]) / (k1 - k2);
	crossPoint.y = (k1 * k2 * (Line1[2] - Line2[2]) + k1 * Line2[3] - k2 * Line1[3]) / (k1 - k2);
	return crossPoint;
}

bool CvUtils::CheckRectBorder(Mat& img, RotatedRect rect)
{
	bool res = false;
	Point2f pts[4];
	rect.points(pts);

	for (auto item : pts) {
		if (item.x<0 || item.y<0 || item.x> img.cols || item.y>img.rows) {
			res = true;
			break;
		}
	}
	return res;
}

//����������ת���������
void CvUtils::SortRotatedRectPoints(Point2f vetPoints[], RotatedRect rect, int flag)
{
	rect.points(vetPoints);

	cout << vetPoints[0] << vetPoints[1] << vetPoints[2] << vetPoints[3] << endl;
	cout << rect.angle << endl;

	Point2f curpoint;

	if (flag == 0) {
		//��X������
		for (int i = 0; i < 4; ++i) {
			for (int k = i + 1; k < 4; ++k) {
				if (vetPoints[i].x > vetPoints[k].x) {
					curpoint = vetPoints[i];
					vetPoints[i] = vetPoints[k];
					vetPoints[k] = curpoint;
				}
			}
		}

		//�ж�X����ǰ���������������½�
		if (vetPoints[0].y > vetPoints[1].y) {
			curpoint = vetPoints[0];
			vetPoints[0] = vetPoints[1];
			vetPoints[1] = vetPoints[3];
			vetPoints[3] = curpoint;
		}
		else {
			curpoint = vetPoints[3];
			vetPoints[3] = vetPoints[1];
			vetPoints[1] = curpoint;
		}

		//�ж�X��������������������½�
		if (vetPoints[1].y > vetPoints[2].y) {
			curpoint = vetPoints[1];
			vetPoints[1] = vetPoints[2];
			vetPoints[2] = curpoint;
		}
	}
	else {
		//����Rect������㣬Y������ΪP[0]��p[0]Χ��center˳ʱ����ת, 
		//��ת�Ƕ�Ϊ���Ļ�����P[0]�����½ǣ�Ϊ��P[0]�����½�
		//�������������
		if (rect.angle < 0) {
			curpoint = vetPoints[0];
			vetPoints[0] = vetPoints[2];
			vetPoints[2] = curpoint;
			curpoint = vetPoints[1];
			vetPoints[1] = vetPoints[3];
			vetPoints[3] = curpoint;
		}
		else if (rect.angle > 0) {
			curpoint = vetPoints[0];
			vetPoints[0] = vetPoints[1];
			vetPoints[1] = vetPoints[2];
			vetPoints[2] = vetPoints[3];
			vetPoints[3] = curpoint;
		}
	}
}


//���������ľ���
float CvUtils::CalcPointDistance(Point2f point1, Point2f point2)
{
	//�����������Point��ֵ
	Point2f tmppoint = point1 - point2;
	//����ŷ����¾������H
	return sqrt(pow(tmppoint.x, 2) + pow(tmppoint.y, 2));
}
