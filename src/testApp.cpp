#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofLogToFile(ofGetTimestampString("%Y-%m-%d") + "_startup.log");
    
#ifdef ENABLE_OPENGL
    ofLog() << "openGL is enabled";
    ofEnableAlphaBlending();
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetCircleResolution(100);   
    
    // setup scaling and positioning on screen
    ursprung = ofPoint(ofGetWidth()/2, ofGetHeight()-50);
    
    // setup the building
    building.loadImage("Fassade.png");
#endif
    
    // load settings from xml file
    ofLog() << "load settings from xml file";
    loadSettings();
    
    // init the colors
    colors[0]=  ofColor(255,   0,    0);    // red
    colors[1] = ofColor(255,  60,    0);    // orange
    colors[2] = ofColor(255, 175,    0);    // yellow
    colors[3] = ofColor(0,   255,    0);    // green
    colors[4] = ofColor(0,    51,  133);    // blue
    colors[5] = ofColor(50,     0, 255);    // indigo
    colors[6] = ofColor(200,  50,  255);    // violet
    
    // init the states
    program_states[0] = state_bounce; 
    program_states[1] = state_gravity_defocus;
    program_states[2] = state_eyes;
    program_states[3] = state_circle;
    program_states[4] = state_chase_jump;
    program_states[5] = state_egg;
    program_states[6] = state_mirror;
    program_states[7] = state_overlap_white; // "double hex"
    program_states[8] = state_chase;
    program_states[9] = state_mirror_defocus;
    program_states[10] = state_gravity;
    program_states[11] = state_overlap;

    // start with the random color change
    current_color_mode = color_mode_random;
    
    // init the state
    current_state_id = -1;
    state = -1;
    ofLog() << "starting with state " << state;    
    
#ifdef ENABLE_OPENGL
    // setup scaling and positioning on screen
    ursprung = ofPoint(ofGetWidth()/2, ofGetHeight()-100);
    
    // setup the building image
    building.loadImage("Fassade.png");
#endif
    
    // init artnet
    node.addUniverses(2);
    
    // init wind sensor
    ofLog() << "init wind sensor";
    wind_sensor = new WindSensor(wind_sensor_port, wind_speed_max, wind_sensor_fake);
    
    if(activate_network){
        ofLog() << "setting up network";
        if(activate_network) node.setup(local_ip_address, true); // TODO: netzwerk wieder aktivieren
    }

    // setup the heads
    mh1 = MovingHead(mh1_location_x, mh1_location_y, mh1_location_z, mh1_ip_address, mh1_universe);
    mh2 = MovingHead(mh2_location_x, mh2_location_y, mh2_location_z, mh2_ip_address, mh2_universe);
    
    // open the shutters of the heads, give them the dimmer value from the settings and red color
    mh1.openShutter();
    mh1.setColor(255,0,0);
    mh1.setFocus(mh1_focus);
    mh1.setZoom(mh1_zoom);
    mh1.setIris(mh1_iris);
//    mh1.setDimmer(mh1_dimmer);
    
    mh2.openShutter();
    mh2.setColor(255,0,0);
    mh2.setFocus(mh2_focus);
    mh2.setZoom(mh2_zoom);
    mh2.setIris(mh2_iris);
//    mh2.setDimmer(mh2_dimmer);
    
    ofLog() << "opened shutter on both heads and set color to red for the startup";
    
    // update artnet
    node.updateDataByIndex(mh1.getUniverse(), mh1.getChannels(), 36);
    node.updateDataByIndex(mh2.getUniverse(), mh2.getChannels(), 36);
    node.send();
    
    // set up the physics
    // trying if that makes the physics simulation run smoother.
    // I had the impression, that it produces more errors on the small scale
    ofLog() << "setting up the physics";
    physics.setWorldSize(ofVec2f(physics_area_left, physics_area_bottom) * physics_scaling_factor, ofVec2f(physics_area_right, physics_area_top) * physics_scaling_factor);
    physics.setSectorCount(1);
    physics.setDrag(physics_drag);     // Reibung aus = 1
    
    
    // initialize variables for time-based movements
    current_hour_color_id = 0;
    ofLog() << "set current_hour_color_id to " << current_hour_color_id;
    
    initialized_minute = -1;
    day_init_done = false;
    program_init_done = false;
    
    // setup the timing module
    if (time_mode == 0){
        myTime.setup(myTime.REAL_TIME);
    }
    else if(time_mode == 1){
        myTime.setup(myTime.DELTA_TIME, 17, 59, 0);
    }
    else if (time_mode == 2){
        myTime.setup(myTime.MANUAL_TIME, 17, 59, 0);
    }
    
#ifdef ENABLE_OPENGL
    // init the panel
    panel.setup(300, 600);
    panel.addPanel("focus & zoom");
    panel.addSlider("mh1_focus", mh1_focus, 0, MAXIMUM);
    panel.addSlider("mh1_zoom", mh1_zoom, 0, MAXIMUM);
    panel.addSlider("mh1_iris", mh1_iris, 0, MAXIMUM);
    panel.addSlider("mh1_strobe", 0, 0, 255);
    panel.addSlider("mh2_focus", mh2_focus, 0, MAXIMUM);
    panel.addSlider("mh2_zoom", mh2_zoom, 0, MAXIMUM);
    panel.addSlider("mh2_iris", mh2_iris, 0, MAXIMUM);
    panel.addSlider("mh2_strobe", 0, 0, 255);

    panel.addPanel("colors");
    panel.addSlider("mh1_r", 255, 0, 255);
    panel.addSlider("mh1_g", 0, 0, 255);
    panel.addSlider("mh1_b", 0, 0, 255);
    panel.addSlider("mh1_w", 0, 0, MAXIMUM);
    panel.addSlider("mh1_dimmer", mh1_dimmer, 0, MAXIMUM);
    panel.addSlider("mh2_r", 255, 0, 255);
    panel.addSlider("mh2_g", 0, 0, 255);
    panel.addSlider("mh2_b", 0, 0, 255);
    panel.addSlider("mh2_w", 0, 0, MAXIMUM);
    panel.addSlider("mh2_dimmer", mh2_dimmer, 0, MAXIMUM);
