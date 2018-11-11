using UnityEngine;
using System.Collections;



public class EAttackState : EStateManager {


    float timeSinceAttack = 5;

    private readonly EStatePattern enemyPattern;


    public EAttackState(EStatePattern eStatePattern)
    {
        enemyPattern = eStatePattern;
    }

    public void updateStates()
    {
        timeSinceAttack += Time.deltaTime;
        walkTowards();
        if (enemyPattern.GetLineOfSight() < enemyPattern.distanceFrom)
        {
            toIdleState();
        }

        if(enemyPattern.inChallenge == true)
        {
            toChallengeState();
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
        //Already in attack state
    }

    public void toDeadState()
    {

    }

    private void walkTowards()
    {
        float[] distances = new float[enemyPattern.target.Length];

        for(int i = 0; i< distances.Length; ++i)
        {
            distances[i] = Vector3.Distance(enemyPattern.transform.position, enemyPattern.target[i].transform.position);
        }

        float smallestDistance = 999;
        int smallestIDx = 0;

        for(int i = 0; i < distances.Length; ++i)
        {
            if(distances[i] < smallestDistance)
            {
                smallestDistance = distances[i];
                smallestIDx = i;
            }
        }

        for(int i = 0; i < distances.Length; ++i)
        {
            if(distances[i] < enemyPattern.GetLineOfSight())
            {
                enemyPattern.navMeshAgent.SetDestination(enemyPattern.target[smallestIDx].transform.position);
            }
            else
            {
                toIdleState();
            }
        }        
    }

    public void toChallengeState()
    {
        enemyPattern.currentState = enemyPattern.challengeState;
    }
}
