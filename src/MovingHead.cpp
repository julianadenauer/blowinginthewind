//
//  MovingHead.cpp
//
//  Created by Julian Adenauer on 25.06.13.
//
//

#include "MovingHead.h"


MovingHead::MovingHead(float x, float y, float z, string ip, int universe){
    location = ofVec3f(x, y, z);
    openingAngle = 5.0  * DEG_TO_RAD;
    color = ofColor(0, 255, 0, 100);
    drawMeta = false;
    
    lastUpdate = ofGetElapsedTimef();

    this->ip = ip;
    this->universe = universe;
    
    angleRangeTilt = 270.0;
    angleRangePan = 540.0;
    angleOffsetTilt = 225.0;
    angleOffsetPan = 360.0; // was 180.0
    
    setTarget(0, 0);
    
    setSpeed(4);
    setMHSpeed(0);
    setRed(0, 0);
    setGreen(0, 0);
    setBlue(0, 0);
    setWhite(0, 0);
    setDimmer(0, 0);
    setIris(0, 0);
    setZoom(0, 0);
    setFocus(0, 0);
    setColorWheel(0);
    setSpecial(0);
    setCTC(0);
    setAutofocus(0);

    // deactivate all the gobo stuff, we don't need
    channels[channel_gobo_rotation_speed] = 0;
    channels[channel_gobo_static_wheel] = 0;
    channels[channel_gobo_static_wheel_fine] = 0;
    channels[channel_gobo_rotating_wheel] = 0;
    channels[channel_gobo_indexing] = 0;
    channels[channel_gobo_indexing_fine] = 0;
    channels[channel_gobo_prism] = 0;
    channels[channel_gobo_prism_rotation] = 0;
    channels[channel_gobo_frost] = 0;
    
    color_fade.is_fading = false;
}

// getters
ofVec2f MovingHead::getTargetAngles() { return targetAngles; }
ofVec3f MovingHead::getLocation(){ return location; }
float MovingHead::getOpeningAngle(){ return openingAngle; }
float MovingHead::getSpeed() { return speed_internal; }
bool MovingHead::getPositionReached(){return positionReached;}

// setters
void MovingHead::setTarget(float x, float y){
    target = ofVec2f(x, y);
    targetAngles.x = atan((x-location.x)/location.z);
    targetAngles.y = atan((y-location.y) / sqrt( pow(location.z, 2) + pow(double(x-location.x), 2)));
    
//    int nx = (int) round( pow(2.0,16.0) / angleRangePan * (targetAngles.x * RAD_TO_DEG + angleOffsetPan));
//    int ny = (int) round((angleOffsetTilt - targetAngles.y * RAD_TO_DEG) * pow(2.0, 16.0) / angleRangeTilt);
    
//    // set the pan and tilt for the heads
//    setPan(nx);
//    setTilt(ny);

    positionReached = false;
}

void MovingHead::setTarget(ofVec2f v){
    setTarget(v.x, v.y);
}

//void MovingHead::setPosition(float x, float y){
//    position.x = x;
//    position.y = y;
//    angles.x = atan((x-location.x)/location.z);
//    angles.y = atan(((y-location.y) * cos(angles.x)) / location.z );
//}
//
//void MovingHead::setPosition(ofVec2f v){
//    setTarget(v.x, v.y);
//}

ofVec2f MovingHead::getTarget(){
//    ofVec2f target;
//    target.x = tan(targetAngles.x)*location.z + location.x;
//    target.y = tan(targetAngles.y) * sqrt( pow(location.z, 2) + pow(target.x-location.x, 2) );
    return target;
    
}

ofVec2f MovingHead::getPosition(){
    ofVec2f position;
    position.x = tan(currentAngles.x)*location.z + location.x;
    position.y = tan(currentAngles.y) * sqrt( pow(location.z, 2) + pow(position.x-location.x, 2) );
    return position;
}

//void MovingHead::setAngles(float x, float y){
//    currentAngles.x = x;
//    currentAngles.y = y;
//}
//
//void MovingHead::setAngles(ofVec2f angles){
//    setAngles(angles.x, angles.y);
//}

