#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <include/systems/sort_key_update.h>

#include "systems/resolve_movment.h"
#include "systems/drawEntety.h"
#include "systems/attack.h"
#include "systems/mouse_clicked.h"
#include "collections/setUpCreature.h"
#include "collections/addDialoge.h"
#include "states/state_game.h"
#include "map/map.h"
#include "util/utility.h"

StateGame::StateGame(StateStack &stack, StateBase::Context context)
        : StateBase(stack, context)
        , playerCam()
{
    isMovingUp = false;
    isMovingDown = false;
    isMovingLeft = false;
    isMovingRight = false;

    anax::World& world = *getContext().world;
    playerCam.setSize(1920, 1080);
    playerCam.zoom(0.3f);
    player = world.createEntity();

    // Load map information from JSON into object list
    if (!Map::load("assets/map/map.json", objects, context))
        std::runtime_error("StateGame::StateGame - Failed to load map data.");

    sf::Texture& Herobody = context.textures->get(Textures::Hero);
    sf::Texture& GoblinTexture = context.textures->get(Textures::Goblin);
    sf::Texture& TraderTexture = context.textures->get(Textures::Trader);
    sf::Texture& HeroHead = context.textures->get(Textures::HeroHead);
    sf::Texture& HeroWeapon = context.textures->get(Textures::HeroWeapon);
    sf::Texture& HeroShield = context.textures->get(Textures::HeroShield);
    player.addComponent<TextureComponent>();
    TextureComponent& textureComponent = player.getComponent<TextureComponent>();
    textureComponent.texture[0] = Herobody;
    textureComponent.sprite[0].setTexture(textureComponent.texture[0]);
    textureComponent.texture[1] = HeroHead;
    textureComponent.sprite[1].setTexture(textureComponent.texture[1]);
    textureComponent.texture[2] = HeroWeapon;
    textureComponent.sprite[2].setTexture(textureComponent.texture[2]);
    textureComponent.texture[3] = HeroShield;
    textureComponent.sprite[3].setTexture(textureComponent.texture[3]);


    sf::RenderWindow& window = *getContext().window;
    DrawEntetys drawEntetys;


    anax::Entity goblin = world.createEntity();
    anax::Entity goblin2 = world.createEntity();
    anax::Entity goblin3 = world.createEntity();
    anax::Entity goblin4 = world.createEntity();
    anax::Entity trader = world.createEntity();

    SetUpCreature creatureSetup;

    creatureSetup.setUpPlayer(player,window);
    creatureSetup.setUpEnemie(goblin,  GoblinTexture, *getContext().window, 200, 200, "Hard");
    creatureSetup.setUpEnemie(goblin2, GoblinTexture, *getContext().window ,100 ,100, "Medium");
    creatureSetup.setUpEnemie(goblin3, GoblinTexture, *getContext().window ,400 ,200, "Easy");
    creatureSetup.setUpEnemie(goblin4, GoblinTexture, *getContext().window ,300 ,100, "Hard");
    creatureSetup.setUpNPC(trader,TraderTexture,*getContext().window,300,300);

    sf::Font& font = context.fonts->get(Fonts::Main);
    playerGold.setFont(font);
    playerGold.setScale(0.3,0.3);
    sf::Color gold(255,215,0);
    playerGold.setFillColor(gold);

    AddDialoge addDialoge;
    addDialoge.addDialoge(trader,"assets/dialog/trader_dialog_1.txt");
    context.music->play(Music::Test);
}

void StateGame::draw()
{
    sf::RenderWindow& window = *getContext().window;
    window.setView(playerCam);

    anax::World& world = *getContext().world;
    DrawEntetys drawEntetys;

    drawEntetys.draw(window,world, "Game");
    Looteble looteble = player.getComponent<Looteble>();
    sf::Vector2f viewCenter = window.getView().getCenter();
    sf::Vector2f halfExtents = window.getView().getSize() / 2.0f;
    sf::Vector2f translation = viewCenter - halfExtents;

    int mX = static_cast<int>(translation.x);
    int mY = static_cast<int>(translation.y);

    playerGold.setPosition(mX +5 ,mY +5);
    playerGold.setString(std::to_string(looteble.gold));
    window.draw(playerGold);
}

