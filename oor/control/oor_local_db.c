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


#include "oor_local_db.h"
#include "../oor_external.h"
#include "../lib/oor_log.h"


local_map_db_t *
local_map_db_new()
{
    local_map_db_t *db;
    db = xzalloc(sizeof(local_map_db_t));
    if (!db) {
        OOR_LOG(LCRIT, "Could allocate local map database ");
        return(NULL);
    }

    db->db = mdb_new();
    if (!db->db) {
        OOR_LOG(LCRIT, "Could allocate local map database ");
        return(NULL);
    }

    return(db);
}

void
local_map_db_del(local_map_db_t *lmdb)
{
    mdb_del(lmdb->db, (mdb_del_fct)map_local_entry_del);
    free(lmdb);
}

int
local_map_db_add_entry(local_map_db_t *lmdb, map_local_entry_t *map_loc_e)
{
    lisp_addr_t *eid, *ip_pref;

    eid = map_local_entry_eid(map_loc_e);
    ip_pref = lisp_addr_get_ip_pref_addr(eid);
    if (mdb_add_entry(lmdb->db, ip_pref, map_loc_e) != GOOD) {
        OOR_LOG(LDBG_3, "Couldn't add mapping for EID %s to local mappings database",
                lisp_addr_to_char(map_local_entry_eid(map_loc_e)));
        return(BAD);
    }
    return(GOOD);
}

map_local_entry_t *
local_map_db_lookup_eid(local_map_db_t *lmdb, lisp_addr_t *eid, uint8_t check_iid)
{
	map_local_entry_t *map_loc_e = NULL;
	uint32_t iid = 0;
	lisp_addr_t *ip_pref_eid, *db_eid;


	if (lisp_addr_is_lcaf(eid)){
	    if (lcaf_addr_is_iid (lisp_addr_get_lcaf(eid)) == FALSE){
	        OOR_LOG(LDBG_2, "local_map_db_lookup_eid: LCAF %d not supported for EID", lisp_addr_to_char(eid));
	        return (NULL);
	    }
	    iid = lcaf_iid_get_iid(lisp_addr_get_lcaf(eid));
	    ip_pref_eid = lisp_addr_get_ip_pref_addr(eid);
	}else{
	    ip_pref_eid = eid;
	}

	map_loc_e = (map_local_entry_t *)mdb_lookup_entry(lmdb->db, ip_pref_eid);
    if (!map_loc_e) {
        OOR_LOG(LDBG_3, "Couldn't find mapping for EID %s in local mappings database",
                lisp_addr_to_char(eid));
        return (NULL);
    }

    if (check_iid){
        db_eid = map_local_entry_eid(map_loc_e);
        if (iid > 0){
            if (iid != lcaf_iid_get_iid(lisp_addr_get_lcaf(db_eid))){
                OOR_LOG(LDBG_3, "Couldn't find mapping for EID %s in local mappings database. IID not match",
                        lisp_addr_to_char(eid));
                return (NULL);
            }
        }else if (lisp_addr_is_lcaf(db_eid)){
            OOR_LOG(LDBG_3, "Couldn't find mapping for EID %s in local mappings database. Different IID",
                    lisp_addr_to_char(eid));
            return (NULL);
        }
    }

    return (map_loc_e);
}

map_local_entry_t *
local_map_db_lookup_eid_exact(local_map_db_t *lmdb, lisp_addr_t *eid)
{
	map_local_entry_t *map_loc_e;
    lisp_addr_t *ip_pref_eid, *db_eid;


    if (lisp_addr_is_lcaf(eid)){
        if (lcaf_addr_is_iid (lisp_addr_get_lcaf(eid)) == FALSE){
            OOR_LOG(LDBG_2, "local_map_db_lookup_eid: LCAF %d not supported for EID", lisp_addr_to_char(eid));
            return (NULL);
        }
        ip_pref_eid = lisp_addr_get_ip_pref_addr(eid);
    }else{
        ip_pref_eid = eid;
    }

    map_loc_e = (map_local_entry_t *)mdb_lookup_entry_exact(lmdb->db, ip_pref_eid);
    if (!map_loc_e) {
        OOR_LOG(LDBG_3, "Couldn't find mapping for EID %s in local mappings database",
                lisp_addr_to_char(eid));
        return (NULL);
    }
    db_eid = map_local_entry_eid(map_loc_e);
    /* To check IID of local database we should compare both addresses */
    if (lisp_addr_cmp(db_eid,eid) != 0){
        OOR_LOG(LDBG_3, "Couldn't find mapping for EID %s in local mappings database.",
                        lisp_addr_to_char(eid));
        return (NULL);
    }

    return (map_loc_e);
}

void
local_map_db_del_entry(local_map_db_t *lmdb, lisp_addr_t *eid)
{
	map_local_entry_t *map_loc_e = NULL;
	map_loc_e = (map_local_entry_t *)mdb_remove_entry(lmdb->db, eid);
    if (map_loc_e != NULL) {
    	map_local_entry_del(map_loc_e);
    }
}

lisp_addr_t *
local_map_db_get_main_eid(local_map_db_t *lmdb, int afi)
{
    void *it;
    lisp_addr_t *eid , *eid_res = NULL;
    uint8_t found = FALSE;

    mdb_foreach_ip_entry_with_break(lmdb->db, it, found) {
        eid = map_local_entry_eid((map_local_entry_t *)it);
        if (eid && lisp_addr_ip_afi(eid) == afi) {
            found = TRUE;
            eid_res = eid;
        }
    } mdb_foreach_ip_entry_with_break_end(found);
    return (eid_res);
}

int
local_map_db_num_ip_eids(local_map_db_t *lmdb, int afi)
{
    void *it = NULL;
    lisp_addr_t *eid = NULL;
    int ctr = 0;
    /* search could be better implemented but local db is small
     * so this should do for now  */
    mdb_foreach_ip_entry(lmdb->db, it) {
        eid = map_local_entry_eid((map_local_entry_t *)it);
        if (eid && lisp_addr_lafi(eid) == LM_AFI_IPPREF
            && lisp_addr_ip_afi(eid) == afi) {
            ctr ++;
        }
    } mdb_foreach_ip_entry_end;

    return (ctr);
}

inline int
local_map_db_n_entries(local_map_db_t *lmdb)
{
    return(lmdb->db->n_entries);
}

void
local_map_db_dump(local_map_db_t *lmdb, int log_level)
{
    map_local_entry_t *map_loc_e = NULL;
    void *it = NULL;

    OOR_LOG(log_level,"****************** LISP Local Mappings ****************\n");

    mdb_foreach_entry(lmdb->db, it) {
    	map_loc_e = (map_local_entry_t *)it;
    	map_local_entry_dump(map_loc_e,log_level);
    } mdb_foreach_entry_end;
    OOR_LOG(log_level,"*******************************************************\n");
}

