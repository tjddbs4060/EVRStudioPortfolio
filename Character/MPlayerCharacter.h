 /**
 * MPlayerCharacter
 * 정의 : 유저가 플레이 가능한 캐릭터
 * 하는 일 :
*		1. 플레이어 관련된 처리
 * 추가 컴퍼넌트 :
 */
class AMPlayerCharacter : public AMainCharacter,public IPlayerAxisInputInterface
{
#pragma region ParkourAction Input
public:
	void Input_ParkourAction();
protected:
	float ParkourActionInputTime;
#pragma endregion
};
