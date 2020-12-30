bool UCharacterAction_Cover::OnInit_Implementation(AActor* InOwnerActor)
{
	if (!Super::OnInit_Implementation(InOwnerActor))
		return false;

	return true;
}

bool UCharacterAction_Cover::OnPlayAction_Implementation()
{
	if (!Super::OnPlayAction_Implementation())
		return false;
	
	OwnerLocoCharacterMovementComponent = OwnerCharacter->GetLocoCharacterMovement();
	ensure(OwnerLocoCharacterMovementComponent);
	
	OriginRunningSpeed = OwnerCharacter->GetRunParams().MaxSpeed;

	// Action Param Parse
	TArray<FString> StringArray;
	ActionParam.Value.ParseIntoArray(StringArray, TEXT("&&"), true);

	bool bSuccessed = false;
	UKismetStringLibrary::Conv_StringToVector(StringArray[0], TargetLocation, bSuccessed);
	ensure(bSuccessed);

	UKismetStringLibrary::Conv_StringToVector(StringArray[1], ToForwardDirect, bSuccessed);
	ensure(bSuccessed);

	UKismetStringLibrary::Conv_StringToVector(StringArray[2], ToWallDirect, bSuccessed);
	ensure(bSuccessed);

	bCrouched = (bool)UKismetStringLibrary::Conv_StringToInt(StringArray[3]);

	PlayerController = UMBlueprintFunctionLibrary::GetMPlayerController(OwnerCharacter->GetWorld());

	// FIXME :> Root Motion First Frame Location으로 변경 예정
	InCoverArea = 230.0f;

	//SetLockInputMode
	if (OwnerCharacter->IsPlayer())
		PlayerController->SetLockInputMode(true, false);

	OwnerCharacterMovementComponent->bWantsToCrouch = false;
	OwnerCharacter->SetCharacterGait(ECharacterGait::Sprinting);
	ToCoverState(false);

	MainAIController = Cast<AMainAIController>(OwnerCharacter->GetController());
	if (MainAIController)
	{
		MainAIController->MoveToLocation(TargetLocation);
		MainAIController->ReceiveMoveCompleted.AddDynamic(this, &UCharacterAction_Cover::OnMoveSucceed);
	}

	// Montage 재생 영역에 들어왔는지 체크
	float Distance = FVector::Distance(OwnerCharacter->GetActorLocation(), TargetLocation);

	AppendShortString = TEXT("");
	if (Distance < InCoverArea/* + (OriginRunningSpeed * 0.5f)*/)
	{
		AppendShortString = TEXT("_Short");
		StartPositionAlpha = (InCoverArea - Distance) / InCoverArea;
		PlayInCoverMontage(Distance);
	}
	else if (Distance < InCoverArea + OriginRunningSpeed)
	{
		OwnerCharacter->SetCharacterGait(ECharacterGait::Running);
	}

	return true;
}

void UCharacterAction_Cover::OnTickAction_Implementation(float DeltaTime)
{
	Super::OnTickAction_Implementation(DeltaTime);

	if (bInCover)
		return;
	
	// 영역에 들어왔는지 체크
	float Distance = FVector::Distance(OwnerCharacter->GetActorLocation(), TargetLocation);

	if (Distance < InCoverArea + OriginRunningSpeed)
	{
		// 캐릭터 상태에 따른 처리
		if (OwnerCharacter->GetCharacterGait() == ECharacterGait::Sprinting)
		{
			// 캐릭터의 상태 Running으로 전환
			OwnerCharacter->SetCharacterGait(ECharacterGait::Running);
		}
		else if (OwnerCharacter->GetCharacterGait() == ECharacterGait::Running)
		{
			if (Distance < InCoverArea/* + (OriginRunningSpeed * 0.5f)*/)
			{
				PlayInCoverMontage(Distance);
			}
		}
		else if (OwnerCharacter->GetCharacterGait() == ECharacterGait::Walking)
		{
			if (Distance < InCoverArea)
			{
				PlayInCoverMontage(Distance);
			}
		}
	}
}

void UCharacterAction_Cover::OnEndAction_Implementation(bool bCancel)
{
	Super::OnEndAction_Implementation(bCancel);

	if (bCancel)
	{
		OwnerCharacterMovementComponent->bWantsToCrouch = bStartCrouched;
	}

	ToCoverState(true);

	if (OwnerCharacter->IsPlayer())
		PlayerController->SetLockInputMode(true, true);

	if (MainAIController)
	{
		MainAIController->StopMovement();
		if (MainAIController->ReceiveMoveCompleted.Contains(this, TEXT("OnMoveSucceed")))
			MainAIController->ReceiveMoveCompleted.RemoveDynamic(this, &UCharacterAction_Cover::OnMoveSucceed);
	}
}

