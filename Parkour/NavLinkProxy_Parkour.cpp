ANavLinkProxy_Parkour::ANavLinkProxy_Parkour(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LinkPointArea = CreateDefaultSubobject<UBoxComponent>(TEXT("LinkPointArea"));
	LinkPointArea->SetupAttachment(RootComponent);

	NodeDistance = 70.0f;
	ForwardWallLength = 50.0f;

	bSmartLinkIsRelevant = true;
}

void ANavLinkProxy_Parkour::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	for (auto LinkComp : SmartLinkComp)
	{
		if (LinkComp)
			LinkComp->SetNavigationRelevancy(bSmartLinkIsRelevant);
	}
}

void ANavLinkProxy_Parkour::DeleteInSelectIndex()
{
	if (SelectIndex >= 0)
		DeleteParkourLinkIndex(SelectIndex);
}

void ANavLinkProxy_Parkour::BothWayInSelectIndex()
{
	SetSmartLinkDirect(ENavLinkDirection::BothWays);
}

void ANavLinkProxy_Parkour::LeftToRightInSelectIndex()
{
	SetSmartLinkDirect(ENavLinkDirection::LeftToRight);
}

void ANavLinkProxy_Parkour::RightToLeftInSelectIndex()
{
	SetSmartLinkDirect(ENavLinkDirection::RightToLeft);
}

bool ANavLinkProxy_Parkour::IsPointInBoxComponent(FVector Location)
{
	return UKismetMathLibrary::IsPointInBoxWithTransform(Location, LinkPointArea->GetComponentTransform(), LinkPointArea->GetUnscaledBoxExtent());
}

void ANavLinkProxy_Parkour::SpawnParkourNodes()
{
	ClearNavLinkComponent();

	TArray<FVector> Edges;

	if (UMEditorBlueprintFunctionLibrary::GetLevelNavMeshEdge(GetWorld(), Edges) == false)
		return;

	// 기준이 되는 2개의 Edge 단위로 체크
	for (int32 i = 0; i + 1 < Edges.Num(); i += 2)
	{
		FVector StartEdge = Edges[i];
		FVector EndEdge = Edges[i + 1];

		FVector Direct = EndEdge - StartEdge;

		// 노드 간의 거리가 지정한 Node Distance 이하이면 생성하지 않음
		if (Direct.Size() < NodeDistance)
			continue;

		// 노드 사이의 길이가 길 경우, 중간 노드의 개수를 체크해서 노드 생성
		int32 MidNodeCount = Direct.Size() / NodeDistance;

		// Nav Edge Location's Forward Vector (To Wall)
		Direct.Normalize();
		FVector Forward = UKismetMathLibrary::GreaterGreater_VectorRotator(Direct, FRotator(0.0f, 90.0f, 0.0f));
		FVector Back = UKismetMathLibrary::GreaterGreater_VectorRotator(Direct, FRotator(0.0f, -90.0f, 0.0f));

		for (int32 j = 0; j < MidNodeCount; j++)
		{
			FVector CurrentLocation = StartEdge + ((Direct * NodeDistance) * j);
			
			if (CheckAndSpawnParkourNode(CurrentLocation, Forward))
			{
				// Forward Direct Spawn
			}
			else if (CheckAndSpawnParkourNode(CurrentLocation, Back))
			{
				// Back Direct Spawn
			}
		}
	}
	
	UpdateParkourLinkNodes();
}

