using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VisableScript : Leaf {

    //public GameObject player;
   // public GameObject ai;

    public VisableScript()
    {

    }



    public override TaskStatus OnBehave(BehaviourState state)
    {
        Context context = (Context)state;
        if (context.IsVisible(context.Player, context.AI, 40))
        {
            return TaskStatus.SUCCESS;
        }

        return TaskStatus.FAILURE;
    }

    public override void OnReset()
    {

    }
}
