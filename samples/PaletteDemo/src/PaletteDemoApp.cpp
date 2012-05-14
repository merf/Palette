#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;




class PaletteDemoAppApp : public AppBasic 
{
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	CPalette* mp_Palette;
};

void PaletteDemoAppApp::setup()
{
	mp_Palette = new CPalette("test.aco");
}

void PaletteDemoAppApp::mouseDown( MouseEvent event )
{
}

void PaletteDemoAppApp::update()
{
}

void PaletteDemoAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	int num_cols = mp_Palette->getSize();

	float width = getWindowWidth() / (float)num_cols;

	for(size_t i=0; i<num_cols; ++i)
	{
		gl::color(mp_Palette->getColor(i));
		gl::drawSolidRect(Rectf(i * width, 0, (i + 1) * width, getWindowHeight()));
	}
}

CINDER_APP_BASIC( PaletteDemoAppApp, RendererGl )
