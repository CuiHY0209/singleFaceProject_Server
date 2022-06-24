#include "RGBD2EIA.h"
extern long timeFlag[20];

RGBD2EIA::RGBD2EIA() {
	inputRGB = cv::Mat::zeros(720, 1280, CV_8UC3);
	inputDepth = cv::Mat::zeros(720, 1280, CV_8UC1);
	backgroundImg = cv::Mat::zeros(720, 1280, CV_8UC3);
	/*��ñ�˲�����*/
	blackhatThresh = 30;
	/*��С�����̵���ͨ������������С*/
	blackhatContourArea = 100;
}
RGBD2EIA::~RGBD2EIA() {
}


/*
* ���ܣ���ȡ��ʼ����
* ���루�ⲿ����RGB��Depth������ͼ·��
* ������ⲿ������Ӧ��CV����
*/
//bool RGBD2EIA::readImg(char* RGBImgPath, char* depthImgPath, char* backgroundImgPath) {
//	
//	/**
//	* ���룺RGBͼ��8λJPG��ʽ
//	* �����BGR8λ��ͨ������
//	*/
//	inputRGB = cv::imread(RGBImgPath, cv::IMREAD_COLOR);
//	if (inputRGB.data == NULL) {
//		std::cout << "��ȡRGBͼ���ִ���" << std::endl;
//		return false;
//	}
//	/**
//	*  ���룺Depthͼ��8λJPG��ʽ
//	*  ������Ҷ�ͼ8λ��ͨ������
//	*/
//	inputDepth = cv::imread(depthImgPath, cv::IMREAD_GRAYSCALE);
//	if (inputDepth.data == NULL) {
//		std::cout << "��ȡDepthͼ���ִ���" << std::endl;
//		return false;
//	}
//	
//	/**
//	*  ���룺RGB����ͼ��8λJPG��ʽ
//	*  �����BGRͼ8λ��ͨ������
//	*/
//	backgroundImg = cv::imread(backgroundImgPath, cv::IMREAD_COLOR);
//	if (backgroundImg.data == NULL) {
//		std::cout << "��ȡ����ͼ���ִ���" << std::endl;
//		return false;
//	}
//	return true;
//}


bool RGBD2EIA::readMat(cv::Mat rgb , cv::Mat depth , cv::Mat background) {

	/**
	* ���룺RGBͼ��8λJPG��ʽ
	* �����BGR8λ��ͨ������
	*/
	inputRGB= rgb.clone();
	/*cv::imshow("inputRGB", inputRGB);
	cv::waitKey(0);*/

	if (inputRGB.data == NULL) {
		std::cout << "��ȡRGBͼ���ִ���" << std::endl;
		return false;
	}
	/**
	*  ���룺Depthͼ��8λJPG��ʽ
	*  ������Ҷ�ͼ8λ��ͨ������
	*/
	cv::cvtColor(depth, inputDepth, cv::ColorConversionCodes::COLOR_BGR2GRAY, 1);
	if (inputDepth.data == NULL) {
		std::cout << "��ȡDepthͼ���ִ���" << std::endl;
		return false;
	}
	
	//cv::imshow("inputDepth", inputDepth);
	//cv::waitKey(0);

	/**
	*  ���룺RGB����ͼ��8λJPG��ʽ
	*  �����BGRͼ8λ��ͨ������
	*/
	backgroundImg = background.clone();
	if (backgroundImg.data == NULL) {
		std::cout << "��ȡ����ͼ���ִ���" << std::endl;
		return false;
	}
	cu_inputRGB.upload(inputRGB);
	cu_inputDepth.upload(inputDepth);
	cu_backgroundImg.upload(backgroundImg);
	return true;
}

/*
* ���ܣ���ʼ������
* ���룺��
* ��������ڲ�����CPU��BGR����Depth�����Ӳ�ͼ����EIA����
* ��������ڲ�����GPU��BGR����Depth�����Ӳ�ͼ����EIA����
* ��ע����newָ�붨�壡�м���delete[]����ڴ棡
*/
bool RGBD2EIA::initParameters(void) {	
	/*���ٽ��������õ��ڴ�ռ�*/
	outputParaImg = new cv::Mat[hardwarePara._VIEWNUM];
	for (int i = 0; i < hardwarePara._VIEWNUM; i++) {
		outputParaImg[i] = cv::Mat::zeros(inputRGB.size(), CV_8UC3);
	}

	/*�״���GPU�д�������*/
	//cu_inputRGB.upload(inputRGB);
	//cu_inputDepth.upload(inputDepth);
	//cu_backgroundImg.upload(backgroundImg);

	/*���ٽ������ݵ��Դ�*/
	cu_outputParaImg = new cv::cuda::GpuMat[hardwarePara._VIEWNUM];
	for (int i = 0; i < hardwarePara._VIEWNUM; i++) {
		cu_outputParaImg[i].upload(outputParaImg[i]);
	}
	
	/*���ٴ���EIA�õ��ڴ�ռ�*/
	eiaImg = cv::Mat::zeros(hardwarePara._HEIGHT, hardwarePara._WIDTH, CV_8UC3);
	/*���ٴ���EIA�õ��Դ�ռ�*/
	cu_eiaImg.upload(eiaImg);
	
	return true;
}