#else
    last_update = ofGetElapsedTimef();
#endif
    
}

void testApp::loadSettings(){
    ofxXmlSettings xmlSettings;
    
    if(xmlSettings.loadFile("settings.xml")){
        // network
        activate_network    =  ofToBool(xmlSettings.getValue("network:activate", "true"));
        local_ip_address    =  xmlSettings.getValue("network:local_ip_address", "192.168.1.2");
        mh1_ip_address      =  xmlSettings.getValue("network:mh1_ip_address", "192.168.1.10");
        mh2_ip_address      =  xmlSettings.getValue("network:mh2_ip_address", "192.168.1.11");
        mh1_universe        =  xmlSettings.getValue("network:mh1_universe", 0);
        mh2_universe        =  xmlSettings.getValue("network:mh2_universe", 1);
        
        // wind sensor
        wind_sensor_port    = xmlSettings.getValue("wind_sensor:port", "/dev/tty.usbserial");
        wind_speed_max      = xmlSettings.getValue("wind_sensor:speed_max", 20.0);
        wind_sensor_fake    = ofToBool(xmlSettings.getValue("wind_sensor:fake", "false"));
        
        // settings for the heads
        mh1_focus           = xmlSettings.getValue("heads:mh1:focus", MAXIMUM);
        mh1_defocus         = xmlSettings.getValue("heads:mh1:defocus", MAXIMUM);
        mh1_zoom            = xmlSettings.getValue("heads:mh1:zoom", MAXIMUM);
        mh1_iris            = xmlSettings.getValue("heads:mh1:iris", 0);
        mh1_dimmer          = xmlSettings.getValue("heads:mh1:dimmer", MAXIMUM);
        mh1_location_x      = xmlSettings.getValue("heads:mh1:location:x", 11.0f);
        mh1_location_y      = xmlSettings.getValue("heads:mh1:location:y", 0.0f);
        mh1_location_z      = xmlSettings.getValue("heads:mh1:location:z", 19.0f);
        
        mh2_focus           = xmlSettings.getValue("heads:mh2:focus", MAXIMUM);
        mh2_defocus         = xmlSettings.getValue("heads:mh2:defocus", MAXIMUM);
        mh2_zoom            = xmlSettings.getValue("heads:mh2:zoom", MAXIMUM);
        mh2_iris            = xmlSettings.getValue("heads:mh2:iris", 0);
        mh2_dimmer          = xmlSettings.getValue("heads:mh2:dimmer", MAXIMUM);
        mh2_location_x      = xmlSettings.getValue("heads:mh2:location:x", -11.0f);
        mh2_location_y      = xmlSettings.getValue("heads:mh2:location:y", 0.0f);
        mh2_location_z      = xmlSettings.getValue("heads:mh2:location:z", 19.0f);;
        
        // building
        building_width      = xmlSettings.getValue("building:width", 26.23f);
        building_height     = xmlSettings.getValue("building:height", 37.52f);
        
        // display
        draw_debug          = ofToBool(xmlSettings.getValue("display:draw_debug", "false"));
        scaling             = xmlSettings.getValue("display:scaling", 20.0f);
        
        // program
        auto_state_shift    = ofToBool(xmlSettings.getValue("program:auto_state_shift", "true"));
        time_mode           = xmlSettings.getValue("program:time_mode", 0);
        
        
        // *** ANIMATIONS ***
        // still and blowing
        stillandblowing_mh1_x = xmlSettings.getValue("animations:stillandblowing:mh1:x", 6.0);
        stillandblowing_mh1_y = xmlSettings.getValue("animations:stillandblowing:mh1:y", 15.0);
        stillandblowing_mh2_x = xmlSettings.getValue("animations:stillandblowing:mh2:x", -6.0);
        stillandblowing_mh2_y = xmlSettings.getValue("animations:stillandblowing:mh2:y", 15.0);
        
        
        // mirror
        mirror_speed        = xmlSettings.getValue("animations:mirror:speed", 1.0);
        mirror_area_left    = xmlSettings.getValue("animations:mirror:area:left", -13.0f);
        mirror_area_right   = xmlSettings.getValue("animations:mirror:area:right", 13.0f);
        mirror_area_top     = xmlSettings.getValue("animations:mirror:area:top", 30.0f);
        mirror_area_bottom  = xmlSettings.getValue("animations:mirror:area:bottom", 5.0f);
        
        // chase_jump
        chase_jump_speed_leader = xmlSettings.getValue("animations:chase_jump:speed:leader", 4.0f);
        chase_jump_speed_follower = xmlSettings.getValue("animations:chase_jump:speed:follower", 4.0f);
        chase_jump_area_left = xmlSettings.getValue("animations:chase_jump:area:left", -13.0f);
        chase_jump_area_right = xmlSettings.getValue("animations:chase_jump:area:right", 13.0f);
        chase_jump_area_top = xmlSettings.getValue("animations:chase_jump:area:top", 30.0f);
        chase_jump_area_bottom = xmlSettings.getValue("animations:chase_jump:area:bottom", 5.0f);
        
        
        // gravity
        gravity_speed = xmlSettings.getValue("animations:gravity:speed", 4.0f);
        gravity_initial_velocity_mh1_x = xmlSettings.getValue("animations:gravity:initial_velocity:mh1:x", 0.01f);
        gravity_initial_velocity_mh1_y = xmlSettings.getValue("animations:gravity:initial_velocity:mh1:y", 0.01f);
        gravity_initial_velocity_mh2_x = xmlSettings.getValue("animations:gravity:initial_velocity:mh2:x", 0.0f);
        gravity_initial_velocity_mh2_y = xmlSettings.getValue("animations:gravity:initial_velocity:mh2:y", 0.0f);
        
        // circle
        circle_radius = xmlSettings.getValue("animations:circle:radius", 3.5f);
        circle_area_left = xmlSettings.getValue("animations:circle:area:left", -13.0f);
        circle_area_right = xmlSettings.getValue("animations:circle:area:right", 13.0f);
        circle_area_top = xmlSettings.getValue("animations:circle:area:top", 30.0f);
        circle_area_bottom = xmlSettings.getValue("animations:circle:area:bottom", 5.0f);
        circle_rotation_speed_min = xmlSettings.getValue("animations:circle:rotation_speed:min", 0.2f);
        circle_rotation_speed_max = xmlSettings.getValue("animations:circle:rotation_speed:max", 2.0f);
        
        // egg
        egg_radius = xmlSettings.getValue("animations:egg:radius", 2.0f);
        egg_zoom = xmlSettings.getValue("animations:egg:zoom", 25000);
        egg_focus = xmlSettings.getValue("animations:egg:focus", MAXIMUM);
        egg_iris = xmlSettings.getValue("animations:egg:iris", 29854);
        
        // chase
        chase_speed_leader = xmlSettings.getValue("animations:chase:speed:leader", 1.0f);
        chase_speed_follower = xmlSettings.getValue("animations:chase:speed:follower", 0.5f);
        chase_area_left = xmlSettings.getValue("animations:chase:area:left", -13.0f);
        chase_area_right = xmlSettings.getValue("animations:chase:area:right", 13.0f);
        chase_area_top = xmlSettings.getValue("animations:chase:area:top", 30.0f);
        chase_area_bottom = xmlSettings.getValue("animations:chase:area:bottom", 5.0f);
        
        // eyes
        eyes_area_left = xmlSettings.getValue("animations:eyes:area:left", -13.0f);
        eyes_area_right = xmlSettings.getValue("animations:eyes:area:right", -13.0f);
        eyes_area_top = xmlSettings.getValue("animations:eyes:area:top", -37.0f);
        eyes_area_bottom = xmlSettings.getValue("animations:eyes:area:bottom", 20.0f);
        eyes_distance = xmlSettings.getValue("animations:eyes:distance", 11.0f);
        
        // bounce
        bounce_speed = xmlSettings.getValue("animations:bounce:speed", 4.0f);
        
        // overlap
        overlap_area_left = xmlSettings.getValue("animations:overlap:area:left", -5.0f);
        overlap_area_right = xmlSettings.getValue("animations:overlap:area:right", 5.0f);
        overlap_area_top = xmlSettings.getValue("animations:overlap:area:top", 12.0f);
        overlap_area_bottom = xmlSettings.getValue("animations:overlap:area:bottom", 6.0f);
        overlap_speed = xmlSettings.getValue("animations:overlap:speed", 0.2f);
        overlap_zoom = xmlSettings.getValue("animations:overlap:zoom", MAXIMUM);
        overlap_focus = xmlSettings.getValue("animations:overlap:focus", MAXIMUM);
        overlap_defocus = xmlSettings.getValue("animations:overlap:defocus", MAXIMUM);
        // *** END ANIMATIONS ***
        
        // physics
        physics_particle_radius = xmlSettings.getValue("physics:partice_radius", 1.0f);
        physics_attraction = xmlSettings.getValue("physics:attraction", 0.5f);
        physics_scaling_factor = xmlSettings.getValue("physics:scaling_factor", 100.0f);
        physics_bounce = xmlSettings.getValue("physics:bounce", 1.0f);
        physics_area_left = xmlSettings.getValue("physics:area:left", -5.0f);
        physics_area_right = xmlSettings.getValue("physics:area:right", 5.0f);
        physics_area_top = xmlSettings.getValue("physics:area:top", 12.0f);
        physics_area_bottom = xmlSettings.getValue("physics:area:bottom", 6.0f);
        physics_drag = xmlSettings.getValue("physics:drag", 1.0f);
        
        ofLog() << "Settings successfully loaded from settings.xml";
    }
    else{
        ofLog() << "Could not open settings.xml";
    }
}


