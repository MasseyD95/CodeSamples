using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkTowards : Leaf {

    public WalkTowards()
    {

    }



    public override TaskStatus OnBehave(BehaviourState state)
    {
        Context context = (Context)state;
        context.AI.GetComponent<Unit>().ChangeTarget(context.Player.transform);

        return TaskStatus.SUCCESS;
    }

    public override void OnReset()
    {

    }
}
