//#include <iostream>
//#include <cstdio>
//#include <cuda.h>
//#include <cuda_runtime.h>
//#include <device_launch_parameters.h>
//#include <opencv2/core.hpp>
//#include <opencv2/core/cuda.hpp>
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>
//#include "RGBD2EIA.h"
//
//#define NUM_PATTERNS 8
//#define NUM_PI 3.14159
//
//long timeFlag[10] = { 0 };
//
/////***
//// *** 5张视差图并行计算生成视差图
////*/
////__global__ void CalcParaImageKernel(cv::cuda::PtrStepSz<uchar> depthMap_gpu, cv::cuda::PtrStepSz<uchar3> RGBMap_gpu, double paraAngle,
////	int cameraRows, int cameraCols, int depthThreshold, cv::cuda::PtrStepSz<uchar3>* ParaMat_gpu)
////{
////
////	//int coorX = blockIdx.x * blockDim.x + threadIdx.x;
////	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
////	int iCamera = blockIdx.z;
////
////	printf("thread %d successfully ran\n", coorY);
////
////	/*不满足阈值赋值为0*/
////	uchar3 zeros;
////	zeros.x = 0; zeros.y = 0; zeros.z = 0;
////	if (iCamera > blockDim.z / 2) {
////		/*按列遍历，每列同时异步处理*/
////		for (int iCoorX = 0; iCoorX < depthMap_gpu.cols; iCoorX++)
////		{
////			int coorParaX = (iCoorX - (int(depthMap_gpu.ptr(coorY)[iCoorX]) - 50) * paraAngle * (iCamera - cameraCols / 2));
////			int coorParaY = coorY;
////
////			if (iCoorX >= depthMap_gpu.cols || iCoorX < 0 || coorY >= depthMap_gpu.rows || coorY < 0) return;
////			if (coorParaX >= depthMap_gpu.cols || coorParaX < 0 || coorParaY >= depthMap_gpu.rows || coorParaY < 0) return;
////
////
////
////			/*如果小于深度阈值，则该像素直接舍弃*/
////			if (depthMap_gpu.ptr(coorY)[iCoorX] < depthThreshold) {
////				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = zeros;
////			}
////			else {
////				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = RGBMap_gpu.ptr(coorY)[iCoorX];
////
////				/*空洞填补*/
////				if ((ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].x == zeros.x) &&
////					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].y == zeros.y) &&
////					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].z == zeros.z)) {
////					if (coorParaX >= 1) {
////						ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX - 1];
////					}
////				}
////			}
////		}
////	}
////	else {
////		/*按列遍历，每列同时异步处理*/
////		for (int iCoorX = depthMap_gpu.cols - 1; iCoorX >= 0; iCoorX--)
////		{
////			int coorParaX = (iCoorX - (int(depthMap_gpu.ptr(coorY)[iCoorX]) - 50) * paraAngle * (iCamera - cameraCols / 2));
////			int coorParaY = coorY;
////
////			if (iCoorX >= depthMap_gpu.cols || iCoorX < 0 || coorY >= depthMap_gpu.rows || coorY < 0) return;
////			if (coorParaX >= depthMap_gpu.cols || coorParaX < 0 || coorParaY >= depthMap_gpu.rows || coorParaY < 0) return;
////
////
////
////			/*如果小于深度阈值，则该像素直接舍弃*/
////			if (depthMap_gpu.ptr(coorY)[iCoorX] < depthThreshold) {
////				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = zeros;
////			}
////			else {
////				ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = RGBMap_gpu.ptr(coorY)[iCoorX];
////
////				/*空洞填补*/
////				if ((ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].x == zeros.x) &&
////					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].y == zeros.y) &&
////					(ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX].z == zeros.z)) {
////					if (coorParaX >= 1) {
////						ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX] = ParaMat_gpu[iCamera].ptr(coorParaY)[coorParaX - 1];
////					}
////				}
////			}
////		}
////	}
////
////
////}
//
///***
// *** 正向遍历生成视差图
//*/
//__global__ void CalcParaImageKernel1(cv::cuda::PtrStepSz<uchar> depthMap_gpu, cv::cuda::PtrStepSz<uchar3> RGBMap_gpu, double paraAngle,
//	int cameraRows, int cameraCols, int iCamera, int depthThreshold, cv::cuda::PtrStepSz<uchar3> ParaMat_gpu)
//{
//	
//	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
//	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
//	///*测试用*/
//	//printf("thread %d successfully ran\n", coorY);
//
//	/*不满足阈值赋值为0*/
//	uchar3 zeros;
//	zeros.x = 0; zeros.y = 0; zeros.z = 0;
//	/*按列遍历，每列同时异步处理*/
//	int coorParaX = (coorX - (int(depthMap_gpu.ptr(coorY)[coorX]) - 50) * paraAngle * (iCamera - cameraCols / 2));
//	int coorParaY = coorY;
//
//	if (coorX >= depthMap_gpu.cols || coorX < 0 || coorY >= depthMap_gpu.rows || coorY < 0) return;
//	if (coorParaX >= depthMap_gpu.cols || coorParaX < 0 || coorParaY >= depthMap_gpu.rows || coorParaY < 0) return;
//		
//
//	/*如果小于深度阈值，则该像素直接舍弃*/
//	if (depthMap_gpu.ptr(coorY)[coorX] < depthThreshold) {
//		ParaMat_gpu.ptr(coorParaY)[coorParaX] = zeros;
//	}
//	else {
//		ParaMat_gpu.ptr(coorParaY)[coorParaX] = RGBMap_gpu.ptr(coorY)[coorX];
//	}		
//}
//
///***
// *** 反向遍历生成视差图
//*/
//__global__ void CalcParaImageKernel2(cv::cuda::PtrStepSz<uchar> depthMap_gpu, cv::cuda::PtrStepSz<uchar3> RGBMap_gpu, double paraAngle,
//	int cameraRows, int cameraCols, int iCamera, int depthThreshold, cv::cuda::PtrStepSz<uchar3> ParaMat_gpu)
//{
//	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
//	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
//	/*不满足阈值赋值为0*/
//	uchar3 zeros;
//	zeros.x = 0; zeros.y = 0; zeros.z = 0;
//
//	int coorParaX = (coorX - (int(depthMap_gpu.ptr(coorY)[coorX]) - 10) * paraAngle * (iCamera - cameraCols / 2));
//	int coorParaY = coorY;
//
//	if (coorX >= depthMap_gpu.cols || coorX < 0 || coorY >= depthMap_gpu.rows || coorY < 0) return;
//	if (coorParaX >= depthMap_gpu.cols || coorParaX < 0 || coorParaY >= depthMap_gpu.rows || coorParaY < 0) return;
//
//	/*如果小于深度阈值，则该像素直接舍弃*/
//	if (depthMap_gpu.ptr(coorY)[coorX] < depthThreshold) {
//		ParaMat_gpu.ptr(coorParaY)[coorParaX] = zeros;
//	}
//	else {
//		ParaMat_gpu.ptr(coorParaY)[coorParaX] = RGBMap_gpu.ptr(coorY)[coorX];
//	}	
//}
//
///*空洞填补*/
//__global__ void FillVacuum_backward(cv::cuda::PtrStepSz<uchar3> ParaMat_gpu) {
//	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
//	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
//
//	uchar3 zeros;
//	zeros.x = 0; zeros.y = 0; zeros.z = 0;
//
//	if ((ParaMat_gpu.ptr(coorY)[coorX].x == zeros.x)&&
//		(ParaMat_gpu.ptr(coorY)[coorX].y == zeros.y)&&
//		(ParaMat_gpu.ptr(coorY)[coorX].z == zeros.z)) {
//		if (coorX < ParaMat_gpu.cols - 1) {
//			ParaMat_gpu.ptr(coorY)[coorX] = ParaMat_gpu.ptr(coorY)[coorX + 1];
//		}		
//	}
//}
//__global__ void FillVacuum_forward(cv::cuda::PtrStepSz<uchar3> ParaMat_gpu) {
//	int coorX = blockIdx.x * blockDim.x + threadIdx.x;
//	int coorY = blockIdx.y * blockDim.y + threadIdx.y;
//
//	uchar3 zeros;
//	zeros.x = 0; zeros.y = 0; zeros.z = 0;
//
//	if ((ParaMat_gpu.ptr(coorY)[coorX].x == zeros.x) &&
//		(ParaMat_gpu.ptr(coorY)[coorX].y == zeros.y) &&
//		(ParaMat_gpu.ptr(coorY)[coorX].z == zeros.z)) {
//		if (coorX > 0) {
//			ParaMat_gpu.ptr(coorY)[coorX] = ParaMat_gpu.ptr(coorY)[coorX - 1];
//		}
//	}
//}
//
//
//
///*----------------------------
//* 功能 : 读入RGB处理成Stereo cv::Mat-------------------非整数视点代码，记得注释掉整数视点代码，就在上面
//*----------------------------
//* 函数 : RemapRGBMat2StereoMat_KERNEL
//* 访问 : public
//* 返回 : 0 - 操作失败，1 - 操作成功
//*
//* 参数 : inRGBMat_CUDA		[in]		输入视差图序列（RGB格式）
//* 参数 : iCurCam			[in]		当前相机编号
//* 参数 : effCamNum			[in]		有效相机总数
//* 参数 : slope				[in]		斜率
//* 参数 : outStereoMat_CUDA	[in/out]	输出，处理完图像(StereoMat)
//*/
//__global__ void RemapRGBMat2StereoMat_KERNEL(cv::cuda::PtrStepSz<uchar3> inRGBMat_CUDA, int iCurCam, double effCamNum, double slope, cv::cuda::PtrStepSz<uchar3> outStereoMat_CUDA)
//{
//	// 遍历子坐标
//	int coorDim_X = blockIdx.x * blockDim.x + threadIdx.x;
//	int coorDim_Y = blockIdx.y * blockDim.y + threadIdx.y;
//
//	// 输入图像（视差图）坐标
//	int coorIn_X = coorDim_X / 3 * inRGBMat_CUDA.cols / outStereoMat_CUDA.cols;
//	int coorIn_Y = coorDim_Y * inRGBMat_CUDA.rows / outStereoMat_CUDA.rows;
//
//	// 输出图像（合成图像）坐标
//	int coorOut_X = coorDim_X / 3;
//	int coorOut_Y = coorDim_Y;
//	int isubx = coorDim_X % 3;
//
//	float coordinate = 0.000000f;
//
//	//非整数视点个数
//	float effViewpointNum = 4.666666f;
//
//
//	if (coorDim_Y % 2 == 0)
//	{
//		coordinate = coorDim_X + coorDim_Y * 3 * slope + outStereoMat_CUDA.cols * 3;
//	}
//	else
//	{
//		coordinate = coorDim_X + coorDim_Y * 3 * slope + outStereoMat_CUDA.cols * 3 + (float)1 / 3;
//	}
//
//
//
//	int ifactor = effCamNum - 1 - floor(fmodf(coordinate, effViewpointNum));
//
//
//	if (ifactor == iCurCam)
//	{
//		if (coorIn_X >= 0 && coorIn_X < inRGBMat_CUDA.cols && coorIn_Y >= 0 && coorIn_Y < inRGBMat_CUDA.rows)
//		{
//			if (coorOut_X >= 0 && coorOut_X < outStereoMat_CUDA.cols && coorOut_Y >= 0 && coorOut_Y < outStereoMat_CUDA.rows)
//			{
//				if (isubx == 0)
//				{
//					outStereoMat_CUDA.ptr(coorOut_Y)[coorOut_X].x = inRGBMat_CUDA.ptr(coorIn_Y)[coorIn_X].x;
//				}
//				else if (isubx == 1)
//				{
//					outStereoMat_CUDA.ptr(coorOut_Y)[coorOut_X].y = inRGBMat_CUDA.ptr(coorIn_Y)[coorIn_X].y;
//				}
//				else
//				{
//					outStereoMat_CUDA.ptr(coorOut_Y)[coorOut_X].z = inRGBMat_CUDA.ptr(coorIn_Y)[coorIn_X].z;
//				}
//			}
//		}
//	}
//}
//
//extern "C" bool genEIAImgs_CUDA(cv::cuda::GpuMat& RGBMap_gpu, cv::cuda::GpuMat& depthMap_gpu,HARDWAREPARA& hardwarePara, cv::cuda::GpuMat* &paraImages_gpu, cv::cuda::GpuMat& eiaImage_gpu)
//{	
//	timeFlag[2] = clock();
//	///*方法一，五张视差图并行处理*/
//	///*未调通*/
//	//dim3 blockSize(4, depthMap_gpu.rows / 5);
//	//dim3 gridSize(depthMap_gpu.cols / blockSize.x, 5, hardwarePara._VIEWNUM);
//
//	///*测试用*/
//	//cv::Mat Para_obs = cv::Mat::zeros(paraImages_gpu[0].size(), CV_8UC3);
//	//paraImages_gpu[0].download(Para_obs);
//	//cv::imwrite("output/observerPara.jpg", Para_obs);
//
//	///*合成视差图*/
//	//cv::cuda::PtrStepSz<uchar3>* paraImages_gpu_PtrStepSz = new cv::cuda::PtrStepSz<uchar3>[hardwarePara._VIEWNUM];
//	//for (int i = 0; i < hardwarePara._VIEWNUM; i++) {
//	//	paraImages_gpu_PtrStepSz[i] = paraImages_gpu[i];
//	//}
//	//CalcParaImageKernel << <gridSize, blockSize >> > (depthMap_gpu, RGBMap_gpu, hardwarePara._DISPARITYANGLE, 1, hardwarePara._VIEWNUM,
//	//	hardwarePara._DEPTHTHRESHOLD, paraImages_gpu_PtrStepSz);
//	//cudaDeviceSynchronize();
//
//	///*测试用*/				
//	//paraImages_gpu[0].download(Para_obs);
//	//cv::imwrite("output/observerPara.jpg", Para_obs);
//	//for (int iCamera = 0; iCamera < hardwarePara._VIEWNUM; iCamera++)
//	//{
//	//	/*将视差图RGB映射到EIA*/
//	//	RemapRGBMat2StereoMat_CUDA(paraImages_gpu[iCamera], iCamera, hardwarePara, eiaImage_gpu);
//	//	/*测试用*/
//	//	cv::Mat eia_obs = cv::Mat::zeros(eiaImage_gpu.size(), CV_8UC3);
//	//	eiaImage_gpu.download(eia_obs);
//	//	cv::imwrite("output/observerEIA.jpg", eia_obs);
//	//}
//
//	/*方法二，五张视差图串行处理*/
//	for (int iCamera = 0; iCamera < hardwarePara._VIEWNUM; iCamera++)
//	{
//		
//
//		/*合成视差图*/
//		timeFlag[4] = clock();
//		dim3 blockSize(4, depthMap_gpu.rows / 5);
//		dim3 gridSize(depthMap_gpu.cols / blockSize.x, 5);
//
//		// do it in Kernel
//		CalcParaImageKernel1 << <gridSize, blockSize >> > (depthMap_gpu, RGBMap_gpu, hardwarePara._DISPARITYANGLE, 1, hardwarePara._VIEWNUM,
//			iCamera, hardwarePara._DEPTHTHRESHOLD, paraImages_gpu[iCamera]);
//
//		timeFlag[5] = clock();
//		/*空洞填补*/
//		for (int iFillTimes = 0; iFillTimes < 4; iFillTimes++) {
//			for (int iParaImg = 0; iParaImg < hardwarePara._VIEWNUM; iParaImg++) {
//				if (iParaImg < hardwarePara._VIEWNUM / 2) {
//					FillVacuum_backward << <gridSize, blockSize >> > (paraImages_gpu[iParaImg]);
//				}
//				else {
//					FillVacuum_forward << <gridSize, blockSize >> > (paraImages_gpu[iParaImg]);
//				}
//			}
//		}		
//		timeFlag[6] = clock();
//		
//		/*将视差图RGB映射到EIA*/
//		RemapRGBMat2StereoMat_CUDA(paraImages_gpu[iCamera], iCamera, hardwarePara, eiaImage_gpu);
//		//cudaDeviceSynchronize();
//		
//		timeFlag[7] = clock();
//	}	
//	return true;
//}
//
///*----------------------------
// * 功能 : 读入RGB处理成II Eia cv::Mat
// *----------------------------
// * 函数 : RemapRGBMat2EiaMat_CUDA
// * 访问 : public
// * 返回 : 0 - 操作失败，1 - 操作成功
// *
// * 参数 : inRGBMat_GPU		[in]		输入视差图序列（RGB格式）
// * 参数 : iCurCam			[in]		当前相机编号
// * 参数 : camAayPer			[in]		相机及显示配置参数
// * 参数 : outEiaMat_GPU		[in/out]	输出，处理完图像(EiaMat)
// */
//extern "C" bool RemapRGBMat2StereoMat_CUDA(cv::cuda::GpuMat& inRGBMat_GPU, int iCurCam, HARDWAREPARA& hardwarePara, cv::cuda::GpuMat& outEiaMat_GPU)
//{
//	dim3 blockSize(30, 30);
//	int pixel_W = outEiaMat_GPU.cols * outEiaMat_GPU.channels();;
//	int pixel_H = outEiaMat_GPU.rows;
//	dim3 gridSize(pixel_W/ blockSize.x, pixel_H /blockSize.y);
//
//	RemapRGBMat2StereoMat_KERNEL << <gridSize, blockSize >> > (inRGBMat_GPU, iCurCam, hardwarePara._EFFICIENTVIEWNUM
//		, hardwarePara._EFFICIENTSLOPE, outEiaMat_GPU);
//	return true;
//}
//
//
