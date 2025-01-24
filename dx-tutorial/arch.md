# **1. 프로젝트 개요**
- **프로젝트 이름:** DX11_FPS_Cube  
- **목표:** WinAPI 기반 윈도우에서 DirectX 11로 3D 큐브를 렌더링하고, FPS 스타일 카메라를 사용해 시점을 WASD(전후좌우 이동) + 마우스(시야 회전)로 조작.

---

# **2. 요구사항 분석**

## **2.1 기능 요구사항 (Functional Requirements)**

1. **FPS 카메라**  
   - **WASD 키**로 전후좌우 이동  
   - **마우스 이동**으로 상하좌우 회전  
   - 상하 회전 각도는 일정 범위(예: -90° ~ +90°)로 제한  
   - 카메라 위치, 시야 방향(Forward, Right), 상단 벡터(Up) 등을 적절히 관리해야 함

2. **3D 큐브 렌더링**  
   - 정육면체(6면, 36개 인덱스)  
   - 간단한 색상(RGB) 또는 단색, 혹은 면마다 다른 색상  
   - 버텍스 버퍼, 인덱스 버퍼, Input Layout, 셰이더(버텍스 셰이더, 픽셀 셰이더)로 구성
   - **View 행렬 & Projection 행렬**을 통해 원근감 있게 보이도록 설정

3. **WinAPI와 DirectX 11**  
   - **WinAPI**  
     - **WNDCLASSEX**를 통해 창을 등록하고, **CreateWindowEx**로 윈도우 생성  
     - **PeekMessage/DispatchMessage** 기반 메시지 루프  
     - **WM_MOUSEMOVE** 등으로 마우스 이벤트를 처리하거나, `GetAsyncKeyState`로 키 입력 처리  
   - **DirectX 11**  
     - **D3D11CreateDeviceAndSwapChain**로 디바이스, 스왑체인 생성  
     - 렌더 타겟 뷰(Render Target View) 획득, 클리어/프레젠트 호출  
     - 버텍스/픽셀 셰이더 작성 및 컴파일 → 파이프라인에 셋업  
     - 3D 매트릭스(월드, 뷰, 프로젝션) 사용

---

## **2.2 비기능 요구사항 (Non-Functional Requirements)**

1. **개발 언어/환경**  
   - **C++17 이상**  
   - **Windows 8/10** (DirectX 11 기본 지원 환경)  
   - **Visual Studio**(또는 동등 IDE) 권장

2. **성능**  
   - 초당 60프레임(또는 최대 모니터 주사율) 정도는 유지 가능해야 함  
   - 작은 데모 수준이므로, 고성능 최적화는 우선순위 낮음

3. **확장성**  
   - 향후 스카이박스나 추가 모델을 로드, 조명 효과를 넣는 등 확장 가능해야 함  
   - 카메라, 렌더러, 입력 처리를 분리해 설계

---

# **3. 시스템 구성도**

```
+---------------------+               +---------------------+
|    WinMain (main)   |               |    Window Class     |
|---------------------|               |---------------------|
| - 실행, 메시지루프   |   create->    | - CreateWindowEx     |
| - game.Init()       |<------------- | - WndProc(Mouse,Key) |
|                     |     notify->  |   -> game.OnInput()  |
+---------+-----------+               +----+-----------------+
          |                              |
          | init                         | call
          v                              v
+---------------------+               +---------------------+
|   Game (Controller) | <-----------> |  FPSCamera Class    |
|---------------------|   uses        +---------------------+
| - game.Update()     |   (input)     | - pos, forward, up  |
| - game.Render()     |               | - MoveForward/Right |
| - has Window,       |               | - Rotate(dx, dy)    |
|   DXRenderer,Cube,Camera            | - GetViewMatrix()   |
+---------+-----------+               +---------------------+
          |  uses
          v
+---------------------+
|  DXRenderer Class   |
|---------------------|
| - D3D11CreateDevice |
| - Clear, Present    |
| - Pipeline Config   |
+---------+-----------+
          |
          v
+---------------------+
|   Cube Class        |
|---------------------|
| - VertexBuffer      |
| - IndexBuffer       |
| - Render() ->       |
|   IASetBuffers,...  |
+---------------------+
```

---

# **4. 상세 설계**

## **4.1 주요 클래스**

### **4.1.1 `Window` 클래스**
- **역할**: WinAPI 창 생성 및 메시지 처리  
- **핵심 메서드**:  
  - `bool Initialize(HINSTANCE, int)` : 윈도우 등록 & 생성  
  - `static LRESULT CALLBACK WndProc(...)` : WinAPI 메시지 루프 (마우스, 키 입력)  
  - `void Cleanup()` : 자원 정리 (UnregisterClass 등)  

### **4.1.2 `DXRenderer` 클래스**
- **역할**: DirectX 11 초기화 & 스왑 체인 관리, 클리어 & 프레젠트  
- **핵심 메서드**:  
  - `bool Initialize(HWND hWnd)`  
  - `void Clear(...)`, `void Present()`  
  - `void Cleanup()`  
  - `ID3D11Device* GetDevice()`, `ID3D11DeviceContext* GetContext()`  

### **4.1.3 `FPSCamera` 클래스**
- **역할**: 카메라 위치/회전(시야) 관리, View 행렬 계산  
- **데이터**:  
  - `XMFLOAT3 mPos`, `XMFLOAT3 mForward`, `XMFLOAT3 mRight`, `XMFLOAT3 mUp`  
  - `float mYaw, mPitch`, `float mMoveSpeed`, `float mMouseSensitivity`  
- **핵심 메서드**:  
  - `MoveForward(float delta)`, `MoveRight(float delta)` → WASD  
  - `Rotate(float dx, float dy)` → 마우스  
  - `XMMATRIX GetViewMatrix()` → 뷰 행렬 리턴  

