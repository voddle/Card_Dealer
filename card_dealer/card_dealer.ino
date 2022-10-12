#include <Arduino.h>
#include <Stepper.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>

#define direction_Pin_Base 5
#define step_Pin_Base 4

#define direction_Pin_Drawer 7
#define step_Pin_Drawer 6

#define total_steps 3200

// declearation
U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(U8X8_PIN_NONE);
int i = 0;
int lightMode = 0;

// Stepper stepper_drawer(800, direction_Pin_Drawer, step_Pin_Drawer);
// Stepper stepper_base(800, direction_Pin_Base, step_Pin_Base);

Stepper stepper_drawer(800, 5, 4);
Stepper stepper_base(800, 7, 6);

const int buttonPin_0 = 13;
const int buttonPin_1 = 12;
const int buttonPin_2 = 10;
const int buttonPin_3 = 11;
const int buttonPin_Test = 8;

// const int trigPin = 2;
// const int echopin = 3;
// TXD 2, RXD 3
SoftwareSerial BT(3, 2);

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

//init some array
int val[5] = {};
int state[5] = {};
int last_val[5] = {};
int button_Pin[5] = {buttonPin_0, buttonPin_1, buttonPin_2, buttonPin_3, buttonPin_Test};

// for black jack to skip player
bool record[9] = {true, true, true, true, true, true, true, true, true};

// for black jack
int player_index = 0;

// test value
int test = 0;

// index for game mode
int game_index = 0;

// init player number
int number_of_player = 2;

// init value for black jack
int left_player = 2;

// init step related value
// value for rotate back
int accumulate_step = 0;
// value for per step
int per_rotate_step = 0;

// value for Bluetooth read
int bt_read = -1;

// whether countinue the game
int countinue = 0;

// game menu array
String menu[3] = {
    "game mode",
    "number of player",
    "number of player",
};

// game mode array
String game_name[6] = {
    "Texas          ",
    "Fight landowner",
    "Upgrade        ",
    "Three Card     ",
    "Bluff          ",
    "Black Jack     ",

};

// draw function, rotate drawer and rotate back to 
// insure deck posture
void draw() {
    stepper_drawer.step(-3000);
	delay(40);
    stepper_drawer.step(2400);
}

// base rotate funtion, need step value as input
void rotate(int step) {
    stepper_base.step(step);
    accumulate_step += step;
    stepper_drawer.step(-200);
}

// rotate base stepper to initiate position since the wire may tangle
void back_init_position() {
    stepper_base.step(-accumulate_step);
    accumulate_step = 0;
}

// clear the display and write on top
void clear_write(String str) {
    u8x8.clearDisplay();
    u8x8.setCursor(0, 0);
    u8x8.print(str);
}

// write on next line
void next_write(String str) {
    u8x8.print("\n");
    u8x8.print(str);
}

// maybe this is wrong since simple block cannot do the interrupt and goto specific place
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

