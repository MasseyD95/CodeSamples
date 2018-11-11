using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WanderWorld : Leaf {

    public WanderWorld()
    {

    }



    public override TaskStatus OnBehave(BehaviourState state)
    {
        Context context = (Context)state;
        
        if(context.currentWaypoint == null)
        {
            NewWaypoint(context);
        }
        else if(context.IsVisible(context.AI, context.currentWaypoint, 5))
        {
            NewWaypoint(context);
        }

        return TaskStatus.SUCCESS;

    }

    public override void OnReset()
    {

    }

    public void NewWaypoint(Context con)
    {
        GameObject tempWaypoint;
        tempWaypoint = con.Waypoints[Random.Range(0, con.Waypoints.Length - 1)];

        for(int i = 0; i < 100; i++)
        {
            if(con.currentWaypoint != tempWaypoint)
            {
                con.currentWaypoint = tempWaypoint;
                break;
            }
        }
        con.AI.GetComponent<Unit>().ChangeTarget(con.currentWaypoint.transform);
    }
}
