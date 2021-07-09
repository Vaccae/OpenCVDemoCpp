#include "putText.h"

void putText::GetStringSize(HDC hdc, const char* str, int* w, int* h)
{
	SIZE size;
	GetTextExtentPoint32A(hdc, str, strlen(str), &size);
	if (w != 0) *w = size.cx;
	if (h != 0) *h = size.cy;
}

void putText::putTextZH(Mat& dst, const char* str, Point org, Scalar color, int fontsize, const char* fn, bool italic, bool underline)
{
	CV_Assert(dst.data != 0 && (dst.channels() == 1 || dst.channels() == 3));

	int x, y, r, b;
	//坐标点大于图像宽和高直接返回
	if (org.x > dst.cols || org.y > dst.rows) return;
	x = org.x < 0 ? -org.x : 0;
	y = org.y < 0 ? -org.y : 0;

	LOGFONTA lf;
	lf.lfHeight = -fontsize; //字体高度
	lf.lfWidth = 0;          //平均宽度
	lf.lfEscapement = 0;   //字符排列角度
	lf.lfOrientation = 0;   //字符本身旋转的角度
	lf.lfWeight = 5;        //设置字体线条的宽度
	lf.lfItalic = italic; //斜体
	lf.lfUnderline = underline; //下划线
	lf.lfStrikeOut = 0;     //是否字符中央加横线
	lf.lfCharSet = DEFAULT_CHARSET;   //字符集
	lf.lfOutPrecision = 0;  //字体的精确度
	lf.lfQuality = PROOF_QUALITY;  //字体质量
	lf.lfPitchAndFamily = 0; //选择字体的间距和字体家族
	strcpy_s(lf.lfFaceName, fn); //字体的名称

	HFONT hf = CreateFontIndirectA(&lf);
	HDC hdc = CreateCompatibleDC(0);
	HFONT holdfont = (HFONT)SelectObject(hdc, hf);

	int strBaseW = 0, strBaseH = 0;
	int singleRow = 0;
	char buf[1 << 12];
	strcpy_s(buf, str);
	char* bufT[1 << 12];   //这个用于分隔字符串后剩余的字符，可能会超出。

	//处理多行
    {
        int nnh = 0;
        int cw, ch;

        const char* ln = strtok_s(buf, "\n", bufT);
        while (ln != 0)
        {
            GetStringSize(hdc, ln, &cw, &ch);
            strBaseW = max(strBaseW, cw);
            strBaseH = max(strBaseH, ch);

            ln = strtok_s(0, "\n", bufT);
            nnh++;
        }
        singleRow = strBaseH;
        strBaseH *= nnh;
    }

    if (org.x + strBaseW < 0 || org.y + strBaseH < 0)
    {
        SelectObject(hdc, holdfont);
        DeleteObject(hf);
        DeleteObject(hdc);
        return;
    }

    r = org.x + strBaseW > dst.cols ? dst.cols - org.x - 1 : strBaseW - 1;
    b = org.y + strBaseH > dst.rows ? dst.rows - org.y - 1 : strBaseH - 1;
    org.x = org.x < 0 ? 0 : org.x;
    org.y = org.y < 0 ? 0 : org.y;

    BITMAPINFO bmp = { 0 };
    BITMAPINFOHEADER& bih = bmp.bmiHeader;
    int strDrawLineStep = strBaseW * 3 % 4 == 0 ? strBaseW * 3 : (strBaseW * 3 + 4 - ((strBaseW * 3) % 4));

    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = strBaseW;
    bih.biHeight = strBaseH;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = BI_RGB;
    bih.biSizeImage = strBaseH * strDrawLineStep;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    void* pDibData = 0;
    HBITMAP hBmp = CreateDIBSection(hdc, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);

    CV_Assert(pDibData != 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, hBmp);

    //color.val[2], color.val[1], color.val[0]
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkColor(hdc, 0);
    //SetStretchBltMode(hDC, COLORONCOLOR);

    strcpy_s(buf, str);
    const char* ln = strtok_s(buf, "\n", bufT);
    int outTextY = 0;
    while (ln != 0)
    {
        TextOutA(hdc, 0, outTextY, ln, strlen(ln));
        outTextY += singleRow;
        ln = strtok_s(0, "\n", bufT);
    }
    uchar* dstData = (uchar*)dst.data;
    int dstStep = dst.step / sizeof(dstData[0]);
    unsigned char* pImg = (unsigned char*)dst.data + org.x * dst.channels() + org.y * dstStep;
    unsigned char* pStr = (unsigned char*)pDibData + x * 3;
    for (int tty = y; tty <= b; ++tty)
    {
        unsigned char* subImg = pImg + (tty - y) * dstStep;
        unsigned char* subStr = pStr + (strBaseH - tty - 1) * strDrawLineStep;
        for (int ttx = x; ttx <= r; ++ttx)
        {
            for (int n = 0; n < dst.channels(); ++n) {
                double vtxt = subStr[n] / 255.0;
                int cvv = vtxt * color.val[n] + (1 - vtxt) * subImg[n];
                subImg[n] = cvv > 255 ? 255 : (cvv < 0 ? 0 : cvv);
            }

            subStr += 3;
            subImg += dst.channels();
        }
    }

    SelectObject(hdc, hOldBmp);
    SelectObject(hdc, holdfont);
    DeleteObject(hf);
    DeleteObject(hBmp);
    DeleteDC(hdc);
}