// Texas game
void Texas(int player_number) {

    // init player, position, and angle
    int per_rotate_angle = 360 / player_number;
    per_rotate_step = 1.0 / player_number * 3200;

restart:
    // TODO: need interrupt function
    clear_write("Pre-floding");
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < player_number - 1; j++) {
            //draw
            draw();
            //rotate
            rotate(per_rotate_step);
        }
        // here isolate last draw to save one rotate
        // draw
        draw();
        // roll_back
        back_init_position();
    }

    // write 
    next_write("Pre-flod finish");

    // wait
    next_write("Wait Flop");
  
    // wait
    while(1) {
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }
        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;

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
        // draw
        draw();
        // delay
        delay(50);
        //rotate
    }

    // delay
    delay(20);

    // print new info
    next_write("Flop finish");
    next_write("Wait turn");
    next_write("Press to Con");

    // wait
    while(1) {
        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    // turn
    clear_write("Turning");
    draw();
    delay(20);
    next_write("Turn finish");
    next_write("Wait river");

    // wait
    while(1) {
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    // river
    clear_write("Rivering");
    draw();

    // do_something_last
    next_write("River finish");
    next_write("SHOW!!!");

    // wait
    while(1) {
        // blue tooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    // print finish info
    clear_write("Game over");
    next_write("Press to");
    next_write("continue");
    next_write("Press to end");

    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();

            // go back to restart
            if (bt_read == '1'){
                bt_read = -1;
                stepper_base.step(per_rotate_step);
                clear_write("BB moved");
                next_write("Put decker back");
                goto wait_next_Texas;
            }

            // continue
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        // continue
        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;

        // go back to restart
        val_1 = digitalRead(buttonPin_1);
        if ((val_1 == HIGH) && (last_val_1 == LOW)) {
            last_val_1 = val_1;
            stepper_base.step(per_rotate_step);
            clear_write("BB moved");
            next_write("Put decker back");
            while(1) {
// here only goes to finish
wait_next_Texas:

                // bluetooth read
                if (BT.available()) {
                    bt_read = BT.read();
                    if (bt_read == '2') {
                        bt_read = -1;
                        break;
                    }
                    bt_read = -1;
                }

                // button read
                val_2 = digitalRead(buttonPin_2);
                if ((val_2 == HIGH) && (last_val_2 == LOW)) {
                    last_val_2 = val_2;
                    break;
                }
                last_val_2 = val_2;
            }

            // restart!
            goto restart;
        }

        last_val_1 = val_1;
    }

// here game finish
Texas_Finish: 
    u8x8.clearDisplay();
    u8x8.setCursor(0, 0);
}

// Dou Di Zhu !!!
void Fight_landowner() {
// for restart
fight_landowner:

    // print start info
    clear_write("Let's Fight!");
    next_write("Press to start");
    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    // start dealing
    clear_write("Dealing");
    for(int i = 0; i < 17; i++) {
        for(int j = 0; j < 3 - 1; j++) {
            // draw
            draw();
            delay(50);
            // rotate
            rotate(1066);
        }
        draw();
        back_init_position();
    }

    // last three card
    clear_write("Press to show");
    next_write("Pub card");
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;

    }
    next_write("Now pub card");

    for(int i = 0; i < 3 - 1; i++) {
        // draw
        draw();
        delay(50);
        // rotate in small angle
    }

    // last draw
    draw();

    // back
    back_init_position();

    // Call bid and start game
    clear_write("Dealing done!");
    next_write("Game Start!");
    
    next_write("Press to back");
    next_write("Or continue");

    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();

            // restart
            if (bt_read == '1'){
                bt_read = -1;
                // change land lord
                stepper_base.step(1066);

                //jump to the beginning
                goto fight_landowner;
            }

            // just end
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_1 = digitalRead(buttonPin_1);
        if ((val_1 == HIGH) && (last_val_1 == LOW)) {
            last_val_1 = val_1;
            // change land lord
            stepper_base.step(1066);

            //jump to the beginning
            goto fight_landowner;
        }
        last_val_1 = val_1;


        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            // just end
            break;
        }
        last_val_2 = val_2;
    }

}

// Upgrade Game
void Upgrade() {
// this is for restart
upgrade:

    // print info
    clear_write("Let's Upgrade!");
    next_write("Press to start");

    // wait
    while(1) {
        
        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    // start dealing card
    clear_write("Dealing");

    // for four people
    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < 4 - 1; j++) {
            // draw
            draw();
            delay(50);
            // rotate
            rotate(800);
        }
        // last draw
        draw();
        // back position
        back_init_position();
    }

    // print info
    clear_write("Dealing done!");
    next_write("Game Start!");
    
    next_write("Press to back");
    next_write("Or continue");

    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();

            if (bt_read == '1'){
                bt_read = -1;
                // change banker
                stepper_base.step(800);
                //jump to the biginning
                goto upgrade;
            }

            if (bt_read == '2'){
                bt_read = -1;
                // just end
                break;
            }
            bt_read = -1;
        }

        val_1 = digitalRead(buttonPin_1);
        if ((val_1 == HIGH) && (last_val_1 == LOW)) {
            last_val_1 = val_1;
            // change banker
            stepper_base.step(800);
            //jump to the biginning
            goto upgrade;
        }
        last_val_1 = val_1;


        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            // just end
            break;
        }
        last_val_2 = val_2;
    }
}

