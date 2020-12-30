# EVRStudioPortfolio
EVR Studio 내에서 작업한 소스 코드


# Plugin 관련 업무
[Blueprint 전용 플러그인, C++로 변경 및 구조화]
- Handrail : Editor Static Mesh Spawner

# Physics 관련
[캐릭터 차량 물리 구현]
- 언차티드4 참조
- Physics Animation Profiles & Animation Physical Blend Weight를 사용한 물리 흔들림 구현
- 차량 Suspension을 간략히 구현하여, 흔들림에 따름 물리력 생성
- Handle에 대한 Physics Hand 고정
- 운전 유형을 나누어, 유형별 Physics 알고리즘 적용 (평지 / 급 커브&정지 / 방향별 충돌 / 급경사 / 고르지 않은 지형)
- 물리로 구현이 불가능한 부분은, Animation Additive & Blend를 사용하여 보정 (Animation Locomotion)
- [관련 영상]
  - https://youtu.be/R__JStxv-5s
  - https://youtu.be/Dt54orh-YX0
  - https://youtu.be/fHXG4ud6iPs
  - https://youtu.be/Hcqo3jZarkM


# Character 관련
[캐릭터의 커버 및 코너 & 엄폐물 위 사격 구현]
- 고스트 리콘 : 브레이크 포인트 참조
- 커버 & 코너 & 엄폐물 센싱 구현 (Trace, Sweep)
- 기본 커버 상태 구현 (이동, 반전)
- 캐릭터 & 카메라 위치에 따른, 코너 및 엄폐물 위 사격 분리
- 엄폐물 위의 사격 높이 보정
[관련 영상]
- https://youtu.be/MIpFfTEAiKY
- https://youtu.be/8Qhz41Rb5UM
- https://youtu.be/20O_g50Usk0
- https://youtu.be/bqL6sNbfIHo
- https://youtu.be/7-VJ7LjoGwc
- https://youtu.be/8zzZn88Jq_c

[캐릭터 구르기 구현]
- 구르기 낙하 예외 처리
- 구르기 -≫ 이동 고도화
[관련 영상]
- https://youtu.be/D5jshmIh-zA

[Normal ≪-≫ Combat 상태 분리]
- 전투 상태와 비전투 상태를 구분하여 카메라 Modifier 적용
[관련 영상]
- https://youtu.be/Mc--igHfmPc


# AI 관련
[AI Cover 진입 동작 구현]
- 진입별 각도에 따른 Montage Section을 분리하여 구현
[관련 영상]
- https://youtu.be/Nk0ckwc1hQ8
- https://youtu.be/Wu25wnWToAQ

[AI Parkour 구현]
- Nav Link Parkour 구현
- Nav Link Parkour Spawner 기능 구현 (Nav Link 자동 생성 및 수정)
- 장애물에 따른 Parkour Type 분리 적용
[관련 영상]
- https://youtu.be/FcsdxKsp1Ro

[AI 제자리 사격 Task 구현]
- 사격 중, 타겟에 대한 Aim Turn 구현
[관련 영상]
- https://youtu.be/5veJPgHPM4k


# 저격총 구현
[저격총 렌즈 효과 구현 (Material)]
- Custom Stencil & Depth 효과를 이용하여, 렌즈 내부 & 외부 분리
- 렌즈 내부에 보이는 저격총 Mesh 필터 처리
- 렌즈 내부를 확대하여 외부와의 공간감 분리
- 렌즈 내의 굴절 효과 적용
- 렌즈 외각 Shadow 효과 구현
- 렌즈 외각 Sunlinght Effect 효과 구현
- 에임 이동시 따라오는 렌즈 효과 구현
[관련 영상]
- https://youtu.be/R2LqMX7tQ5Q
- https://youtu.be/H4H17bgcVgg
- https://youtu.be/8-XriDULeAo
- https://youtu.be/EXdNIN9Pp6U
- https://youtu.be/8GHxzE-F1V4
- https://youtu.be/_kDKOHQ3mW8
- https://youtu.be/-7Wh2kU9va0

[First -≫ Third Camera Transition 구현]
- 기존 카메라 구조를 변경하여, 3인칭 -≫ 1인칭 카메라 Transition 구현
[관련 영상]
- https://youtu.be/IhY6Qjteuts
- https://youtu.be/_TEbSYSedjE


# 야간 투시경 구현
[야간 투시경 효과 구현 (Material)]
- Base Color와 Light Color를 함께 사용하여, 심하게 어두운 곳도 보이도록 구현
- Noise 효과 구현
- 저격총 렌즈 효과와 함께 적용되도록 Post Process Order 순서 적용 (야간투시경 -≫ 렌즈 굴절 -≫ 렌즈 Shadow)
[관련 영상]
- https://youtu.be/Yw3Q678tGfc
- https://youtu.be/dpqRK0Bw6aQ
