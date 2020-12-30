#pragma region ParkourAction Input
void AMPlayerCharacter::Input_ParkourAction()
{
	if (IsDyingOrDeadOrDeactive())
		return;

	if (MPlayerController && MPlayerController->IsDisableInputMode())
		return;

	if (UMActionBlueprintFunctionLibrary::IsPlayingActionByName(this, ActionTagNames::THROW))
		return;

	// 낙하 도중 파쿠르 액션을 하지 않음
	if (GetMovementType() == EMovementType::Falling)
		return;

	if (CloseCombatComponent)
	{
		if (CloseCombatComponent->IsFindFatalityTarget())
		{
			if (CloseCombatComponent->DoFatalityKill())
				return;
		}

		// 회피를 해야하는지 체크.
		if (CloseCombatComponent->IsIncomingAttack())
		{
			CloseCombatComponent->TryDodge();
			return;
		}
	}

	if (ParkourComponent == nullptr || ParkourComponent->DoParkour() == false)
	{
		// 파쿠르가 아니면 구르기 실행
		const float CurGameTime = GetGameTimeSinceCreation();

		if ((CurGameTime - ParkourActionInputTime) < 0.2f)
		{
			const static FOnActionFinishedDelegate OnFinished;
			UMActionBlueprintFunctionLibrary::PlayAction_RollingDownAction(this, OnFinished);
		}
		else
			ParkourActionInputTime = GetGameTimeSinceCreation();
	}
}
#pragma endregion