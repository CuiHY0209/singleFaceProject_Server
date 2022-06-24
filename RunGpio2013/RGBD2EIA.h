#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

/*����Ӳ������*/
struct HARDWAREPARA {
public:
	/*������в���*/
	/*�ӵ���*/
	int _VIEWNUM = 5;
	/*��Ч�ӵ���*/
	double _EFFICIENTVIEWNUM = 4.666666;
	/*б��*/
	double _EFFICIENTSLOPE = -0.166666;
	/*���֮��ĽǼ��*/
	double _DISPARITYANGLE = 0.016666;

	/*��ʾϵͳ����*/
	/*����ֱ���*/
	int _HEIGHT = 2160;
	int _WIDTH = 3840;
	/*��ɫͨ����*/
	int _CHANNELNUM = 3;
	/*�������ƽ��*/
	int _CENTRALPLANE = 50;
	/*���������*/
	int _DEPTHTHRESHOLD = 6;

};

/*���������к��������̷�װ�ڴ�����*/
class RGBD2EIA {
public:	
	/*Ӳ���������������޸�*/
	HARDWAREPARA hardwarePara;

	/*CPU�еı���*/
	/*����RGBͼ��*/
	cv::Mat inputRGB;	
	/*��������ͼ*/
	cv::Mat inputDepth;
	/*����ͼ*/
	cv::Mat backgroundImg;

	/*���ɵ��Ӳ�ͼ*/
	cv::Mat* outputParaImg;
	/*���պϳɵ�EIA*/
	cv::Mat eiaImg;

	/*GPU�еı���*/
	/*����RGBͼ��*/
	cv::cuda::GpuMat cu_inputRGB;
	/*��������ͼ*/
	cv::cuda::GpuMat cu_inputDepth;
	/*����ͼ*/
	cv::cuda::GpuMat cu_backgroundImg;
	/*���ɵ��Ӳ�ͼ*/
	cv::cuda::GpuMat* cu_outputParaImg;
	/*���պϳɵ�EIA*/
	cv::cuda::GpuMat cu_eiaImg;

	/*RGBת��Ϊ��ͨ���Ҷ�ͼ*/
	cv::cuda::GpuMat cu_rgbGrey;
	/*��ñ�˲��ҵ�����ͨ����*/
	cv::Mat blackhatImg;
	cv::cuda::GpuMat cu_blackhatImg;
	/*��̬ѧ�ں�*/
	cv::Mat element;
	/*��ñ�˲�*/
	cv::Ptr<cv::cuda::Filter> blackhatFilter;
	cv::Mat blackhatImg_bin;
	/*�洢����ͨ��������*/
	std::vector<std::vector<cv::Point>> contours;
	/*��ñ�˲�����*/
	int blackhatThresh;
	int blackhatContourArea;

public:
	RGBD2EIA();
	~RGBD2EIA();
	/*����ͼƬ*/
	bool readImg(char*, char*, char*);

	bool readMat(cv::Mat, cv::Mat, cv::Mat);
	/*��ʼ������*/
	bool initParameters(void);
	/*����ͼ����Ԥ�����ڣ������첿�������Ӧ��ȫ*/
	bool prePoccess(void);

	/*�����Ӳ�ͼ*/
	bool genEIAImgs(void);
	/*�˲��첿���*/
	bool JBFFillMouth(void);
	/*�첿�������*/
	bool simplifiedFillMouth(void);
	/*����˫���˲�*/
	bool JointBilateralFilter(void);
	/*ɾ������*/
	bool deleteData(void);
};


/*���ⲿ����.cu���Ե��õĺ���*/
/*ʹ��CPU�ܹ�����.cu�еĳ���*/
extern "C" {
	bool simplifiedFillMouth_CUDA(cv::cuda::GpuMat&, int, int);
	bool JointBilateralFilter_CUDA(cv::cuda::GpuMat&, cv::cuda::GpuMat&, cv::cuda::GpuMat&, cv::cuda::GpuMat&);
	bool genEIAImgs_CUDA(cv::cuda::GpuMat&, cv::cuda::GpuMat&, cv::cuda::GpuMat&, HARDWAREPARA&, cv::cuda::GpuMat*&, cv::cuda::GpuMat&);
	bool ParaImg2EIA_CUDA(cv::cuda::GpuMat&, int, HARDWAREPARA&, cv::cuda::GpuMat&);
}