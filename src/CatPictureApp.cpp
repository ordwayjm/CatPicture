#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
		void setup();
		void keyDown(KeyEvent event);
		void mouseDown(MouseEvent event);	
		void update();
		void draw();
  private:
		static const int textureSize_ = 2048;
		static const int appWidth_ = 800;
		static const int appHeight_ = 600;
		
		Surface* surface_;
		uint8_t* pixels_;

		int mouseX_;
		int mouseY_;
		bool mouseDown_;
		
		char drawType_;
		char drawSize_;

		void rectangle(uint8_t* pixelArray, int x, int y, int width, int height, Color8u fill);
		void tint(uint8_t* pixelArray, Color8u fill);
		void circle(uint8_t* pixelArray, int originX, int originY, int radius, Color8u fill);
		void gradient(uint8_t* pixelArray);
};

void CatPictureApp::rectangle(uint8_t* pixelArray, int x1, int y1, int width, int height, Color8u fill)
{
	for(int y = y1; y < y1 + height; y++)
	{
		for(int x = x1; x < x1 + width; x++)
		{
			if(x <= 0 || y <= 0 || x >= appWidth_ || y >= appHeight_) continue;
			pixelArray[3*(x + y * textureSize_)] = fill.r;
			pixelArray[3*(x + y * textureSize_)+1] = fill.g;
			pixelArray[3*(x + y * textureSize_)+2] = fill.b;
		}
	}
}

void CatPictureApp::tint(uint8_t* pixelArray, Color8u fill)
{
	for(int y = 0; y < textureSize_; y++)
	{
		for(int x = 0; x < textureSize_; x++)
		{
			if(x <= 0 || y <= 0 || x >= appWidth_ || y >= appHeight_) continue;
			pixelArray[3*(x + y * textureSize_)] += fill.r;
			pixelArray[3*(x + y * textureSize_)+1] += fill.g;
			pixelArray[3*(x + y * textureSize_)+2] += fill.b;
		}
	}

}

void CatPictureApp::circle(uint8_t* pixelArray, int originX, int originY, int radius, Color8u fill)
{
	int distance;
	for(int y = originY - radius; y <= originY + radius; y++)
	{
		for(int x = originX - radius; x <= originX + radius; x++)
		{
			distance = (int)sqrt( (double)((x - originX)*(x - originX)) + ((y - originY)*(y - originY)) );
			if(distance < radius)
			{
				if(x <= 0 || y <= 0 || x >= appWidth_ || y >= appHeight_) continue; 
				pixelArray[3*(x + y * textureSize_)] = fill.r;
				pixelArray[3*(x + y * textureSize_)+1] = fill.g;
				pixelArray[3*(x + y * textureSize_)+2] = fill.b;
			}
		}
	}

}

void CatPictureApp::gradient(uint8_t* pixelArray)
{
	Color8u color = Color8u(0, 10, 0);
	int inc;
	for(int y = 0; y < appHeight_; y++)
	{
		for(int x = 0; x < appWidth_; x++)
		{
			inc = (int)((256 * y) / appHeight_);
			pixelArray[3*(x + y * textureSize_)] = color.r + inc;
			pixelArray[3*(x + y * textureSize_)+1] = color.g + inc;
			pixelArray[3*(x + y * textureSize_)+2] = color.b + inc;
		}
	}
}

void CatPictureApp::setup()
{
	surface_ = new Surface(textureSize_, textureSize_, false);
	pixels_ = (*surface_).getData();
	gradient(pixels_);
	drawType_ = 'r';
}

void CatPictureApp::keyDown(KeyEvent event) {
	drawType_ = event.getChar();	
}

void CatPictureApp::mouseDown(MouseEvent event)
{
	mouseDown_ = event.isLeftDown();
	mouseX_ = event.getX();
	mouseY_ = event.getY();
}

void CatPictureApp::update()
{	
	pixels_ = (*surface_).getData();
	if(mouseDown_ == true) 
	{
		int xValue = mouseX_;
		int yValue = mouseY_;
		// Set a random color fill for shapes
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		Color8u fill = Color8u(r, g, b);
		if(drawType_ == 'r')
		{
			int width = 100;
			int height = 100;

			rectangle(pixels_, xValue, yValue, width, height, fill);
		}
		if(drawType_ == 'c')
		{
			int radius = 100;

			circle(pixels_, xValue, yValue, radius, fill);
		}
		mouseDown_ = false;
	}
	if(drawType_ == 't')
	{
		tint(pixels_, Color8u(0, 0, 255));
		drawType_ = 'r';
	}
	if(drawType_ == 'g')
	{
		gradient(pixels_);
		drawType_ = 'r';
	}
}

void CatPictureApp::draw()
{
	gl::draw(*surface_); 
}

CINDER_APP_BASIC(CatPictureApp, RendererGl)