void MovingHead::setLocation(float x, float y, float z){
    location.x = x;
    location.y = y;
    location.z = z;
}

void MovingHead::setColor(int r, int g, int b, bool stop_fade){
    setRed(r, 0);
    setGreen(g, 0);
    setBlue(b, 0);

    setWhite(0, 0);

    color = ofColor(r, g, b, 120);
    if(stop_fade)
        color_fade.is_fading = false;
}

void MovingHead::setColor(ofColor c, bool stop_fade){
    setColor(c.r, c.g, c.b, stop_fade);
}

void MovingHead::setDrawMeta(bool b){
    drawMeta = b;
}

void MovingHead::fadeToColor(ofColor c, float timeInSeconds){
    color_fade.is_fading = true;
    color_fade.start_time = ofGetElapsedTimef();
    color_fade.length = timeInSeconds;
    color_fade.end_color = c;
    color_fade.start_color = color;
}

void MovingHead::fadeToFocus(int focus, float timeInSeconds){
    
}

void MovingHead::totalFixtureReset(){
    // set all channels to zero
    for(int i = 0; i < 36; i++)
        channels[i] = 0;

    closeShutter();
    setSpecial(205);
}

float MovingHead::getDiameter(){
    ofVec2f position = getPosition();
    
    ofVec2f v0 = ofVec2f(position.x, position.y) - ofVec2f(location.x, location.y);   // vektor von der nullposition des scheinwerfers zum ziel-mittelpunkt an der wand
    
    float l = v0.length();                          // abstand (an der wand) von nullposition zur zielposition
    float angle_v0 = atan(l/location.z);            // winkel zwischen nullposition und mittelpunkt
    float angle_v1 = angle_v0 - openingAngle;       // winkel des nächsten punkts
    float angle_v2 = angle_v0 + openingAngle;       // winkel des entferntesten punkts
    float l1 = tan(angle_v1) * location.z;          // abstand von der nullposition
    float l2 = tan(angle_v2) * location.z;          // abstand von der nullposition
    
    ofVec2f v1 = v0.getNormalized() * l1;       // in die richtige richtung drehen
    ofVec2f v2 = v0.getNormalized() * l2;       // in die richtige richtung drehen
    
    ofPoint ellipseCenter = ofVec2f(location.x, location.y) + ((v1+v2)/2);
    
    ofVec3f centerVector = ofVec3f(position.x, position.y, 0) - location;
    
    // calculate the width of the ellipse
    // float width = 2 * tan(openingAngle) * centerVector.length();   // die breite der ellipse ist nur vom öffnungswinkel und dem abstand des mittelpunkts zur wand abhängig
    float width = 2*location.z*sin(openingAngle) / sqrt( pow(cos(openingAngle),2) + pow(cos(angle_v0),2) - 1); // neue version
    
    ofPushMatrix();
    
    // positionierung und rotation
    ofTranslate(ellipseCenter);
    ofRotate(-v0.angle(ofVec2f(1,0)));
    
    // ellipse zeichnen
    ofEllipse(0,0, (v2-v1).length(), width);
    ofSetColor(255);

}

