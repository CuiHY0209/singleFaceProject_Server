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
////#include "cuda.h"
////#include "cuda_runtime.h"
////#include "device_launch_parameters.h"
//#include "RGBD2EIA.h"
//#define CPPHTTPLIB_OPENSSL_SUPPORT
//
//using namespace std;
//using namespace cv;
//using json = nlohmann::json;
//
//extern long timeFlag[10];
//
//
//unsigned int running_threads_num = 16;
//unsigned int iterval_in_millis = 2000;
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
//cv::Mat rgb = Mat();
//cv::Mat depth = Mat();
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
//void doShow3dImage(const std::string &input)
//{
//	long long ts;
//	int rst = parser::parse(input, rgb, depth, ts);
//
//
//	//cv::imshow("rgb", rgb);
//	//waitKey(1);
//	//cv::imshow("depth", depth);
//	//waitKey(1);
//
//	// Mat processtImage = PROCESSS FUNCTION( rgb ); 
//	if( 0 == rst ) {
//		if( !rgb.empty() && InputData) { 
//			//Sleep(200); 
//			int rst = InputData(rgb.data, rgb.cols, rgb.rows, rgb.channels(), frameIndex, ts);
//			if (0 <= rst)
//				frameIndex++; 
//			printf("Frame %d ------------------------ rst: %d\n", frameIndex, rst);
//		} 
//		else {
//			printf("img is empty or input data function is not ready, InputData: %x\n", InputData);
//		}
//	}
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
//void *scheduleWithMainThread( void* argv) //int sleep
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
//	//启动输入图像线程  -------------------------------------------------------------------------- 2
//	pthread_t pidInput;
//	RunParam param;
//	param.flagRunning = &flagRunning;
//	param.InputData = InputData;
//	pthread_create(&pidInput, NULL, scheduleWithMainThread , &param);  //InputThread
//
//	//scheduleWithMainThread(10);
//	
//	while( rgb.empty() ) Sleep(100);  
//
//	//printf("------------------------ rgb w: %d, h: %d, channels: %d\n", rgb.cols, rgb.rows, rgb.channels());
//	RunPlayerWithConsole(rgb.cols, rgb.rows, rgb.channels());
//
//	//while (1) Sleep(300); 
//}