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

cv::Mat image, reference_image, grayscaled_image, snapshot;

struct CameraWrapper {
  CameraWrapper() {}
  ~CameraWrapper() { cam.release(); }
	raspicam::RaspiCam_Cv cam;
};

int main ( /*int argc,char **argv*/ ) {
	CameraWrapper cameraWrapper;
	raspicam::RaspiCam_Cv & Camera = cameraWrapper.cam;

	//set camera params
	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

	//Open camera
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

	cv::namedWindow(window_title,1); //0 required for FULLSCREEN, 1 is normal (autosize)

	do {
		Camera.grab();
		Camera.retrieve(image);
		cv::cvtColor(image, reference_image, CV_BGR2GRAY);
		cv::imshow(window_title, reference_image);
	} while( cv::waitKey(30) != 32); // spacebar

	unsigned int frame_num = 0;
	float diff_value;

	do {
		Camera.grab();
		Camera.retrieve(image);

		cv::cvtColor(image, grayscaled_image, CV_BGR2GRAY);

		cv::absdiff(grayscaled_image, reference_image, grayscaled_image);

    cv::normalize(grayscaled_image, grayscaled_image, 0, 255, 32, CV_8UC1);

		cv::Scalar m = cv::mean(grayscaled_image);
		diff_value = m[0];
		if( frame_num % 10 == 0 ){
    	cout << "\rDiff: " << diff_value << std::flush;
    }

    if( diff_value > 30.0f ){
			cv::imshow(window_title, image);
		} else {
			cv::imshow(window_title, grayscaled_image);
		}

    ++frame_num;
	} while( cv::waitKey(30) != 32 ); //spacebar

	cout << endl;

	return 0;
}

