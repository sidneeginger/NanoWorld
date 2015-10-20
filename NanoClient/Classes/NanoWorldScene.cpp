#include "NanoWorldScene.h"

USING_NS_CC;

Vec3 camera_offset(0, 45, 60);
#define PLAYER_HEIGHT 0

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
	}
	break;
	case PLAYER_STATE_RIGHT:
	{
		player->setRotation3D(player->getRotation3D() + Vec3(0, -25 * dt, 0));
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
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(NanoWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}


void NanoWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
