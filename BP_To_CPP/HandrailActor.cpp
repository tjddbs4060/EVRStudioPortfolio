#include "HandrailActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Engine.h"
#include "Engine/StaticMesh.h"

#include "System/MDestructibleComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AHandrailActor::AHandrailActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/** Initialize Value */
	splineStaticMesh = nullptr;
	xScale = 1.0f;
	yScale = 1.0f;
	maxDrawDistance = 50000;
	collisionPreset = FName("Blockall");
	castShadow = true;
	meshLength = 1000;
	distributionMethod = EMENDistributionMethod::SINGLE_LEGRACY;

	bMeshExtraEnable = true;

	bStartMeshEnable = false;
	startMeshXScale = 1.0f;
	startMeshYScale = -1.0f;
	bStartMeshYScaleEnable = true;

	bEndMeshEnable = false;
	endMeshXScale = 1.0f;
	endMeshYScale = 1.0f;
	bEndMeshYScaleEnable = false;

	firstPointParentSocketName = NAME_None;
	lastPointParentSocketName = NAME_None;

	bAttachFirstPointToStartMesh = false;
	bAttachFirstPointToEndMesh = false;
	bAttachLastPointToStartMesh = false;
	bAttachLastPointToEndMesh = false;

	/** Initialize Component */
	rootSceneComp = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Scene"));
	rootSceneComp->SetMobility(EComponentMobility::Static);
	rootSceneComp->SetupAttachment(RootComponent);

	spline = CreateOptionalDefaultSubobject<USplineComponent>(TEXT("Spline"));
	spline->SetMobility(EComponentMobility::Static);
	spline->bSplineHasBeenEdited = true;
	spline->bInputSplinePointsToConstructionScript = true;
	spline->bDrawDebug = true;
	spline->SetClosedLoop(false);
	spline->SetupAttachment(rootSceneComp);
	spline->SetRelativeTransform(FTransform());

	start = CreateOptionalDefaultSubobject<UTextRenderComponent>(TEXT("Start"));
	start->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	start->SetMaterial(0, nullptr);
	start->SetText(LOCTEXT("Start Text", "S"));
	start->TextMaterial = nullptr;
	start->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	start->SetWorldSize(35.0f);
	start->SetVisibility(false);
	start->SetHiddenInGame(true);
	start->bIsEditorOnly = true;
	start->SetupAttachment(spline);

	end = CreateOptionalDefaultSubobject<UTextRenderComponent>(TEXT("End"));
	end->SetRelativeLocation(FVector(0.0f, -150.0f, 0.0f));
	end->SetMaterial(0, nullptr);
	end->SetText(LOCTEXT("End Text", "E"));
	end->TextMaterial = nullptr;
	end->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	end->SetWorldSize(35.0f);
	end->SetVisibility(false);
	end->SetHiddenInGame(true);
	end->bIsEditorOnly = true;
	end->SetupAttachment(start);
}

void AHandrailActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// =======================================
	/** 이전에 생성된 Component Destroy */
	DestroyComponentsByThisClass(UMDestructibleComponent::StaticClass());
	DestroyComponentsByThisClass(UInstancedStaticMeshComponent::StaticClass());
	DestroyComponentsByThisClass(UStaticMeshComponent::StaticClass());
	// =======================================

	/** Start & End Mesh 옵션 체크 */
	if (startStaticMesh == nullptr)
		bStartMeshEnable = false;
	if (endStaticMesh == nullptr)
		bEndMeshEnable = false;
	if (startMeshExtra == nullptr)
		bStartMeshExtraEnable = false;
	if (endMeshExtra == nullptr)
		bEndMeshExtraEnable = false;

	/** Start & End Mesh 추가 */
	if (bStartMeshEnable)
		startMeshComponent = EdgeMeshRoad(180.0f, 0, startStaticMesh, startMeshXScale, startMeshYScale, bStartMeshYScaleEnable, bStartMeshExtraEnable, startMeshExtra);
	else
		startMeshComponent = nullptr;

	if (bEndMeshEnable)
		endMeshComponent = EdgeMeshRoad(0.0f, spline->GetNumberOfSplinePoints() - 1, endStaticMesh, endMeshXScale, endMeshYScale, bEndMeshYScaleEnable, bEndMeshExtraEnable, endMeshExtra);
	else
		endMeshComponent = nullptr;

	/** Attach to Parent */
	AHandrailActor* firstCastActor = Cast<AHandrailActor>(firstPointParent);
	AHandrailActor* lastCastActor = Cast<AHandrailActor>(lastPointParent);

	if (firstCastActor != nullptr)
	{
		if (bAttachFirstPointToStartMesh)
			AlignPointToParent(0, firstCastActor->startMeshComponent, firstPointParentSocketName);
		if (bAttachFirstPointToEndMesh)
			AlignPointToParent(0, firstCastActor->endMeshComponent, firstPointParentSocketName);
	}
	if (lastCastActor != nullptr)
	{
		if (bAttachLastPointToStartMesh)
			AlignPointToParent(spline->GetNumberOfSplinePoints() - 1, lastCastActor->startMeshComponent, lastPointParentSocketName);
		if (bAttachLastPointToEndMesh)
			AlignPointToParent(spline->GetNumberOfSplinePoints() - 1, lastCastActor->endMeshComponent, lastPointParentSocketName);
	}

	/** Option Mesh 추가 */
	if (bMeshExtraEnable)
		OptionMeshRoad();

	/** 기본 Mesh 추가 */
	MeshRoad();

	/** Update Children Handrail Actor */
	UpdateChildren();

	/** Update Letter */
	UpdateLetter();

	MeshExtraInstancedStaticMeshCompArray.Empty();
}

void AHandrailActor::Destroyed()
{
	Super::Destroyed();

	Detach();
}

