/*
 * MainGame.cpp
 *
 *  Created on: 22 May 2016
 *      Author: jan
 */

#include <iostream>
#include <typeinfo>

#include "MainGame.h"
#include <JTEngine/Errors.h>

// Constructor, initialises private member variables.
//
MainGame::MainGame() :
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_fps(0.0f),
	_maxFPS(60.0f),
	_frameTime(0.0f),
	_time(0.0f) {

	_camera.init(_screenWidth, _screenHeight);
}

// Destructor.
//
MainGame::~MainGame() {

}


// This function runs the game.
//
void MainGame::run() {
	initSystems();

	_sprites.push_back(new JTEngine::Sprite());
	_sprites.back()->init(0.0f, 0.0f, _screenWidth/2, _screenWidth/2, "Textures/JimmyJumpPack/CharacterRight_Standing.png");

	_sprites.push_back(new JTEngine::Sprite());
	_sprites.back()->init(_screenWidth/2, 0.0f, _screenWidth/2, _screenWidth/2, "Textures/JimmyJumpPack/CharacterRight_Standing.png");

	gameLoop();
}


// Initialises SDL and OpenGL as well as whatever else is needed.
//
void MainGame::initSystems() {
	JTEngine::init();

	_window.create("Game Engine", _screenWidth, _screenHeight, 0);

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

		_camera.update();

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

	const float CAMERA_SPEED = 20.0f;
	const float SCALE_SPEED = 0.1f;

	while(SDL_PollEvent(&e) != 0) {
		switch(e.type) {
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			//std::cout << e.motion.x << " " << e.motion.y << std::endl;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym) {
			case SDLK_w:
				_camera.setPosition(_camera.getPosition() + glm::vec2(0.0, CAMERA_SPEED));
				break;
			case SDLK_s:
				_camera.setPosition(_camera.getPosition() + glm::vec2(0.0, -CAMERA_SPEED));
				break;
			case SDLK_a:
				_camera.setPosition(_camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0));
				break;
			case SDLK_d:
				_camera.setPosition(_camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0));
				break;
			case SDLK_q:
				_camera.setScale(_camera.getScale() + SCALE_SPEED);
				break;
			case SDLK_e:
				_camera.setScale(_camera.getScale() - SCALE_SPEED);
				break;
			}
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

	// Set the camera matrix
	GLint pLocation = _colorProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = _camera.getCameraMatrix();

	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	for(unsigned int i = 0; i < _sprites.size(); i++) {
		_sprites[i]->draw();
	}

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// disable the shader
	_colorProgram.unuse();

	_window.swapBuffer();
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
