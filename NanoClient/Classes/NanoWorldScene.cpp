#include "NanoWorldScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Vec3 camera_offset(0, 45, 60);
#define PLAYER_HEIGHT 0

enum GAME_SCENE {
	SCENE_UI = 0,
	SCENE_WORLD,
	SCENE_DIALOG,
	SCENE_OSD,
	SCENE_COUNT,
};

enum SCENE_LAYER {
	LAYER_BACKGROUND = 0,
	LAYER_DEFAULT,
	LAYER_MIDDLE,
	LAYER_TOP,
	LAYER_COUNT,
};

static CameraFlag s_CF[LAYER_COUNT] =
{
	CameraFlag::USER1,      //  LAYER_BACKGROUND
	CameraFlag::DEFAULT,    //  LAYER_DEFAULT
	CameraFlag::USER3,      //  LAYER_MIDDLE
	CameraFlag::USER4,      //  LAYER_TOP
};

static unsigned short s_CM[LAYER_COUNT] =
{
	(unsigned short)s_CF[0],
	(unsigned short)s_CF[1],
	(unsigned short)s_CF[2],
	(unsigned short)s_CF[3],
};

static const char * s_CameraNames[CAMERA_COUNT] =
{
	"World 3D Skybox",
	"World 3D Scene",
	"UI 2D",
	"Dialog 2D Base",
	"Dialog 3D Model",
	"Dialog 2D Above",
	"OSD 2D Base",
	"OSD 3D Model",
	"OSD 2D Above"
};

static Vec3 s_scenePositons[SCENE_COUNT] = {
	Vec3(0, 0, 0),          //  center  :   UI scene
	Vec3(0, 10000, 0),      //  top     :   World sub scene
	Vec3(10000, 0, 0),      //  right   :   Dialog sub scene
	Vec3(0, -10000, 0),     //  bottom  :   OSD sub scene
};

//////////////////////////////////////////////////////////////////////////


bool Player::isDone() const
{
	return false;
}


void Player::update(float dt)
{
	auto player = (Sprite3D *)this;
	switch (_playerState)
	{
	case PLAYER_STATE_IDLE:
		break;
	case PLAYER_STATE_FORWARD:
	{
		Vec3 curPos = player->getPosition3D();
		Vec3 newFaceDir = _targetPos - curPos;
		newFaceDir.y = 0.0f;
		newFaceDir.normalize();
		Vec3 offset = newFaceDir * 25.0f * dt;
		curPos += offset;
		player->setPosition3D(curPos);
	}
	break;
	case PLAYER_STATE_BACKWARD:
	{
		Vec3 forward_vec;
		player->getNodeToWorldTransform().getForwardVector(&forward_vec);
		forward_vec.normalize();
		player->setPosition3D(player->getPosition3D() - forward_vec * 15 * dt);
	}
	break;
	case PLAYER_STATE_LEFT:
	{
		player->setRotation3D(player->getRotation3D() + Vec3(0, 25 * dt, 0));
		_headingAngle += 25 * dt;
	}
	break;
	case PLAYER_STATE_RIGHT:
	{
		player->setRotation3D(player->getRotation3D() + Vec3(0, -25 * dt, 0));
		_headingAngle -= 25 * dt;
	}
	break;
	default:
		break;
	}
	// transform player position to world coord
	auto playerPos = player->getPosition3D();
	auto playerModelMat = player->getParent()->getNodeToWorldTransform();
	playerModelMat.transformPoint(&playerPos);
	Vec3 Normal;
	float player_h = _terrain->getHeight(playerPos.x, playerPos.z, &Normal);
	if (Normal.isZero())//check the player whether is out of the terrain
	{
		player_h = playerPos.y;
	}
	else
	{
		player_h += PLAYER_HEIGHT;
	}
	player->setPositionY(player_h);
	Quaternion q2;
	q2.createFromAxisAngle(Vec3(0, 1, 0), (float)-M_PI, &q2);

	Quaternion headingQ;
	headingQ.createFromAxisAngle(_headingAxis, _headingAngle, &headingQ);
	player->setRotationQuat(headingQ*q2);
	auto vec_offset = Vec4(camera_offset.x, camera_offset.y, camera_offset.z, 1);
	vec_offset = player->getNodeToWorldTransform()*vec_offset;
	_cam->setPosition3D(player->getPosition3D() + camera_offset);
	updateState();
}

