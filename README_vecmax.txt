

Logic:
  1. Group all numbers into subgroup which has 256 member.
  2. Find maximum number of a subgroup into tmpv.
  3. If tmpv is full, then, find maximum element in tmpv.
  3. Repeat first 3 steps until all elements are processed.
  3. Find the maximum element in tmpv.
  4. The first element in tmpv is the answer.

GPU shared memory
Declare shared memory using __shared__, which is allocated per block
Within a block, threads share data via shared memory.
The function uses shared memory to compare numbers.

