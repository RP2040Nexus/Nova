/*
 * Flappy Bird Game Example for Nova Board
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

#define NUM_LEDS 70 // Total number of LEDs in the matrix
#define DATA_PIN 22  // Pin connected to the LED matrix
#define BRIGHTNESS 10 // Brightness level

CRGB leds[NUM_LEDS];

// Bird's position and characteristics
int birdX = 2; // Starting X position (2nd column)
int birdY = 3; // Starting Y position (3rd row)
int birdSize = 1; // Size of the bird (1 LED)
int flapHeight = 2; // Height of the flap
bool flapping = false;

// Speed variable
int gameSpeed = 100; // Game speed in milliseconds

// Obstacle characteristics
int obstacleX = 10; // Starting X position of the obstacle
int obstacleHeight = 2; // Height of the obstacle
int obstacleGap = 3; // Gap between upper and lower part of the obstacle
int score = 0; // Player's score

// Function to map row and column to LED index
int getLEDIndex(int row, int col) {
    if (row == 0) return 60 + col; // Row 0
    if (row == 1) return 59 - col; // Row 1
    if (row == 2) return 40 + col; // Row 2
    if (row == 3) return 39 - col; // Row 3
    if (row == 4) return 20 + col; // Row 4
    if (row == 5) return 19 - col; // Row 5
    if (row == 6) return col;       // Row 6 (bottom row, straight mapping)
    
    return -1; // Invalid index
}

// Function to reset game state
void resetGame() {
    birdY = 3; // Reset bird position
    obstacleX = 10; // Reset obstacle position
    obstacleHeight = 2; // Reset obstacle height
    score = 0; // Reset score
    FastLED.clear(); // Clear the LED matrix
    FastLED.show(); // Show cleared matrix
}

// Setup function
void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

// Function to draw the bird
void drawBird() {
    for (int i = 0; i < birdSize; i++) {
        int ledIndex = getLEDIndex(birdY, birdX);
        leds[ledIndex] = CRGB::Yellow; // Set the color of the bird
    }
    FastLED.show();
}

// Function to clear the bird
void clearBird() {
    for (int i = 0; i < birdSize; i++) {
        int ledIndex = getLEDIndex(birdY, birdX);
        leds[ledIndex] = CRGB::Black; // Clear the LED where the bird was
    }
}

// Function to handle bird flapping
void flap() {
    if (birdY > 0) { // Make sure the bird doesn't go off the top
        clearBird();
        birdY -= flapHeight; // Move the bird up
        drawBird();
    }
}

// Function to draw the obstacle
void drawObstacle() {
    for (int i = 0; i < obstacleHeight; i++) {
        int ledIndex = getLEDIndex(i, obstacleX);
        leds[ledIndex] = CRGB::Green; // Set color of the obstacle
    }
    for (int i = obstacleHeight + obstacleGap; i <= 6; i++) {
        int ledIndex = getLEDIndex(i, obstacleX);
        leds[ledIndex] = CRGB::Green; // Set color of the lower part of the obstacle
    }
    FastLED.show();
}

// Function to clear the obstacle
void clearObstacle() {
    for (int i = 0; i < 7; i++) {
        int ledIndex = getLEDIndex(i, obstacleX);
        leds[ledIndex] = CRGB::Black; // Clear the obstacle
    }
}

// Function to check for collision
bool checkCollision() {
    for (int i = 0; i < obstacleHeight; i++) {
        if (birdY == i && obstacleX == birdX) {
            return true; // Collision with upper obstacle
        }
    }
    for (int i = obstacleHeight + obstacleGap; i <= 6; i++) {
        if (birdY == i && obstacleX == birdX) {
            return true; // Collision with lower obstacle
        }
    }
    return false;
}

// Loop function
void loop() {
    if (digitalRead(2) == HIGH) { // Flap button pressed
        flap();
    }

    clearObstacle(); // Clear the obstacle before moving
    obstacleX--; // Move obstacle to the left

    if (obstacleX < 0) { // Reset obstacle position and increase score
        obstacleX = 10; // Reset to starting position
        score++; // Increase score
        // Change obstacle height randomly
        obstacleHeight = random(1, 5); 
    }

    drawObstacle(); // Draw the new obstacle

    if (checkCollision()) {
        // Handle game over
        while (true) {
            // Check if the restart button is pressed
            if (digitalRead(6) == HIGH) {
                resetGame(); // Reset game state
                break; // Exit the loop to restart the game
            }
            // Keep the LEDs off or display game over
            FastLED.clear();
            FastLED.show();
        }
    }

    delay(gameSpeed); // Control game speed
    clearBird(); // Clear the bird before next loop
    birdY = min(birdY + 1, 6); // Simulate gravity (move down)
    drawBird(); // Redraw the bird
}
