# Hand-coded solutions for programming synthesis benchmark problems

<!-- TOC -->

- [Problem - Number IO](#problem---number-io)

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