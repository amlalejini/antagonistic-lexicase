# Problem Notes

## Hypotheses

- Coevolution less useful for problems like Collatz

## Problem Implementation (round 1) priority list

- [x] NumberIO (numbers)
- [x] Small or Large (numbers)
- [x] ForLoopIndex (numbers)
- [x] CompareStringLengths (strings, numbers)
- [x] Collatz Numbers (numbers)
- [x] StringLengthBackwards (vector, strings, numbers)
- [x] LastIndexOfZero (vector, numbers)
- [x] CountOdds (vector, numbers)
- [x] MirrorImage (vector, bool)
- [x] VectorsSummed (vector, numbers)
- [ ] VectorAverage (vector, number)
- [x] SumOfSquares (number, number)
- [x] Median (numbers) 
- [x] Smallest (numbers)

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
- CollatzNumbers

## Problem - For Loop Index

Parameter considerations

- Be sure to give programs enough evaluation time to make it through at least 20
  20 iterations. (maybe 256 cycles?)

## Problem - CollatzNumbers

Parameter considerations

- Programs need at least 200 iterations through a 'tight' loop to calculate
  collatz number. Probably want to evolve w/4096 evaluation time (PushGP given
  15K => maybe we want to do that as well).