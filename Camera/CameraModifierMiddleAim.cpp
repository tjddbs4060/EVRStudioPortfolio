bool UCameraModifierMiddleAim::ModifyCamera(float InDeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ModifyCamera(InDeltaTime, InOutPOV);
	auto cameraManager = Cast<AGamePlayerCameraManager>(CameraOwner);
	if (cameraManager == nullptr)
		return false;

	auto cameraModifierInterface = Cast<ICameraModifierInterface>(GetViewTarget());
	if ((cameraModifierInterface == nullptr) || !cameraModifierInterface->HasActivatedCameraModifier(CameraModifierName))
		return false;

	AMainCharacter* targetActor = Cast<AMainCharacter>(cameraModifierInterface->GetTargetActor());
	if (!targetActor || !targetActor->GetLocoAnimInstance())
		return false;

	if (targetActor->GetEquipWeapon() && targetActor->GetEquipWeapon()->GetWeaponType() == EWeaponItemType::Sniper)
		return false;

	static const float CAMERASPEED = 2.0f;

	if (targetActor->GetMovementType() != EMovementType::Cover || !targetActor->IsAiming())
	{
		if (PrevMiddleAimAlpha > 0.0f)
		{
			PrevMiddleAimAlpha -= (CAMERASPEED * InDeltaTime);

			if (PrevMiddleAimAlpha < 0.0f)
				PrevMiddleAimAlpha = 0.0f;
		}
	}
	else
	{
		AGamePlayerCameraActor* PlayerCamera = Cast<AGamePlayerCameraActor>(UMBlueprintFunctionLibrary::GetPlayerCamera((GetWorld())));
		FString NewKey = PlayerCamera->GetCameraControlKey();

		if (CurrentKey.Equals(NewKey) == false)
		{
			// Offset
			CurrentKey = NewKey;

			TArray<FString> State;
			CurrentKey.ParseIntoArray(State, TEXT("_"));

			FString StandingKey = FString::Printf(TEXT("%s_%s_Standing_%s_%s"), *State[0], *State[1], *State[3], *State[4]);
			FString CrouchingKey = FString::Printf(TEXT("%s_%s_Crouching_%s_%s"), *State[0], *State[1], *State[3], *State[4]);

			FGamePlayerCameraStateData* StandingData = PlayerCamera->FindGamePlayerCameraStateData(StandingKey);
			FGamePlayerCameraStateData* CrouchingData = PlayerCamera->FindGamePlayerCameraStateData(CrouchingKey);

			if (StandingData && CrouchingData)
			{
				FVector S_CameraSocketOffset = StandingData->CameraSocketOffset;
				FVector C_CameraSocketOffset = CrouchingData->CameraSocketOffset;

				float S_CameraArmOffset = StandingData->SpringArmLocationOffsetZ;
				float C_CameraArmOffset = CrouchingData->SpringArmLocationOffsetZ;

				CameraSocketOffset = S_CameraSocketOffset - C_CameraSocketOffset;
				CameraArmOffset = S_CameraArmOffset - C_CameraArmOffset;
			}

		}
		else if (targetActor->GetCoverSystemComponent() && 
			FMath::IsNearlyEqual(PrevMiddleAimAlpha, targetActor->GetCoverSystemComponent()->GetCurrentMiddleAimHeight()) == false)
		{
			float AimHeight = targetActor->GetCoverSystemComponent()->GetCurrentMiddleAimHeight();

			if (PrevMiddleAimAlpha < AimHeight)
			{
				PrevMiddleAimAlpha += (CAMERASPEED * InDeltaTime);

				if (PrevMiddleAimAlpha > AimHeight)
					PrevMiddleAimAlpha = AimHeight;
			}
			else if (PrevMiddleAimAlpha > AimHeight)
			{
				PrevMiddleAimAlpha -= (CAMERASPEED * InDeltaTime);

				if (PrevMiddleAimAlpha < AimHeight)
					PrevMiddleAimAlpha = AimHeight;
			}
		}
	}
	if (FMath::IsNearlyZero(PrevMiddleAimAlpha))
		return false;
	
	if (PlayerCameraManager == nullptr)
		PlayerCameraManager = UtilsHelper::PlayerCameraManager<AGamePlayerCameraManager>(GetWorld());

	if (PlayerCameraManager)
	{
		ECameraTransitionType TransitionType = PlayerCameraManager->GetLastCameraTransitionType();
		
		if ((TransitionType == ECameraTransitionType::CT_AimingIn || TransitionType == ECameraTransitionType::CT_AimingOut)
			&& targetActor->GetMovementType() == EMovementType::Cover && targetActor->IsAiming())
		{
			PrevMiddleAimAlpha = targetActor->GetCoverSystemComponent()->GetCurrentMiddleAimHeight();
		}
	}
		
	const FVector viewInfoLocation = InOutPOV.Location;
	auto springArmComponent = Cast<UMSpringArmComponent>(cameraModifierInterface->GetSpringArmComponent(targetActor));
	if (ensure(IsValid(springArmComponent)))
	{
		FVector AdditiveOffset = PrevMiddleAimAlpha * CameraSocketOffset;
		float AdditiveHeight = PrevMiddleAimAlpha * CameraArmOffset;
		float DeltaHeight = targetActor->GetStandCapsuleHeight() - targetActor->GetCharacterMovement()->CrouchedHalfHeight;
		AdditiveHeight += (PrevMiddleAimAlpha * DeltaHeight);
		InOutPOV.Location = FVector(viewInfoLocation.X, viewInfoLocation.Y, viewInfoLocation.Z + AdditiveHeight);
	}

	return false;
}
