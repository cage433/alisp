#ifndef ALISP_FRAME
#define ALISP_FRAME

#include "hash.h"
#include "boxed_value.h"
#include "list.h"

Hash *create_empty_frame();
void frame_add(Hash *frame, char *key, boxed_value *value);
Hash *frame_create(List *args, List *values);

Hash *copy_frame(Hash *frame);
void free_frame(Hash *frame);
#endif
