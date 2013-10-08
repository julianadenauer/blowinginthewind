#include "ofMain.h"
#include "testApp.h"


#define ENABLE_OPENGL

#ifdef ENABLE_OPENGL
#include "ofAppGlutWindow.h"
#endif

//========================================================================
int main( ){
    
#ifdef ENABLE_OPENGL
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,800, OF_WINDOW);			// <-------- setup the GL context
	ofRunApp( new testApp());

#else
    testApp *app = new testApp();
	app->setup();
	while(1)
        app->update();
#endif

}
