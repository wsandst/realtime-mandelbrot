
const float MANDELBROT_INITIAL_X = -2.25;
const float MANDELBROT_INITIAL_Y = -1.5;

struct MandelbrotFractal
{
	double zoom = 1.0f;

	double viewX = MANDELBROT_INITIAL_X;
	double viewY = MANDELBROT_INITIAL_Y;
    double viewWidth = 3;
    double viewHeight = 3;

	int iterations = 50;
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
