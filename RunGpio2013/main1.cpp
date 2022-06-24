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
///*ϵͳ����*/
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
//	/*��������ĺ����Ͳ�����װ��������*/
//	RGBD2EIA rgbd2eia;
//	cv::Mat inputRGB = cv::imread("res/video_depth/0.jpg");
//	cv::Mat inputDepth;
//
//	/*���ٽ��������õ��ڴ�ռ�*/
//	cv::Mat* outputParaImg = new cv::Mat[rgbd2eia.hardwarePara._VIEWNUM];
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		outputParaImg[i] = cv::Mat::zeros(inputRGB.rows, inputRGB.cols, CV_8UC3);
//	}
//
//	/*��GPU�д�������*/
//	cv::cuda::GpuMat cu_inputRGB, cu_inputDepth;
//	cv::cuda::GpuMat* cu_outputParaImg = new cv::cuda::GpuMat[rgbd2eia.hardwarePara._VIEWNUM];
//
//	/*���ٽ������ݵ��Դ�*/
//	for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//		cu_outputParaImg[i].upload(outputParaImg[i]);
//	}
//	/*���ٴ���EIA�õ��ڴ�ռ�*/
//	rgbd2eia.eiaImg = cv::Mat::zeros(rgbd2eia.hardwarePara._HEIGHT, rgbd2eia.hardwarePara._WIDTH, CV_8UC3);
//
//	/*���ٴ���EIA�õ��Դ�ռ�*/
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
//		/* ��ȡͼƬ����*/
//		/**
//		* ���룺RGBͼ��8λJPG��ʽ*
//		* �����BGR��ͨ������
//		*/
//		inputRGB = cv::imread(rgb_path, cv::IMREAD_COLOR);
//		if (inputRGB.data == NULL) {
//			std::cout << "��ȡRGBͼ���ִ���" << std::endl;
//			continue;
//		}
//		/**
//		*  ���룺Depthͼ��8λJPG��ʽ
//		*  ������Ҷ�ͼ8λ��ͨ������
//		*/
//		inputDepth = cv::imread(depth_path, cv::IMREAD_GRAYSCALE);
//		if (inputDepth.data == NULL) {
//			std::cout << "��ȡDepthͼ���ִ���" << std::endl;
//			continue;
//		}
//
//		/*��������*/
//		cout << "rgb rows:" << inputRGB.rows << "rgb cols:" << inputRGB.cols << "rgb.channels():" << inputRGB.channels() << endl;
//		cout << "depth rows:" << inputDepth.rows << "depth cols:" << inputDepth.cols << "depth.channels():" << inputDepth.channels() << endl;
//
//		cv::imshow(rgb_path, inputRGB);
//		cvWaitKey(0);
//		cv::imshow(depth_path, inputDepth);
//		cv::waitKey(0);
//
//
//		/*��GPU�д�������*/
//		cu_inputRGB.upload(inputRGB);
//		cu_inputDepth.upload(inputDepth);
//
//		timeFlag[1] = clock();
//		rgbd2eia.genEIAImgs(cu_inputRGB, cu_inputDepth, cu_outputParaImg, rgbd2eia.cu_eiaImg);
//		timeFlag[3] = clock();
//
//		/*��GPU���������ݵ��ڴ�*/
//		for (int i = 0; i < rgbd2eia.hardwarePara._VIEWNUM; i++) {
//			cu_outputParaImg[i].download(outputParaImg[i]);
//		}
//		/*��GPU������EIA��CPU�Ա����*/
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
//		/*����ϳ��Ӳ�ͼ������*/
//		imwrite("output/EIAtest.jpg", rgbd2eia.eiaImg);
//		
//		cv::imshow("DisplayImageGPU", rgbd2eia.eiaImg);
//		cv::waitKey(1);
//		save_path = "EIA" + to_string(iImg) + ".jpg";
//
//	//	imwrite(save_path, rgbd2eia.eiaImg);
//		//cv::waitKey(5000);
//		std::cout << "ͼƬ��ȡʱ�䣺\t" << timeFlag[1] - timeFlag[0] << "ms\t" << std::endl
//			<< "�����Ӳ�ͼ�ϳ�ʱ�䣺\t" << timeFlag[5] - timeFlag[4] << "ms\t" << std::endl
//			<< "�����Ӳ�ͼӳ�䵽EIAʱ�䣺\t" << timeFlag[7] - timeFlag[6] << "ms\t" << std::endl
//			<< "�Ӳ�ͼ+EIA�ϳ�ʱ�䣺\t" << timeFlag[3] - timeFlag[2] << "ms\t" << std::endl
//			<< "�ն��ʱ�䣺\t" << timeFlag[6] - timeFlag[5] << "ms\t" << std::endl;		
//		iImg++;
//	}	
//	delete[] cu_outputParaImg;
//	delete[] outputParaImg;
//	return 0;
//}
//
//