UInstancedStaticMeshComponent* AHandrailActor::AddInstancedMeshExtra(UStaticMesh* InStaticMesh, const FTransform& InLocalTransform, float InMaxDrawDistance, FName InCollisionProfileName, bool InCastShadow)
{
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent = GetorCreateInstancedMeshExtra(InStaticMesh, InMaxDrawDistance, InCollisionProfileName, InCastShadow);
	if (!ensure(InstancedStaticMeshComponent != nullptr))
		return nullptr;

	InstancedStaticMeshComponent->AddInstance(InLocalTransform);

	return InstancedStaticMeshComponent;
}

UInstancedStaticMeshComponent* AHandrailActor::GetorCreateInstancedMeshExtra(UStaticMesh* InStaticMesh, float InMaxDrawDistance, FName InCollisionProfileName, bool InCastShadow)
{
	UInstancedStaticMeshComponent* FindInstancedStaticMeshComponent = nullptr;
	for (UInstancedStaticMeshComponent* It : MeshExtraInstancedStaticMeshCompArray)
	{
		if (It == nullptr)
			continue;

		if (It->GetStaticMesh() == InStaticMesh)
		{
			FindInstancedStaticMeshComponent = It;
			break;
		}
	}

	if (FindInstancedStaticMeshComponent != nullptr)
		return FindInstancedStaticMeshComponent;

	UActorComponent* NewComp = CreateComponentFromTemplate(UInstancedStaticMeshComponent::StaticClass()->GetDefaultObject<UActorComponent>());
	if (NewComp && NewComp->IsA(UStaticMeshComponent::StaticClass()) == true)
	{
		UInstancedStaticMeshComponent* InstancedStaticMeshComponent = Cast<UInstancedStaticMeshComponent>(NewComp);
		InstancedStaticMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		InstancedStaticMeshComponent->SetStaticMesh(InStaticMesh);
		InstancedStaticMeshComponent->SetMobility(EComponentMobility::Static);
		InstancedStaticMeshComponent->EmptyOverrideMaterials();

		InstancedStaticMeshComponent->OnComponentCreated();
		InstancedStaticMeshComponent->RegisterComponent();
		MeshExtraInstancedStaticMeshCompArray.AddUnique(InstancedStaticMeshComponent);

		return InstancedStaticMeshComponent;
	}

	return nullptr;
}

void AHandrailActor::CheckDebugViewer()
{
	/** First & Last Point Snap Debug View */
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World), 100.0f, 12, FLinearColor::Green, 0.5f);
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), spline->GetLocationAtSplinePoint(spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World), 100.0f, 12, FLinearColor::Green, 0.5f);
}

void AHandrailActor::AddChildrenAtStart()
{
	if (bStartMeshEnable == false)
		return;

	check(startStaticMesh != nullptr);
	if (startStaticMesh == nullptr)
		return;

	AddChildren(startMeshComponent, true);
}

void AHandrailActor::AddChildrenAtLast()
{
	if (bEndMeshEnable == false)
		return;

	check(endStaticMesh != nullptr);
	if (endStaticMesh == nullptr)
		return;

	AddChildren(endMeshComponent, false);
}

void AHandrailActor::Detach()
{
	if (firstPointParent != nullptr)
	{
		AHandrailActor* parentsCastActor = Cast<AHandrailActor>(firstPointParent);
		parentsCastActor->NotifyDetachChildren(this);
	}
	if (lastPointParent != nullptr)
	{
		AHandrailActor* parentsCastActor = Cast<AHandrailActor>(lastPointParent);
		parentsCastActor->NotifyDetachChildren(this);
	}
	firstPointParent = nullptr;
	lastPointParent = nullptr;

	bAttachFirstPointToStartMesh = false;
	bAttachFirstPointToEndMesh = false;
	bAttachLastPointToStartMesh = false;
	bAttachLastPointToEndMesh = false;

}

void AHandrailActor::StartPointSnap()
{
	SnapPoint(true);
}

void AHandrailActor::LastPointSnap()
{
	SnapPoint(false);
}

void AHandrailActor::ResetOrigin()
{
	FVector localStartLocationVector = spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	FVector worldLastLocationVector = spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	int32 splineLength = spline->GetNumberOfSplinePoints();

	for (int32 i = 0; i < splineLength; i++)
	{
		FVector locationVector = spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector setLocationVector = locationVector - localStartLocationVector;

		spline->SetLocationAtSplinePoint(i, setLocationVector, ESplineCoordinateSpace::Local);
	}

	SetActorLocation(worldLastLocationVector);

	OnConstruction(GetActorTransform());
}

void AHandrailActor::StartPointAlignToRoad()
{
	if (bStartMeshEnable == false)
		return;

	TArray<AActor*> outerActors;

	FVector firstLocationVector = spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	if (SelfSphereOverlapActorsAtPoint(firstLocationVector, 100.0f, outerActors) == false)
		return;

	AHandrailActor* castActor = nullptr;
	for (AActor* actor : outerActors)
	{
		castActor = Cast<AHandrailActor>(actor);

		if (castActor != nullptr)
			break;
	}

	check(castActor != nullptr);
	if (castActor == nullptr)
		return;

	AlignMeshToRoad(true, castActor, startMeshComponent);
}

void AHandrailActor::LastPointAlignToRoad()
{
	if (bEndMeshEnable == false)
		return;
	TArray<AActor*> outerActors;

	FVector lastLocationVector = spline->GetLocationAtSplinePoint(spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);

	if (SelfSphereOverlapActorsAtPoint(lastLocationVector, 100.0f, outerActors) == false)
		return;

	AHandrailActor* castActor = nullptr;
	for (AActor* actor : outerActors)
	{
		castActor = Cast<AHandrailActor>(actor);

		if (castActor != nullptr)
			break;
	}

	check(castActor != nullptr);
	if (castActor == nullptr)
		return;

	AlignMeshToRoad(false, castActor, endMeshComponent);
}

