
#include "Sprite3dTest.h"

void Sprite3dTestScene::runThisTest()
{
    auto layer = LayerColor::create(Color4B(80,95,203,255));
    addChild(layer);
    Director::getInstance()->replaceScene(this);
}