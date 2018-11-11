 using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class RuleManager : MonoBehaviour {

    [Range(0, 100)] public float Multiplier;
    public int ScoreNeededToWin;

    public Text PlayerScoreText;
    public Text OtherScoreText;
    public Text RallyScoreText;

    public bool inPlay;
    public bool inGame;
    public GameObject ball;
    public GameObject PlayerBat;
    public GameObject AIBat;
    public GameObject ServeDestinationForPlayer;
    public GameObject ServeDestinationForAI;
    public GameObject ServerStartOne;
    public GameObject ServerStartTwo;
    public GameObject TutorialServe;
    public GameObject SequentialServe;
    public GameObject TrailRed;
    public GameObject TrailBlue;


    public GameObject[] PlayerAim;
    public int CurrentAim;
    //1 and 2 = p1, 3 and 4 = p2
    private int serveCounter = 1;
    private int playerScore;
    private int playerWins;

    private CanvasManager canvasManager;
    private int rallyScore;
    private int otherScore;
    private int otherWins;
    private int whoToServe = 0;
    private bool AICanServe = false;
    // 0 = none, 1 = rally, 2 = classic
    private int GamemodeChoice;
    IEnumerator AIServe()
    {
        yield return new WaitForSeconds(3f);
        Serve(ServeDestinationForAI);
    }

    IEnumerator TimeReductor(GameObject trailOne, GameObject trailTwo)
    {

        for(int i = 0; i < 50; i++ )
        {
            trailOne.GetComponent<TrailRenderer>().time -= 0.0001f;
        }

        trailOne.SetActive(false);
        trailOne.GetComponent<TrailRenderer>().time = 0.5f;
        trailTwo.GetComponent<TrailRenderer>().time = 0f;
        trailTwo.SetActive(true);
        for (int i = 0; i < 50; i++)
        {
            trailTwo.GetComponent<TrailRenderer>().time += 0.01f;
        }

        yield return null;
    }

    // Use this for initialization
    void Start ()
    {
        canvasManager = FindObjectOfType<CanvasManager>();
        inGame = false;
        inPlay = false;
        PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(true);
        ball.transform.position = ServerStartOne.transform.position;
    }

    // Update is called once per frame
    void Update ()
    {
        if (whoToServe == 1)
        {
            SequentialServe.SetActive(true);
            ball.transform.position = ServerStartOne.transform.position;
            whoToServe = 0;
        }
        if (whoToServe == 2)
        {
            ball.transform.position = ServerStartTwo.transform.position;
            whoToServe = 0;
        }

        if (inGame)
        {
            if (Vector3.Distance(ball.transform.position, PlayerBat.transform.position) < 1.0f)
            {
                if (OVRInput.Get(OVRInput.Button.PrimaryIndexTrigger) || Input.GetKeyDown(KeyCode.V))
                {
                    Serve(ServeDestinationForPlayer);
                    inGame = false;
                }
            }
            else if (Vector3.Distance(ball.transform.position, AIBat.transform.position) < 0.5f)
            {
                StartCoroutine("AIServe");
            }
        }

        if(inPlay || inGame)
        {
            //Vector2 TrackPadAxis = OVRInput.Get(OVRInput.RawButton., OVRInput.Controller.RTrackedRemote);
            //float speed = TrackPadAxis.x;

            if (OVRInput.GetUp(OVRInput.Button.PrimaryTouchpad))
            {
                Vector2 TrackPadAxis = OVRInput.Get(OVRInput.Axis2D.PrimaryTouchpad);
                if (TrackPadAxis.x > 0)
                {
                    canvasManager.AimCanvas.SetActive(false);

                    if (CurrentAim < 6)
                    {
                        //Turn old to red
                        PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(false);
                        CurrentAim += 1;
                        //Turn new to green
                        PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(true);
                    }
                }
                else if(TrackPadAxis.x < 0)
                {
                    canvasManager.AimCanvas.SetActive(false);

                    if (CurrentAim > 0)
                    {
                        //Turn old to red
                        PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(false);
                        CurrentAim -= 1;
                        //Turn new to green
                        PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(true);
                    }
                }
                TrackPadAxis = new Vector2(0,0);
            }

            if (Input.GetKeyUp(KeyCode.D))
            {
                canvasManager.AimCanvas.SetActive(false);

                if (CurrentAim < 6)
                {
                    //Turn old to red
                    PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(false);
                    CurrentAim += 1;
                    //Turn new to green
                    PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(true);
                }
            }
            else if (Input.GetKeyUp(KeyCode.A))
            {
                canvasManager.AimCanvas.SetActive(false);

                if (CurrentAim > 0)
                {
                    //Turn old to red
                    PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(false);
                    CurrentAim -= 1;
                    //Turn new to green
                    PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(true);
                }
            }

        }


    }
    public void SetUpServe()
    {
        PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(false);

        TrailRed.SetActive(false);
        TrailBlue.SetActive(false);

        whoToServe = 0;
        if(inPlay)
        {
            serveCounter += 1;
            if (serveCounter == 1 || serveCounter == 2)
            {
                whoToServe = 1;
            }
            else if (serveCounter == 3)
            {
                whoToServe = 1;
            }
            else if (serveCounter == 4)
            {
                whoToServe = 1;
                serveCounter = 0;
            }
        }
        inPlay = false;
    }

    public void Serve(GameObject destination)
    {
        TutorialServe.SetActive(false);
        SequentialServe.SetActive(false);
        inPlay = true;
        ball.GetComponent<BasicRigidBody>().playerWin = 1;
        Vector3 direction = (destination.transform.position - ball.transform.position).normalized;
        direction = direction * Multiplier;
        TrailBlue.SetActive(true);
        ball.GetComponent<BasicRigidBody>().AddForce(direction);
        PlayerAim[CurrentAim].GetComponent<AimSelect>().ChangeColour(true);
    }

    public void AddPlayerScore()
    {
        SetUpServe();
        inGame = true;
        if (GamemodeChoice == 2)
        {
            playerScore += 1;
            PlayerScoreText.text = playerScore.ToString();
        }
        
        //Reset ball to serve location
        if(GamemodeChoice == 2)
        {
            if (playerScore == ScoreNeededToWin && otherScore < 10)
            {
                //Player Win
                BackToMain();
            }
            else if (playerScore > 11 && playerScore >= otherScore + 2)
            {
                BackToMain();
            }
        }   
    }

    public void AddOtherScore()
    {
        SetUpServe();
        inGame = true;

        if(GamemodeChoice == 2)
        {
            otherScore += 1;
            OtherScoreText.text = otherScore.ToString();
        }
        
        //Reset ball to serve location
        if(GamemodeChoice == 2)
        {
            if (otherScore == ScoreNeededToWin && playerScore < 10)
            {
                //Other Win
                BackToMain();
            }
            else if (otherScore > 11 && otherScore >= playerScore + 2)
            {
                BackToMain();
            }
        }      
    }

    public void ResetScore()
    {
        playerScore = 0;
        otherScore = 0;
        rallyScore = 0;
        //Reset ball to serve location
        PlayerScoreText.text = playerScore.ToString();
        OtherScoreText.text = otherScore.ToString();
        RallyScoreText.text = rallyScore.ToString();
    }

    public void ResetWins()
    {
        playerWins = 0;
        otherWins = 0;

        //Reset ball to serve location
    }

    public void ChangeToBlue()
    {
        TrailRed.SetActive(false);
        TrailBlue.SetActive(true);
    }

    public void ChangeToRed()
    {
        TrailBlue.SetActive(false);
        TrailRed.SetActive(true);
    }

    public void BackToMain()
    {
        canvasManager.ResetMenu();
        TutorialServe.SetActive(false);
        SequentialServe.SetActive(false);
        ResetScore();
        ResetWins();
        SetGamemode(0);
        inPlay = false;
        ball.transform.position = ServerStartOne.transform.position;
        inGame = false;
        ResetScore();
    }

    public void SetGamemode(int choice)
    {
        GamemodeChoice = choice;
    }

    public void AddRallyCount()
    {
        rallyScore += 1;
        RallyScoreText.text = rallyScore.ToString();
    }

    public Vector3 GetCurrentAim()
    {
        return PlayerAim[CurrentAim].transform.position;
    }
}