void AHandrailActor::StartToSplit()
{
	SplitToRoad(true);
}

void AHandrailActor::EndToSplit()
{
	SplitToRoad(false);
}

void AHandrailActor::AddOption(const FSTMeshExtraPrimitiveComp option)
{
	if (option.mesh == nullptr || option.mesh->IsValidLowLevel() == false)
		return;

	check(spline != nullptr);
	if (spline == nullptr)
		return;

	float splineLength = spline->GetSplineLength();
	float spacing = FMath::Max(option.spacing, 50.0f);
	float remainder = 0.0f;

	/** Spline의 현재 길이 내에 생성되는 Mesh의 개수 */
	int32 length = UKismetMathLibrary::FMod(splineLength, spacing, remainder);

	for (int32 i = 0; i <= length; i++)
	{
		FRotator rotator = FRotator(0.0f, 0.0f, option.angularOffset);
		FTransform transform = FTransform(rotator);

		// ===================================
		/** Calculate Object's Distance */
		float curSpacing = spacing * i;
		FVector curSplineVector = spline->GetScaleAtDistanceAlongSpline(curSpacing);
		float localYScale = (FMath::IsNearlyEqual(yScale, 1.0f)) ? yScale : curSplineVector.Y;
		FVector curLocation = FVector(1.0f, localYScale, 1.0f) * option.offset;
		float curDistance = FMath::Fmod(curSpacing + curLocation.X, splineLength);
		// ===================================

		FTransform distanceTransform = spline->GetTransformAtDistanceAlongSpline(curDistance, ESplineCoordinateSpace::Local);
		distanceTransform = FTransform(distanceTransform.Rotator(), distanceTransform.GetLocation(), option.scale);

		FVector deltaLocation = curLocation * FVector(0.0f, 1.0f, 1.0f);

		if (option.mesh->GetClass()->IsChildOf<UStaticMesh>())
		{
			/** Mesh is Static Mesh */
			UStaticMesh* staticMesh = Cast<UStaticMesh>(option.mesh);

			check(staticMesh != nullptr);
			if (staticMesh == nullptr)
				continue;

			CreateStaticMeshComponent(distanceTransform, staticMesh, option.maxDrawDistance, deltaLocation, option.angularOffset, option.zVertical);
		}
		else if (option.mesh->GetClass()->IsChildOf<UDestructibleMesh>())
		{
			/** Mesh is Skeletal Mesh */
			USkeletalMesh* skeletalMesh = Cast<USkeletalMesh>(option.mesh);

			check(skeletalMesh != nullptr);
			if (skeletalMesh == nullptr)
				continue;

			CreateDestructibleComponent(distanceTransform, skeletalMesh, option.maxDrawDistance, deltaLocation, option.angularOffset, option.zVertical);
		}
	}
}

