#pragma once
#include "../engine/AbstractGame.h"

// PongDemo.h

// 2 Point struct to hold velocity info
struct Velocity2 { int dx; int dy; };

class GameObject {
public:
	GameObject(int _x, int _y, int _w, int _h, Velocity2 _v = {5, 5});

	// overridable update function
	virtual void update() = 0;

	// getters (allow access to private vars for children with no write access)
	int getX() const { return rect.x; }
	int getY() const { return rect.y; }
	int getW() const { return rect.w; }
	int getH() const { return rect.h; }
	Velocity2 getVelocity() const { return velocity; }
	Rect getRect() const { return rect; }


	// setters
	// update xPos and Rect pos to keep in sync
	void setX(int _x) { rect.x = _x; }
	void setY(int _y) { rect.y = _y; }
	void setW(int _w) { rect.w = _w; }
	void setH(int _h) { rect.h = _h; }
	void setVelocity(int _dx, int _dy) { velocity = { _dx, _dy }; }
	void setDX(int _dx) { velocity.dx = _dx; }
	void setDY(int _dy) { velocity.dy = _dy; }

	// helpers
	Point2 getCenter() const { return { rect.x + rect.w / 2, rect.y + rect.h / 2 }; }
	void moveUp() { rect.y += velocity.dy; }
	void moveDown() { rect.y -= velocity.dy; }
	
private:
	//int x, y, w, h;
	Velocity2 velocity;
	Rect rect;
};

class Paddle : public GameObject {
public:
	Paddle(int _x, int _y, int _w, int _h);
};

class Player : public Paddle {
public:
	Player(int _x, int _y, int _w, int _h);
	void update() override;
};

class NPC : public Paddle {
public: 
	NPC(int _x, int _y, int _w, int _h, GameObject& _target );
	void update() override; 

	int targetTracker = 0; 

private:
	GameObject& target;
	// Deadzone - stop moving when near the target so it's not followed too closely
	int deadzone = 40;
};

class Ball : public GameObject {
public:
	Ball(int _x, int _y, Velocity2 _v, int _r);
	void update() override;

	// getters
	float getR() const { return r; }
	Point2 getCenter() const { return Point2(getX() + getR(), getY() + getR()); }
	
	// setters
	void setR(float _r) { r = _r; }

	// helpers
	void reverseXVel();
	void reverseYVel(); 

private:
	float r;
};

// Game Class
class Pong : public AbstractGame {

private:

	Dimension2i window; // declared first so it is constructed in correct order for access by game objects

	// Game objects
	Player player;
	NPC npc;
	Ball ball;

	// Game state/scores
	bool gameOver;
	int round, maxRounds, playerScore, npcScore, playerHits, npcHits; 

	// Assets
	TTF_Font* bigFont;
	TTF_Font* font;
	TTF_Font* smallFont;
	MIX_Audio* edgeSine;
	MIX_Audio* playerSine;
	MIX_Audio* npcSine;
	MIX_Audio* outSine;
	MIX_Audio* outSineFM;

	// Methods
	void update();
	void checkCollision();
	void checkPositions();
	void reset();
	void resetGame();
	void handleKeyEvents();
	void render();
	void renderUI();
	void gameOverUI();
	void generateSoundEffects();
	void playGeneratedSound(MIX_Audio* _sound);

public:
	Pong(int _maxRounds);
	~Pong();
};