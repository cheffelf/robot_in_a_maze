ROBOT_IN_A_MAZE: A maze runner simulation
===

Goal
===

This is a basic maze runner simulator based on the elma event loop and process manager:
https://github.com/klavinslab/elma

and the enviro simulator:
https://github.com/klavinslab/enviro

The simulation is of a static maze with one exit. A Robot is placed in the opposite corner and has two methods available to it to navigate the maze. The robot will use one of these methods when it gets near an obstacle:
    * Test algorithm - the default
    * Random rotation

The user can click a button in the upper right hand corner of the screen to switch the method the robot will use on the fly.

The robot is using a finite state machine with three different modes:
    * FindPath - it will pick the next path with the Test algorithm
    * Rotating - it will randomly pick a new path
    * MovingForward - driving forward at a constant speed

The Test Algorithm is a work in progress but does a few functions now. It is not really that useful in helping the robot escape the maze, although the robot will escape eventually. In this state the robot will first stop and do a 360 degree scan of the environment, and pick the longest path available to it based on this scan, assuming that path is not behind it. Behind it means relative to the orientation the robot was in when it arrived at this state. Once a new path is found the robot will rotate to this new orientation and trigger a state change to MovingForward.

The robot has 5 distance sensors that it can use in planning the next path, although at this time the Test algorithm is only using one, the sensor gathering data directly in front of the robot.

Challenges
==


How to install
==

Dockerhub Images
===

To start a docker image that has enviro and elma built in, do
```bash
docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.6 bash
```
in docker image:

```bash
esm start
```