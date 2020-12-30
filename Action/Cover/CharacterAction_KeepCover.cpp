bool UCharacterAction_KeepCover::OnInit_Implementation(AActor* InOwnerActor)
{
	if (!Super::OnInit_Implementation(InOwnerActor))
		return false;

	return true;
}

bool UCharacterAction_KeepCover::OnPlayAction_Implementation()
{
	if (!Super::OnPlayAction_Implementation())
		return false;

	LocoAnimInstance = OwnerCharacter->GetLocoAnimInstance();

	ensure(LocoAnimInstance);
	// TODO :> Set Additive Tag 함수로 Idle 상태의 Additive 추가
	/**
	OwnerCharacter->GetLocoAnimInstance()->SetAdditiveTag();
	*/

	return true;
}

void UCharacterAction_KeepCover::OnTickAction_Implementation(float DeltaTime)
{
	Super::OnTickAction_Implementation(DeltaTime);
}

void UCharacterAction_KeepCover::OnEndAction_Implementation(bool bCancel)
{
	Super::OnEndAction_Implementation(bCancel);
}

void UCharacterAction_KeepCover::OnNotifyAction_Implementation(FName NotifyName)
{
	Super::OnNotifyAction_Implementation(NotifyName);
}

bool UCharacterAction_KeepCover::CheckCurrentAction_Implementation(UMAction * CurrentAction, UMAction * NextAction)
{
	if (Super::CheckCurrentAction_Implementation(CurrentAction, NextAction) == false)
		return false;

	return true;
}