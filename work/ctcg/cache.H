/* 
** Tempo, (C) 1995, Irisa
** Version: $Revision: 1.5 $ 
** Date: %D%
** Author: Nic Volanski
*/
#include "ast.H"

class Cache_Answer { 
  // a cache lookup returns a pointer to this
  // (or NULL if lookup failed)
  char *fname;     // the name of the retrieved specialized version
  char *out_store; // the output store (used for re-doing the side-effects)
  // WARNING: this can deal only with some restricted side-effects:
  // - on the store (i.e. no I/O, ...), and
  // - deterministic (i.e. no random assignemts), and
  // - depending only on the static inputs (i.e. dynamic allocation
  // cannot be allowed, unless the whole heap is static).
  // 
  // Other types of side-effects are supposed to be treated by the
  // action analysis.
public:
  Cache_Answer(char *name, char *store) : fname(name), out_store(store)
       { } // doesn't duplicate strings!!
  char *get_fname() { return fname; }
  char *get_out_store() { return out_store; }
};

Cache_Answer *cache_lookup(identifier fname, int index, char *store); 
                  // returns NULL if not found
void *cache_insert_prespec(identifier fname, int ix, char *in_store);
                  // Returns a handle to the cache entry, doesn't copy store.
char *cache_complete_prespec(void *pre, char *out_store, exp body);
                  // Returns fname, doesn't copy store or body.
char *cache_insert(identifier fname, int index, 
		   char *in_store, char *out_store,
		   exp body);
                  // returns fname; doesn't copy stores, nor body
                  // doesn't check for occurence!
int cache_no_hits(void *pre);
void cache_reset_hits(void *pre);
void cache_print(ML_ctx global_ctx);
