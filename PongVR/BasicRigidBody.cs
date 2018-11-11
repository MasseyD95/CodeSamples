using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Audio;
public class BasicRigidBody : MonoBehaviour {
    //Mass 1.0 = 1 kilogram
    public float mass = 1.0f;
    public Vector3 drag = new Vector3(0,0,0);
    public Vector3 position;
    public GameObject controller;
    public GameObject PlayerSideZone;
    public GameObject PlayerAim;
    public GameObject PlayerServe;
    public GameObject AIServe;
    public GameObject PlayerBat;
    public GameObject AIBat;
    [Range(0f, 5f)] public float Y_VelocityAddition = 3f;
    [Range(0f, 5f)] public float Z_VelocityAddition = 1f;

    public AudioSource BatHit;
    public AudioSource PlayerSideBounce;
    public AudioSource AISideBounce;
    // 1 = give player score, 2 = give other score, 3 = last touched is floor
    public int playerWin = 3;
    //1 = player last hit, 2 = other last hit
    private int currentBat = 3;
    private bool serve;
    private Vector3 force;

    private float radius;
    private float diameter;
    public Vector3 velocity;
    private PhysicsManager physManager;
    private RuleManager ruleManager;
    private List<BasicRigidBody> otherRigidBody;
    private List<BoxCollider> otherColliders;
    private bool addPoint = false;
    // Use this for initialization
    void Awake()
    {
        radius = (float)0.04;
        physManager = FindObjectOfType<PhysicsManager>();
        ruleManager = FindObjectOfType<RuleManager>();
        //Create lists
        Collider[] allColliders = FindObjectsOfType<Collider>();
        otherRigidBody = new List<BasicRigidBody>();
        otherColliders = new List<BoxCollider>();
        //Find every instance of this script in the scene
        foreach (BasicRigidBody rigid in FindObjectsOfType<BasicRigidBody>())
        {
            //If current iteration is itself then dont add
            //Continue is the same as break but will carry on the foreach loop
            if (rigid == this)
            {
                continue;
            }

            otherRigidBody.Add(rigid);
        }

        //Loop through every collider in the scene and add the box colliders to our list
        for (int i = 0; i < allColliders.Length; i++)
        {
            if (allColliders[i].GetComponent<BoxCollider>() != null)
            {
                otherColliders.Add(allColliders[i].GetComponent<BoxCollider>());
            }
        }
    }

    public static Vector3 Normalize(Vector3 input)
    {
        //Normalize the vector if the magnitude is greater than 1. otherwise return the result as it is
        if (input.sqrMagnitude > 1)
        {
            return Vector3.Normalize(input);
        }
        else
        {
            return input;
        }
    }

    public bool SphereSphereCollision(BasicRigidBody other, out Vector3 newDist, out Vector3 newNormal)
    {
        //Difference vector
        Vector3 vecd = position - other.position;
        float r1 = radius;
        float r2 = other.radius;
        //Length is computed
        //sqrmag is 6x faster than vector3.distance on mobile
        float dist = Mathf.Sqrt(vecd.x * vecd.x + vecd.y * vecd.y + vecd.z * vecd.z);
        float squareMag = vecd.sqrMagnitude;

        if (dist < r1 + r2)
        {
            //We have a collision
            if(dist == 0)
            {
                Vector3 normal = Normalize(new Vector3(1, 1, 1));
                newDist = position + normal / radius / 10.0f;

                newNormal = Normalize(position - newDist);
            }
            else
            {
                newDist = (position * r1 + other.position * r2) / (r1 + r2);
                newNormal = Normalize(position - newDist);
            }
            return true;
        }
        else
        {
            //No collision
            newDist = new Vector3(0, 0, 0);
            newNormal = new Vector3(0, 0, 0);
            return false;
        }
    }

    public bool SphereAABBCollision(BoxCollider other, out Vector3 newDist, out Vector3 newNormal)
    {
        Vector3 closestPoint = other.ClosestPointOnBounds(position);
        Vector3 vecd = position - closestPoint;

        float dist = Mathf.Sqrt(vecd.x * vecd.x + vecd.y * vecd.y + vecd.z * vecd.z);
        //distance
        float squareMag = vecd.sqrMagnitude;
        if (dist <= radius)
        {
            //We have a collision
            if (dist == 0)
            {
                Vector3 normal = Normalize(new Vector3(1, 1, 1));
                newDist = position + normal / radius / 10.0f;
                Debug.Log("Ball stop bug");

                newNormal = Normalize(position - newDist);
            }
            else
            {
                newDist = closestPoint;
                newNormal = Normalize(position - newDist);
            }
            return true;
        }
        else
        {
            //No collision
            newDist = new Vector3(0, 0, 0);
            newNormal = new Vector3(0, 0, 0);
            return false;
        }

    }

