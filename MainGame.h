/*
 * MainGame.h
 *
 *  Created on: 22 May 2016
 *      Author: jan
 */

#ifndef MAINGAME_H_
#define MAINGAME_H_

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <vector>

#include <JTEngine/JTEngine.h>
#include <JTEngine/GLSLProgram.h>
#include <JTEngine/GLTexture.h>
#include <JTEngine/Sprite.h>
#include <JTEngine/Window.h>
#include <JTEngine/Camera2D.h>


enum class GameState {PLAY, EXIT};

class MainGame {
public:
	MainGame();
	~MainGame();

	void run();

private:
	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput();
	void drawGame();
	void calculateFPS();

	JTEngine::Window _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	std::vector<JTEngine::Sprite*> _sprites;

	JTEngine::GLSLProgram _colorProgram;
	JTEngine::Camera2D _camera;

	float _fps;
	float _maxFPS;
	float _frameTime;
	float _time;



};

#endif /* MAINGAME_H_ */
