// Sets default values for this component's properties
UVehicleSuspensionComponent::UVehicleSuspensionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;

	// ...
	OriginDirectPower = SuspensionDirectPower = CurForce = BeforeForce = FVector::ZeroVector;
	MinReactPower = 20.0f;
	MaxReactPower = 100.0f;

	DrawLineLength = 30.0f;

	SuspensionHeight = 10.0f;
	SuspensionDelay = 1.0f;
	OriginDelay = 0.0f;
	SteeringAngle = 0.0f;
	SuspensionVelocity = 0.0f;

	BeforeSuspensionDirect = SuspensionDirect = ESuspensionDirect::Normal;

	bImpact = false;
}


// Called when the game starts
void UVehicleSuspensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UVehicleSuspensionComponent::OnRegister()
{
	Super::OnRegister();

	Vehicle = Cast<AMWheeledVehicle>(GetOwner());
	check(Vehicle);
	World = Vehicle->GetWorld();
	VehicleMesh = Vehicle->GetMesh();
}

void UVehicleSuspensionComponent::UpdateVariable(float DeltaTime)
{
	if (Vehicle && VehicleMesh && World)
	{
		// DESC :> Vehicle Current Force Initialize
		BeforeForce = CurForce;
		CurForce = Vehicle->GetVelocity();

		// DESC :> Vehicle Wheel Floor Direct Calculate
		VehicleForward = Vehicle->GetActorForwardVector();
		VehicleRight = Vehicle->GetActorRightVector();
	}
}

void UVehicleSuspensionComponent::Calculate_SuspensionDirect(float DeltaTime)
{
}

void UVehicleSuspensionComponent::Calculate_SuspensionDirectPower(float DeltaTime)
{
}

void UVehicleSuspensionComponent::Calculate_SuspensionVelocity(float DeltaTime)
{
}

void UVehicleSuspensionComponent::Calculate_SteeringAngle(float DeltaTime)
{
}

// Called every frame
void UVehicleSuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...

	UpdateVariable(DeltaTime);
	Calculate_SuspensionDirect(DeltaTime);
	Calculate_SuspensionDirectPower(DeltaTime);
	Calculate_SuspensionVelocity(DeltaTime);
	Calculate_SteeringAngle(DeltaTime);
}

bool UVehicleSuspensionComponent::IsSuspensionNormal()
{
	return SuspensionDirect == ESuspensionDirect::Normal;
}
