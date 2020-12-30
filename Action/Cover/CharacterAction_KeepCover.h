/**
  * UCharacterAction_KeepCover
 * 정의 : 캐릭터 행동 관련 액션 스크립트 시스템
 * 하는 일 :
 *		1. Cover Action 이후, Cover 상태에서의 Idle 액션
 */
class UCharacterAction_KeepCover : public UCharacterAction
{
protected:
	virtual bool OnInit_Implementation(AActor* InOwnerActor) override;
	virtual bool OnPlayAction_Implementation() override;
	virtual void OnTickAction_Implementation(float DeltaTime) override;
	virtual void OnEndAction_Implementation(bool bCancel) override;
	virtual void OnNotifyAction_Implementation(FName NotifyName) override;

	virtual bool CheckCurrentAction_Implementation(UMAction* CurrentAction, UMAction* NextAction) override;
	
private:
	ULocoAnimInstance* LocoAnimInstance;

	float CurrentDelay;
	float AdditiveDelay;
};
