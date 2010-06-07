#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( )
{
	try
	{
		ofAppGlutWindow window;
		ofSetupOpenGL(&window, 640*3,480*2+30, OF_WINDOW);			// <-------- setup the GL context

		// this kicks off the running of my app
		// can be OF_WINDOW or OF_FULLSCREEN
		// pass in width and height too:
		ofRunApp( new testApp());
	}
	catch(Poco::Exception& e)
	{
		//cout << "error : " << e.displayText.c_str() << "\n";
		
		std::string what = e.displayText();
		printf("EXCEPTION: %s\n", what.c_str());
		//printf("  name: %s\n", e.name);
		//printf("  className: %s\n", e.className);
	}
}