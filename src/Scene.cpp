#include "Scene.hpp"

Scene::Scene() {

}

Scene::Scene(GameEngine* gameEngine) {
    m_game = gameEngine;
    m_entities = EntityManager(); 
    m_actionMap = std::map<int, std::string>();
}

void Scene::simulate(const size_t frames) {
    for (int i = 0; i < frames; i++) {
        update();
    }
}

void Scene::registerAction(int inputKey, const std::string& actionName) {
    m_actionMap[inputKey] = actionName;
}

size_t Scene::width() const {
    return m_game->window().getSize().x;
}

size_t Scene::height() const {
    return m_game->window().getSize().y;
}

size_t Scene::currentFrames() const {
    return m_currentFrame;
}

bool Scene::hasEnded() const {
    return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const {
    return m_actionMap;
}

// TODO: implement when entity manager is updated
void Scene::drawLine(const Vec2& p1, const Vec2& p2) {
}
