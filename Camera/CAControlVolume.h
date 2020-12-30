class UCAControlVolume final : public UCAControlTransition
{
protected:
    void SetupPrepare() override;

    void InternalUpdate( float InDeltaTime ) override;
    void StartTransition( const FCAControlTransitionInfo& InTransitionInfo ) override;
	void OnTransitionFinished() override;

private:
    ECAControlState ControlState;
    class ICameraControlInterface* CachedInterface;
    FCAControlTransitionInfo OriginTransitionInfo;

	TWeakObjectPtr<class UCurveFloat> CurrentCurve;

    class UCurveFloat* OutVolumeCurve;
};
