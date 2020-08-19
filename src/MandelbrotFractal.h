#pragma once
#include <vector>
#include <algorithm>

// Todo
// Fix changing resolution
// More presets with names
// Fix README file, take nice screenshots
// Investigate double precision

const float MANDELBROT_INITIAL_X = -2.25;
const float MANDELBROT_INITIAL_Y = -1.5;

struct Color
{
	float color[3];
	int iterations;
};

enum ColoringType {LINEAR, EXPONENTIAL, CIRCLE};

struct FractalColoring //Represents a coloring scheme for mandelbrot
{
	ColoringType type = EXPONENTIAL;
	//For interpolation type
	float exponent = 0.98;
	float iterationLoop = 30;
	std::vector<Color> colors;
	std::vector<float> colorMap; //Vector which maps iteration to color, is sent as texture to compute shader

	FractalColoring()
	{

	}
	FractalColoring(std::vector<Color>& colors, float exponent = 0.98)
	{
		this->colors = colors;
		this->iterationLoop = colors[colors.size()-1].iterations;
		this->exponent = exponent;
		calculateColorMap();
	}

	void calculateColorMap() //Generate a color map for the iterations based on the vector of colors
	{
		//X -> Y -> Z -> X ...
		colorMap = std::vector<float>(iterationLoop*4);
		int i = 0;
		for (size_t c = 0; c < colors.size() && i < iterationLoop; c++) //Go through all colors
		{
			while (i < colors[c + 1].iterations)
			{
				int deltaI = colors[c + 1].iterations - colors[c].iterations;
				float t = float(i) / deltaI;; //Linear interpolation
				if (type == EXPONENTIAL) //Apply t [0, 1] to an exponential function determined by the variable exponent
					t = (std::pow(exponent, t) - 1) / (exponent - 1);

				//Interpolate colors
				colorMap[i*4 + 0] = lerp(colors[c].color[0], colors[c+1].color[0], t); 
				colorMap[i*4 + 1] = lerp(colors[c].color[1], colors[c+1].color[1], t); 
				colorMap[i*4 + 2] = lerp(colors[c].color[2], colors[c+1].color[2], t); 
				colorMap[i*4 + 3] = 1;
				i++;
			}
		}
	}
private:
	float lerp(float v0, float v1, float t) {
		return v0 + t * (v1 - v0);
	}
};


class MandelbrotFractal
{
public:
	double zoom = 1.0f;

	double viewX = MANDELBROT_INITIAL_X;
	double viewY = MANDELBROT_INITIAL_Y;
    double viewWidth = 3;
    double viewHeight = 3;

	int iterations = 150;
	
	std::vector<FractalColoring> presets;
	int currentPreset = 0;

	void zoomByFactor(float zoomFactor, float x, float y)
	{
		zoom *= zoomFactor;
		//Zoom viewport adjustment by delta * 0.5f, to adjust for center zooming
		double zoomChangeWidth = ((zoom / zoomFactor) * viewWidth - zoom * viewWidth) * 0.5f;
		double zoomChangeHeight = ((zoom / zoomFactor) * viewHeight - zoom * viewHeight) * 0.5f;
		viewX = viewX + x * zoom + zoomChangeWidth;
		viewY = viewY + y * zoom + zoomChangeHeight;
	}
    double getViewX(float cameraRelativeX)
    {
        return viewX + cameraRelativeX * zoom;
    }
    double getViewY(float cameraRelativeY)
    {
        return viewY + cameraRelativeY * zoom;
    }

	void switchPreset(int increment)
	{
		currentPreset = (currentPreset + increment) % presets.size();
	}

	FractalColoring& getColoring()
	{
		return presets[currentPreset];
	}

	MandelbrotFractal()
	{
		//Setup presets
		//Midnight blue: dark blue, white, orange
		std::vector<Color> midnightBlue = {
			Color {1/255.0f, 9/255.0f, 108/255.0f, 0},
			Color {1.0, 1.0, 1.0, 10},
			Color {234/255.0f, 192/255.0f, 110/255.0f, 20},
			Color {0.0, 5/255.0, 96/255.0, 30}};
		presets.push_back(FractalColoring(midnightBlue));

		std::vector<Color> volcano = {
			Color {0.0f, 0.0f, 0.0f, 0},
			Color {191/255.0f, 14/255.0f, 6/255.0f, 5},
			Color {253/255.0f, 181/255.0f, 0.0f, 10},
			Color {0.0f, 0.0f, 0.0f, 30}};
		presets.push_back(FractalColoring(volcano));
	}
};