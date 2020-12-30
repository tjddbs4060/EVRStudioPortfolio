void AMainCharacter::UpdateCharacterPhysicsAsset()
{
	if (!GetMesh())
		return;

	if (GetMesh()->PhysicsAssetOverride)
	{
		GetMesh()->SetPhysicsAsset(nullptr);
	}

	TArray<UPhysicsAsset*> PhysAssets;
	PhysAssets.Reserve(3);

	if (GetMesh()->GetPhysicsAsset())
	{
		PhysAssets.Add(GetMesh()->GetPhysicsAsset());
	}

	if (HairMeshComponent && HairMeshComponent->GetPhysicsAsset())
	{
		PhysAssets.Add(HairMeshComponent->GetPhysicsAsset());
	}

	if (BodyMeshComponent && BodyMeshComponent->GetPhysicsAsset())
	{
		PhysAssets.Add(BodyMeshComponent->GetPhysicsAsset());
	}

	if (FaceMeshComponent && FaceMeshComponent->GetPhysicsAsset())
	{
		PhysAssets.Add(FaceMeshComponent->GetPhysicsAsset());
	}

	UMBlueprintFunctionLibrary::MergePhysicsAssets(GetMesh(), PhysAssets);
}

void AMainCharacter::ChangeFaceMesh(USkeletalMesh* NewMesh)
{
	// FaceMeshComponent가 없다면 Mesh가 Face이다.
	if (FaceMeshComponent)
		ChangeCharacterSkeletalMesh(FaceMeshComponent, NewMesh);
	else
		ChangeCharacterSkeletalMesh(GetMesh(), NewMesh);
}

void AMainCharacter::ChangeHairMesh(USkeletalMesh* NewMesh)
{
	if (HairMeshComponent)
		ChangeCharacterSkeletalMesh(HairMeshComponent, NewMesh);
}

void AMainCharacter::ChangeBodyMesh(USkeletalMesh* NewMesh)
{
	if (BodyMeshComponent)
	{
		ChangeCharacterSkeletalMesh(BodyMeshComponent, NewMesh);
		UpdateCharacterHeight();
	}
}

#pragma region Camera
FTransform AMainCharacter::GetFirstPersonViewTransform() const
{
	FName ClavicleName = IsMirroredCam() ? CharacterBoneNames::Clavicle_L : CharacterBoneNames::Clavicle_R;
	FVector ClavicleLocation = GetMesh()->GetSocketTransform(ClavicleName, ERelativeTransformSpace::RTS_World).GetLocation();
	FRotator LookingRotation = FollowCameraSphere->GetComponentToWorld().Rotator();

	FTransform ViewTransform(GetLocoCharacterMovement()->GetLookingRotation(), ClavicleLocation);

	if (InventoryComponent)
	{
		AGunWeaponItem* GunItem = Cast<AGunWeaponItem>(InventoryComponent->GetEquipWeapon());

		if (GunItem)
		{
			FTransform CameraOffset = GunItem->GetCurrentData().CameraOffsetTransform;

			if (IsMirroredCam())
			{
				FVector Loaction = CameraOffset.GetLocation();

				//미러시 어깨 좌표가 L,R간 0.1f차이가 있어 보정한다.
				static float CLAVICLE_TOLERANCE = 0.14f;
				CameraOffset.SetLocation(FVector(Loaction.X, -(Loaction.Y+ CLAVICLE_TOLERANCE), Loaction.Z));
			}

			// Player Yaw 값에 의해서, First View Camera 위치와 AWM의 위치가 틀어지는 위치값 보정
			FTransform FirstViewTransform = UKismetMathLibrary::ComposeTransforms(CameraOffset, ViewTransform);
			FVector FirstViewLocation = FirstViewTransform.GetLocation();
			FVector DownVector = GunItem->GetActorRightVector();
			AWeaponPartsItem* SightsParts = GunItem->GetWeaponParts(EWeaponPartsSlot::SIGHTS);

			if (SightsParts)
				DownVector = SightsParts->GetActorUpVector();

			FirstViewTransform.SetLocation(FirstViewLocation + (DownVector * 0.55f));

			return FirstViewTransform; // UKismetMathLibrary::ComposeTransforms(CameraOffset, ViewTransform);
		}
	}

	return ViewTransform;
}

