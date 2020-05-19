#include "RigidBody.h"

namespace redPhysics3d {

    RigidBody::RigidBody() {
        setMass(1.0);
    }

    void RigidBody::setMass(const float& mass) {
        if(mass <= 0.0) return;

        if(mass != m_mass) m_inverseMass = 1.0 / mass;
        m_mass = mass;

        calculateInertia();
    }

    void RigidBody::addForce(const Vector3& force) {
        m_externalForce += force;
    }

    void RigidBody::addTorque(const Vector3& force) {
        m_externalTorque += force;
    }

    void RigidBody::clearForce() {
        m_externalForce = Vector3(0,0,0);
    }

    void RigidBody::clearTorque() {
        m_externalTorque = Vector3(0,0,0);
    }

    void RigidBody::calculateInertia() {
        // 0,0833333333333 = 1 / 12
        float widthSquare = 1.0, heightSqure = 1.0, lengthSquare = 1.0;
        m_inertia.x = 0.0833333333333 * m_mass * (heightSqure + widthSquare);
        m_inertia.y = 0.0833333333333 * m_mass * (lengthSquare + widthSquare);
        m_inertia.z = 0.0833333333333 * m_mass * (heightSqure + lengthSquare);

        m_inverseInertia.x = 1.0 / m_inertia.x;
        m_inverseInertia.y = 1.0 / m_inertia.y;
        m_inverseInertia.z = 1.0 / m_inertia.z;
    }

}