### **4.1.4 `Cube` 클래스**
- **역할**: 정육면체 버텍스/인덱스 버퍼 생성, 렌더링(셰이더 사용)  
- **데이터**:  
  - `ID3D11Buffer* mVertexBuffer`, `ID3D11Buffer* mIndexBuffer`  
  - (선택) `ID3D11Buffer* mConstantBuffer` → 월드행렬 등  
- **핵심 메서드**:  
  - `bool Initialize(ID3D11Device* device)` → 버퍼 생성  
  - `void Render(ID3D11DeviceContext* context)` → IASetVertexBuffers, DrawIndexed(36)  
  - `void Cleanup()`  

### **4.1.5 (선택) `Game`(또는 `Application`) 클래스**
- **역할**: 전역적인 흐름 제어 (윈도우, 렌더러, 카메라, 큐브를 포함)  
- **핵심 메서드**:  
  - `bool Initialize(HINSTANCE, int)` : Window 초기화 → DXRenderer 초기화 → Cube/Cam 초기화  
  - `void Update()` : 입력 처리(카메라 이동)  
  - `void Render()` : 렌더러 Clear, Cube.Render() 후 Present  
  - `void Cleanup()`  

---

## **4.2 입력 처리 (카메라 이동)**
1. **키 입력**  
   - `W` → `camera.MoveForward(+1)`  
   - `S` → `camera.MoveForward(-1)`  
   - `A` → `camera.MoveRight(-1)`  
   - `D` → `camera.MoveRight(+1)`  
   - 구현 예) `GetAsyncKeyState() & 0x8000` 혹은 WM_KEYDOWN, WM_KEYUP  
2. **마우스 입력**  
   - `WM_MOUSEMOVE` 메시지에서 `x = LOWORD(lParam)`, `y = HIWORD(lParam)`  
   - 이전 프레임과의 차이(dx, dy)를 구해 `camera.Rotate(dx, dy)`  

---

## **4.3 그래픽 파이프라인 설정**
1. **버텍스 셰이더(HLSL)**  
   - 정점 좌표에 **(월드 × 뷰 × 프로젝션)** 매트릭스 적용  
   - 간단한 색상 전달  
2. **픽셀 셰이더(HLSL)**  
   - 입력된 색상을 그대로 출력 (또는 간단한 조명)  
3. **Input Layout**  
   - POSITION (float3), COLOR (float4) 구조 정의  
4. **Cube::Render**:  
   - `IASetVertexBuffers`, `IASetIndexBuffer`  
   - `context->DrawIndexed(36, 0, 0)`  

---

## **4.4 카메라 행렬 적용**
- **FPSCamera**  
  - `GetViewMatrix()` → `XMMatrixLookToLH(mPos, mForward, mUp)`  
  - 별도 `GetProjectionMatrix(float aspect)` → `XMMatrixPerspectiveFovLH(...)` 호출 가능  
- **셰이더 상수 버퍼**  
  - 상수 버퍼 구조체에 `view`, `projection` 행렬 (또는 `(world * view * projection)`)  
  - GPU로 업데이트 (`UpdateSubresource`, `VSSetConstantBuffers`)  

---

# **5. 간단한 시나리오 예시**

1. **프로그램 실행**  
   - `WinMain`에서 **Game** 객체 생성, `game.Initialize(...)` 호출  
2. **Initialize**  
   1) `Window.Initialize(...)` → WinAPI 창 생성  
   2) `DXRenderer.Initialize(hwnd)` → DirectX 11 장치, 스왑체인, 렌더타겟 준비  
   3) `Cube.Initialize(device)` → 큐브 버텍스/인덱스 버퍼 생성  
   4) `camera`(FPSCamera) 초기위치 (0,1.6,-5.0) 등 설정  
3. **메인 루프**  
   - WinAPI 메시지(`PeekMessage`) 처리  
   - 메시지가 없으면 → `game.Update()`, `game.Render()`  
4. **Update()**  
   - **키 입력**(WASD) 처리 → `camera.MoveForward()`, `camera.MoveRight()`  
   - **마우스 이동** → `camera.Rotate(dx, dy)`  
5. **Render()**  
   - `DXRenderer.Clear(...)`  
   - 카메라의 `view`, `projection` 얻어 셰이더로 전달  
   - `cube.Render(context)`로 정육면체 그리기  
   - `DXRenderer.Present()`  
6. **종료**  
   - `WM_QUIT` 메시지 → 루프 탈출  
   - `game.Cleanup()` → 모든 리소스 해제 (버퍼, 디바이스, 창 등)

---

# **6. 결론 및 요약**

- **목적:** WinAPI + DirectX11 환경에서 **FPS 카메라**로 3D 공간을 탐색하며, **정육면체**가 그려지는 장면을 구현한다.  
- **핵심:**  
  1) **WinAPI**로 윈도우 생성 & 메시지 처리  
  2) **DirectX 11** 파이프라인 초기화, 버텍스·인덱스 버퍼 및 셰이더 구성  
  3) **FPSCamera**를 통해 WASD+마우스로 이동/회전, 뷰 행렬 생성  
  4) **Cube**를 DrawIndexed로 렌더  
  5) **메인 루프**에서 Update(입력 & 카메라 이동) → Render(씬 그리기) 반복

이상으로, **FPS 카메라 + 3D 큐브 렌더링**을 위한 **요구사항/설계 문서**를 간단히 정리했습니다.  
이 문서를 기반으로 각 클래스를 구현하면, **WinAPI에서 메시지 루프**를 처리하고, **DirectX 11로 3D 장면**을 제어해 목표 기능을 달성할 수 있습니다.