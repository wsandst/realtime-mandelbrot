#include "InputHandler.h"

int count = 0;

///@brief Input function, handles all input and the various actions connected to them
void InputHandler::handleInput(float deltaTime)
{
	//Measure performance in debug mode
	TimePoint beginTime;
	if (renderer.displayDebugInfo)
		beginTime = std::chrono::high_resolution_clock::now();

	camera.cameraStep = 0.05f * deltaTime;
	float xOffset, yOffset;

	//Check for inputs
	while (SDL_PollEvent(&sdlEvent)) {
		if (sdlEvent.key.repeat == 0) //Don't listen for repeat events here
		{
		switch (sdlEvent.type) {
		case SDL_KEYDOWN: //Key down event
			switch (sdlEvent.key.keysym.sym) {

			case SDLK_w: //Move forward
				camera.keyForward = true;
				break;
			case SDLK_s: //Move back
				camera.keyBackward = true;
				break;
			case SDLK_a: //Move left
				camera.keyLeft = true;
				break;
			case SDLK_d: //Move right
				camera.keyRight = true;
				break;
			case SDLK_m: //Switch presents, increment
				renderer.mandelbrot.switchPreset(1);
				renderer.updateMandelbrotColormap();
				break;
			case SDLK_n: //Switch presets, decrement
				renderer.mandelbrot.switchPreset(-1);
				renderer.updateMandelbrotColormap();
				break;
			case SDLK_q: //Decrease iterations
				if (renderer.mandelbrot.iterations == 0)
					renderer.mandelbrot.iterations = 1;
				renderer.mandelbrot.iterations *= 2;
				break;
			case SDLK_e: //Increase iterations
				renderer.mandelbrot.iterations /= 2;
				break;
			case SDLK_o: //Decrease color exponent
				renderer.mandelbrot.getColoring().exponent /= 1.25;
				renderer.updateMandelbrotColormap();
				break;
			case SDLK_p: //Increase color exponent
				renderer.mandelbrot.getColoring().exponent *= 1.25;
				renderer.updateMandelbrotColormap();
				break;
			case SDLK_u: //Decrease iteration loop
				renderer.mandelbrot.getColoring().iterationLoop /= 1.25;
				renderer.updateMandelbrotColormap();
				break;
			case SDLK_i: //Increase color exponent
				renderer.mandelbrot.getColoring().iterationLoop *= 1.25;
				renderer.updateMandelbrotColormap();
				break;
			case SDLK_SPACE: //Move up
				camera.keyUp = true;
				break;
			case SDLK_LSHIFT: //Move down
				camera.keyDown = true;
				break;

			case SDLK_z: //Change max speed by 2x
				camera.changeMaxVelocity(2);
				std::cout << "Max speed: " << camera.maxVelocity << "\n";
				break;

			case SDLK_x: //Change max speed by 0.5x
				camera.changeMaxVelocity(0.5);
				std::cout << "Max speed: " << camera.maxVelocity << "\n";
				break;

			case SDLK_r: //Hot reload shaders
				std::cout << "Reloading Shaders" << "\n";
				renderer.requestShaderReload();
				break;

			case SDLK_F1: //Toggle text display
				std::cout << "F1: Text display toggled\n";
				renderer.displayText = !renderer.displayText;
				break;

			case SDLK_F2: //Take screenshot
				std::cout << "F2: Taking screenshot\n";
				renderer.screenshot();
				break;

			case SDLK_F3: //Toggle coordinate display
				std::cout << "F3: Debug display toggled\n";
				renderer.displayDebugInfo = !renderer.displayDebugInfo;
				renderer.textUpdateRequired	= true;
				F3Pressed = true;
				break;

			case SDLK_F4: //Toggle wireframes
				std::cout << "F4: Wireframes toggled\n";
				renderer.displayWireframes = !renderer.displayWireframes;
				break;

			case SDLK_F11: //Toggle fullscreen
				std::cout << "F11: Fullscreen toggled" << "\n";
				renderer.toggleFullscreen();
				break;

			case SDLK_ESCAPE: //Unfocus window
				windowContext = !windowContext;
				if (windowContext) {
					SDL_ShowCursor(0);
					SDL_SetRelativeMouseMode(SDL_TRUE);
					int relativeX, relativeY; //Skip the next relative mouse event, as the mouse has moved around during unfocus
					SDL_GetRelativeMouseState(&relativeX, &relativeY);
				}
				else {
					SDL_ShowCursor(1);
					SDL_SetRelativeMouseMode(SDL_FALSE);
				}
				break;
			case SDLK_c: //Exiting program
				exit = true;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (sdlEvent.key.keysym.sym) {
			case SDLK_w:
				camera.keyForward = false;
				break;
			case SDLK_s:
				camera.keyBackward = false;
				break;
			case SDLK_a:
				camera.keyLeft = false;
				break;
			case SDLK_d:
				camera.keyRight = false;
				break;
			case SDLK_SPACE:
				camera.keyUp = false;
				break;
			case SDLK_LSHIFT:
				camera.keyDown = false;
				break;
			case SDLK_F3:
				F3Pressed = false;
				break;
			}
		
		}
		}
		switch (sdlEvent.type) {
		case SDL_MOUSEMOTION:
			if (windowContext) //Mouse only moves camera when the window is in focus, ie the mouse is invisible
			{
				int relativeX, relativeY;
				SDL_GetRelativeMouseState(&relativeX, &relativeY);
				//std::cout << relativeX << " " << relativeY << "\n";

				//camera.updateView(relativeX * sensitivity, -relativeY * sensitivity);
			}
			break;
		case SDL_MOUSEWHEEL:
			if (sdlEvent.wheel.y == -1)
				renderer.mandelbrot.zoomByFactor(1.03, camera.getPosition().x, camera.getPosition().z);
			else if (sdlEvent.wheel.y == 1)
				renderer.mandelbrot.zoomByFactor(0.97, camera.getPosition().x, camera.getPosition().z);
			camera.setPosition(0, 0, 0);
			break;
		case SDL_WINDOWEVENT:
			switch (sdlEvent.window.event) {
			case SDL_WINDOWEVENT_RESIZED: //Resizing window
				renderer.resizeWindow(sdlEvent.window.data1, sdlEvent.window.data2);
				std::cout << "Window size changed to " << sdlEvent.window.data1 << "x" << sdlEvent.window.data2 << "\n";
				break;

			case SDL_WINDOWEVENT_CLOSE: //Exiting program
				std::cout << "Exiting program" << "\n";
				exit = true;
				break;
			}
			break;
		}
	}

	camera.move();

	if (renderer.displayDebugInfo && !F3Pressed)
		inputPerformanceMs = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - beginTime ).count() / 1000.0f;
}

InputHandler::InputHandler(Renderer& _renderer, Camera& _camera) : camera(_camera), renderer(_renderer)
{
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

InputHandler::~InputHandler()
{
}