// Zha Jin Hua !!!
void Three_Card(int player_number) {

    // init player, position, and angle
    int per_rotate_angle = 360 / player_number;
    per_rotate_step = 1.0 / player_number * 3200;

// this is for restart
three_card:

    // print info
    clear_write("Let's Play!");
    next_write("Press to start");

    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }
    clear_write("Dealing");

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < player_number - 1; j++) {
            // draw
            draw();
            delay(50);
            // rotate
            rotate(per_rotate_step);
        }
        draw();
        back_init_position();
    }

    clear_write("Dealing done!");
    next_write("Start Bet!");
    next_write("Press to");
    next_write("Continuew");

    while(1) {
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    clear_write("Show!");
    next_write("Press to back");
    next_write("Or continue");
    while(1) {
        if (BT.available()) {
            bt_read = BT.read();

            if (bt_read == '1'){
                bt_read = -1;
                // chagne banker
                stepper_base.step(per_rotate_step);
                // restart game
                goto three_card;
            }

            if (bt_read == '2'){
                bt_read = -1;
                // just end
                break;
            }
            bt_read = -1;
        }

        val_1 = digitalRead(buttonPin_1);
        if ((val_1 == HIGH) && (last_val_1 == LOW)) {
            last_val_1 = val_1;
            // chagne banker
            stepper_base.step(per_rotate_step);
            // restart game
            goto three_card;
        }
        last_val_1 = val_1;


        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            // just end
            break;
        }
        last_val_2 = val_2;
    }
}

// Chui niu!!!
void Bluff(int player_number) {
    // init player, position, and angle
    int per_rotate_angle = 360 / player_number;
    per_rotate_step = 1.0 / player_number * 3200;

// this is for restart
bluff:

    // print info
    clear_write("Let's Bluff!");
    next_write("Press to");
    next_write("Start");

    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    // print info
    clear_write("Dealing");

    for(int i = 0; i < 54 / player_number; i++) {
        for(int j = 0; j < player_number - 1; j++) {
            // draw
            draw();
            delay(50);
            // rotate
            rotate(per_rotate_step);
        }
        draw();
        back_init_position();
    }

    clear_write("Dealing done!");
    next_write("Start Bluff!");
    next_write("Press to");
    next_write("Continue");

    while(1) {
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    clear_write("Press to back");
    next_write("Or continue");
    while(1) {
        if (BT.available()) {
            bt_read = BT.read();

            if (bt_read == '1'){
                bt_read = -1;
                // change banker
                stepper_base.step(per_rotate_step);
                // restart
                goto bluff;
            }

            if (bt_read == '2'){
                bt_read = -1;
                // just end
                break;
            }
            bt_read = -1;
        }

        val_1 = digitalRead(buttonPin_1);
        if ((val_1 == HIGH) && (last_val_1 == LOW)) {
            last_val_1 = val_1;
            // change banker
            stepper_base.step(per_rotate_step);
            // restart
            goto bluff;
        }
        last_val_1 = val_1;


        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            // just end
            break;
        }
        last_val_2 = val_2;
    }
}

