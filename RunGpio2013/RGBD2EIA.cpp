#include "RGBD2EIA.h"
extern long timeFlag[20];

RGBD2EIA::RGBD2EIA() {
	inputRGB = cv::Mat::zeros(720, 1280, CV_8UC3);
	inputDepth = cv::Mat::zeros(720, 1280, CV_8UC1);
	backgroundImg = cv::Mat::zeros(720, 1280, CV_8UC3);
	/*底帽滤波参数*/
	blackhatThresh = 30;
	/*最小可容忍的连通区域像素数大小*/
	blackhatContourArea = 100;
}
RGBD2EIA::~RGBD2EIA() {
}


/*
* 功能：读取初始数据
* 输入（外部）：RGB、Depth、背景图路径
* 输出（外部）：对应的CV矩阵
*/
//bool RGBD2EIA::readImg(char* RGBImgPath, char* depthImgPath, char* backgroundImgPath) {
//	
//	/**
//	* 输入：RGB图，8位JPG格式
//	* 输出：BGR8位三通道数据
//	*/
//	inputRGB = cv::imread(RGBImgPath, cv::IMREAD_COLOR);
//	if (inputRGB.data == NULL) {
//		std::cout << "读取RGB图出现错误！" << std::endl;
//		return false;
//	}
//	/**
//	*  输入：Depth图，8位JPG格式
//	*  输出：灰度图8位单通道数据
//	*/
//	inputDepth = cv::imread(depthImgPath, cv::IMREAD_GRAYSCALE);
//	if (inputDepth.data == NULL) {
//		std::cout << "读取Depth图出现错误！" << std::endl;
//		return false;
//	}
//	
//	/**
//	*  输入：RGB背景图，8位JPG格式
//	*  输出：BGR图8位三通道数据
//	*/
//	backgroundImg = cv::imread(backgroundImgPath, cv::IMREAD_COLOR);
//	if (backgroundImg.data == NULL) {
//		std::cout << "读取背景图出现错误！" << std::endl;
//		return false;
//	}
//	return true;
//}


bool RGBD2EIA::readMat(cv::Mat rgb , cv::Mat depth , cv::Mat background) {

	/**
	* 输入：RGB图，8位JPG格式
	* 输出：BGR8位三通道数据
	*/
	inputRGB= rgb.clone();
	/*cv::imshow("inputRGB", inputRGB);
	cv::waitKey(0);*/

	if (inputRGB.data == NULL) {
		std::cout << "读取RGB图出现错误！" << std::endl;
		return false;
	}
	/**
	*  输入：Depth图，8位JPG格式
	*  输出：灰度图8位单通道数据
	*/
	cv::cvtColor(depth, inputDepth, cv::ColorConversionCodes::COLOR_BGR2GRAY, 1);
	if (inputDepth.data == NULL) {
		std::cout << "读取Depth图出现错误！" << std::endl;
		return false;
	}
	
	//cv::imshow("inputDepth", inputDepth);
	//cv::waitKey(0);

	/**
	*  输入：RGB背景图，8位JPG格式
	*  输出：BGR图8位三通道数据
	*/
	backgroundImg = background.clone();
	if (backgroundImg.data == NULL) {
		std::cout << "读取背景图出现错误！" << std::endl;
		return false;
	}
	cu_inputRGB.upload(inputRGB);
	cu_inputDepth.upload(inputDepth);
	cu_backgroundImg.upload(backgroundImg);
	return true;
}

/*
* 功能：初始化参数
* 输入：无
* 输出（类内部）：CPU中BGR矩阵、Depth矩阵、视差图矩阵、EIA矩阵
* 输出（类内部）：GPU中BGR矩阵、Depth矩阵、视差图矩阵、EIA矩阵
* 备注：有new指针定义！切记用delete[]清除内存！
*/
bool RGBD2EIA::initParameters(void) {	
	/*开辟接收数据用的内存空间*/
	outputParaImg = new cv::Mat[hardwarePara._VIEWNUM];
	for (int i = 0; i < hardwarePara._VIEWNUM; i++) {
		outputParaImg[i] = cv::Mat::zeros(inputRGB.size(), CV_8UC3);
	}

	/*首次向GPU中传入数据*/
	//cu_inputRGB.upload(inputRGB);
	//cu_inputDepth.upload(inputDepth);
	//cu_backgroundImg.upload(backgroundImg);

	/*开辟接收数据的显存*/
	cu_outputParaImg = new cv::cuda::GpuMat[hardwarePara._VIEWNUM];
	for (int i = 0; i < hardwarePara._VIEWNUM; i++) {
		cu_outputParaImg[i].upload(outputParaImg[i]);
	}
	
	/*开辟储存EIA用的内存空间*/
	eiaImg = cv::Mat::zeros(hardwarePara._HEIGHT, hardwarePara._WIDTH, CV_8UC3);
	/*开辟储存EIA用的显存空间*/
	cu_eiaImg.upload(eiaImg);
	
	return true;
}


