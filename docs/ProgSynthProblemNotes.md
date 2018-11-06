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
- [ ] LastIndexOfZero (vector, numbers)
- [ ] CountOdds (vector, numbers)
- [ ] VectorsSummed (vector, numbers)
- [ ] VectorAverage
- [ ] SumOfSquares
- [x] Median (numbers) 
- [x] Smallest (numbers)

## Solution evolution at a glance

Things I've seen solutions evolve for:

- NumberIO
- CompareStringLengths
- Smallest
- Median
- Small Or Large (9k updates, >=64 w/128eval)

Things I haven't seen yet:

- For Loop index
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