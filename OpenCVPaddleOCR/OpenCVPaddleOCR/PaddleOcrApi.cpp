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
