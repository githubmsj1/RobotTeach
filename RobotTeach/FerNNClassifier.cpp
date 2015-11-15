/*
 * FerNNClassifier.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: alantrrs
 */

#include <FerNNClassifier.h>

using namespace cv;
using namespace std;

void FerNNClassifier::read(const FileNode& file){
  ///Classifier Parameters
  valid = (float)file["valid"];
  ncc_thesame = (float)file["ncc_thesame"];
  nstructs = (int)file["num_trees"];
  structSize = (int)file["num_features"];
  thr_fern = (float)file["thr_fern"];
  thr_nn = (float)file["thr_nn"];
  thr_nn_valid = (float)file["thr_nn_valid"];
}

void FerNNClassifier::prepare(const vector<Size>& scales){
  acum = 0;
  //Initialize test locations for features
  int totalFeatures = nstructs*structSize;
  features = vector<vector<Feature> >(scales.size(),vector<Feature> (totalFeatures));
  RNG& rng = theRNG();
  float x1f,x2f,y1f,y2f;
  int x1, x2, y1, y2;
  for (int i=0;i<totalFeatures;i++){
      x1f = (float)rng;
      y1f = (float)rng;
      x2f = (float)rng;
      y2f = (float)rng;
      for (int s=0;s<scales.size();s++){
          x1 = x1f * scales[s].width;
          y1 = y1f * scales[s].height;
          x2 = x2f * scales[s].width;
          y2 = y2f * scales[s].height;
          features[s][i] = Feature(x1, y1, x2, y2);
      }

  }
  //Thresholds
  thrN = 0.5*nstructs;

  //Initialize Posteriors
  for (int i = 0; i<nstructs; i++) {
      posteriors.push_back(vector<float>(pow(2.0,structSize), 0));
      pCounter.push_back(vector<int>(pow(2.0,structSize), 0));
      nCounter.push_back(vector<int>(pow(2.0,structSize), 0));
  }
}

void FerNNClassifier::getFeatures(const cv::Mat& image,const int& scale_idx, vector<int>& fern){
  int leaf;
  for (int t=0;t<nstructs;t++){
      leaf=0;
      for (int f=0; f<structSize; f++){
          leaf = (leaf << 1) + features[scale_idx][t*nstructs+f](image);
      }
      fern[t]=leaf;
  }
}

float FerNNClassifier::measure_forest(vector<int> fern) {
  float votes = 0;
  for (int i = 0; i < nstructs; i++) {
      votes += posteriors[i][fern[i]];
  }
  return votes;
}

void FerNNClassifier::update(const vector<int>& fern, int C, int N) {
  int idx;
  for (int i = 0; i < nstructs; i++) {

      idx = fern[i];
      (C==1) ? pCounter[i][idx] += N : nCounter[i][idx] += N;


      if (pCounter[i][idx]==0) {

          posteriors[i][idx] = 0;

      } else {
          //cout<<"node2<<__"<<(pCounter[i][idx] + nCounter[i][idx])<<endl;
          posteriors[i][idx] = ((float)(pCounter[i][idx]))/(pCounter[i][idx] + nCounter[i][idx]);
          //cout<<"node2>>"<<endl;
      }

  }
}

void FerNNClassifier::trainF(const vector<std::pair<vector<int>,int> >& ferns,int resample){
  // Conf = function(2,X,Y,Margin,Bootstrap,Idx)
  //                 0 1 2 3      4         5
  //  double *X     = mxGetPr(prhs[1]); -> ferns[i].first
  //  int numX      = mxGetN(prhs[1]);  -> ferns.size()
  //  double *Y     = mxGetPr(prhs[2]); ->ferns[i].second
  //  double thrP   = *mxGetPr(prhs[3]) * nTREES; ->threshold*nstructs
  //  int bootstrap = (int) *mxGetPr(prhs[4]); ->resample
  thrP = thr_fern*nstructs;                                                          // int step = numX / 10;
  //for (int j = 0; j < resample; j++) {                      // for (int j = 0; j < bootstrap; j++) {
      for (int i = 0; i < ferns.size(); i++){               //   for (int i = 0; i < step; i++) {
                                                            //     for (int k = 0; k < 10; k++) {
                                                            //       int I = k*step + i;//box index
                                                            //       double *x = X+nTREES*I; //tree index
          if(ferns[i].second==1){                           //       if (Y[I] == 1) {
              if(measure_forest(ferns[i].first)<=thrP)      //         if (measure_forest(x) <= thrP)
                  //cout<<"node1>>"<<endl;
                  update(ferns[i].first,1,1);                 //             update(x,1,1);
                  //cout<<"node1<<"<<endl;
          }else{                                            //        }else{
              if (measure_forest(ferns[i].first) >= thrN)   //         if (measure_forest(x) >= thrN)
                update(ferns[i].first,0,1);                 //             update(x,0,1);
          }
      }
  //}
}

