#ifndef MAPTWOD_H
#define MAPTWOD_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#define FILEPATH "/home/jes/Desktop/project/calibrate/build-untitled-Desktop_Qt_5_5_1_GCC_32bit-Debug/eyetohand.yml"
using namespace cv;
using namespace std;

class MapTwoD
{
public:
    MapTwoD();
    int loadPara();
    int savePara();
    int map(Point3f src,Point3f dst);
    int calH();
    int calKcoef();
    int pushImgPoints(vector<Point2f>points);

    int pushHandSize(int objSize);
    int pushObjectSize(int handSize);
    int calAcoef();
    int calkSize();

    int setS(int s,int index);

    int numData();
    int print();

private:
    Mat H1,H2,H3;
    vector<Point2f> objPoints;
    vector<vector<Point2f> > imgPoints;

    int s1,s2,s3;
    float aCoef;
    vector<int> handSizes;

    vector<Mat> HVec;
    float z1,z2,z3;
    float z0;
    Mat kCoef;
    int numImg,numPoint;
    int handSize0;
    int objSize0;

    float kSize;
//    Size handSize0;
//    Size objSize0;
//    float kwidth;
//    float kheight;


};

#endif // MAPTWOD_H
