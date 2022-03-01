#pragma once
#include<iostream>
#include<set>
#include<iterator>
#include<list>
#include<string>
#include<queue>
#include<utility>
#include<limits>
#include "Graph.h"
#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H
#endif

using namespace std;

void Graphs::BFSShortPath(Image*& I){
	    
        cv::Mat bw = I->bw;
		cv::Point Start=I->startPxl;
		cv::Point End=I->endPxl;
		vector<cv::Point>&Result=I->Result;

		if (bw.at<uchar>(Start) != 255 || bw.at<uchar>(End) != 255) { //error if start or end point is not proper
			std::cerr << "ERROR: Start or End Point Is not Located on the Path..." << std::endl;
			std::getchar();
			std::exit(0);
		}

		// extract the white points as the graph nodes
		cv::findNonZero(bw, nodes);

		// save index of each white point in the vector on the corresponding pixel in other Image
		cv::Mat Index_of_nodes = cv::Mat::zeros(bw.size(), CV_32S); //save index of each node in the corresponding pixel
		for (int pidx = 0; pidx < nodes.size(); pidx++) {
			Index_of_nodes.at<int>(nodes[pidx]) = pidx;
		}

		//now check the neighborhood of each node and save the edges found
		 adjacency_list.resize(nodes.size());

		for (int n = 0; n < nodes.size(); n++) {
			cv::Point node = nodes[n];

			for (int Nx = -1; Nx < 2; Nx++) {
				for (int Ny = -1; Ny < 2; Ny++) {
					if (Ny != 0 || Nx != 0) { //do not check the node itself

						//define the neighbor pixel
						cv::Point neighborP;
						neighborP.x = node.x + Nx;
						neighborP.y = node.y + Ny;

						cv::Rect rect(cv::Point(), bw.size());
						if (rect.contains(neighborP)) { //if the point is inside the image

							if (bw.at<uchar>(neighborP) == 255) { //if neighbor is a white pixel
								int index = Index_of_nodes.at<int>(neighborP); //return the name of the node for this pixel
								adjacency_list[n].push_back(index); //write the name  of the node in the list
							}
						}
					}
				}
			}
		}


		// Define the Start and End Points of the Node
		vertex_t StartNode = Index_of_nodes.at<int>(Start);
		vertex_t EndNode = Index_of_nodes.at<int>(End);

        src=StartNode;
        end=EndNode;

		// Do BFS SINLGE SOURCE SHORTEST PATH Short Path Algorithm
	
		BFS_SSSP_ComputePaths();
		BFSGetShortestPathTo(end);

		// Reconstruct the Image Pixels from the Path nodes
		for (int pn = 0; pn < path.size(); pn++) { //for each element of the path list
			int pnt = path[pn]; //node number in the path
			cv::Point pixel = nodes[pnt]; //corresponding pixel to the node
			Result.push_back(pixel);

		}

}

void Graphs::BFS_SSSP_ComputePaths(){

    int n = adjacency_list.size();
	min_distance.clear();
	min_distance.resize(n, INT_MAX);   //All values set to INT_MAX
	min_distance[src] = 0;
	previous.clear();
	previous.resize(n, -1);           //All values set to -1
    std::queue<vertex_t> vertex_queue;

	vertex_queue.push(src);

	while(!vertex_queue.empty()){

		vertex_t node=vertex_queue.front();
		vertex_queue.pop();
		vector<neighbor> neighbors_of_node=adjacency_list[node];

		for(auto nbr:neighbors_of_node){
			if(min_distance[nbr]==INT_MAX){   //if the mind_distance is INT_MAX then it is a unvisited vertex
				
				vertex_queue.push(nbr);
				previous[nbr]=node;
				min_distance[nbr]=min_distance[node]+1;
			}
		}

	}
}


void Graphs::BFSGetShortestPathTo(vertex_t vertex){
    
	vertex=previous[vertex];
	
	while(vertex!=-1){
		path.push_back(vertex);
		vertex=previous[vertex];
	}

	std::reverse(path.begin(), path.end());  //we've found the path from the end ..so reverse that

}
