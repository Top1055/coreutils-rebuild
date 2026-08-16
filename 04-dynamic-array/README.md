## 04 — Dynamic memory

Implemented a vector type, similar to std:vec
vec grows in increments of doubling.
Supports push, pop, init, indexing and free by using heap allocation

**Learned:** malloc, and realloc, their return values and how to handle type buffers

**Known gaps:** no shrinking of vec on losing values. Would implement a shrink on len < cap/3
