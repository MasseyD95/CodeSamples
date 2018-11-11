using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class CanvasManager : MonoBehaviour {

    public GameObject MainCanvas;
    public GameObject OptionCanvas;
    public GameObject GameModeCanvas;
    public GameObject ScoreBoardCanvas;
    public GameObject RallyCounterCanvas;
    public GameObject AimCanvas;
    [SerializeField] private int CurrentCanvas;
    //MainCanvas
    public Button StartButtonMC;
    public Button OptionsButtonMC;
    public Button QuitButtonMC;
    
    //OptionsCanvas
    public Button MusicButtonOC;
    public Button BackButtonOC;

    //Gamemode Canvas
    public Button ClassicButtonGMC;
    public Button RallyButtonGMC;
    public Button BackButtonGMC;

    public Button BackToMainSBC;

    public Button BackToMainRCC;

    private RuleManager ruleManager;
    private AudioSource backgroundMusic;

    private AIBatScript bat;
    void Start()
    {
        bat = FindObjectOfType<AIBatScript>();
        ruleManager = GetComponent<RuleManager>();
        backgroundMusic = GetComponent<AudioSource>();
        CurrentCanvas = 1;

        StartButtonMC.onClick.AddListener(MCStartOnClick);
        OptionsButtonMC.onClick.AddListener(MCOptionsOnClick);
        QuitButtonMC.onClick.AddListener(MCQuitOnClick);

        MusicButtonOC.onClick.AddListener(OCMusicOnClick);
        BackButtonOC.onClick.AddListener(BackButtonOnClick);

        ClassicButtonGMC.onClick.AddListener(GMCClassicOnClick);
        RallyButtonGMC.onClick.AddListener(GMCRallyOnClick);
        BackButtonGMC.onClick.AddListener(BackButtonOnClick);

        BackToMainSBC.onClick.AddListener(BacktoMenuOnClick);
        BackToMainRCC.onClick.AddListener(BacktoMenuOnClick);
    }

    void MCStartOnClick()
    {
        MainCanvas.SetActive(false);
        GameModeCanvas.SetActive(true);
        CurrentCanvas = 3;
    }

    void MCOptionsOnClick()
    {
        MainCanvas.SetActive(false);
        OptionCanvas.SetActive(true);
        CurrentCanvas = 2;
    }

    void MCQuitOnClick()
    {
        Application.Quit();
    }

    void OCMusicOnClick()
    {
        //ToggleMusic
        if(backgroundMusic.isPlaying)
        {
            backgroundMusic.gameObject.SetActive(false);
        }
        else if (!backgroundMusic.isPlaying)
        {
            backgroundMusic.gameObject.SetActive(true);
        }
    }

    void GMCClassicOnClick()
    {
        //Start classic rules
        ruleManager.inGame = true;
        GameModeCanvas.SetActive(false);
        ScoreBoardCanvas.SetActive(true);
        ruleManager.TutorialServe.SetActive(true);
        bat.Gamemode = true;
        AimCanvas.SetActive(true);
        ruleManager.SetGamemode(2);
    }

    void GMCRallyOnClick()
    {
        //Set AI To hard and start game
        ruleManager.inGame = true;
        GameModeCanvas.SetActive(false);
        RallyCounterCanvas.SetActive(true);
        AimCanvas.SetActive(true);
        bat.Gamemode = false;
        ruleManager.TutorialServe.SetActive(true);
        ruleManager.SetGamemode(1);
    }

    void BackButtonOnClick()
    {
        if (CurrentCanvas == 2)
        {
            OptionCanvas.SetActive(false);
            MainCanvas.SetActive(true);
            CurrentCanvas = 1;
        }
        else if(CurrentCanvas == 3)
        {
            GameModeCanvas.SetActive(false);
            MainCanvas.SetActive(true);
            CurrentCanvas = 1;
        }
    }

    void BacktoMenuOnClick()
    {
        ruleManager.BackToMain();
    }

    public void ResetMenu()
    {
        OptionCanvas.SetActive(false);
        GameModeCanvas.SetActive(false);
        ScoreBoardCanvas.SetActive(false);
        RallyCounterCanvas.SetActive(false);
        AimCanvas.SetActive(false);
        
        MainCanvas.SetActive(true);
        CurrentCanvas = 1;
    }
}
