#ifndef _Drive
#define _Drive

#define NUM_SENSORS 8
#define TIMEOUT     2500
#define EMITTER_PIN 39

class Drive {
  public:
    Drive(int _l_ln2, int _l_ln1, int _l_inv, int _l_en, int _r_ln2, int _r_ln1, int _r_inv, int _r_en, int _encoder0PinA, int _encoder0PinB);
    void lineFollow(bool right_inv, int right_speed, bool left_inv, int left_speed);
    void setupQtr();
    void stopDriving();
    void driveBack();
    ~Drive();
  private:
    void driveLeft(bool inv, int spd);
    void driveRight(bool inv, int spd);
    void setLineRaw();
    float linePosition();
    float scaler(float x, float in_min, float in_max, float out_min, float out_max);
    int l_ln2;
    int l_ln1; 
    int l_inv;
    int l_en;
    int r_ln2; 
    int r_ln1; 
    int r_inv;
    int r_en;
    int val;
    int encoder0PinA;
    int encoder0PinB;
    int encoder0Pos;
    int encoder0PinALast;
    int n;
    int driveBackValue;
    
    bool qtr_setup;
    unsigned int sensor_val[NUM_SENSORS];
    unsigned int line_follow_pos;
    int prev_error;
};

#endif