void Black_Jack(int player_number) {
    // init player, position, and angle
    int per_rotate_angle = 360 / player_number;
    per_rotate_step = 1.0 / player_number * 3200;

black_jack:
    // print info
    clear_write("Let's Bluff!");
    next_write("Press to");
    next_write("Start");

    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    // print info
    clear_write("Dealing");

    next_write("Face down card");

    for(int j = 0; j < player_number - 1; j++) {
        // draw
        draw();
        delay(50);
        // rotate
        rotate(per_rotate_step);
    }
    draw();
    back_init_position();

    next_write("Face up card");

    for(int j = 0; j < player_number - 1; j++) {
        // draw
        draw();
        delay(50);
        // rotate
        rotate(per_rotate_step);
    }
    draw();
    back_init_position();

    left_player = player_number;

    clear_write("Deal finish");
    next_write("Start Call");
    next_write("Press to");
    next_write("continue");

    // wait
    while(1) {

        // bluetooth read
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }


    // // print info
    // clear_write("Player");
    // u8x8.print(left_player);
    // next_write("Stand Or");
    // next_write("Hit");

    for(int i = 0; i < player_number; i++) {
        record[i] = true;
    }

    player_index = 1;

    // wait
    while(left_player > 0) {

        while (!record[player_index]){
            if (player_index == player_number) {
                player_index = 1;
                back_init_position();
            } else {
                player_index += 1;
                rotate(per_rotate_step);
            }
        }

        // print info
        clear_write("Player");
        u8x8.print(player_index);
        next_write("Stand Or");
        next_write("Hit");

        while(1) {

            // bluetooth read
            if (BT.available()) {
                bt_read = BT.read();

                // stand
                if (bt_read == '1'){
                    bt_read = -1;
                    draw();
                    break;
                }

                // hit
                if (bt_read == '2'){
                    bt_read = -1;
                    left_player -= 1;
                    record[player_index] = false;
                    break;
                }
                bt_read = -1;
            }

            // stand
            val_1 = digitalRead(buttonPin_1);
            if ((val_1 == HIGH) && (last_val_1 == LOW)) {
                last_val_1 = val_1;
                draw();
                break;
            }
            last_val_1 = val_1;

            // hit
            val_2 = digitalRead(buttonPin_2);
            if ((val_2 == HIGH) && (last_val_2 == LOW)) {
                last_val_2 = val_2;
                left_player -= 1;
                record[player_index] = false;
                break;
            }
            last_val_2 = val_2;
        }

        if (player_index == player_number) {
            player_index = 1;
            back_init_position();
        } else {
            player_index += 1;
            rotate(per_rotate_step);
        }
    }

    back_init_position();

    clear_write("Nobody can hit");
    next_write("Show!!!");
    next_write("Press to");
    next_write("continue");

    while(1) {
        if (BT.available()) {
            bt_read = BT.read();
            if (bt_read == '2'){
                bt_read = -1;
                break;
            }
            bt_read = -1;
        }

        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            break;
        }
        last_val_2 = val_2;
    }

    clear_write("Press to back");
    next_write("Or continue");
    while(1) {
        if (BT.available()) {
            bt_read = BT.read();

            if (bt_read == '1'){
                bt_read = -1;
                // change banker
                stepper_base.step(per_rotate_step);
                // restart
                goto black_jack;
            }

            if (bt_read == '2'){
                bt_read = -1;
                // just end
                break;
            }
            bt_read = -1;
        }

        val_1 = digitalRead(buttonPin_1);
        if ((val_1 == HIGH) && (last_val_1 == LOW)) {
            last_val_1 = val_1;
            // change banker
            stepper_base.step(per_rotate_step);
            // restart
            goto black_jack;
        }
        last_val_1 = val_1;


        val_2 = digitalRead(buttonPin_2);
        if ((val_2 == HIGH) && (last_val_2 == LOW)) {
            last_val_2 = val_2;
            // just end
            break;
        }
        last_val_2 = val_2;
    }

}

