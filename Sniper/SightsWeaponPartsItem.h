class ASightsWeaponPartsItem : public AWeaponPartsItem
{
public:
	ASightsWeaponPartsItem(const FObjectInitializer& ObjectInitializer);

	void PostInitializeComponents() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	//사이트 어빌리티 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESightsPartsAbility SightsAbility;

	//무기의 Camera Location을 Override 할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOverrideCamera;

	//무기의 Camera Offset을 Override 할 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* SightsCameraOffsetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* LensReflactorMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* NearOuterLensMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* NearInnerLensMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* ReverseFarOuterLensMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* ReverseFarInnerLensMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* FarOuterLensMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* FarInnerLensMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* MaskLensMeshComp;

	EAimingType BeforeAimType;
#pragma endregion

private:
	FVector OriginReverseFarLensLocation;

	void FarLensUpdate(float DeltaTime);
	void ReflactorLensUpdate(float DeltaTime);

#pragma region ScopeRenderTarget
private:
	void UsePostProcessEffect();
	void UnUsePostProcessEffect();
	void InitializeRenderTarget();

	void UseCustomStencil();
	void UnuseCustomStencil();
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* ScopeSceneCapture2D;
	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* PostProcessComponent;
	
	class UTextureRenderTarget2D* ScopeRenderTarget;
	
	class UMaterialInstanceDynamic* MaterialInstance;
	class UMaterialInstanceDynamic* ShadowMaterialInstance;
	class UMaterialInstanceDynamic* ReflactorMaterialInstance;
public:
	UPROPERTY(EditAnywhere)
	class UMaterial* LensMaterial;
	UPROPERTY(EditAnywhere)
	class UMaterial* LensShadowMaterial;
	UPROPERTY(EditAnywhere)
	class UMaterial* ReflactorMaterial;
	// Lens 외각선 굴절 값 (Default = 0.5f)
	UPROPERTY(EditAnywhere)
	float Refraction;
	// Scene Capture Render Target을 사용할 지에 대한 변수
	UPROPERTY(EditAnywhere)
	bool bUseRenderTarget;
private:
	float BeforeScalar;
	FVector BeforeFarLensLocation;
#pragma endregion

#ifdef WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif // WITH_EDITOR
};