void Player::turnLeft()
{
	_playerState = PLAYER_STATE_LEFT;
}

void Player::turnRight()
{
	_playerState = PLAYER_STATE_RIGHT;
}

void Player::idle()
{
	_playerState = PLAYER_STATE_IDLE;
}


void Player::forward()
{
	_playerState = PLAYER_STATE_FORWARD;
}

void Player::backward()
{
	_playerState = PLAYER_STATE_BACKWARD;
}

void Player::updateState()
{
	auto player = (Sprite3D *)this;
	switch (_playerState)
	{
	case PLAYER_STATE_FORWARD:
	{
		Vec2 player_pos = Vec2(player->getPositionX(), player->getPositionZ());
		Vec2 targetPos = Vec2(_targetPos.x, _targetPos.z);
		auto dist = player_pos.distance(targetPos);
		if (dist < 1)
		{
			_playerState = PLAYER_STATE_IDLE;
		}
	}
	break;
	default:
		break;
	}
}

Player * Player::create(const char * file, Camera * cam, Terrain * terrain)
{
	//
	auto sprite = new (std::nothrow) Player();
	if (sprite && sprite->initWithFile(file))
	{
		sprite->_headingAngle = 0;
		sprite->_playerState = PLAYER_STATE_IDLE;
		sprite->_cam = cam;
		sprite->_terrain = terrain;
		sprite->autorelease();
		sprite->scheduleUpdate();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

Scene* NanoWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = NanoWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool NanoWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	// prepare for camera creation, we need several custom cameras
	_gameCameras.resize(CAMERA_COUNT);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Camera *ca = nullptr;   // temp variable

	////////////////////////////////////////////////////////////////////////
	// create world 3D scene, this scene has two camera
	_worldScene = Node::create();
	// create a camera to look the skybox
	ca = _gameCameras[CAMERA_WORLD_3D_SKYBOX] =
		Camera::createPerspective(60,
			visibleSize.width / visibleSize.height,
			10,
			1000);
	ca->setDepth(CAMERA_WORLD_3D_SKYBOX);
	ca->setName(s_CameraNames[CAMERA_WORLD_3D_SKYBOX]);
	ca->setCameraFlag(s_CF[LAYER_BACKGROUND]);
	ca->setPosition3D(Vec3(0.f, 0.f, 50.f));
	_worldScene->addChild(ca);
	// create a camera to look the 3D models in world 3D scene
	ca = _gameCameras[CAMERA_WORLD_3D_SCENE] =
		Camera::createPerspective(60,
			visibleSize.width / visibleSize.height,
			0.1,
			200);
	ca->setDepth(CAMERA_WORLD_3D_SCENE);
	ca->setName(s_CameraNames[CAMERA_WORLD_3D_SCENE]);
	_worldScene->addChild(ca);
	// create 3D objects and add to world scene
	createWorld3D();
	_worldScene->addChild(_skyBox);
	_worldScene->addChild(_terrain);
	_worldScene->addChild(_player);
	_worldScene->addChild(_monsters[0]);
	_worldScene->addChild(_monsters[1]);
	// move camera above player
	ca->setPosition3D(_player->getPosition3D() + Vec3(0, 45, 60));
	ca->setRotation3D(Vec3(-30, 0, 0));
	_worldScene->setPosition3D(s_scenePositons[SCENE_WORLD]);
	this->addChild(_worldScene);

	//auto listener = EventListenerTouchOneByOne::create();
	auto lsnkeyborad = EventListenerKeyboard::create();
	auto lsnMouse = EventListenerMouse::create();

	lsnkeyborad->onKeyPressed = CC_CALLBACK_2(NanoWorld::onKeyPressed, this);
	lsnkeyborad->onKeyReleased = CC_CALLBACK_2(NanoWorld::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(lsnkeyborad, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(lsnMouse, this);
    
    return true;
}

void NanoWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		_player->idle();
		break;
	default:
		break;
	}

	event->stopPropagation();
}

void NanoWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		_player->turnLeft();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		_player->turnRight();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_E:
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		_player->backward();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		_player->forward();
		break;
	default:
		break;
	}

	event->stopPropagation();
}


void NanoWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void NanoWorld::createWorld3D()
{
	// create skybox
	//create and set our custom shader
	auto shader = GLProgram::createWithFilenames("Sprite3D/cube_map.vert",
		"Sprite3D/cube_map.frag");
	auto state = GLProgramState::create(shader);

	// create the second texture for cylinder
	_textureCube = TextureCube::create("Sprite3D/skybox/left.jpg",
		"Sprite3D/skybox/right.jpg",
		"Sprite3D/skybox/top.jpg",
		"Sprite3D/skybox/bottom.jpg",
		"Sprite3D/skybox/front.jpg",
		"Sprite3D/skybox/back.jpg");
	//set texture parameters
	Texture2D::TexParams tRepeatParams;
	tRepeatParams.magFilter = GL_LINEAR;
	tRepeatParams.minFilter = GL_LINEAR;
	tRepeatParams.wrapS = GL_MIRRORED_REPEAT;
	tRepeatParams.wrapT = GL_MIRRORED_REPEAT;
	_textureCube->setTexParameters(tRepeatParams);

	// pass the texture sampler to our custom shader
	state->setUniformTexture("u_cubeTex", _textureCube);

	// add skybox
	_skyBox = Skybox::create();
	_skyBox->setCameraMask(s_CM[LAYER_BACKGROUND]);
	_skyBox->setTexture(_textureCube);
	_skyBox->setScale(700.f);

	// create terrain
	Terrain::DetailMap r("Terrain/dirt.jpg");
	Terrain::DetailMap g("Terrain/Grass2.jpg", 10);
	Terrain::DetailMap b("Terrain/road.jpg");
	Terrain::DetailMap a("Terrain/GreenSkin.jpg", 20);
	Terrain::TerrainData data("Terrain/heightmap16.jpg",
		"Terrain/alphamap.png",
		r, g, b, a, Size(32, 32), 40.0f, 2);
	_terrain = Terrain::create(data, Terrain::CrackFixedType::SKIRT);
	_terrain->setMaxDetailMapAmount(4);
	_terrain->setDrawWire(false);

	_terrain->setSkirtHeightRatio(3);
	_terrain->setLODDistance(64, 128, 192);

	// create player
	_player = Player::create("Sprite3D/girl.c3b",
		_gameCameras[CAMERA_WORLD_3D_SCENE],
		_terrain);
	_player->setScale(0.2);
	_player->setPositionY(_terrain->getHeight(_player->getPositionX(),
		_player->getPositionZ()));

	auto animation = Animation3D::create("Sprite3D/girl.c3b", "Take 001");
	if (animation)
	{
		auto animate = Animate3D::create(animation);
		_player->runAction(RepeatForever::create(animate));
	}
	// add a particle 3d above player
	auto rootps =
		PUParticleSystem3D::create("Particle3D/scripts/blackHole.pu",
			"Particle3D/materials/pu_mediapack_01.material");
	rootps->setScale(2);
	rootps->setPosition3D(Vec3(0, 150, 0));
	auto moveby = MoveBy::create(2.0f, Vec2(50.0f, 0.0f));
	auto moveby1 = MoveBy::create(2.0f, Vec2(-50.0f, 0.0f));
	rootps->runAction(RepeatForever::create(Sequence::create(moveby, moveby1, nullptr)));
	rootps->startParticleSystem();

	_player->addChild(rootps, 0);

	// add BillBoard for test blend
	//auto billboard = BillBoard::create("Images/btn-play-normal.png");
	//billboard->setPosition3D(Vec3(0, 180, 0));
	//_player->addChild(billboard);

	// create two Sprite3D monster, one is transparent
	auto monster = Sprite3D::create("Sprite3D/orc.c3b");
	monster->setRotation3D(Vec3(0, 180, 0));
	monster->setPosition3D(_player->getPosition3D() + Vec3(50, -10, 0));
	monster->setOpacity(128);
	_monsters[0] = monster;
	monster = Sprite3D::create("Sprite3D/orc.c3b");
	monster->setRotation3D(Vec3(0, 180, 0));
	monster->setPosition3D(_player->getPosition3D() + Vec3(-50, -5, 0));
	_monsters[1] = monster;
}