void FerNNClassifier::trainNN(const vector<cv::Mat>& nn_examples){
  float conf,dummy;
  vector<int> y(nn_examples.size(),0);
  y[0]=1;
  vector<int> isin;
  for (int i=0;i<nn_examples.size();i++){                          //  For each example
      NNConf(nn_examples[i],isin,conf,dummy);                      //  Measure Relative similarity
      if (y[i]==1 && conf<=thr_nn){                                //    if y(i) == 1 && conf1 <= tld.model.thr_nn % 0.65
          if (isin[1]<0){                                          //      if isnan(isin(2))
              pEx = vector<Mat>(1,nn_examples[i]);                 //        tld.pex = x(:,i);
              continue;                                            //        continue;
          }                                                        //      end
          //pEx.insert(pEx.begin()+isin[1],nn_examples[i]);        //      tld.pex = [tld.pex(:,1:isin(2)) x(:,i) tld.pex(:,isin(2)+1:end)]; % add to model
          pEx.push_back(nn_examples[i]);
      }                                                            //    end
      if(y[i]==0 && conf>0.5)                                      //  if y(i) == 0 && conf1 > 0.5
        nEx.push_back(nn_examples[i]);                             //    tld.nex = [tld.nex x(:,i)];

  }                                                                 //  end
  acum++;
  printf("%d. Trained NN examples: %d positive %d negative\n",acum,(int)pEx.size(),(int)nEx.size());
}                                                                  //  end


void FerNNClassifier::NNConf(const Mat& example, vector<int>& isin,float& rsconf,float& csconf){
  /*Inputs:
   * -NN Patch
   * Outputs:
   * -Relative Similarity (rsconf), Conservative Similarity (csconf), In pos. set|Id pos set|In neg. set (isin)
   */
  isin=vector<int>(3,-1);
  if (pEx.empty()){ //if isempty(tld.pex) % IF positive examples in the model are not defined THEN everything is negative
      rsconf = 0; //    conf1 = zeros(1,size(x,2));
      csconf=0;
      return;
  }
  if (nEx.empty()){ //if isempty(tld.nex) % IF negative examples in the model are not defined THEN everything is positive
      rsconf = 1;   //    conf1 = ones(1,size(x,2));
      csconf=1;
      return;
  }
  Mat ncc(1,1,CV_32F);
  float nccP,csmaxP,maxP=0;
  bool anyP=false;
  int maxPidx,validatedPart = ceil(pEx.size()*valid);
  float nccN, maxN=0;
  bool anyN=false;
  for (int i=0;i<pEx.size();i++){
      //cout<<pEx[i].size()<<endl;
      //cout<<pEx[i].type()<<" "<<example.type()<<" "<<CV_8U<<" "<<CV_32F<<endl;
      matchTemplate(pEx[i],example,ncc,CV_TM_CCORR_NORMED);      // measure NCC to positive examples
      nccP=(((float*)ncc.data)[0]+1)*0.5;
      if (nccP>ncc_thesame)
        anyP=true;
      if(nccP > maxP){
          maxP=nccP;
          maxPidx = i;
          if(i<validatedPart)
            csmaxP=maxP;
      }
  }
  for (int i=0;i<nEx.size();i++){
      matchTemplate(nEx[i],example,ncc,CV_TM_CCORR_NORMED);     //measure NCC to negative examples
      nccN=(((float*)ncc.data)[0]+1)*0.5;
      if (nccN>ncc_thesame)
        anyN=true;
      if(nccN > maxN)
        maxN=nccN;
  }
  //set isin
  if (anyP) isin[0]=1;  //if he query patch is highly correlated with any positive patch in the model then it is considered to be one of them
  isin[1]=maxPidx;      //get the index of the maximall correlated positive patch
  if (anyN) isin[2]=1;  //if  the query patch is highly correlated with any negative patch in the model then it is considered to be one of them
  //Measure Relative Similarity
  float dN=1-maxN;
  float dP=1-maxP;
  rsconf = (float)dN/(dN+dP);
  //Measure Conservative Similarity
  dP = 1 - csmaxP;
  csconf =(float)dN / (dN + dP);
}

