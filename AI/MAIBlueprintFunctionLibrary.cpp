void UMAIBlueprintFunctionLibrary::CheckCoverLocation(FVector CoverLoc, FVector TargetLoc, bool bShootCover, AMCharacter* OwnerCharacter, AActor* TargetActor, bool& bEnableCover, bool& bStandCover, FVector& ExactLocation)
{
	bEnableCover = false;
	ExactLocation = CoverLoc;
	if (!OwnerCharacter || (bShootCover && !TargetActor))
		return;
	const float CHECK_LENGTH = 150.f;
	const float COVER_TRACE_ANGLE = 15.f;
	const float MINIMUM_HEIGHT = 60.0f;
	const float MAXIMUM_HEIGHT = 130.0f;
	FVector CenterLoc = CoverLoc + FVector(0.0f, 0.0f, MINIMUM_HEIGHT);
	FVector StandCenterLoc = CoverLoc + FVector(0.0f, 0.0f, MAXIMUM_HEIGHT);
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CenterLoc, TargetLoc);
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(LookAtRot);
	FVector CenterCheckLocation = CenterLoc + (ForwardVector * CHECK_LENGTH);
	FVector StandCenterCheckLocation = StandCenterLoc + (ForwardVector * CHECK_LENGTH);
	FVector RightCheckLocation = CenterLoc + (FRotator(0.f, COVER_TRACE_ANGLE, 0.f).RotateVector(ForwardVector) * CHECK_LENGTH);
	FVector LeftCheckLocation = CenterLoc + (FRotator(0.f, -COVER_TRACE_ANGLE, 0.f).RotateVector(ForwardVector) * CHECK_LENGTH);
	TArray<AActor*> Ignores;
	Ignores.Add(OwnerCharacter);
	if(TargetActor)
		Ignores.Add(TargetActor);
	FHitResult CenterTraceResult;
	FHitResult StandCenterTraceResult;
	FHitResult RightTraceResult;
	FHitResult LeftTraceResult;
	ECollisionChannel Ch = CoverTrace;
	ETraceTypeQuery CoverTraceChannel = UEngineTypes::ConvertToTraceType(Ch);
	UWorld* WorldObj = OwnerCharacter->GetWorld();
	// 커버 엄폐물 3방향 트레이스 테스트
	bool bCenterHitted = UKismetSystemLibrary::LineTraceSingle(WorldObj, CenterLoc, CenterCheckLocation, CoverTraceChannel, false, Ignores, EDrawDebugTrace::None, CenterTraceResult, true);
	if (!bCenterHitted)
		return;
	bStandCover = UKismetSystemLibrary::LineTraceSingle(WorldObj, StandCenterLoc, StandCenterCheckLocation, CoverTraceChannel, false, Ignores, EDrawDebugTrace::None, StandCenterTraceResult, true);

	bool bRightHitted = false;
	bool bLeftHitted = false;
	bRightHitted = UKismetSystemLibrary::LineTraceSingle(WorldObj, CenterLoc, RightCheckLocation, CoverTraceChannel, false, Ignores, EDrawDebugTrace::None, RightTraceResult, true);
	if (!bRightHitted)
		return;
	bLeftHitted = UKismetSystemLibrary::LineTraceSingle(WorldObj, CenterLoc, LeftCheckLocation, CoverTraceChannel, false, Ignores, EDrawDebugTrace::None, LeftTraceResult, true);
	if (!bLeftHitted)
		return;
	// 커버위치를 보정해준다.
	ExactLocation = CenterTraceResult.ImpactPoint + (CenterTraceResult.ImpactNormal * 40.0f);
	ExactLocation.Z = CoverLoc.Z;
	if (bShootCover)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OwnerCharacter);
		if (!MainCharacter)
			return;
		FVector MuzzleLoc = MainCharacter->GetShooterComponent()->GetMuzzleLocation() - MainCharacter->GetNavAgentLocation();
		ECollisionChannel VisibleCh = ECC_Visibility;
		ETraceTypeQuery ShootTraceChannel = UEngineTypes::ConvertToTraceType(VisibleCh);
		FHitResult ShootTraceResult;
		TArray<AActor*> ShootIgnores;
		ShootIgnores.Add(OwnerCharacter);
		// Half Cover
		bool bShootHitted = UKismetSystemLibrary::LineTraceSingle(WorldObj, ExactLocation + MuzzleLoc, TargetLoc, ShootTraceChannel, false, ShootIgnores, EDrawDebugTrace::None, ShootTraceResult, true, FLinearColor::Black);
		if (bShootHitted && ShootTraceResult.GetActor() == TargetActor)
		{
			bEnableCover = true;
			return;
		}
		// Left Corner Cover
		FHitResult LeftCornerHit;
		FVector CornerCenterLoc = ExactLocation + MuzzleLoc;
		FVector LeftCornerMuzzle = CornerCenterLoc + (FRotator(0.f, 90, 0.f).RotateVector(CenterTraceResult.Normal) * 100.0f);
		bShootHitted = UKismetSystemLibrary::LineTraceSingle(WorldObj, LeftCornerMuzzle, TargetLoc, ShootTraceChannel, false, ShootIgnores, EDrawDebugTrace::None, LeftCornerHit, true, FLinearColor::Blue);
		if (bShootHitted && LeftCornerHit.GetActor() == TargetActor)
		{
			bEnableCover = true;
			return;
		}
		// Right Corner Cover
		FHitResult RightCornerHit;
		FVector RightCornerMuzzle = CornerCenterLoc + (FRotator(0.f, -90, 0.f).RotateVector(CenterTraceResult.Normal) * 100.0f);
		bShootHitted = UKismetSystemLibrary::LineTraceSingle(WorldObj, RightCornerMuzzle, TargetLoc, ShootTraceChannel, false, ShootIgnores, EDrawDebugTrace::None, RightCornerHit, true, FLinearColor::Yellow);
		if (bShootHitted && RightCornerHit.GetActor() == TargetActor)
		{
			bEnableCover = true;
			return;
		}
	}
	else
	{
		bEnableCover = true;
	}
}

