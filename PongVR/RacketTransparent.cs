using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RacketTransparent : MonoBehaviour {

    public GameObject ball;
    public GameObject cam;
    Ray ray;
    RaycastHit hit;

    Renderer rend;
    RuleManager ruleManager;
	// Use this for initialization
	void Start ()
    {
        ruleManager = FindObjectOfType<RuleManager>();
        rend = GetComponent<Renderer>();
	}
	
	// Update is called once per frame
	void Update ()
    {
        if(ruleManager.inGame || ruleManager.inPlay)
        {
            Vector3 direction = cam.transform.position - ball.transform.position;
            if (Physics.Raycast(ball.transform.position, direction, out hit, 400f))
            {
                Debug.DrawRay(ball.transform.position, direction, Color.blue);

                if (hit.transform.CompareTag("Axis_Z_PlayerBat"))
                {
                    GoTransparent();
                }

            }
            else
            {
                GoOpaque();
            }
        }

	}

    void GoTransparent()
    {
        Color col = GetComponent<Renderer>().material.color;
        if(col.a > 0.5f)
        {
            col.a -= 0.01f;
        }
        GetComponent<Renderer>().material.SetColor("_Color", col);
    }

    void GoOpaque()
    {
        Color col = GetComponent<Renderer>().material.color;
        if (col.a < 1f)
        {
            col.a += 0.01f;
        }
        GetComponent<Renderer>().material.SetColor("_Color", col);
    }
}
