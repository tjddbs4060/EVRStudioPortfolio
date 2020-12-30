// Sets default values for this component's properties
UVehicleCharacterPhysicsComponent::UVehicleCharacterPhysicsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;

	// ...
	PhysicalBodyName = CharacterBoneNames::Spine_03;

	bSimulate = false;
	EnabledBlendWeight = 0.25f;

	StrongBlendWeight = 500.0f;
	CollisionTimeCorrection = 1000.0f;
	NotFlatImpulseWeight = 2000.0f;
}


// Called when the game starts
void UVehicleCharacterPhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	Vehicle = Cast<AMWheeledVehicle>(GetOwner());
	ensure(Vehicle);

	Suspension = Cast<UVehicleSuspensionComponent>(Vehicle->GetComponentByClass(UVehicleSuspensionComponent::StaticClass()));
	ensure(Suspension);

	// ...
	SetVehiclePhysicsSimulate(false);
}


// Called every frame
void UVehicleCharacterPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (TakenCharacter == nullptr)
		return;

	UpdateCollisionBlendWeight();
	UpdateNotFlatFloorBlendWeight();
	UpdateBlendWeight(DeltaTime);
}

void UVehicleCharacterPhysicsComponent::SetVehiclePhysicsSimulate(bool Simulate)
{
	ensure(TakenCharacter);
	if (TakenCharacter == nullptr)
		return;

	if (bSimulate != Simulate)
	{
		bSimulate = Simulate;

		// Simulate is True
		if (bSimulate)
		{
			TakenCharacter->GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PhysicalBodyName, EnabledBlendWeight);
		}
		// Simulate is False
		else
		{
			TakenCharacter->GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PhysicalBodyName, 0.0f);
		}
	}
}

void UVehicleCharacterPhysicsComponent::Take(AMCharacter * Character)
{
	TakenCharacter = Character;
}

void UVehicleCharacterPhysicsComponent::UpdateCollisionBlendWeight()
{
	float CollisionValue = Suspension->GetSuspensionVelocity();

	if (CollisionValue > StrongBlendWeight)
	{
		float Time = CollisionValue / CollisionTimeCorrection;
		PhysicsLimitDelayTime = PhysicsDelayTime = Time;
	}
}

void UVehicleCharacterPhysicsComponent::UpdateNotFlatFloorBlendWeight()
{
	FVector DirectPower = Suspension->GetSuspensionDirectPower();
	if (DirectPower.Equals(FVector::ZeroVector) == false)
	{
		if (Suspension->IsImpaceEnabled())
		{
			PhysicsLimitDelayTime = PhysicsDelayTime = 1.0f;
			TakenCharacter->GetMesh()->AddImpulse(DirectPower * NotFlatImpulseWeight, PhysicalBodyName);
		}
	}
}

void UVehicleCharacterPhysicsComponent::UpdateBlendWeight(float DeltaTime)
{
	if (PhysicsDelayTime > 0)
	{
		SetVehiclePhysicsSimulate(true);

		float Weight = PhysicsDelayTime * (PhysicsDelayTime / PhysicsLimitDelayTime);
		Weight = FMath::Clamp<float>(Weight, 0.0f, EnabledBlendWeight);
		TakenCharacter->GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PhysicalBodyName, Weight);

		PhysicsDelayTime -= DeltaTime;
	}
	else
	{
		SetVehiclePhysicsSimulate(false);
	}
}