void UMAIBlueprintFunctionLibrary::SetCombatZone(UObject* WorldContextObject, FName WorldName, TArray<AMainCharacter*> Characters)
{
	ClearCombatZone(WorldContextObject, Characters);
	AddCombatZone(WorldContextObject, WorldName, Characters);
}

void UMAIBlueprintFunctionLibrary::AddCombatZone(UObject* WorldContextObject, FName WorldName, TArray<AMainCharacter*> Characters)
{
	if (!ensure(!WorldName.IsNone()))
		return;
	if (!ensure(0 < Characters.Num()))
		return;

	AActor* ObjectActor = OBJECTCACHE_MANAGER(WorldContextObject)->GetCachedActor(WorldName);
	if (!ensure(ObjectActor))
		return;

	ACombatZoneVolume* VolumeActor = Cast<ACombatZoneVolume>(ObjectActor);
	if (!ensure(VolumeActor))
		return;

	for (AMainCharacter* Character : Characters)
	{
		if (!ensure(Character) || !ensure(Character->GetAICharacterComponent()))
			continue;
		Character->GetAICharacterComponent()->CombatZones.Add(VolumeActor);
	}
}

void UMAIBlueprintFunctionLibrary::ClearCombatZone(UObject* WorldContextObject, TArray<AMainCharacter*> Characters)
{
	if (!ensure(0 < Characters.Num()))
		return;
	for (AMainCharacter* Character : Characters)
	{
		if (!ensure(Character) || !ensure(Character->GetAICharacterComponent()))
			continue;
		Character->GetAICharacterComponent()->CombatZones.Empty();
	}
}

bool UMAIBlueprintFunctionLibrary::ParkourSensing(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect)
{
	OutParkourInfo = FSensingParkourInfo();

	OutParkourInfo.ParkourUpType = EParkourUpType::Max;
	OutParkourInfo.ParkourDownType = EParkourDownType::None;

	return ParkourUpSensing(WorldContextObject, OutParkourInfo, ParkourStartLocation, ForwardDirect) ||
		ParkourDownSensing(WorldContextObject, OutParkourInfo, ParkourStartLocation, ForwardDirect);
}

bool UMAIBlueprintFunctionLibrary::ParkourUpSensing(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect)
{
	ParkourStartLocation += FVector(0.0f, 0.0f, 60.0f);

	if (CheckParkourUpType(WorldContextObject, OutParkourInfo, ParkourStartLocation, ForwardDirect))
	{
		FTransform CharacterParkourTransform = FTransform(ForwardDirect.Rotation(), ParkourStartLocation);

		FTransform StartWarpLocation = FTransform();
		FTransform EndWarpLocation = FTransform();

		if (CheckParkourUpWarpTrace(WorldContextObject, CharacterParkourTransform, StartWarpLocation, EndWarpLocation))
		{
			OutParkourInfo.StartWarpTransform = StartWarpLocation;
			OutParkourInfo.EndWarpTransform = EndWarpLocation;
			OutParkourInfo.bIsClimbUp = true;

			return true;
		}
	}

	return false;
}

