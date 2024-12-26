/*
 * Pixel.h
 *
 *  Created on: ------
 *      Author: ------
 */

#ifndef PIXEL_H_
#define PIXEL_H_

#define CHANNEL *(uint8_t *) 0x41220000
#define CONTROL *(uint8_t *) 0x41220008

#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"

typedef struct s_Coord {
	uint8_t x;
	uint8_t y;
} Coord;

typedef struct s_Bullet {
	uint8_t is_bullet_shot;
	uint8_t bullets_left;
	Coord* coord;
} Bullet;

enum DIRECTION {
	LEFT = 0,
	RIGHT = 1
};

typedef struct s_Ship {
	// only X coordinate needed
	uint8_t pos;
} Ship;

extern uint8_t is_game_over;


void setup();
void SetPixel(uint8_t x,uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void run(uint8_t x);
void latch();
void open_line(uint8_t x);

void initShip(Ship* ship);
void moveShipRight(Ship* ship);
void moveShipLeft(Ship* ship);

void initBullet(Bullet* bullet);

void moveAlien();
void alienLeft();
void alienRight();

void shootBullet(Ship* ship, Bullet* bullet);
void moveBullet(Bullet* bullet);

void addPoint(Bullet* bullet);

void clearScreen();
void displayScore(uint8_t* score);

void resetGame(Ship* ship, Bullet* bullet);


#endif /* PIXEL_H_ */
