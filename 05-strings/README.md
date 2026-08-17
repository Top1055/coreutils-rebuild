# 05 - Strings

Reimplementations of `strlen`, `strcpy`, `strdup`, `strcmp` and `strcat`
without `<string.h>`, plus a `split`/`split_free` pair that breaks a string
into a NULL-terminated array of tokens (the same shape as `argv`).

## Learned

**A C string is a run of bytes ending in `\0`, this changes how we operate on them,
and also explains the benefit of programming langauges turning these into objects
with functions, macros, and storing len as a field. `strlen` is O(n) every time you call it,
which makes naive `strcat` loops quadratic: each call rescans from the start to
find the end. `std::string` and Rust's `String` store the length as a member
precisely to kill this whole class of problem.

**`strcpy` doesn't copy the terminator for free.** Copying `strlen(src)`
bytes stops one short. This showed up as a valgrind
"conditional jump depends on uninitialised value" inside `printf`.

**`stpcpy` vs `strcpy`**: identical work, different return. `strcpy` gives
back the start, `stpcpy` gives back the end.

**`strcmp` logic is very interesting. Walk both strings together,
return the difference at the first byte that differs. This gives more information to the caller
and the size difference can be inferred, and can be used for a sorting algorithm.
Comparison must be done on `unsigned char` for accurate differences.

## Ownership

Three patterns turn up repeatedly in C:

1. Caller allocates, callee fills - `strcat`, `read`
2. Callee allocates, caller frees - `strdup`, `malloc`, `split`
3. Callee may reallocate, caller must reassign - `realloc`

my_split uses pattern 2, and in order to handle an error gracefully,
made use of a goto label to free up the resources before returning NULL.
This will later be replaced with exceptions in C++