    public bool CheckCollisions()
    {
        bool hasCollided = false;

        for(int i = 0; i < otherRigidBody.Count; i++)
        {
            BasicRigidBody other = otherRigidBody[i];
            Vector3 point;
            Vector3 normal;

            if(SphereSphereCollision(other,out point,out normal))
            {
                hasCollided = true;
                Vector3 newVelocity = PhysicsManager.VectorAfterNormalForce(velocity, normal);
                Vector3 lostVelocity = velocity - newVelocity;

                Vector3 o_newVelocity = PhysicsManager.VectorAfterNormalForce(other.velocity, -normal);
                Vector3 o_losVelocity = other.velocity - o_newVelocity;

                Vector3 avgLostVelocity = (lostVelocity + o_losVelocity) / 2.0f;

                velocity = newVelocity * physManager.friction + avgLostVelocity;
                other.velocity = o_newVelocity * physManager.friction + avgLostVelocity;

                Vector3 differenceVector = point - position;
                float squareMag = differenceVector.sqrMagnitude;
                float dist = Mathf.Sqrt(squareMag);

                Vector3 offset = normal * (radius - dist);

                position += offset;

                other.position -= offset;
            }
        }
        for(int i = 0; i < otherColliders.Count; i++)
        {
            BoxCollider other = otherColliders[i];
            Vector3 point;
            Vector3 normal;
            
            if(SphereAABBCollision(other, out point, out normal))
            {

                if (other.gameObject.CompareTag("Axis_Y_Win"))
                {
                    if (playerWin == 1)
                    {
                        ruleManager.AddPlayerScore();
                    }
                    else if (playerWin == 2)
                    {
                        ruleManager.AddOtherScore();
                    }

                    playerWin = 3;
                }
                hasCollided = true;
                //Vector3 newVelocity = PhysicsManager.VectorAfterNormalForce(velocity, normal);

                //velocity = newVelocity * physManager.friction;

                // For temp solution this is fine but not ideal in the most case
                if (other.gameObject.CompareTag("Axis_Y_PlayerSideTable"))
                {
                    PlayerSideBounce.Play();
                    velocity.y = -velocity.y;
                }
                else if (other.gameObject.CompareTag("Axis_Y_OtherSideTable"))
                {
                    AISideBounce.Play();
                    velocity.y = -velocity.y;
                }
                else if (other.gameObject.CompareTag("Axis_Z_PlayerBat"))
                {
                    BatHit.Play();
                    ruleManager.ChangeToBlue();
                    if(!addPoint)
                    {
                        ruleManager.AddRallyCount();
                        addPoint = true;
                    }
                    Vector3 destination = new Vector3(ruleManager.GetCurrentAim().x, ruleManager.GetCurrentAim().y, ruleManager.GetCurrentAim().z);
                    Vector3 heading = (destination - transform.position);//.normalized;

                    velocity = heading;
                    velocity = new Vector3(velocity.x, velocity.y + Y_VelocityAddition, velocity.z - Z_VelocityAddition);
                    playerWin = 1;
                }
                else if (other.gameObject.CompareTag("Axis_Z_OtherBat"))
                {
                    addPoint = false;
                    BatHit.Play();
                    ruleManager.ChangeToRed();
                    Vector3 destination = new Vector3(Random.Range(-6.15f, -7.73f), PlayerSideZone.transform.position.y, PlayerSideZone.transform.position.z);
                    Vector3 heading = (destination - transform.position);//.normalized;

                    velocity = heading;
                    velocity = new Vector3(velocity.x, velocity.y + Y_VelocityAddition, velocity.z + (Z_VelocityAddition + 0.3f));
                    playerWin = 2;
                }
                else if (other.gameObject.CompareTag("Axis_X"))
                {
                    velocity.x = -velocity.x;
                }

                Vector3 differenceVector = point - position;
                float squareMag = differenceVector.sqrMagnitude;
                float dist = Mathf.Sqrt(squareMag);
                Vector3 offset = normal * (radius - dist);

                position += offset;
                velocity -= normal * Dot(velocity, differenceVector);
            }
        }
        return hasCollided;
    }

    public void PhysicsUpdate()
    {
        if(ruleManager.inPlay)
        {
            for (int i = 0; i < physManager.iterationsPerFrame; i++)
            {
                float dt = physManager.step_size / physManager.iterationsPerFrame;

                drag = (float)0.4f * velocity;

                Vector3 acceleration = (force / mass) + (physManager.gravity - drag);
                position += velocity * dt + 0.5f * acceleration * dt * dt;

                velocity += acceleration * dt;

                force = new Vector3(0, 0, 0);
                CheckCollisions();
            }
        }
    }

    public void AddForce(Vector3 newForce)
    {
        velocity = new Vector3(newForce.x, newForce.y + 1, newForce.z); // * 50;// * (float) 0.1;
    }

    public void Reset()
    {
        velocity = new Vector3(0, 0, 0);
    }

    public static Vector3 Subtract(Vector3 a, Vector3 b)
    {
        return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    public static Vector3 Scale(float f, Vector3 a)
    {
        return new Vector3(f * a.x, f * a.y, f * a.z);
    }

    public static float Dot(Vector3 a, Vector3 b)
    {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }

    public static Vector3 Projection(Vector3 a, Vector3 b)
    {
        return Scale(Dot(a, b) / Dot(b, b), b);
    }
    public static Vector3 Rejection(Vector3 a, Vector3 b)
    {
        return Subtract(a, Projection(a, b));
    }
}
