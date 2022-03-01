#pragma once                     
#ifndef IOSTREAM                
#define IOSTREAM
#endif
#include<cstring>
#include "Image_Processing.h"

using namespace std;

void Image::Read(){
	
	cout<<"Enter the name of the image:";
	getline(cin,img_pth);
    im0=cv::imread(img_pth,0);   //read the img in grayscale
}


void Image::Process(){
    Resize();
    Threshold();  //binarize(resulting values either 0 or 255 )
    Thinning();  //For Thinnning the white spaces inside the maze
    Clear_Borders(); //For clearing the borders of the image (not borders of the maze)
    Find_endpoints(); //For finding the possible points which maybe endpoints
    Find_endpxls();  //for finding the correct start and endpoints

}

void Image::Resize(){
     if(im0.rows > 640 && im0.cols > 480)
	    cv::resize(im0,im0,cv::Size(700,500),0,0,cv::INTER_AREA);     //Shrink the image to 640x480
}


void Image::Threshold(){
    cv::threshold(im0,bw,127,255,cv::THRESH_BINARY); 
}

void Image::Thinning(){
	const cv::Mat& src=bw;
	cv::Mat& dst=bw;

	dst = src.clone();   //copying the src Mat to dst Mat(it copies the values because of .clone())
	dst /= 255;         // convert to binary image  (All the 255 values will be 1 .Either 0 or 1 value in dst)

	cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);  
	cv::Mat diff;

	do {
		Thinning_iteration(dst, 0);
		Thinning_iteration(dst, 1);
		cv::absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (cv::countNonZero(diff) > 0);

	 dst *= 255;   //All the pixels of value 1 will be 255

}

void Image::Thinning_iteration(cv::Mat& img,int iter){

	/***Zhang Suen Algorithm For thinning the binary image***/
	 
    CV_Assert(img.channels() == 1);
	CV_Assert(img.depth() != sizeof(uchar));
	CV_Assert(img.rows > 3 && img.cols > 3);

	cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

	int nRows = img.rows;
	int nCols = img.cols;

	if (img.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	int x, y;
	uchar *pAbove;
	uchar *pCurr;
	uchar *pBelow;
	uchar *nw, *no, *ne;    // north (pixel Above)
	uchar *we, *me, *ea;    // center 
	uchar *sw, *so, *se;    // south (pixel Below)

	uchar *pDst;

	// initialize row pointers
	pAbove = NULL;
	pCurr = img.ptr<uchar>(0);    //ptr to the first row first pixel
	pBelow = img.ptr<uchar>(1);   //ptr to the second row first pixel

	for (y = 1; y < img.rows - 1; ++y) {    //iterating the matrix
		// shift the rows up by one   (scan from up to down)
		pAbove = pCurr;
		pCurr = pBelow;
		pBelow = img.ptr<uchar>(y + 1);

		pDst = marker.ptr<uchar>(y);

		// initialize col pointers
		no = &(pAbove[0]);
		ne = &(pAbove[1]);
		me = &(pCurr[0]);
		ea = &(pCurr[1]);
		so = &(pBelow[0]);
		se = &(pBelow[1]);

		for (x = 1; x < img.cols - 1; ++x) {
			// shift col pointers left by one (scan from left to right)
			nw = no;
			no = ne;
			ne = &(pAbove[x + 1]);
			we = me;
			me = ea;
			ea = &(pCurr[x + 1]);
			sw = so;
			so = se;
			se = &(pBelow[x + 1]);

			int A = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
				(*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
				(*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
				(*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);      //number of transitions from black to white
			int B = *no + *ne + *ea + *se + *so + *sw + *we + *nw;    //number of white pixels
			int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we); //atleast one pixel should be black
			int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);  

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
				pDst[x] = 1;      //We don't need these pixels
		}
	}

	img &= ~marker;    //no need the pixels in the marker so...(~)
}

void Image::Clear_Borders(){
    //clear the borders
	for (int r = 0; r < bw.rows; r++) {
		cv::Point P;
		P.x = 0;
		P.y = r;
		bw.at<uchar>(P) = 0;   
		P.x = bw.cols-1;
		P.y = r;
		bw.at<uchar>(P) = 0;
	}
	
	for (int c = 0; c < bw.cols; c++) {
		cv::Point P;
		P.x = c;
		P.y = 0;
		bw.at<uchar>(P) = 0;
		P.x = c;
		P.y = bw.rows-1;
		bw.at<uchar>(P) = 0;
	}
    
}

void Image::Find_endpoints(){
        cv::Mat img=bw;
        cv::Mat Filtered , endpImg;
		cv::Mat kernel = cv::Mat::zeros(cv::Size(3, 3), CV_8U);
		
		//kernel of filter for endpoints
		int kern_data[9] = { 1 , 1 , 1 , 1 , 10 , 1 , 1 , 1 , 1 };
		for (int i = 0; i < 9; i++) {
			kernel.data[i] = kern_data[i];
		}
		int src_depth = -1;
		cv::filter2D(img/255, Filtered, src_depth, kernel); //apply endpoints kernel to skeleton image
		endpImg = (Filtered == 11); //values of 11 are the endpoint pixels
		cv::findNonZero(endpImg, endP); //storing the endpoints in  endP vector

}

void Image::Find_endpxls(){

    
	bool flag = false; 
	for (int i = 0; i < endP.size(); i++) { 
		cv::Point P = endP[i];
		if (P.x == 1 || P.y == 1 || P.x==bw.cols-2 || P.y==bw.rows-2) { //if one of the endpoints is in the border of the image
			if (!flag) {
				startPxl = P;
				flag = true; 
			}
			else
				endPxl = P;
		}
	}

}

void Image::Visualize(){
	Video::Visualize(im0,Result);
}


void Video::Visualize(cv::Mat& im0,vector<cv::Point>& Result){
    cv::Mat video;
	std::vector<cv::Mat> im2(3);   //creating a three channels image
	im2[0] = im0;
	im2[1] = im0;
	im2[2] = im0;
	cv::merge(im2, video);

	
	for (int n = 0; n < Result.size(); n++) {
		video.at<cv::Vec3b>(Result[n])[0] = 0;
		video.at<cv::Vec3b>(Result[n])[1] = 0;   //BGR (B=0,G=0,R=255)  path color->red

		cv::namedWindow("MazeSolver");

		if(n==0){
			cv::setWindowProperty("MazeSolver",cv::WND_PROP_TOPMOST,1);

		}//for focussing the window on screen

		cv::imshow("MazeSolver", video);
		cv::waitKey(1);
	}
	cv::waitKey(0);
	cv::destroyAllWindows();
}