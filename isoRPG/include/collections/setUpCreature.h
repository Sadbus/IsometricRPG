#ifndef ISORPG_SETUPPLAYER_H
#define ISORPG_SETUPPLAYER_H

#include <anax/System.hpp>
#include <anax/anax.hpp>
#include <anax/World.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "states/state_base.h"
#include "application.h"

#include <components/Comp_size.h>
#include <components/Comp_position.h>
#include <components/Comp_Texture.h>
#include <components/Comp_animation.h>
#include <components/Comp_movable.h>
#include <components/Comp_healt.h>
#include <components/Comp_lootable.h>
#include <components/Comp_State.h>
#include <components/Comp_mousedOver.h>
#include <components/Comp_talk.h>


class SetUpCreature
{
public:
    void setUpPlayer(anax::Entity& entity, sf::RenderWindow& window);

    void setUpEnemies(anax::Entity &entity, sf::Texture &texture, sf::RenderWindow &window, int Xpos, int Ypos,
                      std::string diffeculty);

    void setUpNPC(anax::Entity& entity, sf::Texture& texture, sf::RenderWindow& window, int Xpos, int Ypos, int Xofset, int Yofset);

    void setUpBoss(anax::Entity& entity, sf::Texture& texture, sf::RenderWindow& window, int Xpos, int Ypos);
};

#endif //ISORPG_SETUPPLAYER_H
