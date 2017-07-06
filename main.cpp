#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <unistd.h>
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;

const char * window_title = "GuardDog";

struct CameraWrapper {
  CameraWrapper() {}
  ~CameraWrapper() { cam.release(); }
	raspicam::RaspiCam_Cv cam;
};

std::vector<std::pair<std::string,cv::Mat> > writeQueue;

std::string getImageNameNow(){
	std::time_t t = std::time(NULL);
	char mbstr[100];
	if (std::strftime(mbstr, sizeof(mbstr), "../images/%d-%m-%Y_%H_%M_%S.jpg", std::localtime(&t))) {
		return std::string(mbstr);
	} else {
		return "foo";
	}
}

void saveImage(const cv::Mat & frame){
	auto name = getImageNameNow();
	writeQueue.push_back(std::make_pair(name, frame.clone()));
}

void writeImages(){
	for(auto it : writeQueue){
		cv::imwrite(it.first,it.second);
	}
	writeQueue.clear();
}

int main ( /*int argc,char **argv*/ ) {
	CameraWrapper cameraWrapper;
	raspicam::RaspiCam_Cv & Camera = cameraWrapper.cam;

	//set camera params
	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

	//Open camera
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

	cv::namedWindow(window_title,1); //0 required for FULLSCREEN, 1 is normal (autosize)

	float mean_singleframe_diff = 0; // the mean difference between one frame and the next
	cv::Mat reference_image; // grayscaled, last image in stabilization phase

	// allow camera image to stabilize; grab two images and compare to establish mean_singleframe_diff
	const int stabilize_frames = 100;
	for(int i = 0; i < stabilize_frames; ++i) {
		cv::Mat image, previous_frame;

		Camera.grab();
		Camera.retrieve(previous_frame);
		cv::cvtColor(previous_frame, previous_frame, CV_BGR2GRAY);

		cv::imshow(window_title, previous_frame);

		Camera.grab();
		Camera.retrieve(image);
		cv::cvtColor(image, reference_image, CV_BGR2GRAY);

		cv::absdiff(previous_frame, reference_image, previous_frame);
		float diff = cv::mean(previous_frame)[0];
		mean_singleframe_diff += diff / stabilize_frames;

		cout << "\rStabilizing: " << std::fixed << std::setprecision(3) << diff << " -> " << mean_singleframe_diff << std::flush;
		cv::waitKey(1); // need to wait the same as in main phase
	}

	cout << endl << "Starting..." << endl;

	const float max_diff_ratio = 1.1f;

	// we're going to record from the first interesting frame for at least movement_record_lagg frames
	unsigned int frame_num = 0;
	bool recording = false;
	unsigned int last_record_frame = 0;
	const unsigned int movement_record_lagg = 2;

	// main phase
	while(true){
		cv::Mat image, previous_frame;

		Camera.grab();
		Camera.retrieve(previous_frame);
		if( recording ){
			cv::imshow(window_title, previous_frame);
		}
		cv::cvtColor(previous_frame, previous_frame, CV_BGR2GRAY);
		if( !recording ){
			cv::imshow(window_title, previous_frame);
		}

		Camera.grab();
		Camera.retrieve(image);
		cv::cvtColor(image, reference_image, CV_BGR2GRAY);

		cv::absdiff(previous_frame, reference_image, previous_frame);
		float diff = cv::mean(previous_frame)[0];

		// ratio between this diff and stable diff, >= 1
		float ratio = diff > mean_singleframe_diff ? diff / mean_singleframe_diff : mean_singleframe_diff / diff;

		cout << "\rDiff: " << std::fixed << std::setprecision(3) << diff << " : " \
			   << std::fixed << std::setprecision(3) << mean_singleframe_diff \
			   << " = " << std::fixed << std::setprecision(3) << ratio << "/" \
			   << std::fixed << std::setprecision(3) << max_diff_ratio << std::flush;

		if( ratio > max_diff_ratio ){
    	recording = true;
    	last_record_frame = frame_num;
    } else if ( last_record_frame + movement_record_lagg > frame_num ){
    	// if movement_record_lagg is zero, this is always true and we stop recording at the first uninteresting frame
    	recording = false;
		}

		++frame_num;
		if( cv::waitKey(1) == 32 ){ // press spacebar to stop
			break;
		}
	}

	cout << endl;

	return 0;
}