bool UMAIBlueprintFunctionLibrary::ParkourDownSensing(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect)
{
	if (CheckParkourDownType(WorldContextObject, OutParkourInfo, ParkourStartLocation, ForwardDirect))
	{
		FTransform CharacterParkourTransform = FTransform(ForwardDirect.Rotation(), ParkourStartLocation);

		FTransform StartWarpLocation = FTransform();
		FTransform EndWarpLocation = FTransform();

		if (CheckParkourDownWarpTrace(WorldContextObject, CharacterParkourTransform, StartWarpLocation, EndWarpLocation))
		{
			OutParkourInfo.StartWarpTransform = EndWarpLocation;
			OutParkourInfo.EndWarpTransform = StartWarpLocation;
			OutParkourInfo.bIsClimbUp = false;

			return true;
		}
	}

	return false;
}

bool UMAIBlueprintFunctionLibrary::CheckParkourUpType(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect)
{
	// Default Parkour Height
	TMap<EParkourUpType, float> MinHeight;
	TMap<EParkourUpType, float> MaxHeight;

	MinHeight.Add(EParkourUpType::Max, 0.0f);
	MinHeight.Add(EParkourUpType::Stair, 40.0f);
	MinHeight.Add(EParkourUpType::Hurdle, 60.0f);
	MinHeight.Add(EParkourUpType::Wall, 110.0f);
	MinHeight.Add(EParkourUpType::NextFloor, 160.0f);

	MaxHeight.Add(EParkourUpType::Max, 40.0f);
	MaxHeight.Add(EParkourUpType::Stair, 60.0f);
	MaxHeight.Add(EParkourUpType::Hurdle, 110.0f);
	MaxHeight.Add(EParkourUpType::Wall, 160.0f);
	MaxHeight.Add(EParkourUpType::NextFloor, 210.0f);

	TMap<EParkourUpType, FVector> Min;
	TMap<EParkourUpType, FVector> Max;

	for (int32 ParkourIndex = 0; ParkourIndex <= (int32)EParkourUpType::Max; ParkourIndex++)
	{
		EParkourUpType ParkourType = (EParkourUpType)ParkourIndex;

		Min.Add(ParkourType, ParkourStartLocation + FVector(0.0f, 0.0f, MinHeight[ParkourType]));
		Max.Add(ParkourType, ParkourStartLocation + FVector(0.0f, 0.0f, MaxHeight[ParkourType]));
	}

	// Calc Parkour Type
	FHitResult MaxHit = FHitResult();
	FHitResult StairHit = FHitResult();
	FHitResult HurdleHit = FHitResult();
	FHitResult WallHit = FHitResult();
	FHitResult NextFloorHit = FHitResult();

	if (CheckParkourUpTrace(WorldContextObject, MaxHit, Min[EParkourUpType::Max], Max[EParkourUpType::Max], ForwardDirect) == false)
	{
		return false;
	}
	else if (CheckParkourUpTrace(WorldContextObject, StairHit, Min[EParkourUpType::Stair], Max[EParkourUpType::Stair], ForwardDirect) == false)
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourUpType = EParkourUpType::Stair;
		OutParkourInfo.ParkourDownType = EParkourDownType::ClimbDown;
		OutParkourInfo.ParkourTarget = MaxHit.GetComponent();

		return true;
	}
	else if (CheckParkourUpTrace(WorldContextObject, HurdleHit, Min[EParkourUpType::Hurdle], Max[EParkourUpType::Hurdle], ForwardDirect) == false)
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourUpType = EParkourUpType::Hurdle;
		OutParkourInfo.ParkourDownType = EParkourDownType::ClimbDown;
		OutParkourInfo.ParkourTarget = StairHit.GetComponent();

		return true;
	}
	else if (CheckParkourUpTrace(WorldContextObject, WallHit, Min[EParkourUpType::Wall], Max[EParkourUpType::Wall], ForwardDirect) == false)
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourUpType = EParkourUpType::Wall;
		OutParkourInfo.ParkourDownType = EParkourDownType::Falling;
		OutParkourInfo.ParkourTarget = HurdleHit.GetComponent();

		return true;
	}
	else if (CheckParkourUpTrace(WorldContextObject, NextFloorHit, Min[EParkourUpType::NextFloor], Max[EParkourUpType::NextFloor], ForwardDirect) == false)
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourUpType = EParkourUpType::NextFloor;
		OutParkourInfo.ParkourDownType = EParkourDownType::Falling;
		OutParkourInfo.ParkourTarget = WallHit.GetComponent();

		return true;
	}
	else if (CheckParkourUpTrace(WorldContextObject, MaxHit, Max[EParkourUpType::NextFloor], Max[EParkourUpType::NextFloor] + FVector(0.0f, 0.0f, 1.0f), ForwardDirect) == false)
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourUpType = EParkourUpType::NextFloor;
		OutParkourInfo.ParkourDownType = EParkourDownType::Falling;
		OutParkourInfo.ParkourTarget = NextFloorHit.GetComponent();

		return true;
	}

	return false;
}

