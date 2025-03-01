#pragma once
#ifndef ACTION_H
#define ACTION_H
#include <SFML/Graphics.hpp>

#include <string>

using sf::Vector2f;

enum ActionName
{
    LeftClick = 0,
    MiddleClick,
    RightClick,
    MouseMove,
    ScrollUp,
    ScrollDown,

    Up,
    Down,
    Right,
    Left,

    Activate,
    Mute,
    Quit
};

enum ActionType
{
    Start,
    End
};

class Action {
    ActionName m_name;
    ActionType m_type;
    Vector2f m_pos = Vector2f(0.0f, 0.0f);

public:
    Action();
    Action(ActionName name, ActionType type);
    Action(ActionName name, Vector2f pos);
    Action(ActionName name, ActionType type, Vector2f pos);

    [[nodiscard]] const ActionName& name() const;
    [[nodiscard]] const ActionType& type() const;
    [[nodiscard]] const Vector2f& pos() const;
    [[nodiscard]] std::string toString() const;
};

#endif // ACTION_H
