/*
 
 dunkerque
 - include the distortion and set the positioning/scaling to real world coordinates into the former timeline_v1 app

 */

#define ENABLE_OPENGL

#pragma once

#include "ofMain.h"
#include "MovingHead.h"
#include "MSAPhysics2D.h"
#include "ofxLibArtnet.h"
#include "WindSensor.h"
#include "CustomTime.h"

#ifdef ENABLE_OPENGL
    #include "ofxAutoControlPanel.h"
#else
    #include "ofxXmlSettings.h"
#endif

#define MAXIMUM 65535

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void updateStates();
    void loadSettings();
    
    void keyPressed  (int key);
    
    void switchState(int state);
    int nextState();
    
    ofVec2f toReal(ofVec2f screen);
    ofVec2f toReal(float x, float y);
    ofVec2f toScreen(ofVec2f real);
    ofVec2f toScreen(float x, float y);
    
    MovingHead mh1, mh2;
    ofxLibArtnet::Node node;
    
    CustomTime myTime;
    
    WindSensor* wind_sensor;
    
    int state;
    
    ofPoint ursprung;   // 0 position on screen
    ofImage building;
    
    // physics
    msa::physics::Particle2D *p1;
    msa::physics::Particle2D *p2;
    msa::physics::World2D physics;    
    
    // circle mode
    ofVec2f rotationCenter;
    ofVec2f rotationV1, rotationV2;
    
    ofVec2f chase_target;
    float last_movement;
    float last_update;
    
    bool state_initialized;
    bool overlap_initiated;
    bool day_init_done, program_init_done;
    int current_state_id;

    // state stuff
    enum states{ state_mirror, state_mirror_defocus, state_chase_jump, state_gravity, state_gravity_defocus, state_circle, state_egg, state_chase, state_eyes, state_bounce, state_overlap, state_overlap_white, state_mouse, state_stillandblowing, state_stillandblowing_white};     // list of states of the program
    states program_states[12];     // order in which the different states should be activated
    
    // color stuff
    ofColor colors[7];
    int current_color_wheel_id;
    int current_hour_color_id;
    enum colorMode { color_mode_random, color_mode_wheel, color_mode_plus3 } current_color_mode;    // list of the different color changing modes
    
    int initialized_minute;
    
    
 
#ifdef ENABLE_OPENGL
    ofxAutoControlPanel panel;
#endif
    
    // settings
    string local_ip_address, mh1_ip_address, mh2_ip_address;
    string wind_sensor_port;
    float wind_speed_max;
    bool wind_sensor_fake;
    int mh1_universe, mh2_universe;
    float mh1_location_x, mh1_location_y, mh1_location_z;
    int mh1_focus, mh1_defocus, mh1_zoom, mh1_iris, mh1_dimmer;
    int mh2_focus, mh2_defocus, mh2_zoom, mh2_iris, mh2_dimmer;
    float mh2_location_x, mh2_location_y, mh2_location_z;
    float building_width, building_height;
    bool auto_state_shift, draw_debug;
    int time_mode;
    float scaling;
    float stillandblowing_mh1_x, stillandblowing_mh1_y, stillandblowing_mh2_x, stillandblowing_mh2_y;
    float mirror_speed;
    float mirror_area_left, mirror_area_right, mirror_area_top, mirror_area_bottom;
    float chase_jump_speed_leader, chase_jump_speed_follower;
    float chase_jump_area_left, chase_jump_area_right, chase_jump_area_top, chase_jump_area_bottom;
    float gravity_speed;
    float gravity_initial_velocity_mh1_x, gravity_initial_velocity_mh1_y, gravity_initial_velocity_mh2_x, gravity_initial_velocity_mh2_y;
    float circle_radius, egg_radius;
    float circle_rotation_speed_min, circle_rotation_speed_max;
    int egg_zoom, egg_focus, egg_iris;
    float circle_area_left, circle_area_right, circle_area_top, circle_area_bottom;
    float chase_speed_leader, chase_speed_follower;
    float chase_area_left, chase_area_right, chase_area_top, chase_area_bottom;
    float eyes_area_left, eyes_area_right, eyes_area_top, eyes_area_bottom;
    float eyes_distance;
    float bounce_speed;
    float overlap_area_left, overlap_area_right, overlap_area_top, overlap_area_bottom;
    float overlap_speed;
    int overlap_focus, overlap_defocus, overlap_zoom;
    int physics_scaling_factor;
    float physics_particle_radius, physics_attraction, physics_bounce;
    float physics_area_left, physics_area_right, physics_area_top, physics_area_bottom;
    float physics_drag;
    bool activate_network;
};