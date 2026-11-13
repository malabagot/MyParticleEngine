# This is my particle engine
Made using C++, I'm using this as a deep dive into simulation.
I aim to build on this further, eventually hoping to implement fluid simulation.
Here are its current best features:
 - Verlet Integration
 - Stable Stacking
 - Spatial Hashing
 - .h + .cpp file structure
 - fps checker
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
- **Hardware:** [CPU model], [RAM] — a low-spec laptop, plugged in, only VS Code open
- **Build:** `-O2`
- **Simulation:** Verlet integration, 2 substeps per frame, gravity on, ball radius 0.1 units
- **World:** 80 × 80 units (800 × 800 px) unless stated
- **Timing:** `std::chrono::steady_clock` around the physics step only (no rendering), averaged over [X] frames after [Y] warm-up frames
- **Spawning:** balls spawned in code in a fixed layout, so every run starts from the same state
- Brute force figures are the median of 4 runs; grid figures are the mean of 2 runs. Run-to-run variation was about ±5–10%.

### Brute force vs spatial grid

| Balls | Brute force (ms/frame) | Grid (ms/frame) | Speedup |
|------:|-----------------------:|----------------:|--------:|
| 1,000 | 11.4 | 1.88 | **6×** |
| 2,500 | 65.1 | 3.47 | **19×** |
| 5,000 | 258.2 | 5.95 | **43×** |

Brute force scales as O(n²), as predicted: 2.5× more balls cost about 6.4× more time (theory: 6.25×), and 2× more balls cost about 3.9× more time (theory: 4×). Above 5,000 balls brute force fell below 4 FPS, so it was stopped.

### Grid scaling (80 × 80 world)

| Balls | ms/frame | µs per ball | Area covered |
|------:|---------:|------------:|-------------:|
| 1,000 | 1.9 | 1.88 | 0.5% |
| 5,000 | 5.9 | 1.19 | 2.5% |
| 10,000 | 10.6 | **1.06** | 5% |
| 25,000 | 31.6 | 1.26 | 12% |
| 50,000 | 92.1 | 1.84 | 25% |
| 100,000 | 301.7 | 3.02 | 49% |

- **Up to 10k:** cost per ball *falls*, because fixed overhead (clearing the grid, loop setup) is spread over more balls.
- **Above 10k:** cost per ball *rises*, so scaling is worse than linear. To find out why, I ran the same test in a world 100× larger in area.

### Experiment: 800 × 800 unit world (8000 × 8000 px)

| Balls | ms/frame | µs per ball (excluding ~104 ms overhead) | Same N in 80 × 80 world |
|------:|---------:|-----------------------------------------:|------------------------:|
| 1,000 | 105.6 | — (noise) | 1.88 |
| 10,000 | 117.4 | 1.34 | 1.06 |
| 25,000 | 136.1 | 1.28 | 1.26 |
| 50,000 | 167.4 | 1.27 | 1.84 |
| 75,000 | 209.2 | 1.40 | 2.39 |

**Findings**
1. **A uniform grid costs O(N + cells), not O(N).** The large world has 4,000 × 4,000 = 16M cells (~384 MB of empty `std::vector`s). Clearing them every substep costs a fixed **~104 ms per frame**, even with only 1,000 balls. One run crashed with `std::bad_alloc` (out of memory).
2. **Density caused the slowdown, not particle count.** In the large world the pile is shallow, and cost per ball stays roughly flat (~1.3 µs) from 10k to 75k, so the grid itself scales linearly. In the small world the pile is deep and compressed, so each cell holds more balls and each ball checks more neighbours.
3. **Brute force was unaffected by world size** (it has no grid), which confirms the overhead comes from the grid.

### What I'd do next for benchmarking
- Clear only the cells used this step, instead of every cell.
- Replace the uniform grid with a true fixed-size spatial hash, so memory and clearing cost depend on N, not on world size.
- Handle each colliding pair once instead of twice.

# Thoughts
 - Spatial hashing especially useful for larger numbers of balls. Collision checks stay linear now (O(n) probably about 9n), whereas before they were quadratic as every ball checked collisions with every other ball, but now its around 8 at most.
 - Spatial hashing was by far the hardest implementation; thinking about, and coding to find 9 closest locations within the spatial Grid was very complicated.
 - I expected its benefit to be larger. Especially the ball amount of 30 fps, considering that its benefit over naive collisions is larger for higher numbers of balls O(n^2) compared to O(n) with a chunky bit of overhead.
 - In all simulations so far, the max balls at 30 fps is usually about 3x that at 60fps. The largest being V3 at around 3.06x, the lowest being V2 at 2.23x. I infer this is from the processing complexity of doing spatial hashing over naive collisions.
 - Fps in v2 and v3 would be much higher, most likely 4x higher if it wasnt at the compromise of substeps, which makes physics more stable and realistic, but mean almost 4x the computation and spatial hashing, ball collisions and verlet integration are repeated 4x every frame.
 - My favourite part of this so far has been observing the visual difference of using smarter logic like verlet over euler and spatial hashing. 
 - The most satisfying part was when i identified this problem: Fast particles hitting a bed of other particles would trampoline back up, with almost all momentum conserved, whereas slow particles settled realistically. I realised this must be because the faster balls travel so quick that they travel very far into the pile in a single frame because of their velocity, and since rebound is proportional to ball overlap, it caused the balls to bounce back further than they should. This is how i learnt about the benefit of substepping.

# What I want to add next for functionality
 - I could effectively halve processing time for ball collisions. if ball A checks collisions with ball B, ball B also checks collisions with ball A, when it doesnt need to because only 1 needs to be computed for both bounces to occur
 - I add a benchmarking tool that automatically adds 1k, 2.5k, 5k and 10k balls for easy benchmarking that also keeps substeps consistent.
 - Big visual changes like friction between balls, ropes, fabric, dynamic gravity and more other optimisation if possible.
 - Make this runnable on other devices that arent on windows x86 like me, apparently Cmake is a good way to do so.
 - Variable clean up to make sure all calculations are done the minimum amount of times possible

