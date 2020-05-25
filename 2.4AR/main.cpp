#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "tigl.h"
#include "3D.h"
#include "BackgroundRemover.h"
#include "SkinDetector.h"
#include "FaceDetector.h"
#include "FingerCount.h"
#include <thread>

using namespace cv;
using namespace std;

void facerecognition();

int main(int argc, char* argv[])
{
	thread t1(startup);
	thread t2(facerecognition);
	t1.join();
	t2.join();
	return 0;
}

void facerecognition() {

	VideoCapture cam("http://192.168.1.27:4747/mjpegfeed");
	cam.set(CAP_PROP_SETTINGS, 1);

	if (!cam.isOpened()) {
		cout << "Can't find camera!" << endl;
		return ;
	}

	Mat frame, frameOut, handMask, foreground, fingerCountDebug;

	BackgroundRemover backgroundRemover;
	SkinDetector skinDetector;
	FaceDetector faceDetector;
	FingerCount fingerCount;
	int i = 0;


	while (true) {
		cam >> frame;
		frameOut = frame.clone();
		skinDetector.drawSkinColorSampler(frameOut);
		if (i % 3 == 0) {
			foreground = backgroundRemover.getForeground(frame);
			faceDetector.removeFaces(frame, foreground);
			handMask = skinDetector.getSkinMask(foreground);
			fingerCountDebug = fingerCount.findFingersCount(handMask, frameOut);
			imshow("foreground", foreground);
			imshow("handMask", handMask);
		}
		imshow("output", frameOut);
		imshow("foreground", foreground);
		imshow("handMask", handMask);
		imshow("handDetection", fingerCountDebug);

		int key = waitKey(1);

		if (key == 27) // esc
			break;
		else if (key == 98) { // b
			backgroundRemover.calibrate(frame);
		}
		else if (key == 115) {// s
			skinDetector.calibrate(frame);
		}
		i++;
	}
}