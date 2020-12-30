UCLASS()
class UVehicleSuspensionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Blueprint Editable
	/** 충돌 인식 최솟값 */
	UPROPERTY(EditAnywhere, Transient, Category = "Suspension Setting")
		float MinReactPower;
	/** 충돌 적용 최댓값 */
	UPROPERTY(EditAnywhere, Transient, Category = "Suspension Setting")
		float MaxReactPower;
	/** 급 커브시 적용되는 힘의 최솟값 */
	UPROPERTY(EditAnywhere, Transient, Category = "Suspension Setting")
		float CurveMinReactPower;
	/** Debug Line 길이 비율 */
	UPROPERTY(EditAnywhere, Transient, Category = "Debug")
		float DrawLineLength;

	/** 흔들리는 낙차 최솟값 */
	UPROPERTY(EditAnywhere, Transient, Category = "Suspension Setting")
		float SuspensionHeight;
	/** 힘이 적용되는 최소 시간 */
	UPROPERTY(EditAnywhere, Transient, Category = "Suspension Setting")
		float SuspensionDelay;
	// Blueprint Editable End

protected:
	/** Vehicle 변수의 World */
	UPROPERTY(Transient)
		UWorld* World;

	/** Suspension이 계산될 Vehicle (Component가 추가된 Actor) */
	UPROPERTY(Transient)
		class AMWheeledVehicle* Vehicle;
	/** Vehicle Main Mesh */
	UPROPERTY(Transient)
		class USkeletalMeshComponent* VehicleMesh;

	/** 이전 프레임의 Force & 현재 프레임의 Force */
	FVector CurForce;
	FVector BeforeForce;

	/** Actor기준의 Forward & Right */
	FVector VehicleForward;
	FVector VehicleRight;

	/** 이전 프레임의 힘의 방향 & 현재 프레임의 힘의 방향 */
	ESuspensionDirect BeforeSuspensionDirect;
	ESuspensionDirect SuspensionDirect;

	/** 차량의 현재 힘의 방향과 세기 */
	FVector SuspensionDirectPower;
	/** 힘의 변화량 */
	float SuspensionVelocity;
	/** 힘을 적용할 수 있는 상태 */
	bool bImpact;

	/** 현재 핸들의 각도 */
	float SteeringAngle;

	/** Tick 계산에 사용될 변수 */
	FVector OriginDirectPower;
	float OriginDelay;

public:	
	// Sets default values for this component's properties
	UVehicleSuspensionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

	/** Tick마다 Update되는 변수들 초기화 함수 */
	virtual void UpdateVariable(float DeltaTime);
	/** 현재 차량이 기울어진 방향 계산 함수 */
	virtual void Calculate_SuspensionDirect(float DeltaTime);
	/** 힘의 크기 계산 함수 - Vehicle Character Physics Component에 사용 */
	virtual void Calculate_SuspensionDirectPower(float DeltaTime);
	/** Suspension 힘의 변화량 계산 함수 - Vehicle Character Physics Component에 사용 */
	virtual void Calculate_SuspensionVelocity(float DeltaTime);
	/** 핸들 각도 계산 함수 - Cuver Animation에 사용 */
	virtual void Calculate_SteeringAngle(float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 현재 지형의 Normal 여부 판단 (캐릭터의 기울기 Animation) 
	@return bool - true is Normal
	*/
	UFUNCTION(BlueprintPure)
		bool IsSuspensionNormal();

	/** 핸들 각도 Get&Set 함수 */
	UFUNCTION(BlueprintPure)
		float GetSteeringAngle() const { return SteeringAngle; }
	UFUNCTION(BlueprintCallable)
		void SetSteeringAngle(float Angle) { SteeringAngle = Angle; }

	/** Suspension 상태 Get 함수 */
	UFUNCTION(BlueprintPure)
		ESuspensionDirect GetSuspensionDirect() const { return SuspensionDirect; }

	/** 차량의 현재 힘의 방향과 세기 반환 */
	UFUNCTION(BlueprintPure)
		FVector GetSuspensionDirectPower() const { return SuspensionDirectPower; }
	/** 힘의 변화량 반환 */
	UFUNCTION(BlueprintPure)
		float GetSuspensionVelocity() const { return SuspensionVelocity; }
	/** 힘의 변화 가능 상태 반환 */
	UFUNCTION(BlueprintPure)
		bool IsImpaceEnabled() const { return bImpact; }

};
