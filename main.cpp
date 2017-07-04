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
			Camera.retrieve(image);
		} while(cv::waitKey(30) != 32); //first spacebar; get ref image

		cv::split(image,reference_split);
		const int erosion_type = cv::MORPH_RECT;
		const int erosion_size = 1;
		cv::Mat erode_brush = cv::getStructuringElement( 	erosion_type,
                                           				cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                           				cv::Point( erosion_size, erosion_size ) );

		do {
			Camera.grab();
			Camera.retrieve(image);

			cv::split(image, frame_split);
			for(int channel = 0; channel < 3; channel++){
				cv::absdiff(frame_split.at(channel), reference_split.at(channel), frame_split.at(channel));

				cv::threshold(frame_split.at(channel), frame_split.at(channel), 155, 255, CV_THRESH_BINARY);

				//cv::erode( frame_split.at(channel), frame_split.at(channel), erode_brush );

				merge(frame_split, image);
			}

			cv::imshow(window_title, *current_image);
		} while( cv::waitKey(30) != 32 ); //spacebar
	}

	return 0;
}

