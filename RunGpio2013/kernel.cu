#include <iostream>
#include <cstdio>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "RGBD2EIA.h"

#define NUM_PATTERNS 8
#define NUM_PI 3.14159

long timeFlag[20] = { 0 };

///*并行生成多张视差图*/
//__global__ void CalcParaImageKernel(cv::cuda::PtrStepSz<uchar> depthMap_gpu, cv::cuda::PtrStepSz<uchar3> RGBMap_gpu, double paraAngle,
//	int cameraRows, int cameraCols, int depthThreshold, cv::cuda::PtrStepSz<uchar3>* ParaMat_gpu)
//{
//
//	//int coorX = blockIdx.x * blockDim.x + threadIdx.x;
//	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
//	int iCamera = blockIdx.z;
//
//	printf("thread %d successfully ran\n", coorY);
//
//	/*不满足阈值赋值为0*/
//	uchar3 zeros;
//	zeros.x = 0; zeros.y = 0; zeros.z = 0;
//	if (iCamera > blockDim.z / 2) {
//		/*按列遍历，每列同时异步处理*/
//		for (int iCoorX = 0; iCoorX < depthMap_gpu.cols; iCoorX++)
//		{
//			int coorParaX = (iCoorX - (int(depthMap_gpu.ptr(coorY)[iCoorX]) - 50) * paraAngle * (iCamera - cameraCols / 2));
//			int coorParaY = coorY;
//
//			if (iCoorX >= depthMap_gpu.cols || iCoorX < 0 || coorY >= depthMap_gpu.rows || coorY < 0) return;
//			if (coorParaX >= depthMap_gpu.cols || coorParaX < 0 || coorParaY >= depthMap_gpu.rows || coorParaY < 0) return;
//
//
//
//			/*如果小于深度阈值，则该像素直接舍弃*/
//			if (depthMap_gpu.ptr(coorY)[iCoorX] < depthThreshold) {
//				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = zeros;
//			}
//			else {
//				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = RGBMap_gpu.ptr(coorY)[iCoorX];
//
//				/*空洞填补*/
//				if ((ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].x == zeros.x) &&
//					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].y == zeros.y) &&
//					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].z == zeros.z)) {
//					if (coorParaX >= 1) {
//						ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX - 1];
//					}
//				}
//			}
//		}
//	}
//	else {
//		/*按列遍历，每列同时异步处理*/
//		for (int iCoorX = depthMap_gpu.cols - 1; iCoorX >= 0; iCoorX--)
//		{
//			int coorParaX = (iCoorX - (int(depthMap_gpu.ptr(coorY)[iCoorX]) - 50) * paraAngle * (iCamera - cameraCols / 2));
//			int coorParaY = coorY;
//
//			if (iCoorX >= depthMap_gpu.cols || iCoorX < 0 || coorY >= depthMap_gpu.rows || coorY < 0) return;
//			if (coorParaX >= depthMap_gpu.cols || coorParaX < 0 || coorParaY >= depthMap_gpu.rows || coorParaY < 0) return;
//
//
//
//			/*如果小于深度阈值，则该像素直接舍弃*/
//			if (depthMap_gpu.ptr(coorY)[iCoorX] < depthThreshold) {
//				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = zeros;
//			}
//			else {
//				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = RGBMap_gpu.ptr(coorY)[iCoorX];
//
//				/*空洞填补*/
//				if ((ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].x == zeros.x) &&
//					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].y == zeros.y) &&
//					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].z == zeros.z)) {
//					if (coorParaX >= 1) {
//						ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX - 1];
//					}
//				}
//			}
//		}
//	}
//
//
//}


