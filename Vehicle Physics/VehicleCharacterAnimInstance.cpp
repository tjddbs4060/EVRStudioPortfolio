UVehicleCharacterAnimInstance::UVehicleCharacterAnimInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	RightHandleAnimSequenceLength = 0.0f;
	LeftHandleAnimSequenceLength = 0.0f;
	VehicleCurveAngle = 0.0f;
	Suspension = ESuspensionDirect::Normal;
	RightHandLocation = FVector::ZeroVector;
	LeftHandLocation = FVector::ZeroVector;
	SteeringAngle = 0.0f;
	SuspensionDelay = 3.0f;
	BlendSpaceTimeCorrection = 0.5f;
	LimitCurveAngle = 8.0f;
	LimitHandleAngle = 800.0f;
	bRide = false;
	bSteeringRotate = false;
	bRightHandle = false;
	HandleSequencePlayTime = 0.0f;
	CurveType = EVehicleCharacterPhysicsCurve::BlendSpace;
	SpineRotateCorrection = 5.0f;
	SpineSecondsRotateCorrection = -0.5f;
}

void UVehicleCharacterAnimInstance::NativeInitializeAnimation()
{
	MainCharacter = Cast<AMainCharacter>(GetOwningActor());
	if (MainCharacter)
	{
		CharacterMesh = MainCharacter->GetMesh();
		ensure(CharacterMesh);
	}

	ensure(RightHandleAnimSequence);
	ensure(LeftHandleAnimSequence);

	RightHandleAnimSequenceLength = UMAnimationBlueprintFunctionLibrary::GetAnimSequenceLength(RightHandleAnimSequence);
	LeftHandleAnimSequenceLength = UMAnimationBlueprintFunctionLibrary::GetAnimSequenceLength(LeftHandleAnimSequence);

	Super::NativeInitializeAnimation();
}

void UVehicleCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UVehicleCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	CalculateVehicle();

	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UVehicleCharacterAnimInstance::CalculateVehicle()
{
	if (MainCharacter == nullptr)
		return;

	AMWheeledVehicle* Vehicle = MainCharacter->GetVehicle();
	UVehicleSuspensionComponent* VSComponent = MainCharacter->GetVehicleSuspension();

	if (MainCharacter == nullptr || Vehicle == nullptr || CharacterMesh == nullptr || VSComponent == nullptr)
		return;

	VehicleCurveAngle = (CurveType == EVehicleCharacterPhysicsCurve::SpineRotate) ? Vehicle->GetActorRotation().Roll : FMath::Abs(Vehicle->GetActorRotation().Roll);
	SteeringAngle = VSComponent->GetSteeringAngle();
	Suspension = VSComponent->GetSuspensionDirect();

	FTransform CharacterRootTransform = CharacterMesh->GetSocketTransform(CharacterBoneNames::Root);

	RightHandLocation =
		FVector(GetCurveValue(VehicleAnimCurveKeyNames::RightHand_X),
			GetCurveValue(VehicleAnimCurveKeyNames::RightHand_Y),
			GetCurveValue(VehicleAnimCurveKeyNames::RightHand_Z));
	LeftHandLocation =
		FVector(GetCurveValue(VehicleAnimCurveKeyNames::LeftHand_X),
			GetCurveValue(VehicleAnimCurveKeyNames::LeftHand_Y),
			GetCurveValue(VehicleAnimCurveKeyNames::LeftHand_Z));

	if (CurveType == EVehicleCharacterPhysicsCurve::SpineRotate)
	{
		RightHandLocation = UKismetMathLibrary::GreaterGreater_VectorRotator(RightHandLocation, CharacterRootTransform.Rotator());
		LeftHandLocation = UKismetMathLibrary::GreaterGreater_VectorRotator(LeftHandLocation, CharacterRootTransform.Rotator());

		RightHandLocation += CharacterRootTransform.GetLocation();
		LeftHandLocation += CharacterRootTransform.GetLocation();
	}

	// Calculate Vehicle Handle Rotate Angle & Sequence Time
	bSteeringRotate = FMath::IsNearlyEqual(SteeringAngle, 0.0f);

	float HandleSequenceLength = (bSteeringRotate ? RightHandleAnimSequenceLength : LeftHandleAnimSequenceLength);
	float HandleSequenceRate = HandleSequenceLength / LimitHandleAngle;

	HandleSequencePlayTime = HandleSequenceRate * SteeringAngle;

	if (CurveType == EVehicleCharacterPhysicsCurve::BlendSpace)
		HandleSequencePlayTime *= BlendSpaceTimeCorrection;

	bRightHandle = SteeringAngle > 0;
}