using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AIBehaviour : MonoBehaviour {

    GameObject player;
    Task behaviourTree;
    Context behaviourState;
    GameManager gamemanager;
    public GameObject[] Waypoints;
	// Use this for initialization
	void Start () {
        player = GameObject.FindGameObjectWithTag("Player");
        gamemanager = GameObject.FindObjectOfType<GameManager>();
        Waypoints = GameObject.FindGameObjectsWithTag("Waypoint");
        behaviourTree = CreateBehaviourTree();

        behaviourState = new Context(player, gameObject, gamemanager, Waypoints);
    }

    // Update is called once per frame
    public void Update ()
    {
        behaviourTree.Behave(behaviourState);
	}

    
    Task CreateBehaviourTree()
    {
        Selector WanderDecision = new Selector("WanderDecision" ,
             new WanderWorld()//,
            // new StandStill()
            );

        Sequence JoinFlockCheck = new Sequence("JoinFlockCheck"
            // new InRangeOfFlock(),
            // new JoinFlock();
            );

        Selector WanderCheck = new Selector("wander",
            //JoinFlockCheck,
            WanderDecision
            );

        Sequence AlterPath = new Sequence("AlterPath"
            // new LookAtWeapon(),
            // new AlterPath(),
            // new WalkTowards()
            );

        Selector RandInspectWeapon = new Selector("RandInspectWeapon",
            new WalkTowards()
            // new walkTowards()
            );

        Sequence RangeCheck = new Sequence("Attack" ,
            new InRangeScript(),
            new AttackScript()
            );

        Selector RangeDecision = new Selector("inRange",
            RangeCheck,
            RandInspectWeapon);

        Sequence VisibleCheck = new Sequence("visible",
            new VisableScript(),
            RangeDecision
            );

        Selector root = new Selector("root",
            VisibleCheck,
            WanderCheck
            );

        Repeater repeater = new Repeater(root);
        return repeater;
    }

}