/*
* 函数名：GenParaImage_KERNEL
* 功能：CUDA并行计算生成单张视差图
* 输入：	
*	RGBMap_gpu	【原RGB图（GPU）】
*	depthMap_gpu	【经过预处理的Depth图（GPU）】
*	backgraound_gpu	【背景填充图（GPU）】
*	paraAngle	【相邻视点视角差】
*	cameraRows	【相机行数（二维集成成像EIA生成时用到）】
*	cameraCols	【相机列数】
*	iCamera		【视差图序号/相机索引】
*	depthThreshold	【深度阈值】
* 输出：
*	ParaMat_gpu	【视差图（GPU）】
*/
__global__ void GenParaImage_KERNEL(cv::cuda::PtrStepSz<uchar3> RGBMap_gpu, cv::cuda::PtrStepSz<uchar> depthMap_gpu, cv::cuda::PtrStepSz<uchar3> backgraound_gpu,
	double paraAngle, int cameraRows, int cameraCols, int iCamera, int depthThreshold, cv::cuda::PtrStepSz<uchar3> ParaMat_gpu)
{
	
	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
	///*测试用*/
	//printf("thread %d successfully ran\n", coorY);

	/*不满足阈值赋值为0*/
	uchar3 zeros;
	zeros.x = 0; zeros.y = 0; zeros.z = 0;
	/*按列遍历，每列同时异步处理*/
	int coorParaX = (coorX - (int(depthMap_gpu.ptr(coorY)[coorX]) + 50) * paraAngle * (iCamera - cameraCols / 2));
	int coorParaY = coorY;

	if (coorX >= depthMap_gpu.cols || coorX < 0 || coorY >= depthMap_gpu.rows || coorY < 0) return;
	if (coorParaX >= depthMap_gpu.cols || coorParaX < 0 || coorParaY >= depthMap_gpu.rows || coorParaY < 0) return;
		

	/*如果小于深度阈值，则该像素直接舍弃*/
	if (depthMap_gpu.ptr(coorY)[coorX] < depthThreshold) {
		ParaMat_gpu.ptr(coorParaY)[coorParaX] = 
			backgraound_gpu.ptr((int)(coorParaY*backgraound_gpu.rows/depthMap_gpu.rows))
			[(int)(coorParaX*backgraound_gpu.cols / depthMap_gpu.cols)];
	}
	else {
		ParaMat_gpu.ptr(coorParaY)[coorParaX] = RGBMap_gpu.ptr(coorY)[coorX];
	}		
}


/*
* 函数名：FillVacuum_forward_KERNEL
* 功能：视差图反向空洞填补
* 输入：ParaMat_gpu	【RGB视差图（GPU）】
* 输出：ParaMat_gpu 【RGB视差图（GPU）】
*/
__global__ void FillVacuum_backward_KERNEL(cv::cuda::PtrStepSz<uchar3> ParaMat_gpu) {
	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
	int coorY = blockIdx.y * blockDim.y + threadIdx.y;

	uchar3 zeros;
	zeros.x = 0; zeros.y = 0; zeros.z = 0;

	if ((ParaMat_gpu.ptr(coorY)[coorX].x == zeros.x)&&
		(ParaMat_gpu.ptr(coorY)[coorX].y == zeros.y)&&
		(ParaMat_gpu.ptr(coorY)[coorX].z == zeros.z)) {
		if (coorX < ParaMat_gpu.cols - 1) {
			ParaMat_gpu.ptr(coorY)[coorX] = ParaMat_gpu.ptr(coorY)[coorX + 1];
		}		
	}
}


/*
* 函数名：FillVacuum_forward_KERNEL
* 功能：视差图前向空洞填补
* 输入：ParaMat_gpu	【RGB视差图】
* 输出：ParaMat_gpu 【RGB视差图】
*/
__global__ void FillVacuum_forward_KERNEL(cv::cuda::PtrStepSz<uchar3> ParaMat_gpu) {
	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
	int coorY = blockIdx.y * blockDim.y + threadIdx.y;

	uchar3 zeros;
	zeros.x = 0; zeros.y = 0; zeros.z = 0;

	if ((ParaMat_gpu.ptr(coorY)[coorX].x == zeros.x) &&
		(ParaMat_gpu.ptr(coorY)[coorX].y == zeros.y) &&
		(ParaMat_gpu.ptr(coorY)[coorX].z == zeros.z)) {
		if (coorX > 0) {
			ParaMat_gpu.ptr(coorY)[coorX] = ParaMat_gpu.ptr(coorY)[coorX - 1];
		}
	}
}