bool StateGame::update(sf::Time dt)
{
    AnimationComponent& animationComponent = player.getComponent<AnimationComponent>();
    ResolveMovment resolve;
    anax::World& world = *getContext().world;

    if(animationComponent.idleTimer.getElapsedTime().asSeconds() >= 0.2f && animationComponent.idle == false) {
        if(animationComponent.direction != "Idle" )animationComponent.changedDirection = true;
        animationComponent.direction = "Idle";
        float deltaTime = animationComponent.animationClock.restart().asSeconds();
        resolve.resolveMovment(world);

        PositionComponent& positionComponent = player.getComponent<PositionComponent>();
        Movable& moveble = player.getComponent<Movable>();
    }
    animationComponent.animationClock.restart().asSeconds();

    PositionComponent& positionComponent = player.getComponent<PositionComponent>();
    playerCam.setCenter(positionComponent.XPos, positionComponent.YPos);

    // Update the sort key for movable entities
    SortKeyUpdate sortKeyUpdate;
    sortKeyUpdate.Update(*getContext().world);

    return true;
}

bool StateGame::handleEvent(const sf::Event &event)
{
    if(event.type == sf::Event::MouseButtonPressed)
    {
        anax::World& world = *getContext().world;
        sf::RenderWindow& window = *getContext().window;
        MouseClicked mouseClicked;
        mouseClicked.Clicked(world, player, window, playerCam);

    }

    switch (event.type)
    {
        case sf::Event::KeyPressed:
            handleUserInput(event.key.code, true);
            break;
        case sf::Event::KeyReleased:
            handleUserInput(event.key.code, false);
            break;
    }

    return true;
}

