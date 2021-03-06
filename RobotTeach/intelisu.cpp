
/*
 * l: load model
 * s: save model
 * i: save s1
 * o: save s2
 * p: save s3
 * j: save initial hand size
 * ]: calculate acoef
 * k: save the inital object size and calculate kSize
 * ,: record points
 * .: save points
 * /: send points
*/
#include <opencv2/opencv.hpp>
#include <tld_utils.h>
#include <iostream>
#include <sstream>
#include <TLD.h>
#include <stdio.h>
#include "serial.h"
#include "gaussian.h"
#include "maptwod.h"
#include "orbit.h"

using namespace cv;
using namespace std;

//#define SERIAL_PORT

//Global variables
Rect box;
bool drawing_box = false;
bool gotBB = false;
bool tl = true;//track or not
bool rep = false;
bool fromfile=false;
string video;
bool handCalibrate=false;
bool trackRecord=false;
bool giveCon=false;

enum mode{learnMode,recognizeMode};
enum targetAquireMode{motionMode,mouseMode};
mode tldMode=learnMode;
targetAquireMode roiMode=mouseMode;
//read the imformation of initial box
void readBB(char* file){
  ifstream bb_file (file);
  string line;
  getline(bb_file,line);
  istringstream linestream(line);
  string x1,y1,x2,y2;
  getline (linestream,x1, ',');
  getline (linestream,y1, ',');
  getline (linestream,x2, ',');
  getline (linestream,y2, ',');
  int x = atoi(x1.c_str());// = (int)file["bb_x"];
  int y = atoi(y1.c_str());// = (int)file["bb_y"];
  int w = atoi(x2.c_str())-x;// = (int)file["bb_w"];
  int h = atoi(y2.c_str())-y;// = (int)file["bb_h"];
  box = Rect(x,y,w,h);
}

//bounding box mouse callback
void mouseHandler(int event, int x, int y, int flags, void *param){
  switch( event ){
  case CV_EVENT_MOUSEMOVE:
    if (drawing_box){
        box.width = x-box.x;
        box.height = y-box.y;
    }
    break;
  case CV_EVENT_LBUTTONDOWN:
    drawing_box = true;
    box = Rect( x, y, 0, 0 );
    break;
  case CV_EVENT_LBUTTONUP:
    drawing_box = false;
    if( box.width < 0 ){
        box.x += box.width;
        box.width *= -1;
    }
    if( box.height < 0 ){
        box.y += box.height;
        box.height *= -1;
    }
    gotBB = true;
    break;
  }
}
void sampleHandler(int event, int x, int y, int flags, void *param){

}

int main(int argc, char * argv[])
{

    string modelName;
    if(argc>2)
    {
        modelName=argv[2];
        if(modelName=="hand")
        {
            handCalibrate=true;
        }
    }
    else
    {
        modelName="";
    }
    cout<<"model: "<<modelName<<endl;
    VideoCapture capture;
    if(argc>1)
    {
        capture.open(argv[1][0]-'0');
    }
    else
    {
        capture.open(0);
    }

    FileStorage fs;
    fs.open("../data/parameters.yml", FileStorage::READ);

    if(!capture.isOpened())
    {
        cout<<"Cam Invalid"<<endl;
        return -1;
    }


    //serial initialize
    #ifdef SERIAL_PORT
    Serial serial;
    unsigned char sendBuff[10];
    if(serial.init()!=-1)
    {
        cout<<"serial init success"<<endl;
    }
    else
    {
        cout<<"serial init failed,please check the interface"<<endl;
        return -1;
    }
    #endif

    //set capture size
    capture.set(CV_CAP_PROP_FRAME_WIDTH,340);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);

    namedWindow("Intelisu",CV_WINDOW_NORMAL);//CV_WINDOW_AUTOSIZE
    namedWindow("Examples",WINDOW_NORMAL);
    setMouseCallback( "Intelisu", mouseHandler, NULL );

    TLD tld;
    tld.read(fs.getFirstTopLevelNode());
    MapTwoD mptd;
    mptd.loadPara();
    Orbit orb;

    Mat frame;
    Mat last_gray;
    Mat first;
    capture>>frame;//test image acquirement;
    cout<<frame.size()<<endl;
    //aquire the object patch