bool ANavLinkProxy_Parkour::CheckAndSpawnParkourNode(FVector Location, FVector Forward)
{
	// Floor Location이 설정된 Box Component 안에 들어갈 경우 노드 생성 실행
	if (IsPointInBoxComponent(Location) && CheckStartLocationOverlap(Location) == false && GetFloor(Location))
	{
		const float CharacterBaseHeight = 60.0f;

		FSensingParkourInfo SensingParkourInfo = FSensingParkourInfo();

		SensingParkourInfo.ParkourUpType = EParkourUpType::Max;
		SensingParkourInfo.ParkourDownType = EParkourDownType::None;
		SensingParkourInfo.bIsClimbUp = true;

		FVector FloorLocation = Location;
		GetFloor(FloorLocation, FloorLocation);

		FVector CharacterParkourLocation = Location + FVector(0.0f, 0.0f, CharacterBaseHeight);
		FRotator CharacterParkourRotation = FRotator(0.0f, UKismetMathLibrary::MakeRotFromX(Forward).Yaw, 0.0f);

		FTransform CharacterParkourTransform = FTransform(CharacterParkourRotation, CharacterParkourLocation);

		FTransform StartWarpLocation = FTransform();
		FTransform EndWarpLocation = FTransform();

		// 중첩 Node 생성으로 인한 Down Parkour Sensing -> Update Sensing (Up Parkour Sensing) 으로 변경
		//if (UMAIBlueprintFunctionLibrary::ParkourUpSensing(GetWorld(), SensingParkourInfo, FloorLocation, Forward))
		//{
		//	// Up Sensing으로 인한 생성은, Swap으로 Climb Down & Up 사용 가능
		//	AddNavLinkComponent(SensingParkourInfo, CharacterParkourTransform, ENavLinkDirection::BothWays);

		//	return true;
		//}
		if (UMAIBlueprintFunctionLibrary::ParkourDownSensing(GetWorld(), SensingParkourInfo, FloorLocation, Forward))
		{
			// Down Sensing으로 인한 생성은, Swap으로 Climb Down & Up 사용 불가능
			AddNavLinkComponent(SensingParkourInfo, CharacterParkourTransform, ENavLinkDirection::LeftToRight);

			return true;
		}
	}

	return false;
}

void ANavLinkProxy_Parkour::AddNavLinkComponent(FSensingParkourInfo SensingParkourInfo, FTransform StartTransform, ENavLinkDirection::Type Direct)
{
	if (SensingParkourInfo.ParkourUpType == EParkourUpType::Max && SensingParkourInfo.ParkourDownType == EParkourDownType::None)
		return;

	const float ParkourMaxHeight = 210.0f;

	// DESC :> Calc Warp Start & End Location
	FVector TraceStartLocation = StartTransform.GetLocation();
	FVector TraceForward = StartTransform.GetRotation().Vector();
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Sensing")), false, this);
	FHitResult WallHit = FHitResult();

	// Start Location 에서 벽으로 Trace Check
	if (GetWorld()->SweepSingleByChannel(
		WallHit, TraceStartLocation, TraceStartLocation + TraceForward * 100.0f,
		StartTransform.GetRotation(), ParkourTrace, FCollisionShape::MakeBox(FVector::ZeroVector), TraceParams))
	{
		FHitResult EndHit = FHitResult();

		FVector HitEndLocation = WallHit.ImpactPoint + TraceForward;

		// 벽에서의 Trace 높이 체크
		if (GetWorld()->SweepSingleByChannel(
			EndHit, HitEndLocation + StartTransform.GetRotation().GetUpVector() * ParkourMaxHeight, HitEndLocation,
			StartTransform.GetRotation(), ParkourTrace, FCollisionShape::MakeBox(FVector::ZeroVector), TraceParams))
		{
			SensingParkourInfo.StartWarpTransform.SetLocation(EndHit.ImpactPoint);
			SensingParkourInfo.EndWarpTransform.SetLocation(TraceStartLocation);
		}
	}
	// Calc End

	// DESC :> Calc Smart Link Start & End Location
	const float ActorHeight = 30.0f;
	FVector StartLocation = TraceStartLocation + FVector(0.0f, 0.0f, -ActorHeight) + (TraceForward * -ForwardWallLength);
	FVector EndLocation = SensingParkourInfo.StartWarpTransform.GetLocation() + (TraceForward * ForwardWallLength) + FVector(0.0f, 0.0f, ActorHeight);
	
	FTransform InverseTransform = GetActorTransform().Inverse();
	InverseTransform.SetScale3D(FVector::OneVector);

	StartLocation = UKismetMathLibrary::TransformLocation(GetActorTransform().Inverse(), StartLocation);
	EndLocation = UKismetMathLibrary::TransformLocation(GetActorTransform().Inverse(), EndLocation);

	StartLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), StartLocation);
	EndLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), EndLocation);

	//if (FVector::Distance(StartLocation, EndLocation) > ParkourMaxHeight + ForwardWallLength + ActorHeight * 2.0f)
	//	return;

	FVector StartFloorLocation = StartLocation;
	FVector EndFloorLocation = EndLocation;

	FVector RotateVector = GetActorRotation().RotateVector(GetActorLocation());

	GetFloor(StartFloorLocation, StartFloorLocation);
	GetFloor(EndFloorLocation, EndFloorLocation);

	StartFloorLocation = UKismetMathLibrary::TransformLocation(GetActorTransform().Inverse(), StartLocation);
	EndFloorLocation = UKismetMathLibrary::TransformLocation(GetActorTransform().Inverse(), EndLocation);

	AddParkourLinkNode(SensingParkourInfo, StartFloorLocation, EndFloorLocation, Direct);
	// Calc End
}

