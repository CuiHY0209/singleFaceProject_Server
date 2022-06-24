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
///*ϵͳ����*/
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
//	/*���ٽ��������õ��ڴ�ռ�*/
//	cv::Mat* outputParaImg = new cv::Mat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		outputParaImg[i] = cv::Mat::zeros(inputRGB.size(), CV_8UC3);
//	}
//	/*��GPU�д�������*/
//	cv::cuda::GpuMat cu_inputRGB, cu_inputDepth;
//	cu_inputRGB.upload(inputRGB);
//	cu_inputDepth.upload(inputDepth);
//	/*���ٽ������ݵ��Դ�*/
//	cv::cuda::GpuMat* cu_outputParaImg = new cv::cuda::GpuMat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].upload(outputParaImg[i]);
//	}
//	/*���ٴ���EIA�õ��ڴ�ռ�*/
//	rgbd2eia.eiaImg = cv::Mat::zeros(rgbd2eia.hardwarePara._HEIGHT, rgbd2eia.hardwarePara._WIDTH, CV_8UC3);
//	/*���ٴ���EIA�õ��Դ�ռ�*/
//	rgbd2eia.cu_eiaImg.upload(rgbd2eia.eiaImg);
//
//	timeFlag[1] = clock();
//
//
//	rgbd2eia.genEIAImgs(cu_inputRGB, cu_inputDepth, cu_outputParaImg, rgbd2eia.cu_eiaImg);
//
//	timeFlag[3] = clock();
//
//	/*��GPU���������ݵ��ڴ�*/
//	/*for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].download(outputParaImg[i]);
//	}*/
//	/*��GPU������EIA��CPU�Ա����*/
//	rgbd2eia.cu_eiaImg.download(rgbd2eia.eiaImg);
//
//
//
//	/*cv::imshow("test", rgbd2eia.eiaImg);
//	cv::waitKey(0);*/
//
//	/*����ϳ��Ӳ�ͼ������*/
//	imwrite("output/EIA.jpg", rgbd2eia.eiaImg);
//
//	std::cout << "ͼƬ��ȡʱ�䣺\t" << timeFlag[1] - timeFlag[0] << "ms\t" << std::endl
//		<< "�����Ӳ�ͼ�ϳ�ʱ�䣺\t" << timeFlag[5] - timeFlag[4] << "ms\t" << std::endl
//		<< "�����Ӳ�ͼӳ�䵽EIAʱ�䣺\t" << timeFlag[7] - timeFlag[6] << "ms\t" << std::endl
//		<< "�Ӳ�ͼ+EIA�ϳ�ʱ�䣺\t" << timeFlag[3] - timeFlag[2] << "ms\t" << std::endl
//		<< "�ն��ʱ�䣺\t" << timeFlag[6] - timeFlag[5] << "ms\t" << std::endl
//		<< "��ʱ�䣺\t" << timeFlag[3] << "ms\t" << std::endl;
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
//	//std::cout << "rgb->" << rgb << '\n'; //��ӡrgb
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
//	//���ض�̬�⼰���й��ܺ��� -------------------------------------------------------------------------- 1
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
//	//��������ͼ���߳�  -------------------------------------------------------------------------- 2
//	pthread_t pidInput;
//	RunParam param;
//	param.flagRunning = &flagRunning;
//	param.InputData = InputData;
//	pthread_create(&pidInput, NULL, InputThread, &param);
//
//	//������Ƶ����Ƶ�̣߳��Զ��ɼ��Ͳ��ţ�  -------------------------------------------------------------------------- 3
//	RunPlayer(hIns, h, szCmd, nShow, imagePath, &flagRunning);
//
//	return 0;
//}
//#endif
//
//int LoadPlayLibrary() {
//	//���ض�̬�⼰���й��ܺ��� -------------------------------------------------------------------------- 1
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
//	//��������ͼ���߳�  -------------------------------------------------------------------------- 2
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
//	//������Ƶ����Ƶ�̣߳��Զ��ɼ��Ͳ��ţ�  -------------------------------------------------------------------------- 3
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
//	//���ض�̬�⼰���й��ܺ��� -------------------------------------------------------------------------- 1
//	int rst = LoadPlayLibrary();
//	if (0 > rst) {
//		printf("laodPlay libray failed\n");
//		return 0; //����ʧ��ʱ�� �˴����Լ��޸����������߳���Դ
//	}
//
//	//���Ա��أ�ͼƬ��ʾ��������������-------
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
