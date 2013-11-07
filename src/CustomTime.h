//
//  CustomTime.h
//  Dunkerque
//
//  Created by Julian Adenauer on 07.11.13.
//
//

#ifndef __Dunkerque__CustomTime__
#define __Dunkerque__CustomTime__

#include "ofMain.h"

class CustomTime {
public:
    CustomTime(){}
    
    void setup(int mode, int start_hours = 0, int start_minutes = 0, int start_seconds = 0);
    void update();
    
    void addMinutes(int numMinutes);
    void setTime(int hours, int minutes, int seconds = 0);
    
    int getMinutes();
    int getHours();
    int getSeconds();
    
    enum timeModes {
        REAL_TIME, DELTA_TIME, MANUAL_TIME
    } timeMode;
    
    int getCurrentTimeInSeconds();
    
private:
    int seconds, minutes, hours;
    int delta;
};




#endif /* defined(__Dunkerque__CustomTime__) */
