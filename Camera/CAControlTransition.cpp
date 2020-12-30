void UCAControlTransition::Configure(UCameraActorControl* InControl)
{
	if (ensure(InControl != nullptr)) {
		CameraActorControl = CastChecked<UCameraActorControlCharacter>(InControl);
		Timeline = MakeShareable<FTimeline>(new FTimeline());
	}

	SetupPrepare();
	SetupTimeline();
}

void UCAControlTransition::Stop()
{
	if (Timeline.IsValid() && Timeline->IsPlaying()) {
		Timeline->Stop();
		if (OnFinished)
			OnFinished();
	}
}

void UCAControlTransition::SetupPrepare()
{
	SpringArmComp = Cast<UMSpringArmComponent>(GetSpringArmComponent());
	if (ensure(SpringArmComp.IsValid()))
		OriginSpringArmLocation = SpringArmComp->GetRelativeTransform().GetLocation();
}

void UCAControlTransition::SetupTimeline()
{
    if ( ensure( Timeline.IsValid() ) && ensure( TransitionCurve != nullptr ) ) {
        float min = 0.0f, max = 0.0f;
        TransitionCurve->GetTimeRange( min, max );

        Timeline->SetLooping( false );
        Timeline->SetTimelineLength( max );
        Timeline->SetTimelineLengthMode( ETimelineLengthMode::TL_TimelineLength );

        FOnTimelineFloat onTimelineFloatCallback;
        onTimelineFloatCallback.BindUFunction( this, TEXT( "OnTransition" ) );
        Timeline->AddInterpFloat( TransitionCurve, onTimelineFloatCallback, NAME_None, TEXT( "TransitionCurve" ) );

        FOnTimelineEventStatic onTimelineFinishedCallback;
        onTimelineFinishedCallback.BindUFunction( this, TEXT( "OnTransitionFinished" ) );
        Timeline->SetTimelineFinishedFunc( onTimelineFinishedCallback );

        OnFinished = [this]{ OnTransitionFinished(); };
    }
}

void UCAControlTransition::SetupCurveDataToTheTimeline(UCurveFloat* InCurveData, const FName& InCurveName)
{
	if (ensure(InCurveData != nullptr)) {
		float min = 0.0f, max = 0.0f;
		InCurveData->GetTimeRange(min, max);
		Timeline->SetFloatCurve(InCurveData, InCurveName);
		Timeline->SetTimelineLength(max);
	}
}

void UCAControlTransition::SetTransitionInfoToSpringArmComponent()
{
	SpringArmComp->SocketOffset = CameraActorControl->CurrentTransitionInfo.SocketOffset;
	SpringArmComp->SetBaseTargetArmLength(CameraActorControl->CurrentTransitionInfo.TargetArmLength);
	SpringArmComp->SetRelativeLocation(CameraActorControl->CurrentTransitionInfo.SpringArmCompLocation);
}

ICameraModifierInterface* UCAControlTransition::GetCameraModifierInterface() const
{
	if (!CameraActorControl.IsValid())
		return nullptr;

	auto cameraActor = Cast<ACameraActor>(CameraActorControl->GetOuter());
	if (!ensure(cameraActor != nullptr))
		return nullptr;

	return Cast<ICameraModifierInterface>(cameraActor);
}

USpringArmComponent* UCAControlTransition::GetSpringArmComponent() const
{
	auto cameraModifierInterface = GetCameraModifierInterface();
	return ensure(cameraModifierInterface != nullptr) ?
		cameraModifierInterface->GetSpringArmComponent(cameraModifierInterface->GetTargetActor()) : nullptr;
}

AMainCharacter* UCAControlTransition::GetCharacter() const
{
	auto cameraModifierInterface = GetCameraModifierInterface();
	return ensure(cameraModifierInterface != nullptr) ?
		Cast<AMainCharacter>(cameraModifierInterface->GetTargetActor()) : nullptr;
}

void UCAControlTransition::OnTransition( float InValue )
{
    if ( CurrentState == ECAControlState::CACState_End )
        return;

    if ( SpringArmComp.IsValid() && CameraActorControl.IsValid() ) {
        FCAControlTransitionInfo deltaInfo = TargetTransitionInfo - SourceTransitionInfo;
        CameraActorControl->CurrentTransitionInfo = SourceTransitionInfo + (deltaInfo * InValue);

        auto cameraActor = Cast<ACineCameraActorBase>( CameraActorControl->GetOuter() );
        if ( cameraActor != nullptr/* && CameraActorControl->IsAiming()*/)
            cameraActor->SetIngameFOV( CameraActorControl->CurrentTransitionInfo.FieldOfView );

        SetTransitionInfoToSpringArmComponent();
    }

	GetCharacter()->RemainTime = 1.0f - InValue;
}

