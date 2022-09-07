#include <Arduino.h>
#include <Stepper.h>
#include <U8x8lib.h>

#define direction_Pin_Base 4
#define step_Pin_Base 5

#define direction_Pin_Drawer 6
#define step_Pin_Drawer 7

#define total_steps 3200

// declearation
U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(U8X8_PIN_NONE);
int i = 0;
int lightMode = 0;

// Stepper stepper_drawer(800, direction_Pin_Drawer, step_Pin_Drawer);
// Stepper stepper_base(800, direction_Pin_Base, step_Pin_Base);

Stepper stepper_drawer(800, 4, 5);
Stepper stepper_base(800, 6, 7);

const int buttonPin_0 = 13;
const int buttonPin_1 = 10;
const int buttonPin_2 = 11;
const int buttonPin_3 = 8;
const int buttonPin_Test = 8;

//for read button0
int val_0 = 1;
int state_0 = 0;
int last_val_0 = 1;

//for read button1
int val_1 = 1;
int state_1 = 0;
int last_val_1 = 1;

//for read button2
int val_2 = 1;
int state_2 = 0;
int last_val_2 = 1;

//for read button3
int val_3 = 1;
int state_3 = 0;
int last_val_3 = 1;

//for read button_test
int val_test = 1;
int state_test = 0;
int last_val_test = 1;

int val[5] = {};
int state[5] = {};
int last_val[5] = {};
int button_Pin[5] = {buttonPin_0, buttonPin_1, buttonPin_2, buttonPin_3, buttonPin_Test};


int test = 0;

int game_index = 0;

int number_of_player = 2;

int accumulate_step = 0;
int per_rotate_step = 0;

// whether countinue the game
int countinue = 0;

String menu[3] = {
    "game mode",
    "number of player",
    "number of player",
};

String game_name[3] = {
    "Texas          ",
    "Fight landowner",
    "Upgrade        ",
};

void draw() {
    stepper_drawer.step(2400);
}

void rotate(int step) {
    stepper_base.step(step);
    accumulate_step += per_rotate_step;
}

void back_init_position() {
    stepper_base.step(total_steps - accumulate_step);
    accumulate_step = 0;
}

void clear_write(String str) {
    u8x8.clearDisplay();
    u8x8.setCursor(0, 0);
    u8x8.print(str);
}

void next_write(String str) {
    u8x8.print("\n");
    u8x8.print(str);
}

void simple_block(int pin_Index) {
    //TODO: need use array manage varible that used to manage button
    // wait
    while(1) {
        val[pin_Index] = digitalRead(button_Pin[pin_Index]);
        if ((val[pin_Index] == HIGH) && (last_val[pin_Index] == LOW)) {
            last_val[pin_Index] = val[pin_Index];
            break;
        }
        last_val[pin_Index] = val[pin_Index];
    }

}

void Texas(int player_number) {

    // init player, position, and angle
    int per_rotate_angle = 360 / player_number;
    per_rotate_step = 1 / player_number * 3200;

restart:
    // TODO: need interrupt function
    clear_write("Pre-floding");
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < player_number; j++) {
            //draw
            draw();
            delay(50);
            //rotate
            rotate(per_rotate_step);
        }
        // roll_back
        back_init_position();
    }

    next_write("Pre-flod finish");

    // wait
    next_write("Wait Flop");
  
    while(1) {
        val_3 = digitalRead(buttonPin_3);
        if ((val_3 == HIGH) && (last_val_3 == LOW)) {
            last_val_3 = val_3;
            break;
        }
        last_val_3 = val_3;

        // check interruption
        val_0 = digitalRead(buttonPin_0);
        if ((val_0 == HIGH) && (last_val_0 == LOW)) {
            last_val_0 = val_0;
            goto Texas_Finish;
        }
        last_val_0 = val_0;

    }

    // flop
    clear_write("Floping");
    for(int i = 0; i < 3; i++) {
        draw();
        delay(50);
        //rotate
    }

    delay(20);
    next_write("Flop finish");
    next_write("Wait turn");

    // wait
    while(1) {
        val_3 = digitalRead(buttonPin_3);
        if ((val_3 == HIGH) && (last_val_3 == LOW)) {
            last_val_3 = val_3;
            break;
        }
        last_val_3 = val_3;
    }

    // turn
    clear_write("Turning");
    draw();
    delay(20);
    next_write("Turn finish");
    next_write("Wait river");

    // wait
    while(1) {
        val_3 = digitalRead(buttonPin_3);
        if ((val_3 == HIGH) && (last_val_3 == LOW)) {
            last_val_3 = val_3;
            break;
        }
        last_val_3 = val_3;
    }

    // river
    clear_write("Rivering");
    draw();

    // do_something_last
    next_write("River finish");
    next_write("SHOW!!!");

    while(1) {
        val_3 = digitalRead(buttonPin_3);
        if ((val_3 == HIGH) && (last_val_3 == LOW)) {
            last_val_3 = val_3;
            break;
        }
        last_val_3 = val_3;
    }

    clear_write("Game over");
    next_write("Press to continue");
    next_write("Press to end");
    while(1) {
        val_3 = digitalRead(buttonPin_3);
        if ((val_3 == HIGH) && (last_val_3 == LOW)) {
            last_val_3 = val_3;
            break;
        }
        last_val_3 = val_3;

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            clear_write("Put decker back");
            while(1) {
                val_2 = digitalRead(buttonPin_2);
                if ((val_2 == HIGH) && (last_val_2 == LOW)) {
                    last_val_2 = val_2;
                    break;
                }
                last_val_2 = val_2;
            }
            stepper_base.step(per_rotate_step);
            goto restart;
        }
        last_val_2 = val_2;
    }

