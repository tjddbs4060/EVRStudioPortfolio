// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include "HandrailActor.generated.h"

UENUM(BlueprintType)
enum class EMENDistributionMethod : uint8
{
	SINGLE_LEGRACY UMETA(DisplayName = "Single(Legacy)"),
	SINGLE UMETA(DisplayName = "Single"),
	RANDOM UMETA(DisplayName = "Random"),
	SEQUENTIAL UMETA(DisplayName = "Sequential"),
	FIRST_AND_LAST UMETA(DisplayName = "FirstAndLast"),
};

/** Handrail 생성 Mesh 구조체 정의 */
USTRUCT(BlueprintType)
struct FSTMeshExtraPrimitiveComp
{
	GENERATED_USTRUCT_BODY()

public:
	/** Created Instance Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject* mesh;
	/** Created Distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float spacing;
	/** Created Offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector offset;
	/** Created Angular (Degree) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float angularOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool zVertical;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxDrawDistance;
	/** Default Scale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector scale;

	FSTMeshExtraPrimitiveComp()
	{
		Initialize();
	}

private:
	void Initialize()
	{
		mesh = nullptr;
		spacing = 200.0f;
		offset = FVector::ZeroVector;
		angularOffset = 0.0f;
		zVertical = false;
		maxDrawDistance = 50000.0f;
		scale = FVector::OneVector;
	}
};

// DESC :> 전방 선언
class UInstancedStaticMeshComponent;
class UStaticMesh;
class USkeletalMesh;
class USplineComponent;
class UTextRenderComponent;
class USplineMeshComponent;
class UMDestructibleComponent;

using namespace std;

UCLASS()
class AHandrailActor : public AActor
{
	GENERATED_BODY()

private:
	/** Default Component Declara */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "Scene", Meta = (AllowPrivateAccess = "true"))
		USceneComponent* rootSceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "Spline", Meta = (AllowPrivateAccess = "true"))
		USplineComponent* spline;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "Start", Meta = (AllowPrivateAccess = "true"))
		UTextRenderComponent* start;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "End", Meta = (AllowPrivateAccess = "true"))
		UTextRenderComponent* end;

private:
	/** Non Blueprint Variable */

	UStaticMeshComponent* startMeshComponent;
	UStaticMeshComponent* endMeshComponent;

	// =======================================
	/** Children Mesh Setting */
	FName firstPointParentSocketName;
	FName lastPointParentSocketName;

	bool bAttachFirstPointToStartMesh;
	bool bAttachFirstPointToEndMesh;
	bool bAttachLastPointToStartMesh;
	bool bAttachLastPointToEndMesh;
	// =======================================

private:
	/** Blueprint Variable */

	// =======================================
	/** Spline */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		UStaticMesh* splineStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		TArray<UStaticMesh*> splineMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		float xScale;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		float yScale;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		bool wobbleFix;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		float maxDrawDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		FName collisionPreset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		bool castShadow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		int32 seed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = Spline)
		bool bClosedLoop;
	// =======================================

	// =======================================
	/** Mesh Extra */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = MeshExtra)
		TArray<FSTMeshExtraPrimitiveComp> meshExtraArray;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = MeshExtra)
		bool bMeshExtraEnable;
	// =======================================

	// =======================================
	/** Start Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = StartMesh)
		UStaticMesh* startStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = StartMesh)
		bool bStartMeshEnable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = StartMesh)
		float startMeshXScale;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = StartMesh)
		float startMeshYScale;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = StartMesh)
		bool bStartMeshYScaleEnable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = StartMesh)
		UObject* startMeshExtra;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = StartMesh)
		bool bStartMeshExtraEnable;
	// =======================================

	// =======================================
	/** End Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = EndMesh)
		UStaticMesh* endStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = EndMesh)
		bool bEndMeshEnable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = EndMesh)
		float endMeshXScale;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = EndMesh)
		float endMeshYScale;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = EndMesh)
		bool bEndMeshYScaleEnable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = EndMesh)
		UObject* endMeshExtra;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Category = EndMesh)
		bool bEndMeshExtraEnable;
	// =======================================

	// =======================================
	/** Children Option */
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"), Category = Children)
		TArray<AHandrailActor*> childrenHandrail;
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"), Category = Children)
		AActor* firstPointParent;
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"), Category = Children)
		AActor* lastPointParent;
	// =======================================

	// =======================================
	/** Split Mesh */
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"), Category = HandrailHelper_Split)
		UStaticMesh* splitMesh;
	// =======================================

