UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNightVisionComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNightVisionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	void ToggleNightVision();

	UFUNCTION(BlueprintPure)
	bool IsNightVision() const { return bIsNightVision; }

private:
	UFUNCTION()
	void NightVisionTransition(float InValue);

	void InitializeNightVision();

protected:
	UPROPERTY(EditAnywhere)
	class UMaterial* NightVisionMaterial;
	UPROPERTY(EditAnywhere)
	class UCurveFloat* NightVisionCurve;

private:
	/** Night Vision Post Process Volume */
	class APostProcessVolume* NightVisionPostProcess;
	/** Night Vision Dynamic Material */
	class UMaterialInstanceDynamic* NightVisionMaterialInstance;
	/** Night Vision Timeline */
	TSharedPtr<struct FTimeline> NightVisionTimeline;

	bool bIsNightVision;
};