void AHandrailActor::AddRoadSegments(int32 segmentsNumber, TArray<int32> meshSegmentIndexes, TArray<float> meshLengthCoefficients, const TArray<UStaticMesh*> staticMeshes, UStaticMesh* staticMesh_Legacy, FVector offset, bool flipY, bool useScale, bool useCustomTangents, float startOffset, float endOffset)
{
	float curSplinePosition = startOffset;

	FVector flipVector = (flipY) ? FVector(1.0f, -1.0f, 1.0f) : FVector::OneVector;
	FTransform relativeTransform = (flipY) ? FTransform(FRotator::ZeroRotator, FVector::ZeroVector, flipVector) : FTransform();

	// ClearCreatedSpline();
	TArray<UActorComponent*> actors;
	GetComponents(USplineMeshComponent::StaticClass(), actors);

	DestroyComponentsByThisClass(USplineMeshComponent::StaticClass());

	for (int i = 0; i < segmentsNumber; i++)
	{
		USplineMeshComponent* comp = NewObject<USplineMeshComponent>(this);
		comp->AttachToComponent(rootSceneComp, FAttachmentTransformRules::KeepRelativeTransform);
		comp->SetRelativeTransform(relativeTransform);

		int32 meshIndex = (meshSegmentIndexes.Num() == 0) ? 0 : (i % meshSegmentIndexes.Num());
		UStaticMesh* sectionMesh = (distributionMethod == EMENDistributionMethod::SINGLE_LEGRACY) ? (staticMesh_Legacy) : (staticMeshes[meshIndex]);

		SetProperties(comp, sectionMesh, maxDrawDistance, castShadow, collisionPreset);

		/** Calculate Start & End Vector */
		float splineLength = spline->GetSplineLength();
		float usingSplineLength = splineLength - endOffset;
		float spacingSplineLength = splineLength - (startOffset + endOffset);

		int32 coeIndex = i % meshLengthCoefficients.Num();
		float coefficient = spacingSplineLength * meshLengthCoefficients[coeIndex];

		float startSplineDistance = FMath::Min<float>(curSplinePosition, usingSplineLength);
		float endSplineDistance = FMath::Min<float>(usingSplineLength, curSplinePosition + coefficient);
		float gapSplineDistance = endSplineDistance - startSplineDistance;

		// =======================================
		/** Calculator Gap Spline */
		FVector gapStartDistanceLocationVector = spline->GetLocationAtDistanceAlongSpline(startSplineDistance - gapSplineDistance, ESplineCoordinateSpace::Local);
		FVector gapEndDistanceLocationVector = spline->GetLocationAtDistanceAlongSpline(endSplineDistance + gapSplineDistance, ESplineCoordinateSpace::Local);
		FTransform gapStartDistanceTransform = spline->GetTransformAtDistanceAlongSpline(startSplineDistance - gapSplineDistance, ESplineCoordinateSpace::Local, true);
		FTransform gapEndDistanceTransform = spline->GetTransformAtDistanceAlongSpline(endSplineDistance + gapSplineDistance, ESplineCoordinateSpace::Local, true);
		FVector gapStartDirectionVector = offset * ((FMath::IsNearlyEqual(yScale, 1.0f)) ? FVector(1.0f, gapStartDistanceTransform.GetScale3D().Y, 1.0f) : FVector(1.0f, yScale, 1.0f));
		FVector gapEndDirectionVector = offset * ((FMath::IsNearlyEqual(yScale, 1.0f)) ? FVector(1.0f, gapEndDistanceTransform.GetScale3D().Y, 1.0f) : FVector(1.0f, yScale, 1.0f));

		FVector gapStartTransformDirection = UKismetMathLibrary::TransformDirection(gapStartDistanceTransform, gapStartDirectionVector);
		FVector gapEndTransformDirection = UKismetMathLibrary::TransformDirection(gapEndDistanceTransform, gapEndDirectionVector);
		// =======================================

		// =======================================
		/** Calculator Start Spline */
		FVector startDistanceDirectionVector = spline->GetDirectionAtDistanceAlongSpline(startSplineDistance, ESplineCoordinateSpace::Local);
		FVector startDistanceLocationVector = spline->GetLocationAtDistanceAlongSpline(startSplineDistance, ESplineCoordinateSpace::Local);
		FTransform startDistanceTransform = spline->GetTransformAtDistanceAlongSpline(startSplineDistance, ESplineCoordinateSpace::Local, true);
		FVector startDistanceScaleVector = spline->GetScaleAtDistanceAlongSpline(startSplineDistance);
		FVector startDirectionVector = offset * ((FMath::IsNearlyEqual(yScale, 1.0f)) ? FVector(1.0f, startDistanceScaleVector.Y, 1.0f) : FVector(1.0f, yScale, 1.0f));

		FVector startTransformDirection = UKismetMathLibrary::TransformDirection(startDistanceTransform, startDirectionVector) + startDistanceLocationVector;
		// =======================================

		// =======================================
		/** Calculator End Spline */
		FVector endDistanceDirectionVector = spline->GetDirectionAtDistanceAlongSpline(endSplineDistance, ESplineCoordinateSpace::Local);
		FVector endDistanceLocationVector = spline->GetLocationAtDistanceAlongSpline(endSplineDistance, ESplineCoordinateSpace::Local);
		FTransform endDistanceTransform = spline->GetTransformAtDistanceAlongSpline(endSplineDistance, ESplineCoordinateSpace::Local, true);
		FVector endDistanceScaleVector = spline->GetScaleAtDistanceAlongSpline(endSplineDistance);
		FVector endDirectionVector = offset * ((FMath::IsNearlyEqual(yScale, 1.0f)) ? FVector(1.0f, endDistanceScaleVector.Y, 1.0f) : FVector(1.0f, yScale, 1.0f));

		FVector endTransformDirection = UKismetMathLibrary::TransformDirection(endDistanceTransform, endDirectionVector) + endDistanceLocationVector;
		// =======================================


		// =======================================
		/** Calculator Start Tangent */
		FVector startTangentConsiderScale = endTransformDirection - (gapStartDistanceLocationVector + gapStartTransformDirection);
		UKismetMathLibrary::Vector_Normalize(startTangentConsiderScale, 0.0001);
		// =======================================

		// =======================================
		/** Calculator Start Tangent */
		FVector endTangentConsiderScale = (gapEndDistanceLocationVector + gapEndTransformDirection) - startTransformDirection;
		UKismetMathLibrary::Vector_Normalize(endTangentConsiderScale, 0.0001);
		// =======================================

		bool bZeroOffset = FMath::IsNearlyEqual(offset.Size(), 0.0f);
		bool bStartTangent = !bZeroOffset && useCustomTangents;

		/** Set Start & End Spline */
		FVector startPos = startTransformDirection;
		FVector startTangent = (bStartTangent) ? (startTangentConsiderScale) : (startDistanceDirectionVector);
		FVector endPos = endTransformDirection;
		FVector endTangent = (bStartTangent) ? (endTangentConsiderScale) : (endDistanceDirectionVector);

		float tangentRate = (bZeroOffset) ? (gapSplineDistance) : ((endTransformDirection - startTransformDirection).Size());

		startTangent *= tangentRate;
		endTangent *= tangentRate;

		startPos *= flipVector;
		startTangent *= flipVector;
		endPos *= flipVector;
		endTangent *= flipVector;

		comp->SetStartAndEnd(startPos, startTangent, endPos, endTangent);

		if (useScale)
		{
			float startScaleX = (FMath::IsNearlyEqual(yScale, 1.0f)) ? (yScale) : (startDistanceScaleVector.Y);
			comp->SetStartScale(FVector2D(startScaleX, 1.0f));

			float endScaleX = (FMath::IsNearlyEqual(yScale, 1.0f)) ? (yScale) : (endDistanceScaleVector.Y);
			comp->SetEndScale(FVector2D(endScaleX, 1.0f));
		}

		comp->RegisterComponent();
		curSplinePosition += gapSplineDistance;
	}
}

float AHandrailActor::GetMeshLength(const UStaticMesh* mesh, float parm_xScale)
{
	FBox box = mesh->GetBoundingBox();

	float minX = FMath::Abs(box.Min.X);
	float maxX = box.Max.X;

	float scale = (FMath::IsNearlyEqual(parm_xScale, 0.0f)) ? 1.0f : parm_xScale;
	scale = FMath::Max(scale, 0.1f);

	return (minX + maxX) * scale;
}

void AHandrailActor::SetProperties(USplineMeshComponent* target, UStaticMesh* newMesh, float newCullDistance, bool newCastShadow, FName inCollisionProfileName)
{
	check(target != nullptr);
	if (target == nullptr)
		return;

	if (newMesh != nullptr)
	{
		target->SetStaticMesh(newMesh);
	}

	target->SetCachedMaxDrawDistance(newCullDistance);
	target->SetCastShadow(newCastShadow);
	target->SetCollisionProfileName(inCollisionProfileName);
}

