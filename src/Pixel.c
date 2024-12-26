/*
 * Pixel.c
 *
 *  Created on: -----
 *      Author: -----
 */

#include "Pixel.h"



//Table for pixel dots.
//				 dots[X][Y][COLOR]
volatile uint8_t dots[8][8][3]={0};

volatile uint8_t RST_BIT = 0b000001;
volatile uint8_t BIT1 = 0b000010;
volatile uint8_t SB_BIT = 0b000100;
volatile uint8_t CLK_BIT = 0b001000;
volatile uint8_t SDA_BIT = 0b010000;
volatile uint8_t BIT8 = 0b10000000;

uint8_t alien_direction = LEFT;
uint8_t alien_coord = 3;
static uint8_t alien_y = 6;

static uint8_t game_score = 0;
uint8_t is_game_over = 0;

// Here the setup operations for the LED matrix will be performed
void setup(){

	CONTROL = 0;
	CHANNEL = 0;

	//reseting screen at start is a MUST to operation (Set RST-pin to 1).
	CONTROL |= 1;
	usleep(500);
	CONTROL &=~ 1;
	usleep(500);
	CONTROL |= 1;
	usleep(500);

	CONTROL |= SB_BIT;

	//Write code that sets 6-bit values in register of DM163 chip. Recommended that every bit in that register is set to 1.
	//6-bits and 24 "bytes", so some kind of loop structure could be nice.
	//24*6 bits needs to be transmitted
	for (int i = 0; i < 144; i++) {
		CONTROL &=~ 0b001000;
		CONTROL |= 0b001000;
	}

	//Final thing in this function is to set SB-bit to 1 to enable transmission to 8-bit register.
	CONTROL |= SB_BIT;

}

//Change value of one pixel at led matrix. This function is only used for changing values of dots array
void SetPixel(uint8_t x,uint8_t y, uint8_t r, uint8_t g, uint8_t b){

	//Hint: you can invert Y-axis quite easily with 7-y
	dots[x][7-y][0]=b;
	//Write rest of two lines of code required to make this function work properly (green and red colors to array).
	dots[x][7-y][1]=g;
	dots[x][7-y][2]=r;

}


//Put new data to led matrix. Hint: This function is supposed to send 24-bytes and parameter x is for channel x-coordinate.
void run(uint8_t x){


	//Write code that writes data to led matrix driver (8-bit data). Use values from dots array
	//Hint: use nested loops (loops inside loops)
	//Hint2: loop iterations are 8,3,8 (pixels,color,8-bitdata)
	latch();
	for (int pixel = 0; pixel < 8; pixel++) {

		for (int color = 0; color < 3; color++) {
			uint8_t data = dots[x][pixel][color];
			for (int bit = 0; bit < 8; bit++) {

				if (data & 0x80) {
					CONTROL |= SDA_BIT;
				}
				else {
					CONTROL &=~ SDA_BIT;
				}

				CONTROL &= ~CLK_BIT;
				data <<= 1;
				CONTROL |= CLK_BIT;
			}
		}
	}
	latch();
	CONTROL &=~ CLK_BIT;

}

//Latch signal. See colorsshield.pdf or DM163.pdf in project folder on how latching works
void latch(){
	CONTROL |= 0b000010;
	CONTROL &=~ 0b000010;
}


//Set one line (channel) as active, one at a time.
void open_line(uint8_t x){

	switch(x) {
	case 0:
		CHANNEL |= 0b00000001;
		break;
	case 1:
		CHANNEL |= 0b00000010;
		break;
	case 2:
		CHANNEL |= 0b00000100;
		break;
	case 3:
		CHANNEL |= 0b00001000;
		break;
	case 4:
		CHANNEL |= 0b00010000;
		break;
	case 5:
		CHANNEL |= 0b00100000;
		break;
	case 6:
		CHANNEL |= 0b01000000;
		break;
	case 7:
		CHANNEL |= 0b10000000;
		break;
	default:
        CHANNEL = 0;  // Clear all bits if x is out of range
		break;
	}
}

void moveAlien(){
	if (alien_direction == RIGHT) {
		alienRight();
	}
	else if (alien_direction == LEFT) {
		alienLeft();
	}
}

