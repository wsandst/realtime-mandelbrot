
const float MANDELBROT_INITIAL_X = -2.25;
const float MANDELBROT_INITIAL_Y = -1.5;

struct MandelbrotFractal
{
	double zoom = 1.0f;
	double posX = MANDELBROT_INITIAL_X;
	double posY = MANDELBROT_INITIAL_Y;
	int iterations = 50;
	void zoomByFactor(float zoomFactor, float x, float y)
	{
		zoom *= zoomFactor;
		double zoomChange = (zoom / zoomFactor) * 1.5 - zoom * 1.5; //Zoom width delta / 2, to adjust for center zooming
		posX = posX + x * zoom + zoomChange;
		posY = posY + y * zoom + zoomChange;
	}
};