void AHandrailActor::CalculateSpacing(int32& segNum, float& segLength, float startOffset, float endOffset)
{
	check(spline != nullptr);
	if (spline == nullptr)
		return;

	float length = spline->GetSplineLength();
	float remainLength = length - (startOffset + endOffset);

	float remainder = 0.0f;

	int32 seg = UKismetMathLibrary::FMod(remainLength, meshLength, remainder);

	segNum = (remainder >= (meshLength * 0.5)) ? (seg + 1) : FMath::Max(seg, 1);
	segLength = remainLength / segNum;
}

UStaticMeshComponent* AHandrailActor::CreateStaticMeshComponent(FTransform relativeTransform, UStaticMesh* newT, float newCullDistance, FVector deltaLocation, float deltaYaw, bool zVertical)
{
	UStaticMeshComponent* comp = NewObject<UStaticMeshComponent>(this);
	comp->AttachToComponent(rootSceneComp, FAttachmentTransformRules::KeepRelativeTransform);
	comp->SetRelativeTransform(relativeTransform);

	comp->SetStaticMesh(newT);

	comp->SetCachedMaxDrawDistance(newCullDistance);
	comp->AddLocalOffset(deltaLocation);
	comp->AddLocalRotation(FRotator(0.0f, 0.0f, deltaYaw));

	if (zVertical)
		comp->SetRelativeRotation(FRotator(0.0f, 0.0f, comp->GetRelativeRotation().Yaw));

	comp->SetMobility(EComponentMobility::Static);
	comp->RegisterComponent();

	return comp;
}

UMDestructibleComponent* AHandrailActor::CreateDestructibleComponent(FTransform relativeTransform, USkeletalMesh* newT, float newCullDistance, FVector deltaLocation, float deltaYaw, bool zVertical)
{
	UMDestructibleComponent* comp = NewObject<UMDestructibleComponent>(this);
	comp->AttachToComponent(rootSceneComp, FAttachmentTransformRules::KeepRelativeTransform);
	comp->SetRelativeTransform(relativeTransform);
	comp->SetSkeletalMesh(newT);

	comp->SetCachedMaxDrawDistance(newCullDistance);
	comp->AddLocalOffset(deltaLocation);
	comp->AddLocalRotation(FRotator(0.0f, 0.0f, deltaYaw));

	if (zVertical)
		comp->SetRelativeRotation(FRotator(0.0f, 0.0f, comp->GetRelativeRotation().Yaw));

	comp->SetMobility(EComponentMobility::Static);
	comp->RegisterComponent();

	return comp;
}

void AHandrailActor::OptionMeshRoad()
{
	for (FSTMeshExtraPrimitiveComp meshOption : meshExtraArray)
	{
		AddOption(meshOption);
	}
}

void AHandrailActor::MeshRoad()
{
	if (splineStaticMesh == nullptr)
		return;

	meshLength = GetMeshLength(splineStaticMesh, 0.0f);

	int32 segNum = 0;
	float segLength = 0.0f;

	CalculateSpacing(segNum, segLength, 0.0f, 0.0f);

	FRandomStream stream = UKismetMathLibrary::MakeRandomStream(seed);

	TArray<UStaticMesh*> localSplineMeshes = splineMeshes;

	float totalMeshesLength = meshLength * segNum;
	TArray<float> meshLengthCoefficients;

	for (int32 i = 0; i < segNum; i++)
	{
		meshLengthCoefficients.Add(meshLength / totalMeshesLength);
	}

	TArray<int32> meshSegmentIndexes;
	AddRoadSegments(segNum, meshSegmentIndexes, meshLengthCoefficients, localSplineMeshes, splineStaticMesh, FVector::ZeroVector, false, true, false, 0.0f, 0.0f);
}

UStaticMeshComponent* AHandrailActor::EdgeMeshRoad(float angle, int32 index, UStaticMesh* staticMesh, float meshXScale, float meshYScale, bool bYScaleEnable, bool bExtraEnable, UObject* meshExtra)
{
	check(staticMesh != nullptr);
	if (staticMesh == nullptr)
		return nullptr;

	// =======================================
	/** Add Static Mesh */
	UStaticMeshComponent* comp = NewObject<UStaticMeshComponent>(this);
	comp->AttachToComponent(rootSceneComp, FAttachmentTransformRules::KeepRelativeTransform);

	FTransform splineTransform = spline->GetTransformAtSplinePoint(index, ESplineCoordinateSpace::Local, true);
	FVector splineLocation = splineTransform.GetLocation();
	FRotator splineRotator = spline->GetRotationAtSplinePoint(index, ESplineCoordinateSpace::Local);
	FVector splineScale = splineTransform.GetScale3D();

	float relativeYScale = (FMath::IsNearlyEqual(yScale, 1.0f)) ? (splineScale.Y) : (yScale);
	relativeYScale = bYScaleEnable ? meshYScale : relativeYScale;

	FRotator relativeRotator = FRotator(0.0f, splineRotator.Yaw + angle, 0.0f);
	FVector relativeScale = FVector(xScale, relativeYScale, 1.0f);
	FTransform relativeTransform = FTransform(relativeRotator, splineLocation, relativeScale);

	comp->SetRelativeTransform(relativeTransform);

	comp->SetStaticMesh(staticMesh);
	comp->SetCachedMaxDrawDistance(maxDrawDistance);
	comp->SetCastShadow(castShadow);
	comp->SetCollisionProfileName(collisionPreset);

	comp->RegisterComponent();
	// =======================================

	// =======================================
	/** Add Extra Mesh */
	if (bExtraEnable == false)
		return comp;

	check(meshExtra != nullptr);
	if (meshExtra == nullptr)
		return comp;

	UPrimitiveComponent* extraPrimitiveComp = nullptr;

	if (meshExtra->GetClass()->IsChildOf<UStaticMesh>())
	{
		UStaticMesh* mesh = Cast<UStaticMesh>(meshExtra);

		UStaticMeshComponent* extraComp = NewObject<UStaticMeshComponent>(this);
		extraComp->AttachToComponent(rootSceneComp, FAttachmentTransformRules::KeepRelativeTransform);
		extraComp->SetStaticMesh(mesh);
		extraComp->RegisterComponent();

		extraPrimitiveComp = Cast<UPrimitiveComponent>(extraComp);
	}
	else if (meshExtra->GetClass()->IsChildOf<USkeletalMesh>())
	{
		USkeletalMesh* mesh = Cast<USkeletalMesh>(meshExtra);
		UMDestructibleComponent* extraComp = NewObject<UMDestructibleComponent>(this);
		extraComp->AttachToComponent(rootSceneComp, FAttachmentTransformRules::KeepRelativeTransform);
		extraComp->SetSkeletalMesh(mesh);
		extraComp->RegisterComponent();

		extraPrimitiveComp = Cast<UPrimitiveComponent>(extraComp);
	}

	if (extraPrimitiveComp == nullptr)
		return comp;

	extraPrimitiveComp->AttachToComponent(comp, FAttachmentTransformRules::SnapToTargetIncludingScale);

	extraPrimitiveComp->SetCachedMaxDrawDistance(maxDrawDistance);
	extraPrimitiveComp->SetCastShadow(castShadow);
	extraPrimitiveComp->SetCollisionProfileName(collisionPreset);
	// =======================================

	return comp;
}

