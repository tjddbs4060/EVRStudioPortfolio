void UCAControlCrouch::Configure( UCameraActorControl* InControl )
{
    Super::Configure( InControl );

    SetupDifferenceHeight();
    SetupEventHandler();
}

void UCAControlCrouch::SetupDifferenceHeight()
{
    if ( ensure( CameraActorControl.IsValid() ) ) {
        auto capsuleComp = Cast<UCapsuleComponent>( CameraActorControl->GetTargetActor()->GetRootComponent() );
        auto movementComp = UtilsHelper::FindChildActorComponentBy<UCharacterMovementComponent>( CameraActorControl->GetTargetActor() );
        if ( ensure( capsuleComp != nullptr ) && ensure( movementComp != nullptr ) )
            DifferenceHeight = capsuleComp->GetUnscaledCapsuleHalfHeight() - movementComp->CrouchedHalfHeight;
    }
}

void UCAControlCrouch::SetupEventHandler()
{
	auto character = GetCharacter();
	if (ensure(character != nullptr)) {
		character->RegisterHandler(TEXT("Crouch"), [this](const FEventHandleParam* param) {
			if (ensure(param->GetClassName().Equals(TNameOf<FCameraActorCrouchParam>::GetName()))) {
				auto crouchParam = static_cast<const FCameraActorCrouchParam*>(param);
				bCrouch = crouchParam->GetCrouched();

				CameraActorControl->ChangeControlTransition(this, true);
			}
		});

		FEventHandleParamFactory::Get()->RegisterFactory(TNameOf<FCameraActorCrouchParam>::GetName(),
			[] {
			return new FCameraActorCrouchParam();
		});
	}
}

void UCAControlCrouch::ModifyTransition()
{
	Super::ModifyTransition();

	auto currentLocation = SourceTransitionInfo.SpringArmCompLocation;
	auto currentLocationZ = currentLocation.Z;
	auto crouchedOffsetZ = currentLocationZ + DifferenceHeight;
	auto uncrouchedOffsetZ = currentLocationZ - DifferenceHeight;
	LOG_EVREXT(Log, TEXT("Crouch Location Info: Current:[%f] Crouch Offset:[%f] Uncrouch Offset:[%f] DifferenceHeight:[%f]"),
		currentLocationZ, crouchedOffsetZ, uncrouchedOffsetZ, DifferenceHeight);

	float offsetZ = bCrouch
		? UKismetMathLibrary::MapRangeUnclamped(currentLocationZ, uncrouchedOffsetZ, currentLocationZ, currentLocationZ, crouchedOffsetZ)
		: UKismetMathLibrary::MapRangeUnclamped(currentLocationZ, currentLocationZ, crouchedOffsetZ, uncrouchedOffsetZ, currentLocationZ);
	currentLocation.Z = offsetZ;
	SourceTransitionInfo.SpringArmCompLocation = currentLocation;
	SpringArmComp->SetRelativeLocation(currentLocation);
}

void UCAControlCrouch::Update(float InDeltaTime)
{
    Super::Update( InDeltaTime );

    // FIXME: 카메라 업데이트와 스프링암 업데이트간 동기화 문제로 한프레임 튀는 현상이 있음.
    //        그래서 회전과 이동 동기화는 LocoCharacterMovementComponent에서 하고, Crouch시에는
    //        여기서 transform 동기화를 한번 더 해 줌.
	if (CurrentState == ECAControlState::CACState_Begin)
		GetCharacter()->PerformCameraActor(InDeltaTime);
}