void FerNNClassifier::evaluateTh(const vector<pair<vector<int>,int> >& nXT,const vector<cv::Mat>& nExT){
float fconf;
  for (int i=0;i<nXT.size();i++){
    fconf = (float) measure_forest(nXT[i].first)/nstructs;
    if (fconf>thr_fern)
      thr_fern=fconf;
}
  vector <int> isin;
  float conf,dummy;
  for (int i=0;i<nExT.size();i++){
      NNConf(nExT[i],isin,conf,dummy);
      if (conf>thr_nn)
        thr_nn=conf;
  }
  if (thr_nn>thr_nn_valid)
    thr_nn_valid = thr_nn;
}

void FerNNClassifier::show(){
  Mat examples((int)pEx.size()*pEx[0].rows,pEx[0].cols,CV_8U);
  double minval;
  Mat ex(pEx[0].rows,pEx[0].cols,pEx[0].type());
  //cout<<"showtype: "<<pEx[0].type()<<endl;
  for (int i=0;i<pEx.size();i++){
    minMaxLoc(pEx[i],&minval);
    pEx[i].copyTo(ex);
    ex = ex-minval;
    Mat tmp = examples.rowRange(Range(i*pEx[i].rows,(i+1)*pEx[i].rows));
    ex.convertTo(tmp,CV_8U);
  }
  imshow("Examples",examples);
}
void FerNNClassifier::saveNN(string file)
{
    ostringstream filename;
    filename<<file<<".yml";

    FileStorage fs(filename.str(),FileStorage::WRITE);
    fs<<"thr_nn"<<(float)thr_nn;
    fs<<"thr_nn_valid"<<(float)thr_nn_valid;
    fs<<"pExSize"<<(int)pEx.size();
    fs<<"nExSize"<<(int)nEx.size();
    //fs<<"patchSize";
    //fs<<"{";
    fs<<"rows"<<(int)pEx[0].rows;
    fs<<"cols"<<(int)pEx[0].cols;
    //fs<<"}";

    for(int i=0;i<pEx.size();i++)
    {
        ostringstream label;
        label<<"pEx"<<i;
        fs<<label.str();
        fs<<"[";

        for(int irow=0;irow<pEx[0].rows;irow++)
            for(int icol=0;icol<pEx[0].cols;icol++)
            {
                fs<<pEx[i].at<float>(irow,icol);
            }

        fs<<"]";
    }

    for(int i=0;i<nEx.size();i++)
    {
        ostringstream label;
        label<<"nEx"<<i;
        fs<<label.str();
        fs<<"[";

        for(int irow=0;irow<nEx[0].rows;irow++)
            for(int icol=0;icol<nEx[0].cols;icol++)
            {
                fs<<nEx[i].at<float>(irow,icol);
            }

        fs<<"]";
    }

    //for(int i=0;i<;i++)

//    for(int i=0;i<pEx.size();i++)
//    {
//        ostringstream label;
//        label<<"pEx"<<i<<".jpg";
//        //Mat tempPEx;
//        //pEx[i].convertTo(tempPEx,CV_8UC1);
//        vector<int> compress;
//        compress.push_back(CV_IMWRITE_JPEG_QUALITY);
//        compress.push_back(100);
//        //pEx[i].convertTo(pEx[i],CV_8U);
//        imwrite(label.str(),pEx[i],compress);
//    }

//    inwrite
//    fs<<"pEx"<<"[";
//    for(int i=0;i<pEx.size();i++)
//    {
//        //ostringstream label;
//        //label<<"pEx"<<i;
//        fs<<"{";
//        //fs<<label.str()<<pEx[i];
//        fs<<pEx[i];
//        fs<<"}";
//    }
//    fs<<"]";

//    fs<<"nExSize"<<(int)nEx.size();
//    for(int i=0;i<nEx.size();i++)
//    {
//        ostringstream label;
//        label<<"nEx"<<i<<".jpg";
//        imwrite(label.str(),nEx[i]);
//    }


    fs.release();


}

