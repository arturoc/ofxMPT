/*
 * lefteye_largest_face_centered.cc
 *
 *  Copyright (c) 2002 Machine Perception Laboratory 
 *  University of California San Diego.
 * 
 * Please read the disclaimer and notes about redistribution 
 * at the end of this file.
 *
 */

#include "lefteye_largest_face_centered.h"

void lefteye_largest_face_centered::assignData(MPEyeFinder *ef, FeatureData &d){
	d.patchsize=25;
	d.patch_width=25;
	d.patch_height=25;
	d.plus_minus_one=0;
	d.numfeatures=100;
	d.numcascades=1;
	d.cascades = casc;
	d.features = f;
	d.normOffset.top = 0;
	d.normOffset.left = 0;
	d.normOffset.right = 0;
	d.normOffset.bottom = 0;
	d.numStdAdjusts=50;
	d.stdAdjusts = stdAdj;
	d.real_fun = true;
	d.nl=64; //// number of bins 
	ef->SetCentering(face_centered);
	ef->SetRez(largest);
}
