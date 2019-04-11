#include "g_local.h"
#include "q_shared.h"
#include "g_npccontrol.h"
#include "g_admin.h"

int selectedAI = ENTITYNUM_NONE; 

void SelectNPC(gentity_t *ent)
{
	vec3_t		src, dest, forward;
	trace_t		trace;
	VectorCopy(ent->client->renderInfo.eyePoint, src);
	AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
															 //extend to find end of use trace
	VectorMA(src, 32967.0f, forward, dest);

	//Trace ahead to find a valid target
	trap_Trace(&trace, src, vec3_origin, vec3_origin, dest, ent->s.number, MASK_OPAQUE | CONTENTS_SOLID | CONTENTS_BODY);

	if (trace.fraction == 1.0f || trace.entityNum < 1)
	{
		return;
	}
	gentity_t* target = &g_entities[trace.entityNum];
	if (target->client) {
		selectedAI = target->s.number;
		trap_SendServerCommand(ent - g_entities, va("print \"^3Selcted: %s\n\"", target->targetname));
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
