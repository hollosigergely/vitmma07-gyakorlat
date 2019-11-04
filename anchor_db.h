#ifndef __ANCHOR_DB_H
#define __ANCHOR_DB_H

#include <stdbool.h>
#include <stdint.h>

#define ANCHOR_DB_SIZE  6

void anchor_db_init();
bool anchor_db_add(uint16_t anchor_id);
int anchor_db_get_size();
uint16_t anchor_db_get_anchor(int index);

#endif