//    while(!gotBB)
//    {
//        if (!fromfile){
//            capture >> frame;
//        }
//        else
//            first.copyTo(frame);
//        cvtColor(frame, last_gray, CV_RGB2GRAY);
//        drawBox(frame,box);
//        putText(frame,"Wating for the object...",Point(20,20),FONT_HERSHEY_SIMPLEX,0.6,10,2);
//        //Mat scaleImage;
//        //resize(frame,scaleImage,Size(2*round(frame.cols),2*round(frame.rows)));
//        imshow("Intelisu", frame);
//        if (cvWaitKey(33) == 'q')
//            return 0;
//    }

    //motion track
    myupdate_mhi myupdate1;
    Rect result;
    Mat motion=Mat::zeros(frame.rows,frame.cols, CV_8UC1);

    //load existing model
    if((char)waitKey(-1)=='l')
    {
        tld.loadModel(modelName);
        tld.initModel();

        tldMode=recognizeMode;
        //waitKey(-1);
        //return 0;
    }

    if(tldMode!=recognizeMode)
    {
        if(roiMode==motionMode)
        {

            for(;;)
            {
                    capture>>frame;
                    IplImage pframe(frame);
                    IplImage pmotion(motion);

                    myupdate1.init( &pframe, &pmotion, 60 );
                    if(myupdate1.update_mhi(result)==0)
                    {
                        cout<<"x:"<<result.x+result.width/2<<"    "<<"y:"<<result.y+result.height/2<<endl;
                        rectangle(frame,result, Scalar(255,0,0));
                        break;
                    }
                    //resize(frame,frame,Size(3*round(frame.cols),3*round(frame.rows)));

                    static unsigned char safeCount=0;
                    putText(frame,"Environment Safe",Point(20,20),FONT_HERSHEY_SIMPLEX,0.7,CV_RGB(0,safeCount,0),2);
                    safeCount+=16;

                    imshow("Intelisu",frame);
                    waitKey(10);

            }
        }
        else
        {
            cvSetMouseCallback( "Intelisu", mouseHandler, NULL );
            for(;;)
            {
                while(!gotBB)
                {
                  if (!fromfile){
                    capture >> frame;
                  }
                  else
                    first.copyTo(frame);
                  //cvtColor(frame, last_gray, CV_RGB2GRAY);
                  drawBox(frame,box);
                  imshow("Intelisu", frame);
                  if (cvWaitKey(33) == 'q')
                      return 0;
                }

                if (min(box.width,box.height)<15){
                    cout << "Bounding box too small, try again." << endl;
                    gotBB = false;
                }
                else
                {
                    result=box;
                    break;

                }
            }
            cvSetMouseCallback( "Intelisu", NULL, NULL );
        }
    }

    cvtColor(frame, last_gray, CV_RGB2GRAY);
    //waitKey(-1);

