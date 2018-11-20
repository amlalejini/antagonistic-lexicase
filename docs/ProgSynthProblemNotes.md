# Problem Notes

## Hypotheses

- Coevolution less useful for problems like Collatz

## Problem Implementation (round 1) priority list

- [x] NumberIO (numbers)
  - [x] Gradient? Implemented
- [x] Small or Large (numbers)
  - [x] Gradient? None
- [x] ForLoopIndex (numbers)
  - [x] Gradient? Implemented
- [x] CompareStringLengths (strings, numbers)
  - [x] Gradient? None
- [x] Collatz Numbers (numbers)
  - [x] Gradient? Implemented
- [x] StringLengthBackwards (vector, strings, numbers)
  - [x] Gradient? Implemented
- [x] LastIndexOfZero (vector, numbers)
  - [x] Gradient? Integer error, implemented
- [x] CountOdds (vector, numbers)
  - [x] Gradient? - Integer error, implemented
- [x] MirrorImage (vector, bool)
  - [x] Gradient? - No
- [x] VectorsSummed (vector, numbers)
  - [x] Gradient? - float error, implemented
- [x] VectorAverage (vector, number)
  - [x] Gradient? - Float error
- [x] SumOfSquares (number, number)
  - [x] Gradient? - Integer error
- [x] Median (numbers) 
  - [x] Gradient? No
- [x] Smallest (numbers)
  - [x] Gradient? No

## Parameter Selection

- Treatments
  - Co-evolve tests
    - Cohort lexicase
  - Static tests
    - Cohort lexicase w/bolstered tests (STATIC-GEN) -- NOTE: some problems can't
      guarantee unique training/testing examples
    - Full lexicase w/training examples as test population
  - Random tests
    - Cohort lexicase
  - Drifting tests
    - Cohort lexicase (programs, drift for tests)

- Population size
  - Original: 1000
  - Here: 512 (speed considerations)
  - Cohort size: 32 (512/32 = 16 cohorts), 16(?)

- Generations:
  - Original: problem-specific
  - Here: Set goal of 50k, see how far each problem gets.
 
- Problem-specific
  - NumberIO -- Yes
    - Original: {Max size: 200, Eval Time: 200, Generations: 200}
    - Here: {Max size: 32, Eval Time: 32}
  - SmallOrLarge -- Yes
    - Original: {Max size: 200, Eval Time: 300, Generations: 300}
    - Here: {Max size: 64, Eval Time: 64}
  - ForLoopIndex -- Yes
    - Original: {Max size: 300, Eval time: 600, Generations: 300}
    - Here: {Max size: 128, Eval time: 256}
  - CompareStringLengths -- Yes
    - Original: {Max size: 400, Eval time: 600, Generations: 300}
    - Here: {Max size: 64, Eval time: 64}
  - Collatz -- No
    - Original: {Max size: 600, Eval time: 15000, Generations: 300}
  - StringLengthBackwards -- Yes
    - Original: {Max size: 300, Eval time: 600, Generations: 300}
    - Here: {Max size: 128, Eval time: 512}
  - LastIndexOfZero -- Yes
    - Original: {Max size: 300, Eval time: 600, Generations: 300}
    - Here: {Max size: 128, Eval time: 512}
  - CountOdds -- No
    - Original: {Max size: 500, Eval time: 1500, Generations: 300}
    - Here: {Max size: 128, Eval time: 512}
  - MirrorImage -- Yes
    - Original: {Max size: 300, Eval time: 600, Generations: 300}
    - Here: {Max size: 128, Eval time: 512}
  - VectorsSummed -- Yes
    - Original: {Max size: 500, Eval time: 1500, Generations: 300}
    - Here: {Max size: 128, Eval time: 512||1024}
  - VectorAverage -- No
    - Original: {Max size: 400, Eval time: 800, Generations: 300}
    - Here: {Max size: 128, Eval time: 512||1024}
  - SumOfSquares -- Yes
    - Original: {Max size: 400, Eval time: 4000, Generations: 300}
    - Here: {Max size: 128, Eval time: 512}
  - Median -- Yes
    - Original: {Max size: 200, Eval time: 200, Generations: 200}
    - Here: {Max size: 64, Eval time: 64}
  - Smallest -- Yes
    - Original: {Max size: 200, Eval time: 200, Generations: 200}
    - Here: {Max size: 64, Eval time: 64}

## Solution evolution at a glance

Things I've seen solutions evolve for:

- NumberIO
- CompareStringLengths
- Smallest
- Median
- Small Or Large (9k updates, >=64 w/128eval)
- StringLengthBackwards
- MirrorImage

Things I haven't seen yet:

- ForLoopIndex
- LastIndexOfZero
- CountOdds
- VectorsSummed
  - I imagine the gradient is really important for this one.
- SumOfSquares
- VectorAverage
- CollatzNumbers
- VectorAverage

## Problem - For Loop Index

Parameter considerations

- Be sure to give programs enough evaluation time to make it through at least 20
  20 iterations. (maybe 256 cycles?)

## Problem - CollatzNumbers

Parameter considerations

- Programs need at least 200 iterations through a 'tight' loop to calculate
  collatz number. Probably want to evolve w/4096 evaluation time (PushGP given
  15K => maybe we want to do that as well).