using UnityEngine;
using System.Collections;

public class EDeadState : EStateManager
{
    private readonly EStatePattern enemyPattern;


    public EDeadState(EStatePattern eStatePattern)
    {
        enemyPattern = eStatePattern;
    }

    public void updateStates()
    {		
        //Splatter
        toIdleState();
    }

    public void OnTriggerStay(Collider other)
    {
        
    }

    public void toIdleState()
    {
        enemyPattern.currentState = enemyPattern.idleState;
    }

    public void toAttackState()
    {
        enemyPattern.currentState = enemyPattern.attackState;
    }

    public void toDeadState()
    {
        //Already in death state
    }

    public void toChallengeState()
    {

    }
}