/*
* 函数名：simplifiedFillMouth_KERNEL
* 功能：CUDA并行简易嘴部填充
* 输入：
*	depthImg	【原始深度图】
*	ctrPosX		【最大深度横坐标】
*	ctrPosY		【最大深度纵坐标】
* 输出：
*	depthImg	【直接在原图修改后输出】
*/
__global__ void simplifiedFillMouth_KERNEL(cv::cuda::PtrStepSz<uchar> depthGreySrc, int ctrPosX, int ctrPosY) {
	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
	if ((ctrPosY - coorY < 0) &&
		(sqrt(pow((double)(ctrPosY - coorY),2)+ pow((double)(ctrPosX - coorX), 2)) < 0.194 * depthGreySrc.rows) &&
		(abs((double)ctrPosX - coorX) < 0.0625 * depthGreySrc.cols)) {
		if (depthGreySrc.ptr(coorY)[coorX] <= 50) {
			depthGreySrc.ptr(coorY)[coorX] = 150;
		}
	}
}


/*
* 函数名：JointBilateralFilter_KERNEL
* 功能：CUDA并行双边滤波
* 输入：
*	colorImgSrc	【转为单通道的RGB图（GPU）】
*	depthImgSrc	【深度图（GPU）】
*	blackhatImg	【双边滤波限定区域蒙版（GPU）】
* 输出：
*	depthImgDst	【嘴部填充后的深度图（GPU）】
*/
__global__ void JointBilateralFilter_KERNEL(cv::cuda::PtrStepSz<uchar> colorImgSrc, cv::cuda::PtrStepSz<uchar> depthImgSrc, 
	cv::cuda::PtrStepSz<uchar> blackhatImg, cv::cuda::PtrStepSz<uchar> depthImgDst) {
	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
	/*如果非0，那么是嘴部所处的连通区域，需要进行填补*/
	if (blackhatImg.ptr(coorY)[coorX] != 0) {
		///*对找出的嘴部进行等值化简化处理*/
		//depthImgDst.ptr(coorY)[coorX] = 150;
		//printf("一切正常，对(%d,%d)处像素进行处理\n", coorX, coorY);
		/*合适的距离和颜色权重*/
		double sigma_dis = 20;
		double sigma_color = 5;
		/*正方形邻域的大小*/
		int r = 30;
		/*获得[r,r]邻域的起点终点并防止越界*/
		int iMin = MAX(coorY - r, 0),
			iMax = MIN(coorY + r, depthImgSrc.rows - 1),
			jMin = MAX(coorX - r, 0),
			jMax = MIN(coorX + r, depthImgSrc.cols - 1);
		/*计算颜色和距离的权重*/
		double w_color, w_distance, w;
		double totalWeight = 0;
		double totalDepthValue = 0;
		for (int iNeighbour = iMin; iNeighbour <= iMax; iNeighbour++) {
			for (int jNeighbour = jMin; jNeighbour <= jMax; jNeighbour++) {
				w_color = exp(
					-pow((double)(colorImgSrc.ptr(iNeighbour)[jNeighbour] - colorImgSrc.ptr(coorY)[coorX])/255.0f, 2)/
					(double)(2 * pow(sigma_color, 2))
				);
				w_distance = exp(
					-(double)(pow((double)(iNeighbour - coorY), 2) + pow((double)(jNeighbour - coorX), 2)) /
					(double)(2 * pow(sigma_dis, 2))
				);
				w = w_color * w_distance;
				/*使邻域深度为0的像素点对目标像素无贡献*/
				totalWeight += w * (depthImgSrc.ptr(iNeighbour)[jNeighbour] == 0 ? 0 : 1);
				totalDepthValue += w * depthImgSrc.ptr(iNeighbour)[jNeighbour];
			}
		}
		depthImgDst.ptr(coorY)[coorX] = (int)(totalDepthValue / totalWeight);
		//printf("greyscale=\t%d\n", depthImgDst.ptr(coorY)[coorX]);
	}
}