void UCharacterAction_Cover::OnNotifyAction_Implementation(FName NotifyName)
{
	Super::OnNotifyAction_Implementation(NotifyName);
}

bool UCharacterAction_Cover::CheckCurrentAction_Implementation(UMAction * CurrentAction, UMAction * NextAction)
{
	if (Super::CheckCurrentAction_Implementation(CurrentAction, NextAction) == false)
		return false;

	 return true;
}

void UCharacterAction_Cover::OnMoveSucceed(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	//EndAction();
}

void UCharacterAction_Cover::OnMoveFailed(EPathFollowingResult::Type MovementResult)
{
	//EndAction();
}

void UCharacterAction_Cover::ToCoverState(bool bState)
{
	// Cover 상태로 변환 (LocoAnimation::bCoverMode)
	EMovementType MovementType = bState ? EMovementType::Cover : EMovementType::OnGround;
	OwnerCharacter->SetMovementType(MovementType);
		
	bLeftCover = (FVector::CrossProduct(ToForwardDirect, ToWallDirect).Z > 0);
	bInCover = bState;
}

void UCharacterAction_Cover::PlayInCoverMontage(float Distance)
{
	if (bPlayMontage)
		return;

	// Stop Move To
	if (MainAIController)
	{
		if (MainAIController->ReceiveMoveCompleted.Contains(this, TEXT("OnMoveSucceed")))
			MainAIController->ReceiveMoveCompleted.RemoveDynamic(this, &UCharacterAction_Cover::OnMoveSucceed);
		MainAIController->StopMovement();
	}

	// Start In Cover Animation
	FVector TargetPoint = TargetLocation - OwnerCharacter->GetActorLocation();
	TargetPoint.Normalize();
	CoverDirect = TargetPoint.Rotation();

	float InCoverAngle = UMBlueprintFunctionLibrary::BetweenVector2D_Angle(FVector2D(ToForwardDirect), FVector2D(TargetPoint));
	int32 InCoverAngleIndex = FMath::RoundToFloat(InCoverAngle / 45.0f);
	FName SectionName = FName(*(FString::FromInt(InCoverAngleIndex * 45)));
	
	FName AnimationTag;

	if (bCrouched)
	{
		// Crouch Animation
		AnimationTag = bLeftCover ? CharacterAnimationTags::C_InCover_L : CharacterAnimationTags::C_InCover_R;
	}
	else
	{
		// Stand Animation
		AnimationTag = bLeftCover ? CharacterAnimationTags::S_InCover_L : CharacterAnimationTags::S_InCover_R;
	}

	// Name None이 아닐 때, 몽타주 등록하고 Length 계산 후, Start Position으로 설정
	if (AppendShortString.Equals(TEXT("")) == false)
	{
		FName NewSectionName = FName(*(SectionName.ToString() + AppendShortString));

		ULocoAnimInstance* AnimInstance = OwnerCharacter->GetLocoAnimInstance();
		UAnimMontage* Asset = Cast<UAnimMontage>(UMAnimationBlueprintFunctionLibrary::GetCharacterAnimationByQuery(AnimInstance->GetAnimationQueryParam(), FGameplayTag::RequestGameplayTag(AnimationTag)));
		
		float ShortSectionLength = Asset->GetSectionLength((InCoverAngleIndex * 2) + 1);
		float SectionLength = Asset->GetSectionLength(InCoverAngleIndex * 2);

		float StartPosition = FMath::Clamp(FMath::Abs(SectionLength - ShortSectionLength) * StartPositionAlpha, 0.0f, 1.0f);

		PlayMontageNoLatentByTag(OwnerCharacter, FGameplayTag::RequestGameplayTag(AnimationTag), 1.0f, StartPosition, SectionName);
	}
	else
	{
		PlayMontageNoLatentByTag(OwnerCharacter, FGameplayTag::RequestGameplayTag(AnimationTag), 1.0f, 0.0f, SectionName);
	}

	AnimationPlayLength = AnimMontage->PlayLength;

	float DistanceRatio = Distance / InCoverArea;
	CoverRatio = FVector::OneVector * DistanceRatio;
	OwnerCharacterMovementComponent->ApplyWarpFacing(CoverDirect, CoverRatio);
	//OwnerCharacterMovementComponent->ApplyWarpFacingSweep(CoverDirect, FVector::OneVector);

	bPlayMontage = true;

	// 캐릭터의 Stance 상태 변경
	bStartCrouched = OwnerCharacterMovementComponent->bWantsToCrouch;
	OwnerCharacterMovementComponent->bWantsToCrouch = bCrouched;
}