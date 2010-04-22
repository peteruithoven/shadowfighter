/*
 *  Blob.h
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 4/22/10.
 */

#ifndef _Blob
#define _Blob

#include <ofMain.h>
#include <ofxCvBlob.h>

class Blob : public ofxCvBlob
{
	public:
		int color;
		vector <ofRectangle> rectHistory;
	
		Blob();
	
		int operator<(const Blob * blob) const;
		int operator>(const Blob * blob) const;
		int operator<(const Blob blob) const;
		int operator>(const Blob blob) const;
	
		int operator<(Blob * blob);
		int operator>(Blob * blob);
		int operator<(Blob blob);
		int operator>(Blob blob);
};

#endif
