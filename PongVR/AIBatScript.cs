using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AIBatScript : MonoBehaviour {

    public GameObject CurrentBall;
    public float Speed = 55f;
    public float LerpSpeed = 1f;
    public bool Gamemode;
    private Rigidbody rigidBody;

	// Use this for initialization
	void Start ()
    {
        rigidBody = GetComponent<Rigidbody>();	
	}

    void FixedUpdate()
    {
        if (Gamemode)
        {
            rigidBody.velocity = Vector3.zero;
            rigidBody.velocity = Vector3.Lerp(rigidBody.velocity, (CurrentBall.transform.position - transform.position) * Random.Range(25, 37), LerpSpeed * Time.deltaTime);
            rigidBody.velocity = new Vector3(rigidBody.velocity.x, rigidBody.velocity.y, 0.0f);
        }
        else if (!Gamemode)
        {
            rigidBody.velocity = Vector3.zero;
            rigidBody.velocity = Vector3.Lerp(rigidBody.velocity, (CurrentBall.transform.position - transform.position) * Speed, LerpSpeed * Time.deltaTime);
            rigidBody.velocity = new Vector3(rigidBody.velocity.x, rigidBody.velocity.y, 0.0f);
        }
    }
}
