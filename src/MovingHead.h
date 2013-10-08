//
//  MovingHead.h
//
//  Created by Julian Adenauer on 25.06.13.
//
//

#ifndef __MovingHead__
#define __MovingHead__

#include <iostream>
#include "ofMain.h"

#define IRIS_OPEN 0
#define IRIS_CLOSED 180

#define SHUTTER_OPEN 255
#define SHUTTER_CLOSED 0


class MovingHead{
    
    ofVec2f currentAngles;
    ofVec2f targetAngles;  // target position in angle-coordinates
    ofVec2f target;
    
    float speed_internal;
    ofVec3f location;   // 3d location of the moving head
    ofColor color;
    float openingAngle; // halber öffnungswinkel
    bool drawMeta;
    float lastUpdate;
    bool positionReached;
    
//    void setAngles(float x, float y);
//    void setAngles(ofVec2f angles);
    
    unsigned char channels[36];
    
    string ip;
    int universe;
    
    float angleRangeTilt;
    float angleRangePan;
    float angleOffsetTilt;
    float angleOffsetPan;

public:
    
    MovingHead(float x = 0, float y = 0, float z = 10, string ip = "192.168.1.10", int universe = 0);
    void update();
    void draw();
    
    enum channelNumbers {
        // these channels are 0 based and NOT 1-based as in the dmx table
        channel_pan,
        channel_pan_fine,
        channel_tilt,
        channel_tilt_fine,
        channel_speed,
        channel_special,
        channel_colorwheel,
        channel_red,
        channel_red_fine,
        channel_green,
        channel_green_fine,
        channel_blue,
        channel_blue_fine,
        channel_white,
        channel_white_fine,
        channel_ctc,
        
        // gobo stuff
        channel_gobo_rotation_speed,
        channel_gobo_static_wheel,
        channel_gobo_static_wheel_fine,
        channel_gobo_rotating_wheel,
        channel_gobo_indexing,
        channel_gobo_indexing_fine,
        channel_gobo_prism,
        channel_gobo_prism_rotation,
        channel_gobo_frost,
        
        channel_iris = 25,
        channel_iris_fine,
        channel_zoom,
        channel_zoom_fine,
        channel_focus,
        channel_focus_fine,
        channel_autofokus,
        channel_shutter,
        channel_dimmer,
        channel_dimmer_fine
    };
    
    // getters
    ofVec2f getPosition();
    ofVec3f getLocation();
    ofVec2f getTarget();
    ofVec2f getTargetAngles();
    float getSpeed();
    float getOpeningAngle();
    bool getPositionReached();
    int getUniverse(){return universe;}
    int getPan() { return (channels[channel_pan] << 8) + channels[channel_pan_fine]; }
    int getTilt() { return (channels[channel_tilt] << 8) + channels[channel_tilt_fine]; }
    unsigned char* getChannels(){return channels;}
    int getFocus() { return (channels[channel_focus] << 8) + channels[channel_focus_fine]; }
    int getZoom() { return (channels[channel_zoom] << 8) + channels[channel_zoom_fine]; }
    ofColor getColor() { return color; };
    int getWhite() { return (channels[channel_white] << 8) + channels[channel_white_fine]; }
    float getDiameter();
    unsigned char getStrobe(){return channels[channel_shutter];}
    int getDimmer() { return (channels[channel_dimmer] << 8) + channels[channel_dimmer_fine]; }

    // setters
    void setUniverse(int u){universe = u;}
    void setTarget(float x, float y);               // moving there
    void setTarget(ofVec2f v);
    void setLocation(float x, float y, float z);    // Location of the Moving Head in 3D
    void setColor(int r, int g, int b, bool stop_fade = true);
    void setColor(ofColor c, bool stop_fade = true);
    
    // drawing
    void setDrawMeta(bool b);
    
