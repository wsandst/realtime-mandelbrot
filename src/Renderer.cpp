#include "Renderer.h"
#define GLEW_STATIC
#include "GL/glew.h"

Renderer::Renderer(int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::updateDeltatime()
{
	float currentFrame = SDL_GetTicks();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if (currentFrame - lastFPSframe > 1000)
	{
		fps = 1000 / deltaTime;
		lastFPSframe = currentFrame;
	}
}

void Renderer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.calculateProjectionMatrix();
	camera.calculateViewMatrix();

	draw();

	if (displayText)
		drawText();

	SDL_GL_SwapWindow(window);
}

void Renderer::draw()
{
	geometryShader.use();

	for (auto& it : geometryVBOs) { //Draw chunks
		glm::mat4 matrix = glm::mat4(camera.getProjectionMatrix() * camera.getViewMatrix() * it.translation);

		glBindTexture(GL_TEXTURE_2D, texture);
		geometryShader.setMat4("matrix", matrix);
		glBindVertexArray(it.VAO);
		glDrawArrays(GL_TRIANGLES, 0, it.size);
	}
}

void Renderer::init()
{
	camera = Camera(windowWidth, windowHeight);

	initSDL();
	initOpenGL();
	initFreetype();

	initGeometry();

}

void Renderer::initOpenGL()
{
	glewExperimental = GL_TRUE;
	glewInit();
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	//Wireframes
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO), 0) {
		std::cout << "SDL could not initialize! SDL_Error:" << SDL_GetError() << std::endl;
	}

	//Get monitor resolution for fullscreen toggling
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	screenResHeight = DM.h;
	screenResWidth = DM.w;

	//Antialiasing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MSAALevel*MSAALevel);
	glEnable(GL_MULTISAMPLE);

	window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	glContext = SDL_GL_CreateContext(window);

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetSwapInterval(1);
}

