#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    // 初期化に失敗したら
    if(!Layer::init()) return false;
    
    // タッチイベントを登録
    EventListenerTouchOneByOne* listener { EventListenerTouchOneByOne::create() };
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    // 物理空間の初期化
    this->createPhysicsWorld();
    
    // 粒子のシステムを設定
    this->setUpParticleSystem();
    
    // 更新処理をスケジュール
    this->scheduleUpdate();
    
    return true;
}

void HelloWorld::update(float dt)
{
    // 端末のフレームにかかわらず60fpsでの計算の値で処理をさせる
    this->world->Step(1/60.f, 1, 1, 1);
    
    // 各パーティクルのデータから画像を更新する
    for (b2ParticleGroup* group : this->particleGroups)
    {
        
        //グループのパーティクルを更新
        void ** userData { particleSystem->GetUserDataBuffer() + group->GetBufferIndex() };
        
        //座標バッファ
        b2Vec2* vecList { particleSystem->GetPositionBuffer() + group->GetBufferIndex() };
        
        //色バッファ
        b2ParticleColor * colorList { particleSystem->GetColorBuffer() + group->GetBufferIndex() };
        
        //バッファループ
        for(int i = 0; i < group->GetParticleCount();i++,vecList++,userData++)
        {
            // 位置を更新
            if(Sprite* particleImage { static_cast<Sprite*>(*userData) })
            {
                particleImage->setColor(Color3B((*colorList).r, (*colorList).g, (*colorList).b));
                particleImage->setPosition((*vecList).x * PTM_RATIO, (*vecList).y*PTM_RATIO);
            }
        }
    }

}

// タッチされたら
bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
    
    const Point& position { touch->getLocation() };
    this->createParticle(position);
    return true;
}

// タッチが動いたら
void HelloWorld::onTouchMoved(Touch *touch, Event *event)
{
    
}

// タッチが話されたら
void HelloWorld::onTouchEnded(Touch *touch, Event *event)
{
}

void HelloWorld::setUpParticleSystem()
{
    // パーティクルシステムの構築
    b2ParticleSystemDef particleSystemDef;
    particleSystemDef.density = 0.5;
    particleSystemDef.radius = 10.0f / PTM_RATIO;
    particleSystemDef.gravityScale = 3.0f;
    this->particleSystem = world->CreateParticleSystem(&particleSystemDef);
}

// 粒子の生成
void HelloWorld::createParticle(const Point& position)
{
    //グループで生成する際はパーティクルグループの形を設定する必要がある
    b2CircleShape* shape { new b2CircleShape() };
    shape->m_radius = 50.0f / PTM_RATIO;
    
    //パーティクルグループのデータ設定
    b2ParticleGroupDef groupDef;
    
    //パーティクルのグループを設定
    groupDef.shape = shape;
    groupDef.flags = b2_waterParticle | b2_particleContactListenerParticle;
    groupDef.color = b2ParticleColor(137, 224, 253, 255);
    groupDef.position.Set(position.x/PTM_RATIO,position.y/PTM_RATIO);

    // グループを作成
    b2ParticleGroup* particleGroup { this->particleSystem->CreateParticleGroup(groupDef) };
 
    // 管理配列に追加
    this->particleGroups.push_back(particleGroup);

    void ** userData { particleSystem->GetUserDataBuffer() + particleGroup->GetBufferIndex() };

    for(int i = 0; i < particleGroup->GetParticleCount();i++,userData++)
    {
        Sprite * water { Sprite::create("particle.png") };
        water->getTexture()->setAliasTexParameters();
        water->setScale((this->particleSystem->GetRadius() / (water->getContentSize().width / PTM_RATIO)));
        (*userData) = water;
        this->addChild(water);
    }
}


// 物理空間の初期化
void HelloWorld::createPhysicsWorld()
{
    // 重力ベクトル
    b2Vec2 gravity {0,-9.8f};

    // ワールドを重力を引数として初期化
    b2World* world { new b2World(gravity) };
    this->world = world;
    
    world->SetAllowSleeping(true);  // 動いていないオブジェクトは計算しないフラグを立てる(パフォーマンスのため)
    world->SetContinuousPhysics(true);
    
    // 壁を作る
    const Size& size { this->getContentSize() };
    
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);   // 画面の左下に基準点を置く
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    b2EdgeShape groundBox;
    
    // 天井
    groundBox.Set(b2Vec2{0,size.height/PTM_RATIO}, b2Vec2{size.width/PTM_RATIO,size.height/PTM_RATIO});
    groundBody->CreateFixture(&groundBox,0);
    
    // 左壁
    groundBox.Set(b2Vec2{0,size.height/PTM_RATIO}, b2Vec2{0,0});
    groundBody->CreateFixture(&groundBox,0);
    
    // 右壁
    groundBox.Set(b2Vec2{size.width/PTM_RATIO,size.height/PTM_RATIO}, b2Vec2{size.width/PTM_RATIO,0});
    groundBody->CreateFixture(&groundBox,0);
    
    // 地面
    groundBox.Set(b2Vec2(0,0), b2Vec2{size.width/PTM_RATIO,0});
    groundBody->CreateFixture(&groundBox,0);
}