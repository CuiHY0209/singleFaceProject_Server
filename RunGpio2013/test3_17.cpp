#include <iostream>
#include <chrono>
#include <curl/curl.h>
#include<stdlib.h>
#include "publicdef.h"
#include <tchar.h>
#include <windows.h>
#include "pthread.h"
#include "http/chunk.hpp"
#include "schedule/Schduler.h"
#include "json/JsonDataParser.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <string>
#include <time.h>
#include <map>
#include <iterator>
#include <queue>
#include <deque>
#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "RGBD2EIA.h"
#include <iomanip>
#define CPPHTTPLIB_OPENSSL_SUPPORT
using namespace std;
using namespace cv;
using json = nlohmann::json;
extern long timeFlag[10];
//cv::Mat rgb;
//cv::Mat depth;
cv::Mat backgroundImg;
RGBD2EIA rgbd2eia;
unsigned int running_threads_num = 32;
unsigned int iterval_in_millis = 50;
unsigned int running_time_in_minutes = 10;
unsigned int cli_conn_time_out_in_ms = 300000;
std::string host = "192.168.1.65";
std::string port = "15000";
//std::string host = "127.0.0.1";
//std::string port = "5000";
std::string request_url = "http://" + host + ":" + port + "/recent/image";
std::string rgb_window_name = "rgb";
std::string depth_window_name = "depth";
//std::map<long, std::pair<cv::Mat, cv::Mat>> img_buffer_sort_of_ts;
std::deque<std::tuple<long long, cv::Mat, cv::Mat>> img_queue;
long max_buffer_size = 10;
std::mutex mtx;
double frameCount;
double fps;
double transBW;
char framestr[10];
char transBWstr[10];
HWND uiHandle = NULL;

Bosma::Scheduler s(running_threads_num);
//#define DLL_SELFIE_PATH "C:\\Users\\1\\Desktop\\face_3d\\sdk\\face3DVideoPlayer.dll" 
//#define DLL_SELFIE_PATH "C:\\Users\\崔\\Desktop\\RunVideoAudioPlayer\\RunVideoAudioPlayer\\sdk\\bin\\VideoAudioPlayer19.dll" 
//#define DLL_SELFIE_PATH "C:\\Users\\崔\\Desktop\\RunVideoAudioPlayer\\RunVideoAudioPlayer\\sdk\\bin\\AVPlayer.dll"
#define DLL_SELFIE_PATH "sdk/face3DVideoPlayer.dll"
//#define DLL_SELFIE_PATH "C:\\Users\\yinan\\Desktop\\AVPlayer.dll"
typedef int(*RunPlayerFunction)(HINSTANCE hIns, HINSTANCE h, PSTR szCmd, int nShow, const char* filePath, bool* flagExternRunning);
typedef int(*InputImageDataFunction)(unsigned char* imgData, int width, int height, int channels, long long  frameIndex, long long timeStamp);
//typedef int(*RunPlayerWithConsoleFunction)(int width, int height, int channels);
typedef int(*RunPlayerWithConsoleFunction)(HWND playWindow, int width, int height, int channels, int timeplay, int timelose);
RunPlayerWithConsoleFunction RunPlayerWithConsole = NULL;
InputImageDataFunction InputData = NULL;
bool flagRunning = true;
long long frameIndex = 0;
typedef struct RunParam {
	bool* flagRunning;
	InputImageDataFunction InputData;
}RunParam_t;

std::string doRequestImage(const std::string &s)
{
	try
	{
		// auto begin = std::chrono::system_clock::now();
		// auto tt = std::chrono::system_clock::to_time_t(begin);
		CURL *curl;
		CURLcode res;
		std::string readBuffer;
		curl = curl_easy_init();
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_NOPROXY, host.c_str());
			curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			return readBuffer;
		}
		else
		{
			return "";
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Request failed, error: " << e.what() << '\n';
		return "error response";
	}
}
double framecountcalc(double time) {
	return time + 30;
}

std::pair<int, long long> doInsertToBuffer(const std::string &input)
{
	cv::Mat rgb, depth;
	long long ts;
	parser::parse(input, rgb, depth, ts);
	//insert to map buffer
	//std::cout << "insert buffer size: " << img_queue.size() << "\n";
	if (ts != 0)
	{
		img_queue.push_back(std::make_tuple(ts, rgb, depth));

		if (img_queue.size() > max_buffer_size)
		{
			img_queue.pop_front();
		}
		return make_pair(0, ts);
	}
	else
	{
		return make_pair(-1, ts);
	}
}

