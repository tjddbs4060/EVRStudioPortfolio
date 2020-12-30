/**
* ULocoCharacterMovementComponent
* 정의 : Locomotion을 하는 CharacterMovementComponent 확장 클래스, 로코모션이 필요한 캐릭터에 기본 CharacterMovementComponent이다.
*		 
* 하는 일 :
*		1. 캐릭터 로테이션 처리 : Direction Mode(VelocityDirection, LookingDirection)에 따라 캐랙터 로테이션 방법이 달라진다.
*		   VelocityDirection : 입력 방향 또는 목표 지점으로 캐릭터가 로테이션을 함.
*          LookingDirection : 일정 각도(60도)이상 입력하지 않으면 캐릭터가 로테이션을 하지 않고 게걸음(strafe)으로 이동한다.
*		2. CharacterGait에 따른 MaxSpeed 조절
*		3. TurnInPlace 처리
*		4. 로테이션 처리
*		5. Strafe 이동 처리
*		
*/

class ULocoCharacterMovementComponent : public UMCharacterMovementComponent, public IDataTableLoadInterface
{

#pragma region TurnInPlace
public:
	bool TurnInPlace(float TargetYaw, float Speed = 1.f);

protected:
	UFUNCTION()
		void MontageEnd();
	
	bool bAITurn;

#pragma endregion

#pragma region Vehicle
public:
	FRotator GetVehicleDeltaRotation() const { return VehicleDeltaRotation; }
	void SetVehicleDeltaRotation(FRotator DeltaRotaion);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boat")
		FRotator VehicleDeltaRotation;
#pragma endregion

#pragma region Rolling
public:
	void SetRolling(bool State) { bRolling = State; }

	bool IsRolling() const { return bRolling; }
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rolling")
	bool bRolling;
#pragma endregion
};
