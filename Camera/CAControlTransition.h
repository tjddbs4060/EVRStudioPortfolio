enum class ECAControlState : uint8
{
	CACState_None,
	CACState_Begin,
	CACState_Doing,
	CACState_End
};

class UCAControlTransition : public UObject
{
public:
	virtual void Configure(class UCameraActorControl* InControl);
	virtual void Update(float InDeltaTime);
	virtual void ChangeTransitionCurve(class UCAControlTransition* BeforeTransition);
	virtual FName GetTransitionName() const;

	bool IsEqualTransitionType(ECameraTransitionType InTransitionType) const;
	bool IsTransitionPlaying() { return (CurrentState == ECAControlState::CACState_Begin || CurrentState == ECAControlState::CACState_Doing); }

	void Stop();

protected:
	virtual void SetupPrepare();
	virtual void SetupTimeline();

	void SetupCurveDataToTheTimeline(class UCurveFloat* InCurveData, const FName& InCurveName);
	void SetTransitionInfoToSpringArmComponent();
	class ICameraModifierInterface* GetCameraModifierInterface() const;
	class USpringArmComponent* GetSpringArmComponent() const;
	class AMainCharacter* GetCharacter() const;

	virtual void InternalUpdate(float InDeltaTime);
	virtual void PerformTransition(ECameraTransitionType InTransitionType);
	virtual void StartTransition(const FCAControlTransitionInfo& InTransitionInfo);
	virtual void ModifyTransition();

    UFUNCTION()
    virtual void OnTransition( float InValue );
    UFUNCTION()
    virtual void OnTransitionFinished();
	
	virtual void OnTransitionFinishedCencle() { }
	virtual void OnTransitionFinishedCompleted() { }

    //-------------------------------------------------------------------------

    void SetupDefaultCurveData();

public:
	FCAControlTransitionInfo TargetTransitionInfo;
	FCAControlTransitionInfo SourceTransitionInfo;

protected:
    ECameraTransitionType TransitionType;
    class UCurveFloat* TransitionCurve;

	ECAControlState CurrentState;
	FVector OriginSpringArmLocation;
	TFunction<void()> OnFinished;

	TSharedPtr<struct FTimeline> Timeline;
	TWeakObjectPtr<class UCameraActorControlCharacter> CameraActorControl;
	TWeakObjectPtr<class UMSpringArmComponent> SpringArmComp;

public:
	class UCurveFloat* GetTransitionCurve() { return TransitionCurve; }
	uint8 GetTransitionPriority();

};
