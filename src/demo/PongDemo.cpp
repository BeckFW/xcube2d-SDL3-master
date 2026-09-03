#include "PongDemo.h"

// PongDemo.cpp

// Contructor with inline initialisation of member variables
Pong::Pong(int _maxRounds) : 
	AbstractGame(),
	// Get window to position objects
	window(gfx->getCurrentWindowSize()),
	// Game Objects
	player(0, window.h / 2, 20, 50),
	ball(window.w / 2, window.h / 2, { 5,5 }, 10), 
	npc(window.w - 20, window.h / 2, 20, 50, ball),
	// Game State
	gameOver(false),
	round(0),
	maxRounds(_maxRounds),
	// Scores
	playerScore(0), 
	npcScore(0), 
	playerHits(0), 
	npcHits(0)

{
	// Set font from available resources in res folder
	bigFont = ResourceManager::loadFont("res/fonts/arial.ttf", 72);
	font = ResourceManager::loadFont("res/fonts/arial.ttf", 42); 
	smallFont = ResourceManager::loadFont("res/fonts/arial.ttf", 22);

	gfx->setVerticalSync(true);
	gfx->setWindowTitle("PONG Demo");
}

Pong::~Pong() {
	// Deconstructor
	std::cout << "PONG closing" << std::endl; 
}

void Pong::reset() {
	gameOver = false;
	// position ball
	ball.setX(window.w / 2);
	ball.setY(window.h / 2); 
	std::cout << "Reset complete" << std::endl;
}

void Pong::resetGame() {
	round = 0; 
	npcHits = 0;
	npcScore = 0; 
	playerHits = 0; 
	playerScore = 0; 
	reset();
}

void Pong::update() {

	if (round < maxRounds) {
		if (!gameOver) {
			// movement
			handleKeyEvents(); // trigger player movements with xcube events method
			player.update(); // update player
			ball.update(); // move the ball
			npc.update(); // move the npc towards the ball

			// collisions
			checkCollision();

			// check for points and ensure positions are valid
			checkPositions();
		}
	}
	else {
		gameOver = true;
		// Game Over screen is displayed
		// Wait for user input
		if (eventSystem->isPressed(Key::W)) {
			// restart
			resetGame();
		}

		if (eventSystem->isPressed(Key::S)) {
			// quit
			std::cout << "Quit" << std::endl;
			running = false;
		}
	}
}

void Pong::checkCollision() {

	// incremement hit count for every paddle collision
	if (ball.getRect().intersects(player.getRect())) {
		ball.reverseXVel();
		playerHits++;
		// play sound
	}
	
	if (ball.getRect().intersects(npc.getRect())) {
		ball.reverseXVel();
		npcHits++;
	}

	// give npc a point if ball hits left edge
	if ((ball.getX()) <= 0) {
		npcScore++;
		round++;
		gameOver = true;
		reset();
	}

	// give the player a point if the ball touches the right edge
	if ((ball.getX() + ball.getW()) >= window.w) {
		playerScore++;
		round++;
		gameOver = true;
		reset();
	}
}

void Pong::checkPositions() {
	// has the ball hit the top or bottom of the screen
	if ((ball.getY() <= 0) || ((ball.getY() + ball.getW()) >= window.h)) {
		// bounce away
		ball.reverseYVel();
	}

	// stop npc from leaving the screen
	if (npc.getY() < 0) npc.setY(0);
	if ((npc.getY() + npc.getRect().h) > window.h ) npc.setY(window.h - npc.getRect().h);
}

void Pong::render() {
	// draw ball
	gfx->setDrawColor(SDL_COLOR_BLUE);
	gfx->drawCircle(ball.getCenter(), ball.getR());

	// draw player
	gfx->setDrawColor(SDL_COLOR_RED);
	gfx->drawRect(player.getRect()); 

	// draw npc
	gfx->drawRect(npc.getRect());

	// draw collision rect
	gfx->setDrawColor(SDL_COLOR_WHITE);
	gfx->drawRect(ball.getRect());
}

// handle input (move player)
void Pong::handleKeyEvents() {
	if (eventSystem->isPressed(Key::W)) {

		if (player.getY() > 0) {
			player.setY(player.getY() - player.getVelocity().dy);
		}
	}

	if (eventSystem->isPressed(Key::S)) {
	
		if ((player.getY() + player.getH()) < window.h) {
			player.setY(player.getY() + player.getVelocity().dy);
		}
	}
}

