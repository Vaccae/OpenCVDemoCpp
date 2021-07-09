#include <include/ocr_export.h>



DLLEXPORT char* PaddleOCRText(cv::Mat& img)
{
	std::vector<std::string> str_res;
	std::string tmpstr;

	if (!img.data) {
		return "could not read Mat ";
	}
	PaddleOCR::OCRConfig config  = readOCRConfig();
	//��ӡconfig����
	config.PrintConfigInfo();

	//ͼ�����ı�
	PaddleOCR::DBDetector det(config.det_model_dir, config.use_gpu, config.gpu_id,
		config.gpu_mem, config.cpu_math_library_num_threads,
		config.use_mkldnn, config.max_side_len, config.det_db_thresh,
		config.det_db_box_thresh, config.det_db_unclip_ratio,
		config.use_polygon_score, config.visualize,
		config.use_tensorrt, config.use_fp16);

	PaddleOCR::Classifier* cls = nullptr;
	if (config.use_angle_cls == true) {
		cls = new PaddleOCR::Classifier(config.cls_model_dir, config.use_gpu, config.gpu_id,
			config.gpu_mem, config.cpu_math_library_num_threads,
			config.use_mkldnn, config.cls_thresh,
			config.use_tensorrt, config.use_fp16);
	}

	PaddleOCR::CRNNRecognizer rec(config.rec_model_dir, config.use_gpu, config.gpu_id,
		config.gpu_mem, config.cpu_math_library_num_threads,
		config.use_mkldnn, config.char_list_file,
		config.use_tensorrt, config.use_fp16);

	//����ı���
	std::vector<std::vector<std::vector<int>>> boxes;
	det.Run(img, boxes);
	//OCRʶ��
	str_res = rec.RunOCR(boxes, img, cls);

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	for (auto item : str_res) {
		tmpstr += item;
	}

	char* reschar = new char[tmpstr.length() + 1];
	tmpstr.copy(reschar, std::string::npos);

	return reschar;
}

PaddleOCR::OCRConfig readOCRConfig()
{
	////��֤config.txt�ӱ�DLLĿ¼λ�ö�ȡ
   //��ȡDLL��������·��(�˴�����DLL�ļ���)
	char   DllPath[_MAX_PATH] = { 0 };
	getcwd(DllPath, _MAX_PATH);

	std::cout << DllPath << std::endl;

	strcat(DllPath, "\\config.txt");
	std::cout << DllPath << std::endl;
	////��ȡDLL����Ŀ¼(ȥ��DLL�ļ���)
	//char drive[_MAX_DRIVE];
	//char dir[_MAX_DIR];
	//char fname[_MAX_FNAME];
	//char ext[_MAX_EXT];
	//_splitpath(DllPath, drive, dir, fname, ext);
	////�ַ���ƴ��
	//strcat(dir, "config.txt");

	return PaddleOCR::OCRConfig(DllPath);
}



