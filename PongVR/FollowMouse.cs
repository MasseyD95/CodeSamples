using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class FollowMouse : MonoBehaviour {

    public GameObject controller;
    private Vector3 newPaddlePos;
    private Vector3 oldPaddlePos;
    private Vector3 deltaDistance;
    public Text text;
    private Vector3 mousePos;
    private float distance = 1.2f;

    public GameObject Centerpoint;
    private Quaternion paddleRot;

    private bool startGame = false;
	// Use this for initialization
	void Start ()
    {
        if (controller != null)
        {
            if (controller.GetComponent<OVRTrackedRemote>().controllerConnected)
            {
                oldPaddlePos = controller.GetComponent<OVRTrackedRemote>().GetLocalPostition();
            }
        }

        

    }
	
	// Update is called once per frame
	void Update ()
    {
        if(OVRInput.Get(OVRInput.Button.PrimaryIndexTrigger))
        {
            transform.position = Centerpoint.transform.position;
            startGame = true;
        }

        if (controller.GetComponent<OVRTrackedRemote>().controllerConnected && startGame)
        {
            //paddleRot = controller.GetComponent<OVRTrackedRemote>().GetLocalRotation();

            //Vector3 posRot = paddleRot * Vector3.forward;
            //Vector3 posRotUp = paddleRot * Vector3.up;

            //text.text = posRot.x.ToString() + " + " + posRot.y.ToString() + " + " + posRot.z.ToString();
            //text2.text = posRotUp.x.ToString() + " + " + posRotUp.y.ToString() + " + " + posRotUp.z.ToString();

            //posRot.z = 0f;
            //transform.position = posRot;


            newPaddlePos = controller.GetComponent<OVRTrackedRemote>().GetLocalPostition();

            deltaDistance = newPaddlePos - oldPaddlePos;

            oldPaddlePos = newPaddlePos;

            if (deltaDistance.x > -0.003f && deltaDistance.x < 0.003f)
            {
                deltaDistance.x = 0;
            }
            if (deltaDistance.y > -0.003f && deltaDistance.y < 0.003f)
            {
                deltaDistance.y = 0;
            }

            //if (transform.position.x > Centerpoint.transform.position.x + 1.0f && deltaDistance.x < 0.0f)
            //{
            //    deltaDistance.x = 0.0f;
            //}
            //else if (transform.position.x < Centerpoint.transform.position.x - 1.0f && deltaDistance.x > 0.0f)
            //{
            //    deltaDistance.x = 0.0f;
            //}
            //if (transform.position.y > Centerpoint.transform.position.y + 0.7f && deltaDistance.y > 0.0f)
            //{
            //    deltaDistance.y = 0.0f;
            //}
            //else if (transform.position.y < Centerpoint.transform.position.y - 0.7f && deltaDistance.y < 0.0f)
            //{
            //    deltaDistance.y = 0.0f;
            //}

            deltaDistance.x = -deltaDistance.x;

            deltaDistance.z = 0;

            transform.position = transform.position += (deltaDistance * 3);
            transform.position = new Vector3(transform.position.x, transform.position.y, -12.987f);

            //text.text = transform.position.x.ToString() + " + " + transform.position.y.ToString() + " + " + transform.position.z.ToString();
        }
        else
        {
            
            mousePos = Input.mousePosition;
            if (Input.GetMouseButtonDown(1))
            {
                distance += 0.1f;
            }
            if (Input.GetMouseButtonDown(2))
            {
                distance -= 0.1f;
            }
            mousePos.z = distance;

            transform.position = Camera.main.ScreenToWorldPoint(mousePos);

            //text.text = transform.position.x.ToString() + " + " + transform.position.y.ToString() + " + " + transform.position.z.ToString();


        }
    }
}
