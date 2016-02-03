/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_DOM_H
#define GAME_SERVER_GAMEMODES_DOM_H
#include <game/server/gamecontroller.h>
#include <game/server/entity.h>

// you can subclass GAMECONTROLLER_CTF, GAMECONTROLLER_TDM etc if you want
// todo a modification with their base as well.
class CGameControllerDOM : public IGameController
{
public:
	class CCharacter *m_apFlags[64];
	CGameControllerDOM(class CGameContext *pGameServer);
	//virtual void Tick();
	virtual bool OnEntity(int Index, vec2 Pos);
	//virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	// add more virtual functions here if you wish
private:
	int m_NumFlags;
};


#endif