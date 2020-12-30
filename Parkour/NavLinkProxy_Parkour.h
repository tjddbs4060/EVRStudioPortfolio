UCLASS(Blueprintable, autoCollapseCategories = (SmartLink, Actor), hideCategories = (Input))
class ANavLinkProxy_Parkour : public AActor, public INavLinkHostInterface, public INavRelevantInterface
{
private:
	UPROPERTY(EditAnywhere, Category = "SmartLink : SettingsLink")
	int32 SelectIndex;

	UPROPERTY(EditAnywhere, Category = SmartLink)
	bool bUpdateProxy;

	/** Navigation links (point to point) added to navigation data */
	UPROPERTY(EditAnywhere, Category = Link)
	TArray<FNavigationLink> PointLinks;

	/** Navigation links (segment to segment) added to navigation data
	*	@todo hidden from use until we fix segment links. Not really working now*/
	UPROPERTY()
	TArray<FNavigationSegmentLink> SegmentLinks;

	/** Smart link: can affect path following */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Link, Meta = (AllowPrivateAccess = "true"))
	TArray<UNavLinkCustomComponent*> SmartLinkComp;

	UPROPERTY(VisibleAnywhere, Category = Link)
	TMap<UNavLinkCustomComponent*, FSensingParkourInfo> LinkList;
public:
	/** Smart link: toggle relevancy */
	UPROPERTY(EditAnywhere, Category = Link)
	bool bSmartLinkIsRelevant;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* LinkPointArea;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FSensingParkourInfo CurrentParkourInfo;

	UPROPERTY(EditAnywhere, Category = ParkourSettings)
	float NodeDistance;
	UPROPERTY(EditAnywhere, Category = ParkourSettings)
	float ForwardWallLength;

private:
	// Add Nav Link Component
	void AddNavLinkComponent(FSensingParkourInfo SensingParkourInfo, FTransform StartTransform, ENavLinkDirection::Type Direct = ENavLinkDirection::BothWays);

	bool CheckAndSpawnParkourNode(FVector Location, FVector Forward);
	void AddParkourLinkNode(FSensingParkourInfo SensingParkourInfo, FVector StartLocation, FVector EndLocation, ENavLinkDirection::Type Direct);
	void DeleteParkourLinkIndex(int32 index);

	// Check Point is In Box
	bool IsPointInBoxComponent(FVector Location);
	// Start Location Overlap Test
	bool CheckStartLocationOverlap(FVector StartLocation);

	bool GetFloor(FVector Location);
	bool GetFloor(FVector Location, FVector& FloorLocation);

	bool UpdateParkourData(int32 index);
	void SetSmartLinkDirect(ENavLinkDirection::Type NavLinkType);
public:
	/** 생성 가능한 Parkour 노드 모두 생성 */
	UFUNCTION(CallInEditor, Category = SmartLink, meta = (DisplayName = "SpawnParkourNodes (InBoxVolume)"))
	void SpawnParkourNodes();

	/** Call In Editor Detail*/
	// Add Smart Link Point
	UFUNCTION(CallInEditor, Category = SmartLink, meta = (DisplayName = "AddParkourLinkNode"))
	void AddParkourLinkNode();

	/** Update Nav Link Proxy Parkour Node
	 *	Update to LinkList
	 *  Update to PointLinks
	 *  Update to SmartLinks
	 */
	UFUNCTION(CallInEditor, Category = SmartLink)
	void UpdateParkourLinkNodes();

	// Clear Nav Link Components
	UFUNCTION(CallInEditor, Category = SmartLink)
	void ClearNavLinkComponent();

	// Clear Nav Link Components
	UFUNCTION(CallInEditor, Category = "SmartLink : SettingsLink")
	void DeleteInSelectIndex();

	// Settings Select Index Link Direct
	/** Enable Climb Up & Down */
	UFUNCTION(CallInEditor, Category = "SmartLink : SettingsLink")
	void BothWayInSelectIndex();
	/** Enable Climb Up Only */
	UFUNCTION(CallInEditor, Category = "SmartLink : SettingsLink")
	void LeftToRightInSelectIndex();
	/** Enable Climb Down Only */
	UFUNCTION(CallInEditor, Category = "SmartLink : SettingsLink")
	void RightToLeftInSelectIndex();
#endif
};
