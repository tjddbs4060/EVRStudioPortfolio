ASightsWeaponPartsItem::ASightsWeaponPartsItem(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PartsSlot = EWeaponPartsSlot::SIGHTS;
	SightsAbility = ESightsPartsAbility::NONE;
	bOverrideCamera = false;

	FVector NearLensLocation = FVector(0.0f, -14.3f, -8.0f);
	FVector ReflactorLensLocation = FVector(0.0f, -14.2f, -8.0f);
	
	SightsCameraOffsetComp = CreateDefaultSubobject<USceneComponent>(TEXT("SightsCameraOffsetComp"));
	SightsCameraOffsetComp->SetupAttachment(ItemMesh);

	// Lens Reflactor Mesh Settings
	LensReflactorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LensReflactorMeshComp"));
	LensReflactorMeshComp->SetupAttachment(ItemMesh);
	LensReflactorMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LensReflactorMeshComp->SetCollisionProfileName("NoCollision");
	LensReflactorMeshComp->SetRelativeLocation(ReflactorLensLocation);

	// Near Lens Settings
	NearOuterLensMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NearOuterLensMeshComp"));
	NearOuterLensMeshComp->SetupAttachment(ItemMesh);
	NearOuterLensMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NearOuterLensMeshComp->SetCollisionProfileName("NoCollision");
	NearOuterLensMeshComp->SetRelativeLocation(NearLensLocation);

	NearInnerLensMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NearInnerLensMeshComp"));
	NearInnerLensMeshComp->SetupAttachment(NearOuterLensMeshComp);
	NearInnerLensMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NearInnerLensMeshComp->SetCollisionProfileName("NoCollision");
	NearInnerLensMeshComp->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_1);
	NearInnerLensMeshComp->SetCustomDepthStencilValue(1);

	MaskLensMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MaskLensMeshComp"));
	MaskLensMeshComp->SetupAttachment(ItemMesh);
	MaskLensMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MaskLensMeshComp->SetCollisionProfileName("NoCollision");
	MaskLensMeshComp->SetRelativeLocation(NearLensLocation);
	MaskLensMeshComp->bVisibleInReflectionCaptures = false;
	MaskLensMeshComp->bVisibleInRayTracing = false;
	MaskLensMeshComp->bReceivesDecals = false;
	MaskLensMeshComp->CastShadow = false;

	// Far Lens Settings
	FTransform FarLensTransform = FTransform(FRotator::ZeroRotator, FVector(-0.3f, 29.0f, -26.5f), FVector(2.5f));

	FarOuterLensMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FarOuterLensMeshComp"));
	FarOuterLensMeshComp->SetupAttachment(ItemMesh);
	FarOuterLensMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FarOuterLensMeshComp->SetCollisionProfileName("NoCollision");
	FarOuterLensMeshComp->SetRelativeTransform(FarLensTransform);
	

	FarInnerLensMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FarInnerLensMeshComp"));
	FarInnerLensMeshComp->SetupAttachment(FarOuterLensMeshComp);
	FarInnerLensMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FarInnerLensMeshComp->SetCollisionProfileName("NoCollision");
	FarInnerLensMeshComp->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_2);
	FarInnerLensMeshComp->SetCustomDepthStencilValue(2);

	// Reverse Far Lens Settings
	ReverseFarOuterLensMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReverseFarOuterLensMeshComp"));
	ReverseFarOuterLensMeshComp->SetupAttachment(ItemMesh);
	ReverseFarOuterLensMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ReverseFarOuterLensMeshComp->SetCollisionProfileName("NoCollision");
	ReverseFarOuterLensMeshComp->SetRelativeLocation(NearLensLocation);

	ReverseFarInnerLensMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReverseFarInnerLensMeshComp"));
	ReverseFarInnerLensMeshComp->SetupAttachment(ReverseFarOuterLensMeshComp);
	ReverseFarInnerLensMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ReverseFarInnerLensMeshComp->SetCollisionProfileName("NoCollision");
	ReverseFarInnerLensMeshComp->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_4);
	ReverseFarInnerLensMeshComp->SetCustomDepthStencilValue(4);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(ItemMesh);

	// Scope Scene Capture Settings
	ScopeSceneCapture2D = CreateOptionalDefaultSubobject<USceneCaptureComponent2D>(TEXT("LensSceneCapture"));
	ScopeSceneCapture2D->SetupAttachment(ItemMesh);
	ScopeSceneCapture2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	ScopeSceneCapture2D->bCaptureEveryFrame = false;
	ScopeSceneCapture2D->bCaptureOnMovement = false;

	// 프레임 향상을 위한 Capture Component Flag 설정
	ScopeSceneCapture2D->ShowFlags.DisableAdvancedFeatures();
	ScopeSceneCapture2D->ShowFlags.SetBSP(false);
	ScopeSceneCapture2D->ShowFlags.SetDecals(false);
	ScopeSceneCapture2D->ShowFlags.SetLandscape(false);
	ScopeSceneCapture2D->ShowFlags.SetSkeletalMeshes(false);
	ScopeSceneCapture2D->ShowFlags.SetAtmosphere(false);
	ScopeSceneCapture2D->ShowFlags.SetParticles(false);
	ScopeSceneCapture2D->ShowFlags.SetFog(false);

	ScopeSceneCapture2D->ShowFlags.SetInstancedGrass(false);
	ScopeSceneCapture2D->ShowFlags.SetDeferredLighting(false);
	ScopeSceneCapture2D->ShowFlags.SetTextRender(false);
	ScopeSceneCapture2D->ShowFlags.SetInstancedStaticMeshes(false);
	ScopeSceneCapture2D->ShowFlags.SetTemporalAA(false);
	ScopeSceneCapture2D->ShowFlags.SetPaper2DSprites(false);
	ScopeSceneCapture2D->ShowFlags.SetInstancedFoliage(false);

	ScopeSceneCapture2D->ShowFlags.SetBloom(false);

	ScopeSceneCapture2D->ShowFlags.SetSkyLighting(false);

	ScopeSceneCapture2D->ShowFlags.SetDynamicShadows(false);

	ScopeSceneCapture2D->ShowFlags.SetLightFunctions(false);
	ScopeSceneCapture2D->ShowFlags.SetReflectionEnvironment(false);
	ScopeSceneCapture2D->ShowFlags.SetAmbientCubemap(false);
	ScopeSceneCapture2D->ShowFlags.SetTexturedLightProfiles(false);

	ScopeSceneCapture2D->ShowFlags.SetGame(false);
	ScopeSceneCapture2D->ShowFlags.SetLighting(false);
	ScopeSceneCapture2D->ShowFlags.SetPostProcessing(false);

	Refraction = 0.5f;
	bUseRenderTarget = false;

	BeforeScalar = 0.0f;

	BeforeFarLensLocation = FVector::ZeroVector;
}

