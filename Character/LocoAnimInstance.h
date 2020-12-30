/**
* ULocoAnimInstance
* 정의 : MainCharacter의 AnimInstance.
* 하는 일 :
*		1. AnimBP에서 자주 사용하는 MainCharacter 변수 저장
*		2. 로코모션 관련된 처리
*		3. Pose복사 방식에서 마스터 AnimBP
*/
class ULocoAnimInstance : public UMAnimInstance, public ILocomotionNotifyInterface
{
#pragma region Additive
public:
	virtual void AnimNotify_Entered_AdditiveTag_Implementation() override;
	virtual void AnimNotify_Left_AdditiveTag_Implementation() override;
public:
	UFUNCTION(BlueprintCallable)
	void SetAdditiveTag(FGameplayTag Tag);

	UFUNCTION(BlueprintPure)
	bool IsValidAdditiveTag() { return CurrentAdditiveTag.IsValid(); }
protected:
	/** Locomotion Additive Type 설정(Normal,Wounded) : HP에 따라서 바뀐다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Additive")
	ECharacterLocomotionAdditiveType LocomotionAdditiveType;
protected:
	/** 일회용 Additive Sequence Tag */
	UPROPERTY(BlueprintReadOnly, Category = "Additive")
	FGameplayTag CurrentAdditiveTag;

	UPROPERTY(BlueprintReadOnly, Category = "Additive")
	FGameplayTag NextAdditiveTag;
#pragma endregion

};
