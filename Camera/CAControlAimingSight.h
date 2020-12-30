class UCAControlAimingSight final : public UCAControlTransition
{
protected:
    void PerformTransition( ECameraTransitionType InTransitionType ) override;

private:
    void ExecuteTransitionStrategy( ECameraTransitionType InTransitionType );
    void ChangeModifierStateAndCameraLag( ECameraTransitionType InTransitionType );
};
