
extern void AM_Tele(gentity_t *ent);
extern void AM_Notarget(gentity_t *ent);
extern void AM_Animate(gentity_t *ent);

typedef enum
{
	A_ADMINTELE = 0,
	A_NOTARGET,
	A_GOD,
	A_NOCLIP,
	A_FREEZE,
	A_SILENCE,
	A_PROTECT,
	A_ADMINBAN,
	A_ADMINKICK,
	A_NPC,
	A_INSULTSILENCE,
	A_ANIMATE,
	A_DEMIGOD,
	A_ADMINSIT,
	A_SCALE,
	A_SPLAT,
	A_SLAY,
	A_GRANTADMIN,
	A_CHANGEMAP,
	A_EMPOWER,
	A_RENAME,
	A_LOCKNAME,
	A_CSPRINT,
	A_FORCETEAM,
	A_RENAMECLAN,
	A_HUMAN,
	A_WEATHER,
	A_PLACE,
	A_PUNISH,
	A_SLEEP,
	A_SLAP,
	A_LOCKTEAM
} admin_type_t;