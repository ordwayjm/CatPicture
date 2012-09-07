/*
	@file CatPictureApp.cpp
	CSE 274 - Fall 2012
	HW1 - CatPicture
	
	@author Justin Ordway
	@date 9/5/2012

	This project satisfies requirements A1, A2, A4, A6, B1, and E6.

	Controls:
		r - selects rectangle as draw object
		c - selects circle as draw object
		g - clears screen with a gradient
		t - tints objects on screen
		b - blurs objects on screen
		s - saves current screen
*/

// I tried to add additional directories as shown in class the other day 
// and after doing so the code would still not run on my computer. 
// I will make suggestions under the assumption they are correct, sorry for any inconvenience. 

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

		// Satisfies goal (A.1)
		void rectangle(uint8_t* pixelArray, int x, int y, int width, int height, Color8u fill);
		
		// Satisfies goal (A.2)
		void circle(uint8_t* pixelArray, int originX, int originY, int radius, Color8u fill);
		
		// Satisfies goal (A.6)
		void tint(uint8_t* pixelArray, Color8u fill);
		
		// Satisfies goal (A.4)
		void gradient(uint8_t* pixelArray);
		
		// Satisfies goal (B.1)
		void blur(uint8_t* pixelArray);
};

// The one thing I would suggest when doing your javadoc code is to include:
/*
	Draws a rectangle of width, height with color fill on the surface at indicated x, y position
	@param pixelArray the array of pixels
	@param x1 the top left x coordinate
	@param y1 the top left y coordinate
	@param width the width of the rectangle
	@param height the height of the rectangle
	@param fill fills in the rectangle
*/
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

/*
	Draws a circle with a given radius with color fill on the surface at indicated x, y position
	@param pixelArray the array of pixels
	@param originX the x coordinate of the origin
	@param originY the y coordinate of the origin
	@param radius the radius of the circle
	@fill fill in the circle
*/
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

/*
	Clears the surface with a green gradient from top to bottom, called at the start of the program
	@param pixelArray the pixelArray
*/
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

/*
	Tints the screen using the given color, has some bugs when used with the gradient causing a swiping effect
	before the tint is applied. Technically a bug, but I thought it looked cool so I'll call it a feature.
	@param pixelArray the pixel array
	@param fill fill in the image
*/
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


/*
	Blurs the entire surface, if blur button is held for too long it will eventually turn the entire surface
	black
	@param pixelArray the array of pixels
*/
void CatPictureApp::blur(uint8_t* pixelArray)
{
	int rAvg;
	int gAvg;
	int bAvg;
	for(int y = 1; y < appHeight_; y++)
	{
		for(int x = 1;  x < appWidth_; x++)
		{
				// Calculate average RGB values for surrounding 9 pixels
				// Instead of having those long lines of code, I think this will be more reader friendly.
				rAvg = (pixelArray[3*(x + y * textureSize_)] + pixelArray[3*((x+1) + y * textureSize_)] + 
					pixelArray[3*((x+1) + (y+1) * textureSize_)] + pixelArray[3*((x) + (y+1) * textureSize_)] + 
					pixelArray[3*((x-1) + (y+1) * textureSize_)] + pixelArray[3*((x-1) + (y-1) * textureSize_)] 
				+ pixelArray[3*((x) + (y-1) * textureSize_)])/9;

				gAvg = (pixelArray[3*(x + y * textureSize_)+1] + pixelArray[3*((x+1) + y * textureSize_)+1] + 
					pixelArray[3*((x+1) + (y+1) * textureSize_)+1] + pixelArray[3*((x) + (y+1) * textureSize_)+1] + 
					pixelArray[3*((x-1) + (y+1) * textureSize_)+1] + pixelArray[3*((x-1) + (y-1) * textureSize_)+1] 
				+ pixelArray[3*((x) + (y-1) * textureSize_)+1])/9;

				bAvg = (pixelArray[3*(x + y * textureSize_)+2] + pixelArray[3*((x+1) + y * textureSize_)+2] + 
					pixelArray[3*((x+1) + (y+1) * textureSize_)+2] + pixelArray[3*((x) + (y+1) * textureSize_)+2] + 
					pixelArray[3*((x-1) + (y+1) * textureSize_)+2] + pixelArray[3*((x-1) + (y-1) * textureSize_)+2] 
				+ pixelArray[3*((x) + (y-1) * textureSize_)+2])/9;
				
				pixelArray[3*(x + y * textureSize_)] = rAvg;
				pixelArray[3*(x + y * textureSize_)+1] = gAvg;
				pixelArray[3*(x + y * textureSize_)+2] = bAvg;
		}
	}
}

void CatPictureApp::setup()
{
	surface_ = new Surface(textureSize_, textureSize_, false);
	pixels_ = (*surface_).getData();
	gradient(pixels_); // start with gradient background
	drawType_ = 'r'; // start drawing rectangles
}

/*
	Gets keyboard input from the user to determine what to draw
*/
void CatPictureApp::keyDown(KeyEvent event) {
	drawType_ = event.getChar();	
}

/*
	Gets the mouse position and if the left button is down to determine where to place a shape
*/
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
			int width = 50;
			int height = 50;

			rectangle(pixels_, xValue, yValue, width, height, fill);
		}
		if(drawType_ == 'c')
		{
			int radius = 50;

			circle(pixels_, xValue, yValue, radius, fill);
		}
		mouseDown_ = false;
	}
	if(drawType_ == 't')
	{
		tint(pixels_, Color8u(0, 255, 0));
		drawType_ = 'r';
	}
	if(drawType_ == 'g')
	{
		gradient(pixels_);
		drawType_ = 'r';
	}
	if(drawType_ == 'b')
	{
		blur(pixels_);
		drawType_ = 'r';
	}
	if(drawType_ == 's')
	{
		writeImage("ordwayjm.png", *surface_);
	}
}

void CatPictureApp::draw()
{
	gl::draw(*surface_); 
}

CINDER_APP_BASIC(CatPictureApp, RendererGl)