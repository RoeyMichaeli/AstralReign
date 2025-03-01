#include <sstream>
#include "Action.h"

Action::Action() = default;

Action::Action(ActionName name, ActionType type)
    : m_name(name), m_type(type) {
}

Action::Action(ActionName name, Vector2f pos)
    : m_name(name), m_pos(pos) {
}

Action::Action(ActionName name, ActionType type, Vector2f pos)
    : m_name(name), m_type(type), m_pos(pos) {
}

const ActionName& Action::name() const {
    return m_name;
}

const ActionType& Action::type() const {
    return m_type;
}

const Vector2f& Action::pos() const {
    return m_pos;
}

std::string Action::toString() const {
    std::stringstream ss;
    ss << static_cast<int>(name()) << " "
        << static_cast<int>(type()) << " "
        << static_cast<int>(pos().x) << " "
        << static_cast<int>(pos().y);
    return ss.str();
}
