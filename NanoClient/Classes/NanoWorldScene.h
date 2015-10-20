#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "3d/CCSprite3D.h"
#include "3d/CCTerrain.h"
#include "2d/CCCamera.h"
#include "2d/CCAction.h"
#include "renderer/CCRenderState.h"
#include <spine/spine-cocos2dx.h>

USING_NS_CC;
using namespace spine;

#define PLAYER_STATE_LEFT 0 
#define PLAYER_STATE_RIGHT 1
#define PLAYER_STATE_IDLE 2
#define PLAYER_STATE_FORWARD 3
#define PLAYER_STATE_BACKWARD 4

class Player : public cocos2d::Sprite3D
{
public:
	static Player * create(const char * file, cocos2d::Camera*  cam, cocos2d::Terrain* terrain);
	virtual bool isDone() const;
	virtual void update(float dt);

	void turnLeft();
	void turnRight();
	void forward();
	void backward();
	void idle();
	cocos2d::Vec3 _targetPos;
	void updateState();
	float _headingAngle;
	cocos2d::Vec3 _headingAxis;
private:
	cocos2d::Terrain* _terrain;
	cocos2d::Camera*  _cam;
	int _playerState;
};

class NanoWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(NanoWorld);

private:
	std::vector<Camera *> _gameCameras;
	Node* _worldScene;
	Node* _dlgScene;
	Node* _osdScene;

	// init in createWorld3D()
	TextureCube*        _textureCube;
	Skybox*             _skyBox;
	cocos2d::Terrain*   _terrain;
	Player *            _player;
	Node*               _monsters[2];

	// init in createUI()
	Node* _playerItem;
	Node* _detailItem;
	Node* _descItem;
	Node* _ui;

	// init in createPlayerDlg()
	Node* _playerDlg;
	// init in createDetailDlg()
	Node* _detailDlg;
	// init in createDescDlg()
	Node* _descDlg;
};

#endif // __HELLOWORLD_SCENE_H__
