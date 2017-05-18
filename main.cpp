#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

/*
@author: Francisco José Solís Muñoz
@date: 17th mayo 2017
@desc: This application segmentates an image
using bivariable histogram technique
*/

cv::Mat convertToHLS(cv::Mat image);
cv::Mat bivariableHist(unsigned int histHeight, unsigned int histWidth, cv::Mat firstMat, cv::Mat secondMat);

int main(int argc, char **argv)
{
	//Load image
	cv::Mat image,image32,hlsImage;
	cv::Mat hlsChannels[3];
	image = cv::imread("parrots2.jpg",CV_LOAD_IMAGE_COLOR);
	image.convertTo(image32,CV_32F);
	if(!image.data)
	{
		std::cout << "Error reading image, exiting...\n";
		return -1;
	}
	std::cout << "Image has been read, showing... Press any key to continue.\n";
    cv::namedWindow( "Original image", cv::WINDOW_NORMAL);
    cv::imshow( "Original image", image );	//Images are stored in BGR format
    //Convert to HLS
    image32 = image32/255.0;
    cv::cvtColor(image32,hlsImage,cv::COLOR_BGR2HLS);
    cv::split(hlsImage, hlsChannels);
    //Fix s
    hlsChannels[2].mul(1-2*abs(1/2-hlsChannels[1]));    
    //hlsChannels[2] = hlsChannels[2].*(1-2*abs(1/2-hlsChannels[1]));
    cv::Size imSize = hlsImage.size();
    //Channels are saved at hlsChannels, create the bivariable histogram
    cv::waitKey(0);
    double maxH,maxL,maxS,minH,minL,minS;
    cv::minMaxLoc(hlsChannels[0],&minH,&maxH);
    cv::minMaxLoc(hlsChannels[1],&minL,&maxL);
    cv::minMaxLoc(hlsChannels[2],&minS,&maxS);
    printf("H: %f %f\n",minH,maxH);
    printf("L: %f %f\n",minL,maxL);
    printf("S: %f %f\n",minS,maxS);

    cv::waitKey(0);


    const unsigned int histScale = 2;
    //Achromatic bivariable histogram
    cv::Mat histogram = bivariableHist(256*histScale, 256*histScale, hlsChannels[1]*256*histScale, hlsChannels[2]*256*histScale);
    histogram.convertTo(histogram,CV_32F);
    cv::log(histogram,histogram);
    //Autocontrast
    double minHist,maxHist;
    cv::minMaxLoc(histogram,&minHist,&maxHist);
    histogram = histogram/maxHist*255;
    cv::namedWindow( "Grayscale bivariable achromatic histogram", cv::WINDOW_NORMAL);
    cv::imshow( "Grayscale bivariable achromatic  histogram", histogram);
    cv::waitKey(0);

    //Chromatic bivariable histogram
    histogram = bivariableHist(360*histScale, 256*histScale, hlsChannels[0]*histScale, hlsChannels[2]*256*histScale);
    histogram.convertTo(histogram,CV_32F);
    cv::log(histogram,histogram);
    //Autocontrast
    cv::minMaxLoc(histogram,&minHist,&maxHist);
    histogram = histogram/maxHist*255;
    cv::namedWindow( "Grayscale bivariable chromatic histogram", cv::WINDOW_NORMAL);
    cv::imshow( "Grayscale bivariable chromatic  histogram", histogram);

    cv::waitKey(0);
    return 0;
}

/*
bivariableHist creates a variable histogram
from two channels of the image, using the first
one as rows and the second as columns
*/

cv::Mat bivariableHist(unsigned int histHeight, unsigned int histWidth, cv::Mat firstMat, cv::Mat secondMat)
{
	cv::Mat histogram;
	cv::Size imSize = firstMat.size();
	histogram = cv::Mat::zeros(histHeight, histWidth, CV_32S);
	std::cout << firstMat.size() << "->" << secondMat.size() << std::endl;
	for (int i = 0; i < imSize.height; ++i)
		for (int j = 0; j < imSize.width; ++j)
		{
			//printf("Calc'ing posY...\n");
			int posY=round(firstMat.at<float>(i,j));
			//printf("Calc'ing posX...\n");
			int posX=round(secondMat.at<float>(i,j));
			histogram.at<int>(
				posY,
				posX
			)++;
		}
	return histogram;
}

/*
convertToHLS converts an 8bit unsigned
RGB image to 32bit float signed image
*/
cv::Mat convertToHLS(cv::Mat image)
{
	cv::Size imSize = image.size();
	cv::Mat hlsImage(imSize.height*3,imSize.width*3,CV_32F);
	//Fills hlsImage
	cv::Vec3f hlsComponents;
	cv::Vec3b colComponents;
	float r,g,b;
	float *h=&hlsComponents.val[0],*l=&hlsComponents.val[1],*s=&hlsComponents.val[2];
	float vmax,vmin;
	for (int i = 0; i < imSize.height; ++i)
	{
		for (int j = 0; j < imSize.width; ++j)
		{
			colComponents = image.at<cv::Vec3b>(i,j);
			r=(float)colComponents.val[2]/255;
			g=(float)colComponents.val[1]/255;
			b=(float)colComponents.val[0]/255;
			vmax = std::max(r,std::max(g,b));
			vmin = std::min(r,std::min(g,b));
			vmax /= 255;
			vmin /= 255;
			*l=(vmax+vmin)/2;
			if(*l<0.5) *s=(vmax-vmin)/(vmax+vmin);
			else *s=(vmax-vmin)/(2-vmax-vmin);
			if(r==vmax) *h=(g-b)/(vmax-vmin);
			else if(g==vmax) *h=(b-r)/(vmax-vmin)+2;
			else *h=(r-g)/(vmax-vmin)+4;
			hlsImage.at<cv::Vec3f>(i,j) = hlsComponents;
			if(i==0&&j<5) printf("RGB: (%f,%f,%f) -> HLS: (%f,%f,%f)\n",r,g,b,*h,*l,*s);
		}
	}
	return hlsImage;
}