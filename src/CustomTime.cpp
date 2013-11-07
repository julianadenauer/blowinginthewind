//
//  CustomTime.cpp
//  Dunkerque
//
//  Created by Julian Adenauer on 07.11.13.
//
//

#include "CustomTime.h"

void CustomTime::setup(int mode, int start_hours, int start_minutes, int start_seconds){
    if ( mode == REAL_TIME) timeMode = REAL_TIME;
    else if (mode == DELTA_TIME) timeMode = DELTA_TIME;
    else timeMode = MANUAL_TIME;
    
    seconds = start_seconds;
    minutes = start_minutes;
    hours = start_hours;
    
    if(timeMode == DELTA_TIME){
        // get the time in seconds from midnight
        int start_time = seconds + (minutes * 60) + (hours * 3600);
        int now = getCurrentTimeInSeconds();
        delta = now - start_time;
    }
    
    if(timeMode == REAL_TIME) {
        seconds = ofGetSeconds();
        minutes = ofGetMinutes();
        hours = ofGetHours();
    }
    
}

void CustomTime::update(){
    switch (timeMode) {
        int delta_time;
            
        case REAL_TIME:
            seconds = ofGetSeconds();
            minutes = ofGetMinutes();
            hours = ofGetHours();
            break;
            
        case DELTA_TIME:
            delta_time = getCurrentTimeInSeconds() - delta;
            hours = delta_time / 3600;
            minutes = (delta_time - hours * 3600) / 60;
            seconds = delta_time - hours * 3600 - minutes * 60;
            break;
        
        case MANUAL_TIME:
            // no update needed
            break;
    }
}

int CustomTime::getCurrentTimeInSeconds(){
    return ofGetSeconds() + (ofGetMinutes() * 60) + (ofGetHours() * 3600);
}

void CustomTime::addMinutes(int numMinutes){
    if (numMinutes > 60) { ofLog() << "addMinutes currently only works with values < 60"; return;}

    minutes+=numMinutes;        
    if (minutes > 60){ minutes-=60; hours++; }
    else if (minutes < 0) { minutes = minutes + 60; hours--;}

    if (hours > 23) hours = 0;
    else if (hours < 0) hours = 23;
}

void CustomTime::setTime(int hours, int minutes, int seconds){
    this->hours = hours;
    this->minutes = minutes;
    this->seconds = seconds;
}

int CustomTime::getMinutes(){ return minutes; }
int CustomTime::getHours(){ return hours; }
int CustomTime::getSeconds(){ return seconds; }


