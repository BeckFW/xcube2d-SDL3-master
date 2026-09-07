#pragma once
#include "../engine/AbstractGame.h"

// PongDemo.h

// 2 Point struct to hold velocity info
struct Velocity2 { int dx; int dy; };

// Colors that will be used in the game
struct Colors {
	SDL_Color playerPrimary = SDL_COLOR_VIOLET;
	SDL_Color playerSecondary = SDL_COLOR_WHITE;
	SDL_Color npcPrimary = SDL_COLOR_AQUA;
	SDL_Color npcSecondary = SDL_COLOR_WHITE;
	SDL_Color ballPrimary = SDL_COLOR_WHITE;
	SDL_Color ballSecondary = SDL_COLOR_BLUE;
};

class GameObject {
public:
	// Constructor
	GameObject(int _x, int _y, int _w, int _h, SDL_Color _pColor, SDL_Color _sColor, Velocity2 _v = { 5, 5 });
	// Destructor
	virtual ~GameObject() = default;

	// overridable update function
	virtual void update() = 0;
	// render function with reference to gfx engine
	virtual void render(GraphicsEngine& _gfx) = 0;

	// getters (allow access to private vars for children with no write access)
	int getX() const { return rect.x; }
	int getY() const { return rect.y; }
	int getW() const { return rect.w; }
	int getH() const { return rect.h; }
	Velocity2 getVelocity() const { return velocity; }
	Rect getRect() const { return rect; }
	SDL_Color getPrimaryColor() const { return primaryColor; }
	SDL_Color getSecondaryColor() const { return secondaryColor; }

	// setters
	void setX(int _x) { rect.x = _x; }
	void setY(int _y) { rect.y = _y; }
	void setW(int _w) { rect.w = _w; }
	void setH(int _h) { rect.h = _h; }
	void setVelocity(int _dx, int _dy) { velocity = { _dx, _dy }; }
	void setDX(int _dx) { velocity.dx = _dx; }
	void setDY(int _dy) { velocity.dy = _dy; }
	void setPrimaryColor(SDL_Color _c) { primaryColor = _c; }
	void setSecondaryColor(SDL_Color _c) { secondaryColor = _c; }


	// helpers
	Point2 getCenter() const { return { rect.x + rect.w / 2, rect.y + rect.h / 2 }; }
	void moveUp() { rect.y -= velocity.dy; }
	void moveDown() { rect.y += velocity.dy; }
	
private:
	Velocity2 velocity;
	Rect rect;
	SDL_Color primaryColor;
	SDL_Color secondaryColor;
};

class Paddle : public GameObject {
public:
	Paddle(int _x, int _y, int _w, int _h, SDL_Color _pColor, SDL_Color _sColor);
	void render(GraphicsEngine& _gfx) override;
};

class Player : public Paddle {
public:
	Player(int _x, int _y, int _w, int _h, SDL_Color _pColor, SDL_Color _sColor);
	void update() override;
};

class NPC : public Paddle {
public: 
	NPC(int _x, int _y, int _w, int _h, GameObject* _target, SDL_Color _pColor, SDL_Color _sColor );
	void update() override; 

private:
	GameObject* target;
	// Deadzone - stop moving when near the target so it's not followed too closely
	int deadzone = 40;
	// Previous Location - the last known location of the target being tracked (the ball)
	int targetPreviousLocation = 0;

};

class Ball : public GameObject {
public:
	Ball(int _x, int _y, Velocity2 _v, int _r, SDL_Color _pColor, SDL_Color _sColor);
	void update() override;
	void render(GraphicsEngine& _gfx) override;

	// getters
	int getR() const { return r; }
	Point2 getCenter() const { return Point2(getX() + getR(), getY() + getR()); }
	
	// setters
	void setR(float _r) { r = _r; }

	// helpers
	void reverseXVel();
	void reverseYVel(); 

private:
	int r;
};

// Game Class
class Pong : public AbstractGame {

private:

	Dimension2i window; // declared first so it is constructed in correct order for access by game objects

	Colors gameColors;

	// Game objects
	Player player;
	Ball ball;
	NPC npc;

	std::vector<GameObject*> gameObjects;

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