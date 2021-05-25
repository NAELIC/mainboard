#ifndef _INFOS_H
#define _INFOS_H

void infos_init();

void infos_set(int id, bool kickerInverted);
bool infos_kicker_inverted();
int infos_get_id();
int id_from_hall(bool hall1, bool hall2, bool hall3, bool hall4);


extern bool developer_mode;


#define ID_0_HALL 1
#define ID_1_HALL 2
#define ID_2_HALL 4
#define ID_3_HALL 8
#define ID_4_HALL 5
#define ID_5_HALL 9
#define ID_6_HALL 10

#endif
