# This is my particle engine
Made using C++. Spatial hashing used to improve performance by 49x. Substepping used for improved physics. Fabrics and Fluid soon to be added.
Here are its current best features:
 - Verlet Integration
 - Stable Stacking
 - Spatial Hashing
 - Spawn balls by clicking
 - Air resistance
 - Inelastic Wall collisions


## Here is my first simulation, V1:
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


## Here is my second simulation, V2:
![Gif Not loaded](media/Animation3.gif)

### Performance with -02
- 60 fps at 650 balls
- 30 fps at 1450 balls
 
### Details
- Same as v1
- Now has damping to stop ball piles conserving energy causing waving
- Now uses substeps (4 substeps per frame in the gif) to create more accurate physics and stop trampolining when high velocity balls hit piles


## Here is my third, V3:
![Gif Not loaded](media/Animation4.gif)

### Performance with -02
- 60 fps at 1450 balls
- 30 fps at 4440 balls
 
### Details
- Same as v1
- ball collision overhauled to work on only 2 particles rather than all
- Spatial hashing added to only compute collisions between particles in 9 closest possible places (massive performance boost)

## Benchmarks

### Setup
- **Hardware:** Intell N100, 8GB LPDDR5, couple year old £300 school laptop, only VS Code open, not plugged in.
- **Build:** `-O2`
- **Simulation:** Verlet integration, 2 substeps per frame, gravity on, ball radius 0.1 units, damping.
- **Sizing:** 800 × 800 px, then later 8000 x 8000px. Balls are 2 pixels in diameter.
- **Timing:** `std::chrono::steady_clock` around the physics step only (rendering removed for benchmarking). Timing averaged using 100 samples over 10 seconds
- **Spawning:** balls spawned by code, not longer by clicking, for the sake of benchmarking aswell.

### Brute force vs spatial grid (80 × 80 world)

| Balls | Brute force (ms/frame) | Grid (ms/frame) | Speedup |
|------:|-----------------------:|----------------:|--------:|
| 1,000 | 11.88 | 1.92 | 6× |
| 2,500 | 67.69 | 3.30 | 21× |
| 5,000 | 286.09 | 5.82 | 49× |

Brute force scales roughly as O(n²), which I predicted: 2.5× more balls cost 5.7× more time (O(n²) would be 6.25×), and 2× more balls cost 4.2× more time (O(n²) would be 4×). Above 5,000 balls brute force fell below 4 FPS, so it was stopped.

### Grid scaling (80 × 80 world)

| Balls | ms/frame | µs per ball | Area covered |
|------:|---------:|------------:|-------------:|
| 1,000 | 1.92 | 1.92 | 0.5% |
| 2,500 | 3.30 | 1.32 | 1.2% |
| 5,000 | 5.82 | 1.16 | 2.5% |
| 10,000 | 10.78 | **1.08** | 4.9% |
| 25,000 | 31.67 | 1.27 | 12% |
| 50,000 | 93.10 | 1.86 | 25% |
| 75,000 | 183.71 | 2.45 | 37% |
| 100,000 | 308.52 | 3.09 | 49% |

- **Up to 10k:** cost per ball *falls*, because the overhead of clearing the grid is spread between more balls.
- **Above 10k:** cost per ball *rises*, so scaling is worse than linear. I guessed that the increased density of balls meant the benefit of my spatial hashing was smaller because the cells would be looped over more. I tested this by increaseing the world size to effectly decrease density.

### Experiment: 800 × 800 unit world (8000 × 8000 px)

| Balls | ms/frame | µs per ball (excluding ~105 ms overhead) | Same N in 80 × 80 world |
|------:|---------:|-----------------------------------------:|------------------------:|
| 1,000 | 105.56 | Too Noisy | 1.92 |
| 2,500 | 103.31 | Too Noisy | 1.32 |
| 5,000 | 105.97 | Too Noisy | 1.16 |
| 10,000 | 117.41 | 1.25 | 1.08 |
| 25,000 | 136.05 | 1.24 | 1.27 |
| 50,000 | 167.44 | 1.25 | 1.86 |
| 75,000 | 209.18 | 1.39 | 2.45 |

- Also tested 8000x8000 using brute force. its results effectively stayed the same as when the world was 800x800px. I pressume this is because brute forcing has no grid so increasing world size does not increase computation.

**Findings**
1. **A uniform grid costs O(N + cells), not O(N).** The large world has 4,000 × 4,000 = 16M cells (which is already 384 MB of empty `std::vector`s). Clearing them every substep costs about **~104 ms per frame**. One of my runs crashed saying `std::bad_alloc` (out of memory) when i attempted to run a 80000x80000 world.
2. **Density caused the slowdown, not particle count.** In the large world the pile is shallow,and the cost per ball stays roughly flat (~1.3 µs) from 10k to 75k, so the grid itself scales linearly. In the small world the pile becomes deep and compressed, so theres more local cells containing balls and more balls in each cell, making the spatial hashing slow down from how many loops it has to do.
3. **Brute force was unaffected by world size** (it has no grid), which confirms the overhead comes from the grid.

# Thoughts
 - Spatial hashing especially useful for larger numbers of balls. Collision checks stay linear now (O(n) probably about 9n), whereas before they were quadratic as every ball checked collisions with every other ball, but now its around 8 at most.
 - Spatial hashing was by far the hardest implementation; thinking about, and coding to find 9 closest locations within the spatial Grid was very complicated.
 - I expected its benefit to be larger. Especially the ball amount of 30 fps, considering that its benefit over naive collisions is larger for higher numbers of balls O(n^2) compared to O(n) with a chunky bit of overhead.
 - In all simulations so far, the max balls at 30 fps is usually about 3x that at 60fps. The largest being V3 at around 3.06x, the lowest being V2 at 2.23x. I infer this is from the processing complexity of doing spatial hashing over naive collisions.
 - Fps in v2 and v3 would be much higher, most likely 4x higher if it wasnt at the compromise of substeps, which makes physics more stable and realistic, but mean almost 4x the computation and spatial hashing, ball collisions and verlet integration are repeated 4x every frame.
 - My favourite part of this so far has been observing the visual difference of using smarter logic like verlet over euler and spatial hashing. 
 - The most satisfying part was when i identified this problem: Fast particles hitting a bed of other particles would trampoline back up, with almost all momentum conserved, whereas slow particles settled realistically. I realised this must be because the faster balls travel so quick that they travel very far into the pile in a single frame because of their velocity, and since rebound is proportional to ball overlap, it caused the balls to bounce back further than they should. This is how i learnt about the benefit of substepping.

# What I want to add next
 - I could effectively halve processing time for ball collisions. if ball A checks collisions with ball B, ball B also checks collisions with ball A, when it doesnt need to because only 1 needs to be computed for both bounces to occur
 - For spatial hashing i could clear only the cells used this step, instead of every cell instead of every cell to reduce the overhead.
 - Big visual changes like friction between balls, ropes, fabric, dynamic gravity and more other optimisation if possible.
 - Make this runnable on other devices that arent on windows x86 like me, apparently Cmake is a good way to do so.
 - Variable clean up to make sure all calculations are done the minimum amount of times possible