void ASightsWeaponPartsItem::FarLensUpdate(float DeltaTime)
{
	if (OwnerCharacter->IsAiming() == false)
		return;

	if (ShadowMaterialInstance)
	{
		float Scalar = 0.0f;

		if (OwnerCharacter && OwnerCharacter->GetLocoAnimInstance())
			Scalar = OwnerCharacter->GetLocoAnimInstance()->GetIKRotationValue() * 2.0f;

		if (FMath::IsNearlyEqual(Scalar, BeforeScalar) == false)
			ShadowMaterialInstance->SetScalarParameterValue(TEXT("UseSunlight"), FMath::Clamp(Scalar, 0.0f, 1.0f));
	}

	if (OwnerCharacter->IsFullyAiming())
	{
		FVector AimingVector = OwnerCharacter->GetLocoCharacterMovement()->GetAimingPoint() - OwnerCharacter->GetFirstPersonViewTransform().GetLocation();
		FVector WeaponForwardVector = GetActorForwardVector();
		FVector WeaponUpVector = GetActorUpVector();

		AimingVector.Normalize();
		WeaponForwardVector.Normalize();
		WeaponUpVector.Normalize();

		float UpOffset = FVector::DotProduct(AimingVector, WeaponUpVector) / (AimingVector.Size(), WeaponUpVector.Size());
		float RightOffset = FVector::DotProduct(AimingVector, WeaponForwardVector) / (AimingVector.Size(), WeaponUpVector.Size());

		FVector UpLens = WeaponUpVector * UpOffset * 6.0f;
		FVector RightLens = WeaponForwardVector * RightOffset * 6.0f;
		ReverseFarOuterLensMeshComp->SetRelativeLocation(OriginReverseFarLensLocation + UpLens + RightLens);

		if (MaterialInstance)
		{
			MaterialInstance->SetScalarParameterValue(TEXT("DelayOffsetU"), -OwnerCharacter->GetLocoAnimInstance()->IKYawOffset * 0.3f);
			MaterialInstance->SetScalarParameterValue(TEXT("DelayOffsetV"), -OwnerCharacter->GetLocoAnimInstance()->IKPitchOffset * 0.3f);
		}
	}
}

