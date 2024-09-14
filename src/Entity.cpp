#include "Entity.hpp"

Entity::Entity(const size_t id, const std::string& tag)
    : m_id(id)
    , m_tag(tag)
    , m_components(
        CScore(),
        CTransform(),
        CLifeSpan(),
        CInput(),
        CBoundingBox(),
        CAnimation(),
        CGravity(),
        CState(),
        CCooldown()
    )
{}

bool Entity::isActive() const {
    return m_active;
}

const std::string& Entity::tag() const {
    return m_tag;
}

const size_t Entity::id() const {
    return m_id;
}

void Entity::destroy() {
    m_active = false;
}
