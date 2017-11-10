#include <include/components/Comp_animation.h>


AnimationComponent::AnimationComponent(sf::Vector2u imageCount, float switchTime) {
    this->imageCounter = imageCount;
    this->switchTime = switchTime;
    totalTime = 0.0f;
    spriteRect.width = pixleWidth;
    spriteRect.height = pixleHeight;
}

AnimationComponent::~AnimationComponent() {}

void AnimationComponent::walk(int row, float deltaTime, bool faceRight, bool faceUp, sf::Vector2f speed) {
    currentImage.y = row;

    bool isMooving = false;

    if (speed.x != 0)
        isMooving = true;
    if (speed.y != 0){
        isMooving = true;
    }

    if (!isMooving && !idle){
        idle = true;
        currentImage.x = 0;
    }
    else if(isMooving && idle){
        idle = false;
        currentImage.x = 4;
    }

    totalTime += deltaTime;

    if (totalTime >= switchTime) {
        totalTime -= switchTime;
        currentImage.x++;

        if (idle) {
            if (currentImage.x >= 3)
                currentImage.x = 0;

        } else if (!idle) {
            if (currentImage.x >= 11)
                currentImage.x = 3;

        }
    }

    spriteRect.top = currentImage.y * spriteRect.height;
/*
    if (!faceRight) {
        spriteRect.left = (currentImage.x + 1) * abs(spriteRect.width);
    }

    if(faceRight) {
        spriteRect.left = currentImage.x * abs(spriteRect.width);
    }

    if (!faceUp) {
        spriteRect.left = (currentImage.x + 1) * abs(spriteRect.width);
    }
    if (faceUp) {
        spriteRect.left = (currentImage.x + 1) * abs(spriteRect.width);
    }
    */
}