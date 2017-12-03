/*
 *
 * Copyright (C) 2011, 2015 Cisco Systems, Inc.
 * Copyright (C) 2015 CBA research group, Technical University of Catalonia.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "timers_utils.h"


oor_timer_t *
oor_timer_without_nonce_new(timer_type type, void *owner, oor_timer_callback_t cb_fn,
        void *timer_arg,oor_timer_del_cb_arg_fn free_arg_fn)
{
    oor_timer_t *timer;

    timer = oor_timer_create(type);
    oor_timer_init(timer,owner,cb_fn,timer_arg,free_arg_fn,NULL);

    return (timer);
}

oor_timer_t *
oor_timer_with_nonce_new(timer_type type, void *owner, oor_timer_callback_t cb_fn,
        void *timer_arg,oor_timer_del_cb_arg_fn free_arg_fn)
{
    oor_timer_t *timer;
    nonces_list_t *nonces_lst;

    timer = oor_timer_create(type);
    nonces_lst = nonces_list_new_init(timer);
    oor_timer_init(timer,owner,cb_fn,timer_arg,free_arg_fn,nonces_lst);

    return (timer);
}


int
stop_timer_from_obj(void *obj,oor_timer_t *timer,htable_ptrs_t *ptrs_ht,
        htable_nonces_t *nonce_ht)
{
    nonces_list_t *nonces_lst;

    htable_ptrs_timers_rm_timer(ptrs_ht, obj, timer);

    nonces_lst = oor_timer_nonces(timer);
    if (nonces_lst){
        htable_nonces_reset_nonces_lst(nonce_ht,nonces_lst);
        nonces_list_free(nonces_lst);
    }
    oor_timer_stop(timer);

    return (GOOD);
}

int
stop_timers_from_obj(void *obj,htable_ptrs_t *ptrs_ht, htable_nonces_t *nonce_ht)
{
    glist_t *timer_lst;
    glist_entry_t *timer_it;
    oor_timer_t *timer;
    nonces_list_t *nonces_lst;

    timer_lst = htable_ptrs_timers_rm(ptrs_ht, obj);
    if (!timer_lst){
        return (BAD);
    }

    glist_for_each_entry(timer_it,timer_lst){
        timer = (oor_timer_t*)glist_entry_data(timer_it);
        nonces_lst = oor_timer_nonces(timer);
        if (nonces_lst){
            htable_nonces_reset_nonces_lst(nonce_ht,nonces_lst);
            nonces_list_free(nonces_lst);
        }
        oor_timer_stop(timer);
    }
    glist_destroy(timer_lst);

    return (GOOD);
}


int
stop_timers_of_type_from_obj(void *obj, timer_type type,
        htable_ptrs_t *ptrs_ht, htable_nonces_t *nonce_ht)
{
    glist_t *timers_lst;
    glist_entry_t *timer_it;
    oor_timer_t *timer;
    nonces_list_t *nonces_lst;

    timers_lst = htable_ptrs_timers_rm_timers_of_type(ptrs_ht,obj,type);

    glist_for_each_entry(timer_it,timers_lst){
        timer = (oor_timer_t*)glist_entry_data(timer_it);
        nonces_lst = oor_timer_nonces(timer);
        if (nonces_lst){
            htable_nonces_reset_nonces_lst(nonce_ht,nonces_lst);
            nonces_list_free(nonces_lst);
        }
        oor_timer_stop(timer);
    }
    glist_destroy(timers_lst);

    return (GOOD);
}


int
htable_ptrs_timers_add(htable_ptrs_t *ptr_ht, void *key, oor_timer_t *timer)
{
    glist_t *timer_lst;

    timer_lst = htable_ptrs_lookup(ptr_ht, key);
    if (!timer_lst){
        timer_lst = glist_new();
        if (!timer_lst){
            return (BAD);
        }
        htable_ptrs_insert(ptr_ht, key, timer_lst);
    }
    return (glist_add(timer, timer_lst));
}

/* Return the list of timers associated with the object */
glist_t *
htable_ptrs_timers_get_timers(htable_ptrs_t *ptr_ht, void *key)
{
    return(htable_ptrs_lookup(ptr_ht, key));
}

/* Return the list the timers of the requested type associated with the object */
glist_t *
htable_ptrs_timers_get_timers_of_type_from_obj(htable_ptrs_t *ptr_ht, void *key,
        timer_type type)
{
    oor_timer_t *timer;
    glist_t *set_timers_lst;
    glist_t *timer_lst;
    glist_entry_t *timer_it, *timer_it_aux;

    set_timers_lst = glist_new();

    timer_lst = htable_ptrs_lookup(ptr_ht, key);
    if (!timer_lst){
        return (set_timers_lst);
    }

    glist_for_each_entry_safe(timer_it,timer_it_aux,timer_lst){
        timer = (oor_timer_t*)glist_entry_data(timer_it);
        if (type == oor_timer_type(timer)){
            glist_add(timer, set_timers_lst);
        }
    }
    return set_timers_lst;
}



/* Remove the entry from hash table and returns the list of timers associated
 * with the object */
glist_t *
htable_ptrs_timers_rm(htable_ptrs_t *ptr_ht, void *key)
{
    return ((glist_t *)htable_ptrs_remove(ptr_ht, key));
}

/* Remove from the list the timers of the requested type associated with the object
 * The removed timers are returned in a list */
glist_t *
htable_ptrs_timers_rm_timers_of_type(htable_ptrs_t *ptr_ht, void *key,
        timer_type type)
{
    glist_t *rm_timers_lst = glist_new();
    glist_t *timer_lst;
    glist_entry_t *timer_it, *timer_it_aux;
    oor_timer_t *timer;

    timer_lst = htable_ptrs_lookup(ptr_ht, key);
    if (!timer_lst){
        return (rm_timers_lst);
    }

    glist_for_each_entry_safe(timer_it,timer_it_aux,timer_lst){
        timer = (oor_timer_t*)glist_entry_data(timer_it);
        if (type == oor_timer_type(timer)){
            glist_add(timer, rm_timers_lst);
            glist_remove(timer_it, timer_lst);
        }
    }
    return rm_timers_lst;
}

/* Remove the timer from the list. It doesn't stop the timer */
int
htable_ptrs_timers_rm_timer(htable_ptrs_t *ptr_ht, void *key, oor_timer_t *timer)
{
    glist_t *timer_lst;

    timer_lst = htable_ptrs_lookup(ptr_ht, key);
    if (!timer_lst){
        return(ERR_NO_EXIST);
    }
    glist_remove_obj(timer, timer_lst);
    return (GOOD);
}


