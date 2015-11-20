#include "orbit.h"

using namespace cv;
using namespace std;

Orbit::Orbit()
{
    outCount=0;
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
size_t Orbit::popPoint(cv::Point3f &output,bool repop)
{
    static size_t popCount=0;
    if(repop==true)
    {
        popCount=0;
        outCount=0;
    }
    if(popCount<robotPoints.size())
    {
        output=robotPoints[popCount];
        popCount++;outCount++;
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
        fs<<"{";
        fs<<"x"<<robotPoints[i].x;
        fs<<"y"<<robotPoints[i].y;
        fs<<"z"<<robotPoints[i].z;
        fs<<"}";

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

    for(size_t i=0;it!=orbitNode.end();i++,it++)
    {

        Point3f tmp;
        (*it)["x"]>>tmp.x;
        (*it)["y"]>>tmp.y;
        (*it)["z"]>>tmp.z;
        cout<<tmp<<endl;
        robotPoints.push_back(tmp);

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
size_t Orbit::getSize()
{
    return imgPoints.size();
}
size_t Orbit::getIndex()
{
    return outCount;
}
