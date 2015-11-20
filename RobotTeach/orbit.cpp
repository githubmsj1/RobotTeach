#include "orbit.h"

using namespace cv;
using namespace std;

Orbit::Orbit()
{

}


int Orbit::pushPoint(int x,int y,int scale)
{
    if(imgPoints.empty()!=true)
    {
        unsigned int delx=abs(x-(int)imgPoints[imgPoints.size()-1].x);
        unsigned int dely=abs(y-(int)imgPoints[imgPoints.size()-1].y);
        unsigned int delz=abs(scale-(int)imgPoints[imgPoints.size()-1].z);
        if((delx+dely+delz)<10)
            return -1;

    }
    Point3f tmp;
    tmp.x=x;
    tmp.y=y;
    tmp.z=scale;
    imgPoints.push_back(tmp);
    return 0;
}
size_t Orbit::popPoint(cv::Point3f &output)
{
    static size_t popCount=0;
    if(popCount<robotPoints.size())
    {
        output=robotPoints[popCount];
        popCount++;
        return popCount-1;
    }
    else
        return -1;
}
int Orbit::savePoints(std::string name)
{
    FileStorage fs;

    string fileName=name+"orbit.yml";
    fs.open(fileName, FileStorage::WRITE);

    if(fs.isOpened()==false)
    {
        fs.release();
        return -1;
    }

    fs<<"orbit";
    fs<<"[";
    for(size_t i=0;i<robotPoints.size();i++)
    {
        fs<<"x"<<robotPoints[i].x;
        fs<<"y"<<robotPoints[i].y;
        fs<<"z"<<robotPoints[i].z;

    }
    fs<<"]";


    fs.release();
    return 0;

}
int Orbit::loadPoints(std::string name)
{
    FileStorage fs;

    string fileName=name+"orbit.yml";
    fs.open(fileName, FileStorage::READ);

    if(fs.isOpened()==false)
    {
        fs.release();
        return -1;
    }

    FileNode orbitNode=fs["orbit"];
    FileNodeIterator it=orbitNode.begin();

    for(size_t i=0;i<robotPoints.size();i++,it++)
    {

        robotPoints[i].x=(*it)["x"];
        robotPoints[i].y=(*it)["y"];
        robotPoints[i].z=(*it)["z"];

    }



    fs.release();
    return 0;
}
int Orbit::pointsMap(MapTwoD fmap)
{
    for(size_t i=0;i<imgPoints.size();i++)
    {
        Point3f tmp;
        fmap.map(imgPoints[i],tmp);
        robotPoints.push_back(tmp);
    }
    return 0;
}
