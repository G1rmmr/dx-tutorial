# **1. ������Ʈ ����**
- **������Ʈ �̸�:** DX11_FPS_Cube  
- **��ǥ:** WinAPI ��� �����쿡�� DirectX 11�� 3D ť�긦 �������ϰ�, FPS ��Ÿ�� ī�޶� ����� ������ WASD(�����¿� �̵�) + ���콺(�þ� ȸ��)�� ����.

---

# **2. �䱸���� �м�**

## **2.1 ��� �䱸���� (Functional Requirements)**

1. **FPS ī�޶�**  
   - **WASD Ű**�� �����¿� �̵�  
   - **���콺 �̵�**���� �����¿� ȸ��  
   - ���� ȸ�� ������ ���� ����(��: -90�� ~ +90��)�� ����  
   - ī�޶� ��ġ, �þ� ����(Forward, Right), ��� ����(Up) ���� ������ �����ؾ� ��

2. **3D ť�� ������**  
   - ������ü(6��, 36�� �ε���)  
   - ������ ����(RGB) �Ǵ� �ܻ�, Ȥ�� �鸶�� �ٸ� ����  
   - ���ؽ� ����, �ε��� ����, Input Layout, ���̴�(���ؽ� ���̴�, �ȼ� ���̴�)�� ����
   - **View ��� & Projection ���**�� ���� ���ٰ� �ְ� ���̵��� ����

3. **WinAPI�� DirectX 11**  
   - **WinAPI**  
     - **WNDCLASSEX**�� ���� â�� ����ϰ�, **CreateWindowEx**�� ������ ����  
     - **PeekMessage/DispatchMessage** ��� �޽��� ����  
     - **WM_MOUSEMOVE** ������ ���콺 �̺�Ʈ�� ó���ϰų�, `GetAsyncKeyState`�� Ű �Է� ó��  
   - **DirectX 11**  
     - **D3D11CreateDeviceAndSwapChain**�� ����̽�, ����ü�� ����  
     - ���� Ÿ�� ��(Render Target View) ȹ��, Ŭ����/������Ʈ ȣ��  
     - ���ؽ�/�ȼ� ���̴� �ۼ� �� ������ �� ���������ο� �¾�  
     - 3D ��Ʈ����(����, ��, ��������) ���

---

## **2.2 ���� �䱸���� (Non-Functional Requirements)**

1. **���� ���/ȯ��**  
   - **C++17 �̻�**  
   - **Windows 8/10** (DirectX 11 �⺻ ���� ȯ��)  
   - **Visual Studio**(�Ǵ� ���� IDE) ����

2. **����**  
   - �ʴ� 60������(�Ǵ� �ִ� ����� �ֻ���) ������ ���� �����ؾ� ��  
   - ���� ���� �����̹Ƿ�, ���� ����ȭ�� �켱���� ����

3. **Ȯ�强**  
   - ���� ��ī�̹ڽ��� �߰� ���� �ε�, ���� ȿ���� �ִ� �� Ȯ�� �����ؾ� ��  
   - ī�޶�, ������, �Է� ó���� �и��� ����

---

# **3. �ý��� ������**

