#pragma once

#include "rimage.h"
#include "eyefinderBinary.h"
#include "ofPixels.h"
#include "ofRectangle.h"


class ofxMPT{
public:

	ofxMPT();

	void allocate(int width, int height);
	void close();

	void update(const ofPixels & pixels);
	void update(const ofPixels & pixels, ofRectangle roi);

	void findEyes();

	struct Face{
		ofRectangle rect;
		ofRectangle leftEye;
		ofRectangle rightEye;
		ofRectangle bothEyes;
		ofVec2f		leftEyeCenter;
		ofVec2f		rightEyeCenter;
		ofVec2f		center;
		float		angle;
	};

	vector<Face> getFaces();


	static string LOG_NAME;

private:
	RImage<float> *     rImage;
	int 				width, height;
	int 				totalDetectionPixels;

	VisualObject        faces;
    MPEyeFinderBinary*	eyeFinder;

    vector<Face>		foundFaces;

    bool				firstIteration;
};
