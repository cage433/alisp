#include "frame.h"
#include "utils.h"

Hash *create_empty_frame(){
    return hash_create(string_hash_fn, strings_equal);
}

void frame_add(Hash *frame, char *key, boxed_value *value){
    inc_ref_count(value);
    hash_add(frame, strdup(key), value);
}

void frame_remove(Hash *frame, char *key){
    KeyValuePair *kv = hash_remove(frame, key);
    free(kv->key);
    dec_ref_count(kv->value);
    free(kv);
}

Hash *frame_create(List *args, List *values){
    Hash *frame = create_empty_frame();
    while (args != NULL && values != NULL){
        if (hash_contains(frame, args->car))
            frame_remove(frame, args->car);

        frame_add(frame, args->car, values->car);
        args = args->cdr;
        values = values->cdr;
    }
    die_unless(args == NULL && values == NULL, "Arg names and values have different lengths");
    return frame;
}