/*
* 函数名：ParaImg2EIA_KERNEL
* 功能：将单张视差图按像素映射到EIA矩阵
* 输入：
*	inRGBMat_CUDA	【单张RGB视差图（GPU）】
*	iCurCam		【视差图序号/相机索引（GPU）】
*	effViewpointNum	【有效视点数】
*	slope		【光栅斜率】
* 输出：
*	outStereoMat_CUDA	【部分像素已经排列好的EIA（GPU）】
*/
__global__ void ParaImg2EIA_KERNEL(cv::cuda::PtrStepSz<uchar3> inRGBMat_CUDA, int iCurCam, double effViewpointNum, double slope, cv::cuda::PtrStepSz<uchar3> outStereoMat_CUDA)
{
	// 遍历子坐标
	int coorDim_X = blockIdx.x * blockDim.x + threadIdx.x;
	int coorDim_Y = blockIdx.y * blockDim.y + threadIdx.y;

	// 输入图像（视差图）坐标
	int coorIn_X = coorDim_X / 3 * inRGBMat_CUDA.cols / outStereoMat_CUDA.cols;
	int coorIn_Y = coorDim_Y * inRGBMat_CUDA.rows / outStereoMat_CUDA.rows;

	// 输出图像（合成图像）坐标
	int coorOut_X = coorDim_X / 3;
	int coorOut_Y = coorDim_Y;
	int isubx = coorDim_X % 3;

	float coordinate = 0.000000f;

	if (coorDim_Y % 2 == 0)	{
		coordinate = coorDim_X + coorDim_Y * 3 * slope + outStereoMat_CUDA.cols * 3;
	}
	else{
		coordinate = coorDim_X + coorDim_Y * 3 * slope + outStereoMat_CUDA.cols * 3 + (float)1 / 3;
	}

	int ifactor = effViewpointNum - 1 - floor(fmodf(coordinate, effViewpointNum));

	if (ifactor == iCurCam)	{
		if (coorIn_X >= 0 && coorIn_X < inRGBMat_CUDA.cols && coorIn_Y >= 0 && coorIn_Y < inRGBMat_CUDA.rows){
			if (coorOut_X >= 0 && coorOut_X < outStereoMat_CUDA.cols && coorOut_Y >= 0 && coorOut_Y < outStereoMat_CUDA.rows){
				if (isubx == 0)				{
					outStereoMat_CUDA.ptr(coorOut_Y)[coorOut_X].x = inRGBMat_CUDA.ptr(coorIn_Y)[coorIn_X].x;
				}
				else if (isubx == 1){
					outStereoMat_CUDA.ptr(coorOut_Y)[coorOut_X].y = inRGBMat_CUDA.ptr(coorIn_Y)[coorIn_X].y;
				}
				else{
					outStereoMat_CUDA.ptr(coorOut_Y)[coorOut_X].z = inRGBMat_CUDA.ptr(coorIn_Y)[coorIn_X].z;
				}
			}
		}
	}
}


/*
* 功能：简易嘴部填充
* 输入：
*	depthImg	【原始深度图】
*	ctrPosX		【最大深度横坐标】
*	ctrPosY		【最大深度纵坐标】
* 输出：
*	depthImg	【直接在原图修改后输出】
*/
extern "C" bool simplifiedFillMouth_CUDA(cv::cuda::GpuMat& depthImg, int ctrPosX, int ctrPosY) {
	dim3 blockSize(4, depthImg.rows / 5);
	dim3 gridSize(depthImg.cols / blockSize.x, 5);
	simplifiedFillMouth_KERNEL << <gridSize, blockSize >> > (depthImg, ctrPosX, ctrPosY);
	return true;
}


/*
* 功能：限定区域双边滤波
* 输入：
*	colorGreyImgSrc	【RGB图转化的灰度图】
*	depthImgSrc		【原始深度图】
*	blackhatImg		【底帽滤波结果，双边滤波限定区域蒙版】
* 输出：
*	depthImgDst		【输出滤波后深度图】
*/
extern "C" bool JointBilateralFilter_CUDA(cv::cuda::GpuMat& colorGreyImgSrc, cv::cuda::GpuMat& depthImgSrc, cv::cuda::GpuMat& blackhatImg, cv::cuda::GpuMat& depthImgDst) {
	dim3 blockSize(4, blackhatImg.rows / 5);
	dim3 gridSize(blackhatImg.cols / blockSize.x, 5);
	JointBilateralFilter_KERNEL << <gridSize, blockSize >> > (colorGreyImgSrc, depthImgSrc, blackhatImg, depthImgDst);
	return true;
}


