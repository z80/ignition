// Rotator script object class. Script objects to be added to a scene node must implement the empty ScriptObject interface
class Destroyer : ScriptObject
{
    int lastHitPlayerId = -1;
    Vector3 rotationSpeed = Vector3(0.1, 0.2, 0.3);
    void Start()
    {
        node.AddTag("Cube");
        // Subscribe physics collisions that concern this scene node
        SubscribeToEvent(node, "NodeCollisionStart", "HandleNodeCollision");
    }

    void SetRotationSpeed(const Vector3&in speed)
    {
        rotationSpeed = speed;
    }

    void HandleNodeCollision(StringHash eventType, VariantMap& eventData)
    {
        // Get the other colliding body, make sure it is moving (has nonzero mass)
        RigidBody@ otherBody = eventData["OtherBody"].GetPtr();

        if (otherBody.mass > 0.0f)
        {
            VectorBuffer contacts = eventData["Contacts"].GetBuffer();

            RigidBody@ body = node.GetComponent("RigidBody");
            while (!contacts.eof)
            {
                Vector3 contactPosition = contacts.ReadVector3();
                Vector3 contactNormal = contacts.ReadVector3();
                float contactDistance = contacts.ReadFloat();
                float contactImpulse = contacts.ReadFloat();
                body.ApplyImpulse(contactNormal * contactImpulse * 2);
            }
            node.scale = node.scale * 0.9;

            if (otherBody.node.name == "Player") {
                lastHitPlayerId = otherBody.node.vars["Player"].GetInt();
            }

            if (node.scale.length < 1.0) {
                VariantMap data;

                data["Type"] = SOUND_EFFECT;
                data["SoundFile"] = "Data/Sounds/achievement.wav";
                SendEvent("PlaySound", data);

                data["Player"] = lastHitPlayerId;
                SendEvent("BoxDestroyed", data);
                Burst();
                node.Remove();

                UnsubscribeFromEvent("NodeCollisionStart");
            } else {
                VariantMap data;
                data["Type"] = SOUND_EFFECT;
                data["SoundFile"] = "Data/Sounds/kick.wav";
                SendEvent("PlaySound", data);
            }
        }
    }

    // Update is called during the variable timestep scene update
    void Update(float timeStep)
    {
        if (node.position.y < -10) {
            VariantMap data;
            data["Type"] = SOUND_EFFECT;
            data["SoundFile"] = "Data/Sounds/achievement.wav";
            SendEvent("PlaySound", data);

            data["Player"] = lastHitPlayerId;
            //SendEvent("BoxDestroyed", data);
            SendEvent("BoxDropped", data);

            Burst();
            node.Remove();
        }
    }

    void Burst()
    {
        Node@ emitter = scene.CreateChild();
        emitter.worldPosition = node.worldPosition;
        ParticleEmitter@ particleEmitter = emitter.CreateComponent("ParticleEmitter");
        particleEmitter.effect = cache.GetResource("ParticleEffect", "Particle/Burst.xml");
        particleEmitter.emitting = true;
        //particleEmitterNodeList.Push(emitter);
        emitter.temporary = true;
        particleEmitter.autoRemoveMode = REMOVE_NODE;
    }
}
