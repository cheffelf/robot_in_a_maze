Robot In A Maze: A maze runner simulation
===

Goal
==

This is a basic maze runner simulator based on the elma event loop and process manager:
https://github.com/klavinslab/elma

and the enviro simulator:
https://github.com/klavinslab/enviro


The simulation is of a static maze with one exit and a Robot placed in the opposite corner. As the robot navigates the maze it has two methods it can use to pick a new path when it runs into obstacles. The user can click a button in the upper right hand corner of the screen to switch the method the robot will use at any point. The robot is using a finite state machine and the two methods are represented as states:

- FindPath - the default, will pick a new path based on some logic (very basic logic at this point)
- Random rotation - this state will randomly pick a new path.

There is also a third state that is the normal driving mode:
- MovingForward - driving forward at a constant speed

The FindPath state is a work in progress and at this early stage is not proving that useful in helping the robot escape the maze, although the robot will escape eventually when using either method. In FindPath the robot first does a 360 degree to scan the environment. It will then pick the longest path available to it based on this scan, ignoring paths that were behind it (relative to the orientation of the robot when it arrived at FindPath). Once a new path is found the robot will rotate to this new orientation and trigger a state change to MovingForward to start driving.

The robot has 5 distance sensors that it can use in planning the next path, although at this time FindPath is only using one, sensor 0, the sensor gathering data directly in front of the robot. If the Robot it pointing North, the 5 sensors are oriented like:

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

Figuring out the order of the ID's of the sensors took some time. My solution was to center the robot in a basic rectangle to find sensors with matching values (sensors 2 and 1 would have the same values, 4 and 3 would have the same values, but I didn't know the ID's at this point), then offset the position of the robot to find the changes between the pairs to figure out which was which. I also oriented the robot at various angles and measured the distance to further confirm I had the right ID's. This was prep work to be able to use the sensors to do things in the FindPath state later, but as time started to run short I was only able to add some basic things with sensor 0. Another challenge was getting data between states, which I was trying to do for the button. My plan was to do it with channels but couldn't get it working in time so had to pivot and use methods in the states instead, by calling them from the AgentInterface when the button is pressed.
I have several more plans for the FindPath state and would like to build in more logic, such as finding intersections when sensors 4 or 3 show large jumps in value, and by using sensors to better center the robot when traveling down halls. At this time FindPath is prone to getting stuck at dead ends, although it will get out of them after a few passes. The plan was to identify dead ends and avoid them.

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