/*
* 功能：生成单帧EIA
* 输入：
*	RGBMap_gpu	【原RGB图（GPU）】
*	depthMap_gpu	【经过预处理的Depth图（GPU）】
*	backgraound_gpu	【背景填充图（GPU）】
*	hardwarePara	【硬件参数】
*	paraImages_gpu	【视差图数组（GPU）】
* 输出：
*	eiaImage_gpu	【合成的EIA（GPU）】
*/
extern "C" bool genEIAImgs_CUDA(cv::cuda::GpuMat& RGBMap_gpu, cv::cuda::GpuMat& depthMap_gpu, cv::cuda::GpuMat& backgroundImg_gpu,
	HARDWAREPARA& hardwarePara, cv::cuda::GpuMat* &paraImages_gpu, cv::cuda::GpuMat& eiaImage_gpu)
{		
	/*五张视差图串行处理*/
	/*全并行处理未调通，但整个合成视差图和EIA时间不足1ms，不是主要消耗算力的过程*/
	for (int iCamera = 0; iCamera < hardwarePara._VIEWNUM; iCamera++)
	{
		/*合成视差图*/
		dim3 blockSize(4, depthMap_gpu.rows / 5);
		dim3 gridSize(depthMap_gpu.cols / blockSize.x, 5);

		GenParaImage_KERNEL << <gridSize, blockSize >> > (RGBMap_gpu, depthMap_gpu, backgroundImg_gpu, hardwarePara._DISPARITYANGLE, 1, hardwarePara._VIEWNUM,
			iCamera, hardwarePara._DEPTHTHRESHOLD, paraImages_gpu[iCamera]);
		
		/*空洞填补*/
		for (int iFillTimes = 0; iFillTimes < 2; iFillTimes++) {
			for (int iParaImg = 0; iParaImg < hardwarePara._VIEWNUM; iParaImg++) {
				if (iParaImg < hardwarePara._VIEWNUM / 2) {
					FillVacuum_backward_KERNEL << <gridSize, blockSize >> > (paraImages_gpu[iParaImg]);
				}
				else {
					FillVacuum_forward_KERNEL << <gridSize, blockSize >> > (paraImages_gpu[iParaImg]);
				}
			}
		}		
		
		/*将视差图RGB映射到EIA*/
		ParaImg2EIA_CUDA(paraImages_gpu[iCamera], iCamera, hardwarePara, eiaImage_gpu);
		//cudaDeviceSynchronize();		
	}	
	return true;
}


/*
* 功能：将视差图像素映射到EIA上
* 输入：
*	inRGBMat_GPU	【RGB图像（GPU）】
*	iCurCam		【视差图的序号（GPU）】
*	hardwarePara	【硬件参数（GPU）】
* 输出：
*	outEiaMat_GPU	【EIA（GPU）】
*/
extern "C" bool ParaImg2EIA_CUDA(cv::cuda::GpuMat& inRGBMat_GPU, int iCurCam, HARDWAREPARA& hardwarePara, cv::cuda::GpuMat& outEiaMat_GPU)
{
	dim3 blockSize(30, 30);
	int pixel_W = outEiaMat_GPU.cols * outEiaMat_GPU.channels();;
	int pixel_H = outEiaMat_GPU.rows;
	dim3 gridSize(pixel_W/ blockSize.x, pixel_H /blockSize.y);

	ParaImg2EIA_KERNEL << <gridSize, blockSize >> > (inRGBMat_GPU, iCurCam, hardwarePara._EFFICIENTVIEWNUM
		, hardwarePara._EFFICIENTSLOPE, outEiaMat_GPU);
	return true;
}


