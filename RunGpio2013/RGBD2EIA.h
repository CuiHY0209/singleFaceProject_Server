#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

/*所有硬件参数*/
struct HARDWAREPARA {
public:
	/*相机阵列参数*/
	/*视点数*/
	int _VIEWNUM = 5;
	/*有效视点数*/
	double _EFFICIENTVIEWNUM = 4.666666;
	/*斜率*/
	double _EFFICIENTSLOPE = -0.166666;
	/*相机之间的角间隔*/
	double _DISPARITYANGLE = 0.016666;

	/*显示系统参数*/
	/*输出分辨率*/
	int _HEIGHT = 2160;
	int _WIDTH = 3840;
	/*颜色通道数*/
	int _CHANNELNUM = 3;
	/*中心深度平面*/
	int _CENTRALPLANE = 50;
	/*深度噪声阈*/
	int _DEPTHTHRESHOLD = 6;

};

/*主程序所有函数及过程封装在此类下*/
class RGBD2EIA {
public:	
	/*硬件参数都在这里修改*/
	HARDWAREPARA hardwarePara;

	/*CPU中的变量*/
	/*输入RGB图像*/
	cv::Mat inputRGB;	
	/*输入的深度图*/
	cv::Mat inputDepth;
	/*背景图*/
	cv::Mat backgroundImg;

	/*生成的视差图*/
	cv::Mat* outputParaImg;
	/*最终合成的EIA*/
	cv::Mat eiaImg;

	/*GPU中的变量*/
	/*输入RGB图像*/
	cv::cuda::GpuMat cu_inputRGB;
	/*输入的深度图*/
	cv::cuda::GpuMat cu_inputDepth;
	/*背景图*/
	cv::cuda::GpuMat cu_backgroundImg;
	/*生成的视差图*/
	cv::cuda::GpuMat* cu_outputParaImg;
	/*最终合成的EIA*/
	cv::cuda::GpuMat cu_eiaImg;

	/*RGB转化为单通道灰度图*/
	cv::cuda::GpuMat cu_rgbGrey;
	/*底帽滤波找到的连通区域*/
	cv::Mat blackhatImg;
	cv::cuda::GpuMat cu_blackhatImg;
	/*形态学内核*/
	cv::Mat element;
	/*底帽滤波*/
	cv::Ptr<cv::cuda::Filter> blackhatFilter;
	cv::Mat blackhatImg_bin;
	/*存储单连通区域轮廓*/
	std::vector<std::vector<cv::Point>> contours;
	/*底帽滤波参数*/
	int blackhatThresh;
	int blackhatContourArea;

public:
	RGBD2EIA();
	~RGBD2EIA();
	/*读入图片*/
	bool readImg(char*, char*, char*);

	bool readMat(cv::Mat, cv::Mat, cv::Mat);
	/*初始化参数*/
	bool initParameters(void);
	/*读入图像后的预处理环节，包括嘴部深度自适应补全*/
	bool prePoccess(void);

	/*生成视差图*/
	bool genEIAImgs(void);
	/*滤波嘴部填充*/
	bool JBFFillMouth(void);
	/*嘴部简易填充*/
	bool simplifiedFillMouth(void);
	/*联合双边滤波*/
	bool JointBilateralFilter(void);
	/*删除数据*/
	bool deleteData(void);
};


/*从外部声明.cu可以调用的函数*/
/*使得CPU能够调用.cu中的程序*/
extern "C" {
	bool simplifiedFillMouth_CUDA(cv::cuda::GpuMat&, int, int);
	bool JointBilateralFilter_CUDA(cv::cuda::GpuMat&, cv::cuda::GpuMat&, cv::cuda::GpuMat&, cv::cuda::GpuMat&);
	bool genEIAImgs_CUDA(cv::cuda::GpuMat&, cv::cuda::GpuMat&, cv::cuda::GpuMat&, HARDWAREPARA&, cv::cuda::GpuMat*&, cv::cuda::GpuMat&);
	bool ParaImg2EIA_CUDA(cv::cuda::GpuMat&, int, HARDWAREPARA&, cv::cuda::GpuMat&);
}