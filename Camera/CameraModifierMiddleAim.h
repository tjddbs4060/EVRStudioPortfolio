class UCameraModifierMiddleAim : public UCameraModifierBase
{
public:
	bool ModifyCamera(float InDeltaTime, struct FMinimalViewInfo& InOutPOV) override;

private:
	FString CurrentKey;

	float CameraArmOffset;
	FVector CameraSocketOffset;

	float PrevMiddleAimAlpha;

	class AGamePlayerCameraManager* PlayerCameraManager;
};
