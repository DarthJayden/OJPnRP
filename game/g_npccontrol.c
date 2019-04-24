#include "g_local.h"
#include "q_shared.h"
#include "g_npccontrol.h"
#include "g_admin.h"

int selectedAI = ENTITYNUM_NONE; 

void SelectNPC(gentity_t *ent)
{
	vec3_t		src, dest, forward;
	trace_t		trace;
	if (!ent->client->sess.sessionTeam)
	{
		VectorCopy(ent->client->renderInfo.eyePoint, src);
		AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
		//extend to find end of use trace
		VectorMA(src, 32967.0f, forward, dest);

	}
	else
	{
		VectorCopy(ent->client->ps.origin, src);
		AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
		//extend to find end of use trace
		VectorMA(src, 32967.0f, forward, dest);
	}
	//Trace ahead to find a valid target
	trap_Trace(&trace, src, vec3_origin, vec3_origin, dest, ent->s.number, MASK_OPAQUE | CONTENTS_SOLID | CONTENTS_BODY);

	if (trace.fraction == 1.0f || trace.entityNum < 1)
	{
		return;
	}
	gentity_t* target = &g_entities[trace.entityNum];
	if (target->client) {
		selectedAI = target->s.number;
		trap_SendServerCommand(ent - g_entities, va("print \"^3Selcted: %s\n\"", target->NPC_type));
	}
	else {
		trap_SendServerCommand(ent - g_entities, va("print \"^1No NPC in crosshair\n\""));
		return; 
		 }
}

void DeselectNPC(gentity_t *ent)
{
	selectedAI = ENTITYNUM_NONE; 
}

void SetNPCAnim(gentity_t *ent)
{
	char arg1[MAX_STRING_CHARS]; //Anim name
	gentity_t *target;
	int animName;
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NPC))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (trap_Argc() != 2)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Usage: animNPC anim_name \n\""));
		return;
	}
	if (selectedAI == ENTITYNUM_NONE)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1No NPC selected!\n\""));
		return;
	}
	trap_Argv(1, arg1, sizeof(arg1));
	target = &g_entities[selectedAI];
	animName = GetIDForString(animTable, arg1);
	if (animName == -1)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Incorrect animation ID. Did you type it properly?\n\""));
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

void KillNPC(gentity_t *ent)
{
	gentity_t *target;

	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NPC))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}

	if (selectedAI == ENTITYNUM_NONE)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1No NPC selected!\n\""));
		return;
	}
	target = &g_entities[selectedAI];
	Com_Printf(S_COLOR_GREEN"Killing NPC %s named %s\n", target->NPC_type, target->targetname);
	target->health = 0;
	if (target->die && target->client) //Jayden: i'm not sur if this is needed...
	{
		target->die(target, target, target, target->client->pers.maxHealth, MOD_UNKNOWN);
	}
	G_FreeEntity(target);
}

gentity_t *NPCspawn(gentity_t *ent, char *npc_type, char *targetname, qboolean isVehicle, vec3_t origin)
{
	gentity_t		*NPCspawner = G_Spawn();

	if (!NPCspawner)
	{
		Com_Printf(S_COLOR_RED"NPC_Spawn Error: Out of entities!\n");
		return NULL;
	}

	NPCspawner->think = G_FreeEntity;
	NPCspawner->nextthink = level.time + FRAMETIME;

	if (!npc_type)
	{
		return NULL;
	}

	if (!npc_type[0])
	{
		Com_Printf(S_COLOR_RED"Error, expected one of:\n"S_COLOR_WHITE" NPC spawn [NPC type (from ext_data/NPCs)]\n NPC spawn vehicle [VEH type (from ext_data/vehicles)]\n");
		return NULL;
	}

	if (!ent || !ent->client)
	{//screw you, go away
		return NULL;
	}

	//rwwFIXMEFIXME: Care about who is issuing this command/other clients besides 0?
	//Spawn it at spot of first player
	//FIXME: will gib them!

	G_SetOrigin(NPCspawner, origin);
	VectorCopy(NPCspawner->r.currentOrigin, NPCspawner->s.origin);
	//set the yaw so that they face away from player
	NPCspawner->s.angles[1] = ent->client->ps.viewangles[1];

	trap_LinkEntity(NPCspawner);

	NPCspawner->NPC_type = G_NewString(npc_type);

	if (targetname)
	{
		NPCspawner->NPC_targetname = G_NewString(targetname);
	}

	NPCspawner->count = 1;

	NPCspawner->delay = 0;

	//NPCspawner->spawnflags |= SFB_NOTSOLID;

	//NPCspawner->playerTeam = TEAM_FREE;
	//NPCspawner->behaviorSet[BSET_SPAWN] = "common/guard";

	if (isVehicle)
	{
		NPCspawner->classname = "NPC_Vehicle";
	}

	//[CoOp]
	//converted this stuff into a special function.
	NPC_PrecacheByClassName(NPCspawner->NPC_type);

	return (NPC_Spawn_Do(NPCspawner));
}

