#include "highgui.h"
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

cv::Mat image;

struct CameraWrapper {
  CameraWrapper() {}
  ~CameraWrapper() { cam.release(); }
	raspicam::RaspiCam_Cv cam;
};

int main ( int argc,char **argv ) {
	CameraWrapper cameraWrapper;
	raspicam::RaspiCam_Cv & Camera = cameraWrapper.cam;

	//set camera params
	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );

	//Open camera
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

	cv::namedWindow(window_title,1); //0 required for FULLSCREEN, 1 is normal (autosize)

	cv::Mat reference_image;
	do {
		Camera.grab();
		Camera.retrieve(reference_image);
	} while(cv::waitKey(30) != 32); //first spacebar; get ref image

	const float IM_SCALE = 0.05f;
	resize(reference_image, reference_image, cv::Size(), IM_SCALE, IM_SCALE, cv::INTER_AREA);

	cv::Mat new_ref;
	do {
		Camera.grab();
		Camera.retrieve(new_ref);
		resize(new_ref, new_ref, cv::Size(), IM_SCALE, IM_SCALE, cv::INTER_AREA);

		cv::absdiff(new_ref, reference_image, image);
		reference_image = new_ref.clone();

		cv::imshow(window_title, image);

		cv::Scalar m = cv::mean(image);
    cout << "\r" << m[0] << std::flush;

	} while( cv::waitKey(30) != 32 ); //spacebar

	cout << endl;

	return 0;
}

