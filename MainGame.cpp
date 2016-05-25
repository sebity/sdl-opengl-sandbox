/*
 * MainGame.cpp
 *
 *  Created on: 22 May 2016
 *      Author: jan
 */

#include "MainGame.h"
#include <iostream>
#include <typeinfo>
#include "../JTEngine/Errors.h"
#include "../JTEngine/Sprite.h"


// Constructor, initialises private member variables.
//
MainGame::MainGame() :
	_window(nullptr),
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_fps(0.0f),
	_maxFPS(60.0f),
	_frameTime(0.0f),
	_time(0.0f) {

}

// Destructor.
//
MainGame::~MainGame() {

}


// This function runs the game.
//
void MainGame::run() {
	initSystems();

	_sprites.push_back(new Sprite());
	_sprites.back()->init(-1.0f, -1.0f, 1.0f, 1.0f, "Textures/JimmyJumpPack/CharacterRight_Standing.png");

	_sprites.push_back(new Sprite());
	_sprites.back()->init(0.0f, -1.0f, 1.0f, 1.0f, "Textures/JimmyJumpPack/CharacterRight_Standing.png");

	for (int i = 0; i < 1000; i++) {
		_sprites.push_back(new Sprite());
		_sprites.back()->init(-1.0f, 0.0f, 1.0f, 1.0f, "Textures/JimmyJumpPack/CharacterRight_Standing.png");
	}

	gameLoop();
}


// Initialises SDL and OpenGL as well as whatever else is needed.
//
void MainGame::initSystems() {
	// Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	// Enable double buffering to stop any flickering on the screen.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Open an SDL window
	_window = SDL_CreateWindow("Graphics Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			_screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
	if(_window == nullptr) {
		fatalError("SDL Window could not be created!");
	}

	// Setup the OpenGL context
	SDL_GLContext glContext = SDL_GL_CreateContext(_window);
	if(glContext == nullptr) {
		fatalError("SDL_GL context could not be created!");
	}

	// Setup the GLEW
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		fatalError("Could not initialise glew!");
	}

	// Check the OpenGL version
	std::cout << "*** OpenGL Version: " << glGetString(GL_VERSION) << " ***";

	// Set the background color.
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// Set VSYNC
	SDL_GL_SetSwapInterval(0);

	initShaders();
}


// This function initialises the shaders.
//
void MainGame::initShaders() {
	_colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();

}


// This function is the main game loop.
//
void MainGame::gameLoop() {
	while(_gameState != GameState::EXIT) {
		// used for frame time measuring
		float startTicks = SDL_GetTicks();

		processInput();
		_time += 0.1;
		drawGame();
		calculateFPS();

		// print only once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if(frameCounter == 10) {
			std::cout << _fps << std::endl;
			frameCounter = 0;
		}

		float frameTicks = SDL_GetTicks() - startTicks;

		// Limit the FPS to the max FPS
		if (1000.0f / _maxFPS > frameTicks) {
			SDL_Delay(1000.0f / _maxFPS - frameTicks);
		}
	}
}


// This function processes the input.
//
void MainGame::processInput() {
	SDL_Event e;

	while(SDL_PollEvent(&e) != 0) {
		switch(e.type) {
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			//std::cout << e.motion.x << " " << e.motion.y << std::endl;
			break;
		}
	}
}

// This function draws the game.
//
void MainGame::drawGame() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable the shader
	_colorProgram.use();

	// We are using texture unit 0
	glActiveTexture(GL_TEXTURE0);

	// Get the uniform location
	GLint textureLocation = _colorProgram.getUniformLocation("mySampler");

	// Tell the shader that the texture is in texture unit 0
	glUniform1i(textureLocation, 0);

	// Set the constantly changing time variable
	GLuint timeLocation = _colorProgram.getUniformLocation("time");
	glUniform1f(timeLocation, _time);

	for(unsigned int i = 0; i < _sprites.size(); i++) {
		_sprites[i]->draw();
	}

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// disable the shader
	_colorProgram.unuse();

	SDL_GL_SwapWindow(_window);
}

void MainGame::calculateFPS() {
	static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	static float prevTicks = SDL_GetTicks();

	float currentTicks;
	currentTicks = SDL_GetTicks();

	_frameTime = currentTicks - prevTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;

	prevTicks = currentTicks;

	int count;
	currentFrame++;

	if(currentFrame < NUM_SAMPLES) {
		count = currentFrame;
	}
	else {
		count = NUM_SAMPLES;
	}

	float frameTimeAverage = 0;
	for (int i = 0; i < count; i++) {
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;

	if(frameTimeAverage > 0) {
		_fps = 1000.0f / frameTimeAverage;
	}
	else {
		_fps = 60.0f;
	}


}