bool UMAIBlueprintFunctionLibrary::CheckParkourUpTrace(UObject* WorldContextObject, FHitResult& OutHit, FVector Start, FVector End, FVector Forward)
{
	FVector BoxExtent = FVector(0.0f, 20.0f, 0.0f);
	float ForwardLength = 100.0f;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Sensing")), false, nullptr);

	FVector CenterLocation = (Start + End) * 0.5f;
	FVector SensingHalfHeight = (Start - End) * 0.5f;

	return WorldContextObject->GetWorld()->SweepSingleByChannel(
		OutHit, CenterLocation, CenterLocation + (Forward * ForwardLength), Forward.Rotation().Quaternion(),
		ParkourTrace, FCollisionShape::MakeBox(BoxExtent + SensingHalfHeight), TraceParams);
}

bool UMAIBlueprintFunctionLibrary::CheckParkourUpWarpTrace(UObject* WorldContextObject, FTransform CharacterParkourTransform, FTransform& StartWarpTransform, FTransform& EndWarpTransform)
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Sensing")), false, nullptr);
	FHitResult Hit = FHitResult();

	FVector CharacterLocation = CharacterParkourTransform.GetLocation();
	FQuat CharacterRotation = CharacterParkourTransform.GetRotation();

	// Parkour Node 위치에서 벽으로 Trace Check
	if (WorldContextObject->GetWorld()->SweepSingleByChannel(
		Hit, CharacterLocation, CharacterLocation + CharacterRotation.GetForwardVector() * 100.0f,
		FQuat(), ParkourTrace, FCollisionShape::MakeBox(FVector::ZeroVector), TraceParams))
	{
		FVector EndDirectVector = Hit.TraceEnd - Hit.TraceStart;
		EndDirectVector.Normalize();

		// Trace 된 위치로부터 벽 위의 Location
		if (WorldContextObject->GetWorld()->SweepSingleByChannel(
			Hit, Hit.ImpactPoint + CharacterRotation.Rotator().Vector() + CharacterRotation.GetUpVector() * 210.0f,
			Hit.ImpactPoint + CharacterRotation.Rotator().Vector(),
			FQuat(), ParkourTrace, FCollisionShape::MakeBox(FVector::ZeroVector), TraceParams))
		{
			StartWarpTransform = FTransform(EndDirectVector.Rotation(), Hit.ImpactPoint);
			EndWarpTransform = FTransform((-EndDirectVector).Rotation(), CharacterLocation);
		}

		return true;
	}

	return false;
}
bool UMAIBlueprintFunctionLibrary::CheckParkourDownType(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect)
{
	// Default Parkour Height
	TMap<EParkourDownType, float> MinHeight;
	TMap<EParkourDownType, float> MaxHeight;

	MinHeight.Add(EParkourDownType::Max, 0.0f);
	MinHeight.Add(EParkourDownType::ClimbDown, 40.0f);
	MinHeight.Add(EParkourDownType::Falling, 110.0f);
	MinHeight.Add(EParkourDownType::None, 210.0f);

	MaxHeight.Add(EParkourDownType::Max, 40.0f);
	MaxHeight.Add(EParkourDownType::ClimbDown, 110.0f);
	MaxHeight.Add(EParkourDownType::Falling, 210.0f);
	MaxHeight.Add(EParkourDownType::None, 1000.0f);

	TMap<EParkourDownType, FVector> Min;
	TMap<EParkourDownType, FVector> Max;

	for (int32 ParkourIndex = 0; ParkourIndex <= (int32)EParkourDownType::Max; ParkourIndex++)
	{
		EParkourDownType ParkourType = (EParkourDownType)ParkourIndex;

		Min.Add(ParkourType, ParkourStartLocation - FVector(0.0f, 0.0f, MinHeight[ParkourType]));
		Max.Add(ParkourType, ParkourStartLocation - FVector(0.0f, 0.0f, MaxHeight[ParkourType]));
	}

	// Calc Parkour Type
	FHitResult MaxHit = FHitResult();
	FHitResult StairHit = FHitResult();
	FHitResult HurdleHit = FHitResult();
	FHitResult WallHit = FHitResult();
	FHitResult NextFloorHit = FHitResult();

	if (CheckParkourDownTrace(WorldContextObject, MaxHit, Min[EParkourDownType::Max], Max[EParkourDownType::Max], ForwardDirect))
	{
		return false;
	}
	if (CheckParkourDownTrace(WorldContextObject, MaxHit, Min[EParkourDownType::ClimbDown], Max[EParkourDownType::ClimbDown], ForwardDirect))
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourDownType = EParkourDownType::ClimbDown;
		OutParkourInfo.ParkourTarget = MaxHit.GetComponent();

		return true;
	}
	else if (CheckParkourDownTrace(WorldContextObject, StairHit, Min[EParkourDownType::Falling], Max[EParkourDownType::Falling], ForwardDirect))
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourDownType = EParkourDownType::Falling;
		OutParkourInfo.ParkourTarget = MaxHit.GetComponent();

		return true;
	}
	else if (CheckParkourDownTrace(WorldContextObject, StairHit, Min[EParkourDownType::None], Max[EParkourDownType::None], ForwardDirect))
	{
		OutParkourInfo.bIsParkour = true;
		OutParkourInfo.ParkourDownType = EParkourDownType::Falling;
		OutParkourInfo.ParkourTarget = MaxHit.GetComponent();

		return true;
	}

	return false;
}