void AHandrailActor::AddChildren(UStaticMeshComponent* mesh, bool bStart)
{
	TArray<FName> socketNames = mesh->GetAllSocketNames();

	if (socketNames.Num() <= 0)
		return;
	
	FName socketName = socketNames[0];

	FTransform socketTransform = mesh->GetSocketTransform(socketName);

	TArray<AActor*> outerActors;
	
	/** 찾지 못하였을 때 생성 */
	if (SelfSphereOverlapActorsAtPoint(socketTransform.GetLocation(), 10.0f, outerActors) == false)
	{
		AHandrailActor* spawnHandrail = Cast<AHandrailActor>(GetWorld()->SpawnActor(GetClass()));
		
		check(spawnHandrail != nullptr);
		if (spawnHandrail == nullptr)
			return;

		spawnHandrail->SetActorTransform(socketTransform);

		if (bStart)
			spawnHandrail->bAttachFirstPointToStartMesh = true;
		else
			spawnHandrail->bAttachFirstPointToEndMesh = true;
		spawnHandrail->firstPointParent = this;
		spawnHandrail->firstPointParentSocketName = socketName;

		/** Spawn Handrail Initialize */
		spawnHandrail->splineStaticMesh = splineStaticMesh;
		spawnHandrail->meshExtraArray = meshExtraArray;
		spawnHandrail->bMeshExtraEnable = bMeshExtraEnable;

		spawnHandrail->startStaticMesh = startStaticMesh;
		spawnHandrail->startMeshExtra = startMeshExtra;

		spawnHandrail->endStaticMesh = endStaticMesh;
		spawnHandrail->endMeshExtra = endMeshExtra;

		spawnHandrail->xScale = xScale;
		spawnHandrail->yScale = yScale;

		spawnHandrail->startMeshXScale = startMeshXScale;
		spawnHandrail->startMeshYScale = startMeshYScale;
		spawnHandrail->bStartMeshYScaleEnable = bStartMeshYScaleEnable;
		spawnHandrail->endMeshXScale = endMeshXScale;
		spawnHandrail->endMeshYScale = endMeshYScale;
		spawnHandrail->bEndMeshYScaleEnable = bEndMeshYScaleEnable;

		spawnHandrail->maxDrawDistance = maxDrawDistance;
		spawnHandrail->castShadow = castShadow;
		spawnHandrail->collisionPreset = collisionPreset;

		spawnHandrail->wobbleFix = wobbleFix;
		spawnHandrail->seed = seed;
		spawnHandrail->bClosedLoop = bClosedLoop;

		spawnHandrail->meshLength = meshLength;
		spawnHandrail->distributionMethod = distributionMethod;

		spawnHandrail->OnConstruction(spawnHandrail->GetTransform());
		
		childrenHandrail.Add(spawnHandrail);
	}
}

void AHandrailActor::AlignPointToParent(int32 index, UStaticMeshComponent* parent, FName socketName)
{
	check(parent != nullptr);
	if (parent == nullptr)
		return;

	FTransform socketTransform = parent->GetSocketTransform(socketName);

	spline->SetLocationAtSplinePoint(index, socketTransform.GetLocation(), ESplineCoordinateSpace::World);

	FVector arriveTangentVector = spline->GetArriveTangentAtSplinePoint(index, ESplineCoordinateSpace::Local);
	FVector leaveTangentVector = spline->GetLeaveTangentAtSplinePoint(index, ESplineCoordinateSpace::Local);

	float inverse = (index == 0) ? 1.0f : -1.0f;
	FVector rotatorVector = socketTransform.Rotator().Vector();

	float arriveLength = arriveTangentVector.Size() * inverse;
	float leaveLength = leaveTangentVector.Size() * inverse;

	spline->SetTangentsAtSplinePoint(index, rotatorVector * arriveLength, rotatorVector * leaveLength, ESplineCoordinateSpace::World);
}

