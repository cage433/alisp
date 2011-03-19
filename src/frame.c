#include "frame.h"
#include "utils.h"

Hash *create_empty_frame(){
    return hash_create(string_hash_fn, strings_equal);
}

void frame_add(Hash *frame, char *key, boxed_value *value){
    if (hash_contains(frame, key)){
        boxed_value *current_value = hash_value(frame, key);
        if (value == current_value)
            return;
        else
            dec_ref_count(current_value);
    }
    inc_ref_count(value);
    hash_add(frame, strdup(key), value);
}

void free_key_value_pair(KeyValuePair *kv){
    my_free(kv->key);
    dec_ref_count(kv->value);
    my_free(kv);
}
void frame_remove(Hash *frame, char *key){
    KeyValuePair *kv = hash_remove(frame, key);
    free_key_value_pair(kv);
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

Hash *copy_frame(Hash *frame){
    Hash *copy = create_empty_frame();

    void add_key_value(void *key){
        void *value = hash_value(frame, key);
        frame_add(copy, key, value);
    }
    List *keys = hash_keys(frame);
    list_for_each(keys, add_key_value);
    free_list(keys, nop_free_fn);
    return copy;
}

void free_frame(Hash *frame){
    free_hash(frame, free_key_value_pair);
}


