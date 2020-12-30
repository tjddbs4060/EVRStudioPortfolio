void UCAControlAimingSight::PerformTransition( ECameraTransitionType InTransitionType )
{
    if ( IsEqualTransitionType( InTransitionType ) ) {
        ChangeModifierStateAndCameraLag( InTransitionType );
        ExecuteTransitionStrategy( InTransitionType );
    }
}

void UCAControlAimingSight::ChangeModifierStateAndCameraLag( ECameraTransitionType InTransitionType )
{
    if ( SpringArmComp.IsValid() ) {
        if ( InTransitionType == ECameraTransitionType::CT_AimingIn ) {
            GetCameraModifierInterface()->DeactiveCameraModifier( CameraModifierName::ApplyPitch );
            SpringArmComp->bEnableCameraLag = !CameraActorControl->IsRiding() ? false : SpringArmComp->bEnableCameraLag;
        }

        if ( InTransitionType == ECameraTransitionType::CT_AimingOut ){
            GetCameraModifierInterface()->ActivateCameraModifier( CameraModifierName::ApplyPitch );
            SpringArmComp->bEnableCameraLag = !CameraActorControl->IsRiding() ? true : SpringArmComp->bEnableCameraLag;
        }
    }
}

void UCAControlAimingSight::ExecuteTransitionStrategy( ECameraTransitionType InTransitionType )
{
	CameraActorControl->ChangeControlTransition(this);
	auto key = CameraActorControl->GetFindKeyFromCharacter();
	auto data = CameraActorControl->FindGamePlayerCameraStateData(key);
	if (data != nullptr) {
		float FOV = InTransitionType == ECameraTransitionType::CT_AimingIn ? CameraActorControl->GetFirstPersonViewFOV() : data->CameraFieldOfView;

		float TargetArmLength = 0.0f;
		FVector SocketOffset = FVector::ZeroVector;
		FVector SpringArmLocation = FVector::ZeroVector;

		TargetArmLength = data->CameraTargetArmLength;
		SocketOffset = data->CameraSocketOffset;
		SpringArmLocation = OriginSpringArmLocation + FVector(0.0f, 0.0f, data->SpringArmLocationOffsetZ);

		FCAControlTransitionInfo transitionInfo(FOV, TargetArmLength, SocketOffset, SpringArmLocation);
		if (CameraActorControl->IsMirrored()) {
			transitionInfo.SocketOffset.Y *= -1.0f;
			transitionInfo.SpringArmCompLocation.Y *= -1.0f;
		}

		StartTransition(transitionInfo);
	}
}