void UCAControlTransition::OnTransitionFinished()
{
	if (Timeline->GetPlaybackPosition() < Timeline->GetTimelineLength())
	{
		OnTransitionFinishedCencle();
	}
	else
	{
		OnTransitionFinishedCompleted();
	}

    LOG_EVREXT( Log, TEXT( "Finished camera transition. Type: [%s]." ), 
        *EVR_ENUM_TO_STRING( ECameraTransitionType, TransitionType ) );
    CurrentState = ECAControlState::CACState_End;
    LOG_EVREXT( Log, TEXT( "Finished camera transition Z value: [%f]" ), SpringArmComp->GetRelativeLocation().Z );
}

void UCAControlTransition::Update( float InDeltaTime )
{
    InternalUpdate( InDeltaTime );

	if (Timeline.IsValid() && Timeline->IsPlaying())
		Timeline->TickTimeline(InDeltaTime);
}

void UCAControlTransition::InternalUpdate( float InDeltaTime )
{
    if ( CameraActorControl.IsValid() ) {
        auto type = CameraActorControl->GetCameraTransitionType();
        PerformTransition( type );
    }
}

void UCAControlTransition::PerformTransition( ECameraTransitionType InTransitionType )
{
    if ( IsEqualTransitionType( InTransitionType ) ) 
	{
        CameraActorControl->ChangeControlTransition( this );
        auto key = CameraActorControl->GetFindKeyFromCharacter();
        auto data = CameraActorControl->FindGamePlayerCameraStateData( key );

        if ( data != nullptr ) 
		{
			float TargetArmLength = 0.0f;
			FVector SocketOffset = FVector::ZeroVector;
			FVector SpringArmLocation = FVector::ZeroVector;

			TargetArmLength = data->CameraTargetArmLength;
			SocketOffset = data->CameraSocketOffset;
			SpringArmLocation = OriginSpringArmLocation + FVector(0.0f, 0.0f, data->SpringArmLocationOffsetZ);

            FCAControlTransitionInfo transitionInfo( data->CameraFieldOfView, TargetArmLength, SocketOffset, SpringArmLocation);

            if ( CameraActorControl->IsMirrored() ) 
			{
                transitionInfo.SocketOffset.Y *= -1.0f;
                transitionInfo.SpringArmCompLocation.Y *= -1.0f;
            }

            StartTransition( transitionInfo );
        }
    }
}

void UCAControlTransition::SetupDefaultCurveData()
{
    SetupCurveDataToTheTimeline( TransitionCurve, TEXT( "TransitionCurve" ) );
}

bool UCAControlTransition::IsEqualTransitionType( ECameraTransitionType InTransitionType ) const
{
    return TransitionType == InTransitionType;
}

void UCAControlTransition::StartTransition( const FCAControlTransitionInfo& InTransitionInfo )
{
    SourceTransitionInfo = CameraActorControl->CurrentTransitionInfo;
    TargetTransitionInfo = InTransitionInfo;

	ModifyTransition();

	CurrentState = ECAControlState::CACState_Begin;
    Timeline->PlayFromStart();

    LOG_EVREXT( Log, TEXT( "Start camera transition. Type: [%s]" ), *EVR_ENUM_TO_STRING( ECameraTransitionType, TransitionType ));
}

void UCAControlTransition::ModifyTransition()
{
	if (GetCharacter()->IsAimingDownSightMode() && GetCharacter()->GetEquipWeapon())
	{
		TargetTransitionInfo.FieldOfView = GetCharacter()->GetFirstPersonViewFOV();
	}
}

FName UCAControlTransition::GetTransitionName() const
{
    return TEXT( "Transition" );
}

void UCAControlTransition::ChangeTransitionCurve(UCAControlTransition* BeforeTransition)
{
	if (BeforeTransition && BeforeTransition->GetTransitionPriority() < GetTransitionPriority())
	{
		SetupCurveDataToTheTimeline(BeforeTransition->GetTransitionCurve(), TEXT("TransitionCurve"));
	}
	else
	{
		SetupDefaultCurveData();
	}
}

uint8 UCAControlTransition::GetTransitionPriority()
{
	switch (TransitionType)
	{
	case ECameraTransitionType::CT_Mirror:
		return 1;
	case ECameraTransitionType::CT_AimingOut:
	case ECameraTransitionType::CT_AimingIn:
	case ECameraTransitionType::CT_Aiming :
		return 2;
	case ECameraTransitionType::CT_Stance :
	case ECameraTransitionType::CT_Gait :
		return 3;
	case ECameraTransitionType::CT_Moving :
		return 4;
	//case ECameraTransitionType::CT_None:
	//case ECameraTransitionType::CT_Default:
	//case ECameraTransitionType::CT_Volume:
	default:
		break;
	}

	return 128;
}
