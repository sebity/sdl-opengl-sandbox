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
#include <JTEngine/ResourceManager.h>

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

	gameLoop();
}


// Initialises SDL and OpenGL as well as whatever else is needed.
//
void MainGame::initSystems() {
	JTEngine::init();

	_window.create("Game Engine", _screenWidth, _screenHeight, 0);

	initShaders();

	_spriteBatch.init();
	_fpsLimiter.init(_maxFPS);
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
		_fpsLimiter.begin();

		processInput();
		_time += 0.1;

		_camera.update();

		drawGame();

		_fps = _fpsLimiter.end();

		// print only once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if(frameCounter == 10) {
			std::cout << _fps << std::endl;
			frameCounter = 0;
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
			_inputManager.pressKey(e.key.keysym.sym);

			break;
		case SDL_KEYUP:
			_inputManager.releaseKey(e.key.keysym.sym);
			break;
		}
	}

	if(_inputManager.isKeyPressed(SDLK_w)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0, CAMERA_SPEED));
	}

	if(_inputManager.isKeyPressed(SDLK_s)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0, -CAMERA_SPEED));
	}

	if(_inputManager.isKeyPressed(SDLK_a)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0));
	}

	if(_inputManager.isKeyPressed(SDLK_d)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0));
	}

	if(_inputManager.isKeyPressed(SDLK_q)) {
		_camera.setScale(_camera.getScale() + SCALE_SPEED);
	}

	if(_inputManager.isKeyPressed(SDLK_e)) {
		_camera.setScale(_camera.getScale() - SCALE_SPEED);
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

	_spriteBatch.begin();

	glm::vec4 pos(0.0f ,0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	JTEngine::GLTexture texture = JTEngine::ResourceManager::getTexture("Textures/JimmyJumpPack/CharacterRight_Standing.png");
	JTEngine::Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	for (int i = 0; i < 10; i++) {
		_spriteBatch.draw(pos, uv, texture.id, 0.0f, color);
		_spriteBatch.draw(pos + glm::vec4(50, 0, 0, 0), uv, texture.id, 0.0f, color);
	}
	_spriteBatch.end();


	_spriteBatch.renderBatch();

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// disable the shader
	_colorProgram.unuse();

	_window.swapBuffer();
}

