using UnityEngine;
using System.Collections;

public class EChallengeState : EStateManager {

    private readonly EStatePattern enemyPattern;

    float timeSinceAttack = 5;

    public EChallengeState(EStatePattern eStatePattern)
    {
        enemyPattern = eStatePattern;
    }

    public void updateStates()
    {
        if (enemyPattern.inChallenge == false)
        {
            toIdleState();
        }

        float[] distances = new float[enemyPattern.target.Length];

        for (int i = 0; i < distances.Length; ++i)
        {
            distances[i] = Vector3.Distance(enemyPattern.transform.position, enemyPattern.target[i].transform.position);
        }

        float smallestDistance = 999;
        int smallestIDx = 0;

        for (int i = 0; i < distances.Length; ++i)
        {
            if (distances[i] < smallestDistance)
            {
                smallestDistance = distances[i];
                smallestIDx = i;
            }
        }

        for (int i = 0; i < distances.Length; ++i)
        {
            if (distances[i] < enemyPattern.GetLineOfSight())
            {
                enemyPattern.navMeshAgent.SetDestination(enemyPattern.target[smallestIDx].transform.position);
            }
        }
    }

    public void OnTriggerStay(Collider other)
    {
        if (other.gameObject.CompareTag("Player") && timeSinceAttack >= 3)
        {
            //Damage Player           
            other.gameObject.GetComponent<Health>().LoseLives(1);
            timeSinceAttack = 0;
        }
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
        enemyPattern.currentState = enemyPattern.deadState;

    }

    public void toChallengeState()
    {
        //Already in challenge state
    }
}
