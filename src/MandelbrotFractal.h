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

enum ColoringType {INTERPOLATE, CIRCLE};

struct FractalColoring //Represents a coloring scheme for mandelbrot
{
	ColoringType type = INTERPOLATE;
	//For interpolation type
	float exponent = 0.98;
	float iterationLoop = 10;
	std::vector<Color> colors;
	std::vector<char> colorMap; //Vector which maps iteration to color, is sent as texture to compute shader
	FractalColoring()
	{

	}
	FractalColoring(std::vector<Color>& colors, int iterationLoop, float exponent = 0.98)
	{
		this->colors = colors;
		this->iterationLoop = iterationLoop;
		this->exponent = exponent;
	}
	void calculateColorMap()
	{

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