void MovingHead::update(){
    // **************************
    // MOVEMENT
    // **************************
    bool reached_x = false;
    bool reached_y = false;
    
    // move towards the target position
    float elapsedTime = ofGetElapsedTimef() - lastUpdate;   // how much time has passed since the last update
    float delta = speed_internal * elapsedTime;   // meters the movinghead could have moved since the last update

    ofVec2f deltaVector = target - getPosition(); // vector in target direction
    
    if(deltaVector.length() > delta){
        deltaVector.scale(delta);  // scale down if too long
    }
    else {
        // if the delta vector is shorter, that means we'll reach the target at the end of this update. so set the positionReached flag to true.s
        positionReached = true;
    }
    
    // now get the angular-coordinates of the position we're aiming for in this update
    ofVec2f temporary_target = getPosition() + deltaVector;
    
    ofVec2f temporary_target_angles;
    temporary_target_angles.x = atan((temporary_target.x-location.x)/location.z);
    temporary_target_angles.y = atan((temporary_target.y-location.y) / sqrt( pow(location.z, 2) + pow(double(temporary_target.x-location.x), 2)));
    
    // update the heads
    int nx = (int) round( pow(2.0,16.0) / angleRangePan * (temporary_target_angles.x * RAD_TO_DEG + angleOffsetPan));
    int ny = (int) round((angleOffsetTilt - temporary_target_angles.y * RAD_TO_DEG) * pow(2.0, 16.0) / angleRangeTilt);
    setPan(nx);
    setTilt(ny);
    
    currentAngles = temporary_target_angles;
    
    
    // **************************
    // COLOR FADE
    // **************************
    if(color_fade.is_fading){
        float percentage = (ofGetElapsedTimef() - color_fade.start_time) / color_fade.length;
        
        if(percentage > 1) {
            setColor(color_fade.end_color, true);
            ofLog() << "color fade finished";
        }
        else {
            int dr = color_fade.end_color.r - color_fade.start_color.r;
            int dg = color_fade.end_color.g - color_fade.start_color.g;
            int db = color_fade.end_color.b - color_fade.start_color.b;
            
            setColor(
                     color_fade.start_color.r + (dr * percentage),
                     color_fade.start_color.g + (dg * percentage),
                     color_fade.start_color.b + (db * percentage),
                     false // do not stop the fade
            );
        }
    }
    
    lastUpdate = ofGetElapsedTimef();
    
}

void MovingHead::draw(){
    // if the shutter is not open, don't draw anything
    if(channels[channel_shutter] == SHUTTER_OPEN){    
        ofPushStyle();
        ofSetColor(color, channels[channel_dimmer]);
        
        ofVec2f position = getPosition();
            
        ofVec2f v0 = ofVec2f(position.x, position.y) - ofVec2f(location.x, location.y);   // vektor von der nullposition des scheinwerfers zum ziel-mittelpunkt an der wand
        
        float l = v0.length();                          // abstand (an der wand) von nullposition zur zielposition
        float angle_v0 = atan(l/location.z);            // winkel zwischen nullposition und mittelpunkt
        float angle_v1 = angle_v0 - openingAngle;       // winkel des nächsten punkts
        float angle_v2 = angle_v0 + openingAngle;       // winkel des entferntesten punkts
        float l1 = tan(angle_v1) * location.z;          // abstand von der nullposition
        float l2 = tan(angle_v2) * location.z;          // abstand von der nullposition
        
        ofVec2f v1 = v0.getNormalized() * l1;       // in die richtige richtung drehen
        ofVec2f v2 = v0.getNormalized() * l2;       // in die richtige richtung drehen
        
        ofPoint ellipseCenter = ofVec2f(location.x, location.y) + ((v1+v2)/2);
        
        ofVec3f centerVector = ofVec3f(position.x, position.y, 0) - location;
        
        // calculate the width of the ellipse
        // float width = 2 * tan(openingAngle) * centerVector.length();   // die breite der ellipse ist nur vom öffnungswinkel und dem abstand des mittelpunkts zur wand abhängig
        float width = 2*location.z*sin(openingAngle) / sqrt( pow(cos(openingAngle),2) + pow(cos(angle_v0),2) - 1); // neue version
        
        ofPushMatrix();
        
        // positionierung und rotation
        ofTranslate(ellipseCenter);
        ofRotate(-v0.angle(ofVec2f(1,0)));
        
        // ellipse zeichnen
        ofEllipse(0,0, (v2-v1).length(), width);
        ofSetColor(255);
        
        // draw width and the corresponding line
        if(drawMeta)
        {
            ofDrawBitmapString(ofToString(abs(width), 2), 0, 0);
            ofRotate(90);
            ofLine(-width/2, 0, width/2, 0);
        }
        
        ofPopMatrix();
        ofPopStyle();
    }
}