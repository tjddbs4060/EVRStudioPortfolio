class UMAIBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	/** 커버 가능한 위치인지 판단 */
	static void CheckCoverLocation(FVector CoverLoc, FVector TargetLoc, bool bShootCover, AMCharacter* OwnerCharacter, AActor* TargetCharacter, bool& bEnableCover, bool& bStandCover, FVector& ExactLocation);

	/** AI 전투영역 셋팅. (이전 전투영역 덮어씌움.) */
	static void SetCombatZone(UObject* WorldContextObject, FName WorldName, TArray<AMainCharacter*> Characters);
	/** AI 전투영역 추가. */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void AddCombatZone(UObject* WorldContextObject, FName WorldName, TArray<AMainCharacter*> Characters);
	/** AI 전투영역 삭제. */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void ClearCombatZone(UObject* WorldContextObject, TArray<AMainCharacter*> Characters);

#pragma region AISensing
	/** 
	* return AI Parkour Avaliable
	* return OutparkouInfo : Parkour Info Data
	*/
	UFUNCTION(BlueprintCallable, Category = "AI|AISensing", meta = (WorldContext = "WorldContextObject"))
	static bool ParkourSensing(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect);
	UFUNCTION(BlueprintCallable, Category = "AI|AISensing", meta = (WorldContext = "WorldContextObject"))
	static bool ParkourUpSensing(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect);
	UFUNCTION(BlueprintCallable, Category = "AI|AISensing", meta = (WorldContext = "WorldContextObject"))
	static bool ParkourDownSensing(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect);

	static bool CheckParkourUpType(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect);
	static bool CheckParkourUpTrace(UObject* WorldContextObject, FHitResult& OutHit, FVector Start, FVector End, FVector Forward);
	static bool CheckParkourUpWarpTrace(UObject* WorldContextObject, FTransform CharacterParkourTransform, FTransform& StartWarpTransform, FTransform& EndWarpTransform);

	static bool CheckParkourDownType(UObject* WorldContextObject, FSensingParkourInfo& OutParkourInfo, FVector ParkourStartLocation, FVector ForwardDirect);
	static bool CheckParkourDownTrace(UObject* WorldContextObject, FHitResult& OutHit, FVector Start, FVector End, FVector Forward);
	static bool CheckParkourDownWarpTrace(UObject* WorldContextObject, FTransform CharacterParkourTransform, FTransform& StartWarpTransform, FTransform& EndWarpTransform);
#pragma endregion AISensing
};