void ANavLinkProxy_Parkour::ClearNavLinkComponent()
{
	for (UNavLinkCustomComponent* Link : SmartLinkComp)
	{
		if (Link)
		{
			Link->ConditionalBeginDestroy();
		}
	}
	SmartLinkComp.Empty();

	PointLinks.Empty();
	LinkList.Empty();
}

void ANavLinkProxy_Parkour::AddParkourLinkNode(FSensingParkourInfo SensingParkourInfo, FVector StartLocation, FVector EndLocation, ENavLinkDirection::Type Direct)
{
	UNavLinkCustomComponent* NewSmartLink = AddNewComponentWithActorComp<UNavLinkCustomComponent>(this);
	NewSmartLink->SetFlags(EObjectFlags::RF_Dynamic);
	NewSmartLink->SetNavigationRelevancy(false);

	NewSmartLink->SetLinkData(StartLocation, EndLocation, Direct);

	SmartLinkComp.Add(NewSmartLink);
	LinkList.Add(NewSmartLink, SensingParkourInfo);

	FNavigationLink DefLink;
	DefLink.SetAreaClass(UNavArea_Default::StaticClass());
	DefLink.Left = StartLocation;
	DefLink.Right = EndLocation;
	DefLink.SupportedAgents.PackedBits = 0;

	PointLinks.Add(DefLink);
}

bool ANavLinkProxy_Parkour::CheckStartLocationOverlap(FVector StartLocation)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<UPrimitiveComponent*> OutPrimitiveComponents;

	return UKismetSystemLibrary::BoxOverlapComponents(GetWorld(), StartLocation + (FVector::UpVector * 0.1f), FVector::OneVector * 0.1f, ObjectTypes, nullptr, IgnoreActors, OutPrimitiveComponents);
}

bool ANavLinkProxy_Parkour::GetFloor(FVector Location)
{
	FVector FloorLocation;

	return GetFloor(Location, FloorLocation);
}

bool ANavLinkProxy_Parkour::GetFloor(FVector Location, FVector& FloorLocation)
{
	const float FloorMaxHeight = 1000.0f;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Sensing")), false, this);
	FHitResult Hit = FHitResult();

	if (GetWorld()->SweepSingleByChannel(
		Hit, Location, Location - FVector(0.0f, 0.0f, FloorMaxHeight),
		FQuat(), ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(FVector::ZeroVector), TraceParams))
	{
		FloorLocation = Hit.ImpactPoint;
		return true;
	}

	return false;
}

