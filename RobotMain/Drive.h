#ifndef _Drive
#define _Drive

#define NUM_SENSORS 8
#define TIMEOUT     2500
#define EMITTER_PIN QTR_NO_EMITTER_PIN

class Drive {
  public:
    Drive(int _l_ln2, int _l_ln1, int _l_inv, int _l_en, int _r_ln2, int _r_ln1, int _r_inv, int _r_en);
    void drive(bool right_inv, int right_speed, bool left_inv, int left_speed);
    void stopDriving();
    void displayQtr();
    void setupQtr();
    ~Drive();
  private:
    void driveLeft(bool inv, int spd);
    void driveRight(bool inv, int spd);
    int l_ln2;
    int l_ln1; 
    int l_inv;
    int l_en;
    int r_ln2; 
    int r_ln1; 
    int r_inv;
    int r_en;

    bool qtr_setup;
    unsigned int sensorValues[NUM_SENSORS];
};

#endif
