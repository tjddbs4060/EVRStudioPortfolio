// Sets default values for this component's properties
UVehicleDriveComponent::UVehicleDriveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;

	PhysicalBodyName = CharacterBoneNames::Spine_03;
	PhysicalProfileName = AnimationProfileNames::VehicleBody;
	RightHandBoneName = CharacterBoneNames::Hand_R;
	LeftHandBoneName = CharacterBoneNames::Hand_L;
	DefaultBlendWeight = 0.0f;
	// ...
}


// Called when the game starts
void UVehicleDriveComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	MainCharacter = Cast<AMainCharacter>(GetOwner());
	ensure(MainCharacter);

	PhysicalAnimation = Cast<UPhysicalAnimationComponent>(MainCharacter->GetComponentByClass(UPhysicalAnimationComponent::StaticClass()));
	ensure(PhysicalAnimation);
}


// Called every frame
void UVehicleDriveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVehicleDriveComponent::TakeVehicle(class AMWheeledVehicle* Vehicle)
{
	ensure(Vehicle);
	MainCharacter->SetVehicle(Vehicle);
	
	UVehicleCharacterPhysicsComponent* VehicleCharacterPhysics = Cast<UVehicleCharacterPhysicsComponent>(MainCharacter->GetVehicle()->GetComponentByClass(UVehicleCharacterPhysicsComponent::StaticClass()));
	VehicleCharacterPhysics->Take(MainCharacter);

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepWorldTransform;
	MainCharacter->AttachToComponent(MainCharacter->GetVehicle()->GetMesh(), AttachmentRules);

	USkeletalMeshComponent* CharacterMesh = MainCharacter->GetMesh();
	ensure(CharacterMesh);

	// Character Physical Animation Default Setting
	PhysicalAnimation->SetSkeletalMeshComponent(CharacterMesh);
	PhysicalAnimation->ApplyPhysicalAnimationProfileBelow(PhysicalBodyName, PhysicalProfileName);
	CharacterMesh->SetAllBodiesBelowSimulatePhysics(PhysicalBodyName, true);
	CharacterMesh->SetAllBodiesBelowPhysicsBlendWeight(PhysicalBodyName, DefaultBlendWeight);

	// Character Mesh Hand 고정
	CharacterMesh->SetAllBodiesBelowSimulatePhysics(RightHandBoneName, false);
	CharacterMesh->SetAllBodiesBelowSimulatePhysics(LeftHandBoneName, false);
}

void UVehicleDriveComponent::QuitVehicle()
{
	ensure(MainCharacter->GetVehicle());
	if (MainCharacter->GetVehicle() == nullptr)
		return;

	UVehicleCharacterPhysicsComponent* VehicleCharacterPhysics = Cast<UVehicleCharacterPhysicsComponent>(MainCharacter->GetVehicle()->GetComponentByClass(UVehicleCharacterPhysicsComponent::StaticClass()));
	VehicleCharacterPhysics->Take(nullptr);
	MainCharacter->SetVehicle(nullptr);
}
