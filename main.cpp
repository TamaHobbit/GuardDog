#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
using std::cout;
using std::cerr;
using std::endl;

const char * window_title = "GuardDog";

const int numberOfSteps = 2;
cv::Mat images[numberOfSteps-1];
std::vector<cv::Mat> frame_split(3);
std::vector<cv::Mat> reference_split(3);

struct CameraWrapper {
  CameraWrapper() {}
  ~CameraWrapper() { cout << "Camera released"; cam.release(); }
	raspicam::RaspiCam_Cv cam;
};

int main ( int argc,char **argv ) {
	{
	CameraWrapper cameraWrapper;
	raspicam::RaspiCam_Cv & Camera = cameraWrapper.cam;

	//set camera params
	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

	//Open camera
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

	cv::namedWindow(window_title,1); //0 required for FULLSCREEN, 1 is normal (autosize)

	do {
		Camera.grab();
		Camera.retrieve(images[0]);
	} while(cv::waitKey(30) != 32); //first spacebar; get ref image

	cv::split(images[0],reference_split);

	//Start capture
	do {
		Camera.grab();
		Camera.retrieve(images[0]);

		cv::split(images[0], frame_split);
		for(int channel = 0; channel < 3; channel++){
			cv::absdiff(frame_split.at(channel), reference_split.at(channel), frame_split.at(channel));
			cv::threshold(frame_split.at(channel), frame_split.at(channel), 15, 255, CV_THRESH_BINARY);
			merge(frame_split, images[0]);
		}

		cv::imshow(window_title, images[0]);
	} while( cv::waitKey(30) != 32 ); //spacebar
	}

	return 0;
}