FTransform AMainCharacter::GetThirdPersonViewTransform() const
{
	FTransform ResultTransform = FTransform();
	
	if ((IsAimingDownSightMode() && GetInventoryComponent() && GetInventoryComponent()->GetEquipWeapon() && GetInventoryComponent()->GetEquipWeapon()->IsUseFirstPersonView() && IsFullyAiming())
		|| (bEndAiming && IsAiming() && IsAimingDownSightMode()))
	{
		ResultTransform = GetFirstPersonViewTransform();
	}
	else
	{
		ResultTransform = GetFollowCameraSphere()->GetComponentToWorld();

		if (GetInventoryComponent() && GetInventoryComponent()->GetEquipWeapon() && GetInventoryComponent()->GetEquipWeapon()->IsUseFirstPersonView() && IsAiming() && IsAimingDownSightMode() && RemainTime < 0.1f)
		{
			FVector BeforeLocation = ResultTransform.GetLocation();
			FVector AfterLocation = GetFirstPersonViewTransform().GetLocation();

			FVector Direct = AfterLocation - BeforeLocation;

			ResultTransform.AddToTranslation(Direct * (1.0f - RemainTime));
		}
	}

	if (GetController())
	{
		static const float PITCH_CORRECTION_MIN = 40.f;
		float ControlPitch = GetController()->GetControlRotation().Pitch;

		if (ControlPitch > 180.f)
			ControlPitch -= 360.f;
		else if (ControlPitch < -180.f)
			ControlPitch += 360.f;

		if (ControlPitch > 0.f)
		{
			static const float PITCH_CORRECTION_RATIO = 1.3f;

			if (ControlPitch > PITCH_CORRECTION_MIN)
			{
				FVector CorrectionOffset = FRotator(0.f, GetController()->GetControlRotation().Yaw, 0.f).Vector() * (-(ControlPitch - PITCH_CORRECTION_MIN) * PITCH_CORRECTION_RATIO);
				ResultTransform = UKismetMathLibrary::ComposeTransforms(ResultTransform, FTransform(CorrectionOffset));
			}
		}
		else
		{
			static const float PITCH_CORRECTION_RATIO = 2.f;

			if (ControlPitch < -PITCH_CORRECTION_MIN)
			{
				FVector CorrectionOffset = FRotator(0.f, GetController()->GetControlRotation().Yaw, 0.f).Vector() * (-(ControlPitch + PITCH_CORRECTION_MIN) * PITCH_CORRECTION_RATIO);
				ResultTransform = UKismetMathLibrary::ComposeTransforms(ResultTransform, FTransform(CorrectionOffset));
			}
		}
	}

	return ResultTransform;
}

void AMainCharacter::SetHiddenCharacterHead(bool bInHidden)
{
	if (IsLocalPlayer())
	{
		if (HairMeshComponent->bHiddenInGame == bInHidden)
			return;

		HairMeshComponent->SetHiddenInGame(bInHidden);
		FaceMeshComponent->SetHiddenInGame(bInHidden);
	}
}

float AMainCharacter::GetFirstPersonViewFOV() const
{
    const float DEFAULT_FOV = 80.0f;
    if ( InventoryComponent != nullptr ) {
        auto gunItem = Cast<AGunWeaponItem>( InventoryComponent->GetEquipWeapon() );
        return (gunItem != nullptr) ? gunItem->GetAimingFOV() : DEFAULT_FOV;
    }

    return DEFAULT_FOV;
}
#pragma endregion

#pragma region Vehicle
void AMainCharacter::SetVehicle(AMWheeledVehicle* InVehicle)
{
	if (!ensure(LocoCharacterMovementComponent))
		return;
	Vehicle = InVehicle;

	if (Vehicle)
	{
		VSComponent = Cast<UVehicleSuspensionComponent>(Vehicle->GetComponentByClass(UVehicleSuspensionComponent::StaticClass()));
		SetMovementType(EMovementType::Vehicle);
	}
	else
	{
		VSComponent = nullptr;
		SetMovementType(EMovementType::OnGround);
	}
}
#pragma endregion