void ASightsWeaponPartsItem::ReflactorLensUpdate(float DeltaTime)
{
	if (OwnerCharacter->IsAiming())
	{
		if (LensReflactorMeshComp->IsVisible())
			LensReflactorMeshComp->SetVisibility(false);
	}
	else
	{
		if (LensReflactorMeshComp->IsVisible() == false)
			LensReflactorMeshComp->SetVisibility(true);
	}

	if (ReflactorMaterialInstance)
	{
		if (OwnerCharacter->IsAiming())
		{
			ReflactorMaterialInstance->SetScalarParameterValue(TEXT("Stencil"), 7.0f);
		}
		else
		{
			ReflactorMaterialInstance->SetScalarParameterValue(TEXT("Stencil"), 0.0f);
		}
	}
}

void ASightsWeaponPartsItem::UsePostProcessEffect()
{
	if (bUseRenderTarget == false)
		return;

	ScopeSceneCapture2D->bCaptureEveryFrame = true;
	ScopeSceneCapture2D->bCaptureOnMovement = true;

	MaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), LensMaterial);
	if (MaterialInstance)
	{
		if (ScopeRenderTarget)
			MaterialInstance->SetTextureParameterValue(TEXT("RenderTarget"), ScopeRenderTarget);
		if (Refraction)
			MaterialInstance->SetScalarParameterValue(TEXT("Refraction"), Refraction);

		FWeightedBlendable PostMaterial;
		PostMaterial.Object = MaterialInstance;
		PostMaterial.Weight = 1.0f;

		PostProcessComponent->Settings.WeightedBlendables.Array.Add(PostMaterial);
	}

	ShadowMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), LensShadowMaterial);
	if (ShadowMaterialInstance)
	{
		if (ScopeRenderTarget)
			ShadowMaterialInstance->SetTextureParameterValue(TEXT("RenderTarget"), ScopeRenderTarget);

		FWeightedBlendable PostMaterial;
		PostMaterial.Object = ShadowMaterialInstance;
		PostMaterial.Weight = 1.0f;

		PostProcessComponent->Settings.WeightedBlendables.Array.Add(PostMaterial);
	}

	AMGameModeBase* GameMode = Cast<AMGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (GameMode && GameMode->GetOpaqueMaskCapture())
		GameMode->GetOpaqueMaskCapture()->AddMaskComponent(MaskLensMeshComp);
}

void ASightsWeaponPartsItem::UnUsePostProcessEffect()
{
	ScopeSceneCapture2D->bCaptureEveryFrame = false;
	ScopeSceneCapture2D->bCaptureOnMovement = false;
	PostProcessComponent->Settings.WeightedBlendables.Array.Empty();

	AMGameModeBase* GameMode = Cast<AMGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (GameMode && GameMode->GetOpaqueMaskCapture())
		GameMode->GetOpaqueMaskCapture()->RemoveMaskComponent(MaskLensMeshComp);
}

void ASightsWeaponPartsItem::UseCustomStencil() 
{
	NearInnerLensMeshComp->SetRenderCustomDepth(true);
	FarInnerLensMeshComp->SetRenderCustomDepth(true);
	ReverseFarInnerLensMeshComp->SetRenderCustomDepth(true);
}

void ASightsWeaponPartsItem::UnuseCustomStencil()
{
	NearInnerLensMeshComp->SetRenderCustomDepth(false);
	FarInnerLensMeshComp->SetRenderCustomDepth(false);
	ReverseFarInnerLensMeshComp->SetRenderCustomDepth(false);
}

