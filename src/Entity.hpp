#pragma once

#include "Components.hpp"
#include <string>
#include <tuple>

typedef std::tuple<
    CScore,
    CTransform,
    CLifeSpan,
    CInput,
    CBoundingBox,
    CAnimation,
    CGravity,
    CState,
    CCooldown,
    CEffects
> ComponentTuple;

class Entity {

    friend class EntityManager;

    bool           m_active    = true;
    size_t         m_id        = 0;
    std::string    m_tag       = "default";
    ComponentTuple m_components;

    Entity(const size_t id, const std::string& tag);

public:
    bool               isActive()       const;
    const std::string& tag()            const;
    const size_t       id()             const;
    void               destroy();

    template <typename T, typename... TArgs>
    T& addComponent(TArgs&&... mArgs) {
        auto& component = getComponent<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.has = true;
        return component;
    }

    template <typename T>
    bool hasComponent() {
        return getComponent<T>().has;
    }

    template <typename T>
    T& getComponent() {
        return std::get<T>(m_components);
    }

    template <typename T>
    void removeComponent() {
        getComponent<T>() = T();
    }
};