void SpawnNPC(gentity_t *ent)
{
	char buffer[MAX_STRING_CHARS], npc_type[MAX_STRING_CHARS];
	qboolean	isVehicle = qfalse;
	vec3_t  target_org, forward;
	trace_t trace;
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NPC))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (trap_Argc() < 2 || trap_Argc() > 6)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Usage: npcspawn vehicle (^1nothing if not vehicle^3) npc_type \n or \n npcspawn vehicle (^1nothing if not vehicle^3) npc_type X Y Z\n\""));
		return;
	}
	trap_Argv(1, buffer, sizeof(buffer));
	if (!Q_stricmp("vehicle", buffer)) //3 or 6 args
	{
		trap_Argv(2, buffer, sizeof(buffer));
		Q_strncpyz(npc_type, buffer, sizeof(npc_type));
		isVehicle = qtrue;
			if (trap_Argc() == 3)
			{
				AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
				VectorNormalize(forward);
				VectorMA(ent->client->ps.origin, 64, forward, target_org);
				trap_Trace(&trace, ent->client->ps.origin, NULL, NULL, target_org, 0, MASK_SOLID);
				VectorCopy(trace.endpos, target_org);
				target_org[2] -= 24;
				trap_Trace(&trace, trace.endpos, NULL, NULL, target_org, 0, MASK_SOLID);
				VectorCopy(trace.endpos, target_org);
				target_org[2] += 24;
			}
			if (trap_Argc() == 6)
			{
				trap_Argv(3, buffer, sizeof(buffer));
				target_org[0] = atoi(buffer);
				trap_Argv(4, buffer, sizeof(buffer));
				target_org[1] = atoi(buffer);
				trap_Argv(5, buffer, sizeof(buffer));
				target_org[2] = atoi(buffer);
			}
	}
	else //2 or 5 args
	{
		Q_strncpyz(npc_type, buffer, sizeof(npc_type));
		if (trap_Argc() == 2)
		{
			AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
			VectorNormalize(forward);
			VectorMA(ent->client->ps.origin, 64, forward, target_org);
			trap_Trace(&trace, ent->client->ps.origin, NULL, NULL, target_org, 0, MASK_SOLID);
			VectorCopy(trace.endpos, target_org);
			target_org[2] -= 24;
			trap_Trace(&trace, trace.endpos, NULL, NULL, target_org, 0, MASK_SOLID);
			VectorCopy(trace.endpos, target_org);
			target_org[2] += 24;
		}
		if (trap_Argc() == 5)
		{
			trap_Argv(2, buffer, sizeof(buffer));
			target_org[0] = atoi(buffer);
			trap_Argv(3, buffer, sizeof(buffer));
			target_org[1] = atoi(buffer);
			trap_Argv(4, buffer, sizeof(buffer));
			target_org[2] = atoi(buffer);
		}
	}
	NPCspawn(ent, npc_type, 0, isVehicle, target_org);
}


void SetNpcAngle(gentity_t *ent) 
{
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NPC))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (selectedAI == ENTITYNUM_NONE)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1No NPC selected!\n\""));
		return;
	}


}

void NpcFollowMe(gentity_t *ent)
{
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NPC))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (selectedAI == ENTITYNUM_NONE)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1No NPC selected!\n\""));
		return;
	}
	gentity_t* ai = &g_entities[selectedAI];

	if (ai->client->leader)
	{
		ai->client->leader = NULL; //Не отключается. Как бы обнулить лидера?
		ai->NPC->goalEntity = NULL; 
		ai->NPC->defaultBehavior = BS_STAND_GUARD;
		ai->NPC->behaviorState = BS_DEFAULT;
	}
	else
	{
		ai->client->leader = ent;
		ai->NPC->defaultBehavior = BS_FOLLOW_LEADER;
		ai->NPC->behaviorState = BS_FOLLOW_LEADER;
	}
}

void NpcSetTeam(gentity_t *ent)
{
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NPC))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (selectedAI == ENTITYNUM_NONE)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1No NPC selected!\n\""));
		return;
	}
	if (trap_Argc() < 2)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^3Usage: /npcteam playerTeam enemyTeam\n\""));
		return;
	}
	gentity_t* ai = &g_entities[selectedAI];
	char buffer[MAX_STRING_CHARS];
	int enemyteam, playerteam;

	trap_Argv(1, buffer, sizeof(buffer));

		if (Q_stricmp(buffer, "player") == 0)
		{
			playerteam = NPCTEAM_PLAYER;
		}
		else if (Q_stricmp(buffer, "enemy") == 0)
		{
			playerteam = NPCTEAM_ENEMY;
		}
		else if (Q_stricmp(buffer, "neutral") == 0)
		{
			playerteam = NPCTEAM_NEUTRAL;
		}
		else
		{
		trap_SendServerCommand(ent - g_entities, va("print \"^1Incorrect playerteam!\n\""));
		return;
		}

	trap_Argv(2, buffer, sizeof(buffer));

		if (Q_stricmp(buffer, "player") == 0)
		{
			enemyteam = NPCTEAM_PLAYER;
		}
		else if (Q_stricmp(buffer, "enemy") == 0)
		{
			enemyteam = NPCTEAM_ENEMY;
		}
		else if (Q_stricmp(buffer, "neutral") == 0)
		{
			enemyteam = NPCTEAM_NEUTRAL;
		}
		else
		{
			trap_SendServerCommand(ent - g_entities, va("print \"^1Incorrect enemyteam!\n\""));
			return;
		}

	ai->client->playerTeam = playerteam;
	ai->client->enemyTeam = enemyteam;
}

void NpcAttack(gentity_t *ent)
{
	if (!(ent->r.svFlags & SVF_ADMIN1) || ((ent->r.svFlags & SVF_ADMIN1) && !(g_adminControl1.integer & (1 << A_NPC))))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1You have no rights to use this command\n\""));
		return;
	}
	if (selectedAI == ENTITYNUM_NONE)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"^1No NPC selected!\n\""));
		return;
	}
}