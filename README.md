# This is my particle engine
Made using C++, I'm using this as a deep dive into simulation.
I aim to build on this further, eventually hoping to implement fluid simulation.

## Here is my first simulation:
![Gif Not loaded](media/Animation.gif)

### Performance with -0
- 60 fps at 500 balls
- 30 fps at 900 balls

### Performance with -02
- 60 fps at 1100 balls
- 30 fps at 3000 balls

 
### Details
- Uses Verlet Integration
- Naive collisions, tested for every ball every second (n^2)
- Tested on a £300 school laptop with a browser and IDE also open.
