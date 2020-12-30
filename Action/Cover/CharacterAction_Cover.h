/**
 * UCharacterAction_Cover
 * 정의 : 캐릭터 행동 관련 액션 스크립트 시스템
 * 하는 일 :
 *		1. UMAction과 하는 일은 동일하지만 캐릭터 관련 스크립팅이 편하도록 OwnerCharacter등 필요한 캐릭터 관련 포인터들을 변수로 가지고 있다.
  */
class UCharacterAction_Cover : public UCharacterAction
{
protected:
	virtual bool OnInit_Implementation(AActor* InOwnerActor) override;
	virtual bool OnPlayAction_Implementation() override;
	virtual void OnTickAction_Implementation(float DeltaTime) override;
	virtual void OnEndAction_Implementation(bool bCancel) override;
	virtual void OnNotifyAction_Implementation(FName NotifyName) override;

	virtual bool CheckCurrentAction_Implementation(UMAction* CurrentAction, UMAction* NextAction) override;
protected:
	/** Move Succeed Delegate. */
	UFUNCTION()
	void OnMoveSucceed(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	/** AISplineMove Failed Delegate. */
	UFUNCTION()
	void OnMoveFailed(EPathFollowingResult::Type MovementResult);
private:
	void ToCoverState(bool bState);
	void PlayInCoverMontage(float Distance);

private:
	class UMSimpleMovetoLocation* MoveProxy;
	class AMPlayerController* PlayerController;
	class ULocoCharacterMovementComponent* OwnerLocoCharacterMovementComponent;

	// Action Param 변수
	FVector TargetLocation;
	FVector ToForwardDirect;
	FVector ToWallDirect;
	bool bCrouched;

	// Warp 변수
	FRotator CoverDirect;
	FVector CoverRatio;
	bool bPlayMontage;

	// Montage Play Length
	float AnimationPlayLength;
	float StartPositionAlpha;

	// 기존 Max Speed
	float OriginRunningSpeed;

	// In Cover Animation이 실행될 거리
	float InCoverArea;

	// In Cover Animation 실행 여부
	bool bInCover;
	// Cover 방향
	bool bLeftCover;

	// Play Montage 이전의 Crouch 상태
	bool bStartCrouched;

	/** 짧은 거리일 때의 Section Name 추가 */
	FString AppendShortString;

	class AMainAIController* MainAIController;
};
