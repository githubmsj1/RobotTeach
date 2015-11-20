#include "maptwod.h"

MapTwoD::MapTwoD()
{

    objPoints.push_back(Point2f(0,0));
    objPoints.push_back(Point2f(0,100));
    objPoints.push_back(Point2f(100,0));
    objPoints.push_back(Point2f(100,100));

    z1=0;
    z2=1;
    z3=2;

    //loadPara();

}

int MapTwoD::numData()
{
    return imgPoints.size();
}

int MapTwoD::calH()
{
    Mat H=findHomography(objPoints,imgPoints[imgPoints.size()-1],0);
    H.convertTo(H,CV_32F);
    HVec.push_back(H);
    return 0;
}

int MapTwoD::pushImgPoints(vector<Point2f>points)
{
    imgPoints.resize(imgPoints.size()+1);
    imgPoints[imgPoints.size()-1].assign(points.begin(),points.end());
    for(int i=0;i<imgPoints[imgPoints.size()-1].size();i++)
    {
        cout<<imgPoints[imgPoints.size()-1][i]<<endl;
    }
    return 0;

}

int MapTwoD::calKcoef()
{
//    if(HVec.size()<3)
//    {
//        cerr<<"H matrix is not enough"<<endl;
//        return -1;
//    }
//    H1=HVec[0];
//    H2=HVec[1];
//    H3=HVec[2];
    Mat a1=(H2-H1)/(z2-z1);
    Mat a2=(H3-H2)/(z3-z2);
    kCoef=(a1+a2)/2;

    return 0;
}


int MapTwoD::calAcoef()
{
//    if(handSizes.size()<3)
//    {
//        cerr<<"handsize is less than 3"<<endl;
//        return 0;
//    }
//    s1=handSizes[0];
//    s2=handSizes[1];
//    s3=handSizes[2];

    float a1=(z2-z1)/(float)(s2-s1);
    float a2=(z3-z2)/(float)(s3-s2);
    aCoef=(a1+a2)/2;
    return 0;
}

int MapTwoD::map(Point3f src,Point3f &dst)
{
    float mapHandSize=src.z*kSize;
    float zRobot=aCoef*(mapHandSize-s1)+z1;
    Mat Hn=kCoef*(zRobot-z1)+H1;

    Mat sImg=Mat::zeros(3,1,CV_32F);
    sImg.at<float>(0,0)=src.x;
    sImg.at<float>(1,0)=src.y;
    sImg.at<float>(2,0)=1;

    Mat tmp=Hn*sImg;
    dst.x=tmp.at<float>(0,0)/tmp.at<float>(0,2);
    dst.y=tmp.at<float>(0,1)/tmp.at<float>(0,2);
    dst.z=zRobot;

    return 0;
}

int MapTwoD::savePara()
{
    FileStorage fs;
    fs.open(FILEPATH, FileStorage::WRITE);
    fs<<"H1"<<H1;
    fs<<"H2"<<H2;
    fs<<"H3"<<H3;
    fs<<"kCoef"<<kCoef;
    fs<<"aCoef"<<aCoef;
    fs<<"z1"<<z1;
    fs<<"z2"<<z2;
    fs<<"z3"<<z3;

    fs<<"s1"<<s1;
    fs<<"s2"<<s2;
    fs<<"s3"<<s3;

    fs<<"kSize"<<kSize;
    fs<<"objSize0"<<objSize0;
    fs<<"handSize0"<<handSize0;


    fs.release();

    return 0;
}

int MapTwoD::loadPara()
{
    FileStorage fs;


    fs.open(FILEPATH, FileStorage::READ);

    if(fs.isOpened()==false)
    {
        fs.release();
        return -1;
    }

    fs["kCoef"]>>kCoef;

    fs["aCoef"]>>aCoef;
    fs["z1"]>>z1;
    fs["z2"]>>z2;
    fs["z3"]>>z3;
    fs["s1"]>>s1;
    fs["s2"]>>s2;
    fs["s3"]>>s3;
    fs["H1"]>>H1;
    fs["H2"]>>H2;
    fs["H3"]>>H3;

    fs["kSize"]>>kSize;
    fs["objSize0"]>>objSize0;
    fs["handSize0"]>>handSize0;


    fs.release();

    return 0;
}
int MapTwoD::print()
{
    cout<<" z1: "<<z1<<" z2: "<<z2<<" z3: "<<z3<<endl;
    cout<<" s1: "<<z1<<" s2: "<<z2<<" s3: "<<s3<<endl;
    cout<<"H1"<<H1<<endl;
    cout<<"kCoef"<<kCoef<<endl;
    cout<<"aCoef"<<aCoef<<endl;

    return 0;

}

int MapTwoD::pushObjectSize(int ObjSize)
{
    objSize0=ObjSize;
    return 0;
}

int MapTwoD::pushHandSize(int handSize)
{
    handSize0=handSize;
    return 0;
}

int MapTwoD::calkSize()
{
    kSize=((float)handSize0)/((float)objSize0);
}

int MapTwoD::setS(int s,int index)
{

    if(index==1)
    {
        s1=s;
    }
    if(index==2)
    {
        s2=s;
    }
    if(index==3)
    {
        s3=s;
    }
    return 0;
}
