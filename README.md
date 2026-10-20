# dx-tutorial

DirectX 11 / Bullet 기반 1인칭 게임 실습 프로젝트입니다.

Visual Studio 2022에서 `dx-tutorial.sln`의 x64 Debug 또는 Release를 빌드합니다.
FBX 로더는 저장소에 포함되어 있어 Autodesk FBX SDK를 설치할 필요가 없습니다.
실행에 필요한 모델·셰이더·이미지는 빌드 출력 폴더로 자동 복사됩니다.

`origin/feature/model`의 FBX 5종을 현재 오브젝트 구조에 연결했습니다.
적·엘리트는 모델 크기에 맞는 박스 충돌체를 사용하고, 권총·기관총·수류탄은
바닥의 시각용 소품으로 배치됩니다. [에셋 구성과 임포트 범위](dx-tutorial/assets/README.md)를 참고하세요.

임포트 및 실제 D3D11 렌더링 검증:

```powershell
.\tests\ValidateAssets.ps1 -Configuration Debug
.\tests\ValidateAssets.ps1 -Configuration Release
```

검증 결과와 5종 모델의 3방향 렌더링 이미지는 `.model-validation`에 생성됩니다.