///@brief Initializes the freetype library and loads a font into a texture atlas with the help of the class TextHandler, later used for text rendering
void Renderer::initFreetype()
{
	if (FT_Init_FreeType(&freetype))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(freetype, "assets/fonts/OpenSans-Regular.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
	else
		std::cout << "Loaded OpenSans font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 24);  
  
	int w = 0;
	unsigned int h = 0;

	//Figure out width and height of the character texture atlas
	for(int i = 0; i < 128; i++) {
		if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			std::cout << "Loading character "<< i << " failed" << std::endl;
			continue;
		}

		w += face->glyph->bitmap.width;
		h = std::max(h, face->glyph->bitmap.rows);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	std::cout << w << "," << h << std::endl;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textTexture);
	glBindTexture(GL_TEXTURE_2D, textTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	textHandler.textAtlasWidth = w;
	textHandler.textAtlasHeight = h;

	int x = 0;

	float halfPixelAdjustment = 1.0 / (textHandler.textAtlasWidth * 2);

	for(int c = 0; c < 128; c++) {
		if(FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		Character character = {
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x >> 6,
			float(x) / w + halfPixelAdjustment
		};

		textHandler.characters[c] = character;

		x += face->glyph->bitmap.width;
		}
	
	FT_Done_Face(face);
	FT_Done_FreeType(freetype);
}

void Renderer::initGeometry()
{
	geometryShader = Shader("shaders/geometry.vert", "shaders/geometry.frag");
	textShader = Shader("shaders/text.vert", "shaders/text.frag");
}

void Renderer::requestShaderReload()
{
	geometryShader.reload();
}

void Renderer::loadVBOs(std::vector<Mesh>& meshes)
{
	/*for (auto &mesh : meshes)
	{
		geometryVBOs.push_back(GeometryVBO(mesh.pos, mesh.vertices));
	}*/

	geometryVBOs.push_back(GeometryVBO(meshes[0].pos, meshes[0].vertices));

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, meshes[0].texture.width, meshes[0].texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, meshes[0].texture.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(meshes[0].texture.data);

	updateTextVBO(true);

}

void Renderer::toggleFullscreen() {

	isFullscreen = !isFullscreen;

	if (isFullscreen)
	{
		minimizedWidth = windowWidth;
		minimizedHeight = windowHeight;
		SDL_SetWindowSize(window, screenResWidth, screenResHeight);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		windowWidth = screenResWidth;
		windowHeight = screenResHeight;

	}
	else
	{
		SDL_SetWindowFullscreen(window, 0);
		SDL_SetWindowSize(window, minimizedWidth, minimizedHeight);
		windowWidth = minimizedWidth;
		windowHeight = minimizedHeight;
	}

	updateResolution();
}

void Renderer::updateResolution()
{

	glViewport(0, 0, windowWidth, windowHeight);

	camera.windowWidth = windowWidth;
	camera.windowHeight = windowHeight;

	render();

}

void Renderer::resizeWindow(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	updateResolution();
}

void Renderer::centerWindow()
{
	int newScreenPosX = screenResWidth / 2 - windowWidth / 2;
	int newScreenPosY = screenResHeight / 2 - windowHeight / 2;
	SDL_SetWindowPosition(window, newScreenPosX, newScreenPosY);
}

///@brief Add a new text object at the specified positon and scale
void Renderer::addText(std::string text, float x, float y, float scale)
{
	std::vector<TextVertexAttrib> attribs = textHandler.createTextVertices(text, x, y, scale);
	TextObject textObject = TextObject(textObjects.size(), x, y, scale, text, attribs);
	textObjects.push_back(textObject);
}

///@brief Update an existing text object based on index, ie order added. Pos overload
void Renderer::updateText(int id, std::string text)
{
	TextObject& textObject = textObjects[id];
	textObject.vertices = textHandler.createTextVertices(text, textObject.x, textObject.y, textObject.scale);

	//For several text updating every frame, do this at the end, not at every update
	updateTextVBO();
}

///@brief Same as above. Pos overload
void Renderer::updateText(int id, std::string text, float x, float y)
{
	TextObject& textObject = textObjects[id];
	textObject.x = x;
	textObject.y = y;
	textObject.vertices = textHandler.createTextVertices(text, textObject.x, textObject.y, textObject.scale);
}

///@brief Update the text vertex buffer object with new data
void Renderer::updateTextVBO(bool create)
{
	//Update text vertex buffer data
	std::vector<TextVertexAttrib> vertices; 

	for (size_t i = 0; i < textObjects.size(); i++)
	{
		TextObject& textObject = textObjects[i];
		
		vertices.insert(vertices.end(), textObject.vertices.begin(), textObject.vertices.end());
	}
	if (create)
		textVBO.create(vertices);
	else
		textVBO.update(vertices);

	
}

void Renderer::updateRealtimeTextContent()
{
	if (displayText)
	{
	//FPS counter
	updateText(1, std::to_string(int(fps)), camera.windowWidth-50, camera.windowHeight-40);
	if (displayDebugInfo)
	{
		//Coordinate display
		std::string coordsString = "x: " + std::to_string(camera.getPosition().x) + "\ny: " + std::to_string(camera.getPosition().y) + "\nz: " + std::to_string(camera.getPosition().z);
		updateText(2, coordsString, 20, camera.windowHeight-30);
	}

	updateTextVBO();
	}
}

///@brief Draw text based on the mostly static TextVBO.
void Renderer::drawText()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glDisable(GL_DEPTH_TEST); 
	
	// activate corresponding render state	
	textShader.use();
	textShader.setVec3("textColor", textColor);
	glm::mat4 projection = glm::ortho(0.0f, float(camera.windowWidth), 0.0f, float(camera.windowHeight));
	textShader.setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVBO.VAO);

	glBindTexture(GL_TEXTURE_2D, textTexture);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO.VBO);

	glDrawArrays(GL_TRIANGLES, 0, textVBO.size);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::close()
{
	SDL_Quit();
}
