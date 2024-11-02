/*
 * Ball & Brick Game Example for Nova Board
 *
 * Button Connections:
 * - UP Button: connected to pin 2
 * - LEFT Button: connected to pin 3
 * - DOWN Button: connected to pin 4
 * - RIGHT Button: connected to pin 5
 * - RESET Button: connected to pin 6
 *
 * Note: External pull-down resistors are used with the buttons to ensure stable readings.
 *
 * License:
 * This project is open-source hardware and software, released under the MIT License.
 * See the LICENSE file for details.
 */


#include <FastLED.h>

// LED matrix settings
#define NUM_LEDS 70
#define DATA_PIN 22
#define BRIGHTNESS 10

CRGB leds[NUM_LEDS];

// Button pins
#define LEFT_BUTTON 3
#define RIGHT_BUTTON 5
#define RESET_BUTTON 6

// Matrix Layout
int matrix[10][7] = {
  {69, 50, 49, 30, 29, 10, 9},
  {68, 51, 48, 31, 28, 11, 8},
  {67, 52, 47, 32, 27, 12, 7},
  {66, 53, 46, 33, 26, 13, 6},
  {65, 54, 45, 34, 25, 14, 5},
  {64, 55, 44, 35, 24, 15, 4},
  {63, 56, 43, 36, 23, 16, 3},
  {62, 57, 42, 37, 22, 17, 2},
  {61, 58, 41, 38, 21, 18, 1},
  {60, 59, 40, 39, 20, 19, 0}
};

// Game properties
struct Ball {
  int x, y;
  int dx, dy; // Direction of the ball
};

struct Paddle {
  int x, y; // Paddle position
};

Paddle paddle;
Ball ball;
bool bricks[3][7]; // True if the brick is present (only top 3 rows)
bool gameOver;

// Speed settings
unsigned long gameSpeed = 100; // Game speed in milliseconds
unsigned long lastUpdateTime = 0; // Last time the game was updated
unsigned long lastPaddleMoveTime = 0; // Last time the paddle moved
const unsigned long debounceDelay = 100; // Delay for debounce

// Setup and initialize
void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  randomSeed(analogRead(0));
  initializeGame();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check for reset button (manual reset)
  if (gameOver && digitalRead(RESET_BUTTON) == HIGH) {
    initializeGame();
    return;
  }

  // Update paddle position based on button input with debounce
  if (currentMillis - lastPaddleMoveTime >= debounceDelay) {
    if (digitalRead(LEFT_BUTTON) == HIGH && paddle.x > 0) {
      paddle.x--;
      lastPaddleMoveTime = currentMillis; // Update last paddle move time
    }
    if (digitalRead(RIGHT_BUTTON) == HIGH && paddle.x < 5) { // Paddle can move to 5 to cover 3 LEDs
      paddle.x++;
      lastPaddleMoveTime = currentMillis; // Update last paddle move time
    }
  }

  // Move the ball and update the game only if enough time has passed
  if (currentMillis - lastUpdateTime >= gameSpeed) {
    moveBall();
    lastUpdateTime = currentMillis;

    // Update LED display
    FastLED.clear();
    drawBricks();
    drawPaddle();
    drawBall();
    FastLED.show();
  }
}

// Initialize or reset game
void initializeGame() {
  // Initialize paddle
  paddle.x = 3; // Start in the middle
  paddle.y = 9; // Bottom of the matrix

  // Initialize ball
  ball.x = 3;
  ball.y = 8; // Start above the paddle
  ball.dx = 1; // Initial direction (right)
  ball.dy = -1; // Initial direction (up)
  gameOver = false;

  // Initialize bricks: fill the top 3 rows with bricks
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) {
      bricks[i][j] = true; // All bricks are present in the top 3 rows
    }
  }
}

// Move the ball and check for collisions
void moveBall() {
  // Update ball position
  ball.x += ball.dx;
  ball.y += ball.dy;

  // Check for wall collisions
  if (ball.x < 0 || ball.x >= 7) {
    ball.dx = -ball.dx; // Reverse direction
    ball.x = constrain(ball.x, 0, 6); // Keep ball within bounds
  }
  if (ball.y < 0) {
    ball.dy = -ball.dy; // Reverse direction
    ball.y = 0; // Ensure the ball is at the top row
  }

  // Check for paddle collision
  if (ball.y == paddle.y && ball.x >= paddle.x && ball.x < paddle.x + 3) {
    ball.dy = -ball.dy; // Reverse direction
    // Calculate new horizontal direction based on where it hits the paddle
    int hitPos = ball.x - paddle.x; // Position on paddle (0 to 2)
    if (hitPos == 0) { // Left side
      ball.dx = -1; // Bounce left
    } else if (hitPos == 2) { // Right side
      ball.dx = 1; // Bounce right
    } else { // Center
      ball.dx = 0; // No horizontal movement
    }
    ball.y = paddle.y - 1; // Position ball above paddle
  }

  // Check for brick collision
  if (ball.y < 3 && bricks[ball.y][ball.x]) { // Check only for the rows with bricks
    bricks[ball.y][ball.x] = false; // Remove brick
    ball.dy = -ball.dy; // Reverse direction

    // Position ball above the brick to prevent getting stuck
    if (ball.dy < 0) {
      ball.y--; // Move ball up
    } else {
      ball.y++; // Move ball down if going down (not likely)
    }

    // Check again if the ball is out of bounds
    if (ball.y < 0) {
      ball.y = 0; // Ensure the ball is not stuck above the matrix
    }
  }

  // Check for game over
  if (ball.y >= 10) {
    gameOver = true; // Ball fell below paddle
    initializeGame(); // Automatically reset the game
  }
}

// Draw the paddle on the LED matrix
void drawPaddle() {
  for (int i = 0; i < 3; i++) {
    if (paddle.x + i < 7) {
      leds[matrix[paddle.y][paddle.x + i]] = CRGB::Blue; // Paddle color
    }
  }
}

// Draw the ball on the LED matrix
void drawBall() {
  if (ball.y < 10 && ball.x < 7) {
    leds[matrix[ball.y][ball.x]] = CRGB::White; // Ball color
  }
}

// Draw the bricks on the LED matrix
void drawBricks() {
  for (int i = 0; i < 3; i++) { // Draw only the rows with bricks
    for (int j = 0; j < 7; j++) {
      if (bricks[i][j]) {
        leds[matrix[i][j]] = CRGB::Red; // Brick color
      }
    }
  }
}