```
+---------------------+               +---------------------+
|    WinMain (main)   |               |    Window Class     |
|---------------------|               |---------------------|
| - ����, �޽�������   |   create->    | - CreateWindowEx     |
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

# **4. �� ����**

## **4.1 �ֿ� Ŭ����**

### **4.1.1 `Window` Ŭ����**
- **����**: WinAPI â ���� �� �޽��� ó��  
- **�ٽ� �޼���**:  
  - `bool Initialize(HINSTANCE, int)` : ������ ��� & ����  
  - `static LRESULT CALLBACK WndProc(...)` : WinAPI �޽��� ���� (���콺, Ű �Է�)  
  - `void Cleanup()` : �ڿ� ���� (UnregisterClass ��)  

### **4.1.2 `DXRenderer` Ŭ����**
- **����**: DirectX 11 �ʱ�ȭ & ���� ü�� ����, Ŭ���� & ������Ʈ  
- **�ٽ� �޼���**:  
  - `bool Initialize(HWND hWnd)`  
  - `void Clear(...)`, `void Present()`  
  - `void Cleanup()`  
  - `ID3D11Device* GetDevice()`, `ID3D11DeviceContext* GetContext()`  

### **4.1.3 `FPSCamera` Ŭ����**
- **����**: ī�޶� ��ġ/ȸ��(�þ�) ����, View ��� ���  
- **������**:  
  - `XMFLOAT3 mPos`, `XMFLOAT3 mForward`, `XMFLOAT3 mRight`, `XMFLOAT3 mUp`  
  - `float mYaw, mPitch`, `float mMoveSpeed`, `float mMouseSensitivity`  
- **�ٽ� �޼���**:  
  - `MoveForward(float delta)`, `MoveRight(float delta)` �� WASD  
  - `Rotate(float dx, float dy)` �� ���콺  
  - `XMMATRIX GetViewMatrix()` �� �� ��� ����  

### **4.1.4 `Cube` Ŭ����**
- **����**: ������ü ���ؽ�/�ε��� ���� ����, ������(���̴� ���)  
- **������**:  
  - `ID3D11Buffer* mVertexBuffer`, `ID3D11Buffer* mIndexBuffer`  
  - (����) `ID3D11Buffer* mConstantBuffer` �� ������� ��  
- **�ٽ� �޼���**:  
  - `bool Initialize(ID3D11Device* device)` �� ���� ����  
  - `void Render(ID3D11DeviceContext* context)` �� IASetVertexBuffers, DrawIndexed(36)  
  - `void Cleanup()`  

### **4.1.5 (����) `Game`(�Ǵ� `Application`) Ŭ����**
- **����**: �������� �帧 ���� (������, ������, ī�޶�, ť�긦 ����)  
- **�ٽ� �޼���**:  
  - `bool Initialize(HINSTANCE, int)` : Window �ʱ�ȭ �� DXRenderer �ʱ�ȭ �� Cube/Cam �ʱ�ȭ  
  - `void Update()` : �Է� ó��(ī�޶� �̵�)  
  - `void Render()` : ������ Clear, Cube.Render() �� Present  
  - `void Cleanup()`  

---

## **4.2 �Է� ó�� (ī�޶� �̵�)**
1. **Ű �Է�**  
   - `W` �� `camera.MoveForward(+1)`  
   - `S` �� `camera.MoveForward(-1)`  
   - `A` �� `camera.MoveRight(-1)`  
   - `D` �� `camera.MoveRight(+1)`  
   - ���� ��) `GetAsyncKeyState() & 0x8000` Ȥ�� WM_KEYDOWN, WM_KEYUP  
2. **���콺 �Է�**  
   - `WM_MOUSEMOVE` �޽������� `x = LOWORD(lParam)`, `y = HIWORD(lParam)`  
   - ���� �����Ӱ��� ����(dx, dy)�� ���� `camera.Rotate(dx, dy)`  

---

## **4.3 �׷��� ���������� ����**
1. **���ؽ� ���̴�(HLSL)**  
   - ���� ��ǥ�� **(���� �� �� �� ��������)** ��Ʈ���� ����  
   - ������ ���� ����  
2. **�ȼ� ���̴�(HLSL)**  
   - �Էµ� ������ �״�� ��� (�Ǵ� ������ ����)  
3. **Input Layout**  
   - POSITION (float3), COLOR (float4) ���� ����  
4. **Cube::Render**:  
   - `IASetVertexBuffers`, `IASetIndexBuffer`  
   - `context->DrawIndexed(36, 0, 0)`  

---

## **4.4 ī�޶� ��� ����**
- **FPSCamera**  
  - `GetViewMatrix()` �� `XMMatrixLookToLH(mPos, mForward, mUp)`  
  - ���� `GetProjectionMatrix(float aspect)` �� `XMMatrixPerspectiveFovLH(...)` ȣ�� ����  
- **���̴� ��� ����**  
  - ��� ���� ����ü�� `view`, `projection` ��� (�Ǵ� `(world * view * projection)`)  
  - GPU�� ������Ʈ (`UpdateSubresource`, `VSSetConstantBuffers`)  

---

# **5. ������ �ó����� ����**

1. **���α׷� ����**  
   - `WinMain`���� **Game** ��ü ����, `game.Initialize(...)` ȣ��  
2. **Initialize**  
   1) `Window.Initialize(...)` �� WinAPI â ����  
   2) `DXRenderer.Initialize(hwnd)` �� DirectX 11 ��ġ, ����ü��, ����Ÿ�� �غ�  
   3) `Cube.Initialize(device)` �� ť�� ���ؽ�/�ε��� ���� ����  
   4) `camera`(FPSCamera) �ʱ���ġ (0,1.6,-5.0) �� ����  
3. **���� ����**  
   - WinAPI �޽���(`PeekMessage`) ó��  
   - �޽����� ������ �� `game.Update()`, `game.Render()`  
4. **Update()**  
   - **Ű �Է�**(WASD) ó�� �� `camera.MoveForward()`, `camera.MoveRight()`  
   - **���콺 �̵�** �� `camera.Rotate(dx, dy)`  
5. **Render()**  
   - `DXRenderer.Clear(...)`  
   - ī�޶��� `view`, `projection` ��� ���̴��� ����  
   - `cube.Render(context)`�� ������ü �׸���  
   - `DXRenderer.Present()`  
6. **����**  
   - `WM_QUIT` �޽��� �� ���� Ż��  
   - `game.Cleanup()` �� ��� ���ҽ� ���� (����, ����̽�, â ��)

---

# **6. ��� �� ���**

- **����:** WinAPI + DirectX11 ȯ�濡�� **FPS ī�޶�**�� 3D ������ Ž���ϸ�, **������ü**�� �׷����� ����� �����Ѵ�.  
- **�ٽ�:**  
  1) **WinAPI**�� ������ ���� & �޽��� ó��  
  2) **DirectX 11** ���������� �ʱ�ȭ, ���ؽ����ε��� ���� �� ���̴� ����  
  3) **FPSCamera**�� ���� WASD+���콺�� �̵�/ȸ��, �� ��� ����  
  4) **Cube**�� DrawIndexed�� ����  
  5) **���� ����**���� Update(�Է� & ī�޶� �̵�) �� Render(�� �׸���) �ݺ�

�̻�����, **FPS ī�޶� + 3D ť�� ������**�� ���� **�䱸����/���� ����**�� ������ �����߽��ϴ�.  
�� ������ ������� �� Ŭ������ �����ϸ�, **WinAPI���� �޽��� ����**�� ó���ϰ�, **DirectX 11�� 3D ���**�� ������ ��ǥ ����� �޼��� �� �ֽ��ϴ�.