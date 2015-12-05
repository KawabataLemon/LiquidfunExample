#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

class HelloWorld : public cocos2d::Layer
{
// 定数
public:
    static constexpr float PTM_RATIO { 32.0f }; // Box2D上で1mを何ピクセルとして扱うか
    
// クラスメソッド
public:
    CREATE_FUNC(HelloWorld);
    static cocos2d::Scene* createScene();

// インスタンス変数
private:
    b2World* world { nullptr }; // 物理空間
    b2ParticleSystem* particleSystem { nullptr }; // 粒子軍の設定
    std::vector<b2ParticleGroup*> particleGroups {};// 粒子の配列　
    
// インスタンスメソッド
public:
    bool init() override;    // 初期化
    void update(float dt) override; // 更新処理
    
    // タッチイベント
    bool onTouchBegan(Touch* touch,Event* event) override;
    void onTouchMoved(Touch* touch,Event* event) override;
    void onTouchEnded(Touch* touch,Event* event) override;
    
    void setUpParticleSystem();  // 生成するパーティクルに関するのシステムを設定
    void createParticle(const Point& position);  // 粒子群を生成
    void createPhysicsWorld();         // 物理空間を構築する
};

#endif // __HELLOWORLD_SCENE_H__
