using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackScript : Leaf {

    public int attackDMG = 5;
    private bool readyforAttack = true;

    public AttackScript()
    {

    }

    public override TaskStatus OnBehave(BehaviourState state)
    {
        Context context = (Context)state;

        if (context.GetinRange())
        {
            context.SetinRange(false);
            context.AI.GetComponent<EnemyManager>().StartAttackCoroutine();
            return TaskStatus.SUCCESS;
        }
        return TaskStatus.FAILURE;
    }

    public override void OnReset()
    {

    }
}
