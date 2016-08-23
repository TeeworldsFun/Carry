/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_CARRY_H
#define GAME_SERVER_GAMEMODES_CARRY_H
#include <game/server/gamecontroller.h>
#include <game/server/entity.h>

// you can subclass GAMECONTROLLER_CTF, GAMECONTROLLER_TDM etc if you want
// todo a modification with their base as well.
class CGameControllerCARRY : public IGameController
{
public:
	virtual void Snap(int SnappingClient);
	CGameControllerCARRY(class CGameContext *pGameServer);
	virtual void Tick();
	virtual bool OnEntity(int Index, vec2 Pos);
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	virtual void OnCharacterSpawn(class CCharacter *pChr);
	virtual int GetTeleport();
	// add more virtual functions here if you wish
	class CFlag *m_F;	
	vec2 FlagSpawns[256];
	int m_FlagStandCount;
};


#endif
