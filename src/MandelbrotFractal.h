#pragma once
#include <vector>
#include <algorithm>

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
	float iterationLoop = 45;
	std::vector<Color> colors;
	std::vector<float> colorMap; //Vector which maps iteration to color, is sent as texture to compute shader

	FractalColoring()
	{
		colors.push_back(Color {1.0, 1.0, 1.0, 0});
		colors.push_back(Color {1.0, 0.0, 0.0, 15});
		colors.push_back(Color {1.0, 0.0, 0.0, 30});
		colors.push_back(Color {0.0, 0.0, 1.0, 45});
		calculateColorMap();
	}
	FractalColoring(std::vector<Color>& colors, int iterationLoop, float exponent = 0.98)
	{
		this->colors = colors;
		this->iterationLoop = iterationLoop;
		this->exponent = exponent;
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
				colorMap[i*4 + 0] = 1;
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

	int iterations = 50;

	FractalColoring color;
	
	void zoomByFactor(float zoomFactor, float x, float y)
	{
		zoom *= zoomFactor;
		double zoomChange = (zoom / zoomFactor) * 1.5 - zoom * 1.5; //Zoom width delta / 2, to adjust for center zooming
		viewX = viewX + x * zoom + zoomChange;
		viewY = viewY + y * zoom + zoomChange;
	}
    double getViewX(float cameraRelativeX)
    {
        return viewX + cameraRelativeX * zoom;
    }
    double getViewY(float cameraRelativeY)
    {
        return viewY + cameraRelativeY * zoom;
    }
};
