#include<iostream>
#include<opencv2\opencv.hpp>
#include "Image_Processing_def.h"
#include "Graph_def.h"

using namespace std;
int main(){

    Graphs* G;
    Image* I;

    I=new Image;
    G=new Graphs;

    I->Read();
    I->Process();
    G->BFSShortPath(I);
    I->Visualize();

    delete I;
    delete G;

    return 0;
}