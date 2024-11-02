/*
 * Tetris Game Example for Nova Board
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

#define LED_PIN 22
#define NUM_LEDS 70
#define BRIGHTNESS 10

CRGB leds[NUM_LEDS];

// Matrix layout
const int matrix[10][7] = {
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

// Button pins
const int upButtonPin = 2;
const int leftButtonPin = 3;
const int downButtonPin = 4;
const int rightButtonPin = 5;
const int generalButtonPin = 6;

// Tetromino shapes
const int shapes[7][4][1] = {
    // I
    {{1}, {1}, {1}, {1}},
    // O
    {{1}, {1}, {0}, {0}},
    // T
    {{0}, {1}, {1}, {0}},
    // J
    {{1}, {1}, {1}, {0}},
    // L
    {{0}, {0}, {1}, {1}},
    // S
    {{0}, {1}, {1}, {0}},
    // Z
    {{1}, {1}, {0}, {0}},
};

// Game variables
int currentShapeIndex;
int currentX, currentY;

// Game state
bool board[10][7] = {false};

unsigned long lastUpdate = 0; // Variable to store the last update time
const unsigned long updateInterval = 300; // Game update interval

const unsigned long debounceDelay = 250; // Debounce time in milliseconds
unsigned long lastPressTime[5] = {0}; // Array to store the last pressed time for each button

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);



    // Start a new game
    startNewGame();
}

void loop() {
    readButtons(); // Check for button presses

    unsigned long currentMillis = millis(); // Get the current time
    // Check if it's time to update the game state
    if (currentMillis - lastUpdate >= updateInterval) {
        moveDown(); // Move the piece down
        drawBoard(); // Draw the updated board
        
        lastUpdate = currentMillis; // Update the last update time
    }
    FastLED.show(); // Update the LED strip
}

void startNewGame() {
    // Reset the board state
    memset(board, false, sizeof(board)); // Clear the board
    resetPiece(); // Get a new piece
}



void readButtons() {
    unsigned long currentMillis = millis(); // Get the current time

    // Right button
    if (digitalRead(rightButtonPin) == HIGH && (currentMillis - lastPressTime[0] >= debounceDelay)) {
        lastPressTime[0] = currentMillis; // Update the last pressed time
        movePiece(1, 0); // Move right
    }

    // Left button
    if (digitalRead(leftButtonPin) == HIGH && (currentMillis - lastPressTime[1] >= debounceDelay)) {
        lastPressTime[1] = currentMillis; // Update the last pressed time
        movePiece(-1, 0); // Move left
    }

    // Up button (rotate piece - not implemented)
    if (digitalRead(upButtonPin) == HIGH && (currentMillis - lastPressTime[2] >= debounceDelay)) {
        lastPressTime[2] = currentMillis; // Update the last pressed time
        // Rotate piece (not implemented)
    }

    // Down button
    if (digitalRead(downButtonPin) == HIGH && (currentMillis - lastPressTime[3] >= debounceDelay)) {
        lastPressTime[3] = currentMillis; // Update the last pressed time
        moveDown(); // Move down
    }

    // General button (restart game)
    if (digitalRead(generalButtonPin) == HIGH && (currentMillis - lastPressTime[4] >= debounceDelay)) {
        lastPressTime[4] = currentMillis; // Update the last pressed time
        startNewGame(); // Restart the game
    }
}


void movePiece(int dx, int dy) {
    currentX += dx;
    currentY += dy;

    // Check for collisions
    if (checkCollision()) {
        currentX -= dx; // Undo move
        currentY -= dy; // Undo move
    }
}

void moveDown() {
    currentY++;
    if (checkCollision()) {
        currentY--; // Undo move
        lockPiece(); // Lock the piece
        resetPiece(); // Get a new piece
    }
}

void resetPiece() {
    currentShapeIndex = random(0, 7); // Choose a random shape
    currentX = 3; // Start in the middle
    currentY = 0; // Start at the top

    // Check for game over condition immediately after resetting
    if (checkCollision()) {
        startNewGame(); // Restart the game if collision with the top row occurs
    }
}

bool checkCollision() {
    for (int i = 0; i < 4; i++) {
        int ledX = currentX; 
        int ledY = currentY + (shapes[currentShapeIndex][i][0] == 1 ? i : 0); // Incrementing only for occupied cells

        // Check if out of bounds
        if (ledY >= 10 || ledX < 0 || ledX >= 7) return true; // Ensure within bounds

        // Check if position is occupied
        if (shapes[currentShapeIndex][i][0] == 1) {
            if (ledY < 10 && board[ledY][ledX]) return true; // Check if occupied
        }
    }
    return false; // No collision
}






void lockPiece() {
    for (int i = 0; i < 4; i++) {
        int ledX = currentX;
        int ledY = currentY + i;
        
        if (shapes[currentShapeIndex][i][0] == 1) {
            board[ledY][ledX] = true; // Lock the piece into the board
        }
    }
    clearLines(); // Check for full lines
}

void clearLines() {
    for (int i = 0; i < 10; i++) {
        bool fullLine = true;
        for (int j = 0; j < 7; j++) {
            if (!board[i][j]) {
                fullLine = false;
                break;
            }
        }
        
        if (fullLine) {
            // Display colorful line effect before clearing
            for (int effectTime = 0; effectTime < 20; effectTime++) { // Adjust duration of effect
                // Create a colorful moving line effect
                for (int j = 0; j < 7; j++) {
                    int ledIndex = matrix[i][j];
                    // Cycle through colors
                    leds[ledIndex] = CHSV((effectTime * 10 + j * 30) % 256, 255, 255); // Change colors
                }
                FastLED.show(); // Update the LED strip
                delay(10); // Adjust delay for speed of effect
            }

            // Shift lines down
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < 7; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            // Clear the top line
            for (int j = 0; j < 7; j++) {
                board[0][j] = false;
            }
        }
    }
}


void drawBoard() {
    clearLEDs();
    // Draw the locked pieces
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 7; j++) {
            if (board[i][j]) {
                leds[matrix[i][j]] = CRGB::Red; // Color the locked piece
            }
        }
    }
    // Draw the current piece
    for (int i = 0; i < 4; i++) {
        int ledX = currentX;
        int ledY = currentY + i;
        if (shapes[currentShapeIndex][i][0] == 1 && ledY < 10) {
            leds[matrix[ledY][ledX]] = CRGB::Green; // Color the current piece
        }
    }
}

void clearLEDs() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black; // Clear the LEDs
    }
}
