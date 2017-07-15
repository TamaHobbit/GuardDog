#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <unistd.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <queue>
#include <chrono>
using std::cout;
using std::cerr;
using std::endl;

//const char * window_title = "GuardDog";

struct CameraWrapper {
  CameraWrapper() {}
  ~CameraWrapper() { cam.release(); }
	raspicam::RaspiCam_Cv cam;
};

std::vector<cv::Mat> writeQueue;

std::string getTimestamp(){
	std::time_t t = std::time(NULL);
	char mbstr[20];
	if (std::strftime(mbstr, sizeof(mbstr), "%d-%m-%Y_%H_%M_%S", std::localtime(&t))) {
		return std::string(mbstr);
	} else {
		return "unknown_time";
	}
}

std::string getDataFilename(){
	auto timestamp = getTimestamp();
	std::ostringstream ss;
	ss << "../data/" << getTimestamp() << ".raw";
	return ss.str();
}

std::string getEventFilename(){
	auto timestamp = getTimestamp();
	std::ostringstream ss;
	ss << "../images/events/" << getTimestamp() << ".jpg";
	return ss.str();
}

std::string getHourlyImageFilename(){
	auto timestamp = getTimestamp();
	std::ostringstream ss;
	ss << "../images/hourly/" << getTimestamp() << ".jpg";
	return ss.str();
}

void saveImage(const cv::Mat & frame){
	writeQueue.push_back(frame.clone());
}

void writeImages(){
	if( writeQueue.empty() ){ return; }
	auto name = getEventFilename();
	auto it = writeQueue.begin() + writeQueue.size()/2;
	cv::imwrite(name, *it);
	writeQueue.clear();
}

std::string diffdata_buffer;
std::ofstream diffdata_file(getDataFilename());

void flushData(){
	diffdata_file.write(diffdata_buffer.data(), diffdata_buffer.size());
	diffdata_buffer.clear();
}

void add_timestamp(){
	std::time_t t = std::time(NULL);
	char mbstr[20];
	std::strftime(mbstr, sizeof(mbstr), "%d-%m-%Y_%H_%M_%S", std::localtime(&t)); //ignore returnval; potential failure
	diffdata_buffer.append(mbstr);
	diffdata_buffer.append("\n");
}

void add_data(float value){
	diffdata_buffer.append( std::to_string(value) );
	diffdata_buffer.append("\n");
}

int main ( /*int argc,char **argv*/ ) {
	CameraWrapper cameraWrapper;
	raspicam::RaspiCam_Cv & Camera = cameraWrapper.cam;

	//set camera params
	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

	//Open camera
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

	//cv::namedWindow(window_title,1); //0 required for FULLSCREEN, 1 is normal (autosize)

	float mean_singleframe_diff = 0; // the mean difference between one frame and the next
	cv::Mat reference_image; // grayscaled, last image in stabilization phase

	add_timestamp();
	flushData();

	// allow camera image to stabilize; grab two images and compare to establish mean_singleframe_diff
	const int stabilize_frames = 50;
	const int throwaway_frames = 10;
	std::queue<float> last_non_outliers;

	for(int i = 0; i < stabilize_frames + throwaway_frames; ++i) {
		cv::Mat image, previous_frame;

		Camera.grab();
		Camera.retrieve(previous_frame);
		cv::cvtColor(previous_frame, previous_frame, CV_BGR2GRAY);

		//cv::imshow(window_title, previous_frame);

		Camera.grab();
		Camera.retrieve(image);
		cv::cvtColor(image, reference_image, CV_BGR2GRAY);

		if( i >= throwaway_frames ){
			cv::absdiff(previous_frame, reference_image, previous_frame);
			float diff = cv::mean(previous_frame)[0];
			mean_singleframe_diff += diff / stabilize_frames;
			cout << "\rStabilizing: " << std::fixed << std::setprecision(3) << diff << " -> " << mean_singleframe_diff << std::flush;
			add_data(diff);
			last_non_outliers.push(diff); //assuming initial stabilization frames are never outliers!
		}
		cv::waitKey(1); // need to wait the same as in main phase
	}

	assert(last_non_outliers.size() == stabilize_frames);

	cout << endl << "Starting..." << endl;

	const float max_diff_ratio = 1.1f;
	float max_diff = mean_singleframe_diff * max_diff_ratio;

	// we're going to record from the first interesting frame for at least movement_record_lagg frames
	unsigned int frame_num = 0;
	bool recording = false;
	unsigned int last_record_frame = 0;
	const unsigned int movement_record_lagg = 2;

	const std::chrono::hours image_period(1);
	auto last_periodic_image = std::chrono::steady_clock::now() - image_period;

	// main phase
	while(true){
		cv::Mat image, previous_frame;

		Camera.grab();
		Camera.retrieve(previous_frame);
		if( recording ){
			//cv::imshow(window_title, previous_frame);
		}
		cv::cvtColor(previous_frame, previous_frame, CV_BGR2GRAY);
		if( !recording ){
			//cv::imshow(window_title, previous_frame);
		}

		Camera.grab();
		Camera.retrieve(image);
		cv::cvtColor(image, reference_image, CV_BGR2GRAY);

		cv::absdiff(previous_frame, reference_image, previous_frame);
		float diff = cv::mean(previous_frame)[0];
		add_data(diff);
		if( frame_num % 50 == 0 ){
			flushData();
			add_timestamp();

			auto now = std::chrono::steady_clock::now();
			std::chrono::duration<double> diff = now - last_periodic_image;
			if( diff > image_period ){
				last_periodic_image = now;
				cv::imwrite(getHourlyImageFilename(), image);
			}
		}

		if( diff > max_diff ){
    	recording = true;
    	last_record_frame = frame_num;
    	saveImage(image);
    } else if ( last_record_frame + movement_record_lagg > frame_num ){
    	// if movement_record_lagg is zero, this is always true and we stop recording at the first uninteresting frame
    	recording = false;
    	writeImages();
    	// update average diff of last [stabilize_frames] uninteresting frames
			float discard = last_non_outliers.front();
			last_non_outliers.pop();
			last_non_outliers.push(diff);
			mean_singleframe_diff -= discard / stabilize_frames;
			mean_singleframe_diff += diff / stabilize_frames;
			max_diff = mean_singleframe_diff * max_diff_ratio;
		}

		cout << "\rDiff: " << std::fixed << std::setprecision(3) << diff << " : " \
			   << std::fixed << std::setprecision(3) << max_diff << std::flush;

		++frame_num;
		if( cv::waitKey(1) == 32 ){ // press spacebar to stop
			break;
		}
	}

	cout << endl;

	return 0;
}

