# 06 — Hash map

String keys, `int` values, separate chaining with FNV-1a hashing. Grows
automatically at 75% load. Tested with 10,000 inserts, 5,000 updates and
3,333 deletes — every value verified, valgrind clean.

## Learned

**A pointer is just a number, and copying it copies the number.**
`struct entry *e = m->entries[i]; e = ptr;` changes the local, not the
bucket — exactly like `int x = arr[3]; x = 99;` leaves `arr[3]` alone.
The rule that finally stuck: *to modify something, you need a route to where
it actually lives.* `m->entries[i]` and `prev->next` are routes. A local
copy is not.

**`e = x` vs `*e = x` are different operations.** The first changes the
variable; the second writes to the memory it points at. When `e` is NULL
there is no memory, so `*e = x` segfaults — but you can always put a new
address *into a slot*.

**Stack memory dies at the closing brace.** Storing `&new_e` (a local) in the
map produced a stack-use-after-return: the map pointed into a destroyed
frame. Anything that must outlive its function belongs on the heap.

**Prepending turns insert into two lines.** `new->next = bucket; bucket = new;`
works identically for an empty bucket and a long chain, with no special case
and O(1) cost. Chains become stacks; order doesn't matter since lookup is by
key.

**`malloc` and `realloc` don't zero.** NULL is the "empty bucket" marker, so
the initial array uses `calloc` — and after `reallocarray` doubles the array,
the new half is garbage until zeroed by hand. Missing that let three grows
"work" by luck and crashed on the fourth. Passing tests can hide a bug that
was there all along.

**You can't read a node after freeing it.** List teardown must save `next`
first. A `map_free` that updated the bucket but not the walking pointer
double-freed — invisible with one entry per bucket, exposed immediately by
forcing everything into one bucket (`CAPACITY 1`). Forcing collisions is the
test that matters.

**`sizeof(*p)` over `sizeof(type)`.** It stays correct if the type changes,
and `sizeof` never evaluates its operand, so it's safe on NULL or
uninitialised pointers.

**`[]` means `*` only in function parameters.** Inside a struct,
`struct entry *entries[]` is a flexible array member, not a pointer.

**A leak report shows where memory was allocated, not where the bug is.**
Read it as "who owns this?" — the fix is usually in the teardown.

## Design notes

**Why resizing requires rehashing:** the bucket index is `hash % capacity`,
so changing capacity changes where every key lives. When capacity doubles,
an entry in bucket `i` can only land in `i` or `i + old_capacity` — which is
why the in-place rehash only needs to walk the old half, and moved entries
are never visited twice.

**Worst case:** O(n) lookup, when every key hashes to the same bucket and the
map degenerates into a linked list — whether by bad luck, a bad hash, or an
attacker choosing keys deliberately.

**Why 0.75:** a trade between memory and chain length. Much higher and chains
grow long; much lower and most buckets sit empty.

**Chaining vs open addressing:** chaining stores collisions in per-bucket
lists (simple deletes, tolerates high load). Open addressing stores
everything in the array itself and probes for the next free slot (better
cache locality, but deletes need tombstones and performance falls off
sharply near full).

**Grow before search:** `map_set` checks load before searching, so an update
at exactly the threshold triggers a grow. Deliberate — that map would grow
on its next insert anyway.

**Growth failure is non-fatal:** the map still works, just with longer
chains. `map_set` warns on stderr and carries on.

## Known gaps

- No shrinking. A `map_adjust(m, size)` that allocates a fresh array and
  rehashes into it would cover both directions
- `map_move` re-searches the chain by key when the caller already holds the
  entry
- `map_free` uses a two-branch loop where a single save-next/free/advance
  loop would do, and doesn't null `entries` afterwards
- Word-frequency capstone not built
