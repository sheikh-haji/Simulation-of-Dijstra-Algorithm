#pragma once
#include<opencv2/opencv.hpp>
#include<vector>
using namespace std;


class Image{
    string img_pth;   //For storing the path of the image

    void Resize();
    void Threshold();
    void Thinning();
    void Thinning_iteration(cv::Mat&,int);
    void Clear_Borders();
    void Find_endpoints();
    void Find_endpxls();
    

    public:
    
    cv::Mat im0;     //For reading the image in the grayscale
    cv::Mat bw;      //For storing the pure black and white image(either white or black in each pixel)
    vector<cv::Point> endP; //For storing the end_points
    cv::Point startPxl,endPxl;  //start and end pixels
    vector<cv::Point> Result;


    void Read();
    void Process();
    void Visualize();
};

class Video{
    public:

    static void Visualize(cv::Mat&,vector<cv::Point>&);
};
