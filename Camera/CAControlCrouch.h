class UCAControlCrouch final : public UCAControlTransition
{
public:
    void Configure( UCameraActorControl* InControl ) override;
    void Update( float InDeltaTime ) override;

protected:
	void ModifyTransition() override;

private:
    void SetupEventHandler();
    void SetupDifferenceHeight();

private:
	bool bCrouch;
    float DifferenceHeight;

};
