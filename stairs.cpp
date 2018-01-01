#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<math.h>
#include <iostream>

using namespace cv;
using namespace std;



float cal_ratio(Vec4i l)
{ 
    cout<<"\n reached function";
    cout <<"\n"<<l[1]<<"\t"<<l[3];
	float x = l[2]-l[0];
	float angle=l[3]-l[1];
    float ang= angle/x;
    return ang;

}

bool edgesort(const Vec4i &lhs, const Vec4i &rhs)
{
    return lhs[1] < rhs[1];
}



int main(int argc, char** argv)
{
	VideoCapture capture(0);
 	if(!capture.isOpened())  // check if we succeeded
	        return -1;
	
  int width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
  int height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
  float min_dist= 0.05*height;
  float max_dist=0.2*height;
  Mat src;
  Mat edges,dst, cdst;
  float roi_width = 0.8*width;
  float edge_width;
  
  vector<Vec4i> lines, parallel_edges,final_edges;
  for(;;)
  {

    capture >> src;
 	Rect rect = Rect(0.1*width,0.5*height,0.8*width,0.5*height);
	edges=src(rect);

 
    Canny(edges, dst, 200, 300, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

 
  
    HoughLinesP(dst, lines, 1, CV_PI/180,50, 50,30); //for detecting straight lines

    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        float ang= cal_ratio(l);
        if(ang >=-0.47 && ang <=0.47)   //lines between +/- 35 degree
        {
            //cout<<"\n initial slope:"<<" " <<ang;
		        edge_width=l[2]-l[0];
		        {
                    if(edge_width >= (0.2 * roi_width))  //to check if edge length is 20 per cent of the width of ROI
                    {      
                        parallel_edges.push_back(lines[i]);
                        //cout<<"\n parallel :"<< i;
                    }

                    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0),2, CV_AA); //draw the almost horizontal lines
		
		        }
  	    }

    } 

    //cout<<"\n Parallel edges :"<<parallel_edges.size();
    //cout<<"\n***********************************************************" ;   
    
    for(size_t i=0;i<parallel_edges.size();i++)
    {
        Vec4i d1 = parallel_edges[i];
        Vec4i d2 = parallel_edges[i+1];
        float slope1= cal_ratio(d1);
        float slope2= cal_ratio(d2);
        //cout<<"\n slope of edge1:"<<slope1;
        //cout<<"\n slope of edge2:"<<slope2;
        if(abs(slope1-slope2)>=6.88)                            // to check if the edges are CONGRUENT
            parallel_edges.erase(parallel_edges.begin()+i);

    
    }
    //cout<<"\n Parallel edges AFTER CONGRUENCY CHECK :"<<parallel_edges.size();
    //cout<<"\n reached point 6";
    //cout<<"\n"<<parallel_edges.size();

    //cout<<"\n Sorting of parallel edges:";
    std::sort(parallel_edges.begin(),parallel_edges.end(), &edgesort);



    for(size_t i=0;i<parallel_edges.size();i++)
    {
	      Vec4i d1 = parallel_edges[i];
	      Vec4i d2 = parallel_edges[i+1];
          Vec4i d3;
          float  ydist = abs(d1[3]-d2[3]);
         
	      if( ydist <= min_dist)
          {
            
            d3[1] = (d1[1]+d2[1])/2;
            d3[3] = (d1[3]+d2[3])/2;
            d3[0] = d1[0]; 
            d3[2] = d1[2]; 
            
            //cout<<"\n replacing edge";
            cout<<"\n x1:"<<d3[0]<<"\t y1"<<d3[1]<<"\tx2:"<<d3[2]<<"\t y2:"<<d3[3];

            std::replace(parallel_edges.begin(), parallel_edges.end(),parallel_edges.at(i),d3);
            parallel_edges.erase(parallel_edges.begin()+(i+1));
            //cout<<"\n"<<parallel_edges.size();
        }

        else if( ydist >=max_dist)
        {
           
            parallel_edges.erase(parallel_edges.begin()+i); //removing the bottom edge
            cout<<"\n"<<parallel_edges.size();
        }
    }

    //cout<<"\n******************************************************";
    //cout<<"\n Parallel edges AFTER DISTANCE CHECK :"<<parallel_edges.size();

    for(size_t i=0;i<parallel_edges.size();i++)
    {
        Vec4i l = parallel_edges[i];
        cout<<"\n x1:"<<l[0]<<"\t y1"<<l[1]<<"\tx2:"<<l[2]<<"\t y2:"<<l[3];
		line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(147,20,255),2, CV_AA);   //draw the almost horizontal lines
        
    }

    if(parallel_edges.size()>=3)
        rectangle( cdst, Point(parallel_edges[0][0], parallel_edges[0][1]),Point(parallel_edges[parallel_edges.size()-1][2],
                    parallel_edges[parallel_edges.size()-1][3]), Scalar(0,0,255),2, CV_AA);     //draw the almost horizontal lines

    parallel_edges.clear();
    final_edges.clear();
    lines.clear();
   
    imshow("source", src);
    imshow("detected lines", cdst);
    //if(waitKey(30) >= 0) break;
	 int c = waitKey();
   if (char(c)==27)
   break    ;
     
}

 return 0;
}

       
