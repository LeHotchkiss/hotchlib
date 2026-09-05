Personal library of the almighty Hotchkiss

Provides basic RAII-aware container classes.
(Because i don`t like using STL)

+ Has no dependencies, except for C/C++ runtime libraries
+ Does not use exceptions at all
+ Supports allocator replacement, defaults to C`s malloc
+ Minumum bloat

All containers` access works like this:
The indexing operator[] is a direct access without any sanity checks.
The 'At' method does the same, but is 'safe' in a way that it won`t segfault
if you do a mistake, at the cost of performing some checks.