void StateGame::handleUserInput(sf::Keyboard::Key key, bool isPressed)
{
    ResolveMovment resolve;
    anax::World& world = *getContext().world;
    AnimationComponent& animationComponent = player.getComponent<AnimationComponent>();

    float speedIdle= 1.5f;

    //animationComponent.animationClock.restart().asSeconds();

    if (key == sf::Keyboard::W || key == sf::Keyboard::Up)
    {
        animationComponent.idleTimer.restart().asSeconds();
        animationComponent.idle = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            animationComponent.movementDirection.x -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.movementDirection.y -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowNorthWest;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos -= movable.speed / speedIdle;
            positionComponent.SpriteLeft -= movable.speed / speedIdle;
            positionComponent.YPos -= movable.speed / speedIdle;
            positionComponent.SpriteTop -= movable.speed / speedIdle;

        }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            animationComponent.movementDirection.x += animationComponent.movementSpeed*animationComponent.deltaTime;
            animationComponent.movementDirection.y -= animationComponent.movementSpeed*animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowNorthEast;
            resolve.resolveMovment(world);

            PositionComponent& positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos += movable.speed / speedIdle;
            positionComponent.SpriteLeft += movable.speed / speedIdle;
            positionComponent.YPos -= movable.speed / speedIdle;
            positionComponent.SpriteTop -= movable.speed / speedIdle;

        } else {
            if (animationComponent.direction != "Up")animationComponent.changedDirection = true;
            animationComponent.direction = "Up";
            animationComponent.movementDirection.y -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowNorth;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.YPos -= movable.speed;
            positionComponent.SpriteTop -= movable.speed;
        }
    }

    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
        animationComponent.idleTimer.restart().asSeconds();
        animationComponent.idle = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            animationComponent.movementDirection.x -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.movementDirection.y += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowSouthWest;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos -= movable.speed / speedIdle;
            positionComponent.SpriteLeft -= movable.speed / speedIdle;
            positionComponent.YPos += movable.speed / speedIdle;
            positionComponent.SpriteTop += movable.speed / speedIdle;

        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            animationComponent.movementDirection.x -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.movementDirection.y += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowSouthEast;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos += movable.speed / speedIdle;
            positionComponent.SpriteLeft += movable.speed / speedIdle;
            positionComponent.YPos += movable.speed / speedIdle;
            positionComponent.SpriteTop += movable.speed / speedIdle;

        } else {
            if (animationComponent.direction != "Down")animationComponent.changedDirection = true;
            animationComponent.direction = "Down";
            animationComponent.movementDirection.y += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowSouth;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.YPos += movable.speed;
            positionComponent.SpriteTop += movable.speed;
        }
    }

    else if (key == sf::Keyboard::A || key == sf::Keyboard::Left) {
        animationComponent.idleTimer.restart().asSeconds();
        animationComponent.idle = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            animationComponent.movementDirection.x -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.movementDirection.y -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowNorthWest;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos -= movable.speed / speedIdle;
            positionComponent.SpriteLeft -= movable.speed / speedIdle;
            positionComponent.YPos -= movable.speed / speedIdle;
            positionComponent.SpriteTop -= movable.speed / speedIdle;

        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            animationComponent.movementDirection.x -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.movementDirection.y += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowSouthWest;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos -= movable.speed / speedIdle;
            positionComponent.SpriteLeft -= movable.speed / speedIdle;
            positionComponent.YPos += movable.speed / speedIdle;
            positionComponent.SpriteTop += movable.speed / speedIdle;

        } else {
            if (animationComponent.direction != "Left")animationComponent.changedDirection = true;
            animationComponent.direction = "Left";
            animationComponent.movementDirection.x -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowWest;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos -= movable.speed;
            positionComponent.SpriteLeft -= movable.speed;
        }
    }

    else if (key == sf::Keyboard::D || key == sf::Keyboard::Right) {
        animationComponent.idleTimer.restart().asSeconds();
        animationComponent.idle = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            animationComponent.movementDirection.x += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.movementDirection.y -= animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowNorthEast;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos += movable.speed / speedIdle;
            positionComponent.SpriteLeft += movable.speed / speedIdle;
            positionComponent.YPos -= movable.speed / speedIdle;
            positionComponent.SpriteTop -= movable.speed / speedIdle;

        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            animationComponent.movementDirection.x += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.movementDirection.y += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowSouthEast;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos += movable.speed / speedIdle;
            positionComponent.SpriteLeft += movable.speed / speedIdle;
            positionComponent.YPos += movable.speed / speedIdle;
            positionComponent.SpriteTop += movable.speed / speedIdle;

        } else {
            if (animationComponent.direction != "Right")animationComponent.changedDirection = true;
            animationComponent.direction = "Right";
            animationComponent.movementDirection.x += animationComponent.movementSpeed * animationComponent.deltaTime;
            animationComponent.row = animationComponent.rowEast;
            resolve.resolveMovment(world);

            PositionComponent &positionComponent = player.getComponent<PositionComponent>();
            Movable movable = player.getComponent<Movable>();
            positionComponent.XPos += movable.speed;
            positionComponent.SpriteLeft += movable.speed;
        }
    }

        //Attack method
    else if (key == sf::Keyboard::V)
    {
        animationComponent.idleTimer.restart().asSeconds();
        animationComponent.idle = false;
        if(animationComponent.direction != "Attack" )animationComponent.changedDirection = true;
        animationComponent.direction = "Attack";
        resolve.resolveMovment(world);

        PositionComponent& positionComponent = player.getComponent<PositionComponent>();

    }

        //Defend method
    else if (key == sf::Keyboard::B)
    {
        animationComponent.idleTimer.restart().asSeconds();
        animationComponent.idle = false;
        if(animationComponent.direction != "Defend" )animationComponent.changedDirection = true;
        animationComponent.direction = "Defend";
        resolve.resolveMovment(world);

        PositionComponent& positionComponent = player.getComponent<PositionComponent>();
    }
    else if (key == sf::Keyboard::Space)
    {
        Attack attack;
        anax::World& world = *getContext().world;
        attack.resolveAttack(world,player);
    }

    else if (key == sf::Keyboard::Escape && isPressed)
        requestStackPush(States::Pause);
    else if (key == sf::Keyboard::F5 && isPressed)
    {
        objects.clear();
        std::cout << "Loading map data ..." << std::endl;
        if (!Map::load("assets/map/map.json", objects, getContext()))
        {
            std::cout << "Failed to reload map data." << std::endl;
        }
    }
}