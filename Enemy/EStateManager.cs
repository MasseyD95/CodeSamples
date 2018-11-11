using UnityEngine;
using System.Collections;

public interface EStateManager {

    void updateStates();

    void OnTriggerStay(Collider other);

    void toIdleState();

    void toAttackState();

    void toDeadState();

    void toChallengeState();
}