void start_game(int game_index, int player_number) {
    delay(20);
    switch (game_index)
    {
    case 0:
        /* code */
        clear_write("Texas start!");
        next_write("Press to Continue!");
        while(1) {
            if (BT.available() > 0) {
                bt_read = BT.read();
                if (bt_read == '2'){
                    bt_read = -1;
                    break;
                }
            }
            val_2 = digitalRead(buttonPin_2);
            if ((val_2 == HIGH) && (last_val_2 == LOW)) {
                last_val_2 = val_2;
                break;
            }
            last_val_2 = val_2;
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

    case 3:
        Three_Card(player_number);
        break;

    case 4:
        Bluff(player_number);
        break;

    case 5:
        Black_Jack(player_number);
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
    Serial.begin(9600);
    pinMode(13, INPUT);

    // read value init
    val_0 = 0;
    val_1 = 0;
    val_2 = 0;

    // speed init
    // drawer is up to 1650
    // base now knows can go to 400 
    stepper_drawer.setSpeed(1650);
    stepper_base.setSpeed(100);

    // oled init
    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);

    // BT init
    // HC-05
    // 1234
    BT.begin(9600);
}

void loop() {

    // bluetooth read
    if (BT.available() > 0) {
        bt_read = BT.read();
        Serial.print(bt_read);
        switch (bt_read)
        {
        // botton 0
        case '0':
            if (game_index == 5) {
                game_index = 0;
            } else {
                game_index += 1;

                // limit player number
                if (game_index == 4 && number_of_player > 4) {
                    number_of_player = 4;
                }
            }
            break;

        // botton 1
        case '1':
            if (game_index == 0 || game_index == 3 || game_index == 5) {
                if (number_of_player == 9) {
                    number_of_player = 2;
                } else {
                    number_of_player += 1;
                }
            } else if (game_index == 4) {
                if (number_of_player >= 4) {
                    number_of_player = 2;
                } else {
                    number_of_player += 1;
                }
            }
            break;

        // botton 2
        case '2':
            bt_read = -1;
            start_game(game_index, number_of_player);
            u8x8.clearDisplay();
            u8x8.setCursor(0, 0);
            break;

        
        default:
            break;
        }
        bt_read = -1;
    }

    // read for game index
    val_0 = digitalRead(buttonPin_0);
    if ((val_0 == HIGH) && (last_val_0 == LOW)) {
        state_0 = 1 - state_0;
        if (game_index == 5) {
            game_index = 0;
        } else {
            game_index += 1;

            // limit player number
            if (game_index == 4 && number_of_player > 4) {
                number_of_player = 4;
            }
        }
        delay(20);
    }
    last_val_0 = val_0;

    // read for player number
    val_1 = digitalRead(buttonPin_1);
    if ((val_1 == HIGH) && (last_val_1 == LOW)) {
        state_1 = 1 - state_1;
        if (game_index == 0 || game_index == 3 || game_index == 5) {
            if (number_of_player == 9) {
                number_of_player = 2;
            } else {
                number_of_player += 1;
            }
            delay(20);
        } else if (game_index == 4) {
            if (number_of_player >= 4) {
                number_of_player = 2;
            } else {
                number_of_player += 1;
            }
            delay(20);
        }
    }
    last_val_1 = val_1;

    val_2 = digitalRead(buttonPin_2);
    if ((val_2 == HIGH) && (last_val_2 == LOW)) {
        // here start the game
        last_val_2 = val_2;
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

    // print info
    u8x8.setFlipMode(1);
    u8x8.setCursor(0, 0);
    u8x8.print(menu[1]);
    u8x8.print("\n");
    u8x8.print(game_name[game_index]);
    u8x8.print("\n");

    // only show player number when needed
    if (game_index == 0 || game_index == 3 || game_index == 4 || game_index == 5) {
        u8x8.print("Number of player");
        u8x8.print("\n");
        u8x8.print(number_of_player);
    } else {
        u8x8.print("                ");
        u8x8.print("\n");
        u8x8.print("                ");
    }

}