public:
	/** Blueprint Read or Write Variable */
	// =======================================
	/** Spline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spline)
		float meshLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spline)
		EMENDistributionMethod distributionMethod;
	// =======================================

public:
	AHandrailActor(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed() override;

	// =======================================
	/** C++ 구조화 이후 현재 사용되지 않은 함수 (2019.08.21) */
	//MeshExtra를 InstancedStaticMeshComponent로 추가
	UFUNCTION(BlueprintCallable, Category = HandrailActor)
		UInstancedStaticMeshComponent* AddInstancedMeshExtra(UStaticMesh* InStaticMesh, const FTransform& InLocalTransform, float InMaxDrawDistance, FName InCollisionProfileName, bool InCastShadow);

	UInstancedStaticMeshComponent* GetorCreateInstancedMeshExtra(UStaticMesh* InStaticMesh, float InMaxDrawDistance, FName InCollisionProfileName, bool InCastShadow);
	// =======================================

protected:
	TArray<UInstancedStaticMeshComponent*> MeshExtraInstancedStaticMeshCompArray;

private:
	/** Blueprint Editor Function Declare */

	/** On & Off Debug View */
	UFUNCTION(CallInEditor, Category = HandrailHelper_Debug)
		void CheckDebugViewer();

	/** Start Point에 Child Handrail 생성 */
	UFUNCTION(CallInEditor, Category = HandrailHelper_Children)
		void AddChildrenAtStart();
	/** End Point에 Child Handrail 생성 */
	UFUNCTION(CallInEditor, Category = HandrailHelper_Children)
		void AddChildrenAtLast();
	/** Parent로부터 현재 Handrail 분리 */
	UFUNCTION(CallInEditor, Category = HandrailHelper_Snap)
		void Detach();
	// =======================================
	/** Near Edge Attach */
	UFUNCTION(CallInEditor, Category = HandrailHelper_Snap)
		void StartPointSnap();
	UFUNCTION(CallInEditor, Category = HandrailHelper_Snap)
		void LastPointSnap();
	// =======================================
	
	/** Pivot Point Move to Start Spline Point */
	UFUNCTION(CallInEditor, Category = HandrailHelper)
		void ResetOrigin();
	
	// =======================================
	/** Near Road Attach */
	UFUNCTION(CallInEditor, Category = HandrailHelper_Snap)
		void StartPointAlignToRoad();
	UFUNCTION(CallInEditor, Category = HandrailHelper_Snap)
		void LastPointAlignToRoad();
	// =======================================

	// =======================================
	/** Split Road */
	UFUNCTION(CallInEditor, Category = HandrailHelper_Split)
		void StartToSplit();
	UFUNCTION(CallInEditor, Category = HandrailHelper_Split)
		void EndToSplit();
	// =======================================

