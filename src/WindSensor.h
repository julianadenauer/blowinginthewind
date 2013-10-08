//
//  WindSensor.h
//  Dunkerque
//
//  Created by Julian Adenauer on 05.08.13.
//
//

#ifndef __Dunkerque__WindSensor__
#define __Dunkerque__WindSensor__

#include "ofMain.h"

class WindSensor{
    public:
        WindSensor(string port, float speed_max = 20.0, bool fake = false);
    
        void update();
    
        float getSpeed();
        float getScaledSpeed(); // get speed as percentage from zero to speed_max
        float getDirection();
    
        ofVec2f getVector();
        ofVec2f getScaledVector();
    
        void setSpeedMax(float max);
    
private:
    float speed;
    float speed_max;
    float direction;
    
    bool fake;
    float last_fake_update;
    ofSerial serial;
    
};

#endif /* defined(__Dunkerque__WindSensor__) */
