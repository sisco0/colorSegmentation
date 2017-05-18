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

int main(int argc, char **argv)
{
	//Load image
	cv::Mat image,image32,hlsImage,hlsChannels[3];
	image = cv::imread("parrots.jpg",CV_LOAD_IMAGE_COLOR);
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
    hlsImage = convertToHLS(image);
    cv::split(hlsImage, hlsChannels);
    cv::Size imSize = hlsImage.size();
    printf("\n");
    //Channels are saved at hlsChannels, create the bivariable histogram
    /*double minH,maxH,minL,maxL,minS,maxS;
    cv::minMaxLoc(hlsChannels[0],&minH,&maxH);
    cv::minMaxLoc(hlsChannels[1],&minL,&maxL);
    cv::minMaxLoc(hlsChannels[2],&minS,&maxS);
    printf("H: [%lf,%lf]    L: [%lf,%lf]    S: [%lf,%lf]\n",minH,maxH,minL,maxL,minS,maxS);
    for (int i = 0; i < imSize.width*imSize.height; ++i)
    {
    	printf("%lf\n",hlsChannels[2].at<float>(i));
    }*/
    cv::waitKey(0);
    return 0;
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
	//DEBUG
    for (int i = 0; i < 15; ++i)
    {
    	if(i%3==0) printf("\n");
    	float hlsComp = hlsImage.at<float>(0,i);
    	printf("%f ",hlsComp);
    }
	return hlsImage;
}