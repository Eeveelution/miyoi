#include "BulletList.hpp"
#include "EASTL/allocator.h"
#include "psyqo/ordering-table.hh"
#include "src/game/Bullet.hpp"

BulletList::BulletList() {
    for(int i = 0; i < BULLET_LIST_SIZE; i++) {
        m_bullets[i].alive = false;
        m_dead.push_back(i);
    }
}

void BulletList::addBullet(const Bullet& newBullet) {
    if (m_dead.empty()) return;
    auto id = m_dead.back();
    m_dead.pop_back();
    m_bullets[id] = newBullet;
}

void BulletList::addBullet(Bullet&& newBullet) {
    if (m_dead.empty()) return;
    auto id = m_dead.back();
    m_dead.pop_back();
    m_bullets[id] = eastl::move(newBullet);
}

void BulletList::update() {
    for(int i = 0; i < BULLET_LIST_SIZE; i++) {
        if(m_bullets[i].alive) {
            if (m_bullets[i].update() == UpdateAction::DeleteFromList) {
                m_dead.push_back(i);
            }
        }
    }
}

void BulletList::draw(psyqo::GPU& gpu) {
    for(int i = 0; i < BULLET_LIST_SIZE; i++) {
        if(m_bullets[i].alive) {
            m_bullets[i].draw(gpu);
        }
    }
}