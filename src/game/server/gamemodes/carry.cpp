/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <game/mapitems.h>

#include <game/server/entities/character.h>
#include <game/server/entities/flag.h>
#include <game/server/player.h>
#include <game/server/gamecontext.h>
#include "carry.h"

CGameControllerCARRY::CGameControllerCARRY(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	// Exchange this to a string that identifies your game mode.
	// DM, TDM and CTF are reserved for teeworlds original modes.
	m_F=0;
	m_pGameType = "carry";
	m_GameFlags = GAMEFLAG_FLAGS;
	//m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

bool CGameControllerCARRY::OnEntity(int Index, vec2 Pos)
{

	if(IGameController::OnEntity(Index,Pos))
		return true;
	if(m_F)
		return false;
	if(Index == ENTITY_FLAGSTAND_RED)
	{
		CFlag *F = new CFlag(&GameServer()->m_World, TEAM_RED);
		F->m_StandPos = Pos;
		F->m_Pos = Pos;
		m_F = F;
		GameServer()->m_World.InsertEntity(F);
		return true;
	}
	return false;
}

void CGameControllerCARRY::Tick()
{
	IGameController::Tick();
	
	
	if(GameServer()->m_World.m_ResetRequested || GameServer()->m_World.m_Paused)
		return;
	
	CFlag *F = m_F;
	
	if(!F)
		return;
	
	//flag drops
	if(GameServer()->Collision()->GetCollisionAt(F->m_Pos.x, F->m_Pos.y)&CCollision::COLFLAG_DEATH || F->GameLayerClipped(F->m_Pos))
	{
		GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", "flag_return");
		GameServer()->CreateSoundGlobal(SOUND_CTF_RETURN);
		F->Reset();
		return;
	}
	if(F->m_pCarryingCharacter)
	{
		// update flag position
		F->m_Pos = F->m_pCarryingCharacter->m_Pos;
		//score
		if (Server()->Tick() % (Server()->TickSpeed() * 1) == 0) F->m_pCarryingCharacter->GetPlayer()->m_Score++;

	}
	else
	{
		CCharacter *apCloseCCharacters[MAX_CLIENTS];
		int Num = GameServer()->m_World.FindEntities(F->m_Pos, CFlag::ms_PhysSize, (CEntity**)apCloseCCharacters, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
		for(int i = 0; i < Num; i++)
		{
			if(!apCloseCCharacters[i]->IsAlive() || apCloseCCharacters[i]->GetPlayer()->GetTeam() == TEAM_SPECTATORS || GameServer()->Collision()->IntersectLine(F->m_Pos, apCloseCCharacters[i]->m_Pos, NULL, NULL))
				continue;

			// take the flag
			if(F->m_AtStand)
			{
				F->m_GrabTick = Server()->Tick();
			}

			F->m_AtStand = 0;
			F->m_pCarryingCharacter = apCloseCCharacters[i];

			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "flag_grab player='%d:%s'",
				F->m_pCarryingCharacter->GetPlayer()->GetCID(),
				Server()->ClientName(F->m_pCarryingCharacter->GetPlayer()->GetCID()));
			GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

			for(int c = 0; c < MAX_CLIENTS; c++)
			{
				CPlayer *pPlayer = GameServer()->m_apPlayers[c];
				if(!pPlayer)
					continue;
				else
					GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_PL, c);
			}
			// demo record entry
			GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_EN, -2);
			break;
		}

		if(!F->m_pCarryingCharacter && !F->m_AtStand)
		{
			if(Server()->Tick() > F->m_DropTick + Server()->TickSpeed()*30)
			{
				GameServer()->CreateSoundGlobal(SOUND_CTF_RETURN);
				F->Reset();
			}
			else
			{
				F->m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
				GameServer()->Collision()->MoveBox(&F->m_Pos, &F->m_Vel, vec2(F->ms_PhysSize, F->ms_PhysSize), 0.5f);
			}
		}
	}
}


int CGameControllerCARRY::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int WeaponID)
{
	IGameController::OnCharacterDeath(pVictim, pKiller, WeaponID);
	int HadFlag = 0;

	// drop flags
	CFlag *F = m_F;
	if(F && pKiller && pKiller->GetCharacter() && F->m_pCarryingCharacter == pKiller->GetCharacter())
		HadFlag |= 2;
	if(F && F->m_pCarryingCharacter == pVictim)
	{
		GameServer()->CreateSoundGlobal(SOUND_CTF_DROP);
		F->m_DropTick = Server()->Tick();
		F->m_pCarryingCharacter = 0;
		F->m_Vel = vec2(0,0);

		HadFlag |= 1;
	}

	return HadFlag;
}
void CGameControllerCARRY::Snap(int SnappingClient)
{
	IGameController::Snap(SnappingClient);

	CNetObj_GameData *pGameDataObj = (CNetObj_GameData *)Server()->SnapNewItem(NETOBJTYPE_GAMEDATA, 0, sizeof(CNetObj_GameData));
	if(!pGameDataObj)
		return;

	if(m_F)
	{
		if(m_F->m_AtStand)
			pGameDataObj->m_FlagCarrierRed = FLAG_ATSTAND;
		else if(m_F->m_pCarryingCharacter && m_F->m_pCarryingCharacter->GetPlayer())
			pGameDataObj->m_FlagCarrierRed = m_F->m_pCarryingCharacter->GetPlayer()->GetCID();
		else
			pGameDataObj->m_FlagCarrierRed = FLAG_TAKEN;
	}
	else
		pGameDataObj->m_FlagCarrierRed = FLAG_MISSING;
}
void CGameControllerCARRY::OnCharacterSpawn(class CCharacter *pChr)
{
	IGameController::OnCharacterSpawn(pChr);
}