void AHandrailActor::SnapPoint(bool bFirst)
{
	const float snapRadius = 100.0f;

	int32 index = (bFirst ? 0 : spline->GetNumberOfSplinePoints() - 1);
	/** First or Last Spline Snap Setting */
	FVector splineLocation = spline->GetLocationAtSplinePoint(index, ESplineCoordinateSpace::World);

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(this);

	TArray<UPrimitiveComponent*> outerComponents;

	/** Get Near All Static Component */
	if (UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), splineLocation, snapRadius, objectTypes, UStaticMeshComponent::StaticClass(), ignoreActors, outerComponents) == false)
		return;

	bool bStartMesh = false;

	FName socketName = NAME_None;
	AHandrailActor* pointParent = nullptr;

	for (UPrimitiveComponent* primitiveComp : outerComponents)
	{
		TArray<FName> socketNames = primitiveComp->GetAllSocketNames();

		for (FName name : socketNames)
		{
			FTransform socketTransform = primitiveComp->GetSocketTransform(name);

			FVector distance = socketTransform.GetLocation() - splineLocation;

			if (distance.Size() <= snapRadius)
			{
				AHandrailActor* handrailActor = Cast<AHandrailActor>(primitiveComp->GetOwner());

				if (handrailActor == nullptr)
					continue;

				/** Check Near Static Mesh is Start & End Mesh*/
				if (handrailActor->startMeshComponent == primitiveComp)
					bStartMesh = true;
				else if (handrailActor->endMeshComponent == primitiveComp)
					bStartMesh = false;
				else
					continue;

				socketName = name;
				pointParent = handrailActor;

				break;
			}
		}
	}

	if (pointParent == nullptr)
		return;

	if (bFirst)
	{
		if (bStartMesh)
		{
			bAttachFirstPointToStartMesh = true;
		}
		else
		{
			bAttachFirstPointToEndMesh = true;
		}

		firstPointParentSocketName = socketName;
		firstPointParent = pointParent;
	}
	else
	{
		if (bStartMesh)
		{
			bAttachLastPointToStartMesh = true;
		}
		else
		{
			bAttachLastPointToEndMesh = true;
		}

		lastPointParentSocketName = socketName;
		lastPointParent = pointParent;
	}

	/** Attach Point Setting */
	FTransform attachTrasnform = (bStartMesh ? (pointParent->startMeshComponent->GetSocketTransform(socketName)) : (pointParent->endMeshComponent->GetSocketTransform(socketName)));

	pointParent->NotifyAttachChildren(this);

	SetActorTransform(attachTrasnform);
	OnConstruction(GetActorTransform());
}

bool AHandrailActor::SelfSphereOverlapActorsAtPoint(FVector location, float sphereRadius, TArray<AActor*>& outerActors)
{
	// =======================================
	/** Sphere Overlap Actors Parameter Setting */
	TArray<TEnumAsByte<EObjectTypeQuery>> types;
	types.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(this);
	// =======================================

	return UKismetSystemLibrary::SphereOverlapActors(GetWorld(), location, sphereRadius, types, GetClass(), ignoreActors, outerActors);
}

void AHandrailActor::AlignMeshToRoad(bool bStart, AHandrailActor* otherRoad, UStaticMeshComponent* mesh)
{
	USplineComponent* roadSpline = otherRoad->spline;
	
	check(roadSpline != nullptr);
	if (roadSpline == nullptr)
		return;

	int32 index = (bStart ? 0 : (spline->GetNumberOfSplinePoints() - 1));

	FVector pointLocation = spline->GetLocationAtSplinePoint(index, ESplineCoordinateSpace::World);

	FVector centerPoint = roadSpline->FindLocationClosestToWorldLocation(pointLocation, ESplineCoordinateSpace::World);

	spline->SetLocationAtSplinePoint(index, centerPoint, ESplineCoordinateSpace::World);

	ResetOrigin();
}

