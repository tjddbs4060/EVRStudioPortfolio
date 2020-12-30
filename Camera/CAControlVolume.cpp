void UCAControlVolume::SetupPrepare()
{
    Super::SetupPrepare();

    auto character = GetCharacter();
    if ( ensure( character != nullptr ) ) {
        CachedInterface = CastChecked<ICameraControlInterface>( character );
        CachedInterface->RegisterOverlappedActions( 
            [this]( ATriggerVolume* volume ){
                auto overlappedVolue = Cast<ACAControlOverlappedVolume>( volume );

                if ( ControlState == ECAControlState::CACState_None ) {
                    CameraActorControl->ChangeControlTransition( this );

                    FCAControlTransitionInfo transitionInfo( overlappedVolue->GetCameraFOV(), overlappedVolue->GetSpringArmLength(), overlappedVolue->GetSpringArmSocketOffset(), 
                        /*OriginSpringArmLocation + */FVector(0.0f, 0.0f, overlappedVolue->GetSpringArmLocationZ()) );
					transitionInfo = transitionInfo + OriginTransitionInfo;
					
                    CameraActorControl->SetInVolume( true, overlappedVolue->GetCameraFOV(), overlappedVolue->GetSpringArmLength(), overlappedVolue->GetSpringArmSocketOffset(), overlappedVolue->GetSpringArmLocationZ());
					if (CameraActorControl->IsMirrored()) {
						transitionInfo.SocketOffset.Y *= -1.0f;
						transitionInfo.SpringArmCompLocation.Y *= -1.0f;
					}
                    LOG_EVREXT( Log, TEXT( "Volume control prev state: [%s]" ), *EVR_ENUM_TO_STRING( ECAControlState, ControlState ) );
                    ControlState = ECAControlState::CACState_Doing;
	            
                    auto character = GetCharacter();
                    if (character && !character->IsAimingDownSightMode() ) {
                        StartTransition( transitionInfo );
                        LOG_EVREXT( Log, TEXT( "Start camera transition going to the in volume." ) );
                    }

                    GetCameraModifierInterface()->DeactiveCameraModifier( CameraModifierName::ApplyPitch );
                    return true;
                }
                return false;
            }, 
            [this]( ATriggerVolume* volume ){
                if ( ControlState == ECAControlState::CACState_Doing ) { 
                    CameraActorControl->ChangeControlTransition( this );
                    CameraActorControl->SetInVolume( false );

                    LOG_EVREXT( Log, TEXT( "Volume control prev state: [%s]" ), *EVR_ENUM_TO_STRING( ECAControlState, ControlState ) );
                    ControlState = ECAControlState::CACState_None;
					FCAControlTransitionInfo transitionInfo = OriginTransitionInfo;
					if (CameraActorControl->IsMirrored()) {
						transitionInfo.SocketOffset.Y *= -1.0f;
						transitionInfo.SpringArmCompLocation.Y *= -1.0f;
					}
                    auto character = GetCharacter();
                    if (character && !character->IsAimingDownSightMode() ) {
                        StartTransition(transitionInfo);
                        LOG_EVREXT( Log, TEXT( "Start camera transition out going the volume." ) );
                    }

                    GetCameraModifierInterface()->ActivateCameraModifier(  CameraModifierName::ApplyPitch );
                    return true;
                }
                return false;
            } );

        auto name = character->DataName.ToString();
        auto stance = EVR_ENUM_TO_STRING( ECharacterStance, ECharacterStance::Standing );
        auto gait = EVR_ENUM_TO_STRING( ECharacterGait, ECharacterGait::Running );
        auto keyValue = FString::Printf( TEXT( "%s_%s_%s_%s_%s" ), *name, TEXT( "FALSE" ), *stance, *gait, TEXT( "FALSE" ) );
        LOG_EVREXT( Log, TEXT( "Camera data find key: [%s]" ), *keyValue );
        auto data = CameraActorControl->FindGamePlayerCameraStateData( keyValue );
        if ( ensure( data != nullptr ) ) {
            FCAControlTransitionInfo transitionInfo( data->CameraFieldOfView, data->CameraTargetArmLength, 
                data->CameraSocketOffset, OriginSpringArmLocation + FVector(0.0f, 0.0f, data->SpringArmLocationOffsetZ) );
			OriginTransitionInfo = transitionInfo;
        }
        else {
            OriginTransitionInfo = CameraActorControl->CurrentTransitionInfo;
        }
    }
}

void UCAControlVolume::StartTransition( const FCAControlTransitionInfo& InTransition )
{
	// Out Volume이라면
	if (ControlState == ECAControlState::CACState_None)
	{
		SetupCurveDataToTheTimeline(OutVolumeCurve, TEXT("TransitionCurve"));
		CurrentCurve = OutVolumeCurve;
	}
	
	Super::StartTransition(InTransition);
}
void UCAControlVolume::OnTransitionFinished()
{
	Super::OnTransitionFinished();
	if (CurrentCurve.IsValid()) {
		SetupDefaultCurveData();
		CurrentCurve.Reset();
	}
}
void UCAControlVolume::InternalUpdate( float InDeltaTime )
{
    // FIXME: 상위 클래스의 PerformTransition() 함수 호출을 하지 않도록 하기 위해
    //        빈 함수로 override 함.
}
