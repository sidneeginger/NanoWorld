#pragma once
#include "GLApp.h"
#include "Player.h"
#include "World.h"
#include "../nwlink/WorldLink.h"
#include <unordered_map>

typedef std::unordered_map<uint32, Player*> PlayerMap;

class NWApp : public GLApp
{
public:
	NWApp();
	virtual ~NWApp();

private:
	Player m_player;
	World m_world;
	CWorldLink* m_plink;
	boost::asio::io_service* m_pIo;
	std::vector<std::thread> threadPool;
	PlayerMap m_otherPlayers;

protected:
	virtual void Draw();
	virtual void CalcActor();
	virtual void CheckInput();
	virtual int Init();
	virtual int Finish();

private:
	void DrawWorld();
	void UpdateObjects(ObjectList& list);
};

