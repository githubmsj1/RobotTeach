#ifndef ORBIT_H
#define ORBIT_H
#include<string>
#include<opencv2/opencv.hpp>
#include "maptwod.h"
class Orbit
{
public:
    Orbit();
    int pushPoint(int x,int y,int scale);
    size_t popPoint(cv::Point3f &output, bool repop);
    int savePoints(std::string name);
    int loadPoints(std::string name);
    int pointsMap(MapTwoD fmap);
    size_t getSize();
private:
    std::vector<cv::Point3f> imgPoints;
    std::vector<cv::Point3f> robotPoints;
};

#endif // ORBIT_H
