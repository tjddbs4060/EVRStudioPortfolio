/**
 * UCharacterAction_RollingDown
 * 정의 : 캐릭터 행동 관련 액션 스크립트 시스템
 * 하는 일 :
 *		1. UMAction과 하는 일은 동일하지만 캐릭터 관련 스크립팅이 편하도록 OwnerCharacter등 필요한 캐릭터 관련 포인터들을 변수로 가지고 있다.
  */
class UCharacterAction_RollingDown : public UCharacterAction
{
protected:
	bool OnInit_Implementation(AActor* InOwnerActor) override;
	bool OnPlayAction_Implementation() override;
	void OnTickAction_Implementation(float DeltaTime) override;
	void OnEndAction_Implementation(bool bCancel) override;
	void OnNotifyAction_Implementation(FName NotifyName) override;

	bool CheckCurrentAction_Implementation(UMAction* CurrentAction, UMAction* NextAction) override;
private:
	class AMPlayerController* PlayerController;
	class ULocoAnimInstance* OwnerLocoAnimInstance;

	/** 구르기 이동 거리 */
	float RollingDistance;

	/** 구르기 방향 */
	FVector RollingDirection;

	/** 구르기 액션 실행 시, 실행중인 Upper Action */
	UMAction* UpperAction;

	/** Animation 길이 */
	float AnimationLength;
	/** Pakour로 인한 UnCrouch 실행 여부 */
	bool bUnCrouch;
};
