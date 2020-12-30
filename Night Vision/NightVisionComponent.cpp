// Sets default values for this component's properties
UNightVisionComponent::UNightVisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIsNightVision = false;
}


// Called when the game starts
void UNightVisionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeNightVision();
}


// Called every frame
void UNightVisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (NightVisionTimeline.IsValid() && NightVisionTimeline->IsPlaying())
		NightVisionTimeline->TickTimeline(DeltaTime);
}

void UNightVisionComponent::ToggleNightVision()
{
	bIsNightVision = !bIsNightVision;

	if (bIsNightVision)
	{
		NightVisionTimeline->Play();
	}
	else
	{
		NightVisionTimeline->Reverse();
	}
}

void UNightVisionComponent::NightVisionTransition(float InValue)
{
	if (NightVisionMaterialInstance)
	{
		NightVisionMaterialInstance->SetScalarParameterValue(TEXT("NightVision Alpha"), InValue);
	}
}

void UNightVisionComponent::InitializeNightVision()
{
	AGamePlayerCameraActor* GameCameraActor = UMBlueprintFunctionLibrary::GetMPlayerCamera(GetWorld());
	
	/** Create Material Dynamic */
	if (GameCameraActor && GameCameraActor->GetCineCameraComponent())
	{
		NightVisionMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), NightVisionMaterial);

		if (NightVisionMaterialInstance)
		{
			GameCameraActor->GetCineCameraComponent()->AddOrUpdateBlendable(NightVisionMaterialInstance);
		}
	}

	/** Create Timeline */
	if (NightVisionTimeline.IsValid() == false && NightVisionCurve != nullptr)
	{
		NightVisionTimeline = MakeShareable<FTimeline>(new FTimeline());

		float min = 0.0f, max = 1.0f;
		NightVisionCurve->GetTimeRange(min, max);

		NightVisionTimeline->SetLooping(false);
		NightVisionTimeline->SetTimelineLength(max);
		NightVisionTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);

		FOnTimelineFloat onTimelineFloatCallback;
		onTimelineFloatCallback.BindUFunction(this, TEXT("NightVisionTransition"));
		NightVisionTimeline->AddInterpFloat(NightVisionCurve, onTimelineFloatCallback, NAME_None, TEXT("NightVisionCurve"));
	}
}
