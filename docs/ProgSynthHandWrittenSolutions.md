# Hand-coded solutions for programming synthesis benchmark problems

<!-- TOC -->

- [Problem - Number IO](#problem---number-io)
- [Problem - Small or Large](#problem---small-or-large)
- [Problem - ForLoopIndex](#problem---forloopindex)
- [Problem - CompareStringLengths](#problem---comparestringlengths)

<!-- /TOC -->

## Problem - Number IO

```{C++}
  emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();
  hardware_t::Program sol(inst_lib);
  sol.PushInst("LoadInt", {matrix[0], matrix[0], matrix[0]});
  sol.PushInst("LoadDouble", {matrix[1], matrix[1], matrix[1]});
  sol.PushInst("Add", {matrix[0], matrix[1], matrix[2]});
  sol.PushInst("SubmitNum", {matrix[2], matrix[2], matrix[2]});
  sol.PushInst("Return", {matrix[0], matrix[0], matrix[0]});
  prog_world->Inject(sol, PROG_POP_SIZE);
```

## Problem - Small or Large

```{C++}
  emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();
  hardware_t::Program sol(inst_lib);
  // Create thresholds to compare n to.
  sol.PushInst("Set-10", {matrix[0], matrix[4], matrix[4]});
  sol.PushInst("Set-2",  {matrix[1], matrix[4], matrix[4]});
  sol.PushInst("Mult",   {matrix[0], matrix[0], matrix[2]});
  sol.PushInst("Mult",   {matrix[0], matrix[2], matrix[0]});
  sol.PushInst("Mult",   {matrix[0], matrix[1], matrix[1]});
  // Load input
  sol.PushInst("LoadInt",     {matrix[2], matrix[4], matrix[4]});
  // Check if n < 1000
  sol.PushInst("TestNumLess", {matrix[2], matrix[0], matrix[3]});
  sol.PushInst("If",          {matrix[3], matrix[4], matrix[4]});
  sol.PushInst("SubmitSmall", {matrix[4], matrix[4], matrix[4]});
  sol.PushInst("Return",      {matrix[4], matrix[4], matrix[4]});
  sol.PushInst("Close",       {matrix[4], matrix[4], matrix[4]});
  // Check if n < 2000
  sol.PushInst("TestNumLess", {matrix[2], matrix[1], matrix[3]});
  sol.PushInst("If",          {matrix[3], matrix[4], matrix[4]});
  sol.PushInst("SubmitNone",  {matrix[4], matrix[4], matrix[4]});
  sol.PushInst("Return",      {matrix[4], matrix[4], matrix[4]});
  sol.PushInst("Close",       {matrix[4], matrix[4], matrix[4]});
  // n must be >= 2000
  sol.PushInst("SubmitLarge", {matrix[4], matrix[4], matrix[4]});
  sol.PushInst("Return",      {matrix[4], matrix[4], matrix[4]});
  prog_world->Inject(sol, PROG_POP_SIZE);
```

## Problem - ForLoopIndex

```{C++}
  emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();
  hardware_t::Program sol(inst_lib);

  sol.PushInst("CopyMem",     {matrix[0], matrix[4], matrix[7]});
  sol.PushInst("Inc",         {matrix[5], matrix[7], matrix[7]});
  sol.PushInst("While",       {matrix[5], matrix[7], matrix[7]});
  sol.PushInst("SubmitNum",   {matrix[4], matrix[7], matrix[7]});
  sol.PushInst("Add",         {matrix[4], matrix[2], matrix[4]});
  sol.PushInst("TestNumLess", {matrix[4], matrix[1], matrix[5]});
  sol.PushInst("Close",       {matrix[7], matrix[7], matrix[7]});
  sol.PushInst("Return",      {matrix[7], matrix[7], matrix[7]});

  prog_world->Inject(sol, PROG_POP_SIZE);
```

## Problem - CompareStringLengths

```{C++}
  emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();
  hardware_t::Program sol(inst_lib);

  sol.PushInst("StrLength",   {matrix[0], matrix[0], matrix[4]});
  sol.PushInst("StrLength",   {matrix[1], matrix[1], matrix[4]});
  sol.PushInst("StrLength",   {matrix[2], matrix[2], matrix[4]});
  sol.PushInst("TestNumLess", {matrix[0], matrix[1], matrix[3]});
  sol.PushInst("If",          {matrix[3], matrix[4], matrix[4]});
  sol.PushInst("TestNumLess", {matrix[1], matrix[2], matrix[3]});
  sol.PushInst("If",          {matrix[3], matrix[4], matrix[4]});
  sol.PushInst("SubmitVal",   {matrix[3], matrix[4], matrix[4]});
  sol.PushInst("Close",       {matrix[4], matrix[4], matrix[4]});
  sol.PushInst("Close",       {matrix[4], matrix[4], matrix[4]});
  sol.PushInst("SubmitVal",   {matrix[3], matrix[4], matrix[4]});
  
  prog_world->Inject(sol, PROG_POP_SIZE);
```