using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PhysicsManager : MonoBehaviour {
    public float friction = 0.99f;
    public float iterationsPerFrame = 10.0f;
    public float step_size = 0.01f;

    public Vector3 gravity;
    private Vector3 velocity;
    private float lastUpdatedTime;

    List<BasicRigidBody> otherRigidBody;

    // Use this for initialization
    void Awake()
    {
        otherRigidBody = new List<BasicRigidBody>();
        foreach (BasicRigidBody rigid in FindObjectsOfType<BasicRigidBody>())
        {
            otherRigidBody.Add(rigid);
        }

        gravity = new Vector3(0, (float)-9.80665, 0);

    }

    // Update is called once per frame
    void Update()
    {
        if(Time.time - lastUpdatedTime >= step_size)
        {
            foreach (BasicRigidBody rigid in otherRigidBody)
            {
                Vector3 thingPos = rigid.position;
                if (thingPos != rigid.transform.position)
                {
                    Vector3 dMe = rigid.transform.position - thingPos;
                    //rigid.velocity += (dMe / 4.0f);

                    rigid.position = rigid.transform.position;
                }
            }
            foreach (BasicRigidBody rigid in otherRigidBody)
            {
                rigid.PhysicsUpdate();
            }
            foreach (BasicRigidBody rigid in otherRigidBody)
            {
                rigid.transform.position = rigid.position;
            }
            lastUpdatedTime = Time.time;

        }
    }

    public static Vector3 VectorAfterNormalForce(Vector3 force, Vector3 normal)
    {
        Vector3 output;

        Vector3 rejectionVector = BasicRigidBody.Rejection(force, normal);
        Vector3 projectionVector = BasicRigidBody.Projection(force, normal);

        float dot = BasicRigidBody.Dot(force, normal);
        float direction = dot / Mathf.Abs(dot);

        if(direction <= 0.0f)
        {
            output = rejectionVector;
            return output;
        }
        else
        {
            output = rejectionVector + projectionVector;
            return output;
        }
    }
}
