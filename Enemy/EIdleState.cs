using UnityEngine;
using System.Collections;

public class EIdleState : EStateManager {

    private int currentDestination;
    private readonly EStatePattern enemyPattern;

    public EIdleState(EStatePattern eStatePattern)
    {
        enemyPattern = eStatePattern;
        newDestination(currentDestination);
    }

    public void updateStates()
    {
        idleWalk();
        playerInRange();

        if(enemyPattern.inChallenge == true)
        {
            toChallengeState();
        }

        
    }
    public void OnTriggerStay(Collider other)
    {
        if (other.gameObject.CompareTag("Player"))
        {
            //Damage Player
            other.gameObject.GetComponent<Health>().LoseLives(1);
            toAttackState();
        }  
    }

    public void toIdleState()
    {
        //Already in idle state
    }

    public void toAttackState()
    {
        enemyPattern.currentState = enemyPattern.attackState;
    }

    public void toDeadState()
    {
        enemyPattern.currentState = enemyPattern.deadState;
    }

    private void newDestination(int lastDestination)
    {
        currentDestination = Random.Range(0, enemyPattern.wayPoints.Length);
        //Breaks the game!!!!
        //if (currentDestination == lastDestination)
        //{
        //    newDestination(lastDestination);
        //}
    }
    private void playerInRange()
    {
        if (enemyPattern.endGame == true)
        {
            return;
        }
        else
        {
            for (int i = 0; i < enemyPattern.target.Length; ++i)
            {
                enemyPattern.distanceFrom = Vector3.Distance(enemyPattern.transform.position, enemyPattern.target[i].transform.position);

                if (enemyPattern.distanceFrom < enemyPattern.GetLineOfSight())
                {
                    toAttackState();
                }
                else
                {
                    if (enemyPattern.wayPoints.Length > currentDestination && enemyPattern.inChallenge == false)
                    {
                        if (Vector3.Distance(enemyPattern.transform.position, enemyPattern.wayPoints[currentDestination].transform.position) < 6)
                        {
                            newDestination(currentDestination);
                        }
                        enemyPattern.navMeshAgent.SetDestination(enemyPattern.wayPoints[currentDestination].transform.position);
                    }
                }
            }
        }
        
    }

    private void idleWalk()
    {
        //TODO make a idle walk around
    }

    public void toChallengeState()
    {
        enemyPattern.currentState = enemyPattern.challengeState;

    }
}
