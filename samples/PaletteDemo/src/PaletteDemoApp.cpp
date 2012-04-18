#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class CPalette
{
public:
	CPalette(const std::string& palette_path);

	size_t	getSize() { return mColors.size(); }
	ColorA	getColor(size_t i) { return mColors[i].second; }

private:
	bool CreateFromACO(uint16_t* pp_data);

	std::vector<std::pair<std::wstring, ColorA> >	mColors;
};

CPalette::CPalette(const std::string& palette_path)
{
	DataSourceRef data_source = loadAsset(palette_path);

	if(data_source)
	{
		Buffer buf = data_source->getBuffer();
		uint16_t* p_data = reinterpret_cast<uint16_t*>(buf.getData());

#ifdef WIN32
		//endian swap the entire buffer first on windows as it makes things a lot simpler later on
		int data_size = buf.getDataSize();
		for(int i=0; i<data_size/2; ++i)
		{
			p_data[i] = swapEndian(p_data[i]);
		}
#endif

		if(!CreateFromACO(p_data))
		{
			//In case of failure do something...
		}
	}
}

void ReadNextValue(uint16_t** pp_data, uint16_t& next_value)
{
	next_value = (*pp_data)[0];

	(*pp_data)++;
}

//For description of the ACO format see here - http://www.nomodes.com/aco.html
bool CPalette::CreateFromACO(uint16_t* p_data)
{
	bool success = true;

	//1) Read Version Number
	uint16_t version;
	ReadNextValue(&p_data, version);

	if(version == 1 || version == 2)
	{
		//2) Read Number of Colors
		uint16_t num_colors = 0;
		ReadNextValue(&p_data, num_colors);

		mColors.reserve(num_colors);

		//3) Read the color values
		for(uint16_t curr_color = 0; curr_color < num_colors; ++curr_color)
		{
			uint16_t colour_space, x, y, z, w;
			ReadNextValue(&p_data, colour_space);
			ReadNextValue(&p_data, x);
			ReadNextValue(&p_data, y);
			ReadNextValue(&p_data, z);
			ReadNextValue(&p_data, w);

			ColorA c;

			switch(colour_space)
			{
			case 0:
				c = ColorA(x / 65535.0f, y / 65535.0f, z / 65535.0f, 1.0f);
				break;
			case 1:
				c = hsvToRGB(Vec3f(x / 65535.0f, y / 65535.0f, z / 65535.0f));
				break;
			default:
				//fail...
				break;
			}

			if(version == 1)
			{
				mColors.push_back(std::make_pair<std::wstring, ColorA>(L"", c));
			}
			else if(version == 2)
			{
				p_data++;

				uint16_t name_length = 0;
				ReadNextValue(&p_data, name_length);

				std::wstring color_name = reinterpret_cast<const wchar_t*>(p_data);
				p_data += name_length;

				mColors.push_back(std::make_pair<std::wstring, ColorA>(color_name, c));
			}
		}

		//If we have been reading a version 1 palette there may now follow a version 2 palette with color names - lets try to read that...
		if(version == 1)
		{
			CreateFromACO(p_data);
		}
	}
	else
	{
		//unsupported version...
		success = false;
	}

	return success;
}


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
