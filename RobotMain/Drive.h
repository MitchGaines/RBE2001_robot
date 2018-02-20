#ifndef _Drive
#define _Drive

class Drive {
  public:
    Drive();
    Drive(int l_ln2, int l_ln1, int l_enc, int r_ln2, int r_ln1, int r_enc);
    void driveForward();


    ~Drive();
  private:
  
};

#endif