private:
	/** Function Declare */

	/**
	* Create Mesh Object
	*
	* @param option : Instance Mesh Infor
	*/
	void AddOption(const FSTMeshExtraPrimitiveComp option);

	/**
	* Create Mesh Object
	*
	* @param segmentsNumber :
	* @param meshSegmentIndexes :
	* @param meshLengthCoefficients :
	* @param staticMeshes :
	* @param staticMesh_Legacy :
	* @param offset :
	* @param flipY :
	* @param useScale :
	* @param useCustomTangents :
	* @param startOffset :
	* @param endOffset :
	*/
	void AddRoadSegments(
		int32 segmentsNumber,
		TArray<int32> meshSegmentIndexes,
		TArray<float> meshLengthCoefficients,
		const TArray<UStaticMesh*> staticMeshes,
		UStaticMesh* staticMesh_Legacy,
		FVector offset,
		bool flipY,
		bool useScale,
		bool useCustomTangents,
		float startOffset,
		float endOffset
	);

	/**
	* Get Mesh Length (Bounding Box X Length)
	*
	* @param mesh   - Static Mesh
	* @param xScale - Mesh Length's Default Size Scale
	*/
	float GetMeshLength(const UStaticMesh* mesh, float parm_xScale);

	/**
	* Spline Mesh Properties Settings
	*
	* @param target : Change Setting Spline Mesh
	* @param newMesh : Change target's Mesh
	* @param newCullDistance : Change target's Draw Max Distance
	* @param newCastShader : Change target's Cast Shader State
	* @param inCollisionProfileName : Change target's Collision Preset
	*/
	void SetProperties(
		USplineMeshComponent* target,
		UStaticMesh* newMesh,
		float newCullDistance,
		bool newCastShadow,
		FName inCollisionProfileName
	);

	/**
	* Calculate Mesh's Spacing and Index
	*
	* @param ref setNum : seg's Index
	* @param ref segLength : Remain Will be Create Instance Mesh
	* @param startOffset : Current Start Offset
	* @param endOffset : End Offset
	*/
	void CalculateSpacing(int32& segNum, float& segLength, float startOffset, float endOffset);


	// =======================================
	/**
	* Create Component & Initialize
	*
	* @param relativeTransform : Component's Spawn Transform
	* @param newT : TComp's Mesh
	* @param newCullDistance : Max Draw Distance (Primitive Component)
	* @param deltaLocation : TComp's Local Location Offset
	* @param deltaYaw : TComp's Local Rotation Offset
	* @param zVertical : If Use, TComp's Relative Rotation Yaw Value
	*/
	UStaticMeshComponent* CreateStaticMeshComponent(FTransform relativeTransform, UStaticMesh* newT, float newCullDistance, FVector deltaLocation, float deltaYaw, bool zVertical);
	UMDestructibleComponent* CreateDestructibleComponent(FTransform relativeTransform, USkeletalMesh* newT, float newCullDistance, FVector deltaLocation, float deltaYaw, bool zVertical);
	// =======================================

	// =======================================
	/** Update Meshes */
	void OptionMeshRoad();
	void MeshRoad();

	/**
	* Start & End Mesh 생성
	*
	* @param angle : Mesh's 방향
	* @param index : Spline Index Point
	* @param staticMesh : Static Mesh 소스
	* @param meshXScale : 사용자 설정 X Scale
	* @param meshYScale : 사용자 설정 Y Scale
	* @param bMYScaleEnable : 사용자 설정 Y Scale 사용 여부
	* @param bMeshExtraEnable : Mesh Extra 사용 여부
	* @param meshExtra : Mesh Extra 소스
	* 
	* @return UStaticMeshComponent* : Start or End Mesh Component 정의
	*/
	UStaticMeshComponent* EdgeMeshRoad(float angle, int32 index, UStaticMesh* staticMesh, float meshXScale, float meshYScale, bool bYScaleEnable, bool bExtraEnable, UObject* meshExtra);
	// =======================================

	/**
	* Add Handrail at Edge Point (Start & End)
	*
	* @param mesh : Children Mesh (Start & End)
	* @param bStart : Start or End Point
	*/
	void AddChildren(UStaticMeshComponent* mesh, bool bStart);

	/**
	* Add Child Mesh로 생성된 Mesh일 경우, Location 이동에 따른 Spline 보정
	*
	* @param index : Spline Point Index
	* @param parents : Start or End Mesh
	* @param socketName : Attached Socket Name
	*/
	void AlignPointToParent(int32 index, UStaticMeshComponent* parent, FName socketName);

	/** 
	* Snap to Spline Target 
	*
	* @param bStart : Start & End Point Check
	*/
	void SnapPoint(bool bFirst);

	/**
	* Get Sphere Overlap Actors that World Static & Self Class
	*
	* @param location : Sphere Location
	* @param sphereRadius : Sphere Radius
	* @out param outerActors : Overlap Actors
	*
	* @return bool : Sphere Actors is Exist
	*/
	bool SelfSphereOverlapActorsAtPoint(FVector location, float sphereRadius, TArray<AActor*>& outerActors);

	/**
	* Start or End Mesh Road Connect Near Mesh Road
	*
	* @param bStart : Start & End Mesh Check
	* @param otherRoad : Near Mesh Road
	* @param mesh : Can be Connected Mesh
	*/
	void AlignMeshToRoad(bool bStart, AHandrailActor* otherRoad, UStaticMeshComponent* mesh);

	/**
	* Split to Road
	*/
	void SplitToRoad(bool bStart);

	/**
	* 현재 Actor의 해당 Component 모두 제거
	*
	* @param compClass : Component::StaticClass()
	*/
	void DestroyComponentsByThisClass(TSubclassOf<UActorComponent> compClass);

	/** Notify Detach Children Actor */
	void NotifyDetachChildren(AHandrailActor* children);

	/** Notify Attach Children Actor */
	void NotifyAttachChildren(AHandrailActor* children);

	/** Call Children Handrail's Construction */
	void UpdateChildren();

	/** Update Letter Position */
	void UpdateLetter();
};