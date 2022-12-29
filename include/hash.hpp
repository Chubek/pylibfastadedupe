#ifndef HASH_H
#define HASH_H

#define HASH64(V, RES, MASK)  \
        [V, &RES, MASK]() {\
            RES = (~V + (V << 21)) & MASK; \
            RES = RES ^ RES >> 24; \
            RES = ((RES + (RES << 3)) + (RES << 8)) & MASK; \
            RES = RES ^ RES >> 14; \
            RES = ((RES + (RES << 2) + (RES << 4))) & MASK; \
            RES = RES ^ RES >> 28; \
            RES = (RES + (RES << 31)) & MASK; \      
        }


#define HASHSTR(STR, RES) \
        [STR, &RES]() {\
            char __ARR[STR.length() + 1]; \
            strncpy(__ARR, STR.c_str(), STR.length()); \
            unsigned long RES = 5381; \
            for (int __i = 0; __i < STR.length(); __i++)  \
                RES = ((RES << 5) + RES) + __ARR[__i]; \
        }


#define MASKU64_32(h)   h & 0x00000000FFFFFFFF

#endif