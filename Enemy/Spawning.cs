using UnityEngine;
using System.Collections;

public class Spawning : MonoBehaviour {
    //Add spawn points in Inspector
    public GameObject[] spawnPoints;
    GameObject bus;
    GameObject[] endSpawnPoints;
    GameObject[] players;
    float timeNow;
    float updateTime = 0;
    //ticktime is in seconds so 1 second
    float tickTime = 1;
    int specialSpawn = 0;
	public int enemies = 0;
    int boss = 0;
    [SerializeField]
    private bool currentlySpawning = true;
    private bool inRange = false;
    private bool endGame = false;
    void Start()
    {
        players = GameObject.FindGameObjectsWithTag("Player");
        spawnPoints = GameObject.FindGameObjectsWithTag("SpawnPoint");
        endSpawnPoints = GameObject.FindGameObjectsWithTag("EndGameSpawns");
        bus = GameObject.FindGameObjectWithTag("Bus");
    }

    void Update () {
        timeNow = Time.time;
        if(currentlySpawning)
        {
            if (timeNow - updateTime > tickTime)
            {
                for (int i = 0; i < spawnPoints.Length; ++i)
                {
                    for(int j = 0; j < players.Length; j++)
                    {
                        if (Vector3.Distance(spawnPoints[i].transform.position, players[j].transform.position) < 60)
                        {
                            inRange = true;
                        }
                    }
                    if(!inRange)
                    {
                        GameObject newEnemy = null;
                        newEnemy = ObjectPool.Instance.GetFirstFreeObject("Enemy");
                        if (newEnemy != null)
                        {
                            if (newEnemy.GetComponent<Health>())
                            {
                                if (PlayerStats.Instance.level <= 1)
                                    newEnemy.GetComponent<Health>().HealthIncrease(PlayerStats.Instance.level);
                                else
                                {
                                    int newLife = (int)(PlayerStats.Instance.level / 4f);

                                    if (newLife <= 0)
                                        newLife = 1;

                                    Debug.Log("New life: " + newLife);
                                    newEnemy.GetComponent<Health>().HealthIncrease(newLife); /// 5
                                }
                            }

                            newEnemy.transform.position = spawnPoints[i].transform.position;
                            newEnemy.transform.rotation = spawnPoints[i].transform.rotation;
                            newEnemy.GetComponent<EStatePattern>().navMeshAgent.enabled = true;

                            newEnemy.SetActive(true);
                        }

                        if (specialSpawn % 150 == 0)
                        {
                            GameObject newBoss = null;
                            newBoss = ObjectPool.Instance.GetFirstFreeObject("Boss Enemy");

                            if (newBoss != null)
                            {
                                newBoss.GetComponent<Health>().HealthIncrease(PlayerStats.Instance.level); //+ 10
                                newBoss.transform.position = spawnPoints[i].transform.position;
                                newBoss.transform.rotation = spawnPoints[i].transform.rotation;
                                newBoss.GetComponent<EStatePattern>().navMeshAgent.enabled = true;

                                newBoss.SetActive(true);
                            }
                        }

                        specialSpawn += 5;
                    }

                    inRange = false;
                }
                updateTime = timeNow;
            }

            if(endGame)
            {
                for(int i = 0; i < endSpawnPoints.Length; i++)
                {
                    GameObject newEnemy = null;
                    newEnemy = ObjectPool.Instance.GetFirstFreeObject("Enemy");
                    if (newEnemy != null)
                    {
                        if (newEnemy.GetComponent<Health>())
                            newEnemy.GetComponent<Health>().HealthIncrease(PlayerStats.Instance.level);

                        newEnemy.gameObject.GetComponent<EStatePattern>().Endgame();
                        newEnemy.transform.position = spawnPoints[i].transform.position;
                        newEnemy.transform.rotation = spawnPoints[i].transform.rotation;
                        newEnemy.GetComponent<EStatePattern>().navMeshAgent.enabled = true;

                        newEnemy.SetActive(true);
                    }
                }
            }
        }
    }

    public void spawnEnemy(GameObject obj)
    {
        GameObject newEnemy = null;

        newEnemy = ObjectPool.Instance.GetFirstFreeObject("Enemy");

        //if (newEnemy.GetComponent<Health>())
        //    newEnemy.GetComponent<Health>().HealthIncrease(PlayerStats.Instance.level);

        newEnemy.transform.position = obj.transform.position;
        newEnemy.transform.rotation = obj.transform.rotation;

        newEnemy.GetComponent<EStatePattern>().navMeshAgent.enabled = true;
    }

    public GameObject spawnEnemyWithName(Transform ObjectTransform)
    {
        GameObject newEnemy = null;
        newEnemy = ObjectPool.Instance.GetFirstFreeObject("Enemy");

        if (newEnemy.GetComponent<Health>())
            newEnemy.GetComponent<Health>().HealthIncrease(PlayerStats.Instance.level);

        newEnemy.gameObject.GetComponent<EStatePattern>().SetLineOfSight(500);
        newEnemy.gameObject.GetComponent<EStatePattern>().inChallenge = true;

        newEnemy.transform.position = ObjectTransform.transform.position;
        newEnemy.transform.rotation = ObjectTransform.transform.rotation;

        newEnemy.GetComponent<EStatePattern>().navMeshAgent.enabled = true;

        return newEnemy;
    }

    public void stopSpawning()
    {
        if(currentlySpawning)
        {
            currentlySpawning = false;
        }    
    }

    public void enableSpawning()
    {
        if (!currentlySpawning)
        {
            currentlySpawning = true;
        }
    }

    public void endGameSpawn()
    {
        if(!endGame)
        {
            endGame = true;
        }
    }
}
