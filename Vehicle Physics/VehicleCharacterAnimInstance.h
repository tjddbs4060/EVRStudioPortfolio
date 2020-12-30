UCLASS()
class UVehicleCharacterAnimInstance : public UMAnimInstance
{
	GENERATED_BODY()
	
public:
	UVehicleCharacterAnimInstance(const FObjectInitializer& ObjectInitializer);
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;

private:
	/** 자주 사용되는 클래스 변수 미리 정의 */
	UPROPERTY(Transient)
		class AMainCharacter* MainCharacter;
	UPROPERTY(Transient)
		class USkeletalMeshComponent* CharacterMesh;

protected:
	/** 차량 탑승 변수 업데이트 */
	void CalculateVehicle();

public:
	//~ Begin 편집할 수 있는 변수들
	/** Vehicle Curve 구현 방식 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion: Vehicle")
		EVehicleCharacterPhysicsCurve CurveType;
	/** Handle Rotate로 사용되는 Animation Sequence (최대 총 길이를 구하기 위함) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion: Vehicle")
		class UAnimSequence* RightHandleAnimSequence;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion: Vehicle")
		class UAnimSequence* LeftHandleAnimSequence;
	/** 차량의 Suspension 유지 상태 체크 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float SuspensionDelay;

	//~ Begin 편집할 수 없는 변수들. 계산되는 값들은 굳이 AnimBlueprint에 노출할 필요가 없다. 그런 값들은 Edit옵션을 넣지 않는다.
	/** Vehicle Character Animation 관련 변수들 */
	/** Vehicle 탑승 상태 여부 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		bool bRide;
	/** Handle Animation Sequence Length */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float RightHandleAnimSequenceLength;
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float LeftHandleAnimSequenceLength;
	/** Handle Anim Sequence Play Position */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float HandleSequencePlayTime;
	/** 현재 차량의 좌우 기울어진 각도 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float VehicleCurveAngle;
	/** Vehicle 최대 기울임 각도 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float LimitCurveAngle;
	/** 차량의 Suspension 상태 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		ESuspensionDirect Suspension;
	/** 커브 상태 여부 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		bool bSteeringRotate;
	/** 핸들의 방향 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		bool bRightHandle;
	/** 현재 핸들의 각도 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float SteeringAngle;
	/** 핸들의 최대 각도 (-n ~ n) */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float LimitHandleAngle;
	/** Character Hand Location */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		FVector RightHandLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		FVector LeftHandLocation;
	/** Blend Space 설정 시 Sequence Length 비율 변환 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float BlendSpaceTimeCorrection;
	/** Vehicle 기울임 각도에 따른 Spine 기울임 보정 값 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float SpineRotateCorrection;
	/** Spine 기울임 각도에 따른 Spine_02 기울임 보정 값 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion: Vehicle")
		float SpineSecondsRotateCorrection;

};
