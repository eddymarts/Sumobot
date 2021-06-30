/* Sumobot code.
Made for Hands On Robot Competition.
By Eddy Martínez. 1053420
Instituto Tecnológico de Santo Domingo. */

#define MOTOR1_A 2
#define MOTOR1_B 4
#define MOTOR1_EN 3

#define MOTOR2_A 7
#define MOTOR2_B 8
#define MOTOR2_EN 6

#define MOTOR3_A 12
#define MOTOR3_B 13
#define MOTOR3_EN 11

#define US_SENSOR_1 0
#define US_SENSOR_2 1
#define US_SENSOR_3 2

#define IR_SENSOR_1 3
#define IR_SENSOR_2 4
#define IR_SENSOR_3 5

float a, b, c;
int d, e, f;

class Motor { // DC Motor
  private:
    byte a; // Pin 1
    byte b; // Pin 2
    byte en; // Analog output for Pulse Width Modulation.

  public:
    Motor(byte pinA, byte pinB, byte pinEn) {
      this->a = pinA;
      this->b = pinB;
      this->en = pinEn;

      init();
    }

    void init() {
      pinMode(a, OUTPUT);
      pinMode(b, OUTPUT);
      pinMode(en, OUTPUT);
      off();
    }
    
    
    void off() {
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      analogWrite(en, 0);
    }
    
    // v is analog value for Pulse Width Modulation
    // with a range from 0 to 255.
    void A(int v) {
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      analogWrite(en, v);
    }
    
    void B(int v) {
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      analogWrite(en, v);
    }
};

class Sensor {
  private:
    byte pin; // Pin
    int conv;
    float threshold;

  public:
    Sensor(byte pin, int conv, float threshold) {
      this->pin = pin;
      this->conv = conv;
      this->threshold = threshold;
      init();
    }

    void init() {
      pinMode(pin, INPUT);
    }

    float sense(){
      // analogRead has a range from 0 to 1023.
      // Use conv to convert it to a different range when needed.
      return conv*analogRead(pin);
    }

    bool detect(){
      // If sense() outputs above the threshold, it has detected something.
      return sense() >= threshold;
    }
};

class Sumobot {
  private:
    Motor motor1 = Motor(MOTOR1_A, MOTOR1_B, MOTOR1_EN);
    Motor motor2 = Motor(MOTOR2_A, MOTOR2_B, MOTOR2_EN);
    Motor motor3 = Motor(MOTOR3_A, MOTOR3_B, MOTOR3_EN);
    
    Sensor ir1 = Sensor(IR_SENSOR_1, 1, 800);
    Sensor ir2 = Sensor(IR_SENSOR_2, 1, 800);
    Sensor ir3 = Sensor(IR_SENSOR_3, 1, 800);
    
    Sensor us1 = Sensor(US_SENSOR_1, 0.0049, 1);
    Sensor us2 = Sensor(US_SENSOR_2, 0.0049, 1);
    Sensor us3 = Sensor(US_SENSOR_3, 0.0049, 1);

  public:
    Sumobot() {
      off();
    }

    void off(){
      motor1.off();
      motor2.off();
      motor3.off();
    }

    void x_forward(int v, int b) {
      motor1.off();
      motor2.B(v);
      motor3.B(b);
    }

    void x_backwards(int v, int b) {
      motor1.off();
      motor2.A(v);
      motor3.A(b);
    }

    void y_forward(int v, int b) {
      motor1.A(v);
      motor2.A(b);
      motor3.off();
    }

    void y_backwards(int v, int b) {
      motor1.B(v);
      motor2.B(b);
      motor3.off();
    }

    void z_forward(int v, int b) {
      motor1.B(v);
      motor2.off();
      motor3.A(b);
    }

    void z_backwards(int v, int b) {
      motor1.A(v);
      motor2.off();
      motor3.B(b);
    }

    void turn_right(int v) {
      motor1.B(v);
      motor2.A(v);
      motor3.B(v);
    }

    void turn_left(int v) {
      motor1.A(v);
      motor2.B(v);
      motor3.A(v);
    }

    byte detect_line(){
      if(ir1.detect() && ~(ir2.detect() && ir3.detect()))
        return 1;
      else if(ir2.detect() && ~(ir1.detect() && ir3.detect()))
        return 2;
      else if(ir3.detect() && ~(ir1.detect() && ir2.detect()))
        return 3;
      else if(ir1.detect() && ir2.detect() && (~ir3.detect()))
        return 4;
      else if(ir1.detect() && ir3.detect() && (~ir2.detect()))
        return 5;
      else if(ir2.detect() && ir3.detect() && (~ir1.detect()))
        return 6;
      else if(ir1.detect() && ir2.detect() && ir3.detect())
        return 7;
      else if(~(ir1.detect() && ir2.detect() && ir3.detect()))
        return 0;
    }

    void attack(){
      if (us1.detect() && ~(us2.detect() && us3.detect()))
        x_forward(255, 255);
      else if(us2.detect() && ~(us1.detect() && us3.detect()))
        y_forward(255, 255);
      else if(us3.detect() && ~(us2.detect() && us2.detect()))
        z_forward(255, 255);
      else if(us1.detect() && us2.detect() && (~us3.detect())){
        if(us1.sense()>=us2.sense()) turn_right(100);
        else turn_left(100);
      }
      else if(us1.detect() && us3.detect() && (~us2.detect())){
        if(us1.sense()>=us3.sense()) turn_left(100);
        else turn_right(100);
      }
      else if(us2.detect() && us3.detect() && (~us1.detect())){
        if(us2.sense()>=us3.sense()) turn_right(100);
        else turn_left(100);
      }
      else turn_right(100);
    }

    void navigate() {
      switch(detect_line()){
        case 0:
          attack();
          break;

        case 1:
          x_backwards(255, 255);
          break;

        case 2:
          y_backwards(255, 255);
          break;

        case 3:
          z_backwards(255, 255);
          break;

        case 4:
          z_forward(255, 255);
          break;

        case 5:
          y_forward(255, 255);
          break;

        case 6:
          x_forward(255, 255);
          break;

        case 7:
          turn_left(100);
          break;
      }
    }
};

Sumobot sumobot = Sumobot();

void setup() {
  Serial.begin(9600);
}
void loop() {
  sumobot.navigate();
}
