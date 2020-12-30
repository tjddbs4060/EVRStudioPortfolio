/**
* Vehicle Character Physics Component
* 정의 : Vehicle에 탑승한 캐릭터의 물리 계산
* 추가 컴포넌트 : Vehicle Drive Component (현재 탑승한 캐릭터)
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UVehicleCharacterPhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleCharacterPhysicsComponent();

	/** Physical Animation Main Bone Name */
	UPROPERTY(EditAnywhere)
		FName PhysicalBodyName;
	/** Vehicle Physics Simulate 활성화 되었을 때, Blend Weight 값 */
	UPROPERTY(EditAnywhere)
		float EnabledBlendWeight;

	/** Enable Blend 될, 최소 충격량 */
	UPROPERTY(EditAnywhere)
		float StrongBlendWeight;
	/** Not Flat Floor 상태에서 가해질 충격량 보정값 */
	UPROPERTY(EditAnywhere)
		float NotFlatImpulseWeight;

private:
	/** 현재 Vehicle 미리 정의 */
	UPROPERTY()
		class AMWheeledVehicle* Vehicle;
	/** 현재 Vehicle의 Vehicle Suspension Component 미리 정의 */
	UPROPERTY()
		class UVehicleSuspensionComponent* Suspension;
	/** 현재 탑승한 캐릭터 */
	UPROPERTY()
		AMCharacter* TakenCharacter;
	/** 현재 Vehicle Physics Simulate 상태 */
	bool bSimulate;

	/** Physical Simulate 설정 시간 */
	float PhysicsDelayTime;
	float PhysicsLimitDelayTime;

	/** 충격량에 따른 Physical Simulate 설정 시간 보정 값*/
	float CollisionTimeCorrection;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Vehicle Physics Simulate 상태 변경 */
	UFUNCTION(BlueprintCallable)
		void SetVehiclePhysicsSimulate(bool Simulate);
	/** 캐릭터 탑승 및 하차 (Vehicle Drive Component에서 호출) */
	UFUNCTION(BlueprintCallable)
		void Take(class AMCharacter* Character);

private:
	/** 충격에 대한 Blend Time Setting */
	void UpdateCollisionBlendWeight();
	/** 흔들림에 대한 Blend Time Setting */
	void UpdateNotFlatFloorBlendWeight();

	/** 설정된 Delay Time 관리 */
	void UpdateBlendWeight(float DeltaTime);
};
