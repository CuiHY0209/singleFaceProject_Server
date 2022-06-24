//#include "publicdef.h"
//#include <iostream>
//#include <tchar.h>
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/core.hpp"
//#include<opencv2/opencv.hpp>
//#include <windows.h>
//#include "pthread.h"
//#include <chrono>
//#include <curl/curl.h>
//#include "http/chunk.hpp"
//#include "schedule/Schduler.h"
//#include "json/JsonDataParser.hpp"
//#include "opencv2/core/utility.hpp"
//#include <time.h>
//#include "cuda.h"
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
//#include "opencv2/highgui.hpp"
//
///*系统参数*/
//#include "RGBD2EIA.h"
//#define CPPHTTPLIB_OPENSSL_SUPPORT
//
//using namespace cv;
//using json = nlohmann::json;
//
//typedef int(*RunPlayerFunction)(HINSTANCE hIns, HINSTANCE h, PSTR szCmd, int nShow, const char* filePath, bool* flagExternRunning);
//typedef int(*InputImageDataFunction)(unsigned char* imgData, int width, int height, int channels, long long  frameIndex, long long timeStamp);
//typedef int(*RunPlayerWithConsoleFunction)(int width, int height, int channels);
//
//const char* imageFilePath = "D:\\http_server\\apple.jpg";
//
//unsigned int running_threads_num = 16;
//unsigned int iterval_in_millis = 20000;
//unsigned int running_time_in_minutes = 10;
//unsigned int cli_conn_time_out_in_ms = 300000;
//std::string request_url = "http://127.0.0.1:5000/recent/image";
//
////Mat pic = imread("0.jpg");
//extern long timeFlag[10];
//
//RunPlayerFunction RunPlayer = NULL;
//InputImageDataFunction InputData = NULL;
//RunPlayerWithConsoleFunction RunPlayerWithConsole = NULL;
//long long  frameIndex = 0;
//bool flagRunningLocal = true;
//#define input_resolution_cols 960
//#define input_resolution_rows 540
//#define output_resolution_cols 3840
//#define output_resolution_rows 2160
//
//using namespace std;
//using namespace cv;
//
//#define DLL_SELFIE_PATH "D:\\RunVideoAudioPlayer\\sdk\\bin\\VideoAudioPlayer.dll" 
////#define DLL_SELFIE_PATH "D:\\workspace\\project\\Windows\\RunVideoAudioPlayer\\sdk\\bin\\VideoAudioPlayer.dll" 
//const char* imagePath = "D:\\RunVideoAudioPlayer\\1.jpg";
//
//
//typedef struct RunParam {
//	bool* flagRunning;
//	InputImageDataFunction InputData;
//}RunParam_t;
//
//void* InputThread(void* argv) {
//	RunParam_t* param = (RunParam_t*)argv;
//
//	Mat imgSource = imread(imageFilePath, IMREAD_ANYCOLOR);
//	if (imgSource.empty()) {
//		printf("read img failed: %s\n", imageFilePath);
//		return NULL;
//	}
//
//	long long frameIndex2 = 0;
//
//	while (*param->flagRunning) {
//		int rst = param->InputData(imgSource.data, imgSource.cols, imgSource.rows, imgSource.channels(), frameIndex2, GetTickCount64());
//		if (0 <= rst) frameIndex2++;
//		Sleep(10);
//	}
//	return NULL;
//}
//
//int doShow3dImage(const std::string &input)
//{
//	long ts;
//	int i = 0;
//	string rgb_path = "";
//	string depth_path = "";
//	rgb_path += to_string(i);
//	rgb_path += ".jpg";
//	depth_path += to_string(i);
//	depth_path += "_depth.jpg";
//	i++;
//	if (i > 2) i = 0;
//	cv::Mat inputRGB, inputDepth;
//	timeFlag[0] = clock();
//	RGBD2EIA rgbd2eia;
//	 inputRGB = cv::imread(rgb_path, cv::IMREAD_COLOR);
//	 inputDepth = cv::imread(depth_path, cv::IMREAD_GRAYSCALE);
//	 cout << "path is :"<<rgb_path << endl;
//	 cout << "path is :" << depth_path << endl;
//	/* cv::imshow("test", inputRGB);
//	 waitKey(0);*/
//
//	/*开辟接收数据用的内存空间*/
//	cv::Mat* outputParaImg = new cv::Mat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		outputParaImg[i] = cv::Mat::zeros(inputRGB.size(), CV_8UC3);
//	}
//	/*向GPU中传入数据*/
//	cv::cuda::GpuMat cu_inputRGB, cu_inputDepth;
//	cu_inputRGB.upload(inputRGB);
//	cu_inputDepth.upload(inputDepth);
//	/*开辟接收数据的显存*/
//	cv::cuda::GpuMat* cu_outputParaImg = new cv::cuda::GpuMat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].upload(outputParaImg[i]);
//	}
//	/*开辟储存EIA用的内存空间*/
//	rgbd2eia.eiaImg = cv::Mat::zeros(rgbd2eia.hardwarePara._HEIGHT, rgbd2eia.hardwarePara._WIDTH, CV_8UC3);
//	/*开辟储存EIA用的显存空间*/
//	rgbd2eia.cu_eiaImg.upload(rgbd2eia.eiaImg);
//
//	timeFlag[1] = clock();
//
//
//	rgbd2eia.genEIAImgs(cu_inputRGB, cu_inputDepth, cu_outputParaImg, rgbd2eia.cu_eiaImg);
//
//	timeFlag[3] = clock();
//
//	/*从GPU中下载数据到内存*/
//	/*for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].download(outputParaImg[i]);
//	}*/
//	/*从GPU中下载EIA到CPU以便测试*/
//	rgbd2eia.cu_eiaImg.download(rgbd2eia.eiaImg);
//
//
//
//	/*cv::imshow("test", rgbd2eia.eiaImg);
//	cv::waitKey(0);*/
//
//	/*输出合成视差图到本地*/
//	imwrite("output/EIA.jpg", rgbd2eia.eiaImg);
//
//	std::cout << "图片读取时间：\t" << timeFlag[1] - timeFlag[0] << "ms\t" << std::endl
//		<< "单张视差图合成时间：\t" << timeFlag[5] - timeFlag[4] << "ms\t" << std::endl
//		<< "单张视差图映射到EIA时间：\t" << timeFlag[7] - timeFlag[6] << "ms\t" << std::endl
//		<< "视差图+EIA合成时间：\t" << timeFlag[3] - timeFlag[2] << "ms\t" << std::endl
//		<< "空洞填补时间：\t" << timeFlag[6] - timeFlag[5] << "ms\t" << std::endl
//		<< "总时间：\t" << timeFlag[3] << "ms\t" << std::endl;
//	//(unsigned char* imgData, int width, int height, int channels, long long  frameIndex, long long timeStamp);
//
//	int rst = -1;
//
//	/*if (InputData) {
//		 rst = InputData(rgb.data, rgb.cols, rgb.rows, rgb.channels(), frameIndex, ts);
//		if (rst <= 0) frameIndex++;
//	}*/
//	if (InputData) {
//		rst = InputData(rgbd2eia.eiaImg.data, rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels(), frameIndex, ts);
//		if (rst <= 0) frameIndex++;
//	}
//	delete[] cu_outputParaImg;
//	delete[] outputParaImg;
//	return rst;
//#if 0
//	RunPlayerFunction RunPlayer = (RunPlayerFunction)GetProcAddress(hdll, "RunPlayer");
//	if (NULL == RunPlayer) {
//		ERRP("Get function failed\n");
//
//	}
//#endif
//
//	//std::cout << "rgb->" << rgb << '\n'; //打印rgb
//	//std::cout << rgb.size() <<"|"<< rgb.channels()<< '\n';
//	//cv::namedWindow("DisplayImageGPU", CV_WINDOW_AUTOSIZE);
//	//cv::imshow("DisplayImageGPU", rgb);
//	//waitKey(10);
//}
//
//#if 0
//bool flagRunning = true;
//int WINAPI WinMain(HINSTANCE hIns, HINSTANCE h, PSTR szCmd, int nShow) {
//	MessageBox(NULL, TEXT("WinMain start"), NULL, MB_ICONERROR);
//
//	//加载动态库及所有功能函数 -------------------------------------------------------------------------- 1
//	HINSTANCE hdll = LoadLibrary(_T(DLL_SELFIE_PATH));
//	if (hdll == NULL) {
//		printf("Load libray failed: %s\n", DLL_SELFIE_PATH);
//		MessageBox(NULL, TEXT("Load libray failed"), NULL, MB_ICONERROR);
//		// FreeLibrary(hdll);
//		return -1;
//	}
//	RunPlayerFunction RunPlayer = (RunPlayerFunction)GetProcAddress(hdll, "RunPlayer");
//	InputImageDataFunction InputData = (InputImageDataFunction)GetProcAddress(hdll, "InputImageData");
//	RunPlayerWithConsoleFunction RunPlayerWithConsole = (RunPlayerWithConsoleFunction)GetProcAddress(hdll, "RunPlayerWithConsole");
//	if (NULL == RunPlayer || NULL == InputData || NULL == RunPlayerWithConsole) {
//		ERRP("Get function failed\n");
//		return -1;
//	}
//
//	//启动输入图像线程  -------------------------------------------------------------------------- 2
//	pthread_t pidInput;
//	RunParam param;
//	param.flagRunning = &flagRunning;
//	param.InputData = InputData;
//	pthread_create(&pidInput, NULL, InputThread, &param);
//
//	//启动视频和音频线程（自动采集和播放）  -------------------------------------------------------------------------- 3
//	RunPlayer(hIns, h, szCmd, nShow, imagePath, &flagRunning);
//
//	return 0;
//}
//#endif
//
//int LoadPlayLibrary() {
//	//加载动态库及所有功能函数 -------------------------------------------------------------------------- 1
//	HINSTANCE hdll = LoadLibrary(_T(DLL_SELFIE_PATH));
//	if (hdll == NULL) {
//		printf("Load libray failed: %s\n", DLL_SELFIE_PATH);
//		//MessageBox(NULL, TEXT("Load libray failed"), NULL, MB_ICONERROR);
//		// FreeLibrary(hdll);
//		return -1;
//	}
//	RunPlayer = (RunPlayerFunction)GetProcAddress(hdll, "RunPlayer");
//	InputData = (InputImageDataFunction)GetProcAddress(hdll, "InputImageData");
//	RunPlayerWithConsole = (RunPlayerWithConsoleFunction)GetProcAddress(hdll, "RunPlayerWithConsole");
//	if (NULL == RunPlayer || NULL == InputData || NULL == RunPlayerWithConsole) {
//		ERRP("Get function failed\n");
//		return -1;
//	}
//	return 0;
//}
//
//int  RunVAPlayTest() {
//	//启动输入图像线程  -------------------------------------------------------------------------- 2
//	pthread_t pidInput;
//	RunParam param;
//	param.flagRunning = &flagRunningLocal;
//	param.InputData = InputData;
//	pthread_create(&pidInput, NULL, InputThread, &param);
//
//	Mat imgSource = imread(imageFilePath, IMREAD_ANYCOLOR);
//	if (imgSource.empty()) {
//		printf("read img failed: %s\n", imageFilePath);
//		return -1;
//	}
//
//	//启动视频和音频线程（自动采集和播放）  -------------------------------------------------------------------------- 3
//	RunPlayerWithConsole(imgSource.cols, imgSource.rows, imgSource.channels());
//
//	return 0;
//}
//
//
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
//			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//			res = curl_easy_perform(curl);
//			curl_easy_cleanup(curl);
//
//			auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin).count();
//			// print the result
//			std::cout << "timstamp:" << std::ctime(&tt) << '\n'
//				<< "cost:" << cost << "ms\n"
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
//	std::cout << "<<<<<<<<<<<<<<<start request<<<<<<<<<<<<<<<\n";
//	std::string response = doRequestImage(msg);
//	std::cout << "<<<<<<<<<<<<<<<end request<<<<<<<<<<<<<<<\n";
//	//step2. show the image
//	if (!response.empty())
//	{
//		std::cout << "<<<<<<<<<<<<<<<start show<<<<<<<<<<<<<<<\n";
//		doShow3dImage(response);
//		std::cout << "<<<<<<<<<<<<<<<end show<<<<<<<<<<<<<<<\n";
//	}
//}
//
//// the logic for schedule request image in every 1 seconds
//
//int main() {
//	//加载动态库及所有功能函数 -------------------------------------------------------------------------- 1
//	int rst = LoadPlayLibrary();
//	if (0 > rst) {
//		printf("laodPlay libray failed\n");
//		return 0; //加载失败时候， 此处请自己修改清理开启的线程资源
//	}
//
//	//测试本地：图片显示播放与声音播放-------
//#if 0
//	RunVAPlayTest();
//	return 0;
//#endif
//
//	// number of tasks that can run simultaneously
//  // Note: not the number of tasks that can be added,
//  //       but number of tasks that can be run in parallel
//	unsigned int max_n_threads = running_threads_num;
//	// Make a new scheduling object.
//	// Note: s cannot be moved or copied
//	Bosma::Scheduler s(max_n_threads);
//
//	// every second call message("every second")
//	s.every(std::chrono::milliseconds(iterval_in_millis), doJobExecution, request_url);
//
//
//	RunPlayerWithConsole(output_resolution_cols, output_resolution_rows, 3);
//	//RunVAPlay();
//
//	// destructor of Bosma::Scheduler will cancel all schedules but finish any tasks currently running
//	std::this_thread::sleep_for(std::chrono::minutes(running_time_in_minutes));
//	// std::this_thread::yield();
//
//	return 0;
//}
