#include "g_local.h"
#include "g_admin.h"
#include "q_shared.h"

void AM_Tele(gentity_t *ent)
{
	vec3_t location;
	vec3_t forward;
	if (ent->client->pers.iamanadmin && ((ent->r.svFlags & SVF_ADMIN1) && (g_adminControl1.integer & (1 << A_ADMINTELE))))
	{
		if (trap_Argc() < 2 || trap_Argc() > 4) {
			// trap_SendServerCommand(ent - g_entities, va("print \"^3Type in ^5/help teleport ^3if you need help with this command.\n\""));
			trap_SendServerCommand(ent - g_entities, va("print \"^3Usaage: ^2/tele ID1 ID2^3. Example: tele 3 2\n^2or\n\""));
			trap_SendServerCommand(ent - g_entities, va("print \"^3tele (X) (Y) (Z)\n^3type in ^2/origin ^3OR ^2/origin (name) ^3to find out (X) (Y) (Z)\n\""));
			return;
		}
		if (trap_Argc() == 3)
		{
			int	clId = -1;
			int   clId2 = -1;
			char	arg1[MAX_STRING_CHARS];
			char	arg2[MAX_STRING_CHARS];
			trap_Argv(1, arg1, sizeof(arg1));
			clId = G_ClientNumberFromArg(arg1);
			trap_Argv(2, arg2, sizeof(arg2));
			clId2 = G_ClientNumberFromArg(arg2);


			if (clId == -1 || clId2 == -1)
			{
				trap_SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", arg1));
				return;
			}
			if (clId == -2 || clId2 == -2)
			{
				trap_SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", arg1));
				return;
			}
			if (clId >= 32 || clId2 >= 32)
			{
				trap_SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", arg1));
				return;
			}
			// either we have the client id or the string did not match
			if (!g_entities[clId].inuse && !g_entities[clId2].inuse)
			{ // check to make sure client slot is in use
				trap_SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg1));
				return;
			}
			if (g_entities[clId].health <= 0 && g_entities[clId2].health <= 0)
			{
				return;
			}
			VectorCopy(g_entities[clId2].client->ps.origin, location);
			AngleVectors(g_entities[clId2].client->ps.viewangles, forward, NULL, NULL);
			// set location out in front of your view
			forward[2] = 0; //no elevation change
			VectorNormalize(forward);
			VectorMA(g_entities[clId2].client->ps.origin, 100, forward, location);
			location[2] += 5; //add just a bit of height???
							  // teleport them to you
			TeleportPlayer(&g_entities[clId], location, g_entities[clId2].client->ps.viewangles);
			//trap_SendServerCommand( -1, va("cp \"%s^7\n%s\n\"", g_entities[clId].client->pers.netname, roar_teleport_saying.string ) );  
		}
		if (trap_Argc() == 4)
		{
			vec3_t		origin;
			char		buffer[MAX_TOKEN_CHARS];
			int			i;

			for (i = 0; i < 3; i++) {
				trap_Argv(i + 1, buffer, sizeof(buffer));
				origin[i] = atof(buffer);
			}

			TeleportPlayer(ent, origin, ent->client->ps.viewangles);
		}
	}
	else
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
}

void AM_Notarget(gentity_t *ent)
{
	char	*msg = "Nothing happens";
	char	arg1[MAX_STRING_CHARS];
	int		clID;

	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NOTARGET))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	
	if (trap_Argc() < 2)
	{
		ent->flags ^= FL_NOTARGET;
		if (!(ent->flags & FL_NOTARGET))
			msg = "notarget OFF\n";
		else
			msg = "notarget ON\n";
		trap_SendServerCommand(ent - g_entities, va("print \"%s\"", msg));
	}
	if (trap_Argc() > 2)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Usage: amtarget client_ID\n\""));
		return;
	}
	if (trap_Argc() == 2)
	{
		gentity_t *target;
		trap_Argv(1, arg1, sizeof(arg1));
		clID = G_ClientNumberFromArg(arg1);
		if (!g_entities[clID].inuse)
		{ // check to make sure client slot is in use
			trap_SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg1));
			return;
		}
		target = &g_entities[clID];
		target->flags ^= FL_NOTARGET;
		if (!(target->flags & FL_NOTARGET))
			trap_SendServerCommand(ent - g_entities, va("print \"%s's notarget OFF\n\"", target->client->pers.netname));
		else
			trap_SendServerCommand(ent - g_entities, va("print \"%s's notarget ON\n\"", target->client->pers.netname));
	}
}

void AM_AnimateHold(gentity_t *ent)
{
	char arg1[MAX_STRING_CHARS]; //Client ID
	char arg2[MAX_STRING_CHARS]; //Anim name
	gentity_t *target; 
	int  targetNum; 
	int animName;
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_ANIMATE))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (trap_Argc() != 3)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Usage: animme client_ID anim_name \n\""));
		return;
	}
	trap_Argv(1, arg1, sizeof(arg1));
	trap_Argv(2, arg2, sizeof(arg2));
	targetNum = G_ClientNumberFromArg(arg1);
	target = &g_entities[targetNum];
	animName = GetIDForString(animTable, arg2);
	if (animName == -1)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Incorrect animation ID. Did you type it properly?\n\""));
		return;
	}
	if (!g_entities[targetNum].inuse)
	{ // check to make sure client slot is in use
		trap_SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg1));
		return;
	}
	if (target->client->ps.legsAnim == animName)
	{
		target->client->ps.forceDodgeAnim = 0;
		target->client->ps.forceHandExtendTime = 0;
		target->client->ps.saberCanThrow = qtrue;
	}
	else
	{
		target->client->ps.forceHandExtend = HANDEXTEND_TAUNT;
		target->client->ps.forceDodgeAnim = animName;
		target->client->ps.forceHandExtendTime = level.time + Q3_INFINITE;
		target->client->ps.saberCanThrow = qfalse;
		////ent->client->emote_freeze = 1;
		StandardSetBodyAnim(target, animName, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS);
		target->client->ps.saberMove = LS_NONE;
		ent->client->ps.saberBlocked = 0;
		ent->client->ps.saberBlocking = 0;
	}
}

