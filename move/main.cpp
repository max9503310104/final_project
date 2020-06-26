#include "mbed.h"

#define CENTER_BASE 1500

PwmOut servo1(D9);  // right
PwmOut servo0(D8);  // left
Serial pc(USBTX, USBRX);
Serial uart(D1,D0); //tx,rx
RawSerial xbee(D12, D11);
DigitalInOut ping(D10);
Timer t;


void servo_control1(int speed){
    servo1 = (CENTER_BASE + speed)/20000.0f;
}

void servo_control0(int speed){
    servo0 = (CENTER_BASE + speed)/20000.0f;
}

int main() {
    int i, j, k;
    int move; // 0:front, 1:back, 2:right, 3:left, 4:detect object
    int move_state;
    float ping_val, ping_d;
    int wall_d;
    int first;
    float obj_d[4] = {0};   // distance of object
    float obj_df[3] = {0};  // first difference
    servo0.period(.02);
    servo1.period(.02);
    uart.baud(9600);
    xbee.baud(9600);

    move = 0;
    move_state = 0;
    wall_d = 21;
    first = 1;

    xbee.printf("enter map\r\n");
    while(1) {  
        if (move == 0) {        // front
            servo_control0(100);
            servo_control1(-28);
            
        }
        if (move == 1) {        // back
            servo_control0(0);
            servo_control1(0);
            wait(0.1);
            servo_control0(-100);
            servo_control1(27);
            wait(2);
            xbee.printf("go backward\r\n");
            move_state++;
            move = 0;
        }
        if (move == 2) {        // right
            servo_control0(0);
            servo_control1(0);
            wait(0.3);
            servo_control0(100);
            wait(1.30);
            xbee.printf("turn right\r\n");
            move_state++;
            move = 0;
            
        }
        if (move == 3) {        // left
            servo_control0(0);
            servo_control1(0);
            wait(0.3);
            servo_control1(-30);
            wait(1.74);
            xbee.printf("turn left\r\n");
            move_state++;
            move = 0;
            
        }
        if (move == 4) {        // detect object
            servo_control0(0);
            servo_control1(0);
            wait(1);
            xbee.printf("detect object\r\n");
            servo_control0(100);
            wait(0.3);
            
            servo_control0(0);      // measure distance of object
            wait(0.15);
            ping.output();
            ping = 0; wait_us(10);
            ping = 1; wait_us(5);
            ping = 0; wait_us(5);
            ping.input();
            while(ping.read()==0);
            t.start();
            while(ping.read()==1);
            ping_val = t.read();
            ping_d = ping_val * 17700;
            obj_d[0] = ping_d;
            t.stop();
            t.reset();

            servo_control0(100);    // measure distance of object
            wait(0.15);
            servo_control0(0);
            wait(0.15);
            ping.output();
            ping = 0; wait_us(10);
            ping = 1; wait_us(5);
            ping = 0; wait_us(5);
            ping.input();
            while(ping.read()==0);
            t.start();
            while(ping.read()==1);
            ping_val = t.read();
            ping_d = ping_val * 17700;
            obj_d[1] = ping_d;
            t.stop();
            t.reset();

            servo_control0(100);    // measure distance of object
            wait(0.15);
            servo_control0(0);
            wait(0.15);
            ping.output();
            ping = 0; wait_us(10);
            ping = 1; wait_us(5);
            ping = 0; wait_us(5);
            ping.input();
            while(ping.read()==0);
            t.start();
            while(ping.read()==1);
            ping_val = t.read();
            ping_d = ping_val * 17700;
            obj_d[2] = ping_d;
            t.stop();
            t.reset();

            servo_control0(100);    // measure distance of object
            wait(0.15);
            servo_control0(0);
            wait(0.15);
            ping.output();
            ping = 0; wait_us(10);
            ping = 1; wait_us(5);
            ping = 0; wait_us(5);
            ping.input();
            while(ping.read()==0);
            t.start();
            while(ping.read()==1);
            ping_val = t.read();
            ping_d = ping_val * 17700;
            obj_d[3] = ping_d;
            t.stop();
            t.reset();
            
            servo_control0(0);      // leave mission2
            servo_control1(0);
            wait(1);
            servo_control0(-100);
            servo_control1(27);
            wait(0.3);
            servo_control0(0);
            servo_control1(0);
            wait(0.3);
            servo_control1(-30);
            wait(0.8);

            // identify object
            xbee.printf("%.2f %.2f %.2f %.2f\r\n", obj_d[0], obj_d[1], obj_d[2], obj_d[3]);
            obj_df[0] = obj_d[1] - obj_d[0];    // first difference
            obj_df[1] = obj_d[2] - obj_d[1];
            obj_df[2] = obj_d[3] - obj_d[2];
            xbee.printf("%.2f %.2f %.2f\r\n", obj_df[0], obj_df[1], obj_df[2]);
            if (obj_df[1] > 0) {
                xbee.printf("object 3\r\n");
            } else if (obj_df[0] < -1) {
                if (obj_df[2] > 1) {
                    xbee.printf("object 2\r\n");
                } else {
                    xbee.printf("object 1\r\n");
                }
            } else {
                xbee.printf("object 0\r\n");
            }

            
            move_state++;
            move = 0;
        }

        if (move_state == 5 && first == 1) {    // take photo
            servo_control0(0);
            servo_control1(0);

            char s[21];
            sprintf(s,"image_classification");
            uart.puts(s);
            pc.printf("image classification\r\n");
            xbee.printf("image\r\n");
            wait(0.2);
            char recv = uart.getc();
            pc.printf("%c\r\n", recv);
            xbee.printf("%c\r\n", recv);
            first = 0;
            move = 0;
        }

        // measure distance
        ping.output();
        ping = 0; wait_us(10);
        ping = 1; wait_us(5);
        ping = 0; wait_us(5);
        ping.input();
        while(ping.read()==0);
        t.start();
        while(ping.read()==1);
        ping_val = t.read();
        ping_d = ping_val * 17700;
        printf("Ping = %lf\r\n", ping_d);
        t.stop();
        t.reset();

        if (ping_d < wall_d) {
            /*if (move_state == 0) {
                move = 3;
            }*/
            switch (move_state) {   // change moving direction and the distance of wall
                case 0: move = 3, wall_d = 18; break;    // L
                case 1: move = 2, wall_d = 20; break;    // R
                case 2: move = 1, wall_d = 27; break;    // B
                case 3: move = 2, wall_d = 18; break;    // R
                case 4: move = 3; break;    // L
                case 5: move = 2, wall_d = 37; break;    // R
                case 6: move = 2, wall_d = 17; break;    // R
                case 7: move = 2, wall_d = 18; break;    // R
                case 8: move = 4, wall_d = 18; break;   // object
                case 9: move = 2; break;    // R
            }
            printf("state = %d\r\n", move_state);
        }
        if (move_state == 10) {
            move_state++;
            xbee.printf("exit\r\n");
        }

        
        wait(0.03);
        
    }
}