/*
* ���ܣ�Դͼ��Ԥ����������첿
* ���루���ڲ�����RGB��Depthͼ
* ��������ڲ����������첿����Depthͼ
*/
bool RGBD2EIA::prePoccess(void) {
	/*JBF�첿���*/
	/*if (!JBFFillMouth()){ 
		std::cout << "JBF�첿�����ִ���" << std::endl;
		return false;
	}*/
	/*�����첿���*/
	if (!simplifiedFillMouth()) {
		std::cout << "�����첿�����ִ���" << std::endl;
		return false;
	}	

	cu_inputDepth.download(inputDepth);
	/*cv::imshow("depth", inputDepth);
	cv::waitKey(1);*/
	return true;
}


/*
* ���ܣ���̬ѧͼ���˲������첿���
* ���루���ڲ�����RGBͼ��Depthͼ
* ��������ڲ����������첿���Depthͼ
*/
bool RGBD2EIA::JBFFillMouth(void) {
	//cv::Mat test;
	/*����һ��ȫ��������ڳ�ʼ��*/
	cv::Mat zeros = cv::Mat::zeros(inputDepth.rows, inputDepth.cols, CV_8UC1);
	/*���õ�ñ�˲�Ѱ���첿����*/	
	/*��BGRת��Ϊ8λ�Ҷ�ͼ*/
	cv::cuda::cvtColor(cu_inputRGB, cu_rgbGrey, cv::ColorConversionCodes::COLOR_BGR2GRAY, 1);
	
	/*��ñ�˲�(GPU)*/	
	cu_blackhatImg.upload(zeros);
	/*��̬ѧ�ں�*/
	element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(40, 40));
	/*Ӧ���˲���*/
	blackhatFilter = cv::cuda::createMorphologyFilter(cv::MORPH_BLACKHAT, CV_8UC1, element);
	blackhatFilter->apply(cu_inputDepth, cu_blackhatImg);
	
	/*��ֵ��(GPU)*/
	cv::cuda::threshold(cu_blackhatImg, blackhatImg_bin, blackhatThresh, 255, CV_THRESH_BINARY);
	//cv::imwrite("output/BlackHat_before.jpg", blackhatImg_bin);
	
	/*�޳�������С��100����ͨ�������ֳ�GPU����ʵ�֣�*/
	cv::findContours(blackhatImg_bin, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
	contours.erase(std::remove_if(contours.begin(), contours.end(),
		[](const std::vector<cv::Point>& c) {return cv::contourArea(c) < 100; }), contours.end());
	blackhatImg_bin.setTo(0);
	cv::drawContours(blackhatImg_bin, contours, -1, cv::Scalar(255), cv::FILLED);
	/*cv::imshow("output/BlackHat_after.jpg", blackhatImg_bin);
	cv::waitKey(1);*/
	/*����ֵ�����˲���������ͼ����gpu*/
	cu_blackhatImg.upload(blackhatImg_bin);

	/*����˫���˲�*/
	JointBilateralFilter();

	return true;
}


/*
* ���ܣ��첿�Ŀ��ټ������
* ���루���ڲ�����RGBͼ��Depthͼ
* ��������ڲ����������첿���Depthͼ
*/
bool RGBD2EIA::simplifiedFillMouth(void) {
	
	/*����ֵ*/
	double maxValue, minValue;
	cv::Point maxIdx, minIdx;
	cv::cuda::minMaxLoc(cu_inputDepth, &minValue, &maxValue, &minIdx, &maxIdx);

	if (!simplifiedFillMouth_CUDA(cu_inputDepth, maxIdx.x, maxIdx.y)) {
		std::cout << "Failed to run func: simplifiedFillMouth_CUDA" << std::endl;
		return false;
	}
	return true;
}


/*
* ���ܣ��ֲ�˫���˲���ʵ�ֽ��䰼���첿�
* ���루���ڲ�����RGBͼ��Depthͼ
* ��������ڲ����������첿���Depthͼ
*/
bool RGBD2EIA::JointBilateralFilter(void) {
	cv::cuda::GpuMat cu_depthSrc;
	cu_depthSrc.upload(inputDepth);
	if (!JointBilateralFilter_CUDA(cu_rgbGrey, cu_depthSrc, cu_blackhatImg, cu_inputDepth)) {
		std::cout << "Failed to run func: JointBilateralFilter_CUDA" << std::endl;
		return false;
	}
	return true;
}


/*
* ���ܣ��ϳ��Ӳ�ͼ�������ϳ�EIA
* ���루���ڲ�����RGBͼ��Depthͼ������ͼ��Ӳ������
* ��������ڲ�����EIA
* ������������ڲ������Ӳ�ͼ
*/
bool RGBD2EIA::genEIAImgs(void) {
	/*����.cu�е�����EIA*/
	if (!genEIAImgs_CUDA(cu_inputRGB, cu_inputDepth, cu_backgroundImg, hardwarePara, cu_outputParaImg, cu_eiaImg)) {
		std::cout << "Fshissailed to run func: genEIAImgs_CUDA" << std::endl;
		return false;
	}	
	return true;
}


/*�ڴ�����*/
bool RGBD2EIA::deleteData(void) {
	delete[] cu_outputParaImg;
	delete[] outputParaImg;
	return true;
}