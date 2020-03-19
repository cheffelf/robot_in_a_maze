Robot In A Maze: A maze runner simulation
===

Goal
==

This is a basic maze runner simulator based on the elma event loop and process manager:
https://github.com/klavinslab/elma

and the enviro simulator:
https://github.com/klavinslab/enviro


The simulation is of a static maze with one exit. A Robot is placed in the opposite corner and has two ways available to it to navigate the maze, that it calls when it gets near an obstacle. The user can click a button in the upper right hand corner of the screen to switch the method the robot will use on the fly. The robot is using a finite state machine and the two methods are represented as states:

- FindPath - a test algorithm that is the default
- Random rotation - this state will randomly pick a new path.

There is also a third state that is the normal driving mode:
- MovingForward - driving forward at a constant speed

The FindPath state is a work in progress but does have a few functions now. At this time it is not especially useful in helping the robot escape the maze, although the robot will escape eventually. In FindPath the robot stops and turns 360 degrees to scan the environment. It will then pick the longest path available to it based on this scan, ignoring paths that are behind it. Behind it means relative to the orientation of the robot when it arrived at FindPath. Once a new path is found the robot will rotate to this new orientation and trigger a state change to MovingForward to start driving.

The robot has 5 distance sensors that it can use in planning the next path, although at this time FindPath is only using one, sensor 0, the sensor gathering data directly in front of the robot. If the Robot it pointing North, the 5 sensors are oriented like this:

```bash
    NW   N   NE
 W  _ \  |  / _ E
 ```

 Which maps to sensor ID's:
 ```bash
     2   0   1
 4  _ \  |  /  _ 3
```

Challenges
==

Figuring out the order of the ID's of the sensors took some time. I solved this by centering the robot in a simple rectangle to find sensors with matching values (2 and 1 would have the same values, 4 and 3 would have the same values, when the robot is oriented at 0 radians), then offseting the initial position of the robot to find the changes in the values being returned. I also oriented the robot at various angles and measured the distance being returned by the sensor to confirm. Another challenge was getting data between states, which in the end I pivoted on and used methods in the states that could be called from the AgentInterface.
I have several more plans for the FindPath state and would like to build in more logic, such as finding intersections when sensors 4 or 3 show large jumps in value, and by using sensors to better center the robot down halls and to pick its "longest path" to be orthangonal to walls in front of it. At this time FindPath is prone to getting stuck at dead ends, although it will get out of them.

How to run the simulation
===

Clone this repository to a local directory:
```bash
git clone https://github.com/cheffelf/robot_in_a_maze.git
```
With docker installed, launch a docker image that has enviro and elma built in:
```bash
docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.6 bash
```
Where $PWD is the directory where this repository is located. In the docker image start esm, which will launch a web server:

```bash
esm start
```
You can then navigate to http://localhost and see the basic esm server with no simulation yet.
In docker, compile the code and launch enviro to start the simulation:
```bash
make
enviro
```

This simulation is based on the enviro example program avoiders, found at:
https://github.com/klavinslab/enviro/tree/master/examples/avoiders


Future plans
==

The additional sensors can be used to better center the robot down halls as well as find intersections where it can take multiple paths. These would be tracked in a way that the robot could have information on where it has already traveled and not repeat those paths. One way to do this might be to designate paths as dead ends and block them from being used in future planning.