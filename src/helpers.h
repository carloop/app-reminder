#pragma once

/* Sizeof for a struct member */
#define member_sizeof(T,F) sizeof(((T *)0)->F)

/* Offset and size of a struct member as 2 arguments */
#define offsetof_and_sizeof(T, F) offsetof(T, F), member_sizeof(T, F)

/* Number of elements in static array */
#define countof(arr) sizeof(arr) / sizeof(arr[0])