//--------------------------------------------------------------
void testApp::update(){
    
    myTime.update();
    
#ifdef ENABLE_OPENGL
    // apply the settings from the panel
    mh1.setFocus(panel.getValueI("mh1_focus"));
    mh1.setZoom(panel.getValueI("mh1_zoom"));
    mh1.setIris(panel.getValueI("mh1_iris"));
    mh1.setStrobe(panel.getValueI("mh1_strobe"));
    
    mh2.setFocus(panel.getValueI("mh2_focus"));
    mh2.setZoom(panel.getValueI("mh2_zoom"));
    mh2.setIris(panel.getValueI("mh2_iris"));
    mh2.setStrobe(panel.getValueI("mh2_strobe"));

    mh1.setColor(panel.getValueI("mh1_r"), panel.getValueI("mh1_g"), panel.getValueI("mh1_b"), false);
    mh1.setWhite(panel.getValueI("mh1_w"));
    mh1.setDimmer(panel.getValueI("mh1_dimmer"));
    
    mh2.setColor(panel.getValueI("mh2_r"), panel.getValueI("mh2_g"), panel.getValueI("mh2_b"), false);
    mh2.setWhite(panel.getValueI("mh2_w"));
    mh2.setDimmer(panel.getValueI("mh2_dimmer"));
#else
    // nicht aktualisieren, wenn noch keine 1/60 sekunde vorbei ist
    if(ofGetElapsedTimef() - last_update < 1.0/60.0)
        return;
    else last_update = ofGetElapsedTimef();
#endif
    
    // update the wind sensor
    wind_sensor->update();
    
    // update the heads
    mh1.update();
    mh2.update();
    
    
    //TODO: clean this up! this has gotten really ugly!
    // turn the heads to the front before we start
    if(myTime.getHours() == 17 && myTime.getMinutes() == 59 && myTime.getSeconds() < 30){
        ofLog() << "moving both heads to the still and blowing position";
        mh1.setTarget(stillandblowing_mh1_x, stillandblowing_mh1_y);
        mh2.setTarget(stillandblowing_mh2_x, stillandblowing_mh2_y);
    }
    // go into the "still and blowing" state
    else if((myTime.getMinutes() == 14 || myTime.getMinutes() == 29 || myTime.getMinutes() == 44 || myTime.getMinutes() == 59) && myTime.getSeconds() >= 30){
        if(myTime.getHours() == 17 && (myTime.getMinutes() == 59)){
            switchState(state_stillandblowing_white);
        }
        else if (myTime.getHours() > 17 || myTime.getHours() < 1)
            switchState(state_stillandblowing);
        
//        // the state before this was white => set back the colors
//        if(myTime.getMinutes() == 59 && myTime.getHours() != 17) {
//            mh1.setColor(colors[current_hour_color_id]);
//            mh2.setColor(colors[current_hour_color_id]);
//        }
    }
    
    else if(myTime.getHours() >= 18 || myTime.getHours() < 1 ){
        
        if(initialized_minute != myTime.getMinutes())
        {
            initialized_minute = myTime.getMinutes();
            
            // daily initialisations
            if(!day_init_done){
                day_init_done = true;
                
                
                ofLogToFile(ofGetTimestampString("%Y-%m-%d") + ".log");
                current_hour_color_id = -1;
                state = -1;

                // change the color switching mode
                if(current_color_mode == color_mode_random) current_color_mode = color_mode_wheel;
                else if(current_color_mode == color_mode_wheel) current_color_mode = color_mode_plus3;
                else current_color_mode = color_mode_random;
                
                ofLog() << "today's color mode is " << current_color_mode;
            }
            
            // hourly routine
            if(myTime.getMinutes() == 0){
                // switch to the inital state
                current_state_id = 0;
                switchState(program_states[current_state_id]);
                
                // do this rather than shifting to the next color to make shure that this also works when the installation has turned on after 6pm
                if(myTime.getHours()== 0)
                    current_hour_color_id = 6;
                if(myTime.getHours()>= 18 )
                    current_hour_color_id = myTime.getHours()- 18;
                
                // apply the hour color both movingheads, stopping eventual fadings
                mh1.setColor(colors[current_hour_color_id]);
                mh2.setColor(colors[current_hour_color_id]);
                
                ofLog() << "setting the hour's color on both heads to #" << current_hour_color_id << ": " << colors[current_hour_color_id];
            }
            
            // switch states every 5 minutes
            else if(myTime.getMinutes() % 5 == 0 && auto_state_shift){                
                // wenn der vorherige state der state_overlap_white state war, dann muss man bei color_wheel mode einen harten farbwechsel machen
                if(state == state_overlap_white && current_color_mode == color_mode_wheel){
                    // increase because this wasn't done during the state_overlap_white
                    mh2.setColor(colors[current_color_wheel_id]);
                    ofLog() << "coming out of the white mode. setting the color to #" << current_color_wheel_id << ": " << colors[current_color_wheel_id];
                    current_color_wheel_id++;
                    if(current_color_wheel_id == 7) current_color_wheel_id = 0;
                    mh2.fadeToColor(colors[current_color_wheel_id], 60 * 5);
                    
                    ofLog() << "starting a quick fade (only 5min) to the next color #" << current_color_wheel_id << ": " << colors[current_color_wheel_id];
                }
                nextState();
            }
            
            
            // *******************************
            // COLOR MODES
            // *******************************
            
            // random color select aus den 6 farben (die stunden-farbe wird ausgeschlossen)
            if(current_color_mode == color_mode_random){
                switch(myTime.getMinutes()){
                    case 20:
                    case 40:
                        mh2.setColor(colors[current_hour_color_id]);
                        ofLog() << "switching color of mh2 back to the hour's color #" << current_hour_color_id << ": " << colors[current_hour_color_id];
                        break;
                    case 10:
                    case 30:
                    case 50:
                        int random_color_id;
                        do {random_color_id = (int) floor(ofRandom(6)+0.5f);}
                        while(random_color_id == current_hour_color_id && random_color_id > 6); // keep generating numbers while it is not different from the hourly color (and make sure that it does not become larger than 6 by accident
                        mh2.setColor(colors[random_color_id]);
                        ofLog() << "switching color of mh2 to random color #" << random_color_id << ": " << colors[random_color_id];
                        break;
                }
            }
            
            else if(current_color_mode == color_mode_wheel){
                // set the color wheel id back every hour
                if(myTime.getMinutes() == 0){
                    current_color_wheel_id = current_hour_color_id;
                    ofLog() << "settings back the color wheel";
                }
                
                // the rest of the updates for this state are outside this minute-wise routine
            }
            
            // color + 3
            else if (current_color_mode == color_mode_plus3){
                switch(myTime.getMinutes()){
                    case 20:
                    case 40:
                        mh2.setColor(colors[current_hour_color_id]);
                        ofLog() << "switching color of mh2 back to the hour's color #" << current_hour_color_id << ": " << colors[current_hour_color_id];
                        break;
                    case 10:
                    case 30:
                    case 50:
                        if(current_hour_color_id + 3 <= 6){
                            mh2.setColor(colors[current_hour_color_id + 3]);
                            ofLog() << "switching color of mh2 to +3: #" << current_hour_color_id + 3 << ": " << colors[current_hour_color_id + 3];
                        }
                        else {
                            mh2.setColor(colors[current_hour_color_id - 4]);
                            ofLog() << "switching color of mh2 to +3: #" << current_hour_color_id - 4 << ": " << colors[current_hour_color_id - 4];
                        }
                        break;
                }
            }
        }

        // fade to next color when fading has finished
        // it is not very nice that this color mode has this appendix down here but it does not fit in the minute-timing above
        if(current_color_mode == color_mode_wheel && !mh2.color_fade.is_fading && state != state_overlap_white){            
            // define the next color
            current_color_wheel_id++;
            if(current_color_wheel_id == 7) current_color_wheel_id = 0;

            mh2.fadeToColor(colors[current_color_wheel_id], 7.5 * 60.0);
            ofLog() << "fading mh2 to next color #" << current_color_wheel_id << ": " << colors[current_color_wheel_id];
            
            // fade both heads to the next hour color at the end of the hour
            if(mh2.getColor() == colors[current_hour_color_id] && myTime.getMinutes() != 0){
                if(myTime.getHours()!= 0){
                    mh1.fadeToColor(colors[current_color_wheel_id], 7.5 * 60.0);
                    ofLog() << "fading mh1 to next hour color #" << current_color_wheel_id << ": " << colors[current_color_wheel_id];
                }
                // make a fade to white at the end
                else {
                    mh1.fadeToColor(ofColor(255, 255, 255), 7.5 * 60.0);
                    mh2.fadeToColor(ofColor(255, 255, 255), 7.5 * 60.0);
                    ofLog() << "fading both heads to white for the end";
                }
            }
            
        }
    }
    
    else if(myTime.getHours()== 1 && myTime.getMinutes() == 0 && myTime.getSeconds() < 30){
        switchState(state_stillandblowing_white);
    }
    
    // reset the heads after finishing the program
    else{
        mh1.totalFixtureReset();
        mh2.totalFixtureReset();
        day_init_done = false;
//        ofLog() << "resetting the heads"; // diese ausgabe wŸrde die ganze nacht Ÿber 60mal pro sekunde ausgegeben werden...
    }
    
    // update all the movement stuff
    // TODO: this could also be switched off when the installation is not running.
    updateStates();
    
    
#ifdef ENABLE_OPENGL
    // update the programmatically changed values to the panel
    panel.setValueI("mh1_zoom", mh1.getZoom());
    panel.setValueI("mh2_zoom", mh2.getZoom());
    panel.setValueI("mh1_focus", mh1.getFocus());
    panel.setValueI("mh2_focus", mh2.getFocus());
    panel.setValueI("mh1_r", mh1.getColor().r);
    panel.setValueI("mh1_g", mh1.getColor().g);
    panel.setValueI("mh1_b", mh1.getColor().b);
    panel.setValueI("mh1_w", mh1.getWhite());
    panel.setValueI("mh2_r", mh2.getColor().r);
    panel.setValueI("mh2_g", mh2.getColor().g);
    panel.setValueI("mh2_b", mh2.getColor().b);
    panel.setValueI("mh2_w", mh2.getWhite());
    panel.setValueI("mh1_strobe", mh1.getStrobe());
    panel.setValueI("mh2_strobe", mh2.getStrobe());
    panel.setValueI("mh1_dimmer", mh1.getDimmer());
    panel.setValueI("mh2_dimmer", mh2.getDimmer());
#endif
    
    // todo: decide if this should be done all the time even when the installation is not running
    // update artnet
    node.updateDataByIndex(mh1.getUniverse(), mh1.getChannels(), 36);
    node.updateDataByIndex(mh2.getUniverse(), mh2.getChannels(), 36);
    node.send();
}

