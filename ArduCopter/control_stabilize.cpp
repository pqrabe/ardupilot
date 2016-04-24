/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Copter.h"
//#include "UserCode.cpp"
/*
 * control_stabilize.pde - init and run calls for stabilize flight mode
 */
extern volatile uint32_t forward;//distance to fowared object
extern volatile uint32_t right;
extern volatile uint32_t left;
extern volatile uint32_t back;

extern volatile uint32_t DataFromCont;
extern volatile float Roll;
extern volatile float Pitch;
uint32_t i = 0;
// stabilize_init - initialise stabilize controller

bool Copter::stabilize_init(bool ignore_checks)
{

    // if landed and the mode we're switching from does not have manual throttle and the throttle stick is too high
    if (motors.armed() && ap.land_complete && !mode_has_manual_throttle(control_mode) && (g.rc_3.control_in > get_non_takeoff_throttle())) {
        return false;
    }
    // set target altitude to zero for reporting
    pos_control.set_alt_target(0);



    return true;
}

// stabilize_run - runs the main stabilize controller
// should be called at 100hz or more
void Copter::stabilize_run()
{
    float target_roll, target_pitch;
    float target_yaw_rate;
    int16_t pilot_throttle_scaled;

    // if not armed or throttle at zero, set throttle to zero and exit immediately
    if(!motors.armed() || ap.throttle_zero) {
        attitude_control.set_throttle_out_unstabilized(0,true,g.throttle_filt);
        // slow start if landed
        if (ap.land_complete) {
            motors.slow_start(true);
        }
        return;
    }

    // apply SIMPLE mode transform to pilot inputs
    update_simple_mode();



    DataFromCont = i;//pos = back &right
    #define DST 256
    	int16_t cpitch = channel_pitch->control_in;
    	int16_t croll = channel_roll->control_in;



	if(forward < DST){
		channel_pitch->control_in=(((-44000+cpitch)>>8)*forward)+44000;
	}
	if(back < DST){
		channel_pitch->control_in=(((44000+cpitch)>>8)*forward)-44000;
	}



    //#define LONGDIST 200

    //if(forward < DST && back < DST){
    //	 channel_pitch->control_in = (back - forward)*20;
    //}else{
    //	
    //}

    /*if(forward < DST && back < DST){
    	 channel_pitch->control_in = (back - forward)*20;
    }else{

	    if(forward < DST){
	        channel_pitch->control_in = ((DST-forward)+100)*40;
	    }else if(forward < LONGDIST && channel_pitch->control_in <= 0){
	    	channel_pitch->control_in = (LONGDIST-forward)*40 + (channel_pitch->control_in/2);
	    }
	    if(back < DST){
	        channel_pitch->control_in = (DST-back)*-40;
	    }else if(back < LONGDIST && channel_pitch->control_in >= 0){
	    	channel_pitch->control_in = (LONGDIST-back)*-40 + (channel_pitch->control_in/2);
	    }
	}*/


    if(left < DST && channel_roll->control_in < 0){
        channel_roll->control_in = 0;
    }
    if(right < DST && channel_roll->control_in > 0){
        channel_roll->control_in = 0;
    }


    Roll = channel_roll->control_in;
    Pitch = channel_pitch->control_in;


    if (i >= 1000){
        i = 0;
    }
    i++;




    // convert pilot input to lean angles
    // To-Do: convert get_pilot_desired_lean_angles to return angles as floats
    get_pilot_desired_lean_angles(channel_roll->control_in, channel_pitch->control_in, target_roll, target_pitch, aparm.angle_max);
    // get pilot's desired yaw rate
    target_yaw_rate = get_pilot_desired_yaw_rate(channel_yaw->control_in);

    // get pilot's desired throttle
    pilot_throttle_scaled = get_pilot_desired_throttle(channel_throttle->control_in);

    // call attitude controller
    attitude_control.input_euler_angle_roll_pitch_euler_rate_yaw_smooth(target_roll, target_pitch, target_yaw_rate, get_smoothing_gain());

    // body-frame rate controller is run directly from 100hz loop

    // output pilot's throttle
    attitude_control.set_throttle_out(pilot_throttle_scaled, true, g.throttle_filt);
}