    // fades
    struct colorFadeDescription {
        bool is_fading;
        ofColor start_color;
        ofColor end_color;
        float start_time;
        float length;
    } color_fade;
    void fadeToColor(ofColor c, float timeInSeconds);
    void fadeToFocus(int focus, float timeInSeconds);
    
    // Special Functions
    void totalFixtureReset();
    
    // ***********************
    // movinghead functions
    inline void setStrobe(unsigned char strobe){ channels[channel_shutter] = strobe; }
    inline void openShutter(){ channels[channel_shutter] = SHUTTER_OPEN; }
    void closeShutter(){ channels[channel_shutter] = SHUTTER_CLOSED; }
    void setTilt(unsigned char tilt, unsigned char tilt_fine){ channels[channel_tilt] = tilt; channels[channel_tilt_fine] = tilt_fine; }
    void setTilt(int tilt) { channels[channel_tilt] = tilt >> 8; channels[channel_tilt_fine] = tilt % 256; }
    void setPan(unsigned char pan, unsigned char pan_fine){ channels[channel_pan] = pan; channels[channel_pan_fine] = pan_fine; }
    void setPan(int pan){ channels[channel_pan] = pan >> 8; channels[channel_pan_fine] = pan % 256;}
    void setSpeed(float speedInMeterPerSecond){ // 0 is fastest , ich schätze, dass das ca. 25°/s entspricht
        speed_internal = speedInMeterPerSecond;
    }
    
    // den speed-einstellung der movingheads verändern.
    // eigentlich nicht wirklich nötig, weil ich die geschwindigkeitssteuerung eigentlich aus der software heraus mache
    void setMHSpeed(unsigned char speed) { channels[channel_speed] = speed; }
    
    void setSpecial(unsigned char special){ channels[channel_special] = special; }
    void setCTC(unsigned char ctc){ channels[channel_ctc] = ctc;}
    void setIris(unsigned char iris, unsigned char iris_fine) { channels[channel_iris] = iris; channels[channel_iris_fine] = iris_fine; }
    void setIris(int iris) { setIris(iris >> 8, iris % 256); }

    void setFocus(unsigned char focus, unsigned char focus_fine){ channels[channel_focus] = focus; channels[channel_focus_fine] = focus_fine;}
    void setFocus(int focus){ setFocus(focus >> 8, focus % 256); }
    void setAutofocus(unsigned char autofocus){ channels[channel_autofokus] = autofocus; }

    void setZoom(unsigned char zoom, unsigned char zoom_fine){ // 0-255 from max (45°) to min (10°) angle
        channels[channel_zoom] = zoom;
        channels[channel_zoom_fine] = zoom_fine;
        openingAngle = ((45.0 - (((45.0-10.0) * ((zoom<<8) + zoom_fine))/ pow(2.0, 16))) * DEG_TO_RAD)/2;
    }
    void setZoom(int zoom){ setZoom(zoom >> 8, zoom % 256); }
    void setDimmer(unsigned char dimmer, unsigned char dimmer_fine){ channels[channel_dimmer] = dimmer; channels[channel_dimmer_fine] = dimmer_fine; }
    void setDimmer(int dimmer){ channels[channel_dimmer] = dimmer >> 8; channels[channel_dimmer_fine] = dimmer % 256; }
    void setRed(unsigned char red, unsigned char red_fine){ channels[channel_red] = red; channels[channel_red_fine] = red_fine; }
    void setGreen(unsigned char green, unsigned char green_fine){ channels[channel_green] = green; channels[channel_green_fine] = green_fine; }
    void setBlue(unsigned char blue, unsigned char blue_fine){ channels[channel_blue] = blue; channels[channel_blue_fine] = blue_fine; }
    void setWhite(unsigned char white, unsigned char white_fine){ channels[channel_white] = white; channels[channel_white_fine] = white_fine; }
    void setWhite(int white){ setWhite(white >> 8, white % 256); }
    void setColorWheel(unsigned char colorWheel){ channels[channel_colorwheel] = colorWheel; }
    // ************************
};

#endif /* #define __MovingHead__ */

