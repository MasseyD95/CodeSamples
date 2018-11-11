using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bot : MonoBehaviour {

    public bool IsVisible(GameObject player, GameObject ai, float amount)
    {
        if (Vector3.Distance(player.transform.position, ai.transform.position) < amount)
        {
            return true;
        }
        return false;

    }
}
