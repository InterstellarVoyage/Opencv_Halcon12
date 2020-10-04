#include <HalconCpp.h>    
#include <opencv2/opencv.hpp>  
#include <iostream>  
#include <windows.h>  

using namespace std;  
using namespace Halcon;  
using namespace cv;  

cv::Mat HImageToIplImage(Hobject& Hobj);
Hobject MatToHobject(Mat& image);

Hobject Image, GrayImage;
Hobject hObj;

int main(int argc, char* argv[])
{


	//Hobject - to - Image
	read_image(&Image, "D:/OMEN/Opencv_Halcon12/Opencv_Halcon/lena.jpg");
	cv::Mat opencvImg = HImageToIplImage(Image);
	//rgb1_to_gray(Image, &GrayImage);
	//Mat img = imread("lena.jpg");
	//cv::Mat opencvImg = HImageToIplImage(GrayImage);
	namedWindow("img", 1);
	imshow("img", opencvImg);
	waitKey();
	system("pause");

	//Image - to - Hobject

	//Mat image = imread("D:/OMEN/Opencv_Halcon10/Opencv_Halcon/lena.jpg");//彩色图
	// //Mat image = imread("lena1.png", CV_LOAD_IMAGE_GRAYSCALE);//灰度图
	//hObj = MatToHobject(image);

	//HTuple winHandle;
	//HTuple w, h;

	//get_image_size(hObj, &w, &h);
	//open_window(0, 0, w, h, 0, "", "", &winHandle);
	//disp_obj(hObj, winHandle);

	//waitKey();
	//system("pause");
}

cv::Mat HImageToIplImage(Hobject& Hobj)
{
	cv::Mat pImage;
	HTuple htChannels;
	HTuple width, height;
	width = height = 0;
	//转换图像格式    
	convert_image_type(Hobj, &Hobj, "byte");
	count_channels(Hobj, &htChannels);
	HTuple cType;
	HTuple grayVal;
	HTuple ptr;

	if (htChannels[0].I() == 1)
	{
		get_image_pointer1(Hobj, &ptr, &cType, &width, &height);
		pImage = cv::Mat(height[0].I(), width[0].I(), CV_8UC1);
		pImage = cv::Mat::zeros(height[0].I(), width[0].I(), CV_8UC1);
		uchar* pdata = (uchar*)ptr[0].I();
		memcpy(pImage.data, pdata, width[0].I() * height[0].I());

		//get_image_size(Hobj, &width, &height);
		//pImage = cv::Mat(height[0].I(), width[0].I(), CV_8UC1);
		//pImage = cv::Mat::zeros(height[0].I(), width[0].I(), CV_8UC1);
		//for (int i = 0; i < height[0].I(); ++i)
		//{
		//	for (int j = 0; j < width[0].I(); ++j)
		//	{
		//		get_grayval(Hobj, i, j, &grayVal);
		//		*pImage.ptr<uchar>(i, j) = (uchar)grayVal[0].I();
		//	}
		//}
	}
	else if (htChannels[0].I() == 3)
	{
		HTuple ptrR, ptrG, ptrB;
		get_image_size(Hobj, &width, &height);
		get_image_pointer3(Hobj, &ptrR, &ptrG, &ptrB, &cType, &width, &height);
		pImage = cv::Mat(height[0].I(), width[0].I(), CV_8UC3);

		vector<cv::Mat> vecM(3);
		vecM[2].create(height[0].I(), width[0].I(), CV_8UC1);
		vecM[1].create(height[0].I(), width[0].I(), CV_8UC1);
		vecM[0].create(height[0].I(), width[0].I(), CV_8UC1);
		uchar* pr = (uchar*)ptrR[0].I();
		uchar* pg = (uchar*)ptrG[0].I();
		uchar* pb = (uchar*)ptrB[0].I();
		memcpy(vecM[2].data, pr, width[0].I() * height[0].I());
		memcpy(vecM[1].data, pg, width[0].I() * height[0].I());
		memcpy(vecM[0].data, pb, width[0].I() * height[0].I());
		cv::merge(vecM, pImage);

		//get_image_size(Hobj, &width, &height);
		//pImage = cv::Mat(height[0].I(), width[0].I(), CV_8UC3);
		//for (int row = 0; row < height[0].I(); row++)
		//{
		//	for (int col = 0; col < width[0].I(); col++)
		//	{
		//		get_grayval(Hobj, row, col, &grayVal);
		//		*pImage.ptr<uchar>(row, col * 3) = (uchar)grayVal[2].I();
		//		*pImage.ptr<uchar>(row, col * 3 + 1) = (uchar)grayVal[1].I();
		//		*pImage.ptr<uchar>(row, col * 3 + 2) = (uchar)grayVal[0].I();

		//	}
		//}
	}
	return pImage;
}

// Mat转换为Hobject
Hobject MatToHobject(Mat& image)
{
	if (image.channels() == 1)
	{
		int height = image.rows;
		int width = image.cols;
		uchar* dataGray = new uchar[width * height];

		for (int i = 0; i < height; i++)
		{
			memcpy(dataGray + width * i, image.ptr() + image.step * i, width);
		}
		gen_image1(&hObj, "byte", width, height, (Hlong)(dataGray));
		delete[] dataGray;
	}
	else if (image.channels() == 3)
	{
		Mat ichannels[3];
		Mat imgB;
		Mat imgG;
		Mat imgR;
		// 把一个3通道图像转换成3个单通道图像  
		split(image, ichannels);//分离色彩通道  
		imgB = ichannels[0];
		imgG = ichannels[1];
		imgR = ichannels[2];

		int height = image.rows;
		int width = image.cols;

		uchar* dataRed = new uchar[width * height];
		uchar* dataGreen = new uchar[width * height];
		uchar* dataBlue = new uchar[width * height];


		for (int i = 0; i < height; i++)
		{
			memcpy(dataRed + width * i, imgR.ptr() + imgR.step * i, width);
			memcpy(dataGreen + width * i, imgG.ptr() + imgB.step * i, width);
			memcpy(dataBlue + width * i, imgB.ptr() + imgB.step * i, width);
		}
		gen_image3(&hObj, "byte", width, height, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
		delete[] dataRed;
		delete[] dataGreen;
		delete[] dataBlue;
	}
	return hObj;
}