void ULocoAnimInstance::CalculateVariables(float DeltaSeconds)
{
	if (!MainCharacter || !LocoCharacterMovementComponent)
		return;

	if (bCinematicMode)
		return;

	// Calc AimOffset
	CalcualteAimOffset(DeltaSeconds);
	
	// MovementMode가 Walking or NavWalking일때, WalkingOnGraound 
	CalculateMirrorPoseBlendTime(DeltaSeconds);
	
	BlueprintCalculateVariables(DeltaSeconds);
}

#pragma region Additive
void ULocoAnimInstance::AnimNotify_Entered_AdditiveTag_Implementation()
{
	if (MDebugCVars::IsAnimNotityLog())
		UE_LOG(EVR, Log, TEXT("[ULocoAnimInstance::AnimNotify_Entered_AdditiveTag]"));
	
	CurrentAdditiveTag = FGameplayTag();
	NextAdditiveTag = CurrentAdditiveTag;
}

void ULocoAnimInstance::AnimNotify_Left_AdditiveTag_Implementation()
{
	if (MDebugCVars::IsAnimNotityLog())
		UE_LOG(EVR, Log, TEXT("[ULocoAnimInstance::AnimNotify_Left_AdditiveTag]"));
	
	CurrentAdditiveTag = NextAdditiveTag;
}

void ULocoAnimInstance::SetAdditiveTag(FGameplayTag Tag)
{
	NextAdditiveTag = Tag;
}
#pragma endregion