void alienLeft() {
	if (alien_coord <= 0) {
		alien_direction = RIGHT;
		return;
	}

	SetPixel(alien_coord, alien_y, 0, 0, 0);
	alien_coord--;
	SetPixel(alien_coord, alien_y, 0, 255, 255);
}

void alienRight() {

	if (alien_coord > 6) {
		alien_direction = LEFT;
		return;
	}

	SetPixel(alien_coord, alien_y, 0, 0, 0);
	alien_coord++;
	SetPixel(alien_coord, alien_y, 0, 255, 255);
}


void initShip(Ship* ship) {

	ship->pos = 3;

	SetPixel(ship->pos, 1, 0, 80, 20);
	SetPixel(ship->pos, 0, 0, 80, 20);
	SetPixel(ship->pos-1, 0, 0, 80, 20);
	SetPixel(ship->pos+1, 0, 0, 80, 20);

}

void moveShipRight(Ship* ship) {
	// Check bounds
	if (ship->pos == 6) {
		return;
	}

	// Turn OFF left hand pixel and top pixel
	SetPixel(ship->pos-1, 0, 0, 0, 0);
	SetPixel(ship->pos, 1, 0, 0, 0);

	// Turn ON right hand pixel and top pixel
	SetPixel(ship->pos+2, 0, 0, 80, 20);
	SetPixel(ship->pos+1, 1, 0, 80, 20);

	ship->pos++;
}

void moveShipLeft(Ship* ship) {
	// Check bounds
	if (ship->pos == 1) {
		return;
	}

	// Turn OFF right hand pixel and top pixel
	SetPixel(ship->pos + 1, 0, 0, 0, 0);
	SetPixel(ship->pos, 1, 0, 0, 0);

	// Turn ON left hand pixel and top pixel
	SetPixel(ship->pos-2, 0, 0, 80, 20);
	SetPixel(ship->pos-1, 1, 0, 80, 20);

	ship->pos--;
}

void initBullet(Bullet* bullet) {
	// Initialize bullet
	bullet->is_bullet_shot = 0;
	bullet->bullets_left = 8;
	bullet->coord->x = 0;
	bullet->coord->y = 2;

    // Set initial ammo count on the LED matrix
    for (int i = 0; i < 8; i++) {
        SetPixel(i, 7, 255, 255, 0);
    }
}


void shootBullet(Ship* ship, Bullet* bullet) {
	if (bullet->is_bullet_shot == 1) {
		return;
	}

	bullet->coord->x = ship->pos;
	bullet->coord->y = 2;

	bullet->is_bullet_shot = 1;
	bullet->bullets_left--;
	if (bullet->bullets_left == 0) {
		is_game_over = 1;
		clearScreen();
		displayScore(&game_score);
	}

	// Reduce ammo count
	SetPixel(bullet->bullets_left, 7, 0, 0, 0);
}

void moveBullet(Bullet* bullet) {

	// CASE: No bullets shot
	if (bullet->is_bullet_shot == 0) {
		return;
	}

	// CASE: Bullet hit end of matrix
	if (bullet->coord->y == alien_y) {
		// Check for hit
		if (bullet->coord->x == alien_coord) {
			addPoint(bullet);
		}

		// Reset to alien ships 'cannon'
		SetPixel(bullet->coord->x, bullet->coord->y, 0, 0, 0);
		bullet->is_bullet_shot = 0;
		return;
	}

	// Move bullet
	SetPixel(bullet->coord->x, bullet->coord->y, 0, 0, 0);
	bullet->coord->y++;
	SetPixel(bullet->coord->x, bullet->coord->y, 0, 255, 255);

	bullet->is_bullet_shot = 1;
}

void addPoint(Bullet* bullet) {
	game_score++;
}

void clearScreen() {
	unsigned int i = 0;
	unsigned int j = 0;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			SetPixel(i, j, 0, 0, 0);
		}
	}
}

void displayScore(uint8_t* score) {
	uint8_t i;
	for (i = 0; i < *score; i++) {
		SetPixel(i, 4, 255, 255, 0);
	}
}


void resetGame(Ship* ship, Bullet* bullet) {

	clearScreen();
	game_score = 0;

	// Reset alien
    alien_direction = LEFT;
    alien_coord = 3;  // Start position
    alien_y = 6;

	// Draw alien
    SetPixel(alien_coord, alien_y, 0, 255, 255);
    initShip(ship);
	initBullet(bullet);

	is_game_over = 0;

	setup();
}
