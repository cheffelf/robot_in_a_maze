#ifndef __ROBOT_AGENT__H
#define __ROBOT_AGENT__H 

#include <stdio.h>
#include <string>
#include <math.h>
#include "enviro.h"

namespace {

    using namespace std;
    using namespace enviro;

    // The FindPath state is a work in progress and doesn't yet offer the robot much help in navigating the maze.
    // It's primary goal is offer more intelligent picking of the next path when navigating the maze. At the time of this writing 
    // it is only partially done. This is the default method the robot will use next when it hits an obstacle.
    //
    // It will do a 360 degree scan and find the longest available path that isn't a path behind it.
    //
    // To do this it starts in scanning mode, does a turn of 2 * PI radians, finds the longest path as seen by sensor<0>
    // (in front of the robot), and records the angle.
    // It then leaves scanning mode and orients robot towards the new path.
    // Then emits an event to change the state of the robot to MovingForward

    class FindPath : public State, public AgentInterface {
        public:
        void entry(const Event& e) {
            targetPath = sensor_value(0);
            targetAngle = angle();
            originalAngle = angle();
            // reverseAngleMin and reverseAngleMax are the range of angles behind the robot
            // that will be skipped later when picking a new path
            reverseAngleMin = angle() + M_PI -1.5;
            reverseAngleMax = angle() + M_PI + 1.5;
            scanning = true;
            skipThisAngle = false;
            clear_label();

        }
        void during() {
            curAngle = angle();
            if (useRotate == true ) {
                clear_label();
                emit(Event("wander"));
            }

            // Scanning stage - does a 360 degree scan and picks the longest path that was not behind it when it arrived
            // at this state.

            if (scanning){ 
                if(curAngle <= originalAngle + 2 * M_PI){
                    //label("scanning", 20, 5);
                    //path_options.push_back(make_tuple(sensor_value(0), angle()));
                    track_velocity(0,2);
                    
                    // ignoring things that were behind the robot when it arrived at this state
                    if ((curAngle < reverseAngleMax) && (curAngle > reverseAngleMin)) {
                        skipThisAngle = true;
                    } else { skipThisAngle = false; }

                    // if the value is not behind the robot, keep track of the largest one and the angle
                    if ((sensor_value(0) > targetPath) && (skipThisAngle == false)) {
                        targetPath = sensor_value(0);
                        targetAngle = angle();
                    }
                }
                else {
                    scanning = false;
                    track_velocity(0,-3);
                }
                // no longer in scanning mode, now we turn the robot towards the desired path
                // and when it's within 0.1 radians send an event to move to the next state (of MovingForward)
            } else {
                label("orienting", 20, 5);
                if (angle() <= (targetAngle + 0.1) && angle() >= (targetAngle - 0.1)){
                    emit(Event(tick_name));
                } else if (angle() < targetAngle){
                    track_velocity(0,0.5);
                } else if (angle() > targetAngle){
                    track_velocity(0,-0.5);
                }
            }
        }

        void exit(const Event& e) { clear_label(); }
        void set_tick_name(std::string s) { tick_name = s; }
        void use_rotate() { 
            useRotate = !useRotate;
        }

        std::string tick_name;
        //std::vector<tuple<double, double>> path_options;
        double targetPath;
        double targetAngle;
        double originalAngle;
        double curAngle;
        double reverseAngleMin;
        double reverseAngleMax;
        bool scanning;
        bool skipThisAngle;
        bool useRotate = false;

    };

    class MovingForward : public State, public AgentInterface {
        public:
        void entry(const Event& e) { clear_label(); }
        void during() {
            track_velocity(2,0);

            if ( sensor_value(0) < 40 ) {
                emit(Event(tick_name));
            }    
        }
        void exit(const Event& e) {}
        void set_tick_name(std::string s) { tick_name = s; }
        void set_target_angle(double a) { targetAng = a; }
        std::string tick_name;
        double targetAng;
    };

    class Rotating : public State, public AgentInterface {
        public:
        void entry(const Event& e) { 
            rate = rand() % 2 == 0 ? 2 : -2;
            clear_label();
        }
        void during() { 
            
            if (useRotate == false ) {
                clear_label();
                emit(Event("wander"));
            }
            track_velocity(0,rate); 
            if ( sensor_value(0) > 100 ) {
                emit(Event(tick_name));
            }           
        }
        void exit(const Event& e) {
            decorate(""); 
            clear_label();
        }
        void use_rotate() {
            useRotate = !useRotate;
        }

        double rate;
        void set_tick_name(std::string s) { tick_name = s; }
        std::string tick_name;
        bool useRotate = false;

    };

    class RobotController : public StateMachine, public AgentInterface {

        public:
        RobotController() : StateMachine() {

            set_initial(find_path);
            tick_name = "ping";
            add_transition(tick_name, moving_forward, find_path);
            add_transition(tick_name, rotating, moving_forward);
            add_transition(tick_name, find_path, moving_forward);
            add_transition("wander", find_path, rotating);
            add_transition("wander", rotating, find_path);
            moving_forward.set_tick_name(tick_name);
            rotating.set_tick_name(tick_name);
            find_path.set_tick_name(tick_name);

        }

        void init() {
            watch("button_click", [&](Event& e) {
                //std::cout << e.value() << "\n";
                find_path.use_rotate();
                rotating.use_rotate();
                useRotate = !useRotate;
                if (useRotate){
                    rotate_mode = "Random Rotate mode";
                }
                else {rotate_mode = "Test mode - using FindPath state";}
                cout << "Switching to " << rotate_mode << endl;
                e.stop_propagation();
            });
            StateMachine::init();
        }

        FindPath find_path;
        MovingForward moving_forward;
        Rotating rotating;
        std::string tick_name;
        std::string rotate_mode;
        bool useRotate = false;

    };

    class Robot : public Agent {

        public:
        Robot(json spec, World& world) : Agent(spec, world) {
            add_process(wc);
        }

        RobotController wc;

    };

    DECLARE_INTERFACE(Robot);

}

#endif