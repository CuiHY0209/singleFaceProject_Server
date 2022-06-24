//#include <iostream>
//#include <chrono>
//#include <curl/curl.h>
//#include "publicdef.h"
//#include <tchar.h>
//#include <windows.h>
//#include "pthread.h"
//#include "http/chunk.hpp"
//#include "schedule/Schduler.h"
//#include "json/JsonDataParser.hpp"
//#include "opencv2/core/utility.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/core.hpp"
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>
//#include<opencv2/opencv.hpp>
//#include <string>
//#include <time.h>
//#include "cuda.h"
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
//#include "RGBD2EIA.h"
//#define CPPHTTPLIB_OPENSSL_SUPPORT
//
//using namespace std;
//using namespace cv;
//using json = nlohmann::json;
//
//extern long timeFlag[10];
//cv::Mat rgb;
//cv::Mat depth;
//cv::Mat depth_gray;
//RGBD2EIA rgbd2eia;
//cv::Mat* outputParaImg;
//cv::cuda::GpuMat cu_inputRGB, cu_inputDepth;
//cv::cuda::GpuMat* cu_outputParaImg;
//
//unsigned int running_threads_num = 8;
//unsigned int iterval_in_millis = 3000;
//unsigned int running_time_in_minutes = 10;
//unsigned int cli_conn_time_out_in_ms = 300000;
//std::string host = "192.168.1.65";
//std::string port = "15000";
////std::string host = "127.0.0.1";
////std::string port = "5000";
//std::string request_url = "http://" + host + ":" + port + "/recent/image";
//std::string rgb_window_name = "rgb_test";
//
//#define DLL_SELFIE_PATH "C:\\Users\\崔\\Desktop\\RunVideoAudioPlayer\\RunVideoAudioPlayer\\sdk\\bin\\VideoAudioPlayer18.dll" 
////#define DLL_SELFIE_PATH "D:\\workspace\\project\\Windows\\RunVideoAudioPlayer\\sdk\\bin\\VideoAudioPlayer.dll" 
////const char* imagePath = "D:\\RunVideoAudioPlayer\\1.jpg";
//
//typedef int(*RunPlayerFunction)(HINSTANCE hIns, HINSTANCE h, PSTR szCmd, int nShow, const char* filePath, bool* flagExternRunning);
//typedef int(*InputImageDataFunction)(unsigned char* imgData, int width, int height, int channels, long long  frameIndex, long long timeStamp);
//typedef int(*RunPlayerWithConsoleFunction)(int width, int height, int channels);
//
//
//RunPlayerWithConsoleFunction RunPlayerWithConsole = NULL;
//InputImageDataFunction InputData = NULL;
//bool flagRunning = true;
//long long frameIndex = 0;
//
//typedef struct RunParam {
//	bool* flagRunning;
//	InputImageDataFunction InputData;
//}RunParam_t;
//
////void initParameters()
////{
////
////}
//
//void doShow3dImage(const std::string &input)
//{
//	long long ts;
//	int rst = parser::parse(input, rgb, depth, ts);
//	/*cv::imshow("rgb", rgb);
//	waitKey(1);
//	cv::imshow("depth", depth);
//	waitKey(1);*/
//	/*开辟接收数据用的内存空间*/
//
//	cout << "rbg rows: " << rgb.rows << "rgb cols: " << rgb.cols << "rgb channels():" << rgb.channels() << endl;
//	cout << "depth rows: " << depth.rows << "depth cols: " << depth.cols << "depth channels():" << depth.channels() << endl;
//
//	/* 将三通道深度图转化为单通道*/
//	cv::cvtColor(depth, depth_gray, cv::ColorConversionCodes::COLOR_BGR2GRAY, 1);
//
//	////imwrite("output/rgb.jpg", rgb);
//	////imwrite("output/depth.jpg",depth);
//	//timeFlag[0] = clock();
//	///*向GPU中传入数据*/
//	///*cv::imshow("rgb", rgb);
//	//waitKey(0);
//	//cv::imshow("depth", depth);
//	//waitKey(0);*/
//	//cu_inputDepth.upload(depth_gray);
//	//cu_inputRGB.upload(rgb);
//
//	///*cv::imshow("cu_inputDepth", cu_inputDepth);
//	//waitKey(0);*/
//
//
//
//	//timeFlag[1] = clock();
//	//rgbd2eia.genEIAImgs(cu_inputRGB, cu_inputDepth, cu_outputParaImg, rgbd2eia.cu_eiaImg);
//	//timeFlag[3] = clock();
//	timeFlag[1] = clock();
//
//	/*读入背景图片*/
//	cv::Mat backgroundImg = cv::imread("res/back5.jpg", cv::IMREAD_COLOR);
//
//	/*填补嘴部*/
//	timeFlag[8] = clock();
//	/*拷贝一份深度图作为源，将填补后的信息传给inputDepth*/
//	cv::Mat depthCompensated = depth_gray;
//	rgbd2eia.prePoccess(rgb, depth_gray, depthCompensated);
//	timeFlag[10] = clock();
//
//	cv::imshow("rgb", rgb);
//	waitKey(1);
//	cv::imshow("depth", depthCompensated);
//	waitKey(1);
//	/*开辟接收数据用的内存空间*/
//	cv::Mat* outputParaImg = new cv::Mat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		outputParaImg[i] = cv::Mat::zeros(rgb.size(), CV_8UC3);
//	}
//
//	/*向GPU中传入数据*/
//	cv::cuda::GpuMat cu_rgb, cu_inputDepth, cu_backgroundImg;
//	cu_inputRGB.upload(rgb);
//	cu_inputDepth.upload(depthCompensated);
//	cu_backgroundImg.upload(backgroundImg);
//
//	/*开辟接收数据的显存*/
//	cv::cuda::GpuMat* cu_outputParaImg = new cv::cuda::GpuMat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].upload(outputParaImg[i]);
//	}
//
//	/*开辟储存EIA用的内存空间*/
//	rgbd2eia.eiaImg = cv::Mat::zeros(rgbd2eia.hardwarePara._HEIGHT, rgbd2eia.hardwarePara._WIDTH, CV_8UC3);
//	/*开辟储存EIA用的显存空间*/
//	rgbd2eia.cu_eiaImg.upload(rgbd2eia.eiaImg);
//
//
//	/*生成EIA*/
//	/*
//	*	输入：RGB、D、backgrouImg（GPU）
//	*	输出：ParaImg（GPU）
//	*	输出：EIA（CPU、GPU）
//	*/
//	rgbd2eia.genEIAImgs(cu_inputRGB, cu_inputDepth, cu_backgroundImg, cu_outputParaImg, rgbd2eia.cu_eiaImg);
//
//	timeFlag[3] = clock();
//
//
//	/*从GPU中下载数据到内存*/
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].download(outputParaImg[i]);
//	}
//	/*从GPU中下载EIA到CPU以便测试*/
//	rgbd2eia.cu_eiaImg.download(rgbd2eia.eiaImg);
//	imwrite("output/outputParaImg2.jpg", outputParaImg[2]);
//	/*输出合成视差图到本地*/
//	imwrite("output/EIAtest.jpg", rgbd2eia.eiaImg);
//	/*	cv::namedWindow("DisplayImageGPU", CV_WINDOW_NORMAL);
//	cv::setWindowProperty("DisplayImageGPU", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);*/
//	/*	cv::imshow("DisplayImageGPU", rgbd2eia.eiaImg);
//	cv::waitKey(1);*/
//	/*	cv::imshow("DisplayImageGPU0", outputParaImg[0]);
//	cv::waitKey(0);
//	cv::imshow("DisplayImageGPU1", outputParaImg[1]);
//	cv::waitKey(0);
//	cv::imshow("DisplayImageGPU2", outputParaImg[2]);
//	cv::waitKey(0);
//	cv::imshow("DisplayImageGPU3", outputParaImg[3]);
//	cv::waitKey(0);
//	cv::imshow("DisplayImageGPU4", outputParaImg[4]);
//	cv::waitKey(0);
//	*/
//
//	//	imwrite(save_path, rgbd2eia.eiaImg);
//	//cv::waitKey(5000);
//	std::cout << "图片读取时间：\t" << timeFlag[1] - timeFlag[0] << "ms\t" << std::endl
//		<< "单张视差图合成时间：\t" << timeFlag[5] - timeFlag[4] << "ms\t" << std::endl
//		<< "单张视差图映射到EIA时间：\t" << timeFlag[7] - timeFlag[6] << "ms\t" << std::endl
//		<< "视差图+EIA合成时间：\t" << timeFlag[3] - timeFlag[2] << "ms\t" << std::endl
//		<< "空洞填补时间：\t" << timeFlag[6] - timeFlag[5] << "ms\t" << std::endl;
//
//
//
//
//	// Mat processtImage = PROCESSS FUNCTION( rgb ); 
//	if (0 == rst) {
//		if (!rgbd2eia.eiaImg.empty() && InputData) {
//			Sleep(100);
//			int rst = InputData(rgbd2eia.eiaImg.data, rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels(), frameIndex, ts);
//			if (0 <= rst)
//				frameIndex++;
//			printf("Frame %d ------------------------ rst: %d\n", frameIndex, rst);
//		}
//		else {
//			printf("img is empty or input data function is not ready, InputData: %x\n", InputData);
//		}
//	}
//
//}
//// request the remote image by url
//std::string doRequestImage(const std::string &s)
//{
//	try
//	{
//		auto begin = std::chrono::system_clock::now();
//		auto tt = std::chrono::system_clock::to_time_t(begin);
//		CURL *curl;
//		CURLcode res;
//		std::string readBuffer;
//		curl = curl_easy_init();
//		if (curl)
//		{
//			curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
//			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//			curl_easy_setopt(curl, CURLOPT_NOPROXY, host.c_str());
//			curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
//			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//			res = curl_easy_perform(curl);
//			curl_easy_cleanup(curl);
//
//			auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin).count();
//			// print the result
//			std::cout << "timstamp:" << std::ctime(&tt) << '\n'
//				<< "cost:" << cost << "ms\n"
//				<< "res:" << res << "\n"
//				<< "response size:" << readBuffer.size() << '\n'
//				<< "request:" << s << '\n';
//			return readBuffer;
//		}
//		else
//		{
//			return "";
//		}
//	}
//	catch (const std::exception &e)
//	{
//		std::cerr << "Request failed, error: " << e.what() << '\n';
//		return "error response";
//	}
//}
//
//void doJobExecution(const std::string &msg)
//{
//	//step1. request the image from remote
//	//std::cout << "<<<<<<<<<<<<<<<start request<<<<<<<<<<<<<<<\n";
//	std::string response = doRequestImage(msg);
//	//std::cout << "<<<<<<<<<<<<<<<end request<<<<<<<<<<<<<<<\n";
//	//step2. show the image in 3d mode
//	if (!response.empty())
//	{
//		//std::cout << "<<<<<<<<<<<<<<<start show<<<<<<<<<<<<<<<\n";
//		doShow3dImage(response);
//		//std::cout << "<<<<<<<<<<<<<<<end show<<<<<<<<<<<<<<<\n";
//	}
//}
//void *scheduleWithMainThread(void* argv) //int sleep
//{
//	//cv::namedWindow(rgb_window_name, cv::WINDOW_AUTOSIZE);
//	while (flagRunning)
//	{
//		doJobExecution(request_url);
//	}
//	//cv::destroyAllWindows();
//	return NULL;
//}
//
//int main()
//{
//
//
//
//
//	//加载动态库及所有功能函数 -------------------------------------------------------------------------- 1
//	HINSTANCE hdll = LoadLibrary(_T(DLL_SELFIE_PATH));
//	if (hdll == NULL) {
//		printf("Load libray failed: %s\n", DLL_SELFIE_PATH);
//		// FreeLibrary(hdll);
//		return -1;
//	}
//	RunPlayerWithConsole = (RunPlayerWithConsoleFunction)GetProcAddress(hdll, "RunPlayerWithConsole");
//	InputData = (InputImageDataFunction)GetProcAddress(hdll, "InputImageData");
//	if (NULL == RunPlayerWithConsole || NULL == InputData) {
//		ERRP("Get function failed\n");
//		return -1;
//	}
//	outputParaImg = new cv::Mat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		//	outputParaImg[i] = cv::Mat::zeros(rgb.rows, rgb.cols, CV_8UC3);
//		outputParaImg[i] = cv::Mat::zeros(720, 1280, CV_8UC3);
//	}
//	/*向GPU中传入数据*/
//	cu_outputParaImg = new cv::cuda::GpuMat[rgbd2eia.hardwarePara._VIEWNUM];
//
//	/*开辟接收数据的显存*/
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].upload(outputParaImg[i]);
//	}
//	/*开辟储存EIA用的内存空间*/
//	rgbd2eia.eiaImg = cv::Mat::zeros(rgbd2eia.hardwarePara._HEIGHT, rgbd2eia.hardwarePara._WIDTH, CV_8UC3);
//
//	/*开辟储存EIA用的显存空间*/
//	rgbd2eia.cu_eiaImg.upload(rgbd2eia.eiaImg);
//	//启动输入图像线程  -------------------------------------------------------------------------- 2
//	pthread_t pidInput;
//	RunParam param;
//	param.flagRunning = &flagRunning;
//	param.InputData = InputData;
//	pthread_create(&pidInput, NULL, scheduleWithMainThread, &param);  //InputThread
//
//																	  //scheduleWithMainThread(10);
//
//	while (rgbd2eia.eiaImg.empty()) Sleep(100);
//
//	//printf("------------------------ rgb w: %d, h: %d, channels: %d\n", rgb.cols, rgb.rows, rgb.channels());
//	RunPlayerWithConsole(rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels());
//	//RunPlayerWithConsole(2160, 3840,3);
//	//RunPlayerWithConsole(2560, 1440, 3);
//
//	delete[] cu_outputParaImg;
//	delete[] outputParaImg;
//	return 0;
//	//while (1) Sleep(300); 
//}