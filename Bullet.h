/*
 * Bullet.h
 *
 *  Created on: 22 Jun 2016
 *      Author: jan
 */

#ifndef BULLET_H_
#define BULLET_H_

#include <glm/glm.hpp>
#include <JTEngine/SpriteBatch.h>

class Bullet {
public:
	Bullet(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime);
	virtual ~Bullet();

	void draw(JTEngine::SpriteBatch& spriteBatch);
	// Returns true when we are out of life
	bool update();


private:
	int _lifeTime;
	float _speed;
	glm::vec2 _direction;
	glm::vec2 _position;
};

#endif /* BULLET_H_ */
