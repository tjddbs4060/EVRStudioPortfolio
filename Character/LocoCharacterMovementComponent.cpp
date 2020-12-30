#pragma region TurnInPlace
bool ULocoCharacterMovementComponent::TurnInPlace(float TargetYaw, float Speed)
{
	if (OwnerCharacter->IsPlayer() == false)
	{
		// AI의 경우 Turn을 할 때, 몽타주 재생
		float TurnOffset = TurnTargetYaw - TurnStartYaw;
		TurnOffset += TurnDeltaYaw < 0.f ? -GetAimingTurnInPlaceLimitDeg() : GetAimingTurnInPlaceLimitDeg();

		if (TurnOffset > 180.0f)
		{
			TurnOffset -= 360.0f;
		}
		else if (TurnOffset < -180.0f)
		{
			TurnOffset += 360.0f;
		}

		FCharacterTransitionAnimationData TransitionData;
		if (UMAnimationBlueprintFunctionLibrary::GetCharacterTransitionAnimationSelect(TEXT("AimTurn"), 0.0f, TurnOffset, TransitionData) && bAITurn == false)
		{
			static FOnMPlayAnimationCompletedDelegate _OnCompleted;
			static FOnMPlayAnimationInterruptedDelegate _OnInterrupted;
			static const FOnMPlayAnimationNotifyDelegate _OnNotifyBegin;
			static const FOnMPlayAnimationBlendOutDelegate _OnBlendOut;

			_OnCompleted.BindUFunction(this, TEXT("MontageEnd"));
			_OnInterrupted.BindUFunction(this, TEXT("MontageEnd"));
			
			UMPlayMontage* PlayToMontage = UMAnimationBlueprintFunctionLibrary::MPlayMontageNoLatentByTag(OwnerCharacter, TransitionData.AnimationTag, _OnCompleted, _OnInterrupted, _OnNotifyBegin, _OnBlendOut);

			if (PlayToMontage)
			{
				OwnerCharacter->GetLocoCharacterMovement()->ApplyWarpDefault(PlayToMontage, FVector::OneVector, FMath::Abs(TurnOffset / TransitionData.StandardAngle));
				bNeedTurn = false;
				bAITurn = true;
			}
		}
	}

	return true;
}

void ULocoCharacterMovementComponent::MontageEnd()
{
	bAITurn = false;
}
#pragma endregion

#pragma region Vehicle
void ULocoCharacterMovementComponent::SetVehicleDeltaRotation(FRotator DeltaRotaion)
{
	if (DeltaRotaion.IsZero())
		return;
	VehicleDeltaRotation = DeltaRotaion;
}
#pragma endregion
