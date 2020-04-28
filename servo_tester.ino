#include <Arduino.h>
#include <TextCMD.h>            // https://github.com/jozef/Arduino-TextCMD
#include <Servo.h>

const uint8_t SERVO_PIN = 9;  // attaches the servo on pin 8 to the servo object

Servo myservo;

cmd_dispatch commands[] = {
    { "?",     &cmd_help     },
    { "sweep", &cmd_servo_sweep },
    { "s",     &cmd_servo },
    { "us",    &cmd_us }
};
TextCMD cmd((sizeof(commands)/sizeof(commands[0])),commands);

void setup () {
    Serial.begin(9600);
    cmd.do_dispatch("?");
}

void loop () {
    while (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\b') { Serial.print(F("\b \b")); }
        else { Serial.print(ch); }
        switch (cmd.add_char(ch)) {
            case -1: Serial.println(F("unknown command or syntax error. send '?' for help")); break;
        }
    }
    delay(100);
}

int8_t cmd_help(uint8_t argc, const char* argv[]) {
    Serial.print(F("servo signal pin is "));
    Serial.println(SERVO_PIN);
    Serial.println(F("supported commands:"));
    Serial.println(F("    s [int]              - turn servo to 0-180"));
    Serial.println(F("    us [int]             - set puls width 544-? (2400)"));
    Serial.println(F("    sweep [int] [int]    - sweep 0-180-0, optional step delay and count"));
    Serial.println(F("    ?                    - print this help"));
    return 0;
}

int8_t cmd_servo(uint8_t argc, const char* argv[]) {
    if (argc != 2) return -1;
    int pos = String(argv[1]).toInt();

    set_servo_pos(pos);

    return 0;
}

int8_t cmd_us(uint8_t argc, const char* argv[]) {
    if (argc != 2) return -1;
    int us = String(argv[1]).toInt();

    set_servo_us(us);

    return 0;
}

int8_t cmd_servo_sweep(uint8_t argc, const char* argv[]) {
    int delay_time = 15;
    int loop_count = 1;
    if (argc >= 2) {;
        delay_time = String(argv[1]).toInt();
        if (delay_time <= 0) {
            Serial.println(F("delay has to be positive, non-zero int"));
            return -1;
        }
        if (argc >= 3) {;
            loop_count = String(argv[2]).toInt();
            if (loop_count <= 0) {
                Serial.println(F("loop count has to be positive, non-zero int"));
                return -1;
            }
        }
    }

    for (uint8_t loop = 0; loop < loop_count; loop++) {
        Serial.print(F("Sweep "));
        Serial.print(loop+1);
        Serial.print(F(" of "));
        Serial.println(loop_count);

        for (uint8_t pos = 90; pos < 180; pos++) {
            set_servo_pos(pos);
            delay(delay_time);
        }
        delay(delay_time);
        for (uint8_t pos = 179; pos != 0; pos--) {
            set_servo_pos(pos);
            delay(delay_time);
        }
        delay(delay_time);
        for (uint8_t pos = 0; pos < 91; pos++) {
            set_servo_pos(pos);
            delay(delay_time);
        }
    }

    return 0;
}

void set_servo_pos(int pos) {
    if (pos < 0 || pos > 180) {
        Serial.println(F("pos out of range 0-180"));
        return;
    }

    if (!myservo.attached()) {
        myservo.attach(SERVO_PIN);
    }
    myservo.write(pos);
    Serial.print(F("new pos: "));
    Serial.println(pos);
}

void set_servo_us(int us) {
    if (!myservo.attached()) {
        myservo.attach(SERVO_PIN);
    }
    myservo.writeMicroseconds(us);
    Serial.print(F("new us: "));
    Serial.println(us);
}

/*

=head1 NAME

servo_tester.ino - control servo motor via serial console

=head1 SYNOPSIS

after flashing into Arduino, connect via serial console 9600 to send commands:

    servo signal pin is 9
    supported commands:
        s [int]              - turn servo to 0-180
        us [int]             - set puls width 544-? (2400)
        sweep [int] [int]    - sweep 0-180-0, optional step delay and count
        ?                    - print this help
    s 90
    new pos: 90
    us 1400
    new us: 1400

=head1 DESCRIPTION

Serial console accepts commands and sets servo motor accordingly. Servo
control pin is hard-coded to pin 9.

=head1 COMMANDS

=head2 s [int]

Set servo angle.
Value range 0 to 180.

=head2 us [int]

Set servo angle using PWM peak time length in nano seconds.
Value range 564 to 1496.

=head1 sweep [int] [int]

Moves servo from 90 to 180, then back to 0 and again to 90 degree.

Both parameters are optional. First sets the time in milliseconds to wait
between each angle change, default 15. The second is the number of
repeats, default 1.

=head1 TODO

In the future, if time and motivation persists, the code may evolve
into full multi-servo test/control module.

=head1 SEE ALSO

L<https://blog.kutej.net/2020/04/servo-tester>

=head1 LICENSE

This is free software, licensed under the MIT License.

=cut

*/
