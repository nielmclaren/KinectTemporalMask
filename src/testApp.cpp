#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	kinect.open();
	
	ofSetFrameRate(60);
	
	imageWidth = kinect.width;
	imageHeight = kinect.height;
	frame0.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
	mask.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
	distorted.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
	cout << "Image size: " << imageWidth << "x" << imageHeight << endl;
	
	gotFrame = false;
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
}

void testApp::update() {
	ofBackground(100, 100, 100);
	
	kinect.update();
	if(kinect.isFrameNew()) {
		unsigned char* kinectPixels = kinect.getPixels();
		unsigned char* framePixels = (unsigned char*) malloc(imageWidth * imageHeight * 3);
		for (int i = 0; i < imageWidth * imageHeight * 3; i++) {
			framePixels[i] = kinectPixels[i];
		}
		
		frames.push_back(framePixels);
		if (frames.size() > MAX_FRAMES) {
			delete frames[0];
			frames.erase(frames.begin());
		}
		
		maskOfPixels.setFromPixels(kinect.getDepthPixels(), imageWidth, imageHeight, OF_IMAGE_GRAYSCALE);
		blur(maskOfPixels, 100);
		maskPixels = maskOfPixels.getPixels();
		
		gotFrame = true;
	}
}

void testApp::draw() {
	ofSetColor(255, 255, 255);
	
	if (gotFrame) {
		unsigned char * distortedPixels = distorted.getPixels();
		
		int numFrames = frames.size();
		for (int x = 0; x < imageWidth; x++) {
			for (int y = 0; y < imageHeight; y++) {
				for (int c = 0; c < 3; c++) {
					float maskFraction = maskPixels[y * imageWidth + x] / 255.0;
					int frameIndex = maskFraction * (numFrames - 1);
					//float remainder = (maskFraction * (numFrames - 1) - frameIndex) / (numFrames - 1);
					
					int pixelIndex = y * imageWidth * 3 + x * 3 + c;
					unsigned char* frame = frames[frameIndex];
					//unsigned char* nextFrame = frames[frameIndex + 1];
					distortedPixels[pixelIndex] = frame[pixelIndex];
				}
			}
		}
		
		mask.setFromPixels(maskPixels, imageWidth, imageHeight, OF_IMAGE_GRAYSCALE);
		mask.draw(10, 320, 400, 300);
		
		frame0.setFromPixels(frames[frames.size() - 1], imageWidth, imageHeight, OF_IMAGE_COLOR);
		frame0.draw(10, 10, 400, 300);
		
		distorted.setFromPixels(distortedPixels, imageWidth, imageHeight, OF_IMAGE_COLOR);
		distorted.draw(420, 10, 800, 600);
	}
	
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
	<< ofToString(kinect.getMksAccel().y, 2) << " / "
	<< ofToString(kinect.getMksAccel().z, 2) << endl
	<< "fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
	<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl;
	ofDrawBitmapString(reportStream.str(),20,652);
}

void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
	while (frames.size() > 0) {
		delete [] frames[0];
		frames.erase(frames.begin());
	}
}

void testApp::keyPressed (int key) {
	switch (key) {
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case ' ':
			ofImage capture;
			capture.grabScreen(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
			capture.saveImage("screenshot.jpg");
			distorted.saveImage("capture.jpg");
			break;
	}
}

void testApp::mouseDragged(int x, int y, int button)
{}

void testApp::mousePressed(int x, int y, int button)
{}

void testApp::mouseReleased(int x, int y, int button)
{}

void testApp::windowResized(int w, int h)
{}
