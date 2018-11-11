using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public enum TaskStatus
{
    FAILURE,
    SUCCESS,
    RUNNING
}

public abstract class BehaviourState
{
}


public abstract class Task
{
    public bool starting = true;
    protected bool debug = false;
    public int ticks = 0;
    public static List<string> debugTypeBlacklist = new List<string>() { "Selector", "Sequence", "Repeater", "Inverter", "Succeeder" };
    public virtual TaskStatus Behave(BehaviourState state)
    {
        TaskStatus status = OnBehave(state);

        if(debug && !debugTypeBlacklist.Contains(GetType().Name))
        {
            string result = "Unknown";
            switch(status)
            {
                case TaskStatus.SUCCESS:
                    result = "success";
                    break;
                case TaskStatus.FAILURE:
                    result = "failure";
                    break;
                case TaskStatus.RUNNING:
                    result = "running";
                    break;
                
            }
            Debug.Log("Behaving: " + GetType().Name + " - " + result);
        }
        ticks++;
        starting = false;

        if (status != TaskStatus.RUNNING)
            Reset();


        return status;
    }

    public abstract TaskStatus OnBehave(BehaviourState state);
    public void Reset()
    {
        starting = true;
        ticks = 0;
        OnReset();
    }

    public abstract void OnReset();
    
}


public abstract class Composite : Task
{
    protected List<Task> children = new List<Task>();
    public string CompositeName;

    public Composite(string name, params Task[] tasks)
    {
        CompositeName = name;
        children.AddRange(tasks);
    }

    public override TaskStatus Behave(BehaviourState state)
    {
        bool shouldLog = debug && ticks == 0 ? true : false;
        if (shouldLog)
            Debug.Log("Running behaviour list: " + CompositeName);

        TaskStatus status = base.Behave(state);

        if (debug && status != TaskStatus.RUNNING)
            Debug.Log("Behaviour list " + CompositeName + " returned: " + status.ToString());

        return status;
    }
}

public abstract class Leaf : Task
{

}

public abstract class Decorator : Task
{
    protected Task child;

    public Decorator(Task task)
    {
        child = task;
    }
}

public class Selector : Composite
{
    int currentChild = 0;

    public Selector(string compositeName, params Task[] tasks) : base(compositeName, tasks)
    {

    }

    public override TaskStatus OnBehave(BehaviourState state)
    {
        if(currentChild >= children.Count)
        {
            return TaskStatus.FAILURE;
        }

        TaskStatus status = children[currentChild].Behave(state);

        switch(status)
        {
            case TaskStatus.SUCCESS:
                return TaskStatus.SUCCESS;

            case TaskStatus.FAILURE:
                currentChild++;

                return OnBehave(state);

        }

        return TaskStatus.RUNNING;
    }

    public override void OnReset()
    {
        currentChild = 0;
        foreach(Task child in children)
        {
            child.Reset();
        }
    }
}

public class Sequence : Composite
{
    int currentChild = 0;

    public Sequence(string compositeName, params Task[] tasks) : base(compositeName, tasks)
    {

    }

    public override TaskStatus OnBehave(BehaviourState state)
    {
        TaskStatus status = children[currentChild].Behave(state);

        switch(status)
        {
            case TaskStatus.SUCCESS:
                currentChild++;
                break;

            case TaskStatus.FAILURE:
                return TaskStatus.FAILURE;
        }

        if(currentChild >= children.Count)
        {
            return TaskStatus.SUCCESS;
        }
        else if(status == TaskStatus.SUCCESS)
        {
            return OnBehave(state);
        }

        return TaskStatus.RUNNING;
    }

    public override void OnReset()
    {
        currentChild = 0;
        foreach(Task child in children)
        {
            child.Reset();
        }
    }
}

public class Inverter : Decorator
{
    public Inverter(Task child) : base(child)
    {

    }

    public override TaskStatus OnBehave(BehaviourState state)
    {
        switch(child.Behave(state))
        {
            case TaskStatus.RUNNING:
                return TaskStatus.RUNNING;

            case TaskStatus.SUCCESS:
                return TaskStatus.FAILURE;

            case TaskStatus.FAILURE:
                return TaskStatus.SUCCESS;
        }
        Debug.Log("SHOULD NOT GET HERE");
        return TaskStatus.FAILURE;
    }

    public override void OnReset()
    {
    }
}

public class Repeater : Decorator
{
    public Repeater(Task child) : base(child)
    {
    }

    public override TaskStatus OnBehave(BehaviourState state)
    {
        TaskStatus status = child.Behave(state);
        if(status != TaskStatus.RUNNING)
        {
            Reset();
            child.Reset();
        }
        return TaskStatus.SUCCESS;
    }

    public override void OnReset()
    {
    }

}

public class Succeeder : Decorator
{
    public Succeeder(Task child) : base(child)
    {

    }

    public override TaskStatus OnBehave(BehaviourState state)
    {
        TaskStatus status = child.Behave(state);

        if (status == TaskStatus.RUNNING)
            return TaskStatus.RUNNING;

        return TaskStatus.SUCCESS;
    }

    public override void OnReset()
    {
    }
}
