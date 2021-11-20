#include "hardware.h"

void get_uid(uint8_t id[12]){
    uint8_t *t=(uint8_t *)0x1FFFF7E8;
    for(int i=0;i<12;++i)
        id[i]=t[i];
}
