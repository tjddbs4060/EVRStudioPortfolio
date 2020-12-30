/** 
* Vehicle Drive Component
* 정의 : Vehicle을 탑승하는 캐릭터에 추가할 Component
* 하는 일 : Vehicle 탑승 및 하차, Physical 기본 셋팅
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UVehicleDriveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleDriveComponent();

	/** Physical Animation Main Bone Name */
	UPROPERTY(EditAnywhere)
		FName PhysicalBodyName;
	/** Physical Animation Profile Name */
	UPROPERTY(EditAnywhere)
		FName PhysicalProfileName;
	/** Physics Asset의 Right & Left Hand Bone Name */
	UPROPERTY(EditAnywhere)
		FName RightHandBoneName;
	UPROPERTY(EditAnywhere)
		FName LeftHandBoneName;

private:
	/** Physical Animation Default Blend Weight */
	float DefaultBlendWeight;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 차량 탑승 */
	UFUNCTION(BlueprintCallable)
		void TakeVehicle(class AMWheeledVehicle* Vehicle);
	/** 차량 하차 */
	UFUNCTION(BlueprintCallable)
		void QuitVehicle();

private:
	/** MCharacter 미리 정의 */
	UPROPERTY()
		class AMainCharacter* MainCharacter;
	/** Physical Animation Component 미리 정의 - Component가 존재하지 않으면 Vehicle Physics 불가능 */
	UPROPERTY()
		class UPhysicalAnimationComponent* PhysicalAnimation;

};
