#pragma once

#include "ofMain.h"
#include "ofUtils.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "vector.h"

using namespace std;

#define MAX_FRAMES 40

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	ofxKinect kinect;
	int imageWidth;
	int imageHeight;
	vector<unsigned char *> frames;
	unsigned char * maskPixels;
	
	ofImage frame0;
	ofImage mask;
	ofImage distorted;
	
	bool gotFrame;
	
	int angle;
};