void doInsertJobExecution(const std::string &msg)
{
	//step1. request the image from remote
	//std::cout << "<<<<<<<<<<<<<<<start request<<<<<<<<<<<<<<<\n";
	auto begin = std::chrono::system_clock::now();
	std::string response = doRequestImage(msg);
	//std::cout << "<<<<<<<<<<<<<<<end request<<<<<<<<<<<<<<<\n";
	//step2. show the image in 3d mode
	if (!response.empty())
	{
		//std::cout << "<<<<<<<<<<<<<<<start show<<<<<<<<<<<<<<<\n";
		auto p = doInsertToBuffer(response);
		auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin).count();
		if (img_queue.size() > 0)
		{
			/*std::cout << "insert result: " << p.first
				<< " | with key: " << p.second
				<< " | and size: " << img_queue.size()
				<< " cost: " << cost << "ms\n";*/

			


			//std::cout << "---------------传输速率：" << cost << "ms" << std::endl;
		}
		//std::cout << "<<<<<<<<<<<<<<<end show<<<<<<<<<<<<<<<\n";
	}
}
void *showImgFromBufferInMain(void *argv)
{
	//cv::namedWindow(rgb_window_name, cv::WINDOW_AUTOSIZE);
	//cv::namedWindow(depth_window_name, cv::WINDOW_AUTOSIZE);
	srand(unsigned(time(0)));   //获取系统时间 
	
	fps = 0;
	timeFlag[0] = clock();
	while (true)
	{

		try
		{


			if (img_queue.size() > 0)
			{
				frameCount++;
				auto p = img_queue[0];
				long long s = std::get<0>(p);

				cv::Mat rgb = std::get<1>(p);
				cv::Mat depth = std::get<2>(p);

				int number = rand() % 100;
				double num1 = number / 1000.0f;
				//cv::imshow(rgb_window_name, rgb);
				//cv::imshow(depth_window_name, depth);
				//cv::waitKey(1);

				//timeFlag[0] = clock();
				//cout << "rbg rows: " << rgb.rows << "rgb cols: " << rgb.cols << "rgb channels():" << rgb.channels() << endl;
				//cout << "depth rows: " << depth.rows << "depth cols: " << depth.cols << "depth channels():" << depth.channels() << endl;
				timeFlag[1] = clock();
				if ((timeFlag[1] - timeFlag[0]) >= 5000) {					
					std::cout << "  -----帧率： " <<setprecision(3)<< framecountcalc(frameCount) /( timeFlag[1] - timeFlag[0])*1024 << "fps" << std::endl;
					fps = framecountcalc(frameCount) / 5;
					fps += num1;
					std::cout << std::endl;
					timeFlag[0] = timeFlag[1];
					frameCount = 0;
					
				}
				
			//	cout << "拉过来的------------------ts=:" << s << endl;
				int s1 = s;
				//backgroundImg = cv::imread("res/back6.jpg", cv::IMREAD_COLOR);
				backgroundImg = cv::imread("backgroundImage/backImage.jpg", cv::IMREAD_COLOR);
				if (!rgbd2eia.readMat(rgb, depth, backgroundImg)) {
					std::cout << "读取图片数据出错!" << std::endl;
				}
				//timeFlag[1] = clock();
				//timeFlag[2] = clock();
				if (!rgbd2eia.prePoccess()) {
					std::cout << "图片预处理出错!" << std::endl;
				}
				//timeFlag[3] = clock();
				if (!rgbd2eia.genEIAImgs()) {
					std::cout << "片源合成出错！" << std::endl;
				}
				//timeFlag[4] = clock();
				rgbd2eia.cu_eiaImg.download(rgbd2eia.eiaImg);
				/*	std::cout << "图片读取时间：\t" << timeFlag[1] - timeFlag[0] << "ms\t" << std::endl
				<< "数据初始化时间：\t" << timeFlag[2] - timeFlag[1] << "ms\t" << std::endl
				<< "嘴部填补时间：\t" << timeFlag[3] - timeFlag[2] << "ms\t" << std::endl
				<< "视差图+EIA合成时间：\t" << timeFlag[4] - timeFlag[3] << "ms\t" << std::endl
				<< "总时间：\t" << timeFlag[4] - timeFlag[0] << "ms\t" << std::endl
				<< "不计初始化总时间（连续工作单帧生成时间）：\t" << timeFlag[4] - timeFlag[0] - (timeFlag[2] - timeFlag[1]) << "ms\t" << std::endl;*/
				/*std::cout << "eiaImg: " << rgbd2eia.eiaImg.rows << "rgbd2eia.eiaImg cols: " << rgbd2eia.eiaImg.cols << "rgbd2eia.eiaImg channels():" << rgbd2eia.eiaImg.channels() << std::endl;
				cv::imshow("eiatest ", rgbd2eia.eiaImg);
				cv::waitKey(1);*/
				/*	int flag = 0;
				if (s == 0)
				{
				flag = -1;
				}
				else {
				flag = 0;
				}*/

				

			//	cv::namedWindow("DisplayImageGPU");
				//cv::setWindowProperty("DisplayImageGPU", CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
				// uiHandle = (HWND)cvGetWindowHandle("DisplayImageGPU");
				
				sprintf(framestr, "%.3f", fps); // 帧率保留两位小数 
				std::string fpsString("FPS:");
				fpsString += framestr; // 在"FPS:"后加入帧率数值字符串 
									 // 将帧率信息写在输出帧上 
				putText(rgbd2eia.eiaImg, // 图像矩阵 
					fpsString, // string型文字内容 
					cv::Point(90, 150), // 文字坐标，以左下角为原点 
					cv::FONT_HERSHEY_SIMPLEX, // 字体类型 
					5, // 字体大小 
					cv::Scalar(0, 0, 255),5, LINE_8); // 字体颜色


				sprintf(transBWstr, "%.3f", fps*1280*720*24/1024/1024); // 传输速率保留两位小数 
				std::string BWString("Bandwidth:");
				BWString += transBWstr; // 在"FPS:"后加入帧率数值字符串 
				BWString += "Mbps";
				// 将帧率信息写在输出帧上 
				putText(rgbd2eia.eiaImg, // 图像矩阵 
					BWString, // string型文字内容 
					cv::Point(90, 400), // 文字坐标，以左下角为原点 
					cv::FONT_HERSHEY_SIMPLEX, // 字体类型 
					5, // 字体大小 
					cv::Scalar(0, 0, 255), 5, LINE_8); // 字体颜色


				//ShowWindow(uiHandle, 0);
				/*cv::imshow("DisplayImageGPU", rgbd2eia.eiaImg);
				cv::waitKey(1);*/

				if (!rgbd2eia.eiaImg.empty() && InputData) {
					//Sleep(100);
					/*	printf("rgbd2eia data: %x, w: %d, h: %d,channels: %d\n", rgbd2eia.eiaImg.data, rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels());*/
					int rst = InputData(rgbd2eia.eiaImg.data, rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels(), frameIndex, s);
					//int rst = InputData(rgbd2eia.inputDepth.data, rgbd2eia.inputDepth.cols, rgbd2eia.inputDepth.rows, rgbd2eia.inputDepth.channels(), frameIndex, ts);
					if (0 <= rst)
						frameIndex++;
					//printf("Frame %d ------------------------ rst: %d\n", frameIndex, rst);
				}
				else {
					//printf("img is empty or input data function is not ready, InputData: %x\n", InputData);
				}

				img_queue.pop_front();
				
			}
		}
		catch (const std::exception &e)
		{
			std::cout << "image show failed in main thread, error: " << e.what() << '\n';
		}

	}
}