// draw the UI
void Pong::renderUI() {
	if (!gameOver) {
		// create strings for drawText fn
		std::string scoreStr = std::to_string(playerScore);
		std::string pHitsStr = "Hits: " + std::to_string(playerHits);
		std::string npcScoreStr = std::to_string(npcScore);
		std::string npcHitsStr = "Hits: " + std::to_string(npcHits);

		gfx->setDrawColor(SDL_COLOR_AQUA);

		// magic numbers everywhere :( 

		int textW, textH;
		int padding = 25; 

		// draw main score
		gfx->useFont(bigFont);
		gfx->drawText(scoreStr, padding, 25);
		TTF_GetStringSize(bigFont, npcScoreStr.c_str(), scoreStr.length(), &textW, &textH);
		gfx->drawText(npcScoreStr, window.w - (textW + padding), 25);

		// draw number of hits
		gfx->useFont(font);
		gfx->drawText(pHitsStr, padding, 125);
		TTF_GetStringSize(font, npcHitsStr.c_str(), npcHitsStr.length(), &textW, &textH);
		gfx->drawText(npcHitsStr, window.w - (textW + padding), 125);
	}
	else {
		gameOverUI();
	}

}

void Pong::gameOverUI() {

	// inefficient - better approach to calculate text sizes and positions once and store as member var
	// variables for text
	std::string text = "Game Over";
	std::string winText = "You won with " + std::to_string(playerScore) + " points!";
	std::string looseText = "You lost. Your opponent scored " + std::to_string(npcScore) + " points!";
	std::string hitText = "Between you, there were " + std::to_string(playerHits + npcHits) + " hits!";
	std::string playAgain = "Would you like to play again?";
	std::string input = "Yes -> W | No -> S";

	// reusable variables for storing rendered text dimensions
	int textW, textH;

	// clear any existing UI
	gfx->clearScreen();
	gfx->setDrawColor(SDL_COLOR_AQUA);
	gfx->useFont(bigFont);

	// estimate size of text once drawn so it can be centered dynamically
	TTF_GetStringSize(bigFont, text.c_str(), text.length(), &textW, &textH);
	gfx->drawText(text, (window.w - textW) / 2, 50);

	// switch to smaller font
	gfx->useFont(font);

	// win/loss screens
	if (playerScore > npcScore) {
		TTF_GetStringSize(font, winText.c_str(), winText.length(), &textW, &textH);
		gfx->drawText(winText, (window.w - textW) / 2, 150);
	}
	else {
		TTF_GetStringSize(font, looseText.c_str(), looseText.length(), &textW, &textH);
		gfx->drawText(looseText, (window.w - textW) / 2, 150);
	}

	gfx->setDrawColor(SDL_COLOR_WHITE);
	TTF_GetStringSize(font, hitText.c_str(), hitText.length(), &textW, &textH);
	gfx->drawText(hitText, (window.w - textW) / 2, 300);

	TTF_GetStringSize(font, playAgain.c_str(), playAgain.length(), &textW, &textH);
	gfx->drawText(playAgain, (window.w - textW) / 2, 400);

	gfx->useFont(smallFont);
	TTF_GetStringSize(smallFont, input.c_str(), input.length(), &textW, &textH);
	gfx->drawText(input, (window.w - textW) / 2, 450);
}


GameObject::GameObject(int _x, int _y, int _w, int _h, Velocity2 _v) : x(_x), y(_y), w(_w), h(_h), velocity(_v), rect(x,y,w,h)
{
	std::cout << "GameObject created dimensions: \n W: " << rect.w << "H: " << rect.h << std::endl;
}


Paddle::Paddle(int _x, int _y, int _w, int _h) : GameObject(_x, _y, _w, _h)
{
	std::cout << "Paddle created" << std::endl;
}

Player::Player(int _x, int _y, int _w, int _h) : Paddle(_x, _y, _w, _h) {
	std::cout << "Player Created" << std::endl;
}

void Player::update() {

}

NPC::NPC(int _x, int _y, int _w, int _h, GameObject& _target) : Paddle(_x, _y, _w, _h), target(_target) {
	std::cout << "NPC Created" << std::endl;
}

void NPC::update() {

	// calculate center of box
	int npcCenter = getRect().y + (getRect().h / 2);
	
	// check against last update to see if the ball is approaching
	if (target.getX() > targetTracker) {
		// move towards the target
		if (npcCenter < target.getY() - deadzone) {
			setY(getY() + getVelocity().dy);
		}
		else if (npcCenter > target.getY() + deadzone) {
			setY(getY() - getVelocity().dy);
		}
	} 
	// update the ball's last position
	targetTracker = target.getX();

	// random movement between the boundaries
	// hold reference to window? 
}

Ball::Ball(int _x, int _y, Velocity2 _v, int _r) : GameObject(_x, _y, (_r*2), (_r*2), _v), r(_r) {
	std::cout << "Ball Created dimensions: \n W: " << getRect().w << " H: " << getRect().h << std::endl;
}

void Ball::update() {
	// move the ball
	setX(getX() + getVelocity().dx);
	setY(getY() + getVelocity().dy); 
}

void Ball::reverseXVel() {
	setVelocity(getVelocity().dx * -1, getVelocity().dy);
}

void Ball::reverseYVel() {
	setVelocity(getVelocity().dx, getVelocity().dy * -1);
}