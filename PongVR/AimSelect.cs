using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AimSelect : MonoBehaviour {

    public Material Red;
    public Material Green;

    bool colourBool = false;
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update ()
    {
		if(colourBool)
        {
            this.gameObject.GetComponent<Renderer>().material = Green;
        }
        else if(!colourBool)
        {
            this.gameObject.GetComponent<Renderer>().material = Red;
        }
	}

    public void ChangeColour(bool choice)
    {
        colourBool = choice;
    }
}
