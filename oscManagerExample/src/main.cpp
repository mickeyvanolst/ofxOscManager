#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofSetupOpenGL(new ofAppGlutWindow(), 800,600, OF_WINDOW);
	ofRunApp(new testApp());
}
