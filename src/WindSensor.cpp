//
//  WindSensor.cpp
//  Dunkerque
//
//  Created by Julian Adenauer on 05.08.13.
//
//

#include "WindSensor.h"

WindSensor::WindSensor(string port, float speed_max, bool fake){
    this->speed_max = speed_max;
    this->fake = fake;
    
    if(!fake){
        if(serial.setup(port, 9600)) //open the port
            ofLog() << "Serial port to wind sensor opened";
        
        // initialize the values with zero
        direction = 0;
        speed = 0;
    }
    
    else {
        ofLog() << "Faking wind sensor data is aktivated. Wind sensor data will be random.";
        // initialize the values with some value
        direction = 20;
        speed = 5;

    }
    
    last_fake_update = 0;
}   

void WindSensor::update(){
//    while(serial.available())
//        ofLog() << serial.readByte();

    if(!fake){
        unsigned char buffer[16];
        ofLog() << "trying to update wind sensor";
        int bytes_read = serial.readBytes(buffer, 16);
        if(bytes_read == 16){
            ofLog() << "got 16 bytes";
            // check start and end
            if(buffer[0] == 2 && buffer[15] == 3){ // TODO: hier nochmal genau schauen, ob das so funktioniert!
                speed = 10 * buffer[1] + buffer[2] + 0.1 * buffer[4];
                direction = 100 * buffer[6] + 10 * buffer[7] + buffer[8] + 0.1 * buffer[10];
                ofLog() << "[wind] updated wind sensor: speed=" << speed << " direction=" << direction;
            }
        }
        else{ ofLog() << "only got " << bytes_read << " of the expected 16 bytes from sensor"; }
    }
    else if ( ofGetElapsedTimef() - last_fake_update > 60){
        last_fake_update = ofGetElapsedTimef();
        
//        speed += ofRandom(-3.0, 3.0);
//        if(speed < 0) speed = 0;
//        else if (speed > speed_max) speed = speed_max;
        
        speed = 5;
        
        direction += ofRandom(-50.0, 50.0);
        if(direction >= 360) direction-=360;
        else if (direction < 0) direction+= 360;
        
        ofLog() << "[fake-wind] " << speed << "," << direction;
    }
}

float WindSensor::getSpeed(){ return speed; }

float WindSensor::getScaledSpeed(){
    float scaled_speed = speed / speed_max;
    
    // return 100% if the speed is higher than the maximum value => makes sure that the installation does not go crazy during a storm
    if(scaled_speed > 1.0)
        return 1.0;
    
    else return scaled_speed;
}

float WindSensor::getDirection(){ return direction; }

void WindSensor::setSpeedMax(float max){
    speed_max = max;
}

ofVec2f WindSensor::getVector(){
    ofVec2f v = ofVec2f(0, 1);
    v.rotate(direction);
    v.scale(speed);
    return v;
}

ofVec2f WindSensor::getScaledVector(){
    ofVec2f v = ofVec2f(0, 1);
    v.rotate(direction);
    v.scale(getScaledSpeed());
    return v;
}