bool UMAIBlueprintFunctionLibrary::CheckParkourDownTrace(UObject* WorldContextObject, FHitResult& OutHit, FVector Start, FVector End, FVector Forward)
{
	FVector BoxExtent = FVector(20.0f, 20.0f, 0.0f);
	float ForwardLength = 100.0f;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Sensing")), false, nullptr);

	FVector StartLocation = Start + (Forward * ForwardLength);
	FVector EndLocation = End + (Forward * ForwardLength);
	
	return WorldContextObject->GetWorld()->SweepSingleByChannel(
		OutHit, StartLocation, EndLocation, Forward.Rotation().Quaternion(),
		ParkourTrace, FCollisionShape::MakeBox(BoxExtent), TraceParams);
}

bool UMAIBlueprintFunctionLibrary::CheckParkourDownWarpTrace(UObject* WorldContextObject, FTransform CharacterParkourTransform, FTransform& StartWarpTransform, FTransform& EndWarpTransform)
{
	float ForwardLength = 100.0f;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Sensing")), false, nullptr);
	FHitResult Hit = FHitResult();

	FVector CharacterLocation = CharacterParkourTransform.GetLocation();
	FQuat CharacterRotation = CharacterParkourTransform.GetRotation();

	FVector StartLocation = CharacterLocation + (CharacterRotation.GetForwardVector() * ForwardLength);

	// 일정 거리 앞에서 Floor 방향으로 Trace
	if (WorldContextObject->GetWorld()->SweepSingleByChannel(
		Hit, StartLocation, StartLocation - FVector(0.0f, 0.0f, 1000.0f),
		FQuat(), ParkourTrace, FCollisionShape::MakeBox(FVector::ZeroVector), TraceParams))
	{
		EndWarpTransform = FTransform(CharacterRotation.Inverse(), Hit.ImpactPoint);
	}
	else
	{
		EndWarpTransform = FTransform(CharacterRotation.Inverse(), StartLocation - FVector(0.0f, 0.0f, 1000.0f));
	}

	StartWarpTransform = FTransform(CharacterRotation, CharacterLocation);

	return true;
}
