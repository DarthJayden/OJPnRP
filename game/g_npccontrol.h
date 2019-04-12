
extern stringID_table_t BSTable[];
extern stringID_table_t BSETTable[];
extern stringID_table_t teamTable[];
extern stringID_table_t NPCClassTable[];
extern stringID_table_t RankTable[];
extern stringID_table_t MoveTypeTable[];
extern stringID_table_t FPTable[];

extern void StandardSetBodyAnim(gentity_t *self, int anim, int flags);

extern gentity_t *NPC_Spawn_Do(gentity_t *ent);
extern void NPC_PrecacheByClassName(const char* type);