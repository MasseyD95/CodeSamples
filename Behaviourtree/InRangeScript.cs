using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InRangeScript : Leaf{

    public InRangeScript()
    {

    }

    public override TaskStatus OnBehave(BehaviourState state)
    {
        Context context = (Context)state;
        if(context.AI.GetComponent<EnemyManager>().attacking)
        {
            context.SetinRange(true);
            return TaskStatus.SUCCESS;
        }

        return TaskStatus.FAILURE;
    }

    public override void OnReset()
    {

    }

}
