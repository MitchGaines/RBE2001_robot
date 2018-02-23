#ifndef _Drive
#define _Drive

class Drive {
  public:
    Drive(int _l_ln2, int _l_ln1, int _l_enc, int _r_ln2, int _r_ln1, int _r_enc);
    void driveForward();

    ~Drive();
  private:
    int l_ln2;
    int l_ln1; 
    int l_enc; 
    int r_ln2; 
    int r_ln1; 
    int r_enc;
};

#endif