/*
* 功能：源图像预处理，包括填补嘴部
* 输入（类内部）：RGB和Depth图
* 输出（类内部）：经过嘴部填补后的Depth图
*/
bool RGBD2EIA::prePoccess(void) {
	/*JBF嘴部填充*/
	/*if (!JBFFillMouth()){ 
		std::cout << "JBF嘴部填充出现错误！" << std::endl;
		return false;
	}*/
	/*简易嘴部填充*/
	if (!simplifiedFillMouth()) {
		std::cout << "简易嘴部填充出现错误！" << std::endl;
		return false;
	}	

	cu_inputDepth.download(inputDepth);
	/*cv::imshow("depth", inputDepth);
	cv::waitKey(1);*/
	return true;
}


/*
* 功能：形态学图像滤波进行嘴部填充
* 输入（类内部）：RGB图、Depth图
* 输出（类内部）：经过嘴部填补的Depth图
*/
bool RGBD2EIA::JBFFillMouth(void) {
	//cv::Mat test;
	/*定义一个全零矩阵用于初始化*/
	cv::Mat zeros = cv::Mat::zeros(inputDepth.rows, inputDepth.cols, CV_8UC1);
	/*利用底帽滤波寻找嘴部轮廓*/	
	/*将BGR转化为8位灰度图*/
	cv::cuda::cvtColor(cu_inputRGB, cu_rgbGrey, cv::ColorConversionCodes::COLOR_BGR2GRAY, 1);
	
	/*底帽滤波(GPU)*/	
	cu_blackhatImg.upload(zeros);
	/*形态学内核*/
	element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(40, 40));
	/*应用滤波器*/
	blackhatFilter = cv::cuda::createMorphologyFilter(cv::MORPH_BLACKHAT, CV_8UC1, element);
	blackhatFilter->apply(cu_inputDepth, cu_blackhatImg);
	
	/*二值化(GPU)*/
	cv::cuda::threshold(cu_blackhatImg, blackhatImg_bin, blackhatThresh, 255, CV_THRESH_BINARY);
	//cv::imwrite("output/BlackHat_before.jpg", blackhatImg_bin);
	
	/*剔除像素数小于100的连通区域（无现成GPU代码实现）*/
	cv::findContours(blackhatImg_bin, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
	contours.erase(std::remove_if(contours.begin(), contours.end(),
		[](const std::vector<cv::Point>& c) {return cv::contourArea(c) < 100; }), contours.end());
	blackhatImg_bin.setTo(0);
	cv::drawContours(blackhatImg_bin, contours, -1, cv::Scalar(255), cv::FILLED);
	/*cv::imshow("output/BlackHat_after.jpg", blackhatImg_bin);
	cv::waitKey(1);*/
	/*将二值化的滤波区域像素图传入gpu*/
	cu_blackhatImg.upload(blackhatImg_bin);

	/*进行双边滤波*/
	JointBilateralFilter();

	return true;
}


/*
* 功能：嘴部的快速简易填充
* 输入（类内部）：RGB图、Depth图
* 输出（类内部）：经过嘴部填补的Depth图
*/
bool RGBD2EIA::simplifiedFillMouth(void) {
	
	/*找最值*/
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
* 功能：局部双边滤波，实现渐变凹形嘴部填补
* 输入（类内部）：RGB图、Depth图
* 输出（类内部）：经过嘴部填补的Depth图
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
* 功能：合成视差图，进而合成EIA
* 输入（类内部）：RGB图、Depth图、背景图、硬件参数
* 输出（类内部）：EIA
* 附带输出（类内部）：视差图
*/
bool RGBD2EIA::genEIAImgs(void) {
	/*调用.cu中的生成EIA*/
	if (!genEIAImgs_CUDA(cu_inputRGB, cu_inputDepth, cu_backgroundImg, hardwarePara, cu_outputParaImg, cu_eiaImg)) {
		std::cout << "Fshissailed to run func: genEIAImgs_CUDA" << std::endl;
		return false;
	}	
	return true;
}


/*内存清理*/
bool RGBD2EIA::deleteData(void) {
	delete[] cu_outputParaImg;
	delete[] outputParaImg;
	return true;
}