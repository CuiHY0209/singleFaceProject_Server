//#include <iostream>
//#include <string>
//#include <time.h>
//#include "cuda.h"
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
//
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>
//
///*系统参数*/
//#include "RGBD2EIA.h"
//using namespace std;
//extern long timeFlag[10];
//
//int main()
//{
//	int iImg = 0;
//	std::string rgb_path = "";
//	std::string depth_path = "";
//	std::string save_path= "";
//	/*所有所需的函数和参数封装到该类下*/
//	RGBD2EIA rgbd2eia;
//	cv::Mat inputRGB = cv::imread("res/video_depth/0.jpg");
//	cv::Mat inputDepth;
//
//	/*开辟接收数据用的内存空间*/
//	cv::Mat* outputParaImg = new cv::Mat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		outputParaImg[i] = cv::Mat::zeros(inputRGB.rows, inputRGB.cols, CV_8UC3);
//	}
//
//	/*向GPU中传入数据*/
//	cv::cuda::GpuMat cu_inputRGB, cu_inputDepth;
//	cv::cuda::GpuMat* cu_outputParaImg = new cv::cuda::GpuMat[rgbd2eia.hardwarePara._VIEWNUM];
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
//    cv::namedWindow("DisplayImageGPU", CV_WINDOW_NORMAL);
//	cv::setWindowProperty("DisplayImageGPU", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
//	while (true) {
//		
//		//if (iImg > 254) iImg = 0;
//
//		//rgb_path = "//192.168.1.1/img/";
//		//depth_path = "//192.168.1.1/img/";
//
//		rgb_path = "res/video_depth/";
//		depth_path = "res/video_depth/";
//
//		
//		rgb_path += std::to_string(iImg);
//		//rgb_path += "_rgb.jpg";
//	rgb_path += ".jpg";
//		depth_path += std::to_string(iImg);
//		depth_path += "_depth.jpg";
//
//		timeFlag[0] = clock();
//		
//		/* 读取图片数据*/
//		/**
//		* 输入：RGB图，8位JPG格式*
//		* 输出：BGR三通道数据
//		*/
//		inputRGB = cv::imread(rgb_path, cv::IMREAD_COLOR);
//		if (inputRGB.data == NULL) {
//			std::cout << "读取RGB图出现错误！" << std::endl;
//			continue;
//		}
//		/**
//		*  输入：Depth图，8位JPG格式
//		*  输出：灰度图8位单通道数据
//		*/
//		inputDepth = cv::imread(depth_path, cv::IMREAD_GRAYSCALE);
//		if (inputDepth.data == NULL) {
//			std::cout << "读取Depth图出现错误！" << std::endl;
//			continue;
//		}
//
//		/*测试行列*/
//		cout << "rgb rows:" << inputRGB.rows << "rgb cols:" << inputRGB.cols << "rgb.channels():" << inputRGB.channels() << endl;
//		cout << "depth rows:" << inputDepth.rows << "depth cols:" << inputDepth.cols << "depth.channels():" << inputDepth.channels() << endl;
//
//		cv::imshow(rgb_path, inputRGB);
//		cvWaitKey(0);
//		cv::imshow(depth_path, inputDepth);
//		cv::waitKey(0);
//
//
//		/*向GPU中传入数据*/
//		cu_inputRGB.upload(inputRGB);
//		cu_inputDepth.upload(inputDepth);
//
//		timeFlag[1] = clock();
//		rgbd2eia.genEIAImgs(cu_inputRGB, cu_inputDepth, cu_outputParaImg, rgbd2eia.cu_eiaImg);
//		timeFlag[3] = clock();
//
//		/*从GPU中下载数据到内存*/
//		for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//			cu_outputParaImg[i].download(outputParaImg[i]);
//		}
//		/*从GPU中下载EIA到CPU以便测试*/
//		cv::imshow("DisplayImageGPU", outputParaImg[0]);
//		cv::waitKey(0);
//		cv::imshow("DisplayImageGPU", outputParaImg[1]);
//		cv::waitKey(0);
//		cv::imshow("DisplayImageGPU", outputParaImg[2]);
//		cv::waitKey(0);
//		cv::imshow("DisplayImageGPU", outputParaImg[3]);
//		cv::waitKey(0);
//		cv::imshow("DisplayImageGPU", outputParaImg[4]);
//		cv::waitKey(0);
//		rgbd2eia.cu_eiaImg.download(rgbd2eia.eiaImg);
//
//		/*输出合成视差图到本地*/
//		imwrite("output/EIAtest.jpg", rgbd2eia.eiaImg);
//		
//		cv::imshow("DisplayImageGPU", rgbd2eia.eiaImg);
//		cv::waitKey(1);
//		save_path = "EIA" + to_string(iImg) + ".jpg";
//
//	//	imwrite(save_path, rgbd2eia.eiaImg);
//		//cv::waitKey(5000);
//		std::cout << "图片读取时间：\t" << timeFlag[1] - timeFlag[0] << "ms\t" << std::endl
//			<< "单张视差图合成时间：\t" << timeFlag[5] - timeFlag[4] << "ms\t" << std::endl
//			<< "单张视差图映射到EIA时间：\t" << timeFlag[7] - timeFlag[6] << "ms\t" << std::endl
//			<< "视差图+EIA合成时间：\t" << timeFlag[3] - timeFlag[2] << "ms\t" << std::endl
//			<< "空洞填补时间：\t" << timeFlag[6] - timeFlag[5] << "ms\t" << std::endl;		
//		iImg++;
//	}	
//	delete[] cu_outputParaImg;
//	delete[] outputParaImg;
//	return 0;
//}
//
//
