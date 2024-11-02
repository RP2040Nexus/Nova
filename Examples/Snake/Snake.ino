/*
 * Snake Game Example for Nova Board
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
#define UP_BUTTON 2
#define LEFT_BUTTON 3
#define DOWN_BUTTON 4
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

// Snake properties
struct Point { int x, y; };
Point snake[100];
int snakeLength;
int dx, dy; // Direction variables
Point food;
bool gameOver;

// Speed settings
unsigned long snakeSpeed = 400; // Adjust this value to change speed, increase the value to make it slower
unsigned long lastMoveTime = 0;  // Last time the snake moved

// Setup and initialize
void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  randomSeed(analogRead(0));
  initializeGame();
}

void loop() {
  if (gameOver) {
    if (digitalRead(RESET_BUTTON) == HIGH) {
      initializeGame();
    }
    return;
  }

  // Update direction based on button input
  if (digitalRead(UP_BUTTON) == HIGH && dy == 0)    { dx = 0; dy = -1; }
  if (digitalRead(LEFT_BUTTON) == HIGH && dx == 0)  { dx = -1; dy = 0; }
  if (digitalRead(DOWN_BUTTON) == HIGH && dy == 0)  { dx = 0; dy = 1; }
  if (digitalRead(RIGHT_BUTTON) == HIGH && dx == 0) { dx = 1; dy = 0; }

  // Move snake based on speed
  unsigned long currentMillis = millis();
  if (currentMillis - lastMoveTime >= snakeSpeed) {
    moveSnake();
    lastMoveTime = currentMillis;

    // Update LED display
    FastLED.clear();
    // Set snake head color
    leds[matrix[snake[0].y][snake[0].x]] = CRGB::Green; // Keep head green
    // Set snake body color with rainbow effect
    for (int i = 1; i < snakeLength; i++) {
      leds[matrix[snake[i].y][snake[i].x]] = CHSV((i * 255 / snakeLength + millis() / 10) % 255, 255, 255);
    }
    // Set food color
    leds[matrix[food.y][food.x]] = CRGB::Red;
    FastLED.show();
  }
}
  
// Initialize or reset game
void initializeGame() {
  snakeLength = 3;
  snake[0] = {3, 5};
  snake[1] = {3, 6};
  snake[2] = {4, 6};
  dx = 0;
  dy = -1; // Initial direction: moving up
  gameOver = false;
  spawnFood();
}

// Move snake
void moveSnake() {
  // Calculate new head position
  Point newHead = { snake[0].x + dx, snake[0].y + dy };

  // Check for wall collision
  if (newHead.x < 0 || newHead.x >= 7 || newHead.y < 0 || newHead.y >= 10) {
    gameOver = true;
    return;
  }

  // Check for self-collision
  for (int i = 0; i < snakeLength; i++) {
    if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
      gameOver = true;
      return;
    }
  }

  // Move snake
  for (int i = snakeLength; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0] = newHead;

  // Check for food collision
  if (newHead.x == food.x && newHead.y == food.y) {
    snakeLength++;
    spawnFood();
  }
}

// Spawn food at a random position
void spawnFood() {
  while (true) {
    food.x = random(7);
    food.y = random(10);
    bool onSnake = false;

    for (int i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        onSnake = true;
        break;
      }
    }

    if (!onSnake) break;
  }
}