bool ANavLinkProxy_Parkour::UpdateParkourData(int32 index)
{
	FVector StartLocation = PointLinks[index].Left.Z < PointLinks[index].Right.Z ? PointLinks[index].Left : PointLinks[index].Right;
	FVector EndLocation = PointLinks[index].Left.Z < PointLinks[index].Right.Z ? PointLinks[index].Right : PointLinks[index].Left;

	SmartLinkComp[index]->SetLinkData(StartLocation, EndLocation, ENavLinkDirection::BothWays);
	PointLinks[index].Left = StartLocation;
	PointLinks[index].Right = EndLocation;

	StartLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), StartLocation);
	EndLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), EndLocation);

	FVector ForwardDirect = (EndLocation - StartLocation);
	FRotator Forward = FRotator(0.0f, ForwardDirect.Rotation().Yaw, 0.0f);

	if (GetFloor(StartLocation) == false)
		return false;

	const float CharacterBaseHeight = 60.0f;
	float ForwardOffset = 0.0f;

	// Start -> End 지점까지의 Trace 체크로 오를 벽이 있는지 판별
	FHitResult Hit = FHitResult();
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Sensing")), false, this);

	FVector TraceStartLocation = StartLocation + FVector(0.0f, 0.0f, CharacterBaseHeight);
	
	if (GetWorld()->SweepSingleByChannel(
		Hit, TraceStartLocation, TraceStartLocation + (Forward.Vector() * ForwardDirect.Size()), Forward.Quaternion(),
		ParkourTrace, FCollisionShape::MakeBox(FVector::ZeroVector), TraceParams))
	{
		// Forward Length를 대비하여 여유분 계산
		if (Hit.Distance >= ForwardWallLength * 0.9f)
		{
			ForwardOffset = Hit.Distance - ForwardWallLength * 0.9f;
		}
	}
	else
	{
		return false;
	}

	FVector FloorLocation = StartLocation + Forward.Vector() * ForwardOffset;
	GetFloor(FloorLocation, FloorLocation);

	return UMAIBlueprintFunctionLibrary::ParkourUpSensing(GetWorld(), LinkList[SmartLinkComp[index]], FloorLocation, Forward.Vector());
}

void ANavLinkProxy_Parkour::SetSmartLinkDirect(ENavLinkDirection::Type NavLinkType)
{
	if (SelectIndex < 0 || SmartLinkComp.IsValidIndex(SelectIndex) == false)
		return;

	FVector LeftLocation = FVector::ZeroVector;
	FVector RightLocation = FVector::ZeroVector;
	ENavLinkDirection::Type LinkType = ENavLinkDirection::BothWays;

	SmartLinkComp[SelectIndex]->GetLinkData(LeftLocation, RightLocation, LinkType);
	
	if (LinkType != NavLinkType)
		SmartLinkComp[SelectIndex]->SetLinkData(LeftLocation, RightLocation, NavLinkType);
}

void ANavLinkProxy_Parkour::AddParkourLinkNode()
{
	// 기본 노드 생성
	FSensingParkourInfo SensingParkourInfo = FSensingParkourInfo();

	SensingParkourInfo.bIsParkour = false;
	SensingParkourInfo.ParkourUpType = EParkourUpType::Max;
	SensingParkourInfo.ParkourDownType = EParkourDownType::None;
	SensingParkourInfo.bIsClimbUp = true;

	FRotator NodeRotation = GetActorRotation();
	FVector RightVector = NodeRotation.Quaternion().GetRightVector();

	AddParkourLinkNode(SensingParkourInfo, RightVector * 30.0f, RightVector * -30.0f, ENavLinkDirection::BothWays);

	RegisterAllComponents();

	RefreshEditor();
}

void ANavLinkProxy_Parkour::UpdateParkourLinkNodes()
{
	if (PointLinks.Num() >= SmartLinkComp.Num())
	{
		for (int i = 0; i < PointLinks.Num(); i++)
		{
			if (SmartLinkComp.IsValidIndex(i) && SmartLinkComp[i])
			{
				if (UpdateParkourData(i) == false)
				{
					DeleteParkourLinkIndex(i);
					i--;
				}
			}
		}
	}

	RegisterAllComponents();

	RefreshEditor();
}

void ANavLinkProxy_Parkour::DeleteParkourLinkIndex(int32 index)
{
	if (index < PointLinks.Num() && SmartLinkComp.Num() == PointLinks.Num() && PointLinks.Num() == LinkList.Num())
	{
		PointLinks.RemoveAt(index);
		LinkList.Remove(SmartLinkComp[index]);

		SmartLinkComp[index]->ConditionalBeginDestroy();
		SmartLinkComp.RemoveAt(index);
	}
}