void FerNNClassifier::saveFern(string file)
{
    ostringstream filename;
    filename<<file<<".yml";
    FileStorage fs(filename.str(),FileStorage::WRITE);
    //threshold
    fs<<"thr_fern"<<thr_fern;
    //feature output
    fs<<"feature_table";
    fs<<"{";//feature_table{
    fs<<"dim1"<<(int)features.size();
    fs<<"dim2"<<(int)features[0].size();


    fs<<"features"<<"[";
    for(int i=0;i<features.size();i++)
        for(int j=0;j<features[0].size();j++)
        {
            //ostringstream index;
            //index<<"feature"<<i<<j;
            //fs<<index.str();
            fs<<"{";
            fs<<"x1"<<features[i][j].x1;
            fs<<"x2"<<features[i][j].x2;
            fs<<"y1"<<features[i][j].y1;
            fs<<"y2"<<features[i][j].y1;
            fs<<"}";
        }
    fs<<"]";//features}
    fs<<"}";//feature_table}

    fs<<"posterior_table";
    fs<<"{";//[posterior_table
    fs<<"dim1"<<(int)posteriors.size();
    fs<<"dim2"<<(int)posteriors[0].size();
    fs<<"posteriors";
    fs<<"[";
    for(int i=0;i<posteriors.size();i++)
        for(int j=0;j<posteriors[0].size();j++)
        {
            //fs<<"{";
            //ostringstream index;
            //index<<"posterior"<<i<<j;
            //fs<<index.str();
            fs<<posteriors[i][j];
            //fs<<"}";
        }
    fs<<"]";
    fs<<"}";//"]"posterior_table


    fs<<"pCounter_table";
    fs<<"{";//pCounter_table
    fs<<"dim1"<<(int)pCounter.size();
    fs<<"dim2"<<(int)pCounter[0].size();
    fs<<"pCounters";
    fs<<"[";
    for(int i=0;i<pCounter.size();i++)
        for(int j=0;j<pCounter[0].size();j++)
        {
            //fs<<"{";
            //ostringstream index;
            //index<<"posterior"<<i<<j;
            //fs<<index.str();
            fs<<pCounter[i][j];
            //fs<<"}";
        }
    fs<<"]";
    fs<<"}";//pCounter_table


    fs<<"nCounter_table";
    fs<<"{";//nCounter_table
    fs<<"dim1"<<(int)nCounter.size();
    fs<<"dim2"<<(int)nCounter[0].size();
    fs<<"nCounters";
    fs<<"[";
    for(int i=0;i<nCounter.size();i++)
        for(int j=0;j<nCounter[0].size();j++)
        {
            //fs<<"{";
            //ostringstream index;
            //index<<"posterior"<<i<<j;
            //fs<<index.str();
            fs<<nCounter[i][j];
            //fs<<"}";
        }
    fs<<"]";
    fs<<"}";//nCounter_table

//    fs<<"posteriors"<<posteriors;
//    fs<<"pCounter"<<pCounter;
//    fs<<"nCounter"<<nCounter;
}


void FerNNClassifier::loadNN(string file)
{
    ostringstream filename;
    filename<<file<<".yml";
    FileStorage fs;
    fs.open(filename.str(), FileStorage::READ);


    int pExSize=(int)fs["pExSize"];
    int nExSize=(int)fs["nExSize"];
    //thr_nn=(float)fs["thr_nn"];
    //thr_nn_valid=(float)fs["thr_nn_valid"];

    cout<<"pExSize: "<<pExSize<<endl;
    cout<<"nExSize: "<<nExSize<<endl;
    cout<<"thr_nn_valid: "<<thr_nn_valid<<endl;
    cout<<"thr_nn: "<<thr_nn<<endl;

    pEx.resize(pExSize);
    nEx.resize(nExSize);

    int patchRows=(int)fs["rows"];
    int patchCols=(int)fs["cols"];
    //cout<<"patchRows"<<patchRows<<endl;

    for(int i=0;i<pExSize;i++)
    {
        ostringstream label;
        label<<"pEx"<<i;
        FileNode pixelnode=fs[label.str()];
        FileNodeIterator it=pixelnode.begin();

        pEx[i].create(patchRows,patchCols,CV_32F);

        for(int irow=0;irow<patchRows;irow++)
        {
            for(int icol=0;icol<patchCols;icol++)
            {
                pEx[i].at<float>(irow,icol)=(float)(*it);
                //cout<<pEx[i].at<float>(irow,icol);
                it++;
            }
        }
    }


    for(int i=0;i<nExSize;i++)
    {
        ostringstream label;
        label<<"nEx"<<i;
        FileNode pixelnode=fs[label.str()];
        FileNodeIterator it=pixelnode.begin();

        nEx[i].create(patchRows,patchCols,CV_32F);

        for(int irow=0;irow<patchRows;irow++)
        {
            for(int icol=0;icol<patchCols;icol++)
            {
                nEx[i].at<float>(irow,icol)=(float)(*it);
                //cout<<nEx[i].at<float>(irow,icol);
                it++;
            }
        }
    }
//    FileNode pExnode=fs["pEx"];
//    FileNodeIterator it = pExnode.begin();

//    for( int idx=0; it != pExnode.end(); ++it, idx++ )
//    {
//        //cout<<(Mat)(*it)<<endl;
//    }

    //namedWindow("1",WINDOW_NORMAL);

    //Mat sizePatch=imread("pEx0.jpg",0);
    //tmppatch.create(sizePatch.rows,sizePatch.cols,CV_32F);
//    for(int i=0;i<pExSize;i++)
//    {
//        ostringstream label;
//        label<<"pEx"<<i<<".jpg";
//        Mat tmppatch1=imread(label.str(),0);
//        //imshow("1",tmppatch1);
//        tmppatch1.convertTo(pEx[i],CV_32F);
//        //Mat tmppatch2;
//        //tmppatch.convertTo(tmppatch2,CV_8U);
//        //tmppatch1.copyTo(tmppatch);
//        //tmppatch.at<float>(0,0)=1;
//        //imshow("2",tmppatch2);

//        //pEx.push_back(tmppatch);
//        //fs<<label.str()<<pEx[i];
//    }

//    cout<<pEx[0]<<endl;

//    for(int i=0;i<nExSize;i++)
//    {
//        ostringstream label;
//        label<<"nEx"<<i<<".jpg";
//        Mat tmppatch1=imread(label.str(),0);
//        tmppatch1.convertTo(nEx[i],CV_32F);
//        //nEx.push_back(tmppatch);
//    }

//    namedWindow("1",WINDOW_NORMAL);
//    Mat showimg;
//    pEx[0].convertTo(showimg,CV_8U);
//    imshow("1",showimg);



    //show();
    fs.release();
}

