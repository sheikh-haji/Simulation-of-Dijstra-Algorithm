#pragma once
#include<vector>
#include<opencv2\opencv.hpp>
#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H
#endif
using namespace std;

typedef int vertex_t;
typedef double dist_t;
typedef vertex_t neighbor;
typedef vector<vector<neighbor> > adjacency_list_t;

class Graphs{
    
    vector<cv::Point> nodes;
    vertex_t src;                         //starting vertex
    vertex_t end;                        //finishing vertex
    vector<dist_t> min_distance;         //For storing the min_distance 
    vector<vertex_t> previous;           //For Finding the name of the nodes in the path
    adjacency_list_t adjacency_list;
    vector<vertex_t> path;               //For storing the final path

    public:

    void BFSShortPath(Image*&);
    void BFS_SSSP_ComputePaths();
    void BFSGetShortestPathTo(vertex_t);
};