void ASightsWeaponPartsItem::InitializeRenderTarget()
{
	if (bUseRenderTarget == false || (ScopeRenderTarget && ScopeRenderTarget))
		return;

	FVector2D ScreenSize;
	GEngine->GameViewport->GetViewportSize(ScreenSize);

	ScopeRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), ScreenSize.X, ScreenSize.Y);

	ScopeSceneCapture2D->ShowOnlyActors.Add(this);
	ScopeSceneCapture2D->TextureTarget = ScopeRenderTarget;
	ScopeSceneCapture2D->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;

	TArray<AActor*> OutlineActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOutlineCapture::StaticClass(), OutlineActors);
}

#ifdef WITH_EDITOR
void ASightsWeaponPartsItem::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName().IsEqual(TEXT("Refraction")) && MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue(TEXT("Refraction"), Refraction);
	}
}
#endif // WITH_EDITOR

void ASightsWeaponPartsItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASightsWeaponPartsItem::BeginPlay()
{
	Super::BeginPlay();

	OriginReverseFarLensLocation = ReverseFarOuterLensMeshComp->GetRelativeLocation();

	ItemMesh->SetVisibility(true);

	ReflactorMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), ReflactorMaterial);
	if (ReflactorMaterialInstance)
	{
		LensReflactorMeshComp->SetMaterial(0, ReflactorMaterialInstance);
	}

	InitializeRenderTarget();

	AMPlayerController* PlayerController = Cast<AMPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (PlayerController)
		PlayerController->HiddenPrimitiveComponents.Add(MaskLensMeshComp);

	if (ScopeSceneCapture2D)
		ScopeSceneCapture2D->HiddenComponents.Add(MaskLensMeshComp);

	AMGameModeBase* GameMode = Cast<AMGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (GameMode && GameMode->GetOutlineCapture())
		GameMode->GetOutlineCapture()->AddHiddenOutlineComponent(MaskLensMeshComp);
}

void ASightsWeaponPartsItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerWeapon || !OwnerCharacter)
	{
		SetActorTickEnabled(false);
		return;
	}
	
	ReflactorLensUpdate(DeltaTime);
	FarLensUpdate(DeltaTime);
	if (OwnerCharacter->IsLocalPlayer())
	{
		if (OwnerWeapon->IsUseFirstPersonView())
		{
			ChangeFirstPersonViewMeshStates(OwnerCharacter->IsAimingDownSightMode());

			//카메라 떄문에 숨겨질 경우 복구
			if (OwnerCharacter->IsHidden() && OwnerCharacter->IsAimingDownSightMode())
			{
				SetActorHiddenInGame(false);
				ChangeFirstPersonViewMeshStates(true);
			}

			if (OwnerWeapon->GetWeaponCollideAlpha() != 0.0)
				ChangeFirstPersonViewMeshStates(false);
		}

		// Scene Capture 2D Camera 위치 조정
		if (bUseRenderTarget && OwnerCharacter->GetLocoCharacterMovement())
		{
			ScopeSceneCapture2D->FOVAngle = UMBlueprintFunctionLibrary::GetMPlayerCamera(GetWorld())->GetIngameFOV(); // OwnerCharacter->GetFirstPersonViewFOV();

			FVector CaptureLocation = FVector::ZeroVector;
			FRotator CaptureRotation = FRotator::ZeroRotator;
			OwnerCharacter->GetController()->GetPlayerViewPoint(CaptureLocation, CaptureRotation);

			ScopeSceneCapture2D->SetWorldLocation(CaptureLocation);
			ScopeSceneCapture2D->SetWorldRotation(CaptureRotation);

			if (ScopeRenderTarget)
			{
				FVector2D ScreenSize;
				GEngine->GameViewport->GetViewportSize(ScreenSize);

				ScopeRenderTarget->ResizeTarget(ScreenSize.X, ScreenSize.Y);
			}
		}
	}
}

void ASightsWeaponPartsItem::ChangeFirstPersonViewMeshStates(bool bVisible)
{
	//TRUE = 1인칭 메시 모드 | FALSE = 3인칭 메시 모드
	if (ItemMesh->IsVisible() != bVisible)
		return;

	NearOuterLensMeshComp->SetVisibility(bVisible, true);
	NearInnerLensMeshComp->SetVisibility(bVisible, true);
	FarOuterLensMeshComp->SetVisibility(bVisible, true);
	FarInnerLensMeshComp->SetVisibility(bVisible, true);
}