void AM_Animate(gentity_t *ent)
{
	char buffer[MAX_STRING_CHARS];
	gentity_t *target;
	int  targetNum, animName, length;
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_ANIMATE))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (trap_Argc() != 4)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Usage: animme client_ID anim_name anim_length (ms)\n\""));
		return;
	}
	trap_Argv(1, buffer, sizeof(buffer));
	targetNum = G_ClientNumberFromArg(buffer);
	if (!g_entities[targetNum].inuse)
	{ // check to make sure client slot is in use
		trap_SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", buffer));
		return;
	}

	trap_Argv(2, buffer, sizeof(buffer));
	target = &g_entities[targetNum];
	animName = GetIDForString(animTable, buffer);

	if (animName == -1)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Incorrect animation ID. Did you type it properly?\n\""));
		return;
	}
	trap_Argv(3, buffer, sizeof(buffer));
	length = atoi(buffer);

	
	if (target->client->ps.legsAnim == animName)
	{
		return; 
	}
	else
	{
		target->client->ps.forceHandExtend = HANDEXTEND_TAUNT;
		target->client->ps.forceDodgeAnim = animName;
		target->client->ps.forceHandExtendTime = level.time + length;
		target->client->ps.saberCanThrow = qfalse;
		////ent->client->emote_freeze = 1;
		StandardSetBodyAnim(target, animName, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS);
		target->client->ps.saberMove = LS_NONE;
		ent->client->ps.saberBlocked = 0;
		ent->client->ps.saberBlocking = 0;
	}
}

void AM_Drop(gentity_t *ent)
{
	{
		vec3_t vel, direction;
		gitem_t *item;
		gentity_t *launched;
		gentity_t *self = ent;
		int weapon = self->s.weapon;
		int ammoSub;
		float speed = 500;

		AngleVectors(ent->client->ps.viewangles, direction, NULL, NULL);
		VectorNormalize(direction);
		if (g_gametype.integer == GT_SIEGE)
		{ //no dropping weaps
			return;
		}

		if (weapon <= WP_NONE || weapon == WP_MELEE)
		{ //can't drop our hands!
			return;
		}

		if (weapon == WP_EMPLACED_GUN ||
			weapon == WP_TURRET)
		{
			return;
		}

		// find the item type for this weapon
		item = BG_FindItemForWeapon(weapon);

		ammoSub = (self->client->ps.ammo[weaponData[weapon].ammoIndex] - bg_itemlist[BG_GetItemIndexByTag(weapon, IT_WEAPON)].quantity);

		if (ammoSub < 0)
		{
			int ammoQuan = item->quantity;
			ammoQuan -= (-ammoSub);

			if (ammoQuan <= 0 && (!weapon == WP_SABER || !weapon == WP_BRYAR_PISTOL))
			{ //no ammo
				return;
			}
		}

		vel[0] = direction[0] * speed;
		vel[1] = direction[1] * speed;
		vel[2] = direction[2] * speed;

		launched = LaunchItem(item, self->client->ps.origin, vel);

		launched->s.generic1 = self->s.number;
		launched->s.powerups = level.time + 1500;

		//[WeaponSys]
		launched->count = self->client->ps.ammo[weaponData[weapon].ammoIndex];

		self->client->ps.ammo[weaponData[weapon].ammoIndex] = 0;
		/*
		launched->count = bg_itemlist[BG_GetItemIndexByTag(weapon, IT_WEAPON)].quantity;

		self->client->ps.ammo[weaponData[weapon].ammoIndex] -= bg_itemlist[BG_GetItemIndexByTag(weapon, IT_WEAPON)].quantity;

		if (self->client->ps.ammo[weaponData[weapon].ammoIndex] < 0)
		{
		launched->count -= (-self->client->ps.ammo[weaponData[weapon].ammoIndex]);
		self->client->ps.ammo[weaponData[weapon].ammoIndex] = 0;
		}
		*/
		//[/WeaponSys]

		if ((self->client->ps.ammo[weaponData[weapon].ammoIndex] < 1 && weapon != WP_DET_PACK) ||
			(weapon != WP_THERMAL && weapon != WP_DET_PACK && weapon != WP_TRIP_MINE))
		{
			int i = 0;
			int weap = -1;

			self->client->ps.stats[STAT_WEAPONS] &= ~(1 << weapon);

			while (i < WP_NUM_WEAPONS)
			{
				if ((self->client->ps.stats[STAT_WEAPONS] & (1 << i)) && i != WP_NONE)
				{ //this one's good
					weap = i;
					break;
				}
				i++;
			}

			if (weap != -1)
			{
				self->s.weapon = weap;
				self->client->ps.weapon = weap;
			}
			else
			{
				self->s.weapon = 0;
				self->client->ps.weapon = 0;
			}

			G_AddEvent(self, EV_NOAMMO, weapon);
		}
	}
}
/*
void AM_God()
{
}

void AM_Noclip()
{
}
*/

