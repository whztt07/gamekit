/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include "gkDynamicsWorld.h"
#include "gkRigidBody.h"
#include "gkGameObject.h"
#include "gkScene.h"
#include "btBulletDynamicsCommon.h"



gkDynamicsWorld::gkDynamicsWorld(const gkString& name, gkScene *scene, gkObject::Loader *manual)
:       gkObject(name, manual), m_scene(scene),
        m_dynamicsWorld(0), m_collisionConfiguration(0),
        m_pairCache(0), m_dispatcher(0), m_constraintSolver(0)
    
{
}

gkDynamicsWorld::~gkDynamicsWorld()
{
}


void gkDynamicsWorld::preLoadImpl(void)
{
    // prepare world

    if (m_dynamicsWorld)
        return;

    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_pairCache              = new btDbvtBroadphase();
    m_dispatcher             = new btCollisionDispatcher(m_collisionConfiguration);
    m_constraintSolver       = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld          = new btDiscreteDynamicsWorld(m_dispatcher,
            m_pairCache,
            m_constraintSolver,
            m_collisionConfiguration);

    gkVector3& grav = m_scene->getProperties().gravity;

    m_dynamicsWorld->setGravity(btVector3(grav.x, grav.y, grav.z));
}


void gkDynamicsWorld::loadImpl(void)
{

    if (!m_dynamicsWorld)
        preLoadImpl();


    if (!m_bodies.empty())
    {
        gkRigidBody *rb = m_bodies.begin();

        while (rb)
        {
            if (!rb->isLoaded())
            {
                gkGameObject *ob = rb->getObject();
                if (ob->isInActiveLayer())
                    rb->load();
                rb = rb->getNext();
            }
        }
        createParentChildHierarchy();
    }
}


void gkDynamicsWorld::unloadImpl(void)
{
    delete m_dynamicsWorld;
    m_dynamicsWorld = 0;

    delete m_constraintSolver;
    m_constraintSolver = 0;

    delete m_dispatcher;
    m_dispatcher = 0;

    delete m_pairCache;
    m_pairCache = 0;

    delete m_collisionConfiguration;
    m_collisionConfiguration = 0;

    if (!m_bodies.empty())
    {
        gkRigidBody *rb = m_bodies.begin();

        while (rb)
        {
            // remove bullet body
            gkRigidBody *tmp = rb;
            rb = rb->getNext();
            delete tmp;
        }
        m_bodies.clear();
    }

}


void gkDynamicsWorld::createParentChildHierarchy(void)
{

    int i;
    btAlignedObjectArray<btCollisionObject*> children;

    for (i = 0;i < m_dynamicsWorld->getNumCollisionObjects();i++)
    {
        btCollisionObject* childColObj = m_dynamicsWorld->getCollisionObjectArray()[i];
        gkRigidBody* childNode = (gkRigidBody*)childColObj->getUserPointer();
        if (!childNode)
            continue;

        gkGameObject* obj = childNode->getObject();
        if (obj->getParent())
            children.push_back(childColObj);
    }

    for (i = 0;i < children.size();i++)
    {
        btCollisionObject* childColObj = children[i];
        gkRigidBody* childNode = (gkRigidBody*)childColObj->getUserPointer();

        m_bodies.erase(childNode);

        gkGameObject *ob = childNode->getObject();
        if (ob->isInActiveLayer())
            childNode->unload();
        delete childNode;
    }

}


// Creates the runtime body.
// gkRigidBody takes care of setting motion states, as well as
// applying physics calculations on a gkGameObject level

gkRigidBody *gkDynamicsWorld::createRigidBody(gkGameObject *state, gkObject::Loader *manual)
{
    GK_ASSERT(state);
    gkRigidBody *rb = new gkRigidBody(state->getName(), state, this, manual);
    m_bodies.push_back(rb);
    return rb;
}

// Notifications for collision callbacks
void gkDynamicsWorld::dispatchCollisions(void)
{
}

// Do one full physics step
void gkDynamicsWorld::step(gkScalar tick)
{
    if (!isLoaded())
        return;

    GK_ASSERT(m_dynamicsWorld);
    m_dynamicsWorld->stepSimulation(tick);
}
