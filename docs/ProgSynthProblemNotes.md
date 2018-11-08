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