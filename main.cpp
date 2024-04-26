#include "mbed.h"
#include "uLCD_4DGL.h"
#include <ctime>
#include <cstdlib>
#include <vector>


// Define pins for LEDs
DigitalOut led1(p14);
DigitalOut led2(p15);
DigitalOut led3(p13);

// Define pins for tactile switch
DigitalIn switchUp(p17);
DigitalIn switchDown(p18);
DigitalIn switchLeft(p19);
DigitalIn switchRight(p20);
DigitalIn switchCenter(p21);

uLCD_4DGL lcd(p9, p10, p11); // Initialize uLCD display

class Nav_Switch
{
public:
    Nav_Switch(PinName up,PinName down,PinName left,PinName right,PinName fire);
    int read();
//boolean functions to test each switch
    bool up();
    bool down();
    bool left();
    bool right();
    bool fire();
//automatic read on RHS
    operator int ();
//index to any switch array style
    bool operator[](int index) {
        return _pins[index];
    };
private:
    BusIn _pins;

};
Nav_Switch::Nav_Switch (PinName up,PinName down,PinName left,PinName right,PinName fire):
    _pins(up, down, left, right, fire)
{
    _pins.mode(PullUp); //needed if pullups not on board or a bare nav switch is used - delete otherwise
    wait(0.001); //delays just a bit for pullups to pull inputs high
}
inline bool Nav_Switch::up()
{
    return !(_pins[0]);
}
inline bool Nav_Switch::down()
{
    return !(_pins[1]);
}
inline bool Nav_Switch::left()
{
    return !(_pins[2]);
}
inline bool Nav_Switch::right()
{
    return !(_pins[3]);
}
inline bool Nav_Switch::fire()
{
    return !(_pins[4]);
}
inline int Nav_Switch::read()
{
    return _pins.read();
}
inline Nav_Switch::operator int ()
{
    return _pins.read();
}
Nav_Switch myNav( p17, p18, p19, p20, p21);

class SnakeGame {
private:
    int screenWidth;
    int screenHeight;
    int eggX;
    int eggY;
    int snakex;
    int snakey;
    int score;
    std::vector<std::pair<int, int> > snakeBody;

public:
    SnakeGame(int width, int height) : screenWidth(width), screenHeight(height) {
        lcd.background_color(BLACK);
        lcd.cls();
        snakex = screenWidth / 2; // Initial snake position (centered)
        snakey = screenHeight / 2;
        score = 0;
        placeEggRandomly();
    }

    void drawSnake(int x, int y) {
        lcd.filled_rectangle(x, y, x + 10, y + 10, GREEN);
        wait(1);
        lcd.cls();
    }

    void addToSnakeBody(int x, int y) {
        snakeBody.push_back(std::make_pair(x, y));
    }

    bool snakeCollidedWithBody() {
        if ((snakex == eggX)&&(snakey == eggY)){
            score += 1;
            placeEggRandomly();
        }
    }

    void drawEgg(int x, int y) {
        lcd.filled_rectangle(x, y, x+10, y+10, RED);
    }

    void placeEggRandomly() {
        eggX = (10*(rand() % 12)) + 4;
        eggY = (10*(rand() % 12)) + 4;
        drawEgg(eggX, eggY);
    }

    void startgame() {
        if (switchCenter == 1) {
            update();
        }
    }

    void update() {
        // Read switch inputs and update game logic
        while(1) {
        if (myNav.up()) {
            snakey -= 10; // Move snake up by 10 pixels
        }
        if (myNav.down()) {
            snakey += 10; // Move snake down by 10 pixels
        }
        if (myNav.right()) {
            snakex += 10; // Move snake left by 10 pixels
        }
        if (myNav.left()) {
            snakex -= 10; // Move snake right by 10 pixels
        }

        drawSnake(snakex,snakey);

        // Check collision with egg
        if ((eggX == snakex) && (eggY == snakey)) {
            score += 1;
            addToSnakeBody(snakex, snakey); // Grow the snake
            led1 = 1; // Toggle LED state
            wait(0.2);
            led1 = 0;
            placeEggRandomly(); // Place a new egg
        }

        drawEgg(eggX,eggY);

        // Check collision with screen boundaries or snake body
        if (snakex < 0 || snakex >= screenWidth || snakey < 0 || snakey >= screenHeight) {
            led2 = 1;
            printf("Game over\n");
            lcd.cls(); // Clear the screen
            lcd.text_width(1); //4x size text
            lcd.text_height(1);
            lcd.color(RED);
            lcd.locate(1,2);
            lcd.printf("GAME OVER");
            lcd.printf("        Your Score Is:");
            char stringValue[20];
            sprintf(stringValue, "%d", score);
            lcd.printf("%s", stringValue);
            wait(20);
            lcd.reset(); // Reset the screen
            lcd.cls();
            lcd.text_width(1); //1x size text
            lcd.text_height(1);
            lcd.color(WHITE);
            exit(1);
        }

        if (score == 3){
            led3 = 1;
            printf("Game over\n");
            lcd.cls(); // Clear the screen
            lcd.text_width(1);
            lcd.text_height(1);
            lcd.color(BLUE);
            lcd.locate(1,2);
            lcd.printf("GAME OVER");
            lcd.printf("        You Won!");
            wait(20);
            lcd.reset(); // Reset the screen
            lcd.cls();
            lcd.text_width(1); //1x size text
            lcd.text_height(1);
            lcd.color(WHITE);
            exit(1);

        }
        }

        // Update LED states
         // Turn LED2 off
    }
};

int main() {
    srand(time(NULL)); // Seed the random number generator
    SnakeGame game(128, 128); // Set screen dimensions

    while (1) {
        game.update();
        wait(0.005); // Add a delay to control game speed
    }
}
