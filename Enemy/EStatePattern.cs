using UnityEngine;
using System.Collections;

public class EStatePattern : MonoBehaviour {


    [HideInInspector] public Transform chaseTarget;
    [HideInInspector] public EStateManager currentState;
    [HideInInspector] public EAttackState attackState;
    [HideInInspector] public EIdleState idleState;
    [HideInInspector] public EDeadState deadState;
    [HideInInspector] public EChallengeState challengeState;
    [HideInInspector] public NavMeshAgent navMeshAgent;
    [HideInInspector] public GameObject[] target;
    [HideInInspector] public float distanceFrom;
    [HideInInspector] public float distanceFromTemp;
    [HideInInspector] public bool inChallenge = false;
    [HideInInspector] public bool endGame = false;
    [HideInInspector] public int closestPlayer;
    [HideInInspector] public GameObject[] wayPoints;
    [HideInInspector] public GameObject bus;
    [HideInInspector] private Animator zombie_anim;
    //Enemy Range
    public float lineOfSight = 30;

    private void Awake()
    {
        attackState = new EAttackState(this);
        idleState = new EIdleState(this);
        deadState = new EDeadState(this);
        challengeState = new EChallengeState(this);
        zombie_anim = GetComponent<Animator>();
        navMeshAgent = GetComponent<NavMeshAgent>();
        bus = GameObject.FindGameObjectWithTag("Bus");
    }
    void Start ()
    {
        currentState = idleState;
        wayPoints = GameObject.FindGameObjectsWithTag("Waypoint");
    }

    void Update ()
    {
        target = GameObject.FindGameObjectsWithTag("Player");

        if (inChallenge == true)
        {
            SetLineOfSight(500);

        }
        else
        {
            SetLineOfSight(30);
        }
        if (currentState == null)
            Debug.Log("ERROR - currentState null");

        currentState.updateStates();
	}

    private void OnTriggerStay(Collider other)
    {
        currentState.OnTriggerStay(other);
    }

    public void SetLineOfSight(float sightValue)
    {
        lineOfSight = sightValue;
    }

    public float GetLineOfSight()
    {
        return lineOfSight;
    }

    public void toDead(bool whereFrom)
    {
        //Where from 
        //True is from player killing zombie
        //False is from challenge starting
        if(whereFrom == true)
        {
            //Also adds kill to playerstats
            if(this.GetComponent<Tracker>() != null)
            {
                PlayerStats.Instance.AddMoney(1);
            }
            else
            {
                PlayerStats.Instance.AddMoney(1);
                PlayerStats.Instance.AddKill();
            }

            //Drop Money
            GameObject newMoney = ObjectPool.Instance.GetFirstFreeObject("Money");

            newMoney.transform.position = this.transform.position + new Vector3(0.0f, 0.4f, 0.0f);

            //could call newMoney.GetComponent<Money>().SetRanger(x,y)
            // if want higher or lower reward

            newMoney.SetActive(true);
        }
        else
        {
            //Dont Add money
        }
        

		//Splatter

		

		currentState = idleState;

		if (!ObjectPool.Instance.Kill (this.gameObject)) {
            Debug.LogError("Error");
			Destroy (this.gameObject);
		}
    }

    public void SetChallenge(bool decision)
    {
        inChallenge = decision;
    }

    public void Endgame()
    {
        endGame = true;
    }
}
