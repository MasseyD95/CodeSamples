using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Context : BehaviourState {

   
    public GameObject Player;
    public GameObject AI;
    public GameManager gameManager;
    //public Bot bot = new Bot();
    public GameObject[] Waypoints;
    public GameObject currentWaypoint;
    private bool inRange = false;

    public Context(GameObject player, GameObject ai, GameManager gamemanager, GameObject[] waypoints)
    {
        gameManager = gamemanager;
        Player = player;
        AI = ai;
        Waypoints = waypoints;
    }


    public void SetinRange(bool answer)
    {
        inRange = answer;
    }

    public bool GetinRange()
    {
        return inRange;
    }

    public bool IsVisible(GameObject player, GameObject ai, float amount)
    {
        if (Vector3.Distance(player.transform.position, ai.transform.position) < amount)
        {
            return true;
        }
        return false;

    }


}
