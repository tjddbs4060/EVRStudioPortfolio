bool UCharacterAction_RollingDown::OnInit_Implementation(AActor* InOwnerActor)
{
	if (!Super::OnInit_Implementation(InOwnerActor))
		return false;

	return true;
}

bool UCharacterAction_RollingDown::OnPlayAction_Implementation()
{
	if (!Super::OnPlayAction_Implementation())
		return false;

	switch (OwnerCharacter->GetMovementType())
	{
	case EMovementType::Melee:
	{
		return false;
	}
	case EMovementType::Cover:
	{
		if (OwnerCharacter->CanRollingDown() == false)
			return false;

		break;
	}
	}

	PlayerController = UMBlueprintFunctionLibrary::GetMPlayerController(OwnerCharacter->GetWorld());
	ensure(PlayerController);

	OwnerLocoAnimInstance = OwnerCharacter->GetLocoAnimInstance();
	ensure(OwnerLocoAnimInstance);

	UAnimMontage* Montage = OwnerLocoAnimInstance->GetCurrentActiveMontage();
	if (Montage)
	{
		FString MontageName = Montage->GetName();

		if (MontageName.Contains("Melee"))
		{
			return false;
		}
	}

	// 하위 슬롯 액션들을 모두 정지시킨다.
	for (int SlotIndex = (int)Slot + 1; SlotIndex < (int)EActionSlot::Max; ++SlotIndex)
	{
		UMAction* Action = ActionComponent->GetPlayingActionBySlot(EActionSlot(SlotIndex));

		if (Action == nullptr)
			continue;

		if (Action->GetActionName() == ActionTagNames::EQUIP || Action->GetActionName() == ActionTagNames::UNEQUIP)
			continue;

		ActionComponent->EndActionBySlot(EActionSlot(SlotIndex));
	}

	OwnerCharacter->GetShooterComponent()->SetAiming(EAimingType::None);

	RollingDirection = OwnerCharacterMovementComponent->GetLastInputVector();

	if (RollingDirection.Equals(FVector::ZeroVector))
		RollingDirection = OwnerCharacter->GetActorForwardVector();

	AnimMontage = PlayMontageNoLatentByTag(OwnerCharacter, FGameplayTag::RequestGameplayTag(CharacterAnimationTags::RollingDown), 1.f, 0.f, NAME_None, true, 1.f, false);
	AnimationLength = AnimMontage->PlayLength;

	float Angle = UMBlueprintFunctionLibrary::BetweenVector2D_Angle(FVector2D(OwnerCharacter->GetActorForwardVector()), FVector2D(RollingDirection));
	float Duration = 0.2f * (Angle / 180.0f);

	OwnerCharacterMovementComponent->ApplyWarpFacingSweep(AnimMontage, RollingDirection.Rotation(), FVector::OneVector, 1.0f, Duration);

	if (OwnerCharacter->GetLocoCharacterMovement())
		OwnerCharacter->GetLocoCharacterMovement()->SetRolling(true);

	return true;
}

void UCharacterAction_RollingDown::OnTickAction_Implementation(float DeltaTime)
{
	Super::OnTickAction_Implementation(DeltaTime);
	
	float Angle = UMBlueprintFunctionLibrary::BetweenVector2D_Angle(FVector2D(OwnerCharacter->GetActorForwardVector()), FVector2D(RollingDirection));

	if (Angle > 5.0f)
		OwnerCharacterMovementComponent->ApplyWarpFacingSweep(AnimMontage, RollingDirection.Rotation(), FVector::OneVector);

	if (bUnCrouch)
	{
		if (AnimationLength <= 0.5f)
		{
			OwnerCharacter->GetMCharacterMovement()->bWantsToCrouch = true;
			bUnCrouch = false;
		}
	}

	AnimationLength -= DeltaTime;
}

void UCharacterAction_RollingDown::OnEndAction_Implementation(bool bCancel)
{
	Super::OnEndAction_Implementation(bCancel);

	if (OwnerCharacter->GetLocoCharacterMovement())
		OwnerCharacter->GetLocoCharacterMovement()->SetRolling(false);

	//장착이 끝날때 사격 버튼이 눌려있을 경우 Shoot 바로 진행
	if (OwnerCharacter->IsLocalPlayer() && OwnerCharacter->GetMovementType() == EMovementType::OnGround)
	{
		if (UMBlueprintFunctionLibrary::IsKeyDown(this, InputActionKeyNames::Shoot))
		{
			const FOnActionFinishedDelegate OnFinished;
			UMActionBlueprintFunctionLibrary::PlayAction_Shoot(OwnerCharacter, OnFinished);
		}
		else
		{
			AGunWeaponItem* CurrentWeapon = Cast<AGunWeaponItem>(OwnerCharacter->GetEquipWeapon());

			// 재장전을 해야되는 상황이라면 무조건 재장전을 시켜준다.
			if (CurrentWeapon && CurrentWeapon->CanReload() && CurrentWeapon->NeedReload())
			{
				const FOnActionFinishedDelegate OnFinished;
				UMActionBlueprintFunctionLibrary::PlayAction_Reload(OwnerCharacter, OnFinished);
			}
		}

		if (UMBlueprintFunctionLibrary::IsKeyDown(this, InputActionKeyNames::Aim))
		{
			if (OwnerCharacter->GetShooterComponent() && OwnerCharacter->GetShooterComponent()->GetWeaponItem())
			{
				OwnerCharacter->GetShooterComponent()->SetAiming(EAimingType::AimingDownSight);
			}
		}
		else
		{
			if (OwnerCharacter->GetShooterComponent())
				OwnerCharacter->GetShooterComponent()->SetAiming(EAimingType::None);
		}
	}
}

void UCharacterAction_RollingDown::OnNotifyAction_Implementation(FName NotifyName)
{
	Super::OnNotifyAction_Implementation(NotifyName);

	ActionComponent->EndAction(this);
}

bool UCharacterAction_RollingDown::CheckCurrentAction_Implementation(UMAction * CurrentAction, UMAction * NextAction)
{	
	if (NextAction->GetActionTag().GetTagName() == ActionTagNames::EQUIP ||
		NextAction->GetActionTag().GetTagName() == ActionTagNames::UNEQUIP ||
		NextAction->GetActionTag().GetTagName() == ActionTagNames::RELOAD)
	{
		return true;
	}

	return false;
}