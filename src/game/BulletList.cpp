#include "BulletList.hpp"
#include "EASTL/allocator.h"
#include "psyqo/ordering-table.hh"
#include "src/game/Bullet.hpp"

BulletList::BulletList() {
    for(int i = 0; i < 1024; i++) {
        m_bullets[i].alive = false;
    }
}

void BulletList::addBullet(const Bullet& newBullet) {
    for(int i = 0; i < 1024; i++) {
        if(!m_bullets[i].alive) {
            m_bullets[i] = newBullet;
            return;
        }
    }
}

void BulletList::update() {
    for(int i = 0; i < 1024; i++) {
        if(m_bullets[i].alive) {
            m_bullets[i].update();
        }
    }
}

void BulletList::draw(psyqo::GPU& gpu) {
    Bullet::setupBulletDrawing(gpu);

    for(int i = 0; i < 1024; i++) {
        if(m_bullets[i].alive) {
            m_bullets[i].draw(gpu);
        }
    }
}