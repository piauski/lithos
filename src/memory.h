#ifndef MEMORY_H_
#define MEMORY_H_

#include "core.h"

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ALIGN_UP_POW2(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

#define ARENA_BASE_POS (sizeof(Arena))
#define ARENA_ALIGN (sizeof(void *))

typedef struct {
    u64 capacity;
    u64 pos;
} Arena;

Arena *arena_create(u64 capacity);
void arena_destroy(Arena *arena);
void *arena_push(Arena *arena, u64 size, b32 non_zero);
void arena_pop(Arena *arena, u64 size);
void arena_pop_to(Arena *arena, u64 pos);
void arena_clear(Arena *arena);

#define PUSH_STRUCT(arena, T) (T*)arena_push((arena), sizeof(T), false)
#define PUSH_STRUCT_NZ(arena, T) (T*)arena_push((arena), sizeof(T), true)
#define PUSH_ARRAY(arena, T, n) (T*)arena_push((arena), sizeof(T) * (n), false)
#define PUSH_ARRAY_NZ(arena, T, n) (T*)arena_push((arena), sizeof(T) * (n), true)

#endif // !MEMORY_H_