Texas_Finish: 
    u8x8.clearDisplay();
    u8x8.setCursor(0, 0);
}

void Fight_landowner() {
    clear_write("Let's Fight!");
    next_write("Press to start");
    while(1) {
        val_3 = digitalRead(buttonPin_3);
        if ((val_3 == HIGH) && (last_val_3 == LOW)) {
            last_val_3 = val_3;
            break;
        }
        last_val_3 = val_3;
    }
    clear_write("Dealing");
    for(int i = 0; i < 17; i++) {
        for(int j = 0; j < 3; j++) {
            // draw
            draw();
            delay(50);
            // rotate
        }
    }
    next_write("Now pub card");

    for(int i = 0; i < 3; i++) {
        // draw
        draw();
        delay(50);
        // rotate in small angle
        rotate(300);
    }
    clear_write("Dealing done!");
    next_write("Game Start!");
    
    next_write("Press to back");
    while(1) {
        val_3 = digitalRead(buttonPin_3);
        if ((val_3 == HIGH) && (last_val_3 == LOW)) {
            last_val_3 = val_3;
            break;
        }
        last_val_3 = val_3;
    }

}

void Upgrade() {
    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < 4; j++) {
            // draw
            draw();
            delay(50);
            // rotate
        }
    }
}




void start_game(int game_index, int player_number) {
    switch (game_index)
    {
    case 0:
        /* code */
        clear_write("Texas start!");
        next_write("Press to Continue!");
        while(1) {
            val_3 = digitalRead(buttonPin_3);
            if ((val_3 == HIGH) && (last_val_3 == LOW)) {
                last_val_3 = val_3;
                break;
            }
            last_val_3 = val_3;
        }
        u8x8.clearDisplay();
        Texas(number_of_player);
        break;

    case 1:
        Fight_landowner();
        break;

    case 2:
        Upgrade();
        break;
   
    default:
        break;
    }
}

void setup() {
    pinMode(direction_Pin_Base, OUTPUT);
    pinMode(step_Pin_Base, OUTPUT);
    pinMode(direction_Pin_Drawer, OUTPUT);
    pinMode(step_Pin_Drawer, OUTPUT);

    val_0 = 0;
    val_1 = 0;
    val_2 = 0;


    stepper_drawer.setSpeed(1800);

    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop() {

    val_0 = digitalRead(buttonPin_0);
    if ((val_0 == HIGH) && (last_val_0 == LOW)) {
        state_0 = 1 - state_0;
        if (game_index == 2) {
            game_index = 0;
        } else {
            game_index += 1;
        }
        delay(20);
    }
    last_val_0 = val_0;

    val_1 = digitalRead(buttonPin_1);
    if ((val_1 == HIGH) && (last_val_1 == LOW)) {
        state_1 = 1 - state_1;
        if (number_of_player == 9) {
            number_of_player = 2;
        } else {
            number_of_player += 1;
        }
        delay(20);
    }
    last_val_1 = val_1;

    val_2 = digitalRead(buttonPin_2);
    if ((val_2 == HIGH) && (last_val_2 == LOW)) {
        // here start the game
        start_game(game_index, number_of_player);
        u8x8.clearDisplay();
        u8x8.setCursor(0, 0);
        // state_2 = 1 - state_2;
        // if (number_of_player == 2) {
        //     number_of_player = 9;
        // } else {
        //     number_of_player -= 1;
        // }
        // delay(20);
    }
    last_val_2 = val_2;


    u8x8.setFlipMode(1);
    u8x8.setCursor(0, 0);
    u8x8.print(menu[1]);
    u8x8.print("\n");
    u8x8.print(game_name[game_index]);
    u8x8.print("\n");
    if (game_index == 0) {
        u8x8.print("Number of player");
        u8x8.print("\n");
        u8x8.print(number_of_player);
    } else {
        u8x8.print("                ");
        u8x8.print("\n");
        u8x8.print("                ");
    }



}