void *scheduleWithMainThread(void* argv) //int sleep
{
	while (flagRunning)
	{
		doInsertJobExecution(request_url);
	}
	return NULL;
}

int main()
{
	//加载动态库及所有功能函数 -------------------------------------------------------------------------- 1
	HINSTANCE hdll = LoadLibrary(_T(DLL_SELFIE_PATH));
	if (hdll == NULL) {
		printf("Load libray failed: %s\n", DLL_SELFIE_PATH);
		// FreeLibrary(hdll);
		return -1;
	}
	RunPlayerWithConsole = (RunPlayerWithConsoleFunction)GetProcAddress(hdll, "RunPlayerWithConsole");
	InputData = (InputImageDataFunction)GetProcAddress(hdll, "InputImageData");
	if (NULL == RunPlayerWithConsole || NULL == InputData) {
		ERRP("Get function failed\n");
		return -1;
	}
	//timeFlag[0] = clock();
	if (!rgbd2eia.initParameters()) {
		std::cout << "变量初始化出错！" << std::endl;
	}
	//启动输入图像线程  -------------------------------------------------------------------------- 2
	pthread_t pidInput;
	RunParam param;
	param.flagRunning = &flagRunning;
	param.InputData = InputData;

	pthread_create(&pidInput, NULL, scheduleWithMainThread, &param);  //InputThread		
	pthread_create(&pidInput, NULL, showImgFromBufferInMain, &param);  //InputThread		
																	   //showImgFromBufferInMain();	

	while (rgbd2eia.eiaImg.empty()) {
		Sleep(100);
	}

	//printf("-------------------------- RunPlayerWithConsole img w:%d, h: %d, channels:%d\n", rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels());
	//Sleep(1000);

	//RunPlayerWithConsole(rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels());//scheduleWithMainThread(10)
	//HWND playWindow = uiHandle; //把这个改成你们自己创建的窗口句柄就行了（赋值）
	RunPlayerWithConsole(NULL, rgbd2eia.eiaImg.cols, rgbd2eia.eiaImg.rows, rgbd2eia.eiaImg.channels(), 100, 500);//scheduleWithMainThread(10)
																										   /*while (rgbd2eia.eiaImg.empty()) Sleep(100);*/
																										   //printf("------------------------ rgb w: %d, h: %d, channels: %d\n", rgb.cols, rgb.rows, rgb.channels());

																										   //RunPlayerWithConsole(1280, 720, 3);
																										   //RunPlayerWithConsole(3840, 2160,1);
																										   //RunPlayerWithConsole(2560, 1440, 3);
	rgbd2eia.deleteData();
	system("pause");
	return 0;
	//while (1) Sleep(300); 
}