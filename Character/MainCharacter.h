/**
 * MainCharacter 
 * 정의 : Head(face), Hair, Body(Costume)의 세 부위로 구분되는 Character 클래스.
 * 하는 일 : 
 *		1. 각 부위를 상황별로 교체할 수 있는 중심적인 역할의 Character들에 사용한다.
 *		2. 페이셜(모프타겟) 에니메이션을 할 수 있다.
 *		3. 캐릭터 인풋처리(InputAxis, InputAction)
 * 추가 컴퍼넌트 :
 *		1. FootstepComponent : 발자국 처리  (Optional: If need, manual attach in Blueprint)
 *		2. PhysicalControllerComponent : 물리 처리 (Optional: DoNotCreateDefaultSubobject)
 *		3. Voice AudioComponent : 캐릭터 사운드 (Optional: DoNotCreateDefaultSubobject)
 *		4. FootstepDefault AudioComponent : 기본 발자국 소리 (Optional: DoNotCreateDefaultSubobject)
 *		5. FootstepRattle AudioComponent : 걸을때 장신구나 기타 흔들거리는 소리  (Optional: DoNotCreateDefaultSubobject)
 *		7. ShooterComponent : 총 관련 처리.  (Optional: DoNotCreateDefaultSubobject)
 *		8. AICharacterComponent : 전투 또는 주요한 AI를 할 수 있는 캐릭터 (Optional: DoNotCreateDefaultSubobject)
 *		9. InventoryComponent : 아이템 관리를 위한 컴포넌트 (Optional: DoNotCreateDefaultSubobject)
 *		10. ASM_AISplineMovement : 곡선이동을 위한 컴포넌트 (Optional: DoNotCreateDefaultSubobject)
 *		11. ShortDistanceCapsuleComponent : 길찾기시 다른 캐릭터와의 가까운거리의 오버랩체크를 하기위한 컴포넌트 (Optional: DoNotCreateDefaultSubobject)
 *		12. MiddleDistanceCapsuleComponent : 길찾기시 다른 캐릭터와의 약간 떨어진 거리의 오버랩체크를 하기위한 컴포넌트 (Optional: DoNotCreateDefaultSubobject)
 *		13. ProjectileHearAudioListener : 발사체 관련 컴포넌트들의 부모 컴포넌트 (Optional: DoNotCreateDefaultSubobject)
 *		14. LeftHearCheckSphere, RightHearCheckSphere : 발사체(총알)를 감지. (Optional: ProjectileHearAudioListener DoNotCreateDefaultSubobject)
 *		15. LeftHearAudio, RightHearAudio : 발사체에 대한 소리를 재생. (총알 스치는 소리) (Optional: ProjectileHearAudioListener DoNotCreateDefaultSubobject)

 */
class AMainCharacter : public AMCharacter, public ILocomotionCharacterInterface, public ICameraControlInterface, public FEventHandler
{
public:
	/** 머리에 해당하는 부분 변경 : Mesh */
	void ChangeFaceMesh(class USkeletalMesh* NewMesh);
	/** 헤어에 해당하는 부분 변경 : HairMeshComponent */
	void ChangeHairMesh(class USkeletalMesh* NewMesh);
	/** 복식에 해당하는 부분 변경 : BodyMeshComponent */
	void ChangeBodyMesh(class USkeletalMesh* NewMesh);

protected:
	/** 메시가 변경되었을때 PhysicsAsset 정보를 갱신하는 함수. */
	void UpdateCharacterPhysicsAsset();

#pragma region Vehicle
public:
	/** Get & Set Vehicle */
	void SetVehicle(class AMWheeledVehicle* InVehicle);

	class AMWheeledVehicle* GetVehicle() const { return Vehicle; }
	class UVehicleSuspensionComponent* GetVehicleSuspension() const { return VSComponent; }

private:
	/** 현재 탑승중인 Vehicle & Vehicle Suspension Component */
	class AMWheeledVehicle* Vehicle;
	class UVehicleSuspensionComponent* VSComponent;
#pragma endregion

#pragma region Camera
public:
	FTransform GetFirstPersonViewTransform() const;
	FTransform GetThirdPersonViewTransform() const;
    float GetFirstPersonViewFOV() const;
#pragma endregion
};