void AHandrailActor::SplitToRoad(bool bStart)
{
	if (splitMesh == nullptr)
		return;

	UStaticMeshComponent* mainMesh = (bStart ? startMeshComponent : endMeshComponent);

	if (mainMesh == nullptr)
		return;

	// =======================================
	/** Find Get Near Actor */
	int32 index = (bStart ? 0 : spline->GetNumberOfSplinePoints() - 1);

	TArray<AActor*> outerActors;

	FVector locationVector = spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	if (SelfSphereOverlapActorsAtPoint(locationVector, 100.0f, outerActors) == false)
		return;

	AHandrailActor* castActor = nullptr;
	for (AActor* actor : outerActors)
	{
		castActor = Cast<AHandrailActor>(actor);

		if (castActor != nullptr)
			break;
	}

	check(castActor != nullptr);
	if (castActor == nullptr)
		return;
	// =======================================

	// =======================================
	/** Spawn First Spline Actor */
	FTransform spawnTransform = castActor->GetTransform();

	AHandrailActor* firstSpawnHandrail = Cast<AHandrailActor>(GetWorld()->SpawnActor(GetClass()));
	firstSpawnHandrail->SetActorTransform(spawnTransform);

	USplineComponent* firstSpline = firstSpawnHandrail->spline;
	firstSpline->ClearSplinePoints(false);
	// =======================================

	// =======================================
	/** Spawn Last Spline Actor */
	AHandrailActor* lastSpawnHandrail = Cast<AHandrailActor>(GetWorld()->SpawnActor(GetClass()));
	lastSpawnHandrail->SetActorTransform(spawnTransform);

	USplineComponent* lastSpline = lastSpawnHandrail->spline;
	lastSpline->ClearSplinePoints(false);
	// =======================================

	USplineComponent* castSpline = castActor->spline;

	FVector closestRightVector = castSpline->FindRightVectorClosestToWorldLocation(locationVector, ESplineCoordinateSpace::World);
	FVector tangentVector = spline->GetTangentAtSplinePoint(index, ESplineCoordinateSpace::World);

	bool bDotZero = (UKismetMathLibrary::Dot_VectorVector(closestRightVector, tangentVector) < 0);
	bool bPoint = (bStart ? !bDotZero : bDotZero);

	FVector leftLocation = mainMesh->GetSocketLocation(FName("Left"));
	FVector rightLocation = mainMesh->GetSocketLocation(FName("Right"));

	FVector firstPointVector = (bPoint ? leftLocation : rightLocation);
	FVector secondPointVector = (bPoint ? rightLocation : leftLocation);


	FVector rightVector = spline->GetRightVectorAtSplinePoint(index, ESplineCoordinateSpace::World);
	FVector locationClosestTangentVector = castSpline->FindTangentClosestToWorldLocation(locationVector, ESplineCoordinateSpace::World);

	bool bDotTangentZero = (UKismetMathLibrary::Dot_VectorVector(rightVector, locationClosestTangentVector) >= 0);

	FVector tangentOffsetVector = (bDotTangentZero ? rightVector : UKismetMathLibrary::NegateVector(rightVector));

	// =======================================
	/** Make First Spline */
	int32 firstSplineIndex = castSpline->FindInputKeyClosestToWorldLocation(firstPointVector);

	for (int i = 0; i <= firstSplineIndex; i++)
	{
		FVector pointPosition = castSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector arriveTangent = castSpline->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector leaveTangent = castSpline->GetLeaveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);

		FSplinePoint point = FSplinePoint(i, pointPosition, arriveTangent, leaveTangent, FRotator::ZeroRotator, FVector::OneVector, ESplinePointType::CurveCustomTangent);

		firstSpline->AddPoint(point, false);
	}
	FVector firstClosestLocation = castSpline->FindLocationClosestToWorldLocation(firstPointVector, ESplineCoordinateSpace::Local);
	firstSpline->AddSplinePoint(firstClosestLocation, ESplineCoordinateSpace::Local, false);
	
	firstSpline->SetTangentAtSplinePoint(firstSplineIndex + 1, tangentOffsetVector * 100.0f, ESplineCoordinateSpace::World);
	// =======================================

	// =======================================
	/** Make Last Spline */
	FVector lastClosestLocation = castSpline->FindLocationClosestToWorldLocation(secondPointVector, ESplineCoordinateSpace::Local);
	lastSpline->AddSplinePoint(lastClosestLocation, ESplineCoordinateSpace::Local, false);

	lastSpline->SetTangentAtSplinePoint(0, tangentOffsetVector * 100.0f, ESplineCoordinateSpace::World, false);

	int32 secondSplineIndex = castSpline->FindInputKeyClosestToWorldLocation(secondPointVector);
	int32 secondSplineLoop = castSpline->GetNumberOfSplinePoints() - UKismetMathLibrary::FCeil(secondSplineIndex);

	for (int i = 1; i <= secondSplineLoop; i++)
	{
		int32 pointIndex = secondSplineIndex + i;

		FVector pointPosition = castSpline->GetLocationAtSplinePoint(pointIndex, ESplineCoordinateSpace::Local);
		FVector arriveTangent = castSpline->GetArriveTangentAtSplinePoint(pointIndex, ESplineCoordinateSpace::Local);
		FVector leaveTangent = castSpline->GetLeaveTangentAtSplinePoint(pointIndex, ESplineCoordinateSpace::Local);

		FSplinePoint point = FSplinePoint(i, pointPosition, arriveTangent, leaveTangent, FRotator::ZeroRotator, FVector::OneVector, ESplinePointType::CurveCustomTangent);

		lastSpline->AddPoint(point);
	}
	// =======================================

	float splitMeshLength = GetMeshLength(splitMesh, 0.0f);

	TArray<int32> segmentArray;
	segmentArray.Add(0);

	TArray<UStaticMesh*> meshArray;
	meshArray.Add(splitMesh);
	// =======================================
	/** Draw First Temporary Road */
	float firstSplineLength = firstSpline->GetSplineLength();
	int32 firstSegmentsNumber = splitMeshLength / firstSplineLength;

	TArray<float> firstCoefficientsArray;
	firstCoefficientsArray.Add((firstSplineLength / firstSegmentsNumber) / firstSplineLength);

	AddRoadSegments(firstSegmentsNumber, segmentArray, firstCoefficientsArray, meshArray, splitMesh, FVector::ZeroVector, false, true, false, 0.0f, 0.0f);
	// =======================================

	// =======================================
	/** Draw Last Temporary Road */
	float lastSplineLength = lastSpline->GetSplineLength();
	int32 lastSegmentsNumber = splitMeshLength / lastSplineLength;

	TArray<float> lastCoefficientsArray;
	lastCoefficientsArray.Add((lastSplineLength / lastSegmentsNumber) / lastSplineLength);

	AddRoadSegments(lastSegmentsNumber, segmentArray, lastCoefficientsArray, meshArray, splitMesh, FVector::ZeroVector, false, true, false, 0.0f, 0.0f);
	// =======================================
}

void AHandrailActor::DestroyComponentsByThisClass(TSubclassOf<UActorComponent> compClass)
{
	TArray<UActorComponent*> actors;
	GetComponents(compClass, actors);

	for (UActorComponent* actor : actors)
	{
		actor->DestroyComponent();
	}
}

void AHandrailActor::NotifyDetachChildren(AHandrailActor* children)
{
	childrenHandrail.Remove(children);
}

void AHandrailActor::NotifyAttachChildren(AHandrailActor* children)
{
	childrenHandrail.Add(children);
}

void AHandrailActor::UpdateChildren()
{
	for (AHandrailActor* handrail : childrenHandrail)
	{
		check(handrail != nullptr);

		handrail->OnConstruction(handrail->GetTransform());
	}
}

void AHandrailActor::UpdateLetter()
{
	FVector startLocation = spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	FVector endLocation = spline->GetLocationAtSplinePoint(spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);

	start->SetWorldLocation(startLocation);
	end->SetWorldLocation(endLocation);
}