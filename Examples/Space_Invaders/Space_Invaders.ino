/*
 * Space Invaders Game Example for Nova Board
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

// Define button pins
#define LEFT_BUTTON 3
#define RIGHT_BUTTON 5
#define RESTART_BUTTON 6

// Define game and missile speeds (in milliseconds)
int gameSpeed = 100; // Time interval for starship movement debounce
int missileSpeed = 30; // Time interval for missile movement
int fireInterval = 100; // Time between firing missiles
int enemySpeed = 150; // Speed of enemies moving down (increased for slower movement)
int enemySpeedAdjustment = 50; // Adjusting factor for enemy speed

// Starship position (starts in the middle of the bottom row)
int starshipPosition = 3; // Index for the bottom row (0-6)

// Define LED layout for each row of the matrix
const int rows[10][7] = {
    {69, 50, 49, 30, 29, 10, 9},
    {68, 51, 48, 31, 28, 11, 8},
    {67, 52, 47, 32, 27, 12, 7},
    {66, 53, 46, 33, 26, 13, 6},
    {65, 54, 45, 34, 25, 14, 5},
    {64, 55, 44, 35, 24, 15, 4},
    {63, 56, 43, 36, 23, 16, 3},
    {62, 57, 42, 37, 22, 17, 2},
    {61, 58, 41, 38, 21, 18, 1},
    {60, 59, 40, 39, 20, 19, 0} // Bottom row (starship row)
};

// Enemy positions and colors
const int maxEnemies = 1; // Maximum number of enemies at a time
int enemyRow[maxEnemies]; // Array to hold enemy row positions
int enemyPosition[maxEnemies]; // Array to hold enemy column positions
CRGB enemyColors[maxEnemies]; // Array to hold colors for enemies
bool enemyActive[maxEnemies]; // Array to hold the active state of enemies
unsigned long lastEnemyMoveTime = 0; // Timer for moving enemies

// Missile state
bool missileActive = false;
int missileRow = 8; // Start from row 8 (just above the starship)
int missilePosition = starshipPosition;
unsigned long lastFireTime = 0;
unsigned long lastMissileMoveTime = 0;
unsigned long lastStarshipMoveTime = 0; // Timer for the starship movement debounce

int tailLength = 3; // Length of the missile tail

// Function to clear the starship position
void clearStarship() {
    leds[rows[9][starshipPosition]] = CRGB::Black;
}

// Function to fire a missile
void fireMissile() {
    missileActive = true;
    missileRow = 8; // Start just above the starship in row 8
    missilePosition = starshipPosition;
}

// Function to clear the missile and its tail
void clearMissile() {
    // Clear the missile's position
    if (missileRow < 10) {
        leds[rows[missileRow][missilePosition]] = CRGB::Black; // Clear main missile
    }

    // Clear the tail
    for (int i = 0; i < tailLength; i++) {
        int clearRow = missileRow + i;
        if (clearRow < 10) { // Make sure we don’t go beyond the matrix
            leds[rows[clearRow][missilePosition]] = CRGB::Black; // Clear tail
        }
    }
}

// Function to move the missile upward with a comet-like tail that dims gradually
void moveMissile() {
    if (missileActive) {
        // Clear the previous position of the missile’s tail end
        if (missileRow + tailLength < 10) { // Ensures we clear rows below the missile
            leds[rows[missileRow + tailLength][missilePosition]] = CRGB::Black;
        }

        // Draw the comet-like tail with a dimming effect
        for (int i = 0; i < tailLength; i++) {
            int fadeRow = missileRow + i;
            if (fadeRow < 10) { // Make sure we don’t go beyond the matrix
                leds[rows[fadeRow][missilePosition]] = CRGB::Red;
                leds[rows[fadeRow][missilePosition]].fadeToBlackBy((i + 1) * 50); // Dim further down the tail
            }
        }

        // Move the missile up
        if (missileRow >= 0) {
            leds[rows[missileRow][missilePosition]] = CRGB::Red; // Draw main missile
            missileRow--;
        } else {
            // Clear any remaining tail when the missile reaches the top
            clearMissile(); // Call to clear the missile
            missileActive = false; // Deactivate missile
        }

        // Redraw the starship on the bottom row after updating the missile
        leds[rows[9][starshipPosition]] = CRGB::Blue; // Draw starship
        FastLED.show();
    }
}

// Ensure the starship is drawn after clearing the row in the main loop as well
void drawStarship() {
    leds[rows[9][starshipPosition]] = CRGB::Blue; // Draw starship at its current position
}

// Function to initialize enemies
void initEnemies() {
    // Randomly assign the enemy position between 0 and 6 (valid column positions)
    enemyPosition[0] = random(0, 7); // Random column
    enemyRow[0] = 0; // Start enemy at the top row
    enemyColors[0] = CRGB(random(255), random(255), random(255)); // Assign a random color to the enemy
    enemyActive[0] = true; // Mark enemy as active
}

// Function to draw enemies
void drawEnemies() {
    for (int i = 0; i < maxEnemies; i++) {
        if (enemyActive[i]) {
            leds[rows[enemyRow[i]][enemyPosition[i]]] = enemyColors[i]; // Draw each enemy
        }
    }
}

void clearEnemies() {
    for (int i = 0; i < maxEnemies; i++) {
        if (enemyActive[i]) {
            leds[rows[enemyRow[i]][enemyPosition[i]]] = CRGB::Black; // Clear enemy positions
        }
    }
}

// Function to move enemies down
void moveEnemies() {
    if (millis() - lastEnemyMoveTime >= enemySpeed) {
        clearEnemies(); // Clear current enemy positions

        // Move enemies down
        for (int i = 0; i < maxEnemies; i++) {
            if (enemyActive[i]) {
                enemyRow[i]++;
                if (enemyRow[i] >= 9) { // If the enemy reaches the bottom
                    enemyActive[i] = false; // Mark enemy as inactive (destroyed)
                }
            }
        }

        // Draw the enemies at their new positions
        drawEnemies();
        lastEnemyMoveTime = millis(); // Update last move time
    }
}

// Function to check for missile collisions with enemies
void checkCollisions() {
    for (int i = 0; i < maxEnemies; i++) {
        if (missileActive && enemyActive[i] && missileRow == enemyRow[i] && missilePosition == enemyPosition[i]) {
            enemyActive[i] = false; // Mark enemy as inactive (destroyed)
            clearMissile(); // Clear the missile and its tail
            missileActive = false; // Deactivate the missile
            break; // Exit loop after hit
        }
    }
}

// Function to respawn enemies if all have been destroyed
void respawnEnemies() {
    bool allDestroyed = true;
    for (int i = 0; i < maxEnemies; i++) {
        if (enemyActive[i]) {
            allDestroyed = false; // At least one enemy is still active
            break;
        }
    }

    // Respawn enemies if all have been destroyed
    if (allDestroyed) {
        initEnemies(); // Initialize new enemies
    }
}

// Function to adjust enemy speed
void adjustEnemySpeed(int change) {
    enemySpeed = constrain(enemySpeed + change, 50, 1000); // Constrain the speed between 50 ms and 1000 ms
}

// Setup function
void setup() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    initEnemies(); // Initialize enemies for the first time
    drawStarship(); // Draw the initial starship
}

// Main loop
void loop() {
    unsigned long currentTime = millis();

    // Starship movement
    if (currentTime - lastStarshipMoveTime > gameSpeed) {
        if (digitalRead(LEFT_BUTTON) == HIGH && starshipPosition > 0) {
            clearStarship(); // Clear current starship position
            starshipPosition--; // Move left
            drawStarship(); // Redraw at new position
            lastStarshipMoveTime = currentTime; // Update last move time
        }

        if (digitalRead(RIGHT_BUTTON) == HIGH && starshipPosition < 6) {
            clearStarship(); // Clear current starship position
            starshipPosition++; // Move right
            drawStarship(); // Redraw at new position
            lastStarshipMoveTime = currentTime; // Update last move time
        }
    }

    // Adjust enemy speed when RESTART_BUTTON is pressed
    if (digitalRead(RESTART_BUTTON) == HIGH) {
        adjustEnemySpeed(enemySpeedAdjustment); // Adjust speed
        delay(200); // Debounce delay
    }

    // Fire missile at intervals
    if (currentTime - lastFireTime > fireInterval && !missileActive) {
        fireMissile();
        lastFireTime = currentTime;
    }

    // Move the missile if it's active
    if (missileActive && currentTime - lastMissileMoveTime > missileSpeed) {
        moveMissile();
        lastMissileMoveTime = currentTime;
    }

    // Move enemies down and check for collisions
    moveEnemies();
    checkCollisions();
    respawnEnemies(); // Check if enemies should be respawned

    // Redraw the starship at the end of the loop
    drawStarship();
}
