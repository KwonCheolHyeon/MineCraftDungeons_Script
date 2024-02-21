#include "yaContactCallback.h"

#include "yaGameObject.h"
#include "yaCollider.h"



namespace md
{
    void ContactCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pPairs, physx::PxU32 iNbPairs)
    {
        for (physx::PxU32 i = 0; i < iNbPairs; i++)
        {
            const physx::PxContactPair& cp = pPairs[i];

            if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                if (pairHeader.actors[0]->userData && pairHeader.actors[1]->userData)
                {
                    GameObject* pContact = static_cast<GameObject*>(pairHeader.actors[0]->userData);
                    GameObject* pOther = static_cast<GameObject*>(pairHeader.actors[1]->userData);

                    if (nullptr != pContact && nullptr != pOther)
                    {
                        if (pContact->GetComponent<Collider3D>())
                            pContact->GetComponent<Collider3D>()->OnCollisionEnter(pOther->GetComponent<Collider3D>());

                        if (pOther->GetComponent<Collider3D>())
                            pOther->GetComponent<Collider3D>()->OnCollisionEnter(pContact->GetComponent<Collider3D>());
                    }
                }
            }

            else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
            {
                if (pairHeader.actors[0]->userData && pairHeader.actors[1]->userData)
                {
                    GameObject* pContact = static_cast<GameObject*>(pairHeader.actors[0]->userData);
                    GameObject* pOther = static_cast<GameObject*>(pairHeader.actors[1]->userData);

                    if (nullptr != pContact && nullptr != pOther)
                    {
                        if (pContact->GetComponent<Collider3D>())
                            pContact->GetComponent<Collider3D>()->OnCollisionStay(pOther->GetComponent<Collider3D>());

                        if (pOther->GetComponent<Collider3D>())
                            pOther->GetComponent<Collider3D>()->OnCollisionStay(pContact->GetComponent<Collider3D>());
                    }
                }
            }

            else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (pairHeader.actors[0]->userData && pairHeader.actors[1]->userData)
                {
                    GameObject* pContact = static_cast<GameObject*>(pairHeader.actors[0]->userData);
                    GameObject* pOther = static_cast<GameObject*>(pairHeader.actors[1]->userData);

                    if (nullptr != pContact && nullptr != pOther)
                    {
                        if (pContact->GetComponent<Collider3D>())
                            pContact->GetComponent<Collider3D>()->OnCollisionExit(pOther->GetComponent<Collider3D>());

                        if (pOther->GetComponent<Collider3D>())
                            pOther->GetComponent<Collider3D>()->OnCollisionExit(pContact->GetComponent<Collider3D>());
                    }
                }
            }
        }
    }

    void ContactCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
    {
        for (physx::PxU32 i = 0; i < count; i++)
        {
            const physx::PxTriggerPair& pair = pairs[i];

            if (pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                if (pair.triggerActor && pair.otherActor)
                {
                    GameObject* pTrigger = static_cast<GameObject*>(pair.triggerActor->userData);
                    GameObject* pOther = static_cast<GameObject*>(pair.otherActor->userData);

                    if (nullptr != pTrigger && nullptr != pOther)
                    {
                        if (pTrigger->GetComponent<Collider3D>())
                            pTrigger->GetComponent<Collider3D>()->OnTriggerEnter(pOther->GetComponent<Collider3D>());

                        if (pOther->GetComponent<Collider3D>())
                            pOther->GetComponent<Collider3D>()->OnTriggerEnter(pTrigger->GetComponent<Collider3D>());
                    }
                }
            }

            if (pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (pair.triggerActor && pair.otherActor)
                {
                    GameObject* pTrigger = static_cast<GameObject*>(pair.triggerActor->userData);
                    GameObject* pOther = static_cast<GameObject*>(pair.otherActor->userData);

                    if (nullptr != pTrigger && nullptr != pOther)
                    {
                        if (pTrigger->GetComponent<Collider3D>())
                            pTrigger->GetComponent<Collider3D>()->OnTriggerExit(pOther->GetComponent<Collider3D>());

                        if (pOther->GetComponent<Collider3D>())
                            pOther->GetComponent<Collider3D>()->OnTriggerExit(pTrigger->GetComponent<Collider3D>());
                    }

                }
            }
        }
    }
}
