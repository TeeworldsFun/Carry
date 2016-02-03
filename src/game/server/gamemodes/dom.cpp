/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <game/mapitems.h>

#include <game/server/entities/character.h>
#include <game/server/entities/flag.h>
#include <game/server/player.h>
#include <game/server/gamecontext.h>
#include "dom.h"

CGameControllerDOM::CGameControllerDOM(CGameContext *pGameServer) : IGameController(pGameServer)
{
	m_pGameType = "DOM";
	m_GameFlags = GAMEFLAG_TEAMS;
	for(int i = 0; i < 64; i++)
		m_apFlags[i]=0;
	m_NumFlags = 0;
}

bool CGameControllerDOM::OnEntity(int Index, vec2 Pos)
{
	if(Index < ENTITY_ARMOR_1 || Index > ENTITY_WEAPON_RIFLE || g_Config.m_SvInsta == 0)
	{
		if(IGameController::OnEntity(Index,Pos))
			return true;
	}
	return false;
}