void testApp::updateStates(){
    ofVec2f randomVector;
    
    // **********************************************
    // TODO: edit in the wind sensor data here

    

    
    
//#ifdef ENABLE_OPENGL
//    // rotation variables
//    ofVec2f centerToMouse = toReal(ofGetMouseX(), ofGetMouseY()) - ofVec2f(0, building_height/2);
//    ofVec2f rotationCenterToMouse = toReal(ofGetMouseX(), ofGetMouseY()) - rotationCenter;
//#else
//    ofVec2f centerToMouse = ofVec2f(0, building_height/2);
//    ofVec2f rotationCenterToMouse = rotationCenter;
//#endif
//    centerToMouse.limit(30);
//    float rotationSpeed = ofMap(rotationCenterToMouse.length(), 0, 30, circle_rotation_speed_min, circle_rotation_speed_max);
//    rotationCenter.x += ((rotationCenterToMouse.normalized()/300).x) * rotationSpeed;
//    rotationCenter.y += ((rotationCenterToMouse.normalized()/300).y) * rotationSpeed;
    
    float rotationSpeed = ofMap(wind_sensor->getScaledSpeed(), 0, 1, circle_rotation_speed_min, circle_rotation_speed_max);
    rotationCenter += (wind_sensor->getScaledVector() / 300) * rotationSpeed;
    
    // check for the maximums
    if(rotationCenter.x > circle_area_right ) rotationCenter.x = circle_area_right;
    if(rotationCenter.x < circle_area_left) rotationCenter.x = circle_area_left;
    if(rotationCenter.y > circle_area_top) rotationCenter.y = circle_area_top;
    if(rotationCenter.y < circle_area_bottom) rotationCenter.y = circle_area_bottom;

    
    // END OF WIND SENSOR STUFF
    // **********************************************
    

    switch(state){

            
        // -----------------------------------------------
        // mirror movement
        // -----------------------------------------------
        case state_mirror:
        case state_mirror_defocus:
            if(!state_initialized){
                mh1.setSpeed(4);
                mh2.setSpeed(4);
                mh1.openShutter();
                mh2.openShutter();
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                if(state == state_mirror){
                    mh1.setFocus(mh1_focus);
                    mh2.setFocus(mh2_focus);
                }
                else {
                    mh1.setFocus(mh1_defocus);
                    mh2.setFocus(mh2_defocus);
                }
                
                // move them to the first spot quickly
                randomVector.x = ofRandom(mirror_area_left, mirror_area_right);
                randomVector.y = ofRandom(mirror_area_bottom, mirror_area_top);
                mh1.setTarget(randomVector);
                mh2.setTarget(-randomVector.x, randomVector.y);
                
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
                
                mh1.closeShutter();
                mh2.closeShutter();

            }
            
            if(mh1.getPositionReached() && mh2.getPositionReached()){
                mh1.openShutter();
                mh2.openShutter();
                mh1.setSpeed(mirror_speed + wind_sensor->getScaledSpeed());
                mh2.setSpeed(mirror_speed + wind_sensor->getScaledSpeed());
                randomVector.x = ofRandom(mirror_area_left, mirror_area_right);
                randomVector.y = ofRandom(mirror_area_bottom, mirror_area_top);
                mh1.setTarget(randomVector);
                mh2.setTarget(-randomVector.x, randomVector.y);
            }
            break;
            
        // -----------------------------------------------
        // chase jump
        // -----------------------------------------------
        case state_chase_jump:
            if(!state_initialized){
                mh1.setSpeed(chase_jump_speed_leader);
                mh2.setSpeed(chase_jump_speed_follower);
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                mh1.openShutter();
                mh2.openShutter();
                mh1.setFocus(mh1_focus);
                mh2.setFocus(mh2_focus);
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
            }
            
            // move mh1 to next position when mh2 has reached it
            if(mh1.getPositionReached() && mh2.getPositionReached()){
                randomVector.x = ofRandom(mirror_area_left, mirror_area_right);
                randomVector.y = ofRandom(mirror_area_bottom, mirror_area_top);
                mh1.setTarget(randomVector);
                mh1.closeShutter();
                mh2.setTarget(randomVector);    // set the target here also for mh2 so that the program does not jump in here anymore
                mh2.setSpeed(0); // make it so slow that it doesn't move
            }
            
            // activate the light of mh1 when it has reached its position and start the movement of mh2
            if(mh1.getPositionReached()){
                mh1.openShutter();
                mh2.setTarget(mh1.getTarget());
                
                // make the speed wind effected
                mh2.setSpeed(chase_jump_speed_follower + wind_sensor->getScaledSpeed());
            }
            
            break;
            
        // -----------------------------------------------
        // gravity
        // -----------------------------------------------
        case state_gravity:
        case state_gravity_defocus:
            if(!state_initialized){
                mh1.openShutter();
                mh2.openShutter();
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                mh1.setSpeed(gravity_speed);
                mh2.setSpeed(gravity_speed);
                physics.clear();
                p1 = physics.makeParticle(mh1.getPosition() * physics_scaling_factor);
                p2 = physics.makeParticle(mh2.getPosition() * physics_scaling_factor);
                
                p1->setBounce(physics_bounce)->setRadius(physics_particle_radius)->setVelocity(ofVec2f(gravity_initial_velocity_mh1_x, gravity_initial_velocity_mh1_y));
                p2->setBounce(physics_bounce)->setRadius(physics_particle_radius)->setVelocity(ofVec2f(gravity_initial_velocity_mh2_x, gravity_initial_velocity_mh2_y));
                
                physics.makeAttraction(p1, p2, physics_attraction);
                physics.disableCollision();
                physics.setDrag(physics_drag);
                
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
                
                // set focus
                if(state == state_gravity) {
                    mh1.setFocus(mh1_focus);
                    mh2.setFocus(mh2_focus);
                }
                else {
                    mh1.setFocus(mh1_defocus);
                    mh2.setFocus(mh2_defocus);
                }
            }
            
            // relocate the physics particle so they match with the position of the moving heads
            // this is a bit problematic, though. the particles just shouldn't move faster that the heads can
            p1->moveTo(mh1.getPosition() * physics_scaling_factor);
            p2->moveTo(mh2.getPosition() * physics_scaling_factor);
            physics.update();
            mh1.setTarget(p1->getPosition() / physics_scaling_factor);
            mh2.setTarget(p2->getPosition() / physics_scaling_factor);
            break;
            
        // -----------------------------------------------
        // circle
        // -----------------------------------------------
        case state_circle:
            if(!state_initialized){
                mh1.openShutter();
                mh2.openShutter();
                rotationCenter = ofVec2f(0, building_height/2);
                rotationV1 = ofVec2f(0, circle_radius);
                rotationV2 = ofVec2f(0, -circle_radius);
                mh1.setSpeed(10);  // was 0
                mh2.setSpeed(10);  // was 0
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                mh1.setFocus(mh1_focus);
                mh2.setFocus(mh2_focus);
                
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
            }
            
            // rotate the vectors by an angle that is dependant on the windspeed
            rotationV1.rotate(rotationSpeed);
            rotationV2.rotate(rotationSpeed);
            
            // update the target positions
            mh1.setTarget((rotationCenter+rotationV1).x, (rotationCenter+rotationV1).y);
            mh2.setTarget((rotationCenter+rotationV2).x, (rotationCenter+rotationV2).y);
            
            
            break;
            
        // -----------------------------------------------
        // egg circle
        // -----------------------------------------------
        case state_egg:
            if(!state_initialized){
                mh1.openShutter();
                mh2.openShutter();
                rotationCenter = ofVec2f(0, building_height/2);
                rotationV2 = ofVec2f(0, egg_radius);
                mh1.setSpeed(4); // was 0
                mh2.setSpeed(4); // was 0
                mh1.setZoom(egg_zoom);  // grš§er machen
                mh2.setZoom(MAXIMUM);   // ganz klein
                mh1.setFocus(mh1_defocus);
                mh2.setFocus(mh2_focus);

                mh1.setIris(mh1_iris);
                mh2.setIris(egg_iris);

            }
            
            mh1.setTarget(rotationCenter.x, rotationCenter.y);
            
            // mh2 drehen
            rotationV2.rotate(rotationSpeed);
            mh2.setTarget((rotationCenter+rotationV2).x, (rotationCenter+rotationV2).y);
            
            break;
            
        // -----------------------------------------------
        // chase
        // -----------------------------------------------
        case state_chase:
            if(!state_initialized){
                mh1.openShutter();
                mh2.openShutter();
                mh1.setSpeed(chase_speed_leader);
                mh2.setSpeed(chase_speed_follower);
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                mh1.setFocus(mh1_focus);
                mh2.setFocus(mh2_focus);
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
            }
            
            if(mh1.getPositionReached()){
                randomVector.x = ofRandom(chase_area_left, chase_area_right);
                randomVector.y = ofRandom(chase_area_bottom, chase_area_top);
                mh1.setTarget(randomVector);
            }
            
            mh1.setSpeed(chase_speed_leader + ofMap(wind_sensor->getScaledSpeed(), 0, 1, 0, 0.5));
            mh2.setSpeed(chase_speed_follower + ofMap(wind_sensor->getScaledSpeed(), 0, 1, 0, 0.5));

            mh2.setTarget(mh1.getPosition());
            break;
            
        // -----------------------------------------------
        // eyes
        // -----------------------------------------------
        case state_eyes:
            if(!state_initialized){
                mh1.openShutter();
                mh2.openShutter();
                mh1.setSpeed(4); // was 0
                mh2.setSpeed(4); // was 0
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                mh1.setFocus(mh1_focus);
                mh2.setFocus(mh2_focus);
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
            }
            
            if(mh1.getPositionReached() && ofGetElapsedTimef() - last_movement >= 6.0 - ofMap(wind_sensor->getScaledSpeed(), 0, 1, 0, -2)){
                randomVector.x = ofRandom(eyes_area_left, eyes_area_right);
                randomVector.y = ofRandom(eyes_area_bottom, eyes_area_top);
                mh2.setTarget(randomVector);
                mh1.setTarget(randomVector.x + eyes_distance, randomVector.y);
                last_movement = ofGetElapsedTimef();
            }
            break;
            
        // -----------------------------------------------
        // bouncing no collision
        // -----------------------------------------------
        case state_bounce:
            if(!state_initialized){
                mh1.openShutter();
                mh2.openShutter();
                mh1.setSpeed(4); // was 0
                mh2.setSpeed(4); // was 0
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                mh1.setFocus(mh1_focus);
                mh2.setFocus(mh2_focus);
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
                
                // physics stuff
                physics.clear();
                physics.setDrag(1.0);
                p1 = physics.makeParticle(mh1.getPosition() * physics_scaling_factor);
                p2 = physics.makeParticle(mh2.getPosition() * physics_scaling_factor);
                p1->setBounce(physics_bounce)->setRadius(physics_particle_radius)->setVelocity(ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1)).scale(bounce_speed));
                p2->setBounce(physics_bounce)->setRadius(physics_particle_radius)->setVelocity(ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1)).scale(bounce_speed));
            }
            
            // relocate the physics particle so they match with the position of the moving heads
            p1->moveTo(mh1.getPosition() * physics_scaling_factor);
            p2->moveTo(mh2.getPosition() * physics_scaling_factor);
            physics.update();
            mh1.setTarget(p1->getPosition() / physics_scaling_factor);
            mh2.setTarget(p2->getPosition() / physics_scaling_factor);
            break;
            
        // -----------------------------------------------
        // overlap
        // -----------------------------------------------
        case state_overlap_white:
        case state_overlap:
            if(!state_initialized){
                mh1.setSpeed(4); // full speed to reach start position
                mh2.setSpeed(4); // full speed to reach start position
                mh1.openShutter();
                mh2.openShutter();
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
                
                // punkte ein bisschen grš§er machen
                mh1.setZoom(overlap_zoom);
                mh2.setZoom(overlap_zoom);
                
                // schnell zur startposition fahren
                mh1.setTarget(0, overlap_area_top);
                mh2.setTarget(0, overlap_area_top);

                mh1.setFocus(overlap_focus);
                if(state == state_overlap_white){
                    mh1.setFocus(overlap_defocus);
                    mh2.setFocus(mh2_focus);
                    mh2.setColor(255, 255, 255);
                    mh2.setWhite(255, 255);
                }
                else { mh2.setFocus(overlap_focus); }
                
            }
            
            // beide heads sehr langsam machen, wenn sie die startposition erreicht haben
            if(mh1.getPositionReached() && mh2.getPositionReached() ){
                mh1.setSpeed(overlap_speed); // was 253
                mh2.setSpeed(overlap_speed); // was 253
            }
            
            if(ofGetElapsedTimef() - last_movement >= 10.0){
                randomVector.x = ofRandom(overlap_area_left, overlap_area_right);
                randomVector.y = ofRandom(overlap_area_bottom, overlap_area_top);
                mh1.setTarget(randomVector);
                mh2.setTarget(randomVector);
                last_movement = ofGetElapsedTimef();
            }

            break;
            
        // -----------------------------------------------
        // mouse control
        // -----------------------------------------------
        case state_mouse:
            mh1.setTarget(toReal(ofGetMouseX(), ofGetMouseY()));
            mh2.setTarget(toReal(ofGetMouseX(), ofGetMouseY()));
            break;
            
        
        // -----------------------------------------------
        // still and blowing
        // -----------------------------------------------
        case state_stillandblowing:
        case state_stillandblowing_white:
            if(!state_initialized){                
                mh1.setSpeed(4);
                mh2.setSpeed(4);
                mh1.setZoom(MAXIMUM);
                mh2.setZoom(MAXIMUM);
                mh1.setFocus(mh1_focus);
                mh2.setFocus(mh2_focus);
                mh1.openShutter();
                mh2.openShutter();
                // getting them out of the reset mode (might not be necessary)
                mh1.setSpecial(0);
                mh2.setSpecial(0);
                mh1.setDimmer(mh1_dimmer);
                mh2.setDimmer(mh2_dimmer);
                mh1.setIris(mh1_iris);
                mh2.setIris(mh2_iris);
                
                if(state == state_stillandblowing_white){
                    mh1.setColor(255, 255, 255);
                    mh1.setWhite(255, 255);
                    mh2.setColor(255, 255, 255);
                    mh2.setWhite(255, 255);
                }
                mh1.setTarget(stillandblowing_mh1_x, stillandblowing_mh1_y);
                mh2.setTarget(stillandblowing_mh2_x, stillandblowing_mh2_y);
            }
            
            if(mh1.getPositionReached()){
                randomVector.x = stillandblowing_mh1_x + ofRandom(-0.2, 0.2);
                randomVector.y = stillandblowing_mh1_y + ofRandom(-0.2, 0.2);
                mh1.setTarget(randomVector);
            }
            if(mh2.getPositionReached()){
                randomVector.x = stillandblowing_mh2_x + ofRandom(-0.2, 0.2);
                randomVector.y = stillandblowing_mh2_y + ofRandom(-0.2, 0.2);
                mh2.setTarget(randomVector);
            }
            
            break;
    }
    
    if(!state_initialized){
        state_initialized = true;
    }
}

//--------------------------------------------------------------
void testApp::draw(){

    ofPushMatrix();
    
    ofTranslate(ursprung);                              // ursprung verschieben
    building.draw(scaling * (-building_width/2), -scaling * building_height, scaling * building_width, scaling *building_height);                               // scale and draw building
    ofScale(scaling, -scaling);                         // scale
    ofSetColor(255);
    ofLine(-ofGetWidth()/2, 0, ofGetWidth()/2, 0);      // draw horizon

    // draw the beams of the moving heads
    mh1.draw();
    mh2.draw();
    
    ofPopMatrix();
    
    // draw data
    ofSetColor(255);
    ofDrawBitmapStringHighlight("MH1", 10, ofGetHeight() - 96);
    ofDrawBitmapString("x = " + ofToString(mh1.getPosition().x), 10, ofGetHeight()-75);
    ofDrawBitmapString("y = " + ofToString(mh1.getPosition().y), 10, ofGetHeight()-60);
    ofDrawBitmapString("zoom = " + ofToString(mh1.getZoom()), 10, ofGetHeight()-45);
    ofDrawBitmapString("focus = " + ofToString(mh1.getFocus()), 10, ofGetHeight()-30);
    ofDrawBitmapString("speed = " + ofToString(mh1.getSpeed()), 10, ofGetHeight()-15);

    ofDrawBitmapStringHighlight("MH2", 150, ofGetHeight() - 96);
    ofDrawBitmapString("x = " + ofToString(mh2.getPosition().x), 150, ofGetHeight()-75);
    ofDrawBitmapString("y = " + ofToString(mh2.getPosition().y), 150, ofGetHeight()-60);
    ofDrawBitmapString("zoom = " + ofToString(mh2.getZoom()), 150, ofGetHeight()-45);
    ofDrawBitmapString("focus = " + ofToString(mh2.getFocus()), 150, ofGetHeight()-30);
    ofDrawBitmapString("speed = " + ofToString(mh2.getSpeed()), 150, ofGetHeight()-15);
        
    ofDrawBitmapString(ofToString(myTime.getHours(), 2, '0') + ":" + ofToString(myTime.getMinutes(), 2, '0') + ":" + ofToString(myTime.getSeconds(), 2, '0'), 300, ofGetHeight()-75);
    
    if(draw_debug){
        switch(state){
            case state_gravity:
            case state_gravity_defocus:
            case state_bounce:
                ofCircle(toScreen(p1->getPosition()/physics_scaling_factor), 2);
                ofCircle(toScreen(p2->getPosition()/physics_scaling_factor), 2);
                break;
            case state_egg:
            case state_circle:
                ofLine(toScreen(rotationCenter), ofVec2f(ofGetMouseX(), ofGetMouseY()));
                ofCircle(toScreen(rotationCenter), 2);
                break;
        }
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    int x, y;
    switch(key){
            
        case 'a':
            auto_state_shift = !auto_state_shift;
            break;
            
        case '+':
            myTime.addMinutes(5);
            break;
        
        case '-':
            myTime.addMinutes(-5);
            break;
            
                        
        // -----------------------------------------------
        // states
        // -----------------------------------------------    
        case '0':
            switchState(0);
            break;
            
        case '1':
            switchState(1);
            break;
            
        case '2':
            switchState(2);
            break;
            
        // gravity
        case '3':
            switchState(3);
            break;
        
        // circling
        case '4':
            switchState(4);
            break;
        
        // egg circling
        case '5':
            switchState(5);
            break;
        
        // chase
        case '6':
            switchState(6);
            break;

        // eyes
        case '7':
            switchState(7);
            break;
        
        // bouncing no collision
        case '8':
            switchState(8);
            break;
        
        case '9':
            switchState(9);
            break;
        
        case ' ':
            draw_debug = !draw_debug;
            break;
            
        case 'm':
            switchState(state_mouse);
            break;
    }
}

//--------------------------------------------------------------
void testApp::switchState(int state){
    if(this->state != state){
        this->state = state;
        state_initialized = false;  
        overlap_initiated = false;
        ofLog() << "switched to state " << state;
    }
}

int testApp::nextState(){
    current_state_id++;
    if(current_state_id > 11) current_state_id = 0;
    
    ofLog() << "switching to #"<<current_state_id << "(state " << program_states[current_state_id] << ")";
    
    switchState(program_states[current_state_id]);

    return program_states[current_state_id];
}

ofVec2f testApp::toReal(ofVec2f screen){
    return toReal(screen.x, screen.y);
}

ofVec2f testApp::toReal(float x, float y){
    return ofVec2f((x-ursprung.x)/scaling, -(y-ursprung.y)/scaling);
}

ofVec2f testApp::toScreen(ofVec2f real){
    return toScreen(real.x, real.y);
}

ofVec2f testApp::toScreen(float x, float y){
    return ofVec2f(ursprung.x + x*scaling, ursprung.y - y*scaling);
}