void FerNNClassifier::loadFern(string file)
{
    ostringstream filename;
    filename<<file<<".yml";
    FileStorage fs;
    fs.open(filename.str(), FileStorage::READ);
    FileNode featuretable=fs["feature_table"];
    int dim1=featuretable["dim1"];
    int dim2=featuretable["dim2"];
    FileNode featurenode=featuretable["features"];
    FileNodeIterator it=featurenode.begin();
    thr_fern=fs["thr_fern"];
    cout<<"thr_fern: "<<thr_fern<<endl;

    features = vector<vector<Feature> >(dim1,vector<Feature> (dim2));

    for(int i=0;i<dim1;i++)
        for(int j=0;j<dim2;j++)
        {

            int x1=(int)(*it)["x1"];
            int y1=(int)(*it)["y1"];
            int x2=(int)(*it)["x2"];
            int y2=(int)(*it)["y2"];
            features[i][j]=Feature(x1, y1, x2, y2);
            //cout<<x1<<endl;
            it++;
            if(it==featurenode.end())
                break;
        }



    FileNode posteriorstable=fs["posterior_table"];
    dim1=posteriorstable["dim1"];
    dim2=posteriorstable["dim2"];
    posteriors = vector<vector<float> >(dim1,vector<float> (dim2));
    FileNode posteriornode=posteriorstable["posteriors"];
    it=posteriornode.begin();

    for(int i=0;i<dim1;i++)
        for(int j=0;j<dim2;j++)
        {
            posteriors[i][j]=(float)(*it);
            //cout<<(float)(*it)<<endl;
            it++;
            if(it==posteriornode.end())
                break;
        }



    FileNode pCountertable=fs["pCounter_table"];
    dim1=pCountertable["dim1"];
    dim2=pCountertable["dim2"];
    pCounter = vector<vector<int> >(dim1,vector<int> (dim2));
    FileNode pCounternode=pCountertable["pCounters"];
    it=pCounternode.begin();

    for(int i=0;i<dim1;i++)
        for(int j=0;j<dim2;j++)
        {
            pCounter[i][j]=(int)(*it);
            //cout<<(int)(*it)<<endl;
            it++;
            if(it==pCounternode.end())
                break;
        }



    FileNode nCountertable=fs["nCounter_table"];
    dim1=nCountertable["dim1"];
    dim2=nCountertable["dim2"];
    nCounter = vector<vector<int> >(dim1,vector<int> (dim2));
    FileNode nCounternode=nCountertable["nCounters"];
    it=nCounternode.begin();

    for(int i=0;i<dim1;i++)
        for(int j=0;j<dim2;j++)
        {
            nCounter[i][j]=(int)(*it);
            //cout<<(int)(*it)<<endl;
            it++;
            if(it==nCounternode.end())
                break;
        }

    thrN = 0.5*nstructs;




}
