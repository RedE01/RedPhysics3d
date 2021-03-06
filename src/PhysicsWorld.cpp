#include "PhysicsWorld.h"
#include "Response/CollisionResolver.h"

#include "Math/Matrix3x3.h"
#include "Math/Quaternion.h"

namespace redPhysics3d {

    PhysicsWorld::PhysicsWorld() {
    }

    void PhysicsWorld::init() {
        for(int i = 0; i < m_rigidbodies.size(); ++i)
            m_boundingVolumeTree.insert(m_rigidbodies[i].get());
        for(int i = 0; i < m_staticbodies.size(); ++i)
            m_boundingVolumeTree.insert(m_staticbodies[i].get());
    }

    void PhysicsWorld::stepSimulation(float deltaTime) {
        for(auto& rb : m_rigidbodies) {
            rb->addForce(gravity * rb->gravityScale * rb->getMass());

            rb->updateRotationMatricies();
            rb->updateInertia();

            rb->integrate(deltaTime);
        }

        for(auto& rb : m_rigidbodies) {
            for(auto& cs : rb->collisionShapes) {
                cs->updateCollisionShape();
            }
        }
        for(auto& sb : m_staticbodies) {
            for(auto& cs : sb->collisionShapes) {
                cs->updateCollisionShape();
            }
        }

        CollisionData collisionData;

        generateContacts(collisionData);

        CollisionResolver collisionResolver(collisionData);
        collisionResolver.solveCollision(collisionData.contacts.size() * 4, deltaTime);
    }

    RigidBody* PhysicsWorld::addRigidBody() {
        m_rigidbodies.push_back(std::make_unique<RigidBody>());
        return m_rigidbodies.back().get();
    }

    StaticBody* PhysicsWorld::addStaticBody() {
        m_staticbodies.push_back(std::make_unique<StaticBody>());
        return m_staticbodies.back().get();
    }

    void PhysicsWorld::removeRigidBody(RigidBody* rigidbody) {
        for(int i = 0; i < m_rigidbodies.size(); ++i) {
            if(m_rigidbodies[i].get() == rigidbody) {
                m_rigidbodies.erase(m_rigidbodies.begin() + i);
                return;
            }
        }
    }

    void PhysicsWorld::removeStaticBody(StaticBody* staticbody) {
        for(int i = 0; i < m_staticbodies.size(); ++i) {
            if(m_staticbodies[i].get() == staticbody) {
                m_staticbodies.erase(m_staticbodies.begin() + i);
                return;
            }
        }
    }

    void PhysicsWorld::generateContacts(CollisionData& collisionData) {
        std::vector<PotentialCollision> pCollisions;

        for(auto& rb : m_rigidbodies) {
            m_boundingVolumeTree.update(rb.get());
        }
        for(auto& sb : m_staticbodies) {
            m_boundingVolumeTree.update(sb.get());
        }
        m_boundingVolumeTree.getPotentialCollisions(pCollisions);

        for(const auto& pc : pCollisions) {
            for(auto& collisionShape1 : pc.bodies[0]->collisionShapes) {
                for(auto& collisionShape2 : pc.bodies[1]->collisionShapes) {
                    CollisionAlgorithm* collisionTestAlgorithm = m_collisionDispatcher.getCollisionAlgorithm(collisionShape1->getShapeType(), collisionShape2->getShapeType());

                    collisionTestAlgorithm->generateContacts(collisionData, collisionShape1.get(), collisionShape2.get());
                }
            }
        }
    }

}