//    //Remove callback
//    cvSetMouseCallback( "Intelisu", NULL, NULL );

    //open the output file
    FILE  *bb_file = fopen("bounding_boxes.txt","w");


    //TLD initialization
    //tld.init(last_gray,box,bb_file);
    if(tldMode==learnMode)
    {

        tld.init(last_gray,result,bb_file);

    }else
    {
        tld.initModel();
    }
    //runtime variable
    Mat current_gray;
    BoundingBox pbox;
    vector<Point2f> pts1;
    vector<Point2f> pts2;
    bool status=true;//if model is loaded,detect default
    int frames = 1;
    int detections = 1;

    if(tldMode==recognizeMode)
    {
        status=false;
    }
    while(capture.read(frame)){

            //<<<<<<<<<<<<<<
            double timeMs=0;
            timeMs=getTickCount();


        //get frame
        cvtColor(frame, current_gray, CV_RGB2GRAY);
        //Process Frame
        tld.processFrame(last_gray,current_gray,pts1,pts2,pbox,status,tl,bb_file);
        //Draw Points
        if (status){
            drawPoints(frame,pts1);
            drawPoints(frame,pts2,Scalar(0,255,0));
            drawBox(frame,pbox);
            detections++;

            //drawWarning
            static unsigned char count=0;
            Rect symbol(Point(0,0),Size(frame.cols,frame.rows));
            rectangle(frame,symbol.tl(),symbol.br(),Scalar(0,0,count),15);
            putText(frame,"HIT",Point(90,120),FONT_HERSHEY_SIMPLEX,1.2,CV_RGB(count,0,0),2);
            count+=16;

            //attach coordinate
            ostringstream printText;
            Rect detdObj;
            detdObj.x=(pbox.x+pbox.width/2)/2;
            detdObj.y=(pbox.y+pbox.height/2)/2;
            printText<<"X:"<<detdObj.x<<"  "<<"Y:"<<detdObj.y<<"  "<<"Area:"<<pbox.width*pbox.height;
            putText(frame,printText.str(),Point(20,40),FONT_HERSHEY_SIMPLEX,0.7,CV_RGB(100,255,0),2);



            //record the orbit data
            if(trackRecord==true)
            {
                orb.pushPoint(pbox.y,pbox.x,pbox.width);
                cout<<">>>>>>>>>>>>"<<orb.getSize()<<"orbit recorded"<<endl;
            }


            if(giveCon==false)
            {
                Point3f tmpIn;
                tmpIn.x=pbox.y;
                tmpIn.y=pbox.x;
                tmpIn.z=pbox.width;
                Point3f tmpOut;
                mptd.map(tmpIn,tmpOut);


                //send data through serial port
                #ifdef SERIAL_PORT
                //static int sendRate=0;
                //if((sendRate++)%8==0)
                //{
                static float initMs=getTickCount();
                //static float timeFlow=0;
                if(((getTickCount()-initMs)/getTickFrequency())>0.5)
                {
                    cout<<"##########################################"<<endl;

                    cout<<"-        "<<1<<"       "<<tmpOut<<endl;

                    cout<<"##########################################"<<endl;

                    sendBuff[0]=16;//head1
                    sendBuff[1]=1;//head2
                    sendBuff[2]=(char)((tmpOut.x+300)/4);//x
                    sendBuff[3]=(char)(tmpOut.y/4);//y
                    sendBuff[4]=(char)(tmpOut.z/4);//z
                    sendBuff[5]=16;//tail1
                    sendBuff[6]=2;//tail2

                    serial.send_data_tty(sendBuff,7);
                    initMs=getTickCount();

                }
                //}
                #endif

            }

        }
        else
        {
            //drawSafe
            static unsigned char safeCount=0;
            putText(frame,"NOTHING",Point(20,20),FONT_HERSHEY_SIMPLEX,0.7,CV_RGB(0,safeCount,0),2);
            safeCount+=16;
        }

        //send the data to UR
        if(giveCon==true)
        {
            static float initMs=getTickCount();
            if(((getTickCount()-initMs)/getTickFrequency())>0.5)
            {
                Point3f tmp;
                size_t index=orb.popPoint(tmp);
                if(index!=-1)
                {
                cout<<"##########################################"<<endl;

                cout<<"-        "<<orb.getIndex()<<"       "<<tmp<<endl;

                cout<<"##########################################"<<endl;


                //send data through serial port
                #ifdef SERIAL_PORT
                //static int sendRate=0;
                //if((sendRate++)%8==0)
                //{
                    sendBuff[0]=16;//head1
                    sendBuff[1]=1;//head2
                    sendBuff[2]=(char)((tmp.x+300)/4);//x
                    sendBuff[3]=(char)(tmp.y/4);//y
                    sendBuff[4]=(char)(tmp.z/4);//z
                    sendBuff[5]=16;//tail1
                    sendBuff[6]=2;//tail2

                    serial.send_data_tty(sendBuff,7);
                //}


                initMs=getTickCount();
                #endif
                }
            }

        }






            //<<<<<<<<<<<<<<
            //double timeMs=0;
            //timeMs=getTickCount();
        //resize(frame,frame,Size(3*round(frame.cols),3*round(frame.rows)));
            //timeMs=(getTickCount()-timeMs)/getTickFrequency();
            //cout<<"<<<<<<<<<< "<<"Time: "<<timeMs*1000<<" >>>>>>>>>>"<<endl;
            //>>>>>>>>>>>>>>
        imshow("Intelisu", frame);

        //Super Display
//        Mat scaleImage;
//        resize(frame,scaleImage,Size(2*round(frame.cols),2*round(frame.rows)));
//        ostringstream printText;
//        printText<<"X:"<<pbox.x+pbox.width/2<<"   "<<"Y:"<<pbox.y+pbox.height/2;
//        putText(scaleImage,printText.str(),Point(40,40),FONT_HERSHEY_SIMPLEX,1,CV_RGB(100,255,0),2);
//        imshow("Intelisu", scaleImage);

        //swap points and images
        swap(last_gray,current_gray);

        //clean the track
        pts1.clear();//clean the
        pts2.clear();
        frames++;
        //printf("Detection rate: %d/%d\n",detections,frames);


        char keyValue=(char)waitKey(1);
        if (keyValue== 'q')
          break;
        else if(keyValue=='s')
        {
            cout<<"save......"<<endl;
            tld.saveModel(modelName);
            cout<<"done......"<<endl;
            break;
        }
        else if(keyValue==',')
        {
            trackRecord=!trackRecord;
        }
        else if(keyValue=='.')
        {
            orb.pointsMap(mptd);
            orb.savePoints("block");
            trackRecord=false;
        }
        else if(keyValue=='/')
        {
            orb.loadPoints("block");
            giveCon=!giveCon;
        }
        else {
            if(handCalibrate==true)//calibrate the handsize map to the z-axis
            {
                if(keyValue=='i')//record the hand size of first panel
                {
                    mptd.setS(pbox.width,1);
                    mptd.savePara();
                    cout<<"save s1..."<<endl;
                }
                if(keyValue=='o')//record the hand size of second panel
                {
                    mptd.setS(pbox.width,2);
                    mptd.savePara();
                    cout<<"save s2..."<<endl;
                }
                if(keyValue=='p')//record the hand size of third panel
                {
                    mptd.setS(pbox.width,3);
                    mptd.savePara();
                    cout<<"save s3..."<<endl;
                }
                if(keyValue=='[')//calculate Acoef
                {
                    mptd.calAcoef();
                    mptd.savePara();
                    cout<<"save Acoef"<<endl;
                }

                if(keyValue=='j')//record the inital handsize
                {
                    mptd.pushHandSize(pbox.width);
                    mptd.savePara();
                    cout<<"save s0..."<<endl;
                }

            }
            if(keyValue=='k')//record the inital object size and calculate the ksize
            {
                mptd.pushObjectSize(pbox.width);
                mptd.calkSize();
                mptd.savePara();
                cout<<"save obj0..."<<endl;
            }
        }

        timeMs=(getTickCount()-timeMs)/getTickFrequency();
        cout<<"<<<<<<<<<< "<<"Time: "<<timeMs*1000<<" >>>>>>>>>>"<<endl;

        //record the ordit or not

//        static size_t runingtime=0;
//        runingtime++;
//        cout<<"count:"<<runingtime<<endl;
//        if(runingtime==200)
//        {
//            cout<<"save......"<<endl;
//            tld.saveModel();
//            break;
//        }


  }
    fclose(bb_file);
    return 0;
}
