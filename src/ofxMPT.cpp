/*
 * ofxMPT.cpp
 *
 *  Created on: 29/09/2011
 *      Author: arturo
 */

#include "ofxMPT.h"
#include "visualobject.h"

string ofxMPT::LOG_NAME = "ofxMPT";

static ofRectangle filterRectangle(ofRectangle & curr, ofRectangle & prev, float amt){
	ofRectangle filtered;
	filtered.x = curr.x * amt + prev.x * (1-amt);
	filtered.y = curr.y * amt + prev.y * (1-amt);
	filtered.width = curr.width * amt + prev.width * (1-amt);
	filtered.height = curr.height * amt + prev.height * (1-amt);
	return filtered;
}

static bool isOutlier(ofxMPT::Face & current, ofxMPT::Face & previous){
	if(current.center.distance(previous.center)>current.rect.width*2) return true;
	else return false;
}


ofxMPT::ofxMPT(){
	rImage = 0;
}

void ofxMPT::allocate(int _width, int _height){
	width = _width;
	height = _height;
	totalDetectionPixels = width*height;
	rImage = new RImage<float>(width,height);

	eyeFinder = new MPEyeFinderBinary;
	eyeFinder->initStream(width,height);
	firstIteration = true;
}

void ofxMPT::close(){
	if(rImage) delete rImage;
	rImage = NULL;
	width = 0;
	height = 0;
	if(eyeFinder){
		delete eyeFinder;
		eyeFinder = 0;
	}
}

void ofxMPT::update(const ofPixels & pixels){
	if(pixels.size()!=totalDetectionPixels){
		ofLogError(LOG_NAME) << "error on update pixels size != ofxMPT size";
		return;
	}
	for (int i = 0; i < totalDetectionPixels; i++){
		rImage->array[i] = pixels[i];
	}
}

void ofxMPT::update(const ofPixels & pixels, ofRectangle roi){
	if(roi.width * roi.height!=totalDetectionPixels){
		ofLogError(LOG_NAME) << "error on update pixels size != ofxMPT size";
		return;
	}

	int initPos = int(roi.y * pixels.getWidth() + roi.x);
	int stride = pixels.getWidth() - roi.width;
	float * dst = rImage->array + initPos;
	const unsigned char * src = pixels.getPixels() + initPos;
	for(int y = 0; y < roi.height; y++){
		for(int x = 0; x < roi.width; x++){
			*dst = *src;
			dst++;
			src++;
		}
		dst += stride;
		src += stride;
	}
}

void ofxMPT::findEyes(){
	eyeFinder->findEyes(*rImage,faces);


	int i=0;
	if(!faces.empty()){
		foundFaces.resize(faces.size());
		FaceObject * face = static_cast<FaceObject*>(faces.front());

		if(face->xSize * face->ySize < float(width*height)/16.){
			faces.clear();
			return;
		}

		Face foundFace;

		foundFace.rect.set(face->x,face->y,face->xSize,face->ySize);
		int eyeSize = static_cast<int>(face->xSize * 0.1);
		if(face->eyes.leftEye){
			foundFace.leftEye.set(face->eyes.xLeft - eyeSize/2.0 ,face->eyes.yLeft - eyeSize/2.0,eyeSize,eyeSize);
		}
		if(face->eyes.rightEye){
			foundFace.rightEye.set(face->eyes.xRight - eyeSize/2.0,face->eyes.yRight - eyeSize/2.0,eyeSize,eyeSize);
		}

		/*if(firstIteration){
			firstIteration = false;
		}else{
			foundFace.rect = filterRectangle(foundFace.rect,foundFaces[i].rect,0.2);
			if(face->eyes.leftEye){
				foundFace.leftEye = filterRectangle(foundFace.leftEye,foundFaces[i].leftEye,0.4);
			}
			if(face->eyes.rightEye){
				foundFace.rightEye = filterRectangle(foundFace.rightEye,foundFaces[i].rightEye,0.4);
			}
		}*/

		foundFace.leftEyeCenter = ofVec2f(foundFace.leftEye.x + foundFace.leftEye.width*0.5, foundFace.leftEye.y + foundFace.leftEye.height*0.5);
		foundFace.rightEyeCenter = ofVec2f(foundFace.rightEye.x + foundFace.rightEye.width*0.5, foundFace.rightEye.y + foundFace.rightEye.height*0.5);
		foundFace.angle = ofRadToDeg((foundFace.leftEyeCenter.y - foundFace.rightEyeCenter.y) / (foundFace.leftEyeCenter.x - foundFace.rightEyeCenter.x));
		foundFace.center = ofVec2f(foundFace.rect.x + foundFace.rect.width*0.5, foundFace.rect.y + foundFace.rect.height*0.5);

		if(!firstIteration && isOutlier(foundFace,foundFaces[i])){
			ofLogVerbose(LOG_NAME) << "outlier";
			faces.clear();
			return;
		}

		foundFaces[i] = foundFace;
		i++;
		faces.pop_front();
	}else{
		firstIteration = true;
	}
	faces.clear();
}

vector<ofxMPT::Face> ofxMPT::getFaces(){
	return foundFaces;
}
