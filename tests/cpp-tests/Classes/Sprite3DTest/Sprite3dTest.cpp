
#include "Sprite3dTest.h"

void Sprite3dTestScene::runThisTest()
{
    auto layer = LayerColor::create(Color4B(80,95,203,255));
    addChild(layer);
    Director::getInstance()->replaceScene(this);
    auto model = Sprite3D::create("sprite3d/boss.obj","sprite3d/boss.png");
    model->setPosition(Vector2(200,200));
    model->setScale(20);

    model->runAction(RepeatForever::create(RotateBy::create(1, 40)));
    layer->addChild(model);
}