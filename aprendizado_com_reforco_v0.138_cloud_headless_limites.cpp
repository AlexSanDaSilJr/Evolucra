/*
===============================================================================
 VIDA ARTIFICIAL EVOLUTIVA — APRENDIZADO COM REFORCO v0.138
 ARQUIVO PRINCIPAL / CONTRATO DE MANUTENCAO PARA IA
===============================================================================

LEIA ESTE BLOCO ANTES DE ALTERAR O CODIGO.
O arquivo continua single-file de proposito, mas esta dividido por marcadores [SEC-*].
O documento complementar CONTEXTO_IA_VIDA_ARTIFICIAL_v0.138.md descreve as regras
com mais detalhes. Se houver conflito, a ordem de autoridade e:

  1. Regra explicitamente pedida pelo usuario mais recentemente.
  2. Este CONTRATO DE MANUTENCAO.
  3. Implementacao atual do codigo.
  4. Comentarios/changelogs historicos antigos.

REGRAS-NUCLEO QUE NAO DEVEM SER ALTERADAS ACIDENTALMENTE
----------------------------------------------------------
[R-PLANTA-01] Planta possui raiz biologica fixa em Plant::pos.
[R-PLANTA-02] visualOffset move apenas a copa/sprite. Nao transforma a planta em corpo solto.
[R-PLANTA-03] Planta parcialmente comida nao volta a crescer, mas pode continuar reproduzindo
               se ainda possuir biomassa reprodutivamente viavel.
[R-PLANTA-04] Tamanho/biomassa de planta comida mudam na mordida, nao por encolhimento temporal.

[R-REPRO-01] Existem duas rotas de reproducao de bicho: ovo SOLO e ovo ACASALADO.
[R-REPRO-02] Ovo solo: custo biologico integral; 10% falha e vira carne; 90% eclode.
[R-REPRO-03] Ovo acasalado: dois bichos aptos + intencao de acasalar + cabeca com cabeca.
[R-REPRO-04] Ovo acasalado: 0% de falha biologica, incubacao pela metade, custo principal 70% menor.
[R-REPRO-05] Ovo acasalado: 25% de chance-base de ninhada multipla. Qualidade maior pode permitir ate 10 filhotes.
[R-REPRO-06] Qualidade >=90% e boa; 100% e excelente. No fork RL isso gera recompensa.
[R-REPRO-07] Recompensa extrema de TER FILHO ocorre na ECLOSAO real, nao apenas na postura.
[R-REPRO-08] MORRER e a punicao extrema padrao.

[R-NEURAL-01] O sinal de acasalamento possui alias central MATE_INTENT_OUTPUT_INDEX.
[R-INHERIT-01] Filhos de acasalamento herdam conhecimento neural + memoria persistente + memoria episodica consolidada dos dois pais.
               O indice e historico por compatibilidade de save, mas hoje representa apenas ACASALAR.
               NAO criar/remover saidas neurais sem migrar arrays, biases, estados e saves.
[R-NEURAL-02] Aprendizado plastico durante a vida e herdado de forma lamarckiana nas
               conexoes estruturalmente correspondentes. Eligibility/reward recente nao e herdado.
[R-NEURAL-03] Em acasalamento, memoria episodica consolidada dos dois pais e herdada; estados corporais/reward momentaneo nao sao conhecimento herdavel.
[R-NEURAL-04] Raycasts sao a verdade de visibilidade, mas o cerebro tambem recebe percepcao continua derivada SOMENTE de objetos realmente vistos: presenca, direcao relativa e proximidade.
[R-NEURAL-05] Fundador nasce com cerebro ancestral minimo: planta presente->frente, direcao planta->giro, proximidade->desaceleracao e contato da boca->morder. Sem neuronio oculto obrigatorio.
[R-NEURAL-06] Inputs e outputs sao genes de expressao: podem existir ativos ou dormentes; um gene novo pode nascer sem nenhuma ligacao. Neuronios ocultos tambem podem nascer isolados.
[R-NEURAL-07] Topologia e esparsa. Mais fan-out torna novas ligacoes menos provaveis. Ligacoes para frente sao mais faceis que laterais/retroativas/feedback de output.
[R-NEURAL-08] Areas neurais continuam existindo. Conectar elementos da mesma area e mais provavel que cruzar areas especializadas; isso e vies, nao barreira rigida.
[R-NEURAL-09] Cada ligacao possui funcao de transferencia evolutiva (linear, inversa, absoluta, quadratica, seno, limiar, gaussiana ou saturacao), alem de peso/modo/plasticidade.
[R-NEURAL-10] Complexidade neural tem custo metabolico: neuronios, conexoes, recorrencia, memoria persistente/temporal e plasticidade nao sao gratuitos.
[R-NEURAL-11] Emocoes permanecem no modelo atual e continuam sendo parte do Cerebro; a reforma esparsa nao deve hardcodar novos comportamentos emocionais.

[R-SAVE-01] Nunca inserir/remover campos de structs gravadas com Writer::pod sem migracao por versao.
[R-SAVE-02] Campos novos de entidade devem ser anexados e lidos com gate PROGRAM_VERSION.
[R-SAVE-03] Antes de publicar uma versao: testar save->load e, quando possivel, carregar save anterior.
[R-SAVE-04] PROGRAM_VERSION atual = 137. FORMAT_VERSION continua 3.

[R-UI-01] Mouse wheel consumido por HUD/painel nao pode alterar zoom do mapa no mesmo frame.
[R-UI-02] Topbar/HUD devem respeitar resolucao/fullscreen e nao sobrepor controles.
[R-UI-03] "Visao do bicho" e uma reconstrucao de RayReading: fundo preto e apenas
               informacao efetivamente percebida. Nao revelar objetos fora dos raios.
[R-UI-04] Emocoes pertencem ao laboratorio Cerebro como aba. Nao criar modal paralelo.
[R-UI-05] Tela Memorias e observacional: nao deve escrever registradores nem alterar DNA.
[R-UI-06] Grafico "Ovos atuais" mostra CONTAGEM PRESENTE no mundo, separando SOLO/natural de ACASALAMENTO;
               nao usar total historico/cumulativo. Ovos de reposicao minima nao entram nessas duas series.

[R-WORLD-01] WORLD_W/WORLD_H definem um MUNDO TOROIDAL: esquerda conecta com direita e cima conecta com baixo.
[R-WORLD-02] Ao cruzar uma borda, entidades reaparecem imediatamente na borda oposta; nao existe parede.
[R-WORLD-03] Sensores e interacoes locais devem respeitar a topologia toroidal: raios, proximidade, mordida, fisica e acasalamento enxergam atraves das bordas.
[R-REPRO-09] Ovo tem 50% do antigo raio fisico/visual.
[R-REPRO-10] Ovo por acasalamento pode planejar de 1 a 10 filhotes; qualidade maior abre ninhadas maiores.
[R-REPRO-11] Acasalamento tem prioridade sobre ovo solo. Ovo solo e rota de fallback, especialmente rara em linhagens com heranca sexual.
[R-REPRO-12] Heranca sexual persiste mesmo se uma geracao usar ovo solo; ela nao deve ser apagada pela ausencia de segundo genitor naquele nascimento.
[R-UI-07] Botao Visao abre retina 1D central: uma faixa vertical por raio, tipo acima e intensidade de cor atenuada pela distancia.

[R-PERF-01] Nao trocar sensores otimizados por loops O(N^2) sem medir profiler.
[R-PERF-02] Visao GPU D3D11 implementa o toro com DDA periodico e e o caminho principal. CPU existe somente como fallback/validacao; qualquer alteracao deve manter equivalencia sensorial.
[R-PERF-03] Plantas enraizadas nao devem voltar ao solver fisico planta-planta.
[R-PERF-04] Nao existe mecanica ativa de comunicacao/percepcao por ondas: sem sensores, eventos, campos, custos, genes evolutivos, visualizacao ou profiler associados. Slots binarios antigos existem apenas para compatibilidade e ficam zerados/inertes.
[R-PERF-05] Grades espaciais do mundo finito/toroidal usam tabela densa de celulas + lista compacta de celulas ativas. Nao reintroduzir hash por GridKey no caminho quente sem benchmark.
[R-PERF-06] Consultas circulares toroidais visitam cada celula embrulhada no maximo uma vez; nao alocar unordered_set por consulta.
[R-PERF-07] IDs de criatura usam lookup vetorial direto. Pools preservam ponteiros estaveis e as listas ativas sao periodicamente reordenadas por localidade fisica dos slots para reduzir pointer chasing sem invalidar referencias.

ASSETS EXTERNOS
---------------
  planta : nomes historicos/Planta.png/planta.png
  carne  : nomes historicos/Carne.png/carne.png
  bicho  : nomes historicos/Bicho.png/bicho.png
  ovo    : EXATAMENTE "ovo.png"
Se faltar asset, existe fallback geometrico.

CHECKLIST MINIMO PARA TODA ALTERACAO
------------------------------------
  [ ] Regra nova foi documentada neste contrato ou no documento de contexto?
  [ ] Save/load continua compativel ou possui migracao explicita?
  [ ] CPU e GPU continuam representando a mesma regra sensorial?
  [ ] HUD nao consome o mesmo input do mapa?
  [ ] Nao foi reintroduzido O(N^2) em sensores/fisica?
  [ ] g++ -std=c++17 -Wall -Wextra -Wpedantic -Wreturn-type -fsyntax-only passou?

MARCADORES DE NAVEGACAO
-----------------------
  [SEC-PLATFORM]       Win32 / D3D11 / shim grafico
  [SEC-CONFIG]         constantes e Tuning configuravel
  [SEC-GENETICS]       genes, cerebro, mutacao e recombinacao
  [SEC-ENTITIES]       Plant, Carcass, Egg, Creature
  [SEC-STATS]          estatisticas, recordes, genealogia
  [SEC-SAVE]           serializacao/migracao
  [SEC-SIMULATION]     Simulation e sistemas do mundo
  [SEC-REPRODUCTION]   reproducao vegetal e animal
  [SEC-REINFORCEMENT]  rewards/recordes
  [SEC-RENDER]         sprites/mundo
  [SEC-UI]             HUD, modais, graficos
  [SEC-MAIN]           loop principal
===============================================================================
*/

// [SEC-PLATFORM] Win32 / Direct3D 11 / compatibilidade grafica
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef DrawText
#undef min
#undef max
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <psapi.h>
#include <objbase.h>
#include <wincodec.h>

#include <array>
#include <chrono>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <type_traits>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <filesystem>

struct Vector2 { float x = 0.0f; float y = 0.0f; };
struct Color { unsigned char r=0,g=0,b=0,a=255; };
struct RectF { float x=0,y=0,width=0,height=0; };
struct Camera2D {
    Vector2 offset{};
    Vector2 target{};
    float rotation = 0.0f;
    float zoom = 1.0f;
};

constexpr float PI = 3.14159265358979323846f;
constexpr float DEG2RAD = PI / 180.0f;
constexpr int FLAG_WINDOW_RESIZABLE = 1;
constexpr int FLAG_VSYNC_HINT = 2;
constexpr int FLAG_MSAA_4X_HINT = 4;

// Textura simples do shim. O handle interno e um ID3D11ShaderResourceView* no Windows.
// UVs sao recortados automaticamente para a caixa alpha util do PNG, evitando margens transparentes enormes.
struct Texture2D {
    void* handle = nullptr;
    int width = 0, height = 0;
    float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;
};

constexpr int KEY_R = 'R';
constexpr int KEY_H = 'H';
constexpr int KEY_X = 'X';
constexpr int KEY_M = 'M';
constexpr int KEY_P = 'P';
constexpr int KEY_ONE = '1';
constexpr int KEY_TWO = '2';
constexpr int KEY_FIVE = '5';
constexpr int KEY_ZERO = '0';
constexpr int KEY_SPACE = VK_SPACE;
constexpr int KEY_BACKSPACE = VK_BACK;
constexpr int KEY_ENTER = VK_RETURN;
constexpr int KEY_F5 = VK_F5;
constexpr int KEY_F9 = VK_F9;
constexpr int KEY_F11 = VK_F11;
constexpr int KEY_UP = VK_UP;
constexpr int KEY_DOWN = VK_DOWN;
constexpr int KEY_LEFT = VK_LEFT;
constexpr int KEY_RIGHT = VK_RIGHT;
constexpr int KEY_F = 'F';

constexpr int MOUSE_BUTTON_LEFT = 0;
constexpr int MOUSE_BUTTON_RIGHT = 1;

constexpr Color WHITE{255,255,255,255};
constexpr Color RAYWHITE{245,245,245,255};
constexpr Color RED{230,41,55,255};
constexpr Color YELLOW{253,249,0,255};
constexpr Color GRAY{130,130,130,255};
constexpr Color SKYBLUE{102,191,255,255};
constexpr Color ORANGE{255,161,0,255};

namespace winshim {

// ------------------------- Win32 / input -------------------------
using RegisterClassExAFn = ATOM (WINAPI*)(const WNDCLASSEXA*);
using CreateWindowExAFn = HWND (WINAPI*)(DWORD,LPCSTR,LPCSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID);
using ShowWindowFn = BOOL (WINAPI*)(HWND,int);
using UpdateWindowFn = BOOL (WINAPI*)(HWND);
using PeekMessageAFn = BOOL (WINAPI*)(LPMSG,HWND,UINT,UINT,UINT);
using TranslateMessageFn = BOOL (WINAPI*)(const MSG*);
using DispatchMessageAFn = LRESULT (WINAPI*)(const MSG*);
using DefWindowProcAFn = LRESULT (WINAPI*)(HWND,UINT,WPARAM,LPARAM);
using PostQuitMessageFn = VOID (WINAPI*)(int);
using LoadCursorAFn = HCURSOR (WINAPI*)(HINSTANCE,LPCSTR);
using AdjustWindowRectExFn = BOOL (WINAPI*)(LPRECT,DWORD,BOOL,DWORD);
using SetCaptureFn = HWND (WINAPI*)(HWND);
using ReleaseCaptureFn = BOOL (WINAPI*)();
using GetWindowRectFn = BOOL (WINAPI*)(HWND,LPRECT);
using GetWindowLongPtrAFn = LONG_PTR (WINAPI*)(HWND,int);
using SetWindowLongPtrAFn = LONG_PTR (WINAPI*)(HWND,int,LONG_PTR);
using SetWindowPosFn = BOOL (WINAPI*)(HWND,HWND,int,int,int,int,UINT);
using MonitorFromWindowFn = HMONITOR (WINAPI*)(HWND,DWORD);
using GetMonitorInfoAFn = BOOL (WINAPI*)(HMONITOR,LPMONITORINFO);
using CoInitializeExFn = HRESULT (WINAPI*)(LPVOID,DWORD);
using CoCreateInstanceFn = HRESULT (WINAPI*)(REFCLSID,LPUNKNOWN,DWORD,REFIID,LPVOID*);
using CoUninitializeFn = void (WINAPI*)();

// GDI usado apenas para fonte/texto e medicao. A geometria não usa GDI.
using CreateCompatibleDCFn = HDC (WINAPI*)(HDC);
using SelectObjectFn = HGDIOBJ (WINAPI*)(HDC,HGDIOBJ);
using DeleteObjectFn = BOOL (WINAPI*)(HGDIOBJ);
using DeleteDCFn = BOOL (WINAPI*)(HDC);
using SetBkModeFn = int (WINAPI*)(HDC,int);
using SetTextColorFn = COLORREF (WINAPI*)(HDC,COLORREF);
using TextOutAFn = BOOL (WINAPI*)(HDC,int,int,LPCSTR,int);
using GetTextExtentPoint32AFn = BOOL (WINAPI*)(HDC,LPCSTR,int,LPSIZE);
using CreateFontAFn = HFONT (WINAPI*)(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCSTR);
using CreateRectRgnFn = HRGN (WINAPI*)(int,int,int,int);
using SelectClipRgnFn = int (WINAPI*)(HDC,HRGN);
using GetAsyncKeyStateFn = SHORT (WINAPI*)(int);

static RegisterClassExAFn RegisterClassExA_ = nullptr;
static CreateWindowExAFn CreateWindowExA_ = nullptr;
static ShowWindowFn ShowWindow_ = nullptr;
static UpdateWindowFn UpdateWindow_ = nullptr;
static PeekMessageAFn PeekMessageA_ = nullptr;
static TranslateMessageFn TranslateMessage_ = nullptr;
static DispatchMessageAFn DispatchMessageA_ = nullptr;
static DefWindowProcAFn DefWindowProcA_ = nullptr;
static PostQuitMessageFn PostQuitMessage_ = nullptr;
static LoadCursorAFn LoadCursorA_ = nullptr;
static AdjustWindowRectExFn AdjustWindowRectEx_ = nullptr;
static SetCaptureFn SetCapture_ = nullptr;
static ReleaseCaptureFn ReleaseCapture_ = nullptr;
static GetWindowRectFn GetWindowRect_ = nullptr;
static GetWindowLongPtrAFn GetWindowLongPtrA_ = nullptr;
static SetWindowLongPtrAFn SetWindowLongPtrA_ = nullptr;
static SetWindowPosFn SetWindowPos_ = nullptr;
static MonitorFromWindowFn MonitorFromWindow_ = nullptr;
static GetMonitorInfoAFn GetMonitorInfoA_ = nullptr;
static CoInitializeExFn CoInitializeEx_ = nullptr;
static CoCreateInstanceFn CoCreateInstance_ = nullptr;
static CoUninitializeFn CoUninitialize_ = nullptr;
static bool comInitializedByUs = false;
static bool comReady = false;
static CreateCompatibleDCFn CreateCompatibleDC_ = nullptr;
static SelectObjectFn SelectObject_ = nullptr;
static DeleteObjectFn DeleteObject_ = nullptr;
static DeleteDCFn DeleteDC_ = nullptr;
static SetBkModeFn SetBkMode_ = nullptr;
static SetTextColorFn SetTextColor_ = nullptr;
static TextOutAFn TextOutA_ = nullptr;
static GetTextExtentPoint32AFn GetTextExtentPoint32A_ = nullptr;
static CreateFontAFn CreateFontA_ = nullptr;
static CreateRectRgnFn CreateRectRgn_ = nullptr;
static SelectClipRgnFn SelectClipRgn_ = nullptr;
static GetAsyncKeyStateFn GetAsyncKeyState_ = nullptr;

static HWND hwnd = nullptr;
static int width = 1280;
static int height = 760;
static bool shouldClose = false;
static bool initialized = false;
static bool resizePending = false;
static int configFlags = 0;
static bool fullscreen = false;
static RECT windowedRect{0,0,1280,760};
static LONG_PTR windowedStyle = WS_OVERLAPPEDWINDOW;

static bool keys[256]{};
static bool prevKeys[256]{};
static bool keyPressedEvents[256]{};
static bool mouseButtons[3]{};
static bool prevMouseButtons[3]{};
static bool mousePressedEvents[3]{};
static bool mouseReleasedEvents[3]{};
static Vector2 mousePos{};
static Vector2 prevMousePos{};
static float mouseWheel = 0.0f;
// Caracteres digitados neste frame para campos de texto livres (anotacoes).
static std::string textInputChars;

static Camera2D currentCamera{};
static bool mode2D = false;
static int targetFps = 120;
static float frameTime = 1.0f / 60.0f;
static int fps = 60;
static std::chrono::steady_clock::time_point appStart;
static std::chrono::steady_clock::time_point lastFrameStart;
static std::chrono::steady_clock::time_point frameStart;
static int fpsCounter = 0;
static std::chrono::steady_clock::time_point fpsStart;

// ------------------------- Direct3D 11 -------------------------
using D3D11CreateDeviceAndSwapChainFn = HRESULT (WINAPI*)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
    const D3D_FEATURE_LEVEL*, UINT, UINT,
    const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**,
    ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
using D3DCompileFn = HRESULT (WINAPI*)(
    LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO*, ID3DInclude*,
    LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**);

static D3D11CreateDeviceAndSwapChainFn D3D11CreateDeviceAndSwapChain_ = nullptr;
static D3DCompileFn D3DCompile_ = nullptr;

static IDXGISwapChain* swapChain = nullptr;
static ID3D11Device* device = nullptr;
static ID3D11DeviceContext* context = nullptr;
static bool hardwareDevice = true;
static ID3D11RenderTargetView* renderTarget = nullptr;
static ID3D11Texture2D* backBuffer = nullptr;
static IDXGISurface1* gdiSurface = nullptr;
static ID3D11VertexShader* vertexShader = nullptr;
static ID3D11PixelShader* pixelShader = nullptr;
static ID3D11InputLayout* inputLayout = nullptr;
static ID3D11Buffer* vertexBuffer = nullptr;
static ID3D11BlendState* blendState = nullptr;
static ID3D11RasterizerState* rasterState = nullptr;
static ID3D11SamplerState* spriteSampler = nullptr;
static size_t vertexBufferCapacity = 0;
static UINT drawCallsLastFrame = 0;
static UINT verticesLastFrame = 0;

// GUIDs locais evitam depender de dxguid.lib/uuid.lib ao compilar com MinGW.
static const GUID IID_ID3D11Texture2D_Local = {0x6f15aaf2,0xd208,0x4e89,{0x9a,0xb4,0x48,0x95,0x35,0xd3,0x4f,0x9c}};
static const GUID IID_IDXGISurface1_Local = {0x4ae63092,0x6327,0x4c1b,{0x80,0xae,0xbf,0xe1,0x2e,0xa3,0x2b,0x86}};
// GUIDs WIC locais: evitam dependencia de windowscodecs.lib/uuid.lib.
static const GUID CLSID_WICImagingFactory_Local = {0xcacaf262,0x9370,0x4615,{0xa1,0x3b,0x9f,0x55,0x39,0xda,0x4c,0x0a}};
static const GUID IID_IWICImagingFactory_Local = {0xec5ec8a9,0xc395,0x4314,{0x9c,0x77,0x54,0xd7,0xa9,0x35,0xff,0x70}};
static const GUID GUID_WICPixelFormat32bppRGBA_Local = {0xf5c7ad2d,0x6a8d,0x43dd,{0xa7,0xa8,0xa2,0x99,0x35,0x26,0x1a,0xe9}};

struct GpuVertex {
    float x, y;       // clip-space
    float lx, ly;     // local coords para circle/ring
    float r, g, b, a;
    float mode;       // 0=solid, 1=circle, 2=ring, 3=sprite texturizado
    float param;      // ring inner thickness normalizada
    float u, v;       // UV para sprites
};

struct DrawRange {
    UINT start = 0;
    UINT count = 0;
    bool scissor = false;
    D3D11_RECT rect{};
    ID3D11ShaderResourceView* texture = nullptr;
};

struct TextCommand {
    std::string text;
    int x=0, y=0, size=12;
    Color color{};
    bool scissor=false;
    RECT clip{};
};

static std::vector<GpuVertex> vertices;
static std::vector<DrawRange> ranges;
static std::vector<TextCommand> textCommands;
static UINT activeRangeStart = 0;
static bool currentScissor = false;
static D3D11_RECT currentScissorRect{};
static ID3D11ShaderResourceView* currentTexture = nullptr;
static HDC measureDC = nullptr;
static std::unordered_map<int, HFONT> fontCache;

template<class T> void safeRelease(T*& p) { if (p) { p->Release(); p=nullptr; } }

template<class T>
T loadProc(HMODULE module, const char* name) {
    return reinterpret_cast<T>(GetProcAddress(module, name));
}

void loadApis() {
    HMODULE user = LoadLibraryA("user32.dll");
    HMODULE gdi = LoadLibraryA("gdi32.dll");
    HMODULE d3d = LoadLibraryA("d3d11.dll");
    HMODULE ole = LoadLibraryA("ole32.dll");
    HMODULE compiler = LoadLibraryA("d3dcompiler_47.dll");
    if (!compiler) compiler = LoadLibraryA("d3dcompiler_46.dll");
    if (!compiler) compiler = LoadLibraryA("d3dcompiler_43.dll");
    if (!user || !gdi || !d3d || !compiler) std::abort();

    RegisterClassExA_ = loadProc<RegisterClassExAFn>(user,"RegisterClassExA");
    CreateWindowExA_ = loadProc<CreateWindowExAFn>(user,"CreateWindowExA");
    ShowWindow_ = loadProc<ShowWindowFn>(user,"ShowWindow");
    UpdateWindow_ = loadProc<UpdateWindowFn>(user,"UpdateWindow");
    PeekMessageA_ = loadProc<PeekMessageAFn>(user,"PeekMessageA");
    TranslateMessage_ = loadProc<TranslateMessageFn>(user,"TranslateMessage");
    DispatchMessageA_ = loadProc<DispatchMessageAFn>(user,"DispatchMessageA");
    DefWindowProcA_ = loadProc<DefWindowProcAFn>(user,"DefWindowProcA");
    PostQuitMessage_ = loadProc<PostQuitMessageFn>(user,"PostQuitMessage");
    LoadCursorA_ = loadProc<LoadCursorAFn>(user,"LoadCursorA");
    AdjustWindowRectEx_ = loadProc<AdjustWindowRectExFn>(user,"AdjustWindowRectEx");
    SetCapture_ = loadProc<SetCaptureFn>(user,"SetCapture");
    ReleaseCapture_ = loadProc<ReleaseCaptureFn>(user,"ReleaseCapture");
    GetWindowRect_ = loadProc<GetWindowRectFn>(user,"GetWindowRect");
    GetWindowLongPtrA_ = loadProc<GetWindowLongPtrAFn>(user,"GetWindowLongPtrA");
    SetWindowLongPtrA_ = loadProc<SetWindowLongPtrAFn>(user,"SetWindowLongPtrA");
    SetWindowPos_ = loadProc<SetWindowPosFn>(user,"SetWindowPos");
    MonitorFromWindow_ = loadProc<MonitorFromWindowFn>(user,"MonitorFromWindow");
    GetMonitorInfoA_ = loadProc<GetMonitorInfoAFn>(user,"GetMonitorInfoA");
    if(ole){
        CoInitializeEx_ = loadProc<CoInitializeExFn>(ole,"CoInitializeEx");
        CoCreateInstance_ = loadProc<CoCreateInstanceFn>(ole,"CoCreateInstance");
        CoUninitialize_ = loadProc<CoUninitializeFn>(ole,"CoUninitialize");
        if(CoInitializeEx_ && CoCreateInstance_){
            const HRESULT chr=CoInitializeEx_(nullptr,COINIT_MULTITHREADED);
            comInitializedByUs=SUCCEEDED(chr);
            comReady=SUCCEEDED(chr) || chr==RPC_E_CHANGED_MODE;
        }
    }

    CreateCompatibleDC_ = loadProc<CreateCompatibleDCFn>(gdi,"CreateCompatibleDC");
    SelectObject_ = loadProc<SelectObjectFn>(gdi,"SelectObject");
    DeleteObject_ = loadProc<DeleteObjectFn>(gdi,"DeleteObject");
    DeleteDC_ = loadProc<DeleteDCFn>(gdi,"DeleteDC");
    SetBkMode_ = loadProc<SetBkModeFn>(gdi,"SetBkMode");
    SetTextColor_ = loadProc<SetTextColorFn>(gdi,"SetTextColor");
    TextOutA_ = loadProc<TextOutAFn>(gdi,"TextOutA");
    GetTextExtentPoint32A_ = loadProc<GetTextExtentPoint32AFn>(gdi,"GetTextExtentPoint32A");
    CreateFontA_ = loadProc<CreateFontAFn>(gdi,"CreateFontA");
    CreateRectRgn_ = loadProc<CreateRectRgnFn>(gdi,"CreateRectRgn");
    SelectClipRgn_ = loadProc<SelectClipRgnFn>(gdi,"SelectClipRgn");
    GetAsyncKeyState_ = loadProc<GetAsyncKeyStateFn>(user,"GetAsyncKeyState");

    D3D11CreateDeviceAndSwapChain_ = loadProc<D3D11CreateDeviceAndSwapChainFn>(d3d,"D3D11CreateDeviceAndSwapChain");
    D3DCompile_ = loadProc<D3DCompileFn>(compiler,"D3DCompile");
    if (!D3D11CreateDeviceAndSwapChain_ || !D3DCompile_) std::abort();
}

HFONT cachedFont(int fontSize) {
    fontSize = std::max(1, fontSize);
    auto it = fontCache.find(fontSize);
    if (it != fontCache.end()) return it->second;
    HFONT font = CreateFontA_(
        -fontSize,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Segoe UI"
    );
    fontCache.emplace(fontSize,font);
    return font;
}

void destroyFonts() {
    for (auto& kv : fontCache) DeleteObject_(kv.second);
    fontCache.clear();
    if (measureDC) { DeleteDC_(measureDC); measureDC=nullptr; }
}

COLORREF cref(Color c) {
    float a = c.a / 255.0f;
    unsigned char r = static_cast<unsigned char>(c.r * a);
    unsigned char g = static_cast<unsigned char>(c.g * a);
    unsigned char b = static_cast<unsigned char>(c.b * a);
    return RGB(r,g,b);
}

void closeRange() {
    const UINT end = static_cast<UINT>(vertices.size());
    if (end > activeRangeStart) {
        DrawRange r{};
        r.start = activeRangeStart;
        r.count = end - activeRangeStart;
        r.scissor = currentScissor;
        r.rect = currentScissorRect;
        r.texture = currentTexture;
        ranges.push_back(r);
    }
    activeRangeStart = end;
}

void setTexture(ID3D11ShaderResourceView* texture) {
    if(texture==currentTexture) return;
    closeRange();
    currentTexture=texture;
}

void setScissor(bool enabled, const D3D11_RECT& rect = {}) {
    if (enabled == currentScissor && (!enabled || std::memcmp(&rect,&currentScissorRect,sizeof(rect))==0)) return;
    closeRange();
    currentScissor = enabled;
    currentScissorRect = rect;
}

Vector2 worldToScreen(Vector2 p) {
    if (!mode2D) return p;
    return {
        currentCamera.offset.x + (p.x - currentCamera.target.x) * currentCamera.zoom,
        currentCamera.offset.y + (p.y - currentCamera.target.y) * currentCamera.zoom
    };
}
float scaleLength(float v) { return mode2D ? v * currentCamera.zoom : v; }

GpuVertex makeVertex(float sx, float sy, float lx, float ly, Color c, float mode, float param=0.0f, float u=0.0f, float v=0.0f) {
    const float w = static_cast<float>(std::max(1,width));
    const float h = static_cast<float>(std::max(1,height));
    return {
        sx / w * 2.0f - 1.0f,
        1.0f - sy / h * 2.0f,
        lx, ly,
        c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f,
        mode, param, u, v
    };
}

void addTriScreen(Vector2 a, Vector2 b, Vector2 c, Color col) {
    setTexture(nullptr);
    vertices.push_back(makeVertex(a.x,a.y,0,0,col,0));
    vertices.push_back(makeVertex(b.x,b.y,0,0,col,0));
    vertices.push_back(makeVertex(c.x,c.y,0,0,col,0));
}

void addQuadScreen(Vector2 a, Vector2 b, Vector2 c, Vector2 d, Color col) {
    addTriScreen(a,b,c,col);
    addTriScreen(a,c,d,col);
}

void addRectScreen(float l,float t,float r,float b,Color c) {
    addQuadScreen({l,t},{r,t},{r,b},{l,b},c);
}

void addLineScreen(Vector2 a, Vector2 b, float thick, Color c) {
    float dx=b.x-a.x, dy=b.y-a.y;
    float len=std::sqrt(dx*dx+dy*dy);
    if (len < 0.0001f) return;
    float hx = -dy/len * thick*0.5f;
    float hy =  dx/len * thick*0.5f;
    addQuadScreen({a.x+hx,a.y+hy},{b.x+hx,b.y+hy},{b.x-hx,b.y-hy},{a.x-hx,a.y-hy},c);
}

void addCircleScreen(Vector2 center, float radiusPx, Color c, bool ring=false, float ringThicknessPx=1.0f) {
    setTexture(nullptr);
    radiusPx = std::max(0.5f,radiusPx);
    float l=center.x-radiusPx, t=center.y-radiusPx, r=center.x+radiusPx, b=center.y+radiusPx;
    float mode = ring ? 2.0f : 1.0f;
    float param = ring ? std::clamp(ringThicknessPx/radiusPx,0.01f,0.95f) : 0.0f;
    GpuVertex v0=makeVertex(l,t,-1,-1,c,mode,param);
    GpuVertex v1=makeVertex(r,t, 1,-1,c,mode,param);
    GpuVertex v2=makeVertex(r,b, 1, 1,c,mode,param);
    GpuVertex v3=makeVertex(l,b,-1, 1,c,mode,param);
    vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
    vertices.push_back(v0); vertices.push_back(v2); vertices.push_back(v3);
}

void addSpriteScreen(ID3D11ShaderResourceView* srv, Vector2 center, float widthPx, float heightPx,
                     float rotationDeg, Color tint, float u0,float v0,float u1,float v1) {
    if(!srv || widthPx<=0.0f || heightPx<=0.0f) return;
    setTexture(srv);
    const float hx=widthPx*0.5f, hy=heightPx*0.5f;
    const float rad=rotationDeg*DEG2RAD, cs=std::cos(rad), sn=std::sin(rad);
    auto rot=[&](float x,float y){return Vector2{center.x+x*cs-y*sn,center.y+x*sn+y*cs};};
    const Vector2 p0=rot(-hx,-hy), p1=rot(hx,-hy), p2=rot(hx,hy), p3=rot(-hx,hy);
    const GpuVertex a=makeVertex(p0.x,p0.y,0,0,tint,3.0f,0.0f,u0,v0);
    const GpuVertex b=makeVertex(p1.x,p1.y,0,0,tint,3.0f,0.0f,u1,v0);
    const GpuVertex c=makeVertex(p2.x,p2.y,0,0,tint,3.0f,0.0f,u1,v1);
    const GpuVertex d=makeVertex(p3.x,p3.y,0,0,tint,3.0f,0.0f,u0,v1);
    vertices.push_back(a);vertices.push_back(b);vertices.push_back(c);
    vertices.push_back(a);vertices.push_back(c);vertices.push_back(d);
}

std::vector<Vector2> roundedPerimeter(float l,float t,float r,float b,float radius,int seg=5) {
    std::vector<Vector2> pts;
    radius=std::clamp(radius,0.0f,std::min(r-l,b-t)*0.5f);
    const Vector2 centers[4]{{r-radius,t+radius},{r-radius,b-radius},{l+radius,b-radius},{l+radius,t+radius}};
    const float starts[4]{-PI*0.5f,0.0f,PI*0.5f,PI};
    for(int corner=0;corner<4;++corner){
        for(int i=0;i<=seg;++i){
            float a=starts[corner]+(PI*0.5f)*(float(i)/float(seg));
            pts.push_back({centers[corner].x+std::cos(a)*radius,centers[corner].y+std::sin(a)*radius});
        }
    }
    return pts;
}

bool compileShaders() {
    static const char* hlsl = R"HLSL(
struct VSIn {
    float2 pos : POSITION;
    float2 local : TEXCOORD0;
    float4 color : COLOR0;
    float mode : TEXCOORD1;
    float param : TEXCOORD2;
    float2 uv : TEXCOORD3;
};
struct PSIn {
    float4 pos : SV_POSITION;
    float2 local : TEXCOORD0;
    float4 color : COLOR0;
    float mode : TEXCOORD1;
    float param : TEXCOORD2;
    float2 uv : TEXCOORD3;
};
PSIn VSMain(VSIn i) {
    PSIn o;
    o.pos=float4(i.pos,0,1);
    o.local=i.local;
    o.color=i.color;
    o.mode=i.mode;
    o.param=i.param;
    o.uv=i.uv;
    return o;
}
Texture2D SpriteTex : register(t0);
SamplerState SpriteSampler : register(s0);
float4 PSMain(PSIn i) : SV_TARGET {
    if (i.mode > 2.5) {
        float4 tex = SpriteTex.Sample(SpriteSampler,i.uv);
        float lum = dot(tex.rgb,float3(0.299,0.587,0.114));
        // Colorizacao de arte cinza: sombras continuam escuras, tons medios recebem o gene
        // e highlights podem passar um pouco do RGB-base sem virar branco puro.
        float shade = lerp(0.12,1.22,saturate(lum));
        float3 rgb = saturate(i.color.rgb * shade);
        return float4(rgb, tex.a * i.color.a);
    }
    if (i.mode < 0.5) return i.color;
    float d = length(i.local);
    float aa = max(fwidth(d), 0.0025);
    float outer = 1.0 - smoothstep(1.0-aa, 1.0, d);
    if (i.mode < 1.5) return float4(i.color.rgb, i.color.a * outer);
    float innerR = 1.0 - saturate(i.param);
    float innerCut = smoothstep(innerR-aa, innerR+aa, d);
    return float4(i.color.rgb, i.color.a * outer * innerCut);
}
)HLSL";

    ID3DBlob* vsBlob=nullptr; ID3DBlob* psBlob=nullptr; ID3DBlob* err=nullptr;
    UINT flags=D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
    HRESULT hr=D3DCompile_(hlsl,std::strlen(hlsl),"embedded",nullptr,nullptr,"VSMain","vs_4_0",flags,0,&vsBlob,&err);
    if(FAILED(hr)){
        if(err){ std::fprintf(stderr,"VS HLSL: %s\n",(const char*)err->GetBufferPointer()); err->Release(); }
        return false;
    }
    if(err){err->Release();err=nullptr;}
    hr=D3DCompile_(hlsl,std::strlen(hlsl),"embedded",nullptr,nullptr,"PSMain","ps_4_0",flags,0,&psBlob,&err);
    if(FAILED(hr)){
        if(err){ std::fprintf(stderr,"PS HLSL: %s\n",(const char*)err->GetBufferPointer()); err->Release(); }
        vsBlob->Release(); return false;
    }
    if(err)err->Release();

    hr=device->CreateVertexShader(vsBlob->GetBufferPointer(),vsBlob->GetBufferSize(),nullptr,&vertexShader);
    if(SUCCEEDED(hr)) hr=device->CreatePixelShader(psBlob->GetBufferPointer(),psBlob->GetBufferSize(),nullptr,&pixelShader);
    D3D11_INPUT_ELEMENT_DESC elems[] = {
        {"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,(UINT)offsetof(GpuVertex,x),D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,(UINT)offsetof(GpuVertex,lx),D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,(UINT)offsetof(GpuVertex,r),D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,0,(UINT)offsetof(GpuVertex,mode),D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",2,DXGI_FORMAT_R32_FLOAT,0,(UINT)offsetof(GpuVertex,param),D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",3,DXGI_FORMAT_R32G32_FLOAT,0,(UINT)offsetof(GpuVertex,u),D3D11_INPUT_PER_VERTEX_DATA,0}
    };
    if(SUCCEEDED(hr)) hr=device->CreateInputLayout(elems,6,vsBlob->GetBufferPointer(),vsBlob->GetBufferSize(),&inputLayout);
    vsBlob->Release(); psBlob->Release();
    return SUCCEEDED(hr);
}

bool createBackbufferViews() {
    safeRelease(gdiSurface); safeRelease(renderTarget); safeRelease(backBuffer);
    HRESULT hr=swapChain->GetBuffer(0,IID_ID3D11Texture2D_Local,reinterpret_cast<void**>(&backBuffer));
    if(FAILED(hr)) return false;
    hr=device->CreateRenderTargetView(backBuffer,nullptr,&renderTarget);
    if(FAILED(hr)) return false;
    hr=backBuffer->QueryInterface(IID_IDXGISurface1_Local,reinterpret_cast<void**>(&gdiSurface));
    return SUCCEEDED(hr);
}

bool ensureVertexBuffer(size_t needed) {
    if(vertexBuffer && needed<=vertexBufferCapacity) return true;
    safeRelease(vertexBuffer);
    size_t cap=262144;
    while(cap<needed) cap*=2;
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth=(UINT)(cap*sizeof(GpuVertex));
    bd.Usage=D3D11_USAGE_DYNAMIC;
    bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
    HRESULT hr=device->CreateBuffer(&bd,nullptr,&vertexBuffer);
    if(FAILED(hr)) return false;
    vertexBufferCapacity=cap;
    return true;
}

bool initD3D() {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.Width=std::max(1,width);
    sd.BufferDesc.Height=std::max(1,height);
    sd.BufferDesc.Format=DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count=1;
    sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount=1;
    sd.OutputWindow=hwnd;
    sd.Windowed=TRUE;
    sd.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags=DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;

    const D3D_FEATURE_LEVEL levels[]={D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0};
    D3D_FEATURE_LEVEL actual{};
    UINT flags=0;
    hardwareDevice=true;
    HRESULT hr=D3D11CreateDeviceAndSwapChain_(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,flags,
        levels,(UINT)(sizeof(levels)/sizeof(levels[0])),D3D11_SDK_VERSION,&sd,&swapChain,&device,&actual,&context);
    if(FAILED(hr)){
        hardwareDevice=false;
        hr=D3D11CreateDeviceAndSwapChain_(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,flags,
            levels,(UINT)(sizeof(levels)/sizeof(levels[0])),D3D11_SDK_VERSION,&sd,&swapChain,&device,&actual,&context);
    }
    if(FAILED(hr)) return false;
    if(!createBackbufferViews()) return false;
    if(!compileShaders()) return false;

    D3D11_BLEND_DESC blend{};
    blend.RenderTarget[0].BlendEnable=TRUE;
    blend.RenderTarget[0].SrcBlend=D3D11_BLEND_SRC_ALPHA;
    blend.RenderTarget[0].DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOp=D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_ONE;
    blend.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
    if(FAILED(device->CreateBlendState(&blend,&blendState))) return false;

    D3D11_RASTERIZER_DESC rs{};
    rs.FillMode=D3D11_FILL_SOLID;
    rs.CullMode=D3D11_CULL_NONE;
    rs.ScissorEnable=TRUE;
    rs.DepthClipEnable=TRUE;
    if(FAILED(device->CreateRasterizerState(&rs,&rasterState))) return false;

    D3D11_SAMPLER_DESC samp{};
    samp.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samp.AddressU=D3D11_TEXTURE_ADDRESS_CLAMP;
    samp.AddressV=D3D11_TEXTURE_ADDRESS_CLAMP;
    samp.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;
    samp.MinLOD=0.0f;samp.MaxLOD=D3D11_FLOAT32_MAX;
    if(FAILED(device->CreateSamplerState(&samp,&spriteSampler))) return false;

    vertices.reserve(131072);
    ranges.reserve(32);
    textCommands.reserve(512);
    return ensureVertexBuffer(vertices.capacity());
}

void resizeD3DIfNeeded() {
    if(!resizePending || !swapChain || width<=0 || height<=0) return;
    resizePending=false;
    if(context) context->OMSetRenderTargets(0,nullptr,nullptr);
    safeRelease(gdiSurface); safeRelease(renderTarget); safeRelease(backBuffer);
    HRESULT hr=swapChain->ResizeBuffers(1,std::max(1,width),std::max(1,height),DXGI_FORMAT_B8G8R8A8_UNORM,DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE);
    if(FAILED(hr) || !createBackbufferViews()) {
        std::fprintf(stderr,"Falha ao redimensionar Direct3D 11 (0x%08lx)\n",(unsigned long)hr);
        shouldClose=true;
    }
}

void destroyD3D() {
    if(context) context->ClearState();
    safeRelease(vertexBuffer);
    safeRelease(spriteSampler);
    safeRelease(rasterState);
    safeRelease(blendState);
    safeRelease(inputLayout);
    safeRelease(pixelShader);
    safeRelease(vertexShader);
    safeRelease(gdiSurface);
    safeRelease(renderTarget);
    safeRelease(backBuffer);
    safeRelease(swapChain);
    safeRelease(context);
    safeRelease(device);
    if(comInitializedByUs && CoUninitialize_){CoUninitialize_();comInitializedByUs=false;}
    comReady=false;
}

LRESULT CALLBACK wndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_CLOSE: shouldClose=true; return 0;
        case WM_DESTROY: shouldClose=true; PostQuitMessage_(0); return 0;
        case WM_ERASEBKGND: return 1;
        case WM_SIZE:
            width=std::max(1,static_cast<int>(LOWORD(l)));
            height=std::max(1,static_cast<int>(HIWORD(l)));
            if(initialized) resizePending=true;
            return 0;
        case WM_MOUSEMOVE:
            mousePos.x=static_cast<float>(static_cast<short>(LOWORD(l)));
            mousePos.y=static_cast<float>(static_cast<short>(HIWORD(l)));
            return 0;
        case WM_LBUTTONDOWN:
            if(!mouseButtons[0]) mousePressedEvents[0]=true;
            mouseButtons[0]=true; if(SetCapture_)SetCapture_(h); return 0;
        case WM_LBUTTONUP:
            mouseReleasedEvents[0]=true; mouseButtons[0]=false; if(ReleaseCapture_)ReleaseCapture_(); return 0;
        case WM_RBUTTONDOWN:
            if(!mouseButtons[1]) mousePressedEvents[1]=true;
            mouseButtons[1]=true; if(SetCapture_)SetCapture_(h); return 0;
        case WM_RBUTTONUP:
            mouseReleasedEvents[1]=true; mouseButtons[1]=false; if(ReleaseCapture_)ReleaseCapture_(); return 0;
        case WM_MOUSEWHEEL:
            mouseWheel += static_cast<float>(GET_WHEEL_DELTA_WPARAM(w))/static_cast<float>(WHEEL_DELTA); return 0;
        case WM_CHAR:
            // A janela ANSI entrega os caracteres de texto na pagina de codigo do sistema.
            // Controles (Enter/Backspace etc.) continuam tratados pelos eventos de tecla.
            if (w >= 32 && w <= 255 && textInputChars.size() < 256)
                textInputChars.push_back(static_cast<char>(w));
            return 0;
        case WM_KEYDOWN: case WM_SYSKEYDOWN:
            if(w<256){ if(!keys[w])keyPressedEvents[w]=true; keys[w]=true; }
            if(w==VK_ESCAPE)shouldClose=true; return 0;
        case WM_KEYUP: case WM_SYSKEYUP:
            if(w<256)keys[w]=false; return 0;
        case WM_KILLFOCUS:
            // Evita tecla presa ao alternar janela/Alt+Tab. Ao recuperar foco,
            // GetAsyncKeyState abaixo volta a refletir imediatamente o teclado fisico.
            std::memset(keys,0,sizeof(keys));
            return 0;
    }
    return DefWindowProcA_?DefWindowProcA_(h,msg,w,l):0;
}

} // namespace winshim

void SetConfigFlags(int flags) { winshim::configFlags=flags; }

void InitWindow(int w,int h,const char* title) {
    using namespace winshim;
    loadApis(); width=w; height=h;
    HINSTANCE inst=GetModuleHandleA(nullptr);
    WNDCLASSEXA wc{}; wc.cbSize=sizeof(wc); wc.lpfnWndProc=wndProc; wc.hInstance=inst;
    wc.hCursor=LoadCursorA_(nullptr,IDC_ARROW); wc.lpszClassName="VidaArtificialD3D11SingleCpp";
    wc.style=CS_HREDRAW|CS_VREDRAW; RegisterClassExA_(&wc);
    DWORD style=WS_OVERLAPPEDWINDOW; RECT r{0,0,w,h}; AdjustWindowRectEx_(&r,style,FALSE,0);
    hwnd=CreateWindowExA_(0,wc.lpszClassName,title,style,CW_USEDEFAULT,CW_USEDEFAULT,r.right-r.left,r.bottom-r.top,nullptr,nullptr,inst,nullptr);
    if(!hwnd)std::abort();
    ShowWindow_(hwnd,SW_SHOW); UpdateWindow_(hwnd);
    measureDC=CreateCompatibleDC_(nullptr);
    initialized=true;
    if(!initD3D()){
        std::fprintf(stderr,"Nao foi possivel iniciar Direct3D 11. Verifique o driver de video.\n");
        std::abort();
    }
    appStart=std::chrono::steady_clock::now(); lastFrameStart=appStart; frameStart=appStart; fpsStart=appStart;
}

void SetTargetFPS(int v){winshim::targetFps=std::max(1,v);}

bool IsWindowFullscreen(){ return winshim::fullscreen; }
void ToggleFullscreen(){
    using namespace winshim;
    if(!hwnd || !GetWindowRect_ || !GetWindowLongPtrA_ || !SetWindowLongPtrA_ || !SetWindowPos_ || !MonitorFromWindow_ || !GetMonitorInfoA_) return;
    if(!fullscreen){
        GetWindowRect_(hwnd,&windowedRect);
        windowedStyle=GetWindowLongPtrA_(hwnd,GWL_STYLE);
        HMONITOR mon=MonitorFromWindow_(hwnd,MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};mi.cbSize=sizeof(mi);
        if(!mon || !GetMonitorInfoA_(mon,&mi)) return;
        SetWindowLongPtrA_(hwnd,GWL_STYLE,(windowedStyle & ~WS_OVERLAPPEDWINDOW) | WS_POPUP | WS_VISIBLE);
        SetWindowPos_(hwnd,nullptr,mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right-mi.rcMonitor.left,mi.rcMonitor.bottom-mi.rcMonitor.top,
            SWP_NOOWNERZORDER|SWP_FRAMECHANGED);
        fullscreen=true;
    }else{
        SetWindowLongPtrA_(hwnd,GWL_STYLE,windowedStyle);
        SetWindowPos_(hwnd,nullptr,windowedRect.left,windowedRect.top,windowedRect.right-windowedRect.left,windowedRect.bottom-windowedRect.top,
            SWP_NOOWNERZORDER|SWP_FRAMECHANGED);
        fullscreen=false;
    }
}

bool WindowShouldClose(){
    using namespace winshim;
    std::memcpy(prevKeys,keys,sizeof(keys)); std::memcpy(prevMouseButtons,mouseButtons,sizeof(mouseButtons));
    std::memset(keyPressedEvents,0,sizeof(keyPressedEvents)); std::memset(mousePressedEvents,0,sizeof(mousePressedEvents));
    std::memset(mouseReleasedEvents,0,sizeof(mouseReleasedEvents)); prevMousePos=mousePos; mouseWheel=0.0f; textInputChars.clear();
    MSG msg{}; while(PeekMessageA_(&msg,nullptr,0,0,PM_REMOVE)){
        if(msg.message==WM_QUIT)shouldClose=true; TranslateMessage_(&msg); DispatchMessageA_(&msg);
    }
    auto now=std::chrono::steady_clock::now(); frameTime=std::chrono::duration<float>(now-lastFrameStart).count();
    if(frameTime<=0)frameTime=1.0f/120.0f; lastFrameStart=now; frameStart=now;
    ++fpsCounter; float elapsed=std::chrono::duration<float>(now-fpsStart).count();
    if(elapsed>=0.5f){fps=std::max(1,(int)std::lround(fpsCounter/elapsed));fpsCounter=0;fpsStart=now;}
    return shouldClose;
}

void RL_CloseWindow(){ using namespace winshim; destroyD3D(); destroyFonts(); hwnd=nullptr; }
#define CloseWindow RL_CloseWindow

int GetScreenWidth(){return winshim::width;} int GetScreenHeight(){return winshim::height;}
float GetFrameTime(){return winshim::frameTime;} int GetFPS(){return winshim::fps;}
unsigned int GetRenderDrawCalls(){return winshim::drawCallsLastFrame;}
unsigned int GetRenderVertexCount(){return winshim::verticesLastFrame;}
std::size_t GetRenderCpuBufferBytes(){using namespace winshim;return vertices.capacity()*sizeof(GpuVertex)+ranges.capacity()*sizeof(DrawRange)+textCommands.capacity()*sizeof(TextCommand);}
std::size_t GetRenderGpuVertexBufferBytes(){using namespace winshim;return vertexBufferCapacity*sizeof(GpuVertex);}
double GetTime(){return std::chrono::duration<double>(std::chrono::steady_clock::now()-winshim::appStart).count();}
Vector2 GetMousePosition(){return winshim::mousePos;}
Vector2 GetMouseDelta(){return {winshim::mousePos.x-winshim::prevMousePos.x,winshim::mousePos.y-winshim::prevMousePos.y};}
float GetMouseWheelMove(){return winshim::mouseWheel;}
bool IsKeyPressed(int key){return key>=0&&key<256&&winshim::keyPressedEvents[key];}
bool IsKeyDown(int key){
    if(key<0||key>=256) return false;
    // WM_KEYDOWN e suficiente na maioria dos PCs, mas alguns notebooks/drivers
    // entregam repeticao/estado continuo de forma irregular. GetAsyncKeyState
    // funciona como segunda fonte para teclas mantidas pressionadas.
    const bool eventState=winshim::keys[key];
    const bool physicalState=winshim::GetAsyncKeyState_ && ((winshim::GetAsyncKeyState_(key)&0x8000)!=0);
    return eventState||physicalState;
}
bool IsMouseButtonDown(int b){return b>=0&&b<3&&winshim::mouseButtons[b];}
bool IsMouseButtonPressed(int b){return b>=0&&b<3&&winshim::mousePressedEvents[b];}
bool IsMouseButtonReleased(int b){return b>=0&&b<3&&winshim::mouseReleasedEvents[b];}
const std::string& GetTextInputChars(){return winshim::textInputChars;}
Color Fade(Color c,float a){c.a=(unsigned char)(std::clamp(a,0.0f,1.0f)*255.0f);return c;}
const char* TextFormat(const char* fmt,...){static thread_local char buffer[2048];va_list a;va_start(a,fmt);std::vsnprintf(buffer,sizeof(buffer),fmt,a);va_end(a);return buffer;}

void BeginDrawing(){
    using namespace winshim;
    resizeD3DIfNeeded();
    vertices.clear(); ranges.clear(); textCommands.clear(); activeRangeStart=0; currentScissor=false; currentTexture=nullptr;
    if(!context||!renderTarget)return;
    context->OMSetRenderTargets(1,&renderTarget,nullptr);
    D3D11_VIEWPORT vp{}; vp.Width=(FLOAT)std::max(1,width); vp.Height=(FLOAT)std::max(1,height); vp.MinDepth=0; vp.MaxDepth=1;
    context->RSSetViewports(1,&vp);
}

void EndDrawing(){
    using namespace winshim;
    closeRange();
    drawCallsLastFrame=0; verticesLastFrame=(UINT)vertices.size();
    if(context&&renderTarget&& !vertices.empty() && ensureVertexBuffer(vertices.size())){
        D3D11_MAPPED_SUBRESOURCE map{};
        if(SUCCEEDED(context->Map(vertexBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&map))){
            std::memcpy(map.pData,vertices.data(),vertices.size()*sizeof(GpuVertex)); context->Unmap(vertexBuffer,0);
            UINT stride=sizeof(GpuVertex), offset=0; context->IASetInputLayout(inputLayout);
            context->IASetVertexBuffers(0,1,&vertexBuffer,&stride,&offset); context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->VSSetShader(vertexShader,nullptr,0); context->PSSetShader(pixelShader,nullptr,0);
            context->PSSetSamplers(0,1,&spriteSampler);
            float factor[4]{0,0,0,0}; context->OMSetBlendState(blendState,factor,0xffffffffu); context->RSSetState(rasterState);
            D3D11_RECT full{0,0,std::max(1,width),std::max(1,height)};
            for(const auto& r:ranges){
                D3D11_RECT sc=r.scissor?r.rect:full; context->RSSetScissorRects(1,&sc);
                ID3D11ShaderResourceView* srv=r.texture; context->PSSetShaderResources(0,1,&srv);
                context->Draw(r.count,r.start); ++drawCallsLastFrame;
            }
            ID3D11ShaderResourceView* noSrv=nullptr;context->PSSetShaderResources(0,1,&noSrv);
        }
    }

    // Texto continua via GDI apenas no final do quadro. O mundo/HUD geometrico ja
    // foi renderizado pela GPU. DXGI exige ReleaseDC antes de Present/novo D3D.
    if(gdiSurface && !textCommands.empty()){
        context->Flush(); HDC dc=nullptr;
        if(SUCCEEDED(gdiSurface->GetDC(FALSE,&dc)) && dc){
            SetBkMode_(dc,TRANSPARENT);
            for(const auto& t:textCommands){
                if(t.scissor){HRGN rg=CreateRectRgn_(t.clip.left,t.clip.top,t.clip.right,t.clip.bottom);SelectClipRgn_(dc,rg);DeleteObject_(rg);}else SelectClipRgn_(dc,nullptr);
                HFONT f=cachedFont(t.size); HGDIOBJ old=SelectObject_(dc,f); SetTextColor_(dc,cref(t.color));
                TextOutA_(dc,t.x,t.y,t.text.c_str(),(int)t.text.size()); SelectObject_(dc,old);
            }
            SelectClipRgn_(dc,nullptr); gdiSurface->ReleaseDC(nullptr);
        }
    }
    if(swapChain) swapChain->Present((configFlags&FLAG_VSYNC_HINT)?1:0,0);
    if(context&&renderTarget) context->OMSetRenderTargets(1,&renderTarget,nullptr);

    if(targetFps>0 && !(configFlags&FLAG_VSYNC_HINT)){
        const double target=1.0/(double)targetFps; double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-frameStart).count();
        if(elapsed<target)std::this_thread::sleep_for(std::chrono::duration<double>(target-elapsed));
    }
}

void BeginMode2D(Camera2D c){winshim::currentCamera=c;winshim::mode2D=true;} void EndMode2D(){winshim::mode2D=false;}
Vector2 GetScreenToWorld2D(Vector2 p,Camera2D c){return {c.target.x+(p.x-c.offset.x)/std::max(0.000001f,c.zoom),c.target.y+(p.y-c.offset.y)/std::max(0.000001f,c.zoom)};}

void ClearBackground(Color c){
    using namespace winshim; if(!context||!renderTarget)return;
    const float v[4]{c.r/255.0f,c.g/255.0f,c.b/255.0f,c.a/255.0f}; context->ClearRenderTargetView(renderTarget,v);
}
bool CheckCollisionPointRec(Vector2 p,RectF r){return p.x>=r.x&&p.y>=r.y&&p.x<=r.x+r.width&&p.y<=r.y+r.height;}

void DrawRectangle(int x,int y,int w,int h,Color c){Vector2 a=winshim::worldToScreen({(float)x,(float)y});Vector2 b=winshim::worldToScreen({(float)(x+w),(float)(y+h)});winshim::addRectScreen(std::min(a.x,b.x),std::min(a.y,b.y),std::max(a.x,b.x),std::max(a.y,b.y),c);}
void DrawRectangleV(Vector2 p,Vector2 s,Color c){Vector2 a=winshim::worldToScreen(p);Vector2 b=winshim::worldToScreen({p.x+s.x,p.y+s.y});winshim::addRectScreen(std::min(a.x,b.x),std::min(a.y,b.y),std::max(a.x,b.x),std::max(a.y,b.y),c);}
void DrawRectangleLinesEx(RectF r,float thick,Color c){Vector2 a=winshim::worldToScreen({r.x,r.y}),b=winshim::worldToScreen({r.x+r.width,r.y+r.height});float l=std::min(a.x,b.x),t=std::min(a.y,b.y),rr=std::max(a.x,b.x),bb=std::max(a.y,b.y),pw=std::max(1.0f,winshim::scaleLength(thick));winshim::addLineScreen({l,t},{rr,t},pw,c);winshim::addLineScreen({rr,t},{rr,bb},pw,c);winshim::addLineScreen({rr,bb},{l,bb},pw,c);winshim::addLineScreen({l,bb},{l,t},pw,c);}
void DrawRectangleRounded(RectF r,float roundness,int,Color c){Vector2 a=winshim::worldToScreen({r.x,r.y}),b=winshim::worldToScreen({r.x+r.width,r.y+r.height});float l=std::min(a.x,b.x),t=std::min(a.y,b.y),rr=std::max(a.x,b.x),bb=std::max(a.y,b.y);float rad=std::min(rr-l,bb-t)*std::clamp(roundness,0.0f,0.5f);auto pts=winshim::roundedPerimeter(l,t,rr,bb,rad,5);Vector2 center{(l+rr)*0.5f,(t+bb)*0.5f};for(size_t i=0;i<pts.size();++i)winshim::addTriScreen(center,pts[i],pts[(i+1)%pts.size()],c);}
void DrawRectangleRoundedLines(RectF r,float roundness,int,float thick,Color c){Vector2 a=winshim::worldToScreen({r.x,r.y}),b=winshim::worldToScreen({r.x+r.width,r.y+r.height});float l=std::min(a.x,b.x),t=std::min(a.y,b.y),rr=std::max(a.x,b.x),bb=std::max(a.y,b.y),rad=std::min(rr-l,bb-t)*std::clamp(roundness,0.0f,0.5f),pw=std::max(1.0f,winshim::scaleLength(thick));auto pts=winshim::roundedPerimeter(l,t,rr,bb,rad,5);for(size_t i=0;i<pts.size();++i)winshim::addLineScreen(pts[i],pts[(i+1)%pts.size()],pw,c);}
void DrawLineEx(Vector2 a,Vector2 b,float thick,Color c){a=winshim::worldToScreen(a);b=winshim::worldToScreen(b);winshim::addLineScreen(a,b,std::max(1.0f,winshim::scaleLength(thick)),c);}
void DrawCircleV(Vector2 p,float radius,Color c){p=winshim::worldToScreen(p);winshim::addCircleScreen(p,std::max(0.5f,winshim::scaleLength(radius)),c,false);}
void DrawCircleLinesV(Vector2 p,float radius,Color c){p=winshim::worldToScreen(p);winshim::addCircleScreen(p,std::max(0.5f,winshim::scaleLength(radius)),c,true,1.2f);}
void DrawCircleAlphaV(Vector2 p,float radius,Color fill,Color outline,float opacity){opacity=std::clamp(opacity,0.0f,1.0f);if(opacity<=0.001f)return;fill.a=(unsigned char)std::lround(fill.a*opacity);outline.a=(unsigned char)std::lround(outline.a*opacity);p=winshim::worldToScreen(p);float rr=std::max(0.5f,winshim::scaleLength(radius));winshim::addCircleScreen(p,rr,outline,false);float inner=std::max(0.1f,rr-std::clamp(rr*0.16f,1.0f,2.5f));winshim::addCircleScreen(p,inner,fill,false);}
void DrawTriangle(Vector2 a,Vector2 b,Vector2 c,Color col){winshim::addTriScreen(winshim::worldToScreen(a),winshim::worldToScreen(b),winshim::worldToScreen(c),col);}
void DrawRectanglePro(RectF r,Vector2 origin,float rot,Color c){float rad=rot*DEG2RAD,cs=std::cos(rad),sn=std::sin(rad);Vector2 local[4]{{0,0},{r.width,0},{r.width,r.height},{0,r.height}},p[4];for(int i=0;i<4;++i){float x=local[i].x-origin.x,y=local[i].y-origin.y;p[i]=winshim::worldToScreen({r.x+x*cs-y*sn,r.y+x*sn+y*cs});}winshim::addQuadScreen(p[0],p[1],p[2],p[3],c);}

Texture2D LoadTexture(const char* fileName){
    using namespace winshim;
    Texture2D out{};
    if(!fileName||!*fileName||!device||!comReady||!CoCreateInstance_) return out;
    std::filesystem::path path(fileName);
    IWICImagingFactory* factory=nullptr;IWICBitmapDecoder* decoder=nullptr;IWICBitmapFrameDecode* frame=nullptr;IWICFormatConverter* converter=nullptr;
    UINT w=0,h=0;
    HRESULT hr=CoCreateInstance_(CLSID_WICImagingFactory_Local,nullptr,CLSCTX_INPROC_SERVER,IID_IWICImagingFactory_Local,(void**)&factory);
    if(FAILED(hr)||!factory) goto done;
    hr=factory->CreateDecoderFromFilename(path.wstring().c_str(),nullptr,GENERIC_READ,WICDecodeMetadataCacheOnLoad,&decoder);
    if(FAILED(hr)||!decoder) goto done;
    hr=decoder->GetFrame(0,&frame);if(FAILED(hr)||!frame) goto done;
    hr=factory->CreateFormatConverter(&converter);if(FAILED(hr)||!converter) goto done;
    hr=converter->Initialize(frame,GUID_WICPixelFormat32bppRGBA_Local,WICBitmapDitherTypeNone,nullptr,0.0,WICBitmapPaletteTypeCustom);
    if(FAILED(hr)) goto done;
    hr=converter->GetSize(&w,&h);if(FAILED(hr)||w==0||h==0) goto done;
    {
        std::vector<unsigned char> rgba((size_t)w*(size_t)h*4u);
        const UINT stride=w*4u;
        hr=converter->CopyPixels(nullptr,stride,(UINT)rgba.size(),rgba.data());if(FAILED(hr)) goto done;
        // Para o sprite da planta, remove automaticamente o fundo escuro e quaisquer
        // "sujeiras"/bolinhas isoladas, mantendo so o maior componente conectado do desenho.
        std::string fnLower=path.filename().string();
        std::transform(fnLower.begin(),fnLower.end(),fnLower.begin(),[](unsigned char c){return (char)std::tolower(c);});
        const bool autoBlackKey = fnLower.find("21_31_09")!=std::string::npos || fnLower.find("planta")!=std::string::npos;
        if(autoBlackKey){
            auto idx=[&](UINT x,UINT y){ return ((size_t)y*(size_t)w + (size_t)x)*4u; };
            auto lumMax=[&](size_t i){ return std::max({(int)rgba[i+0],(int)rgba[i+1],(int)rgba[i+2]}); };
            auto colorDist=[&](size_t i,int br,int bg,int bb){ return std::abs((int)rgba[i+0]-br)+std::abs((int)rgba[i+1]-bg)+std::abs((int)rgba[i+2]-bb); };

            // 1) estima a cor de fundo pelas quinas e remove regioes conectadas a borda.
            int br=0,bg=0,bb=0,cornerCount=0;
            const UINT cx[4]={0,w?w-1:0,0,w?w-1:0};
            const UINT cy[4]={0,0,h?h-1:0,h?h-1:0};
            for(int k=0;k<4;++k){ if(w==0||h==0) break; size_t i=idx(cx[k],cy[k]); br+=rgba[i+0]; bg+=rgba[i+1]; bb+=rgba[i+2]; ++cornerCount; }
            if(cornerCount>0){ br/=cornerCount; bg/=cornerCount; bb/=cornerCount; }
            std::vector<unsigned char> bgmask((size_t)w*(size_t)h,0);
            std::vector<std::pair<UINT,UINT>> stack; stack.reserve((size_t)w*2u+(size_t)h*2u+16u);
            auto pushIfBg=[&](UINT x,UINT y){
                size_t p=(size_t)y*(size_t)w+(size_t)x; if(bgmask[p]) return;
                size_t i=idx(x,y); if(rgba[i+3]==0) { bgmask[p]=1; return; }
                const int mx=lumMax(i); const int dist=colorDist(i,br,bg,bb);
                if(mx<=70 || dist<=90){ bgmask[p]=1; stack.push_back({x,y}); }
            };
            for(UINT x=0;x<w;++x){ pushIfBg(x,0); if(h>1) pushIfBg(x,h-1); }
            for(UINT y=1;y+1<h;++y){ pushIfBg(0,y); if(w>1) pushIfBg(w-1,y); }
            while(!stack.empty()){
                auto [x,y]=stack.back(); stack.pop_back();
                const int dx[4]={1,-1,0,0}; const int dy[4]={0,0,1,-1};
                for(int d=0;d<4;++d){
                    int nx=(int)x+dx[d], ny=(int)y+dy[d];
                    if(nx<0||ny<0||nx>=(int)w||ny>=(int)h) continue;
                    size_t p=(size_t)ny*(size_t)w+(size_t)nx; if(bgmask[p]) continue;
                    size_t i=idx((UINT)nx,(UINT)ny); if(rgba[i+3]==0){ bgmask[p]=1; continue; }
                    const int mx=lumMax(i); const int dist=colorDist(i,br,bg,bb);
                    if(mx<=70 || dist<=90){ bgmask[p]=1; stack.push_back({(UINT)nx,(UINT)ny}); }
                }
            }
            for(UINT y=0;y<h;++y)for(UINT x=0;x<w;++x){ size_t p=(size_t)y*(size_t)w+(size_t)x; if(bgmask[p]) rgba[idx(x,y)+3u]=0; }

            // 2) remove pequenos blobs/bolinhas e mantem apenas o maior componente opaco.
            std::vector<int> comp((size_t)w*(size_t)h,-1); std::vector<int> compSize; compSize.reserve(32);
            int compId=0,bestComp=-1,bestSize=0;
            std::vector<UINT> q; q.reserve((size_t)w*(size_t)h/4u+16u);
            for(UINT sy=0;sy<h;++sy) for(UINT sx=0;sx<w;++sx){
                size_t p=(size_t)sy*(size_t)w+(size_t)sx; if(comp[p]!=-1) continue;
                size_t i=idx(sx,sy); if(rgba[i+3]<=24){ comp[p]=-2; continue; }
                comp[p]=compId; q.clear(); q.push_back((sy<<16)|sx); int count=0;
                for(size_t qi=0; qi<q.size(); ++qi){
                    UINT packed=q[qi]; UINT x=packed & 0xffffu, y=packed>>16; ++count;
                    for(int oy=-1; oy<=1; ++oy) for(int ox=-1; ox<=1; ++ox){
                        if(!ox && !oy) continue; int nx=(int)x+ox, ny=(int)y+oy;
                        if(nx<0||ny<0||nx>=(int)w||ny>=(int)h) continue;
                        size_t np=(size_t)ny*(size_t)w+(size_t)nx; if(comp[np]!=-1) continue;
                        size_t ni=idx((UINT)nx,(UINT)ny); if(rgba[ni+3]<=24){ comp[np]=-2; continue; }
                        comp[np]=compId; q.push_back((((UINT)ny)<<16)|((UINT)nx));
                    }
                }
                compSize.push_back(count); if(count>bestSize){ bestSize=count; bestComp=compId; } ++compId;
            }
            if(bestComp>=0){
                for(UINT y=0;y<h;++y)for(UINT x=0;x<w;++x){ size_t p=(size_t)y*(size_t)w+(size_t)x; if(comp[p]>=0 && comp[p]!=bestComp) rgba[idx(x,y)+3u]=0; }
            }
        }
        D3D11_TEXTURE2D_DESC td{};td.Width=w;td.Height=h;td.MipLevels=1;td.ArraySize=1;td.Format=DXGI_FORMAT_R8G8B8A8_UNORM;td.SampleDesc.Count=1;td.Usage=D3D11_USAGE_IMMUTABLE;td.BindFlags=D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA init{};init.pSysMem=rgba.data();init.SysMemPitch=stride;
        ID3D11Texture2D* tex=nullptr;ID3D11ShaderResourceView* srv=nullptr;
        hr=device->CreateTexture2D(&td,&init,&tex);
        if(SUCCEEDED(hr)) hr=device->CreateShaderResourceView(tex,nullptr,&srv);
        if(tex)tex->Release();
        if(FAILED(hr)||!srv) goto done;
        out.handle=srv;out.width=(int)w;out.height=(int)h;
        // Recorta somente margem realmente transparente. Mantem 2 px de folga para filtragem linear.
        UINT minX=w,minY=h,maxX=0,maxY=0;bool any=false;
        for(UINT y=0;y<h;++y)for(UINT x=0;x<w;++x){
            const unsigned char a=rgba[((size_t)y*w+x)*4u+3u];if(a<=6)continue;
            any=true;minX=std::min(minX,x);minY=std::min(minY,y);maxX=std::max(maxX,x);maxY=std::max(maxY,y);
        }
        if(any){
            minX=minX>2?minX-2:0;minY=minY>2?minY-2:0;maxX=std::min(w-1,maxX+2);maxY=std::min(h-1,maxY+2);
            out.u0=(float)minX/(float)w;out.v0=(float)minY/(float)h;
            out.u1=(float)(maxX+1)/(float)w;out.v1=(float)(maxY+1)/(float)h;
        }
    }
done:
    if(converter)converter->Release();if(frame)frame->Release();if(decoder)decoder->Release();if(factory)factory->Release();
    return out;
}
void UnloadTexture(Texture2D& t){if(t.handle){reinterpret_cast<ID3D11ShaderResourceView*>(t.handle)->Release();}t={};}
bool IsTextureReady(const Texture2D& t){return t.handle!=nullptr&&t.width>0&&t.height>0;}
float TextureContentAspect(const Texture2D& t){
    if(!IsTextureReady(t))return 1.0f;
    const float cw=std::max(1.0f,(t.u1-t.u0)*(float)t.width),ch=std::max(1.0f,(t.v1-t.v0)*(float)t.height);return cw/ch;
}
void DrawTextureCentered(const Texture2D& t,Vector2 center,float maxWidth,float maxHeight,float rotationDeg,Color tint){
    if(!IsTextureReady(t))return;
    float aspect=TextureContentAspect(t),w=std::max(0.01f,maxWidth),h=std::max(0.01f,maxHeight);
    if(w/h>aspect)w=h*aspect;else h=w/aspect;
    center=winshim::worldToScreen(center);w=winshim::scaleLength(w);h=winshim::scaleLength(h);
    winshim::addSpriteScreen(reinterpret_cast<ID3D11ShaderResourceView*>(t.handle),center,w,h,rotationDeg,tint,t.u0,t.v0,t.u1,t.v1);
}

int MeasureText(const char* s,int fs){using namespace winshim;if(!s||!measureDC)return s?(int)std::strlen(s)*fs/2:0;HFONT f=cachedFont(fs);HGDIOBJ old=SelectObject_(measureDC,f);SIZE z{};GetTextExtentPoint32A_(measureDC,s,(int)std::strlen(s),&z);SelectObject_(measureDC,old);return (int)z.cx;}
void DrawText(const char* s,int x,int y,int fs,Color c){using namespace winshim;if(!s)return;TextCommand t{};t.text=s;t.x=x;t.y=y;t.size=fs;t.color=c;t.scissor=currentScissor;if(currentScissor){t.clip.left=currentScissorRect.left;t.clip.top=currentScissorRect.top;t.clip.right=currentScissorRect.right;t.clip.bottom=currentScissorRect.bottom;}textCommands.push_back(std::move(t));}
void BeginScissorMode(int x,int y,int w,int h){D3D11_RECT r{std::max(0,x),std::max(0,y),std::min(GetScreenWidth(),x+w),std::min(GetScreenHeight(),y+h)};winshim::setScissor(true,r);}
void EndScissorMode(){winshim::setScissor(false);}

#else
// Fallback apenas para eu conseguir validar a sintaxe do arquivo fora do Windows.
// No seu PC, a parte acima (_WIN32) é a usada.
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <algorithm>
#include <cmath>
struct Vector2 { float x=0,y=0; };
struct Color { unsigned char r=0,g=0,b=0,a=255; };
struct RectF { float x=0,y=0,width=0,height=0; };
struct Camera2D { Vector2 offset{},target{}; float rotation=0,zoom=1; };
struct Texture2D { void* handle=nullptr; int width=0,height=0; float u0=0,v0=0,u1=1,v1=1; };
constexpr float PI=3.14159265358979323846f;
constexpr float DEG2RAD=PI/180.0f;
constexpr int FLAG_WINDOW_RESIZABLE=1,FLAG_VSYNC_HINT=2,FLAG_MSAA_4X_HINT=4;
constexpr int KEY_R='R',KEY_H='H',KEY_X='X',KEY_M='M',KEY_P='P',KEY_ONE='1',KEY_TWO='2',KEY_FIVE='5',KEY_ZERO='0',KEY_SPACE=32,KEY_BACKSPACE=8,KEY_ENTER=13,KEY_F5=0x74,KEY_F9=0x78,KEY_F11=0x7A,KEY_UP=0x26,KEY_DOWN=0x28,KEY_LEFT=0x25,KEY_RIGHT=0x27,KEY_F='F',MOUSE_BUTTON_LEFT=0,MOUSE_BUTTON_RIGHT=1;
constexpr Color WHITE{255,255,255,255},RAYWHITE{245,245,245,255},RED{230,41,55,255},YELLOW{253,249,0,255},GRAY{130,130,130,255},SKYBLUE{102,191,255,255},ORANGE{255,161,0,255};
void SetConfigFlags(int){} void InitWindow(int,int,const char*){} void SetTargetFPS(int){} bool IsWindowFullscreen(){return false;} void ToggleFullscreen(){} bool WindowShouldClose(){return true;} void CloseWindow(){}
unsigned int GetRenderDrawCalls(){return 0;} unsigned int GetRenderVertexCount(){return 0;} std::size_t GetRenderCpuBufferBytes(){return 0;} std::size_t GetRenderGpuVertexBufferBytes(){return 0;}
int GetScreenWidth(){return 1280;} int GetScreenHeight(){return 760;} float GetFrameTime(){return 1.0f/60;} int GetFPS(){return 60;} double GetTime(){return 0;}
Vector2 GetMousePosition(){return{};} Vector2 GetMouseDelta(){return{};} float GetMouseWheelMove(){return 0;} bool IsKeyPressed(int){return false;} bool IsKeyDown(int){return false;} bool IsMouseButtonDown(int){return false;} bool IsMouseButtonPressed(int){return false;} bool IsMouseButtonReleased(int){return false;}
const std::string& GetTextInputChars(){static const std::string empty;return empty;}
Color Fade(Color c,float a){c.a=(unsigned char)(std::clamp(a,0.0f,1.0f)*255);return c;}
const char* TextFormat(const char* f,...){static char b[2048];va_list a;va_start(a,f);std::vsnprintf(b,sizeof(b),f,a);va_end(a);return b;}
void BeginDrawing(){} void EndDrawing(){} void BeginMode2D(Camera2D){} void EndMode2D(){}
Vector2 GetScreenToWorld2D(Vector2 p,Camera2D c){return{c.target.x+(p.x-c.offset.x)/c.zoom,c.target.y+(p.y-c.offset.y)/c.zoom};}
void ClearBackground(Color){} bool CheckCollisionPointRec(Vector2 p,RectF r){return p.x>=r.x&&p.y>=r.y&&p.x<=r.x+r.width&&p.y<=r.y+r.height;}
void DrawRectangle(int,int,int,int,Color){} void DrawRectangleV(Vector2,Vector2,Color){} void DrawRectangleLinesEx(RectF,float,Color){}
void DrawRectangleRounded(RectF,float,int,Color){} void DrawRectangleRoundedLines(RectF,float,int,float,Color){} void DrawLineEx(Vector2,Vector2,float,Color){}
void DrawCircleV(Vector2,float,Color){} void DrawCircleLinesV(Vector2,float,Color){} void DrawCircleAlphaV(Vector2,float,Color,Color,float){} void DrawTriangle(Vector2,Vector2,Vector2,Color){}
void DrawRectanglePro(RectF,Vector2,float,Color){}
Texture2D LoadTexture(const char*){return{};} void UnloadTexture(Texture2D&){} bool IsTextureReady(const Texture2D&){return false;} float TextureContentAspect(const Texture2D&){return 1.0f;} void DrawTextureCentered(const Texture2D&,Vector2,float,float,float,Color){}
int MeasureText(const char* s,int fs){return (int)std::strlen(s)*fs/2;} void DrawText(const char*,int,int,int,Color){}
void BeginScissorMode(int,int,int,int){} void EndScissorMode(){}
#endif


#include <fstream>
#include <type_traits>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <numeric>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <new>
#include <cstddef>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <filesystem>
#include <vector>
#include <csignal>

struct ProcessTelemetry {
    double cpuPercent = 0.0;          // normalizado para 0..100% da maquina
    double workingSetMB = 0.0;        // RAM fisica residente real do processo
    double privateMB = 0.0;           // memoria privada comprometida real
    double pagefileMB = 0.0;
    double systemRamUsedPercent = 0.0;
    unsigned logicalProcessors = 1;
    unsigned long handleCount = 0;
};

ProcessTelemetry readProcessTelemetry(){
    static ProcessTelemetry out{};
#ifdef _WIN32
    using GetProcessMemoryInfoFn = BOOL (WINAPI*)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
    static GetProcessMemoryInfoFn getMemoryInfo = [](){
        HMODULE m=LoadLibraryA("psapi.dll");
        return m?reinterpret_cast<GetProcessMemoryInfoFn>(GetProcAddress(m,"GetProcessMemoryInfo")):nullptr;
    }();
    HANDLE proc=GetCurrentProcess();
    if(getMemoryInfo){
        PROCESS_MEMORY_COUNTERS_EX pmc{};pmc.cb=sizeof(pmc);
        if(getMemoryInfo(proc,reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc),sizeof(pmc))){
            constexpr double MB=1024.0*1024.0;
            out.workingSetMB=double(pmc.WorkingSetSize)/MB;
            out.privateMB=double(pmc.PrivateUsage)/MB;
            out.pagefileMB=double(pmc.PagefileUsage)/MB;
        }
    }
    SYSTEM_INFO si{};GetSystemInfo(&si);out.logicalProcessors=std::max<DWORD>(1,si.dwNumberOfProcessors);
    DWORD handles=0;if(GetProcessHandleCount(proc,&handles))out.handleCount=handles;
    MEMORYSTATUSEX ms{};ms.dwLength=sizeof(ms);if(GlobalMemoryStatusEx(&ms))out.systemRamUsedPercent=(double)ms.dwMemoryLoad;

    static ULONGLONG prevCpu100ns=0;
    static auto prevWall=std::chrono::steady_clock::now();
    static bool initialized=false;
    FILETIME create{},exit{},kernel{},user{};
    if(GetProcessTimes(proc,&create,&exit,&kernel,&user)){
        ULARGE_INTEGER k{},u{};k.LowPart=kernel.dwLowDateTime;k.HighPart=kernel.dwHighDateTime;u.LowPart=user.dwLowDateTime;u.HighPart=user.dwHighDateTime;
        const ULONGLONG cpu100ns=k.QuadPart+u.QuadPart;
        const auto now=std::chrono::steady_clock::now();
        const double wall=std::chrono::duration<double>(now-prevWall).count();
        if(initialized && wall>=0.20){
            const double cpuSec=double(cpu100ns-prevCpu100ns)*1e-7;
            out.cpuPercent=std::clamp(100.0*cpuSec/(wall*double(std::max(1u,out.logicalProcessors))),0.0,100.0);
            prevCpu100ns=cpu100ns;prevWall=now;
        }else if(!initialized){prevCpu100ns=cpu100ns;prevWall=now;initialized=true;}
    }
#else
    out.logicalProcessors=std::max(1u,std::thread::hardware_concurrency());
#endif
    return out;
}

// [SEC-CONFIG] Constantes globais e Tuning configuravel
namespace cfg {
// v0.135: mundo toroidal. O retangulo e apenas a representacao fundamental;
// esquerda/direita e cima/baixo sao a mesma vizinhanca periodica.
inline float WORLD_W = 2000.0f;
inline float WORLD_H = 2000.0f;
constexpr float WORLD_MIN_SIZE = 50.0f;
constexpr float WORLD_MAX_SIZE = 10000.0f;
constexpr float WORLD_SIZE_STEP = 50.0f;
constexpr float CELL = 50.0f;
constexpr int MIN_RAYS = 1;
constexpr int MAX_RAYS = 31;
constexpr float VISION_MIN_RANGE = 40.0f;
constexpr float VISION_MAX_RANGE = 5000.0f;
constexpr float VISION_MAX_FOV = 360.0f;
constexpr float VISION_MIN_FOV = 6.0f;
// Cada raio ativo tem custo metabolico continuo. Alcance nao custa energia diretamente:
// o trade-off do alcance e perder campo de visao; densidade de raios custa energia.
constexpr float VISION_RAY_ENERGY_COST_PER_SECOND = 0.035f;
constexpr int INPUTS_PER_RAY = 11; // distancia + 4 tipos + planta-solta + RGB + afinidade + movimento da copa
// 24 entradas historicas permanecem apenas como padding zero para nao deslocar
// os indices das redes neurais existentes em saves antigos. Nao existe sensor associado.
constexpr int LEGACY_SENSOR_SECTORS = 8;
constexpr int LEGACY_SENSOR_BANDS = 3;
constexpr int LEGACY_SENSOR_PADDING_INPUTS = LEGACY_SENSOR_SECTORS * LEGACY_SENSOR_BANDS;
constexpr int BASE_INTERNAL_INPUTS = 31; // corpo/estado + feedback fisico/tatil + estado de carga
// v0.135 reutiliza parte dos 24 slots historicos (que eram sempre zero) para uma
// PERCEPCAO CONTINUA derivada dos raycasts. Os indices antigos nao se movem.
// Um objeto so entra aqui se algum raio realmente o enxergou; portanto nao existe
// conhecimento magico do mapa. Direcao e -1 esquerda .. +1 direita e proximidade
// e 0 longe .. 1 perto. O contato da boca com planta e um sensor tatil local.
constexpr int PERCEPTION_INPUT_BASE = MAX_RAYS * INPUTS_PER_RAY + BASE_INTERNAL_INPUTS;
constexpr int PERCEPT_PLANT_PRESENT   = PERCEPTION_INPUT_BASE + 0;
constexpr int PERCEPT_PLANT_DIRECTION = PERCEPTION_INPUT_BASE + 1;
constexpr int PERCEPT_PLANT_NEARNESS  = PERCEPTION_INPUT_BASE + 2;
constexpr int PERCEPT_PLANT_MOUTH_CONTACT = PERCEPTION_INPUT_BASE + 3;
constexpr int PERCEPT_CREATURE_PRESENT   = PERCEPTION_INPUT_BASE + 4;
constexpr int PERCEPT_CREATURE_DIRECTION = PERCEPTION_INPUT_BASE + 5;
constexpr int PERCEPT_CREATURE_NEARNESS  = PERCEPTION_INPUT_BASE + 6;
constexpr int PERCEPT_CARCASS_PRESENT   = PERCEPTION_INPUT_BASE + 7;
constexpr int PERCEPT_CARCASS_DIRECTION = PERCEPTION_INPUT_BASE + 8;
constexpr int PERCEPT_CARCASS_NEARNESS  = PERCEPTION_INPUT_BASE + 9;
constexpr int PERCEPT_EGG_PRESENT   = PERCEPTION_INPUT_BASE + 10;
constexpr int PERCEPT_EGG_DIRECTION = PERCEPTION_INPUT_BASE + 11;
constexpr int PERCEPT_EGG_NEARNESS  = PERCEPTION_INPUT_BASE + 12;
constexpr int PERCEPTION_USED_INPUTS = 13;
constexpr int INTERNAL_INPUTS = BASE_INTERNAL_INPUTS + LEGACY_SENSOR_PADDING_INPUTS;
constexpr int MAX_EMOTIONS = 8; // reserva estrutural; quantidade efetiva pertence ao DNA
constexpr int BASE_INPUT_COUNT = MAX_RAYS * INPUTS_PER_RAY + INTERNAL_INPUTS;
// IMPORTANTE: emocoes permanecem exatamente nos indices antigos para que saves/redes
// anteriores continuem apontando para os mesmos sinais.
constexpr int EMOTION_INPUT_BASE = BASE_INPUT_COUNT;
// Memoria episodica e anexada DEPOIS das emocoes. Assim nenhuma conexao antiga muda de significado.
constexpr int LONG_MEMORY_SLOTS = 4;
constexpr int MEMORY_EVENT_TYPES = 5; // 4 eventos ativos + 1 slot legado reservado
constexpr int MEMORY_FEATURES_PER_SLOT = MEMORY_EVENT_TYPES + 5; // tipos + intensidade + idade + dir X/Y + validade
constexpr int MEMORY_INPUTS = LONG_MEMORY_SLOTS * MEMORY_FEATURES_PER_SLOT;
constexpr int MEMORY_INPUT_BASE = BASE_INPUT_COUNT + MAX_EMOTIONS;
// v0.109: memoria "papel e caneta" controlada pelo proprio cerebro. Diferente da
// memoria episodica acima: estes valores nao decaem e so mudam quando uma saida gate manda escrever.
constexpr int REGISTER_MEMORY_SLOTS = 4;
constexpr int REGISTER_MEMORY_INPUT_BASE = MEMORY_INPUT_BASE + MEMORY_INPUTS;
constexpr int INPUT_COUNT = REGISTER_MEMORY_INPUT_BASE + REGISTER_MEMORY_SLOTS;
constexpr int MOTOR_OUTPUT_COUNT = 7; // frente/re/giro E-D/lateral E-D/agarrar
constexpr int GRAB_OUTPUT_INDEX = 6;
// Tres slots sociais historicos preservam indices antigos. Os dois primeiros ficam
// zerados; o terceiro e hoje a saida independente ACASALAR.
constexpr int LEGACY_SOCIAL_OUTPUT_COUNT = 3;
constexpr int LEGACY_SOCIAL_OUTPUT_BASE = MOTOR_OUTPUT_COUNT;
// v0.135 reaproveita o primeiro slot historicamente inerte como MORDE. O segundo
// continua reservado; o terceiro permanece ACASALAR. Assim nao deslocamos outputs antigos.
constexpr int BITE_INTENT_OUTPUT_INDEX = LEGACY_SOCIAL_OUTPUT_BASE + 0;
constexpr int LEGACY_RESERVED_OUTPUT_INDEX = LEGACY_SOCIAL_OUTPUT_BASE + 1;
constexpr int MATE_INTENT_OUTPUT_INDEX = LEGACY_SOCIAL_OUTPUT_BASE + 2;
constexpr int COLOR_OUTPUT_COUNT = 3;
constexpr int COLOR_OUTPUT_BASE = MOTOR_OUTPUT_COUNT + LEGACY_SOCIAL_OUTPUT_COUNT;
constexpr int LEGACY_OUTPUT_COUNT = MOTOR_OUTPUT_COUNT + LEGACY_SOCIAL_OUTPUT_COUNT + COLOR_OUTPUT_COUNT; // v0.107 e anteriores
constexpr int MEMORY_WRITE_OUTPUT_BASE = LEGACY_OUTPUT_COUNT;
constexpr int MEMORY_GATE_OUTPUT_BASE = MEMORY_WRITE_OUTPUT_BASE + REGISTER_MEMORY_SLOTS;
constexpr int OUTPUT_COUNT = MEMORY_GATE_OUTPUT_BASE + REGISTER_MEMORY_SLOTS;
// Um float extra em NeuralGenome::outputBiases guarda a capacidade genetica dos registradores.
// outputBiases e vetor serializado, entao saves antigos simplesmente chegam sem este metadado.
constexpr int BRAIN_META_MEMORY_CAPACITY_INDEX = OUTPUT_COUNT;
constexpr int BRAIN_OUTPUT_STORAGE_COUNT = OUTPUT_COUNT + 1;
constexpr int MAX_SPECIES = 32;
constexpr int MAX_EVOLUTION_HISTORY = 720;
constexpr int DEFAULT_HIDDEN = 12;
constexpr int MIN_HIDDEN = 4;
constexpr int MAX_HIDDEN = 48;
constexpr int MAX_BRAIN_LAYERS = 6;
constexpr float BRAIN_INTERVAL = 0.20f;

// Metade dos fundadores/criados manualmente recebe um circuito inicial simples
// de perseguicao. Ele e gravado na propria rede neural (portanto pode ser
// herdado, mutado ou perdido), em vez de existir como if/else permanente.
constexpr float FOUNDER_SEEK_BIAS_CHANCE = 0.0f;
constexpr float FOUNDER_SEEK_PLANT_CHANCE = 0.95f;
constexpr float FOUNDER_SEEK_CARCASS_CHANCE = 0.04f;
// O 1% restante procura outro bicho.

// Estado linear do cerebro. Conexoes compiladas guardam apenas um indice global,
// eliminando if/else por tipo de origem dentro do loop multiply-accumulate.
constexpr int BRAIN_INPUT_BASE = 0;
constexpr int BRAIN_HIDDEN_BASE = BRAIN_INPUT_BASE + INPUT_COUNT;
constexpr int BRAIN_PREV_HIDDEN_BASE = BRAIN_HIDDEN_BASE + MAX_HIDDEN;
constexpr int BRAIN_PREV_OUTPUT_BASE = BRAIN_PREV_HIDDEN_BASE + MAX_HIDDEN;
constexpr int BRAIN_STATE_COUNT = BRAIN_PREV_OUTPUT_BASE + OUTPUT_COUNT;

constexpr float MAX_SUBSTEP = 1.0f / 60.0f; // sensores/fisica ~60 Hz por segundo simulado

constexpr float BASE_CREATURE_SIZE = 20.0f;
constexpr float MIN_CREATURE_SIZE = 2.0f;
constexpr float MAX_CREATURE_SIZE = 36.0f;
constexpr float FOUNDER_MIN_SIZE = 2.0f;
constexpr float FOUNDER_MAX_SIZE = 10.0f;

constexpr float ENERGY_MAX_BASE = 100.0f;
constexpr float ENERGY_INITIAL_BASE = 55.0f;
constexpr float MINIMUM_FOUNDER_ENERGY_RATIO = 0.82f; // sementes artificiais do minimo com reserva para amadurecer
constexpr float MINIMUM_FOUNDER_NEAR_FOOD_MIN = 24.0f;
constexpr float MINIMUM_FOUNDER_NEAR_FOOD_MAX = 90.0f;
// Metabolismo basal reduzido para 20% do valor anterior (0.55 -> 0.11).
// Custos de visao, movimento, giro etc. permanecem inalterados.
constexpr float METABOLISM = 0.11f;
constexpr float FORWARD_COST = 1.70f; // -50%: custo ativo de avancar
constexpr float REVERSE_COST = 1.40f; // -50%: custo ativo de re
constexpr float TURN_COST = 0.40f; // -50%: custo ativo de giro
constexpr float LATERAL_COST = 1.55f; // deslocamento de lado sem girar o corpo
constexpr float MOTION_COST = 1.30f; // -50%: custo por velocidade efetiva; continua quadratico
constexpr float PASSIVE_DRAG_SCALE = 0.030f; // friccao basal mais leve; energia passa a ser o principal custo de correr
constexpr float BITE_COST = 0.175f; // -50%: custo de acao da mordida
constexpr float HEALTH_MAX = 100.0f;
constexpr float STARVATION_THRESHOLD = 0.10f;
constexpr float STARVATION_DAMAGE = 7.0f;
// Regeneracao de sangue: comeca acima de 50% da energia pessoal.
// A curva e quadratica: perto do limiar quase nao cura; perto de 100% acelera bastante.
constexpr float HEALTH_REGEN_ENERGY_THRESHOLD = 0.50f;
constexpr float HEALTH_REGEN_MAX_PER_SECOND = 2.0f;

constexpr float REPRO_MIN_RATIO = 0.80f;
constexpr float REPRO_COOLDOWN = 10.0f;
constexpr float CREATURE_BIRTH_GAP = 0.85f;
constexpr float CREATURE_BIRTH_EXTRA_RADIUS = 4.0f;
constexpr int CREATURE_REPRO_ATTEMPTS = 36;

// Genes de investimento parental. O custo e retirado diretamente do pai.
// A referencia define quanto recurso no ovo equivale a nascer com barra cheia.
constexpr float EGG_ENERGY_GENE_MIN = 0.12f;
constexpr float EGG_ENERGY_GENE_MAX = 0.45f;
constexpr float EGG_BLOOD_GENE_MIN = 0.08f;
constexpr float EGG_BLOOD_GENE_MAX = 0.35f;
constexpr float EGG_ENERGY_FULL_BIRTH_FRACTION = 0.45f;
constexpr float EGG_BLOOD_FULL_BIRTH_FRACTION = 0.35f;
constexpr float EGG_INCUBATION_MIN = 6.0f;
constexpr float EGG_INCUBATION_MAX = 14.0f;
constexpr float EGG_BLOOD_FOOD_VALUE = 0.35f;
constexpr float EGG_MIN_RESOURCE = 0.05f;

// Vegetacao aplica arrasto extra sem alterar o gene de velocidade.
// Cada planta sobreposta aumenta a friccao, com teto para evitar extremos.
constexpr float VEGETATION_FRICTION_PER_PLANT = 0.45f;
constexpr int VEGETATION_FRICTION_MAX_PLANTS = 4;

// Fisica simples de circulos. A velocidade do motor do bicho continua separada
// do impulso recebido por colisoes, permitindo empurroes laterais sem "virar" o animal.
constexpr float PHYSICS_RESTITUTION = 0.06f;
constexpr float PHYSICS_POSITION_CORRECTION = 0.86f;
constexpr float PHYSICS_SLOP = 0.015f;
constexpr int PHYSICS_SOLVER_ITERATIONS = 2;
constexpr float PHYSICS_CREATURE_PUSH_DAMPING = 4.0f;
constexpr float PHYSICS_OBJECT_DAMPING = 1.8f;
constexpr float PHYSICS_MAX_PUSH_SPEED = 85.0f;
constexpr float BITE_PLANT_KICK = 12.0f;

// Massa 2D proporcional a area: dobrar o diametro torna o corpo ~4x mais pesado.
// Densidades diferentes preservam a sensacao de material sem quebrar a regra tamanho->massa.
constexpr float CREATURE_MASS_DENSITY = 0.78f;
constexpr float CARCASS_MASS_DENSITY = 0.66f;
constexpr float PLANT_MASS_DENSITY = 0.22f;

constexpr float PLANT_BIRTH_SIZE = 1.0f;
// Fragmentos vegetais nunca sao desenhados menores que 1x1. A biomassa pode
// continuar caindo internamente abaixo de 1 para permitir um desaparecimento suave,
// mas fisica/render permanecem no minimo em 1 ate o objeto ser removido.
constexpr float PLANT_MIN_FRAGMENT_SIZE = 1.0f;
// Quanto o tamanho atual protege contra mordidas. 1.0 = resistencia linear:
// tamanho 10 recebe ~1/10 da remocao de uma planta tamanho 1 para a mesma mordida.
constexpr float PLANT_BITE_SIZE_RESISTANCE_EXPONENT = 0.55f;
// Plantas grandes acompanham a biomassa visualmente mais devagar.
constexpr float PLANT_SHRINK_SIZE_SLOWDOWN_EXPONENT = 0.50f;
// Durante decomposicao, o processo acelera conforme o fragmento fica menor.
constexpr float PLANT_DECAY_SMALL_ACCEL_EXPONENT = 0.50f;
constexpr float PLANT_MAX_SIZE = 10.0f;
constexpr float PLANT_GROWTH = 15.0f;
constexpr float PLANT_GROWTH_JITTER = 1.05f;
constexpr float PLANT_REPRO = 15.0f;
constexpr float PLANT_REPRO_JITTER = 1.95f;
constexpr float PLANT_MIN_SPACING = 3.6f;
constexpr float PLANT_OVERLAP_FACTOR = 0.60f;
constexpr float PLANT_CHILD_MIN_DIST = 4.0f;
constexpr float PLANT_CHILD_MAX_DIST = 31.0f;
constexpr int PLANT_REPRO_ATTEMPTS = 24;
constexpr float PLANT_FULL_ENERGY = 20.0f;
// Sistema de afinidade cromatica restaurado da versao web pre-v0.030.
constexpr float PLANT_MIN_COLOR_ENERGY_EFFICIENCY = 0.60f;
constexpr float PLANT_POISON_STARTS_BELOW_SIMILARITY = 0.55f;
constexpr float PLANT_MAX_POISON_DAMAGE = 10.0f;
constexpr float PLANT_POISON_EXPONENT = 2.0f;

// Carne continua visualmente vermelha. A similaridade do corpo do bicho com
// esse vermelho define quanto da energia ingerida ele consegue aproveitar.
constexpr Color CARCASS_COLOR{220,45,40,255};
constexpr float CARCASS_MIN_COLOR_ENERGY_EFFICIENCY = 0.45f;

// =========================================================
// AJUSTES DE BALANCEAMENTO EM TEMPO REAL (v0.060)
// =========================================================
// Os valores abaixo sao multiplicadores sobre as constantes-base.
// 1.00 = 100% do valor-base, 0.30 = 30%, 2.00 = 200%.
//
// Importante: a friccao genetica continua pertencendo ao DNA de cada bicho.
// O multiplicador global apenas escala o efeito fisico de todos os genes,
// preservando as diferencas evolutivas entre individuos.
struct RuntimeTuning {
    // Custos / movimento (multiplicadores das constantes-base)
    float globalFriction = 1.00f;
    float passiveDrag = 1.00f;
    float vegetationFriction = 1.00f;
    float metabolismCost = 1.00f;
    float healthRegen = 1.00f;
    float forwardCost = 0.30f;
    float reverseCost = 0.30f;
    float turnCost = 0.30f;
    float lateralCost = 0.30f;
    // Usar dois eixos de translacao ao mesmo tempo continua permitido, mas exige mais do corpo.
    // combo = intensidade longitudinal * intensidade lateral (0..1).
    float multiAxisEnergyPenalty = 0.90f;
    float multiAxisForcePenalty = 0.45f;
    float motionCost = 0.30f;
    float biteCost = 1.00f;
    float visionCost = 1.00f;

    // Energia / vida
    float energyMaxBase = ENERGY_MAX_BASE;
    float energyInitialBase = ENERGY_INITIAL_BASE;
    float healthMax = HEALTH_MAX;
    float starvationThreshold = STARVATION_THRESHOLD;
    float starvationDamage = STARVATION_DAMAGE;
    float healthRegenThreshold = HEALTH_REGEN_ENERGY_THRESHOLD;
    float healthRegenMaxPerSecond = HEALTH_REGEN_MAX_PER_SECOND;

    // Visao: limites efetivos dentro dos limites estruturais compilados.
    float visionMinRange = VISION_MIN_RANGE;
    float visionMaxRange = VISION_MAX_RANGE;
    float visionMaxFov = VISION_MAX_FOV;
    float visionMinFov = VISION_MIN_FOV;
    float minRaysEffective = (float)MIN_RAYS;
    float maxRaysEffective = (float)MAX_RAYS;
    float founderMinRays = 2.0f;
    float founderMaxRays = 12.0f;
    float founderVisionMinRange = 80.0f;
    float founderVisionMaxRange = 1800.0f;
    float visionFovCurveExponent = 0.82f;
    float visionRayEnergyCost = VISION_RAY_ENERGY_COST_PER_SECOND;
    // Visao evolutiva mais rica: distribuicao dos raios, assimetria, sensibilidade RGB e eficiencia energetica.
    float founderVisionFocusMin = 0.80f, founderVisionFocusMax = 1.25f;
    float visionFocusMin = 0.30f, visionFocusMax = 3.50f;
    float visionAsymmetryMax = 0.80f;
    float visionColorSensitivityMin = 0.15f, visionColorSensitivityMax = 1.75f;
    float visionEfficiencyMin = 0.45f, visionEfficiencyMax = 1.60f;
    float visionTraitMutationChance = 0.07f, visionTraitMutationStrength = 0.10f;

    // Corpo / fundadores / limites geneticos
    float baseCreatureSize = BASE_CREATURE_SIZE;
    float minCreatureSize = MIN_CREATURE_SIZE;
    float maxCreatureSize = MAX_CREATURE_SIZE;
    float founderMinSize = FOUNDER_MIN_SIZE;
    float founderMaxSize = FOUNDER_MAX_SIZE;
    float founderForwardSpeed = 22.5f;
    float founderReverseSpeed = 13.75f;
    float founderLateralSpeedRatio = 0.75f; // velocidade lateral = fracao da velocidade frontal
    float founderAcceleration = 100.0f;
    float founderFriction = 14.4f;
    float founderRotation = 120.0f;
    float founderVariationMin = 0.65f;
    float founderVariationMax = 1.35f;
    float geneForwardMin = 4.5f, geneForwardMax = 75.0f;
    float geneReverseMin = 2.5f, geneReverseMax = 50.0f;
    float geneLateralRatioMin = 0.35f, geneLateralRatioMax = 0.95f;
    float geneAccelerationMin = 15.0f, geneAccelerationMax = 350.0f;
    float geneFrictionMin = 3.2f, geneFrictionMax = 52.0f;
    float geneRotationMin = 15.0f, geneRotationMax = 350.0f;
    float geneHardnessMin = 0.05f, geneHardnessMax = 1.0f;

    // Agarrar/carregar. A capacidade usa a massa do proprio bicho como referencia.
    float grabOutputThreshold = 0.55f;
    float grabReachBodyScale = 0.70f;
    float grabReachExtra = 3.0f;
    float grabMaxLoadMassRatio = 1.35f;
    float grabHoldEnergyCost = 0.10f;
    float grabMoveEnergyCost = 0.38f;
    float grabSpeedPenalty = 0.65f;
    float grabAccelerationPenalty = 0.75f;
    float grabGap = 0.35f;

    // Regras derivadas de corpo/tamanho. Estes parametros controlam as formulas,
    // nao apenas os limites dos genes.
    float energySizeExponent = 1.25f;
    float sizeSpeedExponent = 0.50f, sizeSpeedMin = 0.45f, sizeSpeedMax = 1.90f;
    float sizeAccelExponent = 0.38f, sizeAccelMin = 0.50f, sizeAccelMax = 1.80f;
    float sizeRotationExponent = 0.34f, sizeRotationMin = 0.55f, sizeRotationMax = 1.70f;
    float sizeEnergyCostExponent = 0.55f, sizeEnergyCostMin = 0.55f, sizeEnergyCostMax = 2.00f;
    float sizeBiteExponent = 0.20f, sizeBiteMin = 0.88f, sizeBiteMax = 1.16f;
    float mouthSizeFactorMin = 0.10f, mouthSizeFactorMax = 1.80f;
    float hardnessSpeedPenalty = 0.60f, hardnessSpeedMinMult = 0.25f;
    float hardnessDamageReduction = 0.65f, hardnessDamageMinMult = 0.20f;
    float hardnessMassBonus = 0.28f;
    float creatureMassSizeExponent = 2.0f, creatureMassMin = 0.50f;

    // Ciclo de vida / desenvolvimento genetico. physical.bodySize continua sendo
    // o tamanho ADULTO alvo; Creature::size cresce do nascimento ate esse alvo.
    float founderBirthSizeFractionMin = 0.28f, founderBirthSizeFractionMax = 0.48f;
    float birthSizeFractionMin = 0.10f, birthSizeFractionMax = 0.80f;
    float founderGrowthDurationMin = 16.0f, founderGrowthDurationMax = 36.0f;
    float growthDurationMin = 4.0f, growthDurationMax = 180.0f;
    float founderGrowthCurveMin = 0.75f, founderGrowthCurveMax = 1.35f;
    float growthCurveMin = 0.15f, growthCurveMax = 4.0f;
    float founderAdolescenceStartMin = 0.30f, founderAdolescenceStartMax = 0.48f;
    float adolescenceStartMin = 0.05f, adolescenceStartMax = 0.90f;
    float founderMaturityFractionMin = 0.62f, founderMaturityFractionMax = 0.82f;
    float maturityFractionMin = 0.10f, maturityFractionMax = 1.20f;
    float founderLongevityMin = 140.0f, founderLongevityMax = 260.0f;
    float longevityMin = 20.0f, longevityMax = 1200.0f;
    float founderSenescenceFractionMin = 0.70f, founderSenescenceFractionMax = 0.84f;
    float senescenceFractionMin = 0.20f, senescenceFractionMax = 0.99f;
    float lifecycleMutationChance = 0.10f, lifecycleMutationStrength = 0.10f;

    // Regras globais do desenvolvimento. 0 no custo de crescimento = crescimento gratuito.
    float lifecycleUpdateInterval = 0.10f;
    float growthEnergyCostPerSize = 0.80f;
    float growthEnergyCostMultiplier = 1.00f;
    float oldAgeCurveExponent = 1.55f;
    float oldAgeSpeedMin = 0.58f;
    float oldAgeAccelMin = 0.62f;
    float oldAgeRotationMin = 0.66f;
    float oldAgeBiteMin = 0.68f;
    float oldAgeRegenMin = 0.22f;
    float oldAgeMetabolismMax = 1.40f;
    float oldAgeFertilityMin = 0.15f;
    float oldAgeReproThresholdPenalty = 0.12f;
    float oldAgeReproCooldownMaxMult = 3.0f;

    // Longevidade nao e beneficio gratis: vidas geneticamente maiores podem custar
    // mais manutencao basal. Peso 0 desliga completamente esse trade-off.
    float longevityReference = 180.0f;
    float longevityMetabolismWeight = 0.15f;
    float longevityMetabolismExponent = 0.70f;
    float longevityMetabolismMin = 0.70f;
    float longevityMetabolismMax = 1.80f;

    // Mortalidade natural: o risco inicia na senescencia e cresce continuamente.
    // hazard e probabilidade instantanea por segundo; 0 desliga morte por velhice.
    float naturalDeathHazardAtLongevity = 0.040f;
    float naturalDeathHazardExponent = 2.20f;
    float naturalDeathHazardMax = 0.70f;

    // Cor fenotipica derivada do DNA: pesos de cada bloco genetico.
    float colorPhysicalWeight = 0.28f, colorMouthWeight = 0.22f;
    float colorVisionWeight = 0.20f, colorBrainWeight = 0.30f;
    float colorLifecycleWeight = 0.18f;
    float colorRotationWeight = 0.65f, colorReverseWeight = 0.35f;
    float colorMinSeparation = 0.16f, colorMaxSeparationBoost = 2.40f;
    float colorByteFloor = 18.0f, colorByteRange = 219.0f;

    // v0.071 - cores biologicamente separadas. A boca define afinidade alimentar;
    // o corpo e um display controlavel pelo cerebro para futura comunicacao.
    float mouthColorMutationChance = 0.08f;
    float mouthColorMutationDelta = 18.0f;
    float founderBodyColorChannelChance = 0.0f; // v0.074: fundadores nao nascem com sinalizacao cromatica
    float bodyColorBaseMutationChance = 0.08f;
    float bodyColorBaseMutationDelta = 18.0f;
    float bodyColorChannelToggleChance = 0.005f; // por canal/geracao; capacidade de output cromatico e rara
    float bodyColorRangeMutationChance = 0.08f;
    float bodyColorRangeMutationStrength = 0.10f;
    float founderBodyColorSpeedMin = 0.45f;
    float founderBodyColorSpeedMax = 1.80f;
    float bodyColorSpeedMin = 0.05f;
    float bodyColorSpeedMax = 6.00f;
    float bodyColorSpeedMutationChance = 0.08f;
    float bodyColorSpeedMutationStrength = 0.16f;
    // Custos ficam 0 por padrao: podem ser ligados no editor sem impor trade-off agora.
    float bodyColorActiveChannelCost = 0.0f;
    float bodyColorChangeEnergyCost = 0.0f;

    // Boca: fundadores, limites geneticos, mutacao e formula da mordida.
    float founderMouthBiteMin = 0.40f, founderMouthBiteMax = 1.00f;
    float founderMouthOpeningMin = 0.35f, founderMouthOpeningMax = 1.00f;
    float founderMouthSpeedMin = 0.60f, founderMouthSpeedMax = 2.20f;
    float mouthBiteGeneMin = 0.10f, mouthBiteGeneMax = 1.50f;
    float mouthOpeningGeneMin = 0.15f, mouthOpeningGeneMax = 1.20f;
    float mouthSpeedGeneMin = 0.20f, mouthSpeedGeneMax = 3.50f;
    float mouthMutationChance = 0.10f, mouthMutationStrength = 0.12f;
    float mouthSpeedReference = 2.20f, mouthSpeedFactorMin = 0.20f, mouthSpeedFactorMax = 1.60f;
    float biteOpeningBase = 0.20f, biteOpeningWeight = 0.80f;
    float biteSpeedBase = 0.45f, biteSpeedWeight = 0.55f;
    float bitePowerMax = 1.50f;
    float biteCooldownSpeedFloor = 0.10f, biteCooldownMin = 0.18f, biteCooldownMax = 1.60f;
    float biteReachBase = 1.0f, biteReachOpeningScale = 7.0f;
    float biteRadiusBase = 1.5f, biteRadiusOpeningScale = 4.0f;
    float plantBiteBiomassBase = 10.0f, creatureBiteDamageBase = 34.0f;
    float biteCostPowerMin = 0.25f, biteCostPowerMax = 1.50f;

    // Reproducao / ovos
    float reproMinRatio = REPRO_MIN_RATIO;
    float reproCooldown = REPRO_COOLDOWN;
    float creatureBirthGap = CREATURE_BIRTH_GAP;
    float creatureBirthExtraRadius = CREATURE_BIRTH_EXTRA_RADIUS;
    float creatureReproAttempts = (float)CREATURE_REPRO_ATTEMPTS;
    float eggEnergyGeneMin = EGG_ENERGY_GENE_MIN;
    float eggEnergyGeneMax = EGG_ENERGY_GENE_MAX;
    float eggBloodGeneMin = EGG_BLOOD_GENE_MIN;
    float eggBloodGeneMax = EGG_BLOOD_GENE_MAX;
    float eggEnergyFullBirthFraction = EGG_ENERGY_FULL_BIRTH_FRACTION;
    float eggBloodFullBirthFraction = EGG_BLOOD_FULL_BIRTH_FRACTION;
    float eggIncubationMin = EGG_INCUBATION_MIN;
    float eggIncubationMax = EGG_INCUBATION_MAX;
    float eggBloodFoodValue = EGG_BLOOD_FOOD_VALUE;
    float eggMinResource = EGG_MIN_RESOURCE;
    float reproEnergySafetyRatio = 0.02f;
    float reproHealthSafety = 1.0f;
    float eggQualityEnergyWeight = 0.50f;
    float eggQualityBloodWeight = 0.50f;
    float eggRadiusBase = 1.0f, eggRadiusSizeScale = 0.58f, eggRadiusMin = 1.20f;
    float eggBiteRadiusScale = 0.90f, eggBiteMin = 0.75f, eggBitePowerMax = 1.50f;
    float eggMassSizeCoeff = 0.34f, eggMassSizeExponent = 2.0f;
    float eggMassEnergyCoeff = 0.20f, eggMassBloodCoeff = 0.10f, eggMassMin = 0.30f;
    // Selecao sexual: parceiro pode ser escolhido por distancia e aparencia.
    float founderMateRangeMin = 30.0f, founderMateRangeMax = 180.0f;
    float mateRangeMin = 10.0f, mateRangeMax = 800.0f;
    float mateTraitMutationChance = 0.08f, mateTraitMutationStrength = 0.12f;
    float mateChoiceNoise = 0.08f;
    float mateSpatialWeight = 0.55f, mateVisualWeight = 0.45f;

    // Plantas / ecologia
    // LEGADO de save/config: desde v0.102 nao limita mais a resistencia.
    // Todas as plantas realmente tocadas pelo corpo contam na soma.
    float vegetationFrictionMaxPlants = (float)VEGETATION_FRICTION_MAX_PLANTS;
    float plantBirthSize = PLANT_BIRTH_SIZE;
    // O tamanho historico (plantMaxSize) passa a ser o tamanho adulto MEDIO.
    // Cada planta possui um gene de escala; 1.0 = planta media.
    float plantMaxSize = PLANT_MAX_SIZE;
    float plantSizeScaleMin = 0.35f;
    float plantSizeScaleMax = 2.00f;
    float plantFounderSizeVariation = 0.12f;
    float plantSizeMutationChance = 0.10f;
    float plantSizeMutationStrength = 0.14f;
    float plantSizeExtremeResistance = 2.20f;
    float plantGrowth = PLANT_GROWTH;
    float plantGrowthJitter = PLANT_GROWTH_JITTER;
    float plantRepro = PLANT_REPRO;
    float plantReproJitter = PLANT_REPRO_JITTER;
    float plantMinSpacing = PLANT_MIN_SPACING;
    float plantOverlapFactor = PLANT_OVERLAP_FACTOR;
    float plantChildMinDist = PLANT_CHILD_MIN_DIST;
    float plantChildMaxDist = PLANT_CHILD_MAX_DIST;
    float plantReproAttempts = (float)PLANT_REPRO_ATTEMPTS;
    float plantFullEnergy = PLANT_FULL_ENERGY;
    // Expoente da resistencia da planta a mordida por tamanho. 0 = tamanho nao protege;
    // 1 = resistencia linear. O padrao 0.55 mantem plantas grandes mais demoradas,
    // sem transformar cada mordida grande em quase nenhuma biomassa.
    float plantBiteSizeResistanceExponent = PLANT_BITE_SIZE_RESISTANCE_EXPONENT;
    float plantMinColorEfficiency = PLANT_MIN_COLOR_ENERGY_EFFICIENCY;
    float plantPoisonSimilarity = PLANT_POISON_STARTS_BELOW_SIMILARITY;
    float plantMaxPoisonDamage = PLANT_MAX_POISON_DAMAGE;
    float plantPoisonExponent = PLANT_POISON_EXPONENT;
    float plantMateMutationChance = 0.09f;
    float plantColorMateMutationChance = 0.11f;
    float plantRgbMutationChance = 0.075f;
    float plantRgbMutationDelta = 16.0f;
    float plantPartnerRadiusScale = 10.0f;
    float plantMateSpatialWeight = 0.55f;
    float plantMateColorWeight = 0.45f;
    float plantMateNoise = 0.01f;
    float plantMatePreferenceMutationDelta = 0.09f;
    float plantColorPreferenceMutationDelta = 0.14f;
    float plantRadiusScale = 0.60f, plantRadiusMin = 0.42f;
    // Reconhecimento de estar sob uma copa enraizada. 0 = centro do bicho precisa
    // estar dentro da copa; 1 = qualquer parte do corpo tocando a copa ja conta.
    float plantCoverBodyRadiusWeight = 0.0f;
    // O observador que esta dentro/embaixo da propria copa pode enxergar para fora:
    // as copas enraizadas que efetivamente o cobrem sao ignoradas pelos seus raios.
    float rootedPlantInsideVisionPass = 1.0f;
    // Para observadores fora da copa, um bicho coberto permanece oculto mesmo se uma
    // parte do circulo corporal ultrapassar visualmente a borda da copa.
    float rootedPlantHideOccupants = 1.0f;
    float plantMassSizeExponent = 2.0f, plantMassMin = 0.20f;
    // Planta arrancada se decompoe como a carne, mas por padrao 10x mais devagar.
    // 0 = nao se decompoe automaticamente; 10 = dura 10x a duracao equivalente da carne.
    float plantDetachedDecayMultiplier = 10.0f;
    // Sway visual da vegetacao: a planta continua enraizada, mas a imagem pode
    // ser deslocada temporariamente pelos bichos para simular entrada no mato.
    float plantSwayVelocityScale = 0.18f, plantSwayMaxOffset = 7.5f;

    // Carne
    float carcassMinColorEfficiency = CARCASS_MIN_COLOR_ENERGY_EFFICIENCY;
    float carcassRefSize = 10.0f;
    float carcassBaseEnergy = 30.0f;
    float carcassBaseDuration = 120.0f;
    float carcassBaseBite = 12.0f;
    float carcassSensorScale = 0.60f;
    float carcassVisualScale = 0.50f;
    float carcassEnergySizeExponent = 1.0f;
    float carcassDurationSizeExponent = 1.0f;
    float carcassBiteSizeExponent = 1.0f;
    float carcassMassSizeExponent = 2.0f, carcassMassMin = 0.35f;
    float carcassBitePowerMax = 1.0f;

    // Fisica
    float creaturePushDamping = 1.00f;
    float objectDamping = 1.00f;
    float restitution = 1.00f;
    float positionCorrection = 1.00f;
    float maxPushSpeed = 1.00f;
    float physicsSlop = PHYSICS_SLOP;
    float physicsSolverIterations = (float)PHYSICS_SOLVER_ITERATIONS;
    float creatureMassDensity = CREATURE_MASS_DENSITY;
    float carcassMassDensity = CARCASS_MASS_DENSITY;
    float plantMassDensity = PLANT_MASS_DENSITY;
    float maxSubstep = MAX_SUBSTEP;
    // Feedback mecanico/tatil. Pressao e eventos recentes decaem continuamente.
    float tactilePressureScale = 0.055f;
    float tactileMemorySeconds = 0.65f;
    float impactFeedbackScale = 0.20f;
    float damageFeedbackScale = 0.10f;
    float energyGainFeedbackScale = 0.06f;

    // Cerebro / fundador / mutacao
    float brainInterval = BRAIN_INTERVAL;
    float founderSeekBiasChance = FOUNDER_SEEK_BIAS_CHANCE;
    float founderSeekPlantChance = FOUNDER_SEEK_PLANT_CHANCE;
    float founderSeekCarcassChance = FOUNDER_SEEK_CARCASS_CHANCE;
    float visionRayMutationChance = 0.10f;
    float visionDoubleRayDeltaChance = 0.20f;
    float visionRangeMutationChance = 0.11f;
    float visionRangeMutationStrength = 0.14f;
    float brainWeightMutationChance = 0.05f;
    float brainWeightMutationStrength = 0.28f;
    float brainBiasMutationChance = 0.05f;
    float brainBiasMutationStrength = 0.28f;
    float brainGainMutationChance = 0.045f;
    float brainGainMutationStrength = 0.16f;
    float brainMemoryMutationChance = 0.045f;
    float brainMemoryMutationStrength = 0.10f;
    float brainOutputBiasMutationChance = 0.05f;
    float brainAddNeuronChance = 0.030f;
    float brainRemoveNeuronChance = 0.012f;
    float brainLayerMutationChance = 0.035f;
    float brainAddConnectionChance = 0.145f;
    float brainRemoveConnectionChance = 0.050f;
    float brainReactivateConnectionChance = 0.035f;
    // v0.135: sensores/acoes sao genes dormentes. Fundadores possuem apenas o
    // circuito vegetal minimo; descendentes podem ganhar/perder entradas e saidas.
    float brainAddInputChance = 0.085f;
    float brainRemoveInputChance = 0.010f;
    float brainAddOutputChance = 0.045f;
    float brainRemoveOutputChance = 0.008f;
    float brainNewNeuronIsolatedChance = 0.68f;
    // Conectar a mesma area e para frente e mais facil; fan-out alto e feedback custam probabilidade.
    float brainSameAreaConnectionMult = 2.10f;
    float brainCrossAreaConnectionMult = 0.42f;
    float brainBackwardConnectionMult = 0.16f;
    float brainSameLayerConnectionMult = 0.52f;
    float brainOutputFeedbackConnectionMult = 0.08f;
    float brainFanoutPenalty = 0.32f;
    // A ligacao e uma funcao evolutiva, alem do peso/modo.
    float brainLinkFunctionMutationChance = 0.045f;
    float brainLinkParameterMutationChance = 0.055f;
    float brainLinkParameterMutationStrength = 0.22f;
    // v0.107: mutacoes que aumentam a expressividade/topologia do cerebro sem
    // adicionar comportamentos prontos. Areas sao apenas vieses de modularizacao.
    float brainActivationMutationChance = 0.028f;
    float brainAreaMutationChance = 0.030f;
    float brainConnectionModeMutationChance = 0.024f;
    float brainSplitConnectionChance = 0.040f;
    float brainDuplicateNeuronChance = 0.030f;
    float brainDuplicateCircuitChance = 0.012f;
    float brainSpecializedConnectionBias = 0.68f;
    float founderSpecializedAreaChance = 0.42f;
    float founderAltActivationChance = 0.08f;
    float founderGateConnectionChance = 0.018f;
    // v0.109: registradores persistentes + plasticidade reward-modulated Hebb.
    float founderRegisterMin = 0.0f, founderRegisterMax = 0.0f; // legado; ancestral novo nasce sem registradores
    float brainRegisterCountMutationChance = 0.035f;
    float brainRegisterEnergyCostPerSecond = 0.00035f;
    float founderPlasticConnectionChance = 0.012f;
    float brainPlasticToggleMutationChance = 0.020f;
    float brainPlasticRateMutationChance = 0.035f;
    float brainPlasticLearningRate = 0.085f;
    float brainPlasticEligibilitySeconds = 3.5f;
    float brainPlasticWeightRange = 1.60f;
    float brainPlasticReturnPerSecond = 0.0025f;
    float brainPlasticRewardGain = 1.0f; // legado: fork RL usa a tabela abaixo
    float brainPlasticDamagePenalty = 1.0f; // legado: fork RL usa a tabela abaixo
    float brainPlasticConnectionEnergyCostPerSecond = 0.00004f;

    // v0.116: funcao de recompensa explicitamente configuravel pelo pesquisador.
    // Todo item aceita valores positivos OU negativos: o ambiente nao decide o que e bom.
    float reinforcementEnabled = 1.0f;
    float reinforcementEnergy10Pct = 0.80f;       // por 10% da energia maxima recuperada
    float reinforcementPlantFood = 0.40f;         // mordida util de planta
    float reinforcementMeatFood = 0.70f;          // mordida util de carne
    float reinforcementEggFood = 0.20f;           // mordida util de ovo
    float reinforcementDamage10Pct = -1.00f;      // por 10% da vida maxima perdida
    float reinforcementAttack10Pct = 0.00f;       // por 10% da vida maxima tirada do outro
    float reinforcementReproduction = 25.0f;      // TER FILHO/acasalamento: maior recompensa padrao
    float reinforcementDeath = -25.0f;           // MORRER: pior punicao padrao
    float reinforcementSurvivalPerSecond = 0.002f;// reforco continuo por permanecer vivo
    float reinforcementStarvingPerSecond = -0.12f;// enquanto abaixo do limiar de fome
    float reinforcementRewardClip = 30.0f;        // permite que filho/morte tenham impacto realmente extremo
    // v0.116: recompensas por recordes do mundo. Valores sao recompensa/punicao
    // por unidade REAL de melhora do recorde (segundo ou ovo), portanto proporcionais.
    float reinforcementBittenEvent = -0.35f;       // evento de ser mordido, alem do dano proporcional
    float reinforcementRecordLongestLife = 0.08f; // por segundo alem do recorde de vida
    float reinforcementRecordShortestLife = -1.0f;// por fracao proporcional ao pior recorde terminal
    float reinforcementRecordHealthAbove50 = 0.05f;
    float reinforcementRecordHealthBelow50 = -0.50f; // novo recorde MAIS RAPIDO de 50% -> 0% sangue
    float reinforcementRecordEnergyAbove50 = 0.05f;
    float reinforcementRecordEnergyBelow50 = -0.50f; // novo recorde MAIS RAPIDO de 50% -> 0% energia
    float reinforcementRecordMostEggs = 0.80f;     // por ovo acima do recorde
    float reinforcementRecordFewestEggs = -1.00f; // proporcional ao novo minimo terminal

    // Acasalamento real: sem sexos fixos. Dois individuos maduros/prontos precisam
    // permanecer em contato; o parceiro tambem paga um pequeno custo e recebe cooldown.
    float matingRequired = 1.0f;
    float matingContactSeconds = 0.25f;
    float matingContactExtraRadius = 10.0f;
    float matingPartnerEnergyCostFraction = 0.02f;
    // O parceiro nao precisa estar apto a produzir um ovo inteiro: basta estar maduro
    // e em estado corporal razoavel para participar do acasalamento.
    float matingPartnerMinEnergyRatio = 0.20f;
    float matingPartnerMinHealthRatio = 0.20f;
    // Facilidade geral do acasalamento. Valores >1 ampliam a zona de cortejo,
    // reduzem o tempo efetivo de contato e suavizam os limiares corporais.
    float matingEaseFactor = 3.50f;
    // Fora do contato direto, mas dentro da zona de cortejo, o progresso acumula
    // mais devagar em vez de exigir uma colisao perfeita.
    float matingNearProgressRate = 0.40f;
    // Nova estrategia reprodutiva: ovo solo e arriscado; fecundado e eficiente.
    float soloEggFailureChance = 0.10f;   // 10% dos ovos sem parceiro viram carne
    float partneredEggTwinChance = 0.25f; // 25% dos ovos com parceiro abrem ninhada multipla
    float partneredEggCostScale = 0.30f;  // parceiro presente reduz em 70% o custo de sangue/energia
    float soloFallbackSeconds = 18.0f;    // pronto para reproduzir, espera por sexo antes de usar rota solo
    float sexualHeritageSoloFallbackSeconds = 90.0f; // linhagem sexual insiste muito mais em achar parceiro
    float matingSearchRangeScale = 2.00f; // multiplica o alcance genetico usado para procurar parceiro antes do solo

    float brainRegisterWriteGateThreshold = 0.62f;
    // Custo biologico da complexidade evita crescimento neutro ilimitado do cerebro.
    float brainNeuronEnergyCostPerSecond = 0.0012f;
    float brainConnectionEnergyCostPerSecond = 0.00008f;
    float brainRecurrentConnectionEnergyCostPerSecond = 0.00005f;
    float brainStatefulNeuronEnergyCostPerSecond = 0.00022f;
    float founderHiddenCount = 0.0f; // legado configuravel; o ancestral v0.135 nasce com zero ocultos
    float founderLayerMin = 1.0f, founderLayerMax = 3.0f;
    float founderGainMin = 0.65f, founderGainMax = 1.65f;
    float founderMemoryChance = 0.30f, founderMemoryMin = 0.08f, founderMemoryMax = 0.78f;
    float founderInputHiddenChance = 0.18f;
    float founderHiddenSelfChance = 0.025f, founderHiddenHiddenChance = 0.055f;
    float founderHiddenOutputChance = 0.55f, founderInputOutputChance = 0.018f;
    float founderOutputHiddenChance = 0.018f, founderOutputOutputChance = 0.010f;
    float brainWeightLimit = 4.0f;
    float brainBiasLimit = 4.0f;
    float brainGainMin = 0.30f, brainGainMax = 3.00f;
    float brainMemoryMax = 0.96f;
    float brainMinHiddenEffective = 0.0f, brainMaxHiddenEffective = (float)MAX_HIDDEN;
    float brainLayerStepChance = 0.70f;
    float newNeuronMemoryChance = 0.35f, newNeuronMemoryMin = 0.08f, newNeuronMemoryMax = 0.82f;
    float newNeuronExtraConnectionChance = 0.55f;
    float randomConnectionSrcInputChance = 0.54f, randomConnectionSrcHiddenChance = 0.36f;
    float randomConnectionDstHiddenChance = 0.68f;
    float randomConnectionAttempts = 40.0f;
    float minimumActiveConnections = 0.0f;

    // Classificacao/analytics. "Especie" e apenas um agrupamento visual por distancia genetica.
    float speciesDistanceThreshold = 0.24f;
    float evolutionHistoryInterval = 5.0f;
    // Sistema/UI persistente em arquivo texto. Autosave usa tempo real e pausa junto com a simulacao.
    float autosaveIntervalSeconds = 60.0f; // 0 = desligado
    // Quanto tempo o tamanho fisico/visual da planta leva para acompanhar a biomassa ja consumida.
    float plantConsumeShrinkSeconds = 0.80f;

    // Emocoes / estados internos evolutivos. Nenhuma emocao possui significado
    // hardcoded: o DNA define gatilhos, persistencia e intensidade; a rede neural
    // define o que fazer com o sinal emocional.
    float emotionMinEffective = 0.0f;
    float emotionMaxEffective = (float)MAX_EMOTIONS;
    float founderEmotionMin = 0.0f;
    float founderEmotionMax = 3.0f;
    float founderEmotionTriggerMin = 4.0f;
    float founderEmotionTriggerMax = 10.0f;
    float emotionThresholdMin = 0.20f, emotionThresholdMax = 0.80f;
    float emotionGainMin = 0.50f, emotionGainMax = 2.00f;
    float emotionStrengthMin = 0.35f, emotionStrengthMax = 1.50f;
    float emotionImpactMin = 0.35f, emotionImpactMax = 1.50f;
    float emotionRiseTimeMin = 0.15f, emotionRiseTimeMax = 2.50f;
    float emotionDurationMin = 0.50f, emotionDurationMax = 10.0f;
    float emotionBiasMin = -1.25f, emotionBiasMax = 0.25f;
    float emotionTriggerWeightLimit = 2.50f;
    float emotionFounderInputSourceChance = 0.68f;
    float emotionFounderHiddenSourceChance = 0.22f;
    float emotionFounderOutputSourceChance = 0.07f; // restante = outra emocao
    float emotionAddChance = 0.035f;
    float emotionRemoveChance = 0.018f;
    float emotionGeneMutationChance = 0.085f;
    float emotionGeneMutationStrength = 0.12f;
    float emotionTriggerWeightMutationChance = 0.08f;
    float emotionTriggerWeightMutationStrength = 0.30f;
    float emotionAddTriggerChance = 0.08f;
    float emotionRemoveTriggerChance = 0.04f;
    float emotionMaxTriggersPerEmotion = 24.0f;

    // Mutacao corporal global
    float bodyMutationChance = 0.08f;
    float bodyMutationStrength = 0.06f;
    float sizeMutationChance = 0.09f;
    float sizeMutationStrength = 0.08f;
    float eggMutationChance = 0.10f;
    float eggMutationStrength = 0.10f;

    void reset() { *this = RuntimeTuning{}; }
};

inline RuntimeTuning tuning{};

inline void sanitizeTuning() {
    tuning.healthRegenThreshold = std::clamp(tuning.healthRegenThreshold, 0.0f, 0.99f);
    tuning.visionMinRange = std::max(1.0f, tuning.visionMinRange);
    tuning.visionMaxRange = std::max(tuning.visionMinRange + 1.0f, tuning.visionMaxRange);
    tuning.visionMinFov = std::clamp(tuning.visionMinFov, 1.0f, 360.0f);
    tuning.visionMaxFov = std::clamp(tuning.visionMaxFov, tuning.visionMinFov, 360.0f);
    tuning.minRaysEffective = std::clamp(std::round(tuning.minRaysEffective), (float)MIN_RAYS, (float)MAX_RAYS);
    tuning.maxRaysEffective = std::clamp(std::round(tuning.maxRaysEffective), tuning.minRaysEffective, (float)MAX_RAYS);
    tuning.founderMinRays = std::clamp(std::round(tuning.founderMinRays), tuning.minRaysEffective, tuning.maxRaysEffective);
    tuning.founderMaxRays = std::clamp(std::round(tuning.founderMaxRays), tuning.founderMinRays, tuning.maxRaysEffective);
    tuning.founderVisionMinRange = std::clamp(tuning.founderVisionMinRange, tuning.visionMinRange, tuning.visionMaxRange);
    tuning.founderVisionMaxRange = std::clamp(tuning.founderVisionMaxRange, tuning.founderVisionMinRange, tuning.visionMaxRange);
    tuning.visionFocusMin=std::max(0.05f,tuning.visionFocusMin); tuning.visionFocusMax=std::max(tuning.visionFocusMin,tuning.visionFocusMax);
    tuning.founderVisionFocusMin=std::clamp(tuning.founderVisionFocusMin,tuning.visionFocusMin,tuning.visionFocusMax);
    tuning.founderVisionFocusMax=std::clamp(tuning.founderVisionFocusMax,tuning.founderVisionFocusMin,tuning.visionFocusMax);
    tuning.visionAsymmetryMax=std::clamp(tuning.visionAsymmetryMax,0.0f,1.0f);
    tuning.visionColorSensitivityMin=std::max(0.0f,tuning.visionColorSensitivityMin); tuning.visionColorSensitivityMax=std::max(tuning.visionColorSensitivityMin,tuning.visionColorSensitivityMax);
    tuning.visionEfficiencyMin=std::max(0.05f,tuning.visionEfficiencyMin); tuning.visionEfficiencyMax=std::max(tuning.visionEfficiencyMin,tuning.visionEfficiencyMax);
    tuning.minCreatureSize = std::max(0.25f, tuning.minCreatureSize);
    tuning.maxCreatureSize = std::max(tuning.minCreatureSize, tuning.maxCreatureSize);
    tuning.founderMinSize = std::clamp(tuning.founderMinSize, tuning.minCreatureSize, tuning.maxCreatureSize);
    tuning.founderMaxSize = std::clamp(tuning.founderMaxSize, tuning.founderMinSize, tuning.maxCreatureSize);
    tuning.energyInitialBase = std::clamp(tuning.energyInitialBase, 0.0f, tuning.energyMaxBase);
    tuning.eggEnergyGeneMax = std::max(tuning.eggEnergyGeneMin, tuning.eggEnergyGeneMax);
    tuning.eggBloodGeneMax = std::max(tuning.eggBloodGeneMin, tuning.eggBloodGeneMax);
    tuning.eggIncubationMax = std::max(tuning.eggIncubationMin, tuning.eggIncubationMax);
    // 1x1 e o menor tamanho fisico/visual permitido para plantas.
    tuning.plantBirthSize = std::max(PLANT_MIN_FRAGMENT_SIZE, tuning.plantBirthSize);
    tuning.plantMaxSize = std::max(tuning.plantBirthSize, tuning.plantMaxSize);
    tuning.plantBiteSizeResistanceExponent = std::clamp(tuning.plantBiteSizeResistanceExponent,0.0f,2.0f);
    tuning.plantSizeScaleMin = std::clamp(tuning.plantSizeScaleMin,0.05f,1.0f);
    tuning.plantSizeScaleMax = std::max(1.0f,tuning.plantSizeScaleMax);
    tuning.plantFounderSizeVariation = std::max(0.0f,tuning.plantFounderSizeVariation);
    tuning.plantSizeMutationChance = std::clamp(tuning.plantSizeMutationChance,0.0f,1.0f);
    tuning.plantSizeMutationStrength = std::max(0.0f,tuning.plantSizeMutationStrength);
    tuning.plantSizeExtremeResistance = std::max(0.0f,tuning.plantSizeExtremeResistance);
    tuning.plantChildMaxDist = std::max(tuning.plantChildMinDist, tuning.plantChildMaxDist);
    tuning.founderVariationMax = std::max(tuning.founderVariationMin, tuning.founderVariationMax);
    tuning.geneForwardMax = std::max(tuning.geneForwardMin, tuning.geneForwardMax);
    tuning.geneReverseMax = std::max(tuning.geneReverseMin, tuning.geneReverseMax);
    tuning.geneLateralRatioMin = std::clamp(tuning.geneLateralRatioMin,0.01f,0.99f);
    tuning.geneLateralRatioMax = std::clamp(tuning.geneLateralRatioMax,tuning.geneLateralRatioMin,0.999f);
    tuning.founderLateralSpeedRatio = std::clamp(tuning.founderLateralSpeedRatio,tuning.geneLateralRatioMin,tuning.geneLateralRatioMax);
    tuning.multiAxisEnergyPenalty = std::max(0.0f,tuning.multiAxisEnergyPenalty);
    tuning.multiAxisForcePenalty = std::max(0.0f,tuning.multiAxisForcePenalty);
    tuning.grabOutputThreshold = std::clamp(tuning.grabOutputThreshold,0.0f,1.0f);
    tuning.grabReachBodyScale = std::max(0.0f,tuning.grabReachBodyScale);
    tuning.grabReachExtra = std::max(0.0f,tuning.grabReachExtra);
    tuning.grabMaxLoadMassRatio = std::max(0.01f,tuning.grabMaxLoadMassRatio);
    tuning.grabHoldEnergyCost = std::max(0.0f,tuning.grabHoldEnergyCost);
    tuning.grabMoveEnergyCost = std::max(0.0f,tuning.grabMoveEnergyCost);
    tuning.grabSpeedPenalty = std::max(0.0f,tuning.grabSpeedPenalty);
    tuning.grabAccelerationPenalty = std::max(0.0f,tuning.grabAccelerationPenalty);
    tuning.grabGap = std::max(0.0f,tuning.grabGap);
    tuning.geneAccelerationMax = std::max(tuning.geneAccelerationMin, tuning.geneAccelerationMax);
    tuning.geneFrictionMax = std::max(tuning.geneFrictionMin, tuning.geneFrictionMax);
    tuning.geneRotationMax = std::max(tuning.geneRotationMin, tuning.geneRotationMax);
    tuning.geneHardnessMax = std::max(tuning.geneHardnessMin, tuning.geneHardnessMax);
    tuning.emotionMinEffective = std::clamp(std::round(tuning.emotionMinEffective),0.0f,(float)MAX_EMOTIONS);
    tuning.emotionMaxEffective = std::clamp(std::round(tuning.emotionMaxEffective),tuning.emotionMinEffective,(float)MAX_EMOTIONS);
    tuning.founderEmotionMin = std::clamp(std::round(tuning.founderEmotionMin),tuning.emotionMinEffective,tuning.emotionMaxEffective);
    tuning.founderEmotionMax = std::clamp(std::round(tuning.founderEmotionMax),tuning.founderEmotionMin,tuning.emotionMaxEffective);
    tuning.founderEmotionTriggerMin = std::max(0.0f,std::round(tuning.founderEmotionTriggerMin));
    tuning.founderEmotionTriggerMax = std::max(tuning.founderEmotionTriggerMin,std::round(tuning.founderEmotionTriggerMax));
    tuning.emotionThresholdMax = std::max(tuning.emotionThresholdMin,tuning.emotionThresholdMax);
    tuning.emotionGainMax = std::max(tuning.emotionGainMin,tuning.emotionGainMax);
    tuning.emotionStrengthMax = std::max(tuning.emotionStrengthMin,tuning.emotionStrengthMax);
    tuning.emotionImpactMax = std::max(tuning.emotionImpactMin,tuning.emotionImpactMax);
    tuning.emotionRiseTimeMax = std::max(tuning.emotionRiseTimeMin,tuning.emotionRiseTimeMax);
    tuning.emotionDurationMax = std::max(tuning.emotionDurationMin,tuning.emotionDurationMax);
    tuning.emotionBiasMax = std::max(tuning.emotionBiasMin,tuning.emotionBiasMax);
    tuning.emotionMaxTriggersPerEmotion = std::clamp(std::round(tuning.emotionMaxTriggersPerEmotion),1.0f,128.0f);
    tuning.sizeSpeedMax = std::max(tuning.sizeSpeedMin, tuning.sizeSpeedMax);
    tuning.sizeAccelMax = std::max(tuning.sizeAccelMin, tuning.sizeAccelMax);
    tuning.sizeRotationMax = std::max(tuning.sizeRotationMin, tuning.sizeRotationMax);
    tuning.sizeEnergyCostMax = std::max(tuning.sizeEnergyCostMin, tuning.sizeEnergyCostMax);
    tuning.sizeBiteMax = std::max(tuning.sizeBiteMin, tuning.sizeBiteMax);
    tuning.mouthSizeFactorMax = std::max(tuning.mouthSizeFactorMin, tuning.mouthSizeFactorMax);
    tuning.mouthBiteGeneMax = std::max(tuning.mouthBiteGeneMin, tuning.mouthBiteGeneMax);
    tuning.mouthOpeningGeneMax = std::max(tuning.mouthOpeningGeneMin, tuning.mouthOpeningGeneMax);
    tuning.mouthSpeedGeneMax = std::max(tuning.mouthSpeedGeneMin, tuning.mouthSpeedGeneMax);
    tuning.founderMouthBiteMax = std::max(tuning.founderMouthBiteMin, tuning.founderMouthBiteMax);
    tuning.founderMouthOpeningMax = std::max(tuning.founderMouthOpeningMin, tuning.founderMouthOpeningMax);
    tuning.founderMouthSpeedMax = std::max(tuning.founderMouthSpeedMin, tuning.founderMouthSpeedMax);
    tuning.eggQualityEnergyWeight = std::max(0.0f,tuning.eggQualityEnergyWeight);
    tuning.eggQualityBloodWeight = std::max(0.0f,tuning.eggQualityBloodWeight);
    tuning.mateRangeMin=std::max(1.0f,tuning.mateRangeMin); tuning.mateRangeMax=std::max(tuning.mateRangeMin,tuning.mateRangeMax);
    tuning.founderMateRangeMin=std::clamp(tuning.founderMateRangeMin,tuning.mateRangeMin,tuning.mateRangeMax);
    tuning.founderMateRangeMax=std::clamp(tuning.founderMateRangeMax,tuning.founderMateRangeMin,tuning.mateRangeMax);
    tuning.mateChoiceNoise=std::max(0.0f,tuning.mateChoiceNoise); tuning.mateSpatialWeight=std::max(0.0f,tuning.mateSpatialWeight); tuning.mateVisualWeight=std::max(0.0f,tuning.mateVisualWeight);
    tuning.plantMateSpatialWeight = std::max(0.0f,tuning.plantMateSpatialWeight);
    tuning.plantMateColorWeight = std::max(0.0f,tuning.plantMateColorWeight);
    tuning.founderHiddenCount = std::clamp(std::round(tuning.founderHiddenCount),0.0f,(float)MAX_HIDDEN);
    tuning.brainMinHiddenEffective = std::clamp(std::round(tuning.brainMinHiddenEffective),0.0f,(float)MAX_HIDDEN);
    tuning.brainMaxHiddenEffective = std::clamp(std::round(tuning.brainMaxHiddenEffective),tuning.brainMinHiddenEffective,(float)MAX_HIDDEN);
    tuning.founderHiddenCount = std::clamp(tuning.founderHiddenCount,tuning.brainMinHiddenEffective,tuning.brainMaxHiddenEffective);
    tuning.brainActivationMutationChance=std::clamp(tuning.brainActivationMutationChance,0.0f,1.0f);
    tuning.brainAreaMutationChance=std::clamp(tuning.brainAreaMutationChance,0.0f,1.0f);
    tuning.brainConnectionModeMutationChance=std::clamp(tuning.brainConnectionModeMutationChance,0.0f,1.0f);
    tuning.brainAddInputChance=std::clamp(tuning.brainAddInputChance,0.0f,1.0f);
    tuning.brainRemoveInputChance=std::clamp(tuning.brainRemoveInputChance,0.0f,1.0f);
    tuning.brainAddOutputChance=std::clamp(tuning.brainAddOutputChance,0.0f,1.0f);
    tuning.brainRemoveOutputChance=std::clamp(tuning.brainRemoveOutputChance,0.0f,1.0f);
    tuning.brainNewNeuronIsolatedChance=std::clamp(tuning.brainNewNeuronIsolatedChance,0.0f,1.0f);
    tuning.brainSameAreaConnectionMult=std::max(0.01f,tuning.brainSameAreaConnectionMult);
    tuning.brainCrossAreaConnectionMult=std::max(0.001f,tuning.brainCrossAreaConnectionMult);
    tuning.brainBackwardConnectionMult=std::max(0.001f,tuning.brainBackwardConnectionMult);
    tuning.brainSameLayerConnectionMult=std::max(0.001f,tuning.brainSameLayerConnectionMult);
    tuning.brainOutputFeedbackConnectionMult=std::max(0.0001f,tuning.brainOutputFeedbackConnectionMult);
    tuning.brainFanoutPenalty=std::max(0.0f,tuning.brainFanoutPenalty);
    tuning.brainLinkFunctionMutationChance=std::clamp(tuning.brainLinkFunctionMutationChance,0.0f,1.0f);
    tuning.brainLinkParameterMutationChance=std::clamp(tuning.brainLinkParameterMutationChance,0.0f,1.0f);
    tuning.brainLinkParameterMutationStrength=std::max(0.0f,tuning.brainLinkParameterMutationStrength);
    tuning.brainSplitConnectionChance=std::clamp(tuning.brainSplitConnectionChance,0.0f,1.0f);
    tuning.brainDuplicateNeuronChance=std::clamp(tuning.brainDuplicateNeuronChance,0.0f,1.0f);
    tuning.brainDuplicateCircuitChance=std::clamp(tuning.brainDuplicateCircuitChance,0.0f,1.0f);
    tuning.brainSpecializedConnectionBias=std::clamp(tuning.brainSpecializedConnectionBias,0.0f,1.0f);
    tuning.founderSpecializedAreaChance=std::clamp(tuning.founderSpecializedAreaChance,0.0f,1.0f);
    tuning.founderAltActivationChance=std::clamp(tuning.founderAltActivationChance,0.0f,1.0f);
    tuning.founderGateConnectionChance=std::clamp(tuning.founderGateConnectionChance,0.0f,1.0f);
    tuning.founderRegisterMin=std::clamp(tuning.founderRegisterMin,0.0f,(float)REGISTER_MEMORY_SLOTS);
    tuning.founderRegisterMax=std::clamp(tuning.founderRegisterMax,tuning.founderRegisterMin,(float)REGISTER_MEMORY_SLOTS);
    tuning.brainRegisterCountMutationChance=std::clamp(tuning.brainRegisterCountMutationChance,0.0f,1.0f);
    tuning.brainRegisterEnergyCostPerSecond=std::max(0.0f,tuning.brainRegisterEnergyCostPerSecond);
    tuning.founderPlasticConnectionChance=std::clamp(tuning.founderPlasticConnectionChance,0.0f,1.0f);
    tuning.brainPlasticToggleMutationChance=std::clamp(tuning.brainPlasticToggleMutationChance,0.0f,1.0f);
    tuning.brainPlasticRateMutationChance=std::clamp(tuning.brainPlasticRateMutationChance,0.0f,1.0f);
    tuning.brainPlasticLearningRate=std::max(0.0f,tuning.brainPlasticLearningRate);
    tuning.brainPlasticEligibilitySeconds=std::max(0.05f,tuning.brainPlasticEligibilitySeconds);
    tuning.brainPlasticWeightRange=std::max(0.0f,tuning.brainPlasticWeightRange);
    tuning.brainPlasticReturnPerSecond=std::max(0.0f,tuning.brainPlasticReturnPerSecond);
    tuning.brainPlasticRewardGain=std::max(0.0f,tuning.brainPlasticRewardGain);
    tuning.brainPlasticDamagePenalty=std::max(0.0f,tuning.brainPlasticDamagePenalty);
    tuning.brainPlasticConnectionEnergyCostPerSecond=std::max(0.0f,tuning.brainPlasticConnectionEnergyCostPerSecond);
    tuning.reinforcementEnabled=std::clamp(tuning.reinforcementEnabled,0.0f,1.0f);
    tuning.reinforcementEnergy10Pct=std::clamp(tuning.reinforcementEnergy10Pct,-5.0f,5.0f);
    tuning.reinforcementPlantFood=std::clamp(tuning.reinforcementPlantFood,-5.0f,5.0f);
    tuning.reinforcementMeatFood=std::clamp(tuning.reinforcementMeatFood,-5.0f,5.0f);
    tuning.reinforcementEggFood=std::clamp(tuning.reinforcementEggFood,-5.0f,5.0f);
    tuning.reinforcementDamage10Pct=std::clamp(tuning.reinforcementDamage10Pct,-5.0f,5.0f);
    tuning.reinforcementAttack10Pct=std::clamp(tuning.reinforcementAttack10Pct,-5.0f,5.0f);
    tuning.reinforcementReproduction=std::clamp(tuning.reinforcementReproduction,-50.0f,50.0f);
    tuning.reinforcementDeath=std::clamp(tuning.reinforcementDeath,-50.0f,50.0f);
    tuning.reinforcementSurvivalPerSecond=std::clamp(tuning.reinforcementSurvivalPerSecond,-2.0f,2.0f);
    tuning.reinforcementStarvingPerSecond=std::clamp(tuning.reinforcementStarvingPerSecond,-2.0f,2.0f);
    tuning.reinforcementRewardClip=std::clamp(tuning.reinforcementRewardClip,0.05f,50.0f);
    tuning.reinforcementBittenEvent=std::clamp(tuning.reinforcementBittenEvent,-10.0f,10.0f);
    tuning.reinforcementRecordLongestLife=std::clamp(tuning.reinforcementRecordLongestLife,-10.0f,10.0f);
    tuning.reinforcementRecordShortestLife=std::clamp(tuning.reinforcementRecordShortestLife,-10.0f,10.0f);
    tuning.reinforcementRecordHealthAbove50=std::clamp(tuning.reinforcementRecordHealthAbove50,-10.0f,10.0f);
    tuning.reinforcementRecordHealthBelow50=std::clamp(tuning.reinforcementRecordHealthBelow50,-10.0f,10.0f);
    tuning.reinforcementRecordEnergyAbove50=std::clamp(tuning.reinforcementRecordEnergyAbove50,-10.0f,10.0f);
    tuning.reinforcementRecordEnergyBelow50=std::clamp(tuning.reinforcementRecordEnergyBelow50,-10.0f,10.0f);
    tuning.reinforcementRecordMostEggs=std::clamp(tuning.reinforcementRecordMostEggs,-10.0f,10.0f);
    tuning.reinforcementRecordFewestEggs=std::clamp(tuning.reinforcementRecordFewestEggs,-10.0f,10.0f);
    tuning.matingRequired=std::clamp(tuning.matingRequired,0.0f,1.0f);
    tuning.matingContactSeconds=std::clamp(tuning.matingContactSeconds,0.05f,30.0f);
    tuning.matingContactExtraRadius=std::clamp(tuning.matingContactExtraRadius,0.0f,50.0f);
    tuning.matingPartnerEnergyCostFraction=std::clamp(tuning.matingPartnerEnergyCostFraction,0.0f,0.5f);
    tuning.matingPartnerMinEnergyRatio=std::clamp(tuning.matingPartnerMinEnergyRatio,0.0f,1.0f);
    tuning.matingPartnerMinHealthRatio=std::clamp(tuning.matingPartnerMinHealthRatio,0.0f,1.0f);
    tuning.matingEaseFactor=std::clamp(tuning.matingEaseFactor,0.25f,6.0f);
    tuning.matingNearProgressRate=std::clamp(tuning.matingNearProgressRate,0.0f,2.0f);
    tuning.soloEggFailureChance=std::clamp(tuning.soloEggFailureChance,0.0f,1.0f);
    tuning.partneredEggTwinChance=std::clamp(tuning.partneredEggTwinChance,0.0f,1.0f);
    tuning.partneredEggCostScale=std::clamp(tuning.partneredEggCostScale,0.0f,1.0f);
    tuning.soloFallbackSeconds=std::clamp(tuning.soloFallbackSeconds,0.0f,600.0f);
    tuning.sexualHeritageSoloFallbackSeconds=std::clamp(tuning.sexualHeritageSoloFallbackSeconds,0.0f,1800.0f);
    tuning.matingSearchRangeScale=std::clamp(tuning.matingSearchRangeScale,0.25f,10.0f);
    tuning.brainRegisterWriteGateThreshold=std::clamp(tuning.brainRegisterWriteGateThreshold,0.0f,1.0f);
    tuning.brainNeuronEnergyCostPerSecond=std::max(0.0f,tuning.brainNeuronEnergyCostPerSecond);
    tuning.brainConnectionEnergyCostPerSecond=std::max(0.0f,tuning.brainConnectionEnergyCostPerSecond);
    tuning.brainRecurrentConnectionEnergyCostPerSecond=std::max(0.0f,tuning.brainRecurrentConnectionEnergyCostPerSecond);
    tuning.brainStatefulNeuronEnergyCostPerSecond=std::max(0.0f,tuning.brainStatefulNeuronEnergyCostPerSecond);
    tuning.founderLayerMin = std::clamp(std::round(tuning.founderLayerMin),1.0f,(float)MAX_BRAIN_LAYERS);
    tuning.founderLayerMax = std::clamp(std::round(tuning.founderLayerMax),tuning.founderLayerMin,(float)MAX_BRAIN_LAYERS);
    tuning.founderGainMax = std::max(tuning.founderGainMin,tuning.founderGainMax);
    tuning.founderMemoryMax = std::max(tuning.founderMemoryMin,tuning.founderMemoryMax);
    tuning.newNeuronMemoryMax = std::max(tuning.newNeuronMemoryMin,tuning.newNeuronMemoryMax);
    tuning.founderBodyColorSpeedMin = std::max(0.001f,tuning.founderBodyColorSpeedMin);
    tuning.founderBodyColorSpeedMax = std::max(tuning.founderBodyColorSpeedMin,tuning.founderBodyColorSpeedMax);
    tuning.bodyColorSpeedMin = std::max(0.001f,tuning.bodyColorSpeedMin);
    tuning.bodyColorSpeedMax = std::max(tuning.bodyColorSpeedMin,tuning.bodyColorSpeedMax);
    // Ciclo de vida: protege apenas coerencia numerica; mundos biologicamente
    // estranhos continuam permitidos de proposito pelo editor de leis.
    tuning.birthSizeFractionMin = std::clamp(tuning.birthSizeFractionMin,0.01f,1.0f);
    tuning.birthSizeFractionMax = std::clamp(tuning.birthSizeFractionMax,tuning.birthSizeFractionMin,1.0f);
    tuning.founderBirthSizeFractionMin = std::clamp(tuning.founderBirthSizeFractionMin,tuning.birthSizeFractionMin,tuning.birthSizeFractionMax);
    tuning.founderBirthSizeFractionMax = std::clamp(tuning.founderBirthSizeFractionMax,tuning.founderBirthSizeFractionMin,tuning.birthSizeFractionMax);
    tuning.growthDurationMin = std::max(0.1f,tuning.growthDurationMin);
    tuning.growthDurationMax = std::max(tuning.growthDurationMin,tuning.growthDurationMax);
    tuning.founderGrowthDurationMin = std::clamp(tuning.founderGrowthDurationMin,tuning.growthDurationMin,tuning.growthDurationMax);
    tuning.founderGrowthDurationMax = std::clamp(tuning.founderGrowthDurationMax,tuning.founderGrowthDurationMin,tuning.growthDurationMax);
    tuning.growthCurveMin = std::max(0.01f,tuning.growthCurveMin);
    tuning.growthCurveMax = std::max(tuning.growthCurveMin,tuning.growthCurveMax);
    tuning.founderGrowthCurveMin = std::clamp(tuning.founderGrowthCurveMin,tuning.growthCurveMin,tuning.growthCurveMax);
    tuning.founderGrowthCurveMax = std::clamp(tuning.founderGrowthCurveMax,tuning.founderGrowthCurveMin,tuning.growthCurveMax);
    tuning.adolescenceStartMin = std::clamp(tuning.adolescenceStartMin,0.0f,1.0f);
    tuning.adolescenceStartMax = std::clamp(tuning.adolescenceStartMax,tuning.adolescenceStartMin,1.0f);
    tuning.founderAdolescenceStartMin = std::clamp(tuning.founderAdolescenceStartMin,tuning.adolescenceStartMin,tuning.adolescenceStartMax);
    tuning.founderAdolescenceStartMax = std::clamp(tuning.founderAdolescenceStartMax,tuning.founderAdolescenceStartMin,tuning.adolescenceStartMax);
    tuning.maturityFractionMin = std::clamp(tuning.maturityFractionMin,0.01f,2.0f);
    tuning.maturityFractionMax = std::max(tuning.maturityFractionMin,tuning.maturityFractionMax);
    tuning.founderMaturityFractionMin = std::clamp(tuning.founderMaturityFractionMin,tuning.maturityFractionMin,tuning.maturityFractionMax);
    tuning.founderMaturityFractionMax = std::clamp(tuning.founderMaturityFractionMax,tuning.founderMaturityFractionMin,tuning.maturityFractionMax);
    tuning.longevityMin = std::max(1.0f,tuning.longevityMin);
    tuning.longevityMax = std::max(tuning.longevityMin,tuning.longevityMax);
    tuning.founderLongevityMin = std::clamp(tuning.founderLongevityMin,tuning.longevityMin,tuning.longevityMax);
    tuning.founderLongevityMax = std::clamp(tuning.founderLongevityMax,tuning.founderLongevityMin,tuning.longevityMax);
    tuning.senescenceFractionMin = std::clamp(tuning.senescenceFractionMin,0.0f,0.999f);
    tuning.senescenceFractionMax = std::clamp(tuning.senescenceFractionMax,tuning.senescenceFractionMin,0.999f);
    tuning.founderSenescenceFractionMin = std::clamp(tuning.founderSenescenceFractionMin,tuning.senescenceFractionMin,tuning.senescenceFractionMax);
    tuning.founderSenescenceFractionMax = std::clamp(tuning.founderSenescenceFractionMax,tuning.founderSenescenceFractionMin,tuning.senescenceFractionMax);
    tuning.lifecycleUpdateInterval = std::max(0.01f,tuning.lifecycleUpdateInterval);
    tuning.oldAgeSpeedMin = std::max(0.0f,tuning.oldAgeSpeedMin);
    tuning.oldAgeAccelMin = std::max(0.0f,tuning.oldAgeAccelMin);
    tuning.oldAgeRotationMin = std::max(0.0f,tuning.oldAgeRotationMin);
    tuning.oldAgeBiteMin = std::max(0.0f,tuning.oldAgeBiteMin);
    tuning.oldAgeRegenMin = std::max(0.0f,tuning.oldAgeRegenMin);
    tuning.oldAgeMetabolismMax = std::max(0.0f,tuning.oldAgeMetabolismMax);
    tuning.oldAgeFertilityMin = std::clamp(tuning.oldAgeFertilityMin,0.0f,1.0f);
    tuning.oldAgeReproCooldownMaxMult = std::max(0.01f,tuning.oldAgeReproCooldownMaxMult);
    tuning.longevityReference = std::max(1.0f,tuning.longevityReference);
    tuning.longevityMetabolismMin = std::max(0.0f,tuning.longevityMetabolismMin);
    tuning.longevityMetabolismMax = std::max(tuning.longevityMetabolismMin,tuning.longevityMetabolismMax);
    tuning.naturalDeathHazardAtLongevity = std::max(0.0f,tuning.naturalDeathHazardAtLongevity);
    tuning.naturalDeathHazardMax = std::max(tuning.naturalDeathHazardAtLongevity,tuning.naturalDeathHazardMax);
    tuning.tactilePressureScale=std::max(0.0001f,tuning.tactilePressureScale); tuning.tactileMemorySeconds=std::max(0.01f,tuning.tactileMemorySeconds);
    tuning.impactFeedbackScale=std::max(0.0001f,tuning.impactFeedbackScale); tuning.damageFeedbackScale=std::max(0.0001f,tuning.damageFeedbackScale); tuning.energyGainFeedbackScale=std::max(0.0001f,tuning.energyGainFeedbackScale);
    tuning.speciesDistanceThreshold=std::clamp(tuning.speciesDistanceThreshold,0.01f,2.0f); tuning.evolutionHistoryInterval=std::max(0.25f,tuning.evolutionHistoryInterval);
    tuning.autosaveIntervalSeconds=std::max(0.0f,tuning.autosaveIntervalSeconds);
    tuning.plantConsumeShrinkSeconds=std::clamp(tuning.plantConsumeShrinkSeconds,0.05f,30.0f);
}


constexpr float CARCASS_REF_SIZE = 10.0f;
constexpr float CARCASS_BASE_ENERGY = 30.0f;
constexpr float CARCASS_BASE_DURATION = 120.0f;
constexpr float CARCASS_BASE_BITE = 12.0f;
constexpr float CARCASS_SENSOR_SCALE = 0.60f;
constexpr float CARCASS_VISUAL_SCALE = 0.50f;
constexpr float CARCASS_GLOW_SCALE = 0.50f;

constexpr Color CREATURE_COLOR{229, 57, 53, 255};
constexpr Color PLANT_COLOR{50, 198, 83, 255};
constexpr Color PLANT_OUTLINE{20, 100, 43, 255};
constexpr Color PLANT_HIGHLIGHT{154, 232, 168, 255};
constexpr Color WORLD_BG{0, 0, 0, 255};
constexpr Color OUTSIDE_BG{55, 55, 55, 255};
}

using LegacyRemovedBands = std::array<float, cfg::LEGACY_SENSOR_BANDS>; // somente leitura/escrita de saves antigos


struct FastRng {
    uint64_t s[4]{};

    static uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    static uint64_t splitmix64(uint64_t& x) {
        uint64_t z = (x += 0x9E3779B97F4A7C15ull);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
        return z ^ (z >> 31);
    }

    FastRng() {
        uint64_t seed = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
        // random_device e usado uma unica vez na inicializacao; o loop da simulacao
        // usa somente xoshiro256**, que possui estado pequeno e operacoes inteiras.
        seed ^= (static_cast<uint64_t>(std::random_device{}()) << 32);
        for (auto& v : s) v = splitmix64(seed);
    }

    uint64_t nextU64() {
        const uint64_t result = rotl(s[1] * 5ull, 7) * 9ull;
        const uint64_t t = s[1] << 17;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], 45);
        return result;
    }

    float unitFloat() {
        // 24 bits uteis: suficiente para float e sem std::uniform_distribution.
        return static_cast<float>(nextU64() >> 40) * (1.0f / 16777216.0f);
    }
};

static FastRng gRng{};

float rf(float a, float b) {
    return a + (b - a) * gRng.unitFloat();
}

int ri(int a, int b) {
    if (b <= a) return a;
    const uint64_t span = static_cast<uint64_t>(b - a) + 1ull;
    const uint64_t threshold = (0ull - span) % span;
    uint64_t r;
    do { r = gRng.nextU64(); } while (r < threshold);
    return a + static_cast<int>(r % span);
}

bool chance(float p) {
    return gRng.unitFloat() < std::clamp(p, 0.0f, 1.0f);
}

float clampf(float v, float a, float b) { return std::clamp(v, a, b); }
float sqr(float v) { return v * v; }
float length2(Vector2 v) { return v.x * v.x + v.y * v.y; }
float dist2(Vector2 a, Vector2 b) { return sqr(a.x - b.x) + sqr(a.y - b.y); }
Vector2 add(Vector2 a, Vector2 b) { return {a.x + b.x, a.y + b.y}; }
Vector2 sub(Vector2 a, Vector2 b) { return {a.x - b.x, a.y - b.y}; }
Vector2 mul(Vector2 a, float s) { return {a.x * s, a.y * s}; }
float dot(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }

float pointSegmentDistance(Vector2 p, Vector2 a, Vector2 b) {
    Vector2 ab = sub(b, a);
    const float ab2 = length2(ab);
    if (ab2 <= 1e-8f) return std::sqrt(dist2(p, a));
    const float t = clampf(dot(sub(p, a), ab) / ab2, 0.0f, 1.0f);
    Vector2 q = add(a, mul(ab, t));
    return std::sqrt(dist2(p, q));
}

float pointSegmentDistanceSq(Vector2 p, Vector2 a, Vector2 b) {
    const Vector2 ab = sub(b, a);
    const float ab2 = length2(ab);
    if (ab2 <= 1e-8f) return dist2(p, a);
    const float t = clampf(dot(sub(p, a), ab) / ab2, 0.0f, 1.0f);
    const Vector2 q = add(a, mul(ab, t));
    return dist2(p, q);
}

float pointCircleStrokeDistance(Vector2 p, Vector2 c, float radius) {
    return std::fabs(std::sqrt(length2(sub(p, c))) - radius);
}

float moveToward(float current, float target, float maxDelta) {
    if (std::fabs(target - current) <= maxDelta) return target;
    return current + ((target > current) ? maxDelta : -maxDelta);
}

float lengthV(Vector2 v) { return std::sqrt(length2(v)); }

Vector2 clampMagnitude(Vector2 v, float maxLen) {
    const float l2 = length2(v);
    if (l2 <= maxLen * maxLen || l2 <= 1e-12f) return v;
    return mul(v, maxLen / std::sqrt(l2));
}

float wrapCoord(float v,float size){
    if(size<=0.0f) return 0.0f;
    v=std::fmod(v,size);
    if(v<0.0f) v+=size;
    if(v>=size) v=0.0f;
    return v;
}

Vector2 wrappedPoint(Vector2 p){
    p.x=wrapCoord(p.x,cfg::WORLD_W);
    p.y=wrapCoord(p.y,cfg::WORLD_H);
    return p;
}

float wrappedAxisDelta(float d,float size){
    if(size<=0.0f) return d;
    const float half=size*0.5f;
    if(d>half) d-=size;
    else if(d<-half) d+=size;
    return d;
}

Vector2 toroidalDelta(Vector2 from,Vector2 to){
    return {wrappedAxisDelta(to.x-from.x,cfg::WORLD_W),wrappedAxisDelta(to.y-from.y,cfg::WORLD_H)};
}

float toroidalDist2(Vector2 a,Vector2 b){const Vector2 d=toroidalDelta(a,b);return d.x*d.x+d.y*d.y;}
Vector2 nearestToroidalImage(Vector2 origin,Vector2 target){return add(origin,toroidalDelta(origin,wrappedPoint(target)));}

bool circleInsideWorld(Vector2 p, float radius) {
    (void)radius;
    return p.x>=0.0f && p.y>=0.0f && p.x<cfg::WORLD_W && p.y<cfg::WORLD_H;
}

// Nome legado mantido para compatibilidade com o resto do arquivo. Em mundo toroidal,
// "constrain" significa canonizar a coordenada no tile fundamental, nao prender na borda.
bool constrainCircleToWorld(Vector2& p, float radius) {
    (void)radius;
    const Vector2 before=p;
    p=wrappedPoint(p);
    return std::fabs(p.x-before.x)>1e-6f || std::fabs(p.y-before.y)>1e-6f;
}

void resolveCircleWorldBarrier(Vector2& p, Vector2& velocity, float radius) {
    (void)velocity;
    constrainCircleToWorld(p,radius);
}

void wrapCircle(Vector2& p, float radius) { constrainCircleToWorld(p,radius); }

// Resolve uma colisao circular com correcao de penetracao + impulso.
// Retorna true somente quando os circulos realmente estavam sobrepostos.
bool solveCircleCollision(
    Vector2& pa, Vector2& va, float ra, float invMassA,
    Vector2& pb, Vector2& vb, float rb, float invMassB)
{
    Vector2 delta = toroidalDelta(pa,pb);
    const float minDist = ra + rb;
    const float d2 = length2(delta);
    if (d2 >= minDist * minDist) return false;

    float d = std::sqrt(std::max(d2, 1e-12f));
    Vector2 normal = d > 1e-5f ? mul(delta, 1.0f / d) : Vector2{1.0f, 0.0f};
    const float invMassSum = invMassA + invMassB;
    if (invMassSum <= 1e-8f) return false;

    const float penetration = minDist - d;
    const float correctionMag =
        std::max(0.0f, penetration - cfg::tuning.physicsSlop) *
        cfg::PHYSICS_POSITION_CORRECTION * cfg::tuning.positionCorrection / invMassSum;
    const Vector2 correction = mul(normal, correctionMag);
    pa = sub(pa, mul(correction, invMassA));
    pb = add(pb, mul(correction, invMassB));

    const Vector2 relativeVelocity = sub(vb, va);
    const float alongNormal = dot(relativeVelocity, normal);
    if (alongNormal < 0.0f) {
        const float j = -(1.0f + cfg::PHYSICS_RESTITUTION * cfg::tuning.restitution) * alongNormal / invMassSum;
        const Vector2 impulse = mul(normal, j);
        va = sub(va, mul(impulse, invMassA));
        vb = add(vb, mul(impulse, invMassB));
    }
    return true;
}

float normalizeGene(float value, float minV, float maxV) {
    if (maxV <= minV) return 0.5f;
    return clampf((value - minV) / (maxV - minV), 0.0f, 1.0f);
}

float colorDistance(Color a, Color b) {
    const float dr = float(a.r) - float(b.r);
    const float dg = float(a.g) - float(b.g);
    const float db = float(a.b) - float(b.b);
    return std::sqrt(dr*dr + dg*dg + db*db);
}

float colorSimilarity(Color a, Color b) {
    constexpr float maxDistance = 441.67295593f; // sqrt(3*255^2)
    return clampf(1.0f - colorDistance(a,b) / maxDistance, 0.0f, 1.0f);
}

Color randomRgbColor() {
    return Color{(unsigned char)ri(0,255),(unsigned char)ri(0,255),(unsigned char)ri(0,255),255};
}

Color mixRgb(Color a, Color b) {
    auto channel = [](unsigned char x, unsigned char y) -> unsigned char {
        const float w = rf(0.0f,1.0f);
        return (unsigned char)std::clamp<int>((int)std::lround(float(x)*w + float(y)*(1.0f-w)),0,255);
    };
    return Color{channel(a.r,b.r),channel(a.g,b.g),channel(a.b,b.b),255};
}

Color mutateRgbColor(Color c) {
    auto mutateChannel = [](unsigned char value) -> unsigned char {
        if (!chance(cfg::tuning.plantRgbMutationChance)) return value;
        const int d = std::max(0,(int)std::lround(cfg::tuning.plantRgbMutationDelta));
        const int delta = ri(-d,d);
        return (unsigned char)std::clamp<int>((int)value + delta, 0, 255);
    };
    return Color{mutateChannel(c.r),mutateChannel(c.g),mutateChannel(c.b),255};
}

Color darkenRgb(Color c, float factor) {
    factor = clampf(factor,0.0f,1.0f);
    return Color{
        (unsigned char)std::lround(float(c.r)*factor),
        (unsigned char)std::lround(float(c.g)*factor),
        (unsigned char)std::lround(float(c.b)*factor),
        255};
}


float creatureMaxEnergyForSize(float size) {
    // Filhotes podem ser menores que o limite genetico de tamanho ADULTO.
    const float safeSize = clampf(size, 0.05f, std::max(0.05f,cfg::tuning.maxCreatureSize));
    float ratio = safeSize / std::max(0.05f,cfg::tuning.baseCreatureSize);
    return cfg::tuning.energyMaxBase * std::pow(ratio, cfg::tuning.energySizeExponent);
}

float sigmoid(float x) {
    if (x >= 0.0f) {
        float z = std::exp(-x);
        return 1.0f / (1.0f + z);
    }
    float z = std::exp(x);
    return z / (1.0f + z);
}

// [SEC-GENETICS] Genomas, cerebro, mutacao e recombinacao
struct PhysicalGenes {
    float maxForwardSpeed = 22.5f;
    float maxReverseSpeed = 13.75f;
    float lateralSpeedRatio = 0.75f; // sempre < 1: lateral e mais lento que frente
    float acceleration = 100.0f;
    float friction = 14.4f; // -60% em relacao a 36.0
    float rotationSpeed = 120.0f;
    float bodySize = 3.0f;
    float hardness = 0.4f;
};

struct MouthGenes {
    float biteForce = 0.7f;
    float maxOpening = 0.7f;
    float movementSpeed = 1.2f;
    // Cor digestiva/herbivoria: NAO e mostrada aos raios de outros bichos.
    Color color{60,200,80,255};
};

struct BodyColorControlGenes {
    // bit 0=R, bit 1=G, bit 2=B. Canal desligado fica preso na cor-base herdada.
    uint8_t channelMask = 0x07;
    // Faixa genetica que cada saida neural pode comandar (0..1 -> 0..255).
    float rMin = 0.0f, rMax = 1.0f;
    float gMin = 0.0f, gMax = 1.0f;
    float bMin = 0.0f, bMax = 1.0f;
    // Quantos intervalos completos de 0..255 o canal pode atravessar por segundo simulado.
    float changeSpeed = 1.0f;
};

struct VisionGenes {
    int rayCount = 7;
    float range = 500.0f;
    // 1 = distribuicao uniforme; >1 concentra no centro; <1 privilegia periferia.
    float focusExponent = 1.0f;
    // -1 desloca o conjunto para esquerda, +1 para direita.
    float asymmetry = 0.0f;
    float sensitivityR = 1.0f, sensitivityG = 1.0f, sensitivityB = 1.0f;
    // Maior eficiencia reduz custo energetico da visao.
    float efficiency = 1.0f;
};

// [LEGACY-SAVE-ONLY] Nao participa de sensores, custos, selecao ou mutacao.
struct LegacyRemovedSensorGenes {
    float rangeScale = 1.0f;
    float sensitivity = 1.0f;
    float lowSensitivity = 1.0f, midSensitivity = 1.0f, highSensitivity = 1.0f;
    // Estrutura binaria historica removida da simulacao; campos preservam layout de saves.
    float directionalPrecision = 0.85f;
};

// [LEGACY-SAVE-ONLY] Nao participa de comunicacao; terceiro output historico virou ACASALAR.
struct LegacyRemovedSocialGenes {
    uint8_t enabled = 0;
    float maxGain = 0.75f;
    float lowWeight = 0.34f, midWeight = 0.33f, highWeight = 0.33f;
};

struct ReproductionGenes {
    // Fracoes do maximo do pai transferidas ao ovo.
    float eggEnergyFraction = 0.28f;
    float eggBloodFraction = 0.16f;
    float mateRange = 90.0f;
    // -1 prefere diferente, +1 prefere semelhante; 0 indiferente.
    float visualPreference = 0.0f;
    float legacyRemovedPreference = 0.0f; // [LEGACY-SAVE-ONLY]
    float mateChoiceStrength = 0.5f;
};

struct DevelopmentGenes {
    // physical.bodySize e o tamanho adulto. O nascimento usa uma fracao dele.
    float birthSizeFraction = 0.38f;
    float growthDuration = 24.0f;
    float growthCurveExponent = 1.0f;
    // Inicio visual/comportamental da adolescencia e maturidade sexual sao
    // fracoes do tempo de crescimento. Maturidade pode ocorrer antes de 100%.
    float adolescenceStartFraction = 0.40f;
    float sexualMaturityFraction = 0.72f;
    // Longevidade e uma referencia genetica, nao uma data de morte obrigatoria.
    float longevity = 190.0f;
    float senescenceStartFraction = 0.76f;
};

enum class BrainNodeKind : uint8_t { Input = 0, Hidden = 1, Output = 2 };

struct HiddenNodeGene {
    float bias = 0.0f;
    uint8_t layer = 0;
    // Ganho e memoria tambem evoluem. memory=0 reage imediatamente;
    // valores altos preservam parte do estado anterior e criam escalas
    // temporais diferentes mesmo sem depender apenas de loops explicitos.
    float gain = 1.0f;
    float memory = 0.0f;
};

enum class BrainLinkFunction : uint8_t {
    Linear=0, Invert=1, Absolute=2, SignedSquare=3, Sine=4, Threshold=5, Gaussian=6, Saturating=7
};

// Layout antigo usado somente para migrar saves <= v0.134.
struct LegacyNeuralConnectionGeneV134 {
    BrainNodeKind srcKind = BrainNodeKind::Input;
    int src = 0;
    BrainNodeKind dstKind = BrainNodeKind::Hidden;
    int dst = 0;
    float weight = 0.0f;
    uint8_t enabled = 1;
};

struct NeuralConnectionGene {
    BrainNodeKind srcKind = BrainNodeKind::Input;
    int src = 0;
    BrainNodeKind dstKind = BrainNodeKind::Hidden;
    int dst = 0;
    float weight = 0.0f;
    uint8_t enabled = 1;
    // v0.135: a propria sinapse transforma o sinal. paramA/paramB significam
    // frequencia/limiar/centro/largura dependendo da funcao.
    BrainLinkFunction function = BrainLinkFunction::Linear;
    float paramA = 1.0f;
    float paramB = 0.0f;
};

struct NeuralGenome {
    int inputCount = cfg::INPUT_COUNT;
    int hiddenCount = 0;
    int outputCount = cfg::OUTPUT_COUNT;
    std::vector<HiddenNodeGene> hiddenNodes;
    std::vector<float> outputBiases;
    std::vector<NeuralConnectionGene> connections;
    // O catalogo fisico e fixo por performance, mas o DNA decide quais sensores
    // e acoes realmente existem. Um gene ativo pode perfeitamente nascer sem ligacao.
    std::vector<uint8_t> activeInputs;
    std::vector<uint8_t> activeOutputs;
};

int brainRegisterCapacity(const NeuralGenome& g){
    if((int)g.outputBiases.size()<=cfg::BRAIN_META_MEMORY_CAPACITY_INDEX) return 0;
    const float gene=clampf(g.outputBiases[cfg::BRAIN_META_MEMORY_CAPACITY_INDEX],0.0f,1.0f);
    return (int)std::lround(gene*float(cfg::REGISTER_MEMORY_SLOTS));
}
void setBrainRegisterCapacity(NeuralGenome& g,int slots){
    if((int)g.outputBiases.size()<cfg::BRAIN_OUTPUT_STORAGE_COUNT)g.outputBiases.resize(cfg::BRAIN_OUTPUT_STORAGE_COUNT,0.0f);
    slots=std::clamp(slots,0,cfg::REGISTER_MEMORY_SLOTS);
    g.outputBiases[cfg::BRAIN_META_MEMORY_CAPACITY_INDEX]=cfg::REGISTER_MEMORY_SLOTS>0?float(slots)/float(cfg::REGISTER_MEMORY_SLOTS):0.0f;
}
bool brainInputActive(const NeuralGenome& g,int i){return i>=0&&i<g.inputCount&&i<(int)g.activeInputs.size()&&g.activeInputs[(std::size_t)i]!=0;}
bool brainOutputActive(const NeuralGenome& g,int o){return o>=0&&o<g.outputCount&&o<(int)g.activeOutputs.size()&&g.activeOutputs[(std::size_t)o]!=0;}
int brainActiveInputCount(const NeuralGenome& g){int n=0;for(int i=0;i<g.inputCount;++i)if(brainInputActive(g,i))++n;return n;}
int brainActiveOutputCount(const NeuralGenome& g){int n=0;for(int o=0;o<g.outputCount;++o)if(brainOutputActive(g,o))++n;return n;}

bool brainInputCatalogUseful(int i){
    if(i<0||i>=cfg::INPUT_COUNT)return false;
    // Parte ainda nao reutilizada do padding legado continua sem sinal e nao deve
    // ser sorteada como uma "nova capacidade" falsa.
    if(i>=cfg::PERCEPTION_INPUT_BASE+cfg::PERCEPTION_USED_INPUTS &&
       i<cfg::PERCEPTION_INPUT_BASE+cfg::LEGACY_SENSOR_PADDING_INPUTS)return false;
    return true;
}
bool brainOutputCatalogUseful(int o){
    return o>=0&&o<cfg::OUTPUT_COUNT&&o!=cfg::LEGACY_RESERVED_OUTPUT_INDEX;
}

void ensureBrainOutputStorage(NeuralGenome& g){
    if((int)g.outputBiases.size()<cfg::BRAIN_OUTPUT_STORAGE_COUNT)g.outputBiases.resize(cfg::BRAIN_OUTPUT_STORAGE_COUNT,0.0f);
    else if((int)g.outputBiases.size()>cfg::BRAIN_OUTPUT_STORAGE_COUNT)g.outputBiases.resize(cfg::BRAIN_OUTPUT_STORAGE_COUNT);
    g.outputCount=cfg::OUTPUT_COUNT;g.inputCount=cfg::INPUT_COUNT;
    if((int)g.activeInputs.size()!=cfg::INPUT_COUNT)g.activeInputs.resize(cfg::INPUT_COUNT,0);
    if((int)g.activeOutputs.size()!=cfg::OUTPUT_COUNT)g.activeOutputs.resize(cfg::OUTPUT_COUNT,0);
    setBrainRegisterCapacity(g,brainRegisterCapacity(g));
}

const char* brainLinkFunctionName(BrainLinkFunction f){
    switch(f){
        case BrainLinkFunction::Invert:return "Inversa";
        case BrainLinkFunction::Absolute:return "Absoluta";
        case BrainLinkFunction::SignedSquare:return "Quadratica";
        case BrainLinkFunction::Sine:return "Seno";
        case BrainLinkFunction::Threshold:return "Limiar";
        case BrainLinkFunction::Gaussian:return "Gauss";
        case BrainLinkFunction::Saturating:return "Saturacao";
        default:return "Linear";
    }
}
float applyBrainLinkFunction(BrainLinkFunction f,float x,float a,float b){
    x=clampf(x,-1.0f,1.0f);
    switch(f){
        case BrainLinkFunction::Invert:return -x;
        case BrainLinkFunction::Absolute:return std::fabs(x);
        case BrainLinkFunction::SignedSquare:return x*std::fabs(x);
        case BrainLinkFunction::Sine:return std::sin(x*clampf(a,0.10f,8.0f)+b);
        case BrainLinkFunction::Threshold:return x>=clampf(a,-1.0f,1.0f)?1.0f:-1.0f;
        case BrainLinkFunction::Gaussian:{const float sigma=std::max(0.05f,std::fabs(b));const float d=(x-clampf(a,-1.0f,1.0f))/sigma;return clampf(2.0f*std::exp(-0.5f*d*d)-1.0f,-1.0f,1.0f);}
        case BrainLinkFunction::Saturating:return std::tanh(x*clampf(a,0.10f,8.0f)+b);
        default:return x;
    }
}


// v0.107: sem aumentar o POD dos genes, usamos bits antes ociosos.
// HiddenNodeGene::layer: [7..6]=ativacao, [5..3]=area, [2..0]=camada (0..7).
// NeuralConnectionGene::enabled: bit0=ativa, [2..1]=modo da conexao.
enum class BrainArea : uint8_t { General=0, Vision=1, Reserved=2, Internal=3, Memory=4, Emotion=5, Motor=6, Social=7 };
enum class BrainActivation : uint8_t { Tanh=0, Threshold=1, Sine=2, Linear=3 };
enum class BrainConnectionMode : uint8_t { Add=0, Gate=1, Modulate=2, Shunt=3 };

constexpr uint8_t BRAIN_NODE_LAYER_MASK=0x07u;
constexpr uint8_t BRAIN_NODE_AREA_MASK=0x38u;
constexpr uint8_t BRAIN_NODE_ACT_MASK=0xC0u;

int hiddenNodeLayer(const HiddenNodeGene& n){ return int(n.layer & BRAIN_NODE_LAYER_MASK); }
BrainArea hiddenNodeArea(const HiddenNodeGene& n){ return BrainArea((n.layer>>3)&0x07u); }
BrainActivation hiddenNodeActivation(const HiddenNodeGene& n){ return BrainActivation((n.layer>>6)&0x03u); }
void setHiddenNodeLayer(HiddenNodeGene& n,int layer){n.layer=uint8_t((n.layer&~BRAIN_NODE_LAYER_MASK)|(uint8_t(std::clamp(layer,0,cfg::MAX_BRAIN_LAYERS-1))&BRAIN_NODE_LAYER_MASK));}
void setHiddenNodeArea(HiddenNodeGene& n,BrainArea area){n.layer=uint8_t((n.layer&~BRAIN_NODE_AREA_MASK)|((uint8_t(area)&0x07u)<<3));}
void setHiddenNodeActivation(HiddenNodeGene& n,BrainActivation a){n.layer=uint8_t((n.layer&~BRAIN_NODE_ACT_MASK)|((uint8_t(a)&0x03u)<<6));}
void setHiddenNodeMeta(HiddenNodeGene& n,int layer,BrainArea area,BrainActivation act){n.layer=0;setHiddenNodeLayer(n,layer);setHiddenNodeArea(n,area);setHiddenNodeActivation(n,act);}

bool neuralConnectionEnabled(const NeuralConnectionGene& c){return (c.enabled&0x01u)!=0;}
BrainConnectionMode neuralConnectionMode(const NeuralConnectionGene& c){return BrainConnectionMode((c.enabled>>1)&0x03u);}
// v0.109 usa bits altos antes ociosos sem alterar o POD/saves:
// bit3 = plasticidade ativa; bits7..4 = nivel genetico de aprendizagem (0..15).
bool neuralConnectionPlastic(const NeuralConnectionGene& c){return (c.enabled&0x08u)!=0;}
int neuralConnectionPlasticLevel(const NeuralConnectionGene& c){return int((c.enabled>>4)&0x0Fu);}
void setNeuralConnectionEnabled(NeuralConnectionGene& c,bool on){c.enabled=uint8_t((c.enabled&~0x01u)|(on?1u:0u));}
void setNeuralConnectionMode(NeuralConnectionGene& c,BrainConnectionMode mode){c.enabled=uint8_t((c.enabled&~0x06u)|((uint8_t(mode)&0x03u)<<1));}
void setNeuralConnectionPlastic(NeuralConnectionGene& c,bool on){c.enabled=uint8_t((c.enabled&~0x08u)|(on?0x08u:0u));}
void setNeuralConnectionPlasticLevel(NeuralConnectionGene& c,int level){c.enabled=uint8_t((c.enabled&0x0Fu)|((uint8_t(std::clamp(level,0,15))&0x0Fu)<<4));}
float neuralConnectionPlasticFactor(const NeuralConnectionGene& c){return (float(neuralConnectionPlasticLevel(c))+1.0f)/16.0f;}
void copyNeuralConnectionMeta(NeuralConnectionGene& dst,const NeuralConnectionGene& src){
    setNeuralConnectionMode(dst,neuralConnectionMode(src));
    setNeuralConnectionPlastic(dst,neuralConnectionPlastic(src));
    setNeuralConnectionPlasticLevel(dst,neuralConnectionPlasticLevel(src));
    dst.function=src.function;dst.paramA=src.paramA;dst.paramB=src.paramB;
}

const char* brainAreaName(BrainArea a){
    switch(a){case BrainArea::Vision:return "Visual";case BrainArea::Reserved:return "Reservada";case BrainArea::Internal:return "Interna";case BrainArea::Memory:return "Memoria";case BrainArea::Emotion:return "Emocao";case BrainArea::Motor:return "Motora";case BrainArea::Social:return "Social";default:return "Geral";}
}
const char* brainActivationName(BrainActivation a){switch(a){case BrainActivation::Threshold:return "Limiar";case BrainActivation::Sine:return "Seno";case BrainActivation::Linear:return "Linear";default:return "Tanh";}}
const char* brainConnectionModeName(BrainConnectionMode m){switch(m){case BrainConnectionMode::Gate:return "PORTA";case BrainConnectionMode::Modulate:return "MODULA";case BrainConnectionMode::Shunt:return "SHUNT";default:return "SOMA";}}

BrainArea brainAreaForInput(int index){
    const int visionEnd=cfg::MAX_RAYS*cfg::INPUTS_PER_RAY;
    const int internalEnd=visionEnd+cfg::BASE_INTERNAL_INPUTS;
    if(index<visionEnd)return BrainArea::Vision;
    if(index>=cfg::PERCEPTION_INPUT_BASE && index<cfg::PERCEPTION_INPUT_BASE+cfg::PERCEPTION_USED_INPUTS)return BrainArea::Vision;
    if(index<internalEnd)return BrainArea::Internal;
    if(index<cfg::BASE_INPUT_COUNT)return BrainArea::Reserved;
    if(index>=cfg::MEMORY_INPUT_BASE)return BrainArea::Memory;
    if(index>=cfg::EMOTION_INPUT_BASE)return BrainArea::Emotion;
    return BrainArea::General;
}
BrainArea brainAreaForOutput(int index){
    if(index>=cfg::MEMORY_WRITE_OUTPUT_BASE)return BrainArea::Memory;
    if(index<cfg::MOTOR_OUTPUT_COUNT || index==cfg::BITE_INTENT_OUTPUT_INDEX)return BrainArea::Motor;
    return BrainArea::Social;
}
BrainArea randomSpecializedBrainArea(){
    static constexpr BrainArea areas[]={BrainArea::Vision,BrainArea::Internal,BrainArea::Memory,BrainArea::Emotion,BrainArea::Motor,BrainArea::Social};
    return areas[ri(0,(int)(sizeof(areas)/sizeof(areas[0]))-1)];
}
BrainActivation randomAlternativeActivation(){return BrainActivation(ri(1,3));}

int randomInputForArea(BrainArea area){
    int lo=0,hi=cfg::INPUT_COUNT-1;
    const int visionEnd=cfg::MAX_RAYS*cfg::INPUTS_PER_RAY;
    if(area==BrainArea::Vision){lo=0;hi=visionEnd-1;}
    else if(area==BrainArea::Internal){lo=visionEnd;hi=visionEnd+cfg::BASE_INTERNAL_INPUTS-1;}
    else if(area==BrainArea::Reserved){lo=visionEnd;hi=visionEnd+cfg::BASE_INTERNAL_INPUTS-1;}
    else if(area==BrainArea::Emotion){lo=cfg::EMOTION_INPUT_BASE;hi=cfg::MEMORY_INPUT_BASE-1;}
    else if(area==BrainArea::Memory){lo=cfg::MEMORY_INPUT_BASE;hi=cfg::INPUT_COUNT-1;}
    return ri(std::max(0,lo),std::max(std::max(0,lo),hi));
}
int randomHiddenForArea(const NeuralGenome& g,BrainArea area){
    std::array<int,cfg::MAX_HIDDEN> candidates{};int n=0;
    for(int h=0;h<g.hiddenCount && n<cfg::MAX_HIDDEN;++h)if(hiddenNodeArea(g.hiddenNodes[h])==area)candidates[n++]=h;
    return n>0?candidates[ri(0,n-1)]:ri(0,std::max(0,g.hiddenCount-1));
}
float applyHiddenActivation(BrainActivation a,float x){
    switch(a){case BrainActivation::Threshold:return x>=0.0f?1.0f:-1.0f;case BrainActivation::Sine:return std::sin(x);case BrainActivation::Linear:return clampf(x,-1.0f,1.0f);default:return std::tanh(x);}
}

enum class EmotionSourceKind : uint8_t { Input = 0, Hidden = 1, Output = 2, Emotion = 3 };

struct EmotionTriggerGene {
    EmotionSourceKind sourceKind = EmotionSourceKind::Input;
    int source = 0;
    float weight = 0.0f;
};

struct EmotionGene {
    // drive = sigmoid((bias + soma(gatilhos)) * gain)
    float bias = -0.4f;
    float gain = 1.0f;
    float threshold = 0.60f;
    float strength = 1.0f;   // intensidade interna maxima
    float impact = 1.0f;     // intensidade entregue como entrada ao cerebro
    float riseTime = 0.60f;  // segundos para aproximar-se da intensidade maxima
    float duration = 4.0f;   // segundos aproximados para cair de cheio a zero sem gatilho
    std::vector<EmotionTriggerGene> triggers;
};

struct EmotionGenome {
    std::vector<EmotionGene> emotions;
};

struct CreatureGenome {
    PhysicalGenes physical;
    MouthGenes mouth;
    VisionGenes vision;
    LegacyRemovedSensorGenes legacyRemovedSensor;
    LegacyRemovedSocialGenes legacyRemovedSocial;
    ReproductionGenes reproduction;
    DevelopmentGenes development;
    EmotionGenome emotions;
    // Cor-base e capacidade de sinalizacao sao genes independentes do restante do DNA.
    Color bodyColor{229,57,53,255};
    BodyColorControlGenes bodyColorControl;
    NeuralGenome brain;
};

struct PlantGenes {
    Color color{50,198,83,255};
    // 1.0 = tamanho adulto medio atual. Abaixo = pequena; acima = grande.
    float sizeScale = 1.0f;
    // 1 = prefere espacialmente perto; 0 = prefere espacialmente longe.
    float matePreference = 0.5f;
    // -1 = prefere cor diferente; 0 = indiferente; +1 = prefere cor parecida.
    // Isso permite que isolamento por cor ou mistura entre cores surjam por evolucao.
    float colorMatePreference = 0.0f;
};

enum class MemoryEventType : uint8_t { None=0, Food=1, Attacked=2, AttackHit=3, EggLaid=4, LegacyRemoved=5 };

// Pequena memoria episodica biologica. worldDir guarda uma direcao ABSOLUTA do mundo;
// ao alimentar a rede ela e reprojetada nos eixos atuais do bicho, permitindo lembrar
// "onde" algo aconteceu mesmo depois de ele ter girado.
struct LongMemoryEvent {
    MemoryEventType type = MemoryEventType::None;
    float time = -1.0f;
    float intensity = 0.0f;
    Vector2 worldDir{};
};
static_assert(std::is_trivially_copyable_v<LongMemoryEvent>);

enum class RayType : uint8_t { None, Plant, Creature, Carcass, Egg };

struct RayReading {
    float angleOffset = 0.0f;
    float distance = 0.0f;
    float normalizedDistance = 1.0f;
    RayType type = RayType::None;
    Color targetColor{0,0,0,255};
    // Planta enraizada: Plant + plantDetached=0.
    // Planta mordida/fisica: Plant + plantDetached=1.
    // Assim o cerebro consegue aprender respostas diferentes sem criar dois tipos artificiais.
    float plantDetached = 0.0f;
    // Intensidade visual do balanco da copa ENRAIZADA detectada.
    // 0 = parada; 1 = deslocamento visual no limite. A causa nao e informada.
    float plantSway = 0.0f;

    // Similaridade cromatica contextual: boca x alimento para planta/carne;
    // corpo atual x corpo do alvo para bicho x bicho. Nao existe amigo/inimigo hardcoded.
    float colorAffinity = 0.0f;
    // Centro do objeto realmente atingido pelo raio (imagem toroidal usada no raycast).
    // Permite converter retina discreta em direcao continua sem revelar objetos nao vistos.
    Vector2 targetWorldPos{};
    bool hasTargetWorldPos = false;
};

struct CompiledConnection {
    // Indice direto em Creature::brainState. geneIndex aponta para o peso fenotipico
    // aprendido durante a vida; weight permanece como fallback/base genetica.
    int sourceIndex = 0;
    float weight = 0.0f;
    int geneIndex = -1;
    BrainLinkFunction function = BrainLinkFunction::Linear;
    float paramA = 1.0f;
    float paramB = 0.0f;
};

struct CompiledBrain {
    // SOMA continua no caminho MAC puro; modos menos comuns ficam em vetores separados.
    std::vector<std::vector<CompiledConnection>> hiddenIncoming, hiddenGateIncoming, hiddenModIncoming, hiddenShuntIncoming;
    std::vector<std::vector<CompiledConnection>> outputIncoming, outputGateIncoming, outputModIncoming, outputShuntIncoming;
    std::array<std::vector<int>, cfg::MAX_BRAIN_LAYERS> hiddenByLayer{};
    int activeConnections = 0;
    int recurrentConnections = 0;
    int gateConnections = 0;
    int modConnections = 0;
    int shuntConnections = 0;
    int plasticConnections = 0;
    int statefulHiddenNodes = 0;
    int usedLayers = 1;
};

float randomWeight() { return rf(-1.15f, 1.15f); }

int normalizeRayCount(int value) {
    return std::clamp(value, (int)std::lround(cfg::tuning.minRaysEffective), (int)std::lround(cfg::tuning.maxRaysEffective));
}

float visionFov(float range) {
    // O proprio gene de alcance cria o trade-off optico:
    // alcance minimo -> ate 360 graus; alcance maximo -> feixe de ~6 graus.
    // A escala logaritmica da distancia evita que quase todo o intervalo genetico
    // fique comprimido perto do FOV minimo quando o alcance chega a milhares.
    const float r = clampf(range, cfg::tuning.visionMinRange, cfg::tuning.visionMaxRange);
    const float denom = std::log(cfg::tuning.visionMaxRange / cfg::tuning.visionMinRange);
    const float t = denom > 0.0f
        ? clampf(std::log(r / cfg::tuning.visionMinRange) / denom, 0.0f, 1.0f)
        : 0.0f;
    const float shaped = std::pow(t, cfg::tuning.visionFovCurveExponent);
    return cfg::tuning.visionMaxFov +
        (cfg::tuning.visionMinFov - cfg::tuning.visionMaxFov) * shaped;
}

std::vector<float> visionOffsets(const VisionGenes& v) {
    std::vector<float> out;
    const int count = normalizeRayCount(v.rayCount);
    out.reserve(count);
    if (count <= 1) { out.push_back(clampf(v.asymmetry,-1.0f,1.0f)*visionFov(v.range)*0.22f); return out; }

    const float fov = visionFov(v.range);
    const float focus=clampf(v.focusExponent,cfg::tuning.visionFocusMin,cfg::tuning.visionFocusMax);
    const float shift=clampf(v.asymmetry,-1.0f,1.0f)*fov*0.22f;
    if (fov >= 359.5f) {
        const float step = 360.0f / float(count);
        const float start = -0.5f * step * float(count - 1) + shift;
        for (int i = 0; i < count; ++i) out.push_back(start + step * float(i));
        return out;
    }

    // O gene focusExponent deforma a distribuicao sem mudar o FOV total:
    // >1 concentra raios no centro, <1 concentra mais nas bordas.
    for(int i=0;i<count;++i){
        const float u=-1.0f+2.0f*float(i)/float(count-1);
        const float shaped=(u<0?-1.0f:1.0f)*std::pow(std::fabs(u),focus);
        out.push_back(clampf(shaped*fov*0.5f+shift,-fov*0.5f,fov*0.5f));
    }
    return out;
}

bool validEndpoint(const NeuralGenome& g, BrainNodeKind kind, int index) {
    if (index < 0) return false;
    if (kind == BrainNodeKind::Input) return index < g.inputCount;
    if (kind == BrainNodeKind::Hidden) return index < g.hiddenCount;
    return index < g.outputCount;
}

bool sameConnection(const NeuralConnectionGene& a, BrainNodeKind sk, int s, BrainNodeKind dk, int d) {
    return a.srcKind == sk && a.src == s && a.dstKind == dk && a.dst == d;
}

bool hasConnection(const NeuralGenome& g, BrainNodeKind sk, int s, BrainNodeKind dk, int d) {
    for (const auto& c : g.connections) {
        if (sameConnection(c, sk, s, dk, d)) return true;
    }
    return false;
}

void pushConnectionIfNew(NeuralGenome& g, BrainNodeKind sk, int s, BrainNodeKind dk, int d, float weight = 9999.0f) {
    if (dk == BrainNodeKind::Input) return;
    if (!validEndpoint(g, sk, s) || !validEndpoint(g, dk, d)) return;
    if (hasConnection(g, sk, s, dk, d)) return;
    NeuralConnectionGene c;
    c.srcKind = sk; c.src = s; c.dstKind = dk; c.dst = d;
    c.weight = weight == 9999.0f ? randomWeight() : weight;
    c.enabled = 1; // ativa + modo SOMA
    c.function=BrainLinkFunction::Linear;c.paramA=1.0f;c.paramB=0.0f;
    g.connections.push_back(c);
}

bool connectionIsRecurrent(const NeuralGenome& g, const NeuralConnectionGene& c) {
    if (c.srcKind == BrainNodeKind::Output) return true;
    if (c.dstKind == BrainNodeKind::Output) {
        return c.srcKind == BrainNodeKind::Output;
    }
    if (c.srcKind == BrainNodeKind::Input) return false;
    if (c.srcKind == BrainNodeKind::Hidden && c.dstKind == BrainNodeKind::Hidden) {
        if (c.src == c.dst) return true;
        const int sl = hiddenNodeLayer(g.hiddenNodes[c.src]);
        const int dl = hiddenNodeLayer(g.hiddenNodes[c.dst]);
        return sl >= dl;
    }
    return false;
}

void ensureConnectivity(NeuralGenome& g) {
    g.hiddenCount = (int)g.hiddenNodes.size();
    ensureBrainOutputStorage(g);
    // v0.135: "conectividade" nao significa mais forcar cada output a receber uma
    // ligacao. Genes isolados sao permitidos e importantes para evolucao futura.
    g.connections.erase(
        std::remove_if(g.connections.begin(), g.connections.end(), [&](const NeuralConnectionGene& c) {
            if(c.dstKind == BrainNodeKind::Input || !validEndpoint(g,c.srcKind,c.src) || !validEndpoint(g,c.dstKind,c.dst))return true;
            if(c.srcKind==BrainNodeKind::Input && !brainInputCatalogUseful(c.src))return true;
            if(c.dstKind==BrainNodeKind::Output && !brainOutputCatalogUseful(c.dst))return true;
            return false;
        }),g.connections.end());
}


NeuralGenome randomBrain() {
    NeuralGenome g;
    g.hiddenCount=0;
    g.hiddenNodes.clear();
    g.connections.clear();
    g.outputBiases.assign(cfg::BRAIN_OUTPUT_STORAGE_COUNT,-3.0f);
    g.activeInputs.assign(cfg::INPUT_COUNT,0);
    g.activeOutputs.assign(cfg::OUTPUT_COUNT,0);

    // Cerebro ancestral universal: apenas a percepcao continua da PLANTA e as
    // acoes necessarias para aproximar, alinhar, desacelerar e morder.
    for(int i:{cfg::PERCEPT_PLANT_PRESENT,cfg::PERCEPT_PLANT_DIRECTION,cfg::PERCEPT_PLANT_NEARNESS,cfg::PERCEPT_PLANT_MOUTH_CONTACT})
        g.activeInputs[(std::size_t)i]=1;
    for(int o:{0,1,2,3,cfg::BITE_INTENT_OUTPUT_INDEX})g.activeOutputs[(std::size_t)o]=1;

    // Sem planta, motores quase zerados. A presenca impulsiona frente.
    g.outputBiases[0]=-2.8f; g.outputBiases[1]=-3.2f;
    g.outputBiases[2]=-2.5f; g.outputBiases[3]=-2.5f;
    g.outputBiases[cfg::BITE_INTENT_OUTPUT_INDEX]=-3.2f;

    // Direcao planta e continua: negativo esquerda, positivo direita. O par de
    // motores antagonistas converte isso em giro assinado sem if/else comportamental.
    pushConnectionIfNew(g,BrainNodeKind::Input,cfg::PERCEPT_PLANT_PRESENT,BrainNodeKind::Output,0,5.2f);
    pushConnectionIfNew(g,BrainNodeKind::Input,cfg::PERCEPT_PLANT_DIRECTION,BrainNodeKind::Output,3,4.2f);
    pushConnectionIfNew(g,BrainNodeKind::Input,cfg::PERCEPT_PLANT_DIRECTION,BrainNodeKind::Output,2,-4.2f);
    // Quanto mais perto, mais o motor reverso antagoniza a aproximacao.
    pushConnectionIfNew(g,BrainNodeKind::Input,cfg::PERCEPT_PLANT_NEARNESS,BrainNodeKind::Output,1,4.8f);
    // Encostou a boca na planta -> morde. A mordida deixa de ser automatica para
    // cérebros novos e vira uma acao neural que pode evoluir/mutar.
    pushConnectionIfNew(g,BrainNodeKind::Input,cfg::PERCEPT_PLANT_MOUTH_CONTACT,BrainNodeKind::Output,cfg::BITE_INTENT_OUTPUT_INDEX,7.0f);

    // Registradores existem no catalogo, mas fundadores nao nascem com eles nem com
    // outros sensores/outputs. Podem surgir por mutacao nas geracoes seguintes.
    setBrainRegisterCapacity(g,0);
    ensureConnectivity(g);
    return g;
}


CompiledBrain compileBrain(const NeuralGenome& g) {
    CompiledBrain c;
    c.hiddenIncoming.resize(g.hiddenCount);c.hiddenGateIncoming.resize(g.hiddenCount);c.hiddenModIncoming.resize(g.hiddenCount);c.hiddenShuntIncoming.resize(g.hiddenCount);
    c.outputIncoming.resize(g.outputCount);c.outputGateIncoming.resize(g.outputCount);c.outputModIncoming.resize(g.outputCount);c.outputShuntIncoming.resize(g.outputCount);

    int maxLayer = 0;
    for (int h = 0; h < g.hiddenCount; ++h) {
        const int layer = hiddenNodeLayer(g.hiddenNodes[h]);
        c.hiddenByLayer[layer].push_back(h);
        maxLayer = std::max(maxLayer, layer);
        if(g.hiddenNodes[h].memory > 0.01f) ++c.statefulHiddenNodes;
    }
    c.usedLayers = maxLayer + 1;

    auto sourceIndexFor = [&](const NeuralConnectionGene& gene, bool recurrent) -> int {
        if (gene.srcKind == BrainNodeKind::Input) return cfg::BRAIN_INPUT_BASE + gene.src;
        if (gene.srcKind == BrainNodeKind::Hidden) return (recurrent ? cfg::BRAIN_PREV_HIDDEN_BASE : cfg::BRAIN_HIDDEN_BASE) + gene.src;
        return cfg::BRAIN_PREV_OUTPUT_BASE + gene.src;
    };
    for (int gi=0;gi<(int)g.connections.size();++gi) {
        const auto& gene=g.connections[(std::size_t)gi];
        if (!neuralConnectionEnabled(gene) || gene.dstKind == BrainNodeKind::Input) continue;
        if (!validEndpoint(g, gene.srcKind, gene.src) || !validEndpoint(g, gene.dstKind, gene.dst)) continue;
        if(gene.srcKind==BrainNodeKind::Input && !brainInputActive(g,gene.src))continue;
        if(gene.srcKind==BrainNodeKind::Output && !brainOutputActive(g,gene.src))continue;
        if(gene.dstKind==BrainNodeKind::Output && !brainOutputActive(g,gene.dst))continue;
        const bool recurrent = connectionIsRecurrent(g, gene);
        CompiledConnection cc{sourceIndexFor(gene,recurrent),gene.weight,gi,gene.function,gene.paramA,gene.paramB};
        const auto mode=neuralConnectionMode(gene);
        auto push=[&](auto& add,auto& gate,auto& mod,auto& shunt,int d){
            if(mode==BrainConnectionMode::Gate){gate[d].push_back(cc);++c.gateConnections;}
            else if(mode==BrainConnectionMode::Modulate){mod[d].push_back(cc);++c.modConnections;}
            else if(mode==BrainConnectionMode::Shunt){shunt[d].push_back(cc);++c.shuntConnections;}
            else add[d].push_back(cc);
        };
        if(gene.dstKind==BrainNodeKind::Hidden)push(c.hiddenIncoming,c.hiddenGateIncoming,c.hiddenModIncoming,c.hiddenShuntIncoming,gene.dst);
        else push(c.outputIncoming,c.outputGateIncoming,c.outputModIncoming,c.outputShuntIncoming,gene.dst);
        ++c.activeConnections;if(recurrent)++c.recurrentConnections;if(neuralConnectionPlastic(gene))++c.plasticConnections;
    }
    return c;
}

void addRandomConnection(NeuralGenome& g);

void addHiddenNeuron(NeuralGenome& g) {
    if (g.hiddenCount >= (int)std::lround(cfg::tuning.brainMaxHiddenEffective)) return;
    HiddenNodeGene n;
    n.bias=randomWeight();
    BrainArea area=chance(cfg::tuning.brainSpecializedConnectionBias)?randomSpecializedBrainArea():BrainArea::General;
    setHiddenNodeMeta(n,ri(0,cfg::MAX_BRAIN_LAYERS-1),area,BrainActivation::Tanh);
    n.gain=rf(cfg::tuning.founderGainMin,cfg::tuning.founderGainMax);
    n.memory=chance(cfg::tuning.newNeuronMemoryChance)?rf(cfg::tuning.newNeuronMemoryMin,cfg::tuning.newNeuronMemoryMax):0.0f;
    const int newIndex=g.hiddenCount;g.hiddenNodes.push_back(n);g.hiddenCount=(int)g.hiddenNodes.size();

    // A maioria nasce isolada: possuir um neuronio nao garante que ele ja tenha
    // utilidade. Mutações posteriores podem conecta-lo e recrutar a estrutura.
    if(chance(cfg::tuning.brainNewNeuronIsolatedChance))return;
    std::vector<int> inputs;for(int i=0;i<g.inputCount;++i)if(brainInputActive(g,i))inputs.push_back(i);
    if(!inputs.empty())pushConnectionIfNew(g,BrainNodeKind::Input,inputs[ri(0,(int)inputs.size()-1)],BrainNodeKind::Hidden,newIndex);
    if(chance(cfg::tuning.newNeuronExtraConnectionChance))addRandomConnection(g);
}


void removeHiddenNeuron(NeuralGenome& g) {
    if (g.hiddenCount <= (int)std::lround(cfg::tuning.brainMinHiddenEffective)) return;
    const int removeH = ri(0, g.hiddenCount - 1);

    g.connections.erase(
        std::remove_if(g.connections.begin(), g.connections.end(), [&](const NeuralConnectionGene& c) {
            return (c.srcKind == BrainNodeKind::Hidden && c.src == removeH) ||
                   (c.dstKind == BrainNodeKind::Hidden && c.dst == removeH);
        }),
        g.connections.end()
    );

    for (auto& c : g.connections) {
        if (c.srcKind == BrainNodeKind::Hidden && c.src > removeH) --c.src;
        if (c.dstKind == BrainNodeKind::Hidden && c.dst > removeH) --c.dst;
    }

    g.hiddenNodes.erase(g.hiddenNodes.begin() + removeH);
    g.hiddenCount = (int)g.hiddenNodes.size();
    ensureConnectivity(g);
}

void mutateNeuronLayer(NeuralGenome& g) {
    if (g.hiddenCount <= 0) return;
    const int h = ri(0, g.hiddenCount - 1);
    int layer = hiddenNodeLayer(g.hiddenNodes[h]);
    if (chance(cfg::tuning.brainLayerStepChance)) layer += chance(0.5f) ? 1 : -1;
    else layer = ri(0, cfg::MAX_BRAIN_LAYERS - 1);
    setHiddenNodeLayer(g.hiddenNodes[h],layer);
}

float brainDormantInputActivationWeight(int i){
    if(i>=cfg::PERCEPTION_INPUT_BASE && i<cfg::PERCEPTION_INPUT_BASE+cfg::PERCEPTION_USED_INPUTS)return 5.0f;
    const int rawEnd=cfg::MAX_RAYS*cfg::INPUTS_PER_RAY;
    if(i>=rawEnd && i<rawEnd+cfg::BASE_INTERNAL_INPUTS)return 2.6f;
    if(i<rawEnd)return 0.70f; // detalhe bruto de um raio e uma especializacao mais dificil
    if(i>=cfg::EMOTION_INPUT_BASE)return 1.55f;
    return 1.0f;
}
void activateRandomBrainInput(NeuralGenome& g){
    ensureBrainOutputStorage(g);
    std::vector<std::pair<int,float>> off;off.reserve(cfg::INPUT_COUNT);float total=0.0f;
    for(int i=0;i<cfg::INPUT_COUNT;++i){
        if(!brainInputCatalogUseful(i)||brainInputActive(g,i))continue;
        const float w=brainDormantInputActivationWeight(i);off.push_back({i,w});total+=w;
    }
    if(off.empty()||total<=0.0f)return;
    float r=rf(0.0f,total);
    for(const auto& [idx,w]:off){r-=w;if(r<=0.0f){g.activeInputs[(std::size_t)idx]=1;return;}}
    g.activeInputs[(std::size_t)off.back().first]=1;
}
void deactivateRandomBrainInput(NeuralGenome& g){
    std::vector<int> on;for(int i=0;i<g.inputCount;++i)if(brainInputActive(g,i))on.push_back(i);
    if(on.size()>1)g.activeInputs[(std::size_t)on[ri(0,(int)on.size()-1)]]=0;
}
void activateRandomBrainOutput(NeuralGenome& g){
    ensureBrainOutputStorage(g);std::vector<int> off;
    for(int o=0;o<cfg::OUTPUT_COUNT;++o)if(brainOutputCatalogUseful(o)&&!brainOutputActive(g,o))off.push_back(o);
    if(!off.empty())g.activeOutputs[(std::size_t)off[ri(0,(int)off.size()-1)]]=1;
}
void deactivateRandomBrainOutput(NeuralGenome& g){
    std::vector<int> on;for(int o=0;o<g.outputCount;++o)if(brainOutputActive(g,o))on.push_back(o);
    if(on.size()>1)g.activeOutputs[(std::size_t)on[ri(0,(int)on.size()-1)]]=0;
}
int brainOutgoingCount(const NeuralGenome& g,BrainNodeKind k,int idx){int n=0;for(const auto& c:g.connections)if(neuralConnectionEnabled(c)&&c.srcKind==k&&c.src==idx)++n;return n;}
BrainArea brainAreaForEndpoint(const NeuralGenome& g,BrainNodeKind k,int idx){
    if(k==BrainNodeKind::Input)return brainAreaForInput(idx);
    if(k==BrainNodeKind::Output)return brainAreaForOutput(idx);
    return (idx>=0&&idx<g.hiddenCount)?hiddenNodeArea(g.hiddenNodes[(std::size_t)idx]):BrainArea::General;
}
float brainConnectionDirectionFactor(const NeuralGenome& g,BrainNodeKind sk,int s,BrainNodeKind dk,int d){
    if(sk==BrainNodeKind::Input)return 1.0f;
    if(sk==BrainNodeKind::Output)return cfg::tuning.brainOutputFeedbackConnectionMult;
    if(dk==BrainNodeKind::Output)return 1.0f;
    const int sl=hiddenNodeLayer(g.hiddenNodes[(std::size_t)s]);
    const int dl=hiddenNodeLayer(g.hiddenNodes[(std::size_t)d]);
    if(dl>sl)return 1.0f;
    if(dl==sl)return cfg::tuning.brainSameLayerConnectionMult;
    return cfg::tuning.brainBackwardConnectionMult;
}

void addRandomConnection(NeuralGenome& g) {
    ensureBrainOutputStorage(g);
    std::vector<std::pair<BrainNodeKind,int>> srcs,dsts;
    for(int i=0;i<g.inputCount;++i)if(brainInputActive(g,i))srcs.push_back({BrainNodeKind::Input,i});
    for(int h=0;h<g.hiddenCount;++h){srcs.push_back({BrainNodeKind::Hidden,h});dsts.push_back({BrainNodeKind::Hidden,h});}
    for(int o=0;o<g.outputCount;++o)if(brainOutputActive(g,o)){srcs.push_back({BrainNodeKind::Output,o});dsts.push_back({BrainNodeKind::Output,o});}
    if(srcs.empty()||dsts.empty())return;
    const int tries=std::max(8,(int)std::lround(cfg::tuning.randomConnectionAttempts)*3);
    for(int attempt=0;attempt<tries;++attempt){
        const auto [sk,si]=srcs[(std::size_t)ri(0,(int)srcs.size()-1)];
        const auto [dk,di]=dsts[(std::size_t)ri(0,(int)dsts.size()-1)];
        if(hasConnection(g,sk,si,dk,di))continue;
        float p=brainConnectionDirectionFactor(g,sk,si,dk,di);
        const BrainArea sa=brainAreaForEndpoint(g,sk,si),da=brainAreaForEndpoint(g,dk,di);
        if(sa!=BrainArea::General && da!=BrainArea::General)p*=sa==da?cfg::tuning.brainSameAreaConnectionMult:cfg::tuning.brainCrossAreaConnectionMult;
        else if(sa==da)p*=1.25f;
        const int fan=brainOutgoingCount(g,sk,si);p/=1.0f+cfg::tuning.brainFanoutPenalty*(float)fan;
        if(!chance(clampf(p,0.01f,1.0f)))continue;
        pushConnectionIfNew(g,sk,si,dk,di);
        if(!g.connections.empty()){auto& c=g.connections.back();if(chance(0.28f))c.function=BrainLinkFunction(ri(0,7));}
        return;
    }
}


void splitRandomConnection(NeuralGenome& g){
    if(g.hiddenCount >= (int)std::lround(cfg::tuning.brainMaxHiddenEffective))return;
    std::vector<int> active;active.reserve(g.connections.size());
    for(int i=0;i<(int)g.connections.size();++i)if(neuralConnectionEnabled(g.connections[i]))active.push_back(i);
    if(active.empty())return;
    const int ci=active[ri(0,(int)active.size()-1)];
    const NeuralConnectionGene old=g.connections[ci];setNeuralConnectionEnabled(g.connections[ci],false);
    HiddenNodeGene n{};n.bias=0.0f;n.gain=1.0f;n.memory=0.0f;
    BrainArea area=BrainArea::General;
    if(old.dstKind==BrainNodeKind::Hidden)area=hiddenNodeArea(g.hiddenNodes[old.dst]);
    else if(old.srcKind==BrainNodeKind::Hidden)area=hiddenNodeArea(g.hiddenNodes[old.src]);
    else if(old.srcKind==BrainNodeKind::Input)area=brainAreaForInput(old.src);
    else if(old.dstKind==BrainNodeKind::Output)area=brainAreaForOutput(old.dst);
    int layer=0;
    if(old.srcKind==BrainNodeKind::Hidden)layer=hiddenNodeLayer(g.hiddenNodes[old.src]);
    if(old.dstKind==BrainNodeKind::Hidden)layer=(layer+hiddenNodeLayer(g.hiddenNodes[old.dst])+1)/2;
    setHiddenNodeMeta(n,layer,area,BrainActivation::Tanh);
    const int nh=(int)g.hiddenNodes.size();g.hiddenNodes.push_back(n);g.hiddenCount=(int)g.hiddenNodes.size();
    pushConnectionIfNew(g,old.srcKind,old.src,BrainNodeKind::Hidden,nh,1.0f);
    pushConnectionIfNew(g,BrainNodeKind::Hidden,nh,old.dstKind,old.dst,old.weight);
    if(!g.connections.empty())copyNeuralConnectionMeta(g.connections.back(),old);
}

void duplicateHiddenNeuron(NeuralGenome& g){
    if(g.hiddenCount<=0||g.hiddenCount >= (int)std::lround(cfg::tuning.brainMaxHiddenEffective))return;
    const int srcH=ri(0,g.hiddenCount-1);HiddenNodeGene clone=g.hiddenNodes[srcH];
    clone.bias=clampf(clone.bias+rf(-0.15f,0.15f),-cfg::tuning.brainBiasLimit,cfg::tuning.brainBiasLimit);
    clone.gain=clampf(clone.gain*(1.0f+rf(-0.08f,0.08f)),cfg::tuning.brainGainMin,cfg::tuning.brainGainMax);
    const int nh=g.hiddenCount;g.hiddenNodes.push_back(clone);g.hiddenCount=(int)g.hiddenNodes.size();
    const auto snapshot=g.connections;
    for(const auto& cn:snapshot){
        if(!neuralConnectionEnabled(cn))continue;
        if(cn.srcKind==BrainNodeKind::Hidden&&cn.src==srcH&&cn.dstKind==BrainNodeKind::Hidden&&cn.dst==srcH){
            pushConnectionIfNew(g,BrainNodeKind::Hidden,nh,BrainNodeKind::Hidden,nh,cn.weight*(1.0f+rf(-0.08f,0.08f)));if(!g.connections.empty())copyNeuralConnectionMeta(g.connections.back(),cn);
        }else{
            if(cn.dstKind==BrainNodeKind::Hidden&&cn.dst==srcH){pushConnectionIfNew(g,cn.srcKind,cn.src,BrainNodeKind::Hidden,nh,cn.weight*(1.0f+rf(-0.08f,0.08f)));if(!g.connections.empty())copyNeuralConnectionMeta(g.connections.back(),cn);}
            if(cn.srcKind==BrainNodeKind::Hidden&&cn.src==srcH){pushConnectionIfNew(g,BrainNodeKind::Hidden,nh,cn.dstKind,cn.dst,cn.weight*(1.0f+rf(-0.08f,0.08f)));if(!g.connections.empty())copyNeuralConnectionMeta(g.connections.back(),cn);}
        }
    }
    if(chance(0.35f))pushConnectionIfNew(g,BrainNodeKind::Hidden,srcH,BrainNodeKind::Hidden,nh,randomWeight()*0.5f);
}

void duplicateBrainCircuit(NeuralGenome& g){
    if(g.hiddenCount<2)return;
    const int room=(int)std::lround(cfg::tuning.brainMaxHiddenEffective)-g.hiddenCount;if(room<=0)return;
    const int seed=ri(0,g.hiddenCount-1);const BrainArea area=hiddenNodeArea(g.hiddenNodes[seed]);
    std::vector<int> src;for(int h=0;h<g.hiddenCount&&(int)src.size()<std::min(5,room);++h)if(hiddenNodeArea(g.hiddenNodes[h])==area)src.push_back(h);
    if(src.size()<2){src.clear();src.push_back(seed);for(int h=0;h<g.hiddenCount&&(int)src.size()<std::min(3,room);++h)if(h!=seed)src.push_back(h);}
    if(src.empty())return;
    std::unordered_map<int,int> map;const int oldCount=g.hiddenCount;
    for(int h:src){HiddenNodeGene n=g.hiddenNodes[h];n.bias=clampf(n.bias+rf(-0.12f,0.12f),-cfg::tuning.brainBiasLimit,cfg::tuning.brainBiasLimit);const int nh=(int)g.hiddenNodes.size();g.hiddenNodes.push_back(n);map[h]=nh;}
    g.hiddenCount=(int)g.hiddenNodes.size();const auto snapshot=g.connections;
    for(const auto& cn:snapshot){if(!neuralConnectionEnabled(cn))continue;bool sm=cn.srcKind==BrainNodeKind::Hidden&&map.count(cn.src);bool dm=cn.dstKind==BrainNodeKind::Hidden&&map.count(cn.dst);if(!sm&&!dm)continue;
        BrainNodeKind sk=cn.srcKind,dk=cn.dstKind;int si=sm?map[cn.src]:cn.src,di=dm?map[cn.dst]:cn.dst;
        // Copia conexoes internas e interfaces do modulo; externas sao amostradas para evitar explosao.
        if((sm&&dm) || chance(0.62f)){pushConnectionIfNew(g,sk,si,dk,di,cn.weight*(1.0f+rf(-0.10f,0.10f)));if(!g.connections.empty())copyNeuralConnectionMeta(g.connections.back(),cn);}
    }
    (void)oldCount;
}

void removeRandomConnection(NeuralGenome& g) {
    std::vector<int> active;
    active.reserve(g.connections.size());
    for (int i = 0; i < (int)g.connections.size(); ++i)
        if (neuralConnectionEnabled(g.connections[i])) active.push_back(i);
    if (active.size() <= (size_t)std::max(0,(int)std::lround(cfg::tuning.minimumActiveConnections))) return;
    setNeuralConnectionEnabled(g.connections[active[ri(0, (int)active.size() - 1)]],false);
    ensureConnectivity(g);
}

float mutateScalar(float value, float p, float strength, float minV, float maxV) {
    if (!chance(p)) return value;
    value *= 1.0f + rf(-strength, strength);
    return clampf(value, minV, maxV);
}

Color mutateColorGene(Color c, float mutationChance, float deltaMax) {
    auto ch=[&](unsigned char v)->unsigned char{
        if(!chance(mutationChance)) return v;
        const int d=std::max(0,(int)std::lround(deltaMax));
        return (unsigned char)std::clamp<int>((int)v + ri(-d,d),0,255);
    };
    return Color{ch(c.r),ch(c.g),ch(c.b),255};
}

float mutateUnitAdd(float value,float mutationChance,float strength){
    if(!chance(mutationChance)) return clampf(value,0.0f,1.0f);
    return clampf(value+rf(-strength,strength),0.0f,1.0f);
}

BodyColorControlGenes randomBodyColorControlGenes(){
    BodyColorControlGenes g;
    g.channelMask=0;
    if(chance(cfg::tuning.founderBodyColorChannelChance)) g.channelMask|=1;
    if(chance(cfg::tuning.founderBodyColorChannelChance)) g.channelMask|=2;
    if(chance(cfg::tuning.founderBodyColorChannelChance)) g.channelMask|=4;
    // Por padrao v0.074 fundadores nao possuem canais controlaveis; o Config ainda pode
    // alterar essa lei. Quando um canal surgir, sua faixa genetica inicial ja existe.
    g.rMin=g.gMin=g.bMin=0.0f;
    g.rMax=g.gMax=g.bMax=1.0f;
    g.changeSpeed=rf(cfg::tuning.founderBodyColorSpeedMin,cfg::tuning.founderBodyColorSpeedMax);
    return g;
}

BodyColorControlGenes mutateBodyColorControl(const BodyColorControlGenes& p){
    BodyColorControlGenes g=p;
    for(int bit=0;bit<3;++bit) if(chance(cfg::tuning.bodyColorChannelToggleChance)) g.channelMask ^= (uint8_t)(1u<<bit);
    auto range=[&](float& mn,float& mx){
        mn=mutateUnitAdd(mn,cfg::tuning.bodyColorRangeMutationChance,cfg::tuning.bodyColorRangeMutationStrength);
        mx=mutateUnitAdd(mx,cfg::tuning.bodyColorRangeMutationChance,cfg::tuning.bodyColorRangeMutationStrength);
        if(mx<mn) std::swap(mx,mn);
    };
    range(g.rMin,g.rMax); range(g.gMin,g.gMax); range(g.bMin,g.bMax);
    g.changeSpeed=mutateScalar(g.changeSpeed,cfg::tuning.bodyColorSpeedMutationChance,cfg::tuning.bodyColorSpeedMutationStrength,
        cfg::tuning.bodyColorSpeedMin,cfg::tuning.bodyColorSpeedMax);
    return g;
}

MouthGenes mutateMouth(const MouthGenes& p) {
    MouthGenes m = p;
    m.biteForce = mutateScalar(m.biteForce, cfg::tuning.mouthMutationChance, cfg::tuning.mouthMutationStrength, cfg::tuning.mouthBiteGeneMin, cfg::tuning.mouthBiteGeneMax);
    m.maxOpening = mutateScalar(m.maxOpening, cfg::tuning.mouthMutationChance, cfg::tuning.mouthMutationStrength, cfg::tuning.mouthOpeningGeneMin, cfg::tuning.mouthOpeningGeneMax);
    m.movementSpeed = mutateScalar(m.movementSpeed, cfg::tuning.mouthMutationChance, cfg::tuning.mouthMutationStrength, cfg::tuning.mouthSpeedGeneMin, cfg::tuning.mouthSpeedGeneMax);
    m.color = mutateColorGene(m.color,cfg::tuning.mouthColorMutationChance,cfg::tuning.mouthColorMutationDelta);
    return m;
}

VisionGenes mutateVision(const VisionGenes& p) {
    VisionGenes v = p;
    if (chance(cfg::tuning.visionRayMutationChance)) {
        int delta = chance(cfg::tuning.visionDoubleRayDeltaChance) ? 2 : 1;
        if (chance(0.5f)) delta = -delta;
        v.rayCount = normalizeRayCount(v.rayCount + delta);
    }
    if (chance(cfg::tuning.visionRangeMutationChance))
        v.range = clampf(v.range * (1.0f + rf(-cfg::tuning.visionRangeMutationStrength, cfg::tuning.visionRangeMutationStrength)),
                         cfg::tuning.visionMinRange, cfg::tuning.visionMaxRange);
    v.focusExponent=mutateScalar(v.focusExponent,cfg::tuning.visionTraitMutationChance,cfg::tuning.visionTraitMutationStrength,cfg::tuning.visionFocusMin,cfg::tuning.visionFocusMax);
    if(chance(cfg::tuning.visionTraitMutationChance)) v.asymmetry=clampf(v.asymmetry+rf(-cfg::tuning.visionTraitMutationStrength,cfg::tuning.visionTraitMutationStrength),-cfg::tuning.visionAsymmetryMax,cfg::tuning.visionAsymmetryMax);
    v.sensitivityR=mutateScalar(v.sensitivityR,cfg::tuning.visionTraitMutationChance,cfg::tuning.visionTraitMutationStrength,cfg::tuning.visionColorSensitivityMin,cfg::tuning.visionColorSensitivityMax);
    v.sensitivityG=mutateScalar(v.sensitivityG,cfg::tuning.visionTraitMutationChance,cfg::tuning.visionTraitMutationStrength,cfg::tuning.visionColorSensitivityMin,cfg::tuning.visionColorSensitivityMax);
    v.sensitivityB=mutateScalar(v.sensitivityB,cfg::tuning.visionTraitMutationChance,cfg::tuning.visionTraitMutationStrength,cfg::tuning.visionColorSensitivityMin,cfg::tuning.visionColorSensitivityMax);
    v.efficiency=mutateScalar(v.efficiency,cfg::tuning.visionTraitMutationChance,cfg::tuning.visionTraitMutationStrength,cfg::tuning.visionEfficiencyMin,cfg::tuning.visionEfficiencyMax);
    return v;
}

NeuralGenome mutateBrain(const NeuralGenome& parent) {
    NeuralGenome g = parent;
    // A copia preserva o DNA, mas a capacidade dos vectors costuma vir justa. Reservar
    // uma pequena folga evita realocacoes quando a mutacao estrutural adiciona genes.
    g.hiddenNodes.reserve(std::min<std::size_t>((std::size_t)cfg::MAX_HIDDEN, g.hiddenNodes.size() + 2));
    g.connections.reserve(g.connections.size() + 8);

    for (auto& c : g.connections) {
        if (chance(cfg::tuning.brainWeightMutationChance)) c.weight += rf(-cfg::tuning.brainWeightMutationStrength, cfg::tuning.brainWeightMutationStrength);
        c.weight = clampf(c.weight, -cfg::tuning.brainWeightLimit, cfg::tuning.brainWeightLimit);
        if(chance(cfg::tuning.brainConnectionModeMutationChance)) setNeuralConnectionMode(c,BrainConnectionMode(ri(0,3)));
        if(chance(cfg::tuning.brainPlasticToggleMutationChance)){
            setNeuralConnectionPlastic(c,!neuralConnectionPlastic(c));
            if(neuralConnectionPlastic(c)&&neuralConnectionPlasticLevel(c)==0)setNeuralConnectionPlasticLevel(c,ri(2,9));
        }
        if(neuralConnectionPlastic(c)&&chance(cfg::tuning.brainPlasticRateMutationChance))
            setNeuralConnectionPlasticLevel(c,std::clamp(neuralConnectionPlasticLevel(c)+ri(-2,2),0,15));
        if(chance(cfg::tuning.brainLinkFunctionMutationChance))c.function=BrainLinkFunction(ri(0,7));
        if(chance(cfg::tuning.brainLinkParameterMutationChance)){
            c.paramA=clampf(c.paramA+rf(-cfg::tuning.brainLinkParameterMutationStrength,cfg::tuning.brainLinkParameterMutationStrength),-2.0f,8.0f);
            c.paramB=clampf(c.paramB+rf(-cfg::tuning.brainLinkParameterMutationStrength,cfg::tuning.brainLinkParameterMutationStrength),-2.0f,2.0f);
        }
    }
    for (auto& n : g.hiddenNodes) {
        if (chance(cfg::tuning.brainBiasMutationChance)) n.bias = clampf(n.bias + rf(-cfg::tuning.brainBiasMutationStrength, cfg::tuning.brainBiasMutationStrength), -cfg::tuning.brainBiasLimit, cfg::tuning.brainBiasLimit);
        if (chance(cfg::tuning.brainGainMutationChance)) n.gain = clampf(n.gain * (1.0f + rf(-cfg::tuning.brainGainMutationStrength, cfg::tuning.brainGainMutationStrength)), cfg::tuning.brainGainMin, cfg::tuning.brainGainMax);
        if (chance(cfg::tuning.brainMemoryMutationChance)) n.memory = clampf(n.memory + rf(-cfg::tuning.brainMemoryMutationStrength, cfg::tuning.brainMemoryMutationStrength), 0.0f, cfg::tuning.brainMemoryMax);
        if(chance(cfg::tuning.brainActivationMutationChance))setHiddenNodeActivation(n,BrainActivation(ri(0,3)));
        if(chance(cfg::tuning.brainAreaMutationChance))setHiddenNodeArea(n,randomSpecializedBrainArea());
    }
    ensureBrainOutputStorage(g);
    for (int o=0;o<g.outputCount;++o) {
        auto& b=g.outputBiases[(std::size_t)o];
        if (chance(cfg::tuning.brainOutputBiasMutationChance)) b = clampf(b + rf(-cfg::tuning.brainBiasMutationStrength, cfg::tuning.brainBiasMutationStrength), -cfg::tuning.brainBiasLimit, cfg::tuning.brainBiasLimit);
    }
    if(chance(cfg::tuning.brainRegisterCountMutationChance))
        setBrainRegisterCapacity(g,std::clamp(brainRegisterCapacity(g)+(chance(0.5f)?1:-1),0,cfg::REGISTER_MEMORY_SLOTS));

    // Mutacoes estruturais: capacidade sensorial/motora e topologia inteira evoluem.
    // Um input/output novo nasce como gene, nao obrigatoriamente conectado.
    if(chance(cfg::tuning.brainAddInputChance))activateRandomBrainInput(g);
    if(chance(cfg::tuning.brainRemoveInputChance))deactivateRandomBrainInput(g);
    if(chance(cfg::tuning.brainAddOutputChance))activateRandomBrainOutput(g);
    if(chance(cfg::tuning.brainRemoveOutputChance))deactivateRandomBrainOutput(g);
    if (chance(cfg::tuning.brainAddNeuronChance)) addHiddenNeuron(g);
    if (chance(cfg::tuning.brainRemoveNeuronChance)) removeHiddenNeuron(g);
    if (chance(cfg::tuning.brainLayerMutationChance)) mutateNeuronLayer(g);
    if (chance(cfg::tuning.brainAddConnectionChance)) addRandomConnection(g);
    if (chance(cfg::tuning.brainRemoveConnectionChance)) removeRandomConnection(g);
    if (chance(cfg::tuning.brainSplitConnectionChance)) splitRandomConnection(g);
    if (chance(cfg::tuning.brainDuplicateNeuronChance)) duplicateHiddenNeuron(g);
    if (chance(cfg::tuning.brainDuplicateCircuitChance)) duplicateBrainCircuit(g);

    // Uma ligacao desativada pode reaparecer; isso preserva "genes dormentes".
    if (chance(cfg::tuning.brainReactivateConnectionChance)) {
        std::vector<int> disabled;
        for (int i = 0; i < (int)g.connections.size(); ++i)
            if (!neuralConnectionEnabled(g.connections[i])) disabled.push_back(i);
        if (!disabled.empty()) setNeuralConnectionEnabled(g.connections[disabled[ri(0, (int)disabled.size() - 1)]],true);
    }

    ensureConnectivity(g);
    return g;
}

std::array<float,3> brainColorSignature(const NeuralGenome& brain) {
    std::array<float,3> sums{0,0,0};
    std::array<int,3> counts{0,0,0};
    int globalIndex = 0;
    auto addValue = [&](float raw, bool enabled=true) {
        const int ch = globalIndex % 3;
        const float normalized = (std::tanh(raw * 0.70f) + 1.0f) * 0.5f;
        const float encoded = enabled ? (0.20f + normalized * 0.80f) : (normalized * 0.18f);
        sums[ch] += encoded;
        counts[ch]++;
        globalIndex++;
    };
    for (const auto& c : brain.connections) addValue(c.weight, neuralConnectionEnabled(c));
    for (const auto& n : brain.hiddenNodes) {
        addValue(n.bias, true);
        addValue(float(hiddenNodeLayer(n)) / float(std::max(1,cfg::MAX_BRAIN_LAYERS-1)), true);
        addValue(float(uint8_t(hiddenNodeArea(n)))/7.0f,true);
        addValue(float(uint8_t(hiddenNodeActivation(n)))/3.0f,true);
        addValue(n.gain - 1.0f, true);
        addValue(n.memory * 2.0f - 1.0f, true);
    }
    for (int o=0;o<brain.outputCount && o<(int)brain.outputBiases.size();++o) addValue(brain.outputBiases[(std::size_t)o], true);
    addValue(float(brainRegisterCapacity(brain))/float(std::max(1,cfg::REGISTER_MEMORY_SLOTS))*2.0f-1.0f,true);

    const float hiddenComplexity = normalizeGene((float)brain.hiddenCount,(float)cfg::MIN_HIDDEN,(float)cfg::MAX_HIDDEN);
    int active=0, fromInput=0, toOutput=0;
    for (const auto& c : brain.connections) if (neuralConnectionEnabled(c)) {
        active++;
        if (c.srcKind == BrainNodeKind::Input) fromInput++;
        if (c.dstKind == BrainNodeKind::Output) toOutput++;
    }
    const float inputDensity = clampf(float(fromInput) / float(std::max(1, brain.inputCount * (brain.hiddenCount + brain.outputCount))),0,1);
    const float outputDensity = clampf(float(toOutput) / float(std::max(1, (brain.inputCount + brain.hiddenCount + brain.outputCount) * brain.outputCount)),0,1);
    std::array<float,3> channels{};
    for(int i=0;i<3;++i) channels[i]=sums[i]/float(std::max(1,counts[i]));
    return {
        clampf(channels[0]*0.66f + hiddenComplexity*0.18f + inputDensity*0.16f,0,1),
        clampf(channels[1]*0.66f + outputDensity*0.18f + hiddenComplexity*0.16f,0,1),
        clampf(channels[2]*0.66f + inputDensity*0.17f + outputDensity*0.17f,0,1)
    };
}

std::array<float,3> developmentColorSignature(const DevelopmentGenes& d){
    return {
        clampf((d.birthSizeFraction + d.adolescenceStartFraction + d.sexualMaturityFraction)/3.0f,0.0f,1.0f),
        clampf(0.55f*normalizeGene(d.growthDuration,cfg::tuning.growthDurationMin,cfg::tuning.growthDurationMax) +
               0.45f*normalizeGene(d.longevity,cfg::tuning.longevityMin,cfg::tuning.longevityMax),0.0f,1.0f),
        clampf(0.50f*normalizeGene(d.growthCurveExponent,cfg::tuning.growthCurveMin,cfg::tuning.growthCurveMax) +
               0.50f*d.senescenceStartFraction,0.0f,1.0f)
    };
}

std::array<float,3> emotionColorSignature(const EmotionGenome& emotions){
    if(emotions.emotions.empty()) return {0.5f,0.5f,0.5f};
    std::array<float,3> sum{0,0,0}; int n=0;
    for(const auto& e:emotions.emotions){
        const float vals[]={e.bias,e.gain-1.0f,e.threshold*2.0f-1.0f,e.strength-0.75f,e.impact-0.75f,e.riseTime*0.2f-0.5f,e.duration*0.08f-0.5f};
        for(float v:vals){sum[n%3]+=(std::tanh(v)+1.0f)*0.5f;++n;}
        for(const auto& t:e.triggers){sum[n%3]+=(std::tanh(t.weight*0.7f)+1.0f)*0.5f;++n;}
    }
    const float denom=float(std::max(1,n/3));
    for(float& v:sum)v=clampf(v/denom,0,1);
    return sum;
}

Color deriveBodyColorFromDNA(const PhysicalGenes& physical, const MouthGenes& mouth,
                             const VisionGenes& vision, const NeuralGenome& brain,
                             const EmotionGenome& emotions, const DevelopmentGenes& development) {
    // Mesma filosofia da v0.023-v0.028: cor e um resumo continuo do DNA inteiro,
    // portanto DNA parecido tende a produzir cor parecida.
    const float forward=normalizeGene(physical.maxForwardSpeed,cfg::tuning.geneForwardMin,cfg::tuning.geneForwardMax);
    const float reverse=normalizeGene(physical.maxReverseSpeed,cfg::tuning.geneReverseMin,cfg::tuning.geneReverseMax);
    const float accel=normalizeGene(physical.acceleration,cfg::tuning.geneAccelerationMin,cfg::tuning.geneAccelerationMax);
    const float friction=normalizeGene(physical.friction,cfg::tuning.geneFrictionMin,cfg::tuning.geneFrictionMax);
    const float rotation=normalizeGene(physical.rotationSpeed,cfg::tuning.geneRotationMin,cfg::tuning.geneRotationMax);
    const float hardness=clampf(physical.hardness,0,1);
    const float bodySize=normalizeGene(physical.bodySize,cfg::tuning.minCreatureSize,cfg::tuning.maxCreatureSize);
    const float bite=normalizeGene(mouth.biteForce,cfg::tuning.mouthBiteGeneMin,cfg::tuning.mouthBiteGeneMax);
    const float opening=normalizeGene(mouth.maxOpening,cfg::tuning.mouthOpeningGeneMin,cfg::tuning.mouthOpeningGeneMax);
    const float mouthSpeed=normalizeGene(mouth.movementSpeed,cfg::tuning.mouthSpeedGeneMin,cfg::tuning.mouthSpeedGeneMax);
    const float range=normalizeGene(vision.range,cfg::tuning.visionMinRange,cfg::tuning.visionMaxRange);
    const float rays=normalizeGene((float)vision.rayCount,(float)cfg::MIN_RAYS,(float)cfg::MAX_RAYS);
    const float fov=normalizeGene(visionFov(vision.range),cfg::tuning.visionMinFov,cfg::tuning.visionMaxFov);
    auto avg=[](std::initializer_list<float> vals){ float total=0; for(float v:vals) total+=v; return total/std::max<size_t>(1,vals.size()); };
    const std::array<float,3> physicalSig{
        avg({forward,accel,1-hardness,1-bodySize,rotation*cfg::tuning.colorRotationWeight+reverse*cfg::tuning.colorReverseWeight}),
        avg({rotation,accel,1-friction,reverse,1-std::fabs(bodySize-0.5f)*2.0f}),
        avg({hardness,friction,reverse,bodySize,1-forward})};
    const std::array<float,3> mouthSig{bite,avg({opening,mouthSpeed}),avg({mouthSpeed,1-bite,opening})};
    const std::array<float,3> visionSig{range,rays,avg({1-range,rays,fov})};
    auto brainSig=brainColorSignature(brain);
    const auto emotionSig=emotionColorSignature(emotions);
    const auto lifeSig=developmentColorSignature(development);
    for(int i=0;i<3;++i) brainSig[i]=clampf(brainSig[i]*0.82f+emotionSig[i]*0.18f,0.0f,1.0f);
    const float totalW=std::max(0.0001f,cfg::tuning.colorPhysicalWeight+cfg::tuning.colorMouthWeight+
        cfg::tuning.colorVisionWeight+cfg::tuning.colorBrainWeight+cfg::tuning.colorLifecycleWeight);
    float red=(physicalSig[0]*cfg::tuning.colorPhysicalWeight+mouthSig[0]*cfg::tuning.colorMouthWeight+visionSig[0]*cfg::tuning.colorVisionWeight+brainSig[0]*cfg::tuning.colorBrainWeight+lifeSig[0]*cfg::tuning.colorLifecycleWeight)/totalW;
    float green=(physicalSig[1]*cfg::tuning.colorPhysicalWeight+mouthSig[1]*cfg::tuning.colorMouthWeight+visionSig[1]*cfg::tuning.colorVisionWeight+brainSig[1]*cfg::tuning.colorBrainWeight+lifeSig[1]*cfg::tuning.colorLifecycleWeight)/totalW;
    float blue=(physicalSig[2]*cfg::tuning.colorPhysicalWeight+mouthSig[2]*cfg::tuning.colorMouthWeight+visionSig[2]*cfg::tuning.colorVisionWeight+brainSig[2]*cfg::tuning.colorBrainWeight+lifeSig[2]*cfg::tuning.colorLifecycleWeight)/totalW;
    const float mean=(red+green+blue)/3.0f;
    const float maxDev=std::max({std::fabs(red-mean),std::fabs(green-mean),std::fabs(blue-mean)});
    if(maxDev>0 && maxDev<cfg::tuning.colorMinSeparation){ const float scale=std::min(cfg::tuning.colorMaxSeparationBoost,cfg::tuning.colorMinSeparation/maxDev); red=mean+(red-mean)*scale; green=mean+(green-mean)*scale; blue=mean+(blue-mean)*scale; }
    auto byte=[](float v)->unsigned char{return (unsigned char)std::clamp<int>((int)std::lround(cfg::tuning.colorByteFloor+clampf(v,0,1)*cfg::tuning.colorByteRange),0,255);};
    return Color{byte(red),byte(green),byte(blue),255};
}

float mutatePlantSizeScale(float current){
    current=clampf(current,cfg::tuning.plantSizeScaleMin,cfg::tuning.plantSizeScaleMax);
    if(!chance(cfg::tuning.plantSizeMutationChance)) return current;
    float delta=rf(-cfg::tuning.plantSizeMutationStrength,cfg::tuning.plantSizeMutationStrength);
    const float candidate=current+delta;
    // Ir ainda mais para um extremo fica progressivamente mais dificil; voltar em direcao
    // ao tamanho medio (1.0) nao recebe essa resistencia extra.
    if(std::fabs(candidate-1.0f)>std::fabs(current-1.0f)){
        const float maxDist=delta>=0.0f?std::max(0.001f,cfg::tuning.plantSizeScaleMax-1.0f):std::max(0.001f,1.0f-cfg::tuning.plantSizeScaleMin);
        const float dist=clampf(std::fabs(current-1.0f)/maxDist,0.0f,1.0f);
        const float ease=std::pow(std::max(0.03f,1.0f-dist),cfg::tuning.plantSizeExtremeResistance);
        delta*=ease;
    }
    return clampf(current+delta,cfg::tuning.plantSizeScaleMin,cfg::tuning.plantSizeScaleMax);
}

PlantGenes randomPlantGenes(){
    PlantGenes g;
    g.color = randomRgbColor();
    g.sizeScale=clampf(1.0f+rf(-cfg::tuning.plantFounderSizeVariation,cfg::tuning.plantFounderSizeVariation),cfg::tuning.plantSizeScaleMin,cfg::tuning.plantSizeScaleMax);
    g.matePreference = rf(0.0f,1.0f);
    g.colorMatePreference = rf(-1.0f,1.0f);
    return g;
}

PlantGenes inheritPlantGenes(const PlantGenes& a, const PlantGenes* b) {
    PlantGenes g = a;
    if (b) {
        const float w = rf(0.0f,1.0f);
        g.color = mixRgb(a.color,b->color);
        g.sizeScale=clampf(a.sizeScale*w+b->sizeScale*(1.0f-w),cfg::tuning.plantSizeScaleMin,cfg::tuning.plantSizeScaleMax);
        g.matePreference = clampf(a.matePreference*w + b->matePreference*(1.0f-w),0.0f,1.0f);
        g.colorMatePreference = clampf(
            a.colorMatePreference*w + b->colorMatePreference*(1.0f-w), -1.0f, 1.0f);
    }
    // Mesmo uma linhagem isolada pode continuar evoluindo lentamente.
    g.color = mutateRgbColor(g.color);
    g.sizeScale=mutatePlantSizeScale(g.sizeScale);
    if (chance(cfg::tuning.plantMateMutationChance)) g.matePreference = clampf(g.matePreference + rf(-cfg::tuning.plantMatePreferenceMutationDelta,cfg::tuning.plantMatePreferenceMutationDelta),0.0f,1.0f);
    if (chance(cfg::tuning.plantColorMateMutationChance)) g.colorMatePreference = clampf(g.colorMatePreference + rf(-cfg::tuning.plantColorPreferenceMutationDelta,cfg::tuning.plantColorPreferenceMutationDelta),-1.0f,1.0f);
    return g;
}

enum class FounderSeekTarget : uint8_t { Plant, Carcass, Creature };

void setFounderConnection(NeuralGenome& g, int inputIndex, int outputIndex, float weight) {
    for (auto& c : g.connections) {
        if (sameConnection(c, BrainNodeKind::Input, inputIndex, BrainNodeKind::Output, outputIndex)) {
            setNeuralConnectionEnabled(c,true);
            setNeuralConnectionMode(c,BrainConnectionMode::Add);
            c.weight = weight;
            return;
        }
    }
    pushConnectionIfNew(g, BrainNodeKind::Input, inputIndex, BrainNodeKind::Output, outputIndex, weight);
}

FounderSeekTarget randomFounderSeekTarget() {
    const float r = rf(0.0f,1.0f);
    if (r < cfg::tuning.founderSeekPlantChance) return FounderSeekTarget::Plant;
    if (r < cfg::tuning.founderSeekPlantChance + cfg::tuning.founderSeekCarcassChance)
        return FounderSeekTarget::Carcass;
    return FounderSeekTarget::Creature;
}

void seedFounderSeekBias(NeuralGenome& brain, const VisionGenes& vision, FounderSeekTarget target) {
    // Layout por raio preservado para compatibilidade: canal planta-solta permanece fixo em 0 na v0.098.
    const int feature = target == FounderSeekTarget::Plant ? 1
                      : target == FounderSeekTarget::Creature ? 2 : 3;
    const auto offsets = visionOffsets(vision);
    const float halfFov = std::max(1.0f, visionFov(vision.range) * 0.5f);

    for (int ray = 0; ray < (int)offsets.size(); ++ray) {
        const int input = ray * cfg::INPUTS_PER_RAY + feature;
        const float side = clampf(offsets[ray] / halfFov, -1.0f, 1.0f);
        const float mag = std::fabs(side);

        // Se o tipo escolhido aparece em qualquer raio, favorece frente e inibe re.
        setFounderConnection(brain,input,0, 3.20f);
        setFounderConnection(brain,input,1,-2.60f);

        // Raios a esquerda/direita empurram os motores antagonistas na direcao do alvo.
        if (side < -0.035f) {
            setFounderConnection(brain,input,2, 1.10f + 3.40f*mag); // esquerda
            setFounderConnection(brain,input,3,-0.80f - 2.20f*mag); // inibe direita
        } else if (side > 0.035f) {
            setFounderConnection(brain,input,3, 1.10f + 3.40f*mag); // direita
            setFounderConnection(brain,input,2,-0.80f - 2.20f*mag); // inibe esquerda
        }
    }
    ensureConnectivity(brain);
}

EmotionTriggerGene randomEmotionTrigger(const NeuralGenome& brain, int emotionCount) {
    EmotionTriggerGene t;
    const float r = rf(0.0f,1.0f);
    const float inputCut = cfg::tuning.emotionFounderInputSourceChance;
    const float hiddenCut = inputCut + cfg::tuning.emotionFounderHiddenSourceChance;
    const float outputCut = hiddenCut + cfg::tuning.emotionFounderOutputSourceChance;
    if (r < inputCut || (brain.hiddenCount<=0 && emotionCount<=0)) {
        t.sourceKind=EmotionSourceKind::Input;
        t.source=ri(0,cfg::BASE_INPUT_COUNT-1);
    } else if (r < hiddenCut && brain.hiddenCount>0) {
        t.sourceKind=EmotionSourceKind::Hidden;
        t.source=ri(0,brain.hiddenCount-1);
    } else if (r < outputCut) {
        t.sourceKind=EmotionSourceKind::Output;
        t.source=ri(0,cfg::OUTPUT_COUNT-1);
    } else if (emotionCount>0) {
        t.sourceKind=EmotionSourceKind::Emotion;
        t.source=ri(0,emotionCount-1);
    } else {
        t.sourceKind=EmotionSourceKind::Input;
        t.source=ri(0,cfg::BASE_INPUT_COUNT-1);
    }
    t.weight=rf(-cfg::tuning.emotionTriggerWeightLimit,cfg::tuning.emotionTriggerWeightLimit);
    return t;
}

EmotionGene randomEmotionGene(const NeuralGenome& brain, int emotionCount) {
    EmotionGene e;
    e.bias=rf(cfg::tuning.emotionBiasMin,cfg::tuning.emotionBiasMax);
    e.gain=rf(cfg::tuning.emotionGainMin,cfg::tuning.emotionGainMax);
    e.threshold=rf(cfg::tuning.emotionThresholdMin,cfg::tuning.emotionThresholdMax);
    e.strength=rf(cfg::tuning.emotionStrengthMin,cfg::tuning.emotionStrengthMax);
    e.impact=rf(cfg::tuning.emotionImpactMin,cfg::tuning.emotionImpactMax);
    e.riseTime=rf(cfg::tuning.emotionRiseTimeMin,cfg::tuning.emotionRiseTimeMax);
    e.duration=rf(cfg::tuning.emotionDurationMin,cfg::tuning.emotionDurationMax);
    const int minT=(int)std::lround(cfg::tuning.founderEmotionTriggerMin);
    const int maxT=(int)std::lround(cfg::tuning.founderEmotionTriggerMax);
    const int n=ri(std::min(minT,maxT),std::max(minT,maxT));
    e.triggers.reserve(n);
    for(int i=0;i<n;++i)e.triggers.push_back(randomEmotionTrigger(brain,emotionCount));
    return e;
}

EmotionGenome randomEmotionGenome(const NeuralGenome& brain) {
    EmotionGenome eg;
    const int lo=(int)std::lround(cfg::tuning.founderEmotionMin);
    const int hi=(int)std::lround(cfg::tuning.founderEmotionMax);
    const int count=ri(std::min(lo,hi),std::max(lo,hi));
    eg.emotions.reserve(count);
    for(int i=0;i<count;++i)eg.emotions.push_back(randomEmotionGene(brain,i));
    return eg;
}

void removeEmotionAt(EmotionGenome& eg,int idx){
    if(idx<0||idx>=(int)eg.emotions.size())return;
    eg.emotions.erase(eg.emotions.begin()+idx);
    for(auto& e:eg.emotions){
        e.triggers.erase(std::remove_if(e.triggers.begin(),e.triggers.end(),[&](const EmotionTriggerGene& t){
            return t.sourceKind==EmotionSourceKind::Emotion && t.source==idx;
        }),e.triggers.end());
        for(auto& t:e.triggers)if(t.sourceKind==EmotionSourceKind::Emotion&&t.source>idx)--t.source;
    }
}

void sanitizeEmotionTriggers(EmotionGenome& eg,const NeuralGenome& brain){
    const int ec=(int)eg.emotions.size();
    for(auto& e:eg.emotions){
        e.triggers.erase(std::remove_if(e.triggers.begin(),e.triggers.end(),[&](const EmotionTriggerGene& t){
            if(t.source<0)return true;
            if(t.sourceKind==EmotionSourceKind::Input)return t.source>=cfg::BASE_INPUT_COUNT;
            if(t.sourceKind==EmotionSourceKind::Hidden)return t.source>=brain.hiddenCount;
            if(t.sourceKind==EmotionSourceKind::Output)return t.source>=cfg::OUTPUT_COUNT;
            return t.source>=ec;
        }),e.triggers.end());
        for(auto& t:e.triggers)t.weight=clampf(t.weight,-cfg::tuning.emotionTriggerWeightLimit,cfg::tuning.emotionTriggerWeightLimit);
    }
}

EmotionGenome mutateEmotionGenome(const EmotionGenome& parent,const NeuralGenome& brain){
    EmotionGenome eg=parent;
    const int minE=(int)std::lround(cfg::tuning.emotionMinEffective);
    const int maxE=(int)std::lround(cfg::tuning.emotionMaxEffective);
    eg.emotions.reserve(std::min<std::size_t>((std::size_t)maxE, eg.emotions.size() + 1));
    const std::size_t triggerCap=(std::size_t)std::max(1,(int)std::lround(cfg::tuning.emotionMaxTriggersPerEmotion));
    for(auto& e:eg.emotions) e.triggers.reserve(std::min<std::size_t>(triggerCap,e.triggers.size()+1));
    while((int)eg.emotions.size()>maxE)removeEmotionAt(eg,(int)eg.emotions.size()-1);
    if((int)eg.emotions.size()<maxE && chance(cfg::tuning.emotionAddChance))
        eg.emotions.push_back(randomEmotionGene(brain,(int)eg.emotions.size()));
    if((int)eg.emotions.size()>minE && chance(cfg::tuning.emotionRemoveChance))
        removeEmotionAt(eg,ri(0,(int)eg.emotions.size()-1));

    auto mutateRel=[&](float& v,float chanceP,float strength,float lo,float hi){
        if(chance(chanceP))v=clampf(v*(1.0f+rf(-strength,strength)),lo,hi);
    };
    auto mutateAdd=[&](float& v,float chanceP,float strength,float lo,float hi){
        if(chance(chanceP))v=clampf(v+rf(-strength,strength),lo,hi);
    };
    for(auto& e:eg.emotions){
        mutateAdd(e.bias,cfg::tuning.emotionGeneMutationChance,cfg::tuning.emotionGeneMutationStrength,cfg::tuning.emotionBiasMin,cfg::tuning.emotionBiasMax);
        mutateRel(e.gain,cfg::tuning.emotionGeneMutationChance,cfg::tuning.emotionGeneMutationStrength,cfg::tuning.emotionGainMin,cfg::tuning.emotionGainMax);
        mutateAdd(e.threshold,cfg::tuning.emotionGeneMutationChance,cfg::tuning.emotionGeneMutationStrength,cfg::tuning.emotionThresholdMin,cfg::tuning.emotionThresholdMax);
        mutateRel(e.strength,cfg::tuning.emotionGeneMutationChance,cfg::tuning.emotionGeneMutationStrength,cfg::tuning.emotionStrengthMin,cfg::tuning.emotionStrengthMax);
        mutateRel(e.impact,cfg::tuning.emotionGeneMutationChance,cfg::tuning.emotionGeneMutationStrength,cfg::tuning.emotionImpactMin,cfg::tuning.emotionImpactMax);
        mutateRel(e.riseTime,cfg::tuning.emotionGeneMutationChance,cfg::tuning.emotionGeneMutationStrength,cfg::tuning.emotionRiseTimeMin,cfg::tuning.emotionRiseTimeMax);
        mutateRel(e.duration,cfg::tuning.emotionGeneMutationChance,cfg::tuning.emotionGeneMutationStrength,cfg::tuning.emotionDurationMin,cfg::tuning.emotionDurationMax);
        for(auto& t:e.triggers)if(chance(cfg::tuning.emotionTriggerWeightMutationChance))
            t.weight=clampf(t.weight+rf(-cfg::tuning.emotionTriggerWeightMutationStrength,cfg::tuning.emotionTriggerWeightMutationStrength),-cfg::tuning.emotionTriggerWeightLimit,cfg::tuning.emotionTriggerWeightLimit);
        if((int)e.triggers.size()<(int)std::lround(cfg::tuning.emotionMaxTriggersPerEmotion) && chance(cfg::tuning.emotionAddTriggerChance))
            e.triggers.push_back(randomEmotionTrigger(brain,(int)eg.emotions.size()));
        if(e.triggers.size()>1 && chance(cfg::tuning.emotionRemoveTriggerChance))
            e.triggers.erase(e.triggers.begin()+ri(0,(int)e.triggers.size()-1));
    }
    sanitizeEmotionTriggers(eg,brain);
    return eg;
}

DevelopmentGenes randomDevelopmentGenes(){
    DevelopmentGenes d;
    d.birthSizeFraction=rf(cfg::tuning.founderBirthSizeFractionMin,cfg::tuning.founderBirthSizeFractionMax);
    d.growthDuration=rf(cfg::tuning.founderGrowthDurationMin,cfg::tuning.founderGrowthDurationMax);
    d.growthCurveExponent=rf(cfg::tuning.founderGrowthCurveMin,cfg::tuning.founderGrowthCurveMax);
    d.adolescenceStartFraction=rf(cfg::tuning.founderAdolescenceStartMin,cfg::tuning.founderAdolescenceStartMax);
    d.sexualMaturityFraction=rf(cfg::tuning.founderMaturityFractionMin,cfg::tuning.founderMaturityFractionMax);
    d.longevity=rf(cfg::tuning.founderLongevityMin,cfg::tuning.founderLongevityMax);
    d.senescenceStartFraction=rf(cfg::tuning.founderSenescenceFractionMin,cfg::tuning.founderSenescenceFractionMax);
    return d;
}

DevelopmentGenes mutateDevelopmentGenes(const DevelopmentGenes& p){
    DevelopmentGenes d=p;
    const float c=cfg::tuning.lifecycleMutationChance, st=cfg::tuning.lifecycleMutationStrength;
    d.birthSizeFraction=mutateScalar(d.birthSizeFraction,c,st,cfg::tuning.birthSizeFractionMin,cfg::tuning.birthSizeFractionMax);
    d.growthDuration=mutateScalar(d.growthDuration,c,st,cfg::tuning.growthDurationMin,cfg::tuning.growthDurationMax);
    d.growthCurveExponent=mutateScalar(d.growthCurveExponent,c,st,cfg::tuning.growthCurveMin,cfg::tuning.growthCurveMax);
    d.adolescenceStartFraction=mutateScalar(d.adolescenceStartFraction,c,st,cfg::tuning.adolescenceStartMin,cfg::tuning.adolescenceStartMax);
    d.sexualMaturityFraction=mutateScalar(d.sexualMaturityFraction,c,st,cfg::tuning.maturityFractionMin,cfg::tuning.maturityFractionMax);
    d.longevity=mutateScalar(d.longevity,c,st,cfg::tuning.longevityMin,cfg::tuning.longevityMax);
    d.senescenceStartFraction=mutateScalar(d.senescenceStartFraction,c,st,cfg::tuning.senescenceFractionMin,cfg::tuning.senescenceFractionMax);
    return d;
}

CreatureGenome randomGenome() {
    CreatureGenome g;
    g.physical.maxForwardSpeed = cfg::tuning.founderForwardSpeed * rf(cfg::tuning.founderVariationMin, cfg::tuning.founderVariationMax);
    g.physical.maxReverseSpeed = cfg::tuning.founderReverseSpeed * rf(cfg::tuning.founderVariationMin, cfg::tuning.founderVariationMax);
    g.physical.lateralSpeedRatio = cfg::tuning.founderLateralSpeedRatio;
    g.physical.acceleration = cfg::tuning.founderAcceleration * rf(cfg::tuning.founderVariationMin, cfg::tuning.founderVariationMax);
    g.physical.friction = cfg::tuning.founderFriction * rf(cfg::tuning.founderVariationMin, cfg::tuning.founderVariationMax); // faixa fundadora -60%
    g.physical.rotationSpeed = cfg::tuning.founderRotation * rf(cfg::tuning.founderVariationMin, cfg::tuning.founderVariationMax);
    g.physical.bodySize = rf(cfg::tuning.founderMinSize, cfg::tuning.founderMaxSize);
    g.physical.hardness = rf(0.15f, 0.85f);
    g.mouth.biteForce = rf(cfg::tuning.founderMouthBiteMin, cfg::tuning.founderMouthBiteMax);
    g.mouth.maxOpening = rf(cfg::tuning.founderMouthOpeningMin, cfg::tuning.founderMouthOpeningMax);
    g.mouth.movementSpeed = rf(cfg::tuning.founderMouthSpeedMin, cfg::tuning.founderMouthSpeedMax);
    g.mouth.color = randomRgbColor();
    g.bodyColor = randomRgbColor();
    g.bodyColorControl = randomBodyColorControlGenes();
    g.vision.rayCount = normalizeRayCount(ri((int)std::lround(cfg::tuning.founderMinRays), (int)std::lround(cfg::tuning.founderMaxRays)));
    // Fundadores ocupam uma faixa ampla, mas extremos 40/5000 continuam
    // acessiveis por mutacao e selecao ao longo das geracoes.
    g.vision.range = rf(cfg::tuning.founderVisionMinRange, cfg::tuning.founderVisionMaxRange);
    g.vision.focusExponent=rf(cfg::tuning.founderVisionFocusMin,cfg::tuning.founderVisionFocusMax);
    g.vision.asymmetry=rf(-0.12f,0.12f);
    g.vision.sensitivityR=rf(0.80f,1.20f); g.vision.sensitivityG=rf(0.80f,1.20f); g.vision.sensitivityB=rf(0.80f,1.20f);
    g.vision.efficiency=rf(0.85f,1.15f);
    g.legacyRemovedSensor=LegacyRemovedSensorGenes{}; // legado binario, sem mecanica
    g.legacyRemovedSocial=LegacyRemovedSocialGenes{}; // legado binario, sem mecanica
    g.reproduction.eggEnergyFraction = rf(cfg::tuning.eggEnergyGeneMin, cfg::tuning.eggEnergyGeneMax);
    g.reproduction.eggBloodFraction = rf(cfg::tuning.eggBloodGeneMin, cfg::tuning.eggBloodGeneMax);
    g.reproduction.mateRange=rf(cfg::tuning.founderMateRangeMin,cfg::tuning.founderMateRangeMax);
    g.reproduction.visualPreference=rf(-0.35f,0.35f);
    g.reproduction.legacyRemovedPreference=0.0f; // legado binario
    g.reproduction.mateChoiceStrength=rf(0.15f,0.75f);
    g.development = randomDevelopmentGenes();
    g.brain = randomBrain();
    g.emotions = randomEmotionGenome(g.brain);
    return g;
}

CreatureGenome mutateGenome(const CreatureGenome& p) {
    CreatureGenome g;
    g.physical.maxForwardSpeed = mutateScalar(p.physical.maxForwardSpeed, cfg::tuning.bodyMutationChance, cfg::tuning.bodyMutationStrength, cfg::tuning.geneForwardMin, cfg::tuning.geneForwardMax);
    g.physical.maxReverseSpeed = mutateScalar(p.physical.maxReverseSpeed, cfg::tuning.bodyMutationChance, cfg::tuning.bodyMutationStrength, cfg::tuning.geneReverseMin, cfg::tuning.geneReverseMax);
    g.physical.lateralSpeedRatio = mutateScalar(p.physical.lateralSpeedRatio, cfg::tuning.bodyMutationChance, cfg::tuning.bodyMutationStrength, cfg::tuning.geneLateralRatioMin, cfg::tuning.geneLateralRatioMax);
    g.physical.acceleration = mutateScalar(p.physical.acceleration, cfg::tuning.bodyMutationChance, cfg::tuning.bodyMutationStrength, cfg::tuning.geneAccelerationMin, cfg::tuning.geneAccelerationMax);
    g.physical.friction = mutateScalar(p.physical.friction, cfg::tuning.bodyMutationChance, cfg::tuning.bodyMutationStrength, cfg::tuning.geneFrictionMin, cfg::tuning.geneFrictionMax);
    g.physical.rotationSpeed = mutateScalar(p.physical.rotationSpeed, cfg::tuning.bodyMutationChance, cfg::tuning.bodyMutationStrength, cfg::tuning.geneRotationMin, cfg::tuning.geneRotationMax);
    g.physical.bodySize = mutateScalar(p.physical.bodySize, cfg::tuning.sizeMutationChance, cfg::tuning.sizeMutationStrength, cfg::tuning.minCreatureSize, cfg::tuning.maxCreatureSize);
    g.physical.hardness = mutateScalar(p.physical.hardness, cfg::tuning.bodyMutationChance, cfg::tuning.bodyMutationStrength, cfg::tuning.geneHardnessMin, cfg::tuning.geneHardnessMax);
    g.mouth = mutateMouth(p.mouth);
    g.vision = mutateVision(p.vision);
    g.legacyRemovedSensor = LegacyRemovedSensorGenes{}; // bytes antigos neutralizados nos novos descendentes
    g.legacyRemovedSocial = LegacyRemovedSocialGenes{}; // bytes antigos neutralizados nos novos descendentes
    g.reproduction = p.reproduction;
    g.reproduction.eggEnergyFraction = mutateScalar(
        p.reproduction.eggEnergyFraction, cfg::tuning.eggMutationChance, cfg::tuning.eggMutationStrength,
        cfg::tuning.eggEnergyGeneMin, cfg::tuning.eggEnergyGeneMax);
    g.reproduction.eggBloodFraction = mutateScalar(
        p.reproduction.eggBloodFraction, cfg::tuning.eggMutationChance, cfg::tuning.eggMutationStrength,
        cfg::tuning.eggBloodGeneMin, cfg::tuning.eggBloodGeneMax);
    g.reproduction.mateRange=mutateScalar(p.reproduction.mateRange,cfg::tuning.mateTraitMutationChance,cfg::tuning.mateTraitMutationStrength,cfg::tuning.mateRangeMin,cfg::tuning.mateRangeMax);
    if(chance(cfg::tuning.mateTraitMutationChance))g.reproduction.visualPreference=clampf(p.reproduction.visualPreference+rf(-cfg::tuning.mateTraitMutationStrength,cfg::tuning.mateTraitMutationStrength),-1.0f,1.0f);
    g.reproduction.legacyRemovedPreference=0.0f; // legado binario inerte
    g.reproduction.mateChoiceStrength=mutateScalar(p.reproduction.mateChoiceStrength,cfg::tuning.mateTraitMutationChance,cfg::tuning.mateTraitMutationStrength,0.0f,1.0f);
    g.development = mutateDevelopmentGenes(p.development);
    g.bodyColor = mutateColorGene(p.bodyColor,cfg::tuning.bodyColorBaseMutationChance,cfg::tuning.bodyColorBaseMutationDelta);
    g.bodyColorControl = mutateBodyColorControl(p.bodyColorControl);
    g.brain = mutateBrain(p.brain);
    g.emotions = mutateEmotionGenome(p.emotions,g.brain);
    return g;
}


CreatureGenome breedGenome(const CreatureGenome& a,const CreatureGenome* bp){
    if(!bp) return mutateGenome(a);
    const CreatureGenome& b=*bp;
    CreatureGenome base=chance(0.5f)?a:b; // preserva uma topologia neural coerente inteira
    auto avg=[](float x,float y){return 0.5f*(x+y);};
    base.physical.maxForwardSpeed=avg(a.physical.maxForwardSpeed,b.physical.maxForwardSpeed);
    base.physical.maxReverseSpeed=avg(a.physical.maxReverseSpeed,b.physical.maxReverseSpeed);
    base.physical.lateralSpeedRatio=avg(a.physical.lateralSpeedRatio,b.physical.lateralSpeedRatio);
    base.physical.acceleration=avg(a.physical.acceleration,b.physical.acceleration);
    base.physical.friction=avg(a.physical.friction,b.physical.friction);
    base.physical.rotationSpeed=avg(a.physical.rotationSpeed,b.physical.rotationSpeed);
    base.physical.bodySize=avg(a.physical.bodySize,b.physical.bodySize);
    base.physical.hardness=avg(a.physical.hardness,b.physical.hardness);
    base.mouth.biteForce=avg(a.mouth.biteForce,b.mouth.biteForce);
    base.mouth.maxOpening=avg(a.mouth.maxOpening,b.mouth.maxOpening);
    base.mouth.movementSpeed=avg(a.mouth.movementSpeed,b.mouth.movementSpeed);
    base.mouth.color=mixRgb(a.mouth.color,b.mouth.color);
    base.vision.rayCount=normalizeRayCount((a.vision.rayCount+b.vision.rayCount+1)/2);
    base.vision.range=avg(a.vision.range,b.vision.range); base.vision.focusExponent=avg(a.vision.focusExponent,b.vision.focusExponent);
    base.vision.asymmetry=avg(a.vision.asymmetry,b.vision.asymmetry); base.vision.sensitivityR=avg(a.vision.sensitivityR,b.vision.sensitivityR);
    base.vision.sensitivityG=avg(a.vision.sensitivityG,b.vision.sensitivityG); base.vision.sensitivityB=avg(a.vision.sensitivityB,b.vision.sensitivityB); base.vision.efficiency=avg(a.vision.efficiency,b.vision.efficiency);
    base.reproduction.eggEnergyFraction=avg(a.reproduction.eggEnergyFraction,b.reproduction.eggEnergyFraction); base.reproduction.eggBloodFraction=avg(a.reproduction.eggBloodFraction,b.reproduction.eggBloodFraction);
    base.reproduction.mateRange=avg(a.reproduction.mateRange,b.reproduction.mateRange); base.reproduction.visualPreference=avg(a.reproduction.visualPreference,b.reproduction.visualPreference);
    base.legacyRemovedSensor=LegacyRemovedSensorGenes{}; base.legacyRemovedSocial=LegacyRemovedSocialGenes{};
    base.reproduction.legacyRemovedPreference=0.0f; base.reproduction.mateChoiceStrength=avg(a.reproduction.mateChoiceStrength,b.reproduction.mateChoiceStrength);
    base.development.birthSizeFraction=avg(a.development.birthSizeFraction,b.development.birthSizeFraction); base.development.growthDuration=avg(a.development.growthDuration,b.development.growthDuration);
    base.development.growthCurveExponent=avg(a.development.growthCurveExponent,b.development.growthCurveExponent); base.development.adolescenceStartFraction=avg(a.development.adolescenceStartFraction,b.development.adolescenceStartFraction);
    base.development.sexualMaturityFraction=avg(a.development.sexualMaturityFraction,b.development.sexualMaturityFraction); base.development.longevity=avg(a.development.longevity,b.development.longevity);
    base.development.senescenceStartFraction=avg(a.development.senescenceStartFraction,b.development.senescenceStartFraction);
    base.bodyColor=mixRgb(a.bodyColor,b.bodyColor);
    return mutateGenome(base);
}

struct Creature;
struct Plant;
struct Carcass;
struct Egg;

using GridKey = std::int64_t;
constexpr GridKey INVALID_GRID_KEY = std::numeric_limits<GridKey>::min();

inline GridKey makeGridKey(int cx,int cy){
    const std::uint64_t ux=(std::uint64_t)(std::uint32_t)cx;
    const std::uint64_t uy=(std::uint64_t)(std::uint32_t)cy;
    return (GridKey)((ux<<32)|uy);
}
inline int gridKeyX(GridKey key){return (int)(std::int32_t)((std::uint64_t)key>>32);}
inline int gridKeyY(GridKey key){return (int)(std::int32_t)((std::uint32_t)(std::uint64_t)key);}

struct GridTracked {
    GridKey gridCell = INVALID_GRID_KEY;
    int gridSlot = -1; // indice dentro do bucket para remocao O(1)
};

// Eventos acusticos foram removidos na v0.135.

template <typename T>
struct MainGridCellAccessor {
    static GridKey& cell(T* item) { return item->gridCell; }
    static int& slot(T* item) { return item->gridSlot; }
};

template <typename T>
struct DetachedGridCellAccessor {
    static GridKey& cell(T* item) { return item->detachedGridCell; }
    static int& slot(T* item) { return item->detachedGridSlot; }
};

// Grade espacial esparsa mantida como implementacao interna. O mundo e finito, mas
// alocar somente celulas ocupadas continua economizando memoria e nao altera a barreira fisica.
template <typename T, typename CellAccessor = MainGridCellAccessor<T>>
class SpatialGrid {
public:
    SpatialGrid(float cellSize, float width, float height):cellSize_(cellSize),width_(width),height_(height){rebuildDenseBuckets();}
    int cols() const { return cols_; }
    int rows() const { return rows_; }
    float cellSize() const { return cellSize_; }
    std::size_t allocatedBytes() const {
        std::size_t bytes=sizeof(*this)+buckets_.capacity()*sizeof(std::vector<T*>)+activeCells_.capacity()*sizeof(int)+activeCellSlot_.capacity()*sizeof(int);
        for(int idx:activeCells_) if(idx>=0&&idx<(int)buckets_.size()) bytes+=buckets_[(std::size_t)idx].capacity()*sizeof(T*);
        return bytes;
    }
    static int modCell(int a,int n){int r=a%n;return r<0?r+n:r;}
    GridKey cellFor(float x,float y) const {
        x=wrapCoord(x,width_);y=wrapCoord(y,height_);
        const int cx=std::clamp((int)std::floor(x/cellSize_),0,cols_-1);
        const int cy=std::clamp((int)std::floor(y/cellSize_),0,rows_-1);
        return makeGridKey(cx,cy);
    }
    void insert(T* item){
        item->pos.x=wrapCoord(item->pos.x,width_);item->pos.y=wrapCoord(item->pos.y,height_);
        const GridKey key=cellFor(item->pos.x,item->pos.y);
        const int idx=indexForKey(key);auto& b=buckets_[(std::size_t)idx];if(b.empty())activateCell(idx);
        CellAccessor::cell(item)=key;CellAccessor::slot(item)=(int)b.size();b.push_back(item);
    }
    void remove(T* item){
        const GridKey key=CellAccessor::cell(item);const int slot=CellAccessor::slot(item);const int idx=indexForKey(key);
        if(idx>=0){
            auto& b=buckets_[(std::size_t)idx];
            if(slot>=0&&slot<(int)b.size()&&b[(std::size_t)slot]==item){
                T* moved=b.back();b[(std::size_t)slot]=moved;b.pop_back();if(moved!=item)CellAccessor::slot(moved)=slot;
            }else{
                auto fit=std::find(b.begin(),b.end(),item);
                if(fit!=b.end()){const int found=(int)std::distance(b.begin(),fit);T* moved=b.back();b[(std::size_t)found]=moved;b.pop_back();if(moved!=item)CellAccessor::slot(moved)=found;}
            }
            if(b.empty())deactivateCell(idx);
        }
        CellAccessor::cell(item)=INVALID_GRID_KEY;CellAccessor::slot(item)=-1;
    }
    void update(T* item){
        item->pos.x=wrapCoord(item->pos.x,width_);item->pos.y=wrapCoord(item->pos.y,height_);
        const GridKey now=cellFor(item->pos.x,item->pos.y);if(now==CellAccessor::cell(item))return;
        remove(item);const int idx=indexForKey(now);auto& b=buckets_[(std::size_t)idx];if(b.empty())activateCell(idx);
        CellAccessor::cell(item)=now;CellAccessor::slot(item)=(int)b.size();b.push_back(item);
    }
    void clear(){
        for(int idx:activeCells_)if(idx>=0&&idx<(int)buckets_.size()){buckets_[(std::size_t)idx].clear();activeCellSlot_[(std::size_t)idx]=-1;}
        activeCells_.clear();
    }
    void reconfigure(float width,float height){width_=width;height_=height;rebuildDenseBuckets();}

    template <typename Fn> void forCircle(float x,float y,float radius,Fn&& fn) const {
        if(activeCells_.empty())return;
        x=wrapCoord(x,width_);y=wrapCoord(y,height_);radius=std::max(0.0f,radius);const float rr=radius*radius;
        if(radius>=0.5f*std::min(width_,height_)){
            for(int idx:activeCells_)for(T* item:buckets_[(std::size_t)idx]){
                const float dx=wrappedAxisDelta(item->pos.x-x,width_),dy=wrappedAxisDelta(item->pos.y-y,height_);if(dx*dx+dy*dy<=rr)fn(item);
            }
            return;
        }
        // O intervalo bruto pode cruzar a costura do toro. Visitamos cada coluna/linha
        // embrulhada no maximo uma vez; assim nao precisamos mais de unordered_set por consulta.
        const int rawSx=(int)std::floor((x-radius)/cellSize_),rawSy=(int)std::floor((y-radius)/cellSize_);
        const int rawEx=(int)std::floor((x+radius)/cellSize_),rawEy=(int)std::floor((y+radius)/cellSize_);
        const int nx=std::min(cols_,std::max(0,rawEx-rawSx+1)),ny=std::min(rows_,std::max(0,rawEy-rawSy+1));
        const int sx=modCell(rawSx,cols_),sy=modCell(rawSy,rows_);
        for(int oy=0;oy<ny;++oy){const int cy=(sy+oy)%rows_;for(int ox=0;ox<nx;++ox){const int cx=(sx+ox)%cols_;const auto& b=buckets_[(std::size_t)(cy*cols_+cx)];
            for(T* item:b){const float dx=wrappedAxisDelta(item->pos.x-x,width_),dy=wrappedAxisDelta(item->pos.y-y,height_);if(dx*dx+dy*dy<=rr)fn(item);}
        }}
    }
    template <typename Fn> bool anyCircle(float x,float y,float radius,Fn&& predicate) const {bool found=false;forCircle(x,y,radius,[&](T* item){if(!found&&predicate(item))found=true;});return found;}
    template <typename Fn> void forAABB(float minX,float minY,float maxX,float maxY,Fn&& fn) const {
        const int sx=(int)std::floor(minX/cellSize_),sy=(int)std::floor(minY/cellSize_),ex=(int)std::floor(maxX/cellSize_),ey=(int)std::floor(maxY/cellSize_);
        const std::int64_t nx=(std::int64_t)ex-(std::int64_t)sx+1,ny=(std::int64_t)ey-(std::int64_t)sy+1;const std::uint64_t cells=(nx>0&&ny>0)?(std::uint64_t)nx*(std::uint64_t)ny:0u;
        if(cells>std::max<std::uint64_t>(128u,(std::uint64_t)activeCells_.size()*6u)){
            for(int idx:activeCells_){const int cx=idx%cols_,cy=idx/cols_;if(cx<sx||cx>ex||cy<sy||cy>ey)continue;for(T* item:buckets_[(std::size_t)idx])fn(item);}return;
        }
        for(int cy=sy;cy<=ey;++cy)for(int cx=sx;cx<=ex;++cx){if(cx<0||cy<0||cx>=cols_||cy>=rows_)continue;for(T* item:buckets_[(std::size_t)(cy*cols_+cx)])fn(item);}
    }
    template <typename Fn> void forCellKeys(const std::vector<GridKey>& keys,Fn&& fn) const {for(GridKey key:keys)forCellKey(key,fn);}
    template <typename Fn> void forCellKey(GridKey key,Fn&& fn) const {const int idx=indexForKey(key);if(idx>=0)for(T* item:buckets_[(std::size_t)idx])fn(item);}
private:
    int indexForKey(GridKey key) const {if(key==INVALID_GRID_KEY)return -1;const int cx=gridKeyX(key),cy=gridKeyY(key);if(cx<0||cy<0||cx>=cols_||cy>=rows_)return -1;return cy*cols_+cx;}
    void activateCell(int idx){if(activeCellSlot_[(std::size_t)idx]>=0)return;activeCellSlot_[(std::size_t)idx]=(int)activeCells_.size();activeCells_.push_back(idx);}
    void deactivateCell(int idx){const int slot=activeCellSlot_[(std::size_t)idx];if(slot<0)return;const int moved=activeCells_.back();activeCells_[(std::size_t)slot]=moved;activeCells_.pop_back();activeCellSlot_[(std::size_t)idx]=-1;if(moved!=idx)activeCellSlot_[(std::size_t)moved]=slot;}
    void rebuildDenseBuckets(){cols_=std::max(1,(int)std::ceil(width_/cellSize_));rows_=std::max(1,(int)std::ceil(height_/cellSize_));buckets_.clear();buckets_.resize((std::size_t)cols_*(std::size_t)rows_);activeCells_.clear();activeCells_.reserve(std::min<std::size_t>(buckets_.size(),4096u));activeCellSlot_.assign(buckets_.size(),-1);}
    float cellSize_;float width_,height_;int cols_=1,rows_=1;
    std::vector<std::vector<T*>> buckets_;
    std::vector<int> activeCells_;
    std::vector<int> activeCellSlot_;
};
struct RayCellCollector {
    struct Entry { GridKey key=INVALID_GRID_KEY; float minDistance=0.0f; int tileX=0,tileY=0; };
    float cellSize=cfg::CELL;
    std::unordered_set<GridKey> seenUnwrapped;
    std::vector<Entry> entries;
    RayCellCollector(){seenUnwrapped.reserve(512);entries.reserve(256);}
    static int floorDiv(int a,int b){int q=a/b,r=a%b;if(r<0)--q;return q;}
    static int modCell(int a,int b){int r=a%b;return r<0?r+b:r;}
    void addCell(int ux,int uy,float minDistance,int cols,int rows){
        const GridKey unwrappedKey=makeGridKey(ux,uy);
        if(!seenUnwrapped.insert(unwrappedKey).second)return;
        const int tx=floorDiv(ux,cols),ty=floorDiv(uy,rows);
        const int cx=modCell(ux,cols),cy=modCell(uy,rows);
        entries.push_back({makeGridKey(cx,cy),std::max(0.0f,minDistance),tx,ty});
    }
    const std::vector<Entry>& collect(Vector2 origin,Vector2 dir,float range){
        entries.clear();seenUnwrapped.clear();
        const int cols=std::max(1,(int)std::ceil(cfg::WORLD_W/cellSize));
        const int rows=std::max(1,(int)std::ceil(cfg::WORLD_H/cellSize));
        origin=wrappedPoint(origin);
        int cx=(int)std::floor(origin.x/cellSize),cy=(int)std::floor(origin.y/cellSize);
        const int stepX=dir.x>0?1:(dir.x<0?-1:0),stepY=dir.y>0?1:(dir.y<0?-1:0);
        const float inf=1e30f;
        const float nextBoundaryX=stepX>0?float(cx+1)*cellSize:float(cx)*cellSize;
        const float nextBoundaryY=stepY>0?float(cy+1)*cellSize:float(cy)*cellSize;
        float tMaxX=stepX==0?inf:(nextBoundaryX-origin.x)/dir.x;
        float tMaxY=stepY==0?inf:(nextBoundaryY-origin.y)/dir.y;
        const float tDeltaX=stepX==0?inf:cellSize/std::fabs(dir.x),tDeltaY=stepY==0?inf:cellSize/std::fabs(dir.y);
        if(tMaxX<0)tMaxX=0;
        if(tMaxY<0)tMaxY=0;
        float traveled=0.0f;const float neighborReach=cellSize*1.45f;
        while(traveled<=range){
            const float minD=std::max(0.0f,traveled-neighborReach);
            for(int oy=-1;oy<=1;++oy)for(int ox=-1;ox<=1;++ox)addCell(cx+ox,cy+oy,minD,cols,rows);
            if(tMaxX<tMaxY){traveled=tMaxX;tMaxX+=tDeltaX;cx+=stepX;}else{traveled=tMaxY;tMaxY+=tDeltaY;cy+=stepY;}
        }
        std::sort(entries.begin(),entries.end(),[](const Entry&a,const Entry&b){return a.minDistance<b.minDistance;});
        return entries;
    }
};

float rayCircle(Vector2 origin, Vector2 dir, Vector2 center, float radius, float range) {
    Vector2 oc = sub(center, origin);
    float proj = dot(oc, dir);
    float oc2 = length2(oc);
    float r2 = radius * radius;
    if (proj < 0.0f && oc2 > r2) return -1.0f;
    // Se ate a borda mais proxima do circulo ja esta alem do melhor hit atual,
    // nao precisamos calcular perpendicular/sqrt para este candidato.
    if (proj - radius > range) return -1.0f;
    float perp2 = oc2 - proj * proj;
    if (perp2 > r2) return -1.0f;
    float thc = std::sqrt(std::max(0.0f, r2 - perp2));
    float t = proj - thc;
    if (t < 0.0f) t = proj + thc;
    if (t < 0.0f || t > range) return -1.0f;
    return t;
}

// Aproximacao sensorial da silhueta do trevo: tres folhas + miolo.
// Evita a antiga "bola invisivel" circular em volta do PNG da planta.
float rayTrefoil(Vector2 origin, Vector2 dir, Vector2 center, float radius, float range) {
    const float leafR = radius * 0.55f;
    const Vector2 centers[4] = {
        {center.x,                 center.y - radius*0.30f},
        {center.x - radius*0.27f, center.y + radius*0.22f},
        {center.x + radius*0.27f, center.y + radius*0.22f},
        {center.x,                 center.y + radius*0.02f}
    };
    const float radii[4] = {leafR,leafR,leafR,radius*0.30f};
    float best=-1.0f;
    for(int i=0;i<4;++i){
        const float hit=rayCircle(origin,dir,centers[i],radii[i],range);
        if(hit>=0.0f && (best<0.0f || hit<best)) best=hit;
    }
    return best;
}

struct Plant : GridTracked {
    GridKey detachedGridCell = INVALID_GRID_KEY;
    int detachedGridSlot = -1;
    Vector2 pos{};
    float size = cfg::tuning.plantBirthSize;
    float biomassSize = cfg::tuning.plantBirthSize; // recurso real; size acompanha a biomassa nas mordidas
    bool hasBeenEaten = false;
    float growthDuration = cfg::tuning.plantGrowth;
    float reproTimer = 0.0f;
    float nextRepro = cfg::tuning.plantRepro;
    PlantGenes genes{};
    Vector2 visualOffset{};
    // Estado temporario usado apenas no passe de vegetacao. Nao e DNA nem save.
    Vector2 pendingSway{};
    bool swaySuppressedThisPass = false;
    bool detached = false;          // enraizada ate receber a primeira mordida
    // Planta solta nao participa da fisica dinamica. velocity serve somente como
    // informacao instantanea enquanto um bicho a carrega.
    // Fora do agarrar ela permanece sempre zero e a posicao fica fixa.
    Vector2 velocity{};
    float detachedAge = 0.0f;
    float detachedDuration = 0.0f;
    float detachedDecayPerSecond = 0.0f;
    int grabbedByCreatureId = -1;

    Plant(Vector2 p, PlantGenes g = PlantGenes{}) : pos(p), genes(g) {
        // Mesmo se um config antigo vier com nascimento < 1, nenhuma planta nasce
        // visual/fisicamente menor que o fragmento minimo.
        size = std::max(cfg::PLANT_MIN_FRAGMENT_SIZE, cfg::tuning.plantBirthSize);
        biomassSize=size;
        growthDuration = std::max(0.5f, cfg::tuning.plantGrowth + rf(-cfg::tuning.plantGrowthJitter, cfg::tuning.plantGrowthJitter));
        nextRepro = std::max(0.5f, cfg::tuning.plantRepro + rf(-cfg::tuning.plantReproJitter, cfg::tuning.plantReproJitter));
    }

    float adultSizeTarget() const {
        return std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,
            std::max(cfg::tuning.plantBirthSize,
                cfg::tuning.plantMaxSize*clampf(genes.sizeScale,cfg::tuning.plantSizeScaleMin,cfg::tuning.plantSizeScaleMax)));
    }
    const char* sizeClassName() const { const float sc=genes.sizeScale; return sc<0.78f?"Pequena":(sc>1.28f?"Grande":"Media"); }

    bool update(float dt) {
        // v0.098: toda planta permanece enraizada e visualmente fixa para sempre.
        // Campos de planta solta continuam no struct apenas para compatibilidade binaria
        // com saves antigos, mas nao participam mais da jogabilidade.
        detached = false;
        velocity = {0.0f,0.0f};
        detachedAge = detachedDuration = detachedDecayPerSecond = 0.0f;
        grabbedByCreatureId = -1;
        pendingSway = {0.0f,0.0f};
        swaySuppressedThisPass = false;

        const float adultTarget=adultSizeTarget();
        if (!hasBeenEaten && size < adultTarget) {
            const float birthSize = std::max(cfg::PLANT_MIN_FRAGMENT_SIZE, cfg::tuning.plantBirthSize);
            float grow = (adultTarget - birthSize) / growthDuration;
            size = std::min(adultTarget, size + grow * dt);
            size = std::max(cfg::PLANT_MIN_FRAGMENT_SIZE, size);
            biomassSize=size;
        }

        // v0.100+: depois da primeira mordida NAO existe qualquer encolhimento por tempo.
        // O tamanho ja foi atualizado atomicamente em consume(), na mesma proporcao da
        // biomassa/energia removida naquela mordida. Entre mordidas ele fica 100% constante.
        if (hasBeenEaten) {
            // v0.122: plantas parcialmente comidas nao travam mais a reproducao.
            // Elas apenas param de crescer; se ainda tiverem biomassa suficiente, continuam
            // biologicamente ativas e podem reproduzir mais devagar.
            if (biomassSize > 0.001f && size > biomassSize + 0.001f)
                size = std::max(cfg::PLANT_MIN_FRAGMENT_SIZE, biomassSize);
            if (biomassSize < std::max(cfg::PLANT_MIN_FRAGMENT_SIZE, adultTarget * 0.45f))
                return false;
        }

        reproTimer += dt;
        const float reproSpeedMult = hasBeenEaten ? 0.55f : 1.0f;
        if (reproTimer >= nextRepro / reproSpeedMult) {
            reproTimer -= nextRepro;
            nextRepro = std::max(0.5f, cfg::tuning.plantRepro + rf(-cfg::tuning.plantReproJitter, cfg::tuning.plantReproJitter));
            return true;
        }
        return false;
    }

    float radius() const { return std::max(cfg::tuning.plantRadiusMin, std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,size) * cfg::tuning.plantRadiusScale); }
    // A biologia continua usando radius()/pos. A visao e o sprite usam a copa
    // visual deslocada pelo sway e com a mesma escala desenhada na tela.
    float visualRadius() const { return std::max(0.22f, radius() * 0.60f); }
    Vector2 visualCenter() const { return {pos.x + visualOffset.x, pos.y + visualOffset.y}; }
    float mass() const { return std::max(cfg::tuning.plantMassMin, cfg::tuning.plantMassDensity * std::pow(std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,size), cfg::tuning.plantMassSizeExponent)); }

    // Compatibilidade com codigo/save antigo: nao existe mais corpo fisico de planta.
    float physicalSize() const { return std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,size); }
    float physicalRadius() const { return radius(); }
    float physicalMass() const { return mass(); }
    void beginDetachedState(){
        // LEGADO v0.098: desenraizamento foi removido. A planta permanece no lugar.
        detached=false; velocity={0.0f,0.0f}; detachedAge=detachedDuration=detachedDecayPerSecond=0.0f; grabbedByCreatureId=-1;
    }

    float consume(float requested) {
        if(requested<=0.0f || dead() || biomassSize<=0.001f) return 0.0f;
        hasBeenEaten = true;
        detached=false; velocity={0.0f,0.0f}; grabbedByCreatureId=-1;

        // O tamanho atual funciona como resistencia de consumo. O expoente e configuravel:
        // 0 = nenhuma protecao por tamanho; 1 = resistencia linear.
        const float effectiveSize = std::max(cfg::PLANT_MIN_FRAGMENT_SIZE, biomassSize);
        const float sizeResistance = std::pow(
            std::max(1.0f, effectiveSize / cfg::PLANT_MIN_FRAGMENT_SIZE),
            cfg::tuning.plantBiteSizeResistanceExponent);
        const float sizeAdjustedRequest = std::max(0.0f, requested) / std::max(1.0f, sizeResistance);

        const float biomassBefore = biomassSize;
        const float sizeBefore = size;
        const float actual = std::min(biomassBefore, sizeAdjustedRequest);
        biomassSize = std::max(0.0f, biomassBefore-actual);
        if (biomassSize < 0.0001f) biomassSize = 0.0f;

        // REGRA v0.100: a fracao de recurso restante vira a mesma fracao de tamanho.
        // Ex.: consumiu 50% da energia/biomassa desta planta -> tamanho cai para 50%
        // imediatamente nesta mordida. Nao ha animacao/decomposicao depois.
        const float remainingFraction = biomassBefore > 0.0001f
            ? clampf(biomassSize/biomassBefore,0.0f,1.0f) : 0.0f;
        size = biomassSize > 0.0f
            ? std::max(cfg::PLANT_MIN_FRAGMENT_SIZE, sizeBefore*remainingFraction)
            : cfg::PLANT_MIN_FRAGMENT_SIZE;
        return actual;
    }

    bool dead() const {
        // Sem decomposicao: desaparece assim que a ultima biomassa/energia e comida.
        return biomassSize <= 0.001f;
    }
    float energyFor(float removed) const { return cfg::tuning.plantFullEnergy * clampf(removed / cfg::tuning.plantMaxSize, 0.0f, 1.0f); }
    float partnerRadius() const { return size * cfg::tuning.plantPartnerRadiusScale; }

    void sway(Vector2 velocity) {
        visualOffset.x += velocity.x * cfg::tuning.plantSwayVelocityScale;
        visualOffset.y += velocity.y * cfg::tuning.plantSwayVelocityScale;
        float m2 = length2(visualOffset);
        float maxOff = cfg::tuning.plantSwayMaxOffset;
        if (m2 > maxOff * maxOff) {
            float m = std::sqrt(m2);
            visualOffset = mul(visualOffset, maxOff / m);
        }
    }
};

struct Carcass : GridTracked {
    Vector2 pos{};
    float bodySize = 1.0f;
    float age = 0.0f;
    float duration = 12.0f;
    float initialEnergy = 3.0f;
    float energy = 3.0f;
    float maxBite = 1.2f;
    float sensorRadius = 0.6f;
    float visualSize = 1.55f;
    float glowRadius = 1.25f;
    float decayPerSecond = 0.25f;
    Vector2 velocity{};
    int grabbedByCreatureId = -1;

    Carcass(Vector2 p, float size) : pos(p), bodySize(size) {
        float ratio = bodySize / cfg::tuning.carcassRefSize;
        initialEnergy = cfg::tuning.carcassBaseEnergy * std::pow(ratio,cfg::tuning.carcassEnergySizeExponent);
        energy = initialEnergy;
        duration = cfg::tuning.carcassBaseDuration * std::pow(ratio,cfg::tuning.carcassDurationSizeExponent);
        maxBite = cfg::tuning.carcassBaseBite * std::pow(ratio,cfg::tuning.carcassBiteSizeExponent);
        sensorRadius = std::max(0.25f, bodySize * cfg::tuning.carcassSensorScale);
        visualSize = std::max(1.0f, bodySize * cfg::tuning.carcassVisualScale);
        glowRadius = std::max(1.0f, bodySize * cfg::CARCASS_GLOW_SCALE);
        decayPerSecond = duration > 0.0f ? initialEnergy / duration : initialEnergy;
    }

    void update(float dt) {
        age += dt;
        energy = std::max(0.0f, energy - decayPerSecond * dt);
        pos = add(pos, mul(velocity, dt));
        const float damp = std::exp(-cfg::PHYSICS_OBJECT_DAMPING * cfg::tuning.objectDamping * dt);
        velocity = mul(velocity, damp);
        resolveCircleWorldBarrier(pos, velocity, radius());
    }
    float radius() const { return std::max(0.08f, visualSize * fraction()); }
    float mass() const { return std::max(cfg::tuning.carcassMassMin, cfg::tuning.carcassMassDensity * std::pow(bodySize,cfg::tuning.carcassMassSizeExponent)); }
    float fraction() const { return initialEnergy > 0.0f ? clampf(energy / initialEnergy, 0.0f, 1.0f) : 0.0f; }
    float bite(float power) { float req = maxBite * clampf(power, 0.0f, cfg::tuning.carcassBitePowerMax); float actual = std::min(req, energy); energy -= actual; return actual; }
    bool finished() const { return energy <= 0.01f || age >= duration; }
};


struct Egg : GridTracked {
    Vector2 pos{};
    CreatureGenome genome{};
    int generation = 1;
    int parentId = -1;
    int secondParentId = -1;
    bool minimumReplacement = false; // reserva uma vaga do minimo populacional

    float childSize = 3.0f;
    float childMaxEnergy = 10.0f;
    float radius = 1.0f;
    float age = 0.0f;
    float incubationDuration = 10.0f;

    float initialEnergy = 1.0f;
    float energy = 1.0f;
    float initialBlood = 1.0f;
    float blood = 1.0f;
    float maxBite = 1.0f;
    Vector2 velocity{};

    // v0.110: estado aprendido preparado no momento da fecundacao. Fica dentro
    // do ovo para sobreviver a morte dos pais e a save/load durante a incubacao.
    std::vector<float> inheritedLearnedWeights;
    std::array<float,cfg::REGISTER_MEMORY_SLOTS> inheritedMemoryRegisters{};
    // v0.135: filhos de acasalamento carregam tambem memoria episodica consolidada
    // dos dois pais. O ovo guarda isso para sobreviver a save/load e morte dos pais.
    std::array<LongMemoryEvent,cfg::LONG_MEMORY_SLOTS> inheritedLongMemory{};
    std::uint8_t inheritedLongMemoryHead = 0;
    std::uint8_t inheritedLongMemoryCount = 0;
    bool hasInheritedLearning = false;
    float qualityScore = 0.0f;
    int plannedBirthCount = 1;
    bool conceivedWithMate = false;
    std::uint16_t sexualHeritageDepth = 0;

    Egg(Vector2 p, CreatureGenome g, int gen, int parent, float energyInvestment, float bloodInvestment)
        : pos(p), genome(std::move(g)), generation(gen), parentId(parent) {
        childSize = clampf(genome.physical.bodySize, cfg::tuning.minCreatureSize, cfg::tuning.maxCreatureSize);
        childMaxEnergy = creatureMaxEnergyForSize(childSize);
        radius = 0.50f * std::max(cfg::tuning.eggRadiusMin, cfg::tuning.eggRadiusBase + childSize * cfg::tuning.eggRadiusSizeScale);

        initialEnergy = energy = std::max(0.0f, energyInvestment);
        initialBlood = blood = std::max(0.0f, bloodInvestment);

        const float eQ = clampf(
            initialEnergy / std::max(0.01f, childMaxEnergy * cfg::tuning.eggEnergyFullBirthFraction),
            0.0f, 1.0f);
        const float bQ = clampf(
            initialBlood / (cfg::tuning.healthMax * cfg::tuning.eggBloodFullBirthFraction),
            0.0f, 1.0f);
        const float qualityWeightSum = std::max(0.0001f, cfg::tuning.eggQualityEnergyWeight + cfg::tuning.eggQualityBloodWeight);
        const float quality = (eQ * cfg::tuning.eggQualityEnergyWeight + bQ * cfg::tuning.eggQualityBloodWeight) / qualityWeightSum;
        qualityScore = clampf(quality,0.0f,1.20f);
        incubationDuration = cfg::tuning.eggIncubationMax +
            (cfg::tuning.eggIncubationMin - cfg::tuning.eggIncubationMax) * clampf(qualityScore,0.0f,1.0f);
        maxBite = std::max(cfg::tuning.eggBiteMin, radius * cfg::tuning.eggBiteRadiusScale);
    }

    void update(float dt) {
        age += dt;
        pos = add(pos, mul(velocity, dt));
        const float damp = std::exp(-cfg::PHYSICS_OBJECT_DAMPING * cfg::tuning.objectDamping * dt);
        velocity = mul(velocity, damp);
        resolveCircleWorldBarrier(pos, velocity, radius);
    }

    float mass() const {
        // Casca + recursos + tamanho do futuro filhote influenciam o peso.
        const float resourceMass = cfg::tuning.eggMassEnergyCoeff * std::max(0.0f, energy) + cfg::tuning.eggMassBloodCoeff * std::max(0.0f, blood);
        return std::max(cfg::tuning.eggMassMin, cfg::tuning.eggMassSizeCoeff * std::pow(childSize, cfg::tuning.eggMassSizeExponent) + resourceMass);
    }

    float edibleValue() const {
        return std::max(0.0f, energy) + std::max(0.0f, blood) * cfg::tuning.eggBloodFoodValue;
    }

    float bite(float power) {
        const float available = edibleValue();
        if (available <= 0.0f) return 0.0f;
        const float requested = maxBite * clampf(power, 0.0f, cfg::tuning.eggBitePowerMax);
        const float taken = std::min(requested, available);
        const float fractionTaken = clampf(taken / available, 0.0f, 1.0f);
        energy *= (1.0f - fractionTaken);
        blood *= (1.0f - fractionTaken);
        return taken;
    }

    float resourceFraction() const {
        const float initial = initialEnergy + initialBlood * cfg::tuning.eggBloodFoodValue;
        return initial > 0.0f ? clampf(edibleValue() / initial, 0.0f, 1.0f) : 0.0f;
    }

    bool destroyed() const {
        return energy <= cfg::tuning.eggMinResource && blood <= cfg::tuning.eggMinResource;
    }

    bool readyToHatch() const {
        return age >= incubationDuration && !destroyed();
    }

    float childEnergyBase() const {
        const float ratio = clampf(
            energy / std::max(0.01f, childMaxEnergy * cfg::tuning.eggEnergyFullBirthFraction),
            0.01f, 1.0f);
        // Creature recebe um valor base de 0..100 e converte para sua propria barra maxima.
        return ratio * cfg::tuning.energyMaxBase;
    }

    float childHealth() const {
        const float ratio = clampf(
            blood / (cfg::tuning.healthMax * cfg::tuning.eggBloodFullBirthFraction),
            0.01f, 1.0f);
        return cfg::tuning.healthMax * ratio;
    }
};

enum class GrabKind : std::uint8_t { None, Plant, Carcass, Creature };

enum class DeathCause : std::uint8_t { Unknown=0, NaturalAge=1, Starvation=2, Attack=3, Poison=4 };

struct Creature : GridTracked {
    int id = 0;
    Vector2 pos{};
    float angle = 0.0f;
    float sinA = 0.0f;
    float cosA = 1.0f;
    float speed = 0.0f;
    int generation = 1;
    int parentId = -1;
    int secondParentId = -1;
    CreatureGenome genome{};
    CompiledBrain compiled{};

    float size = 3.0f;
    float energy = 10.0f;
    float health = cfg::tuning.healthMax;
    float age = 0.0f; // idade em segundos simulados
    float lifecycleTimer = 0.0f;
    float developmentProgress = 0.0f;
    float senescenceProgress = 0.0f;
    float ageSpeedMult = 1.0f;
    float ageAccelMult = 1.0f;
    float ageRotationMult = 1.0f;
    float ageBiteMult = 1.0f;
    float ageRegenMult = 1.0f;
    float ageMetabolismMult = 1.0f;
    float ageFertilityMult = 1.0f;
    float longevityMaintenanceMult = 1.0f;
    bool naturalDeath = false;
    DeathCause deathCause = DeathCause::Unknown;
    float reproCooldown = 0.0f;
    float biteCooldown = 0.0f;
    float biteAnimation = 0.0f;
    float brainTimer = 0.0f;

    std::vector<float> rayOffsets;
    std::vector<float> raySin;
    std::vector<float> rayCos;
    std::vector<RayReading> rays;
    std::array<float, cfg::INPUT_COUNT> inputs{};
    std::vector<float> hidden;
    // Estado neural linear: inputs atuais + ocultos atuais + ocultos anteriores + saidas anteriores.
    // As conexoes compiladas guardam indices diretos para este array.
    std::array<float, cfg::BRAIN_STATE_COUNT> brainState{};
    std::array<float, cfg::OUTPUT_COUNT> netOut{};
    std::array<float, cfg::OUTPUT_COUNT> brainOut{};
    std::array<float, cfg::MAX_EMOTIONS> emotionState{};
    std::array<float, cfg::MAX_EMOTIONS> emotionDrive{};

    // Cor do corpo e estado fenotipico dinamico; genome.bodyColor e apenas a base herdada.
    Color displayColor{229,57,53,255};
    float maxEnergy = 10.0f;
    float sizeSpeedMult = 1.0f;
    float sizeAccelMult = 1.0f;
    float sizeRotMult = 1.0f;
    float sizeEnergyCostMult = 1.0f;
    float sizeBitePowerMult = 1.0f;
    float mouthSizeFactor = 1.0f;
    float hardnessSpeedMult = 1.0f;
    float damageMult = 1.0f;
    float maxForwardEffective = 100.0f;
    float maxReverseEffective = 60.0f;
    float maxLateralEffective = 75.0f;
    float lateralSpeed = 0.0f; // negativo=esquerda, positivo=direita no referencial do corpo
    float accelEffective = 100.0f;
    float rotEffective = 100.0f;
    float bitePowerCached = 0.5f;
    float biteCooldownCached = 0.5f;
    float vegetationFrictionMult = 1.0f;
    float rootedPlantCover = 0.0f; // 1 quando esta sob a copa de planta ainda enraizada
    // Copa dominante calculada junto da cobertura e reutilizada pelo compute shader
    // para manter a regra de esconder bicho sob planta sem nova busca espacial na GPU.
    Vector2 rootedCoverPlantPos{};
    float rootedCoverPlantRadius = 0.0f;
    // Scratch de profiling visual. Nao e DNA e nao entra no save.
    std::uint32_t perfVisionRays = 0, perfVisionCells = 0;
    std::uint32_t perfVisionPlantChecks = 0, perfVisionCreatureChecks = 0;
    std::uint32_t perfVisionCarcassChecks = 0, perfVisionEggChecks = 0;
    Vector2 pushVelocity{}; // impulso lateral/externo recebido por colisoes
    float angularVelocity = 0.0f; // graus/s no ultimo subpasso
    std::array<float,4> contactPressure{{0,0,0,0}}; // frente,direita,tras,esquerda
    float recentImpact = 0.0f;
    float recentDamage = 0.0f;
    float recentEnergyGain = 0.0f;

    // Memoria de longo prazo por eventos. Ring buffer fixo e compacto; nao substitui
    // memoria recorrente dos neuronios, apenas oferece fatos discretos de dezenas de segundos.
    std::array<LongMemoryEvent,cfg::LONG_MEMORY_SLOTS> longMemory{};
    std::uint8_t longMemoryHead = 0;   // proxima posicao a sobrescrever
    std::uint8_t longMemoryCount = 0;  // quantos slots ja contem evento valido

    // v0.109: memoria neural persistente controlada pelas proprias saidas da rede.
    // Nao decai. O slot so muda quando o respectivo gate de escrita supera o limiar.
    std::array<float,cfg::REGISTER_MEMORY_SLOTS> memoryRegisters{};
    // Fenotipo sinaptico aprendido durante a vida. genome.brain.connections[*].weight
    // continua sendo o peso genetico-base; neste fork os deltas adquiridos tambem podem ser herdados.
    std::vector<float> learnedWeights;
    std::vector<float> eligibilityTraces;
    float lastPlasticReward = 0.0f;
    std::uint64_t plasticUpdates = 0;

    // v0.116: reward externo configuravel. Pending acumula eventos entre dois ticks
    // cerebrais; os demais campos servem para observar/comparar o treinamento.
    float reinforcementPending = 0.0f;
    float reinforcementTotal = 0.0f;
    float reinforcementPositive = 0.0f;
    float reinforcementNegative = 0.0f;
    std::uint64_t reinforcementEvents = 0;
    // v0.116: metricas de vida usadas pelos marcadores de recorde do RL.
    float timeHealthAbove50 = 0.0f, timeHealthBelow50 = 0.0f; // below = cronometro atual 50%->0%; negativo = tentativa concluida
    float timeEnergyAbove50 = 0.0f, timeEnergyBelow50 = 0.0f; // idem para energia
    int eggsLaidLifetime = 0;
    int matingPartnerId = -1;
    float matingContactTime = 0.0f;
    std::uint16_t sexualHeritageDepth = 0; // persiste: quantidade de ancestrais sexuais consecutivos/consolidados
    bool matingTouchThisFrame = false; // runtime only; nao precisa ser salvo
    float matingSearchGrace = 0.0f;     // runtime only; diagnostico/espera curta
    float soloFallbackTimer = 0.0f;     // runtime only; tempo pronto sem concluir acasalamento

    void reinforce(float reward) {
        if(cfg::tuning.reinforcementEnabled <= 0.5f || std::fabs(reward) <= 1e-8f) return;
        reinforcementPending = clampf(reinforcementPending + reward, -50.0f, 50.0f);
        ++reinforcementEvents;
    }

    // Agarrar/carregar: um bicho segura no maximo um alvo; cada alvo aceita um carregador.
    GrabKind grabbedKind = GrabKind::None;
    Plant* grabbedPlant = nullptr;
    Carcass* grabbedCarcass = nullptr;
    Creature* grabbedCreature = nullptr;
    int grabbedByCreatureId = -1;
    float grabLoadRatio = 0.0f; // massa da carga / massa do carregador
    int speciesId = 0; // classificacao analitica, nao faz parte do DNA

    Creature(int id_, Vector2 p, CreatureGenome g, int gen, int parent, float initialEnergyBase)
        : id(id_), pos(p), generation(gen), parentId(parent), genome(std::move(g)) {
        const float adult = clampf(genome.physical.bodySize, cfg::tuning.minCreatureSize, cfg::tuning.maxCreatureSize);
        const float birthFrac = clampf(genome.development.birthSizeFraction,cfg::tuning.birthSizeFractionMin,cfg::tuning.birthSizeFractionMax);
        size = std::max(0.05f,adult*birthFrac);
        displayColor = genome.bodyColor;
        // Antes da primeira decisao neural, as saidas cromaticas apontam para a cor-base.
        auto initialColorOutput=[&](float base,float mn,float mx){
            return (mx>mn+1e-6f)?clampf((base-mn)/(mx-mn),0.0f,1.0f):0.5f;
        };
        const float br=float(genome.bodyColor.r)/255.0f, bg=float(genome.bodyColor.g)/255.0f, bb=float(genome.bodyColor.b)/255.0f;
        netOut[cfg::COLOR_OUTPUT_BASE+0]=initialColorOutput(br,genome.bodyColorControl.rMin,genome.bodyColorControl.rMax);
        netOut[cfg::COLOR_OUTPUT_BASE+1]=initialColorOutput(bg,genome.bodyColorControl.gMin,genome.bodyColorControl.gMax);
        netOut[cfg::COLOR_OUTPUT_BASE+2]=initialColorOutput(bb,genome.bodyColorControl.bMin,genome.bodyColorControl.bMax);
        angle = rf(0.0f, 360.0f);
        refreshDirection();
        cachePhenotype();
        float initialRatio = clampf(initialEnergyBase / cfg::tuning.energyMaxBase, 0.0f, 1.0f);
        energy = maxEnergy * initialRatio;
        brainTimer = rf(0.0f, cfg::tuning.brainInterval);
        ensureBrainOutputStorage(genome.brain);
        compiled = compileBrain(genome.brain);
        hidden.assign(genome.brain.hiddenCount, 0.0f);
        learnedWeights.resize(genome.brain.connections.size());
        eligibilityTraces.assign(genome.brain.connections.size(),0.0f);
        for(std::size_t i=0;i<genome.brain.connections.size();++i)learnedWeights[i]=genome.brain.connections[i].weight;
        memoryRegisters.fill(0.0f);
        brainState.fill(0.0f);
        emotionState.fill(0.0f);
        emotionDrive.fill(0.0f);
        rayOffsets = visionOffsets(genome.vision);
        rays.resize(rayOffsets.size());
        raySin.resize(rayOffsets.size());
        rayCos.resize(rayOffsets.size());
        for (size_t i = 0; i < rayOffsets.size(); ++i) {
            float r = rayOffsets[i] * DEG2RAD;
            raySin[i] = std::sin(r);
            rayCos[i] = std::cos(r);
            rays[i].angleOffset = rayOffsets[i];
            rays[i].distance = genome.vision.range;
        }
    }

    void refreshDirection() { float r = angle * DEG2RAD; sinA = std::sin(r); cosA = std::cos(r); }
    Vector2 forward() const { return {sinA, -cosA}; }
    Vector2 right() const { return {cosA, sinA}; }

    void cachePhenotype() {
        float ratio = size / cfg::tuning.baseCreatureSize;
        maxEnergy = creatureMaxEnergyForSize(size);
        sizeSpeedMult = clampf(std::pow(1.0f / ratio, cfg::tuning.sizeSpeedExponent), cfg::tuning.sizeSpeedMin, cfg::tuning.sizeSpeedMax);
        sizeAccelMult = clampf(std::pow(1.0f / ratio, cfg::tuning.sizeAccelExponent), cfg::tuning.sizeAccelMin, cfg::tuning.sizeAccelMax);
        sizeRotMult = clampf(std::pow(1.0f / ratio, cfg::tuning.sizeRotationExponent), cfg::tuning.sizeRotationMin, cfg::tuning.sizeRotationMax);
        sizeEnergyCostMult = clampf(std::pow(1.0f / ratio, cfg::tuning.sizeEnergyCostExponent), cfg::tuning.sizeEnergyCostMin, cfg::tuning.sizeEnergyCostMax);
        sizeBitePowerMult = clampf(std::pow(ratio, cfg::tuning.sizeBiteExponent), cfg::tuning.sizeBiteMin, cfg::tuning.sizeBiteMax);
        mouthSizeFactor = clampf(ratio, cfg::tuning.mouthSizeFactorMin, cfg::tuning.mouthSizeFactorMax);
        hardnessSpeedMult = clampf(1.0f - genome.physical.hardness * cfg::tuning.hardnessSpeedPenalty, cfg::tuning.hardnessSpeedMinMult, 1.0f);
        damageMult = clampf(1.0f - genome.physical.hardness * cfg::tuning.hardnessDamageReduction, cfg::tuning.hardnessDamageMinMult, 1.0f);
        maxForwardEffective = genome.physical.maxForwardSpeed * hardnessSpeedMult * sizeSpeedMult * ageSpeedMult;
        maxReverseEffective = genome.physical.maxReverseSpeed * hardnessSpeedMult * sizeSpeedMult * ageSpeedMult;
        maxLateralEffective = maxForwardEffective * clampf(genome.physical.lateralSpeedRatio,cfg::tuning.geneLateralRatioMin,cfg::tuning.geneLateralRatioMax);
        accelEffective = genome.physical.acceleration * sizeAccelMult * ageAccelMult;
        rotEffective = genome.physical.rotationSpeed * sizeRotMult * ageRotationMult;
        float speedFactor = clampf(genome.mouth.movementSpeed / cfg::tuning.mouthSpeedReference, cfg::tuning.mouthSpeedFactorMin, cfg::tuning.mouthSpeedFactorMax);
        bitePowerCached = clampf(genome.mouth.biteForce * sizeBitePowerMult * (cfg::tuning.biteOpeningBase + cfg::tuning.biteOpeningWeight * genome.mouth.maxOpening) * (cfg::tuning.biteSpeedBase + cfg::tuning.biteSpeedWeight * speedFactor) * ageBiteMult, 0.0f, cfg::tuning.bitePowerMax);
        float rawCd = 1.0f / std::max(cfg::tuning.biteCooldownSpeedFloor, genome.mouth.movementSpeed);
        biteCooldownCached = clampf(rawCd, cfg::tuning.biteCooldownMin, cfg::tuning.biteCooldownMax);
    }

    float adultSize() const { return clampf(genome.physical.bodySize,cfg::tuning.minCreatureSize,cfg::tuning.maxCreatureSize); }
    float adolescenceAge() const { return std::max(0.0f,genome.development.growthDuration*genome.development.adolescenceStartFraction); }
    float maturityAge() const { return std::max(0.0f,genome.development.growthDuration*genome.development.sexualMaturityFraction); }
    float senescenceStartAge() const { return std::max(0.0f,genome.development.longevity*genome.development.senescenceStartFraction); }
    bool sexuallyMature() const { return age >= maturityAge(); }
    float ageToLongevityRatio() const { return clampf(age/std::max(0.01f,genome.development.longevity),0.0f,2.0f); }
    const char* lifeStageName() const {
        if(age>=senescenceStartAge()) return "Idoso";
        if(age<adolescenceAge()) return "Infancia";
        if(age<std::max(0.01f,genome.development.growthDuration)) return "Adolescencia";
        return "Adulto";
    }
    float naturalDeathHazard() const {
        if(cfg::tuning.naturalDeathHazardAtLongevity<=0.0f) return 0.0f;
        const float start=senescenceStartAge();
        const float span=std::max(0.01f,genome.development.longevity-start);
        const float t=std::max(0.0f,(age-start)/span); // 1 = longevidade genetica de referencia
        if(t<=0.0f) return 0.0f;
        return std::min(cfg::tuning.naturalDeathHazardMax,
            cfg::tuning.naturalDeathHazardAtLongevity*std::pow(t,std::max(0.01f,cfg::tuning.naturalDeathHazardExponent)));
    }

    // Atualiza crescimento e senescencia em baixa frequencia. Retorna o custo
    // energetico do tecido novo desde a ultima atualizacao.
    float updateLifecycle(float dt) {
        lifecycleTimer += dt;
        if(lifecycleTimer < cfg::tuning.lifecycleUpdateInterval) return 0.0f;
        const float step=lifecycleTimer; lifecycleTimer=0.0f;

        const float oldSize=size;
        const float growthDuration=std::max(0.01f,genome.development.growthDuration);
        developmentProgress=clampf(age/growthDuration,0.0f,1.0f);
        const float shaped=std::pow(developmentProgress,std::max(0.01f,genome.development.growthCurveExponent));
        const float birth=clampf(genome.development.birthSizeFraction,0.01f,1.0f);
        size=std::max(0.05f,adultSize()*(birth+(1.0f-birth)*shaped));

        const float senStart=senescenceStartAge();
        const float senSpan=std::max(0.01f,genome.development.longevity-senStart);
        const float senRaw=std::max(0.0f,(age-senStart)/senSpan);
        senescenceProgress=clampf(senRaw,0.0f,1.0f);
        const float a=std::pow(senescenceProgress,std::max(0.01f,cfg::tuning.oldAgeCurveExponent));
        auto fromOne=[&](float target){return 1.0f+(target-1.0f)*a;};
        ageSpeedMult=fromOne(cfg::tuning.oldAgeSpeedMin);
        ageAccelMult=fromOne(cfg::tuning.oldAgeAccelMin);
        ageRotationMult=fromOne(cfg::tuning.oldAgeRotationMin);
        ageBiteMult=fromOne(cfg::tuning.oldAgeBiteMin);
        ageRegenMult=fromOne(cfg::tuning.oldAgeRegenMin);
        ageMetabolismMult=fromOne(cfg::tuning.oldAgeMetabolismMax);
        ageFertilityMult=clampf(fromOne(cfg::tuning.oldAgeFertilityMin),0.0f,1.0f);

        const float longRatio=std::max(0.01f,genome.development.longevity/std::max(1.0f,cfg::tuning.longevityReference));
        const float rawLong=std::pow(longRatio,cfg::tuning.longevityMetabolismExponent);
        longevityMaintenanceMult=clampf(1.0f+cfg::tuning.longevityMetabolismWeight*(rawLong-1.0f),
            cfg::tuning.longevityMetabolismMin,cfg::tuning.longevityMetabolismMax);

        cachePhenotype();
        energy=clampf(energy,0.0f,maxEnergy);
        speed=clampf(speed,-maxReverseEffective,maxForwardEffective);
        lateralSpeed=clampf(lateralSpeed,-maxLateralEffective,maxLateralEffective);

        const float growthCost=std::max(0.0f,size-oldSize)*cfg::tuning.growthEnergyCostPerSize*cfg::tuning.growthEnergyCostMultiplier;

        const float hazard=naturalDeathHazard();
        if(!naturalDeath && hazard>0.0f){
            const float p=1.0f-std::exp(-hazard*step);
            if(chance(p)){ naturalDeath=true; deathCause=DeathCause::NaturalAge; health=0.0f; }
        }
        return growthCost;
    }

    float updateBodyColor(float dt) {
        const auto& cg=genome.bodyColorControl;
        float current[3]{float(displayColor.r),float(displayColor.g),float(displayColor.b)};
        const float base[3]{float(genome.bodyColor.r),float(genome.bodyColor.g),float(genome.bodyColor.b)};
        const float mins[3]{cg.rMin,cg.gMin,cg.bMin};
        const float maxs[3]{cg.rMax,cg.gMax,cg.bMax};
        float changeNorm=0.0f; int active=0;
        for(int i=0;i<3;++i){
            float target=base[i];
            if(cg.channelMask&(1u<<i)){
                ++active;
                const float out=clampf(netOut[cfg::COLOR_OUTPUT_BASE+i],0.0f,1.0f);
                target=(mins[i]+(maxs[i]-mins[i])*out)*255.0f;
            }
            const float before=current[i];
            current[i]=moveToward(before,target,std::max(0.0f,cg.changeSpeed)*255.0f*dt);
            changeNorm+=std::fabs(current[i]-before)/255.0f;
        }
        displayColor=Color{
            (unsigned char)std::clamp<int>((int)std::lround(current[0]),0,255),
            (unsigned char)std::clamp<int>((int)std::lround(current[1]),0,255),
            (unsigned char)std::clamp<int>((int)std::lround(current[2]),0,255),255};
        return cfg::tuning.bodyColorActiveChannelCost*float(active)*dt +
               cfg::tuning.bodyColorChangeEnergyCost*changeNorm;
    }


    float radius() const { return std::max(0.5f, size * 0.5f); }
    float visionEnergyCostPerSecond() const {
        const float efficiency=clampf(genome.vision.efficiency,cfg::tuning.visionEfficiencyMin,cfg::tuning.visionEfficiencyMax);
        return float(normalizeRayCount(genome.vision.rayCount)) *
            cfg::tuning.visionRayEnergyCost * cfg::tuning.visionCost / std::max(0.05f,efficiency);
    }
    float mass() const {
        // Area corporal (size^2) define a maior parte da massa.
        // Dureza adiciona um pequeno fator de densidade sem superar o efeito do tamanho.
        const float density = cfg::tuning.creatureMassDensity * (1.0f + cfg::tuning.hardnessMassBonus * genome.physical.hardness);
        return std::max(cfg::tuning.creatureMassMin, density * std::pow(size, cfg::tuning.creatureMassSizeExponent));
    }
    Vector2 worldVelocity() const {
        return add(add(mul(forward(), speed), mul(right(), lateralSpeed)), pushVelocity);
    }
    void setWorldVelocity(Vector2 v) {
        const Vector2 controlled=add(mul(forward(),speed),mul(right(),lateralSpeed));
        pushVelocity = clampMagnitude(
            sub(v, controlled),
            cfg::PHYSICS_MAX_PUSH_SPEED * cfg::tuning.maxPushSpeed);
    }

    Vector2 bitePoint() const {
        float reach = size * 0.5f + (cfg::tuning.biteReachBase + genome.mouth.maxOpening * cfg::tuning.biteReachOpeningScale) * mouthSizeFactor;
        return add(pos, mul(forward(), reach));
    }
    float biteRadius() const { return (cfg::tuning.biteRadiusBase + genome.mouth.maxOpening * cfg::tuning.biteRadiusOpeningScale) * mouthSizeFactor; }
    bool canBite() const { return biteCooldown <= 0.0f; }
    float eggEnergyInvestment() const {
        return maxEnergy * clampf(
            genome.reproduction.eggEnergyFraction,
            cfg::tuning.eggEnergyGeneMin, cfg::tuning.eggEnergyGeneMax);
    }
    float eggBloodInvestment() const {
        return cfg::tuning.healthMax * clampf(
            genome.reproduction.eggBloodFraction,
            cfg::tuning.eggBloodGeneMin, cfg::tuning.eggBloodGeneMax);
    }
    bool canReproduce(float eggCostScale=1.0f) const {
        if(!sexuallyMature() || ageFertilityMult<=0.001f) return false;
        const float scale=clampf(eggCostScale,0.0f,1.0f);
        const float eCost = eggEnergyInvestment()*scale;
        const float bCost = eggBloodInvestment()*scale;
        const float ease=std::max(0.25f,cfg::tuning.matingEaseFactor);
        // vNEXT: reproducao estava rara demais. A facilidade agora reduz de forma
        // mais forte o limiar energetico, e a reproducao com parceiro ganha um
        // alivio adicional proporcional ao custo menor do ovo.
        const float easeThresholdReduction=clampf((ease-1.0f)*0.24f,0.0f,0.35f);
        const float partnerThresholdReduction=(1.0f-scale)*0.15f;
        const float ageThreshold=clampf(cfg::tuning.reproMinRatio - easeThresholdReduction - partnerThresholdReduction +
            (1.0f-ageFertilityMult)*cfg::tuning.oldAgeReproThresholdPenalty,0.30f,1.0f);
        return reproCooldown <= 0.0f &&
               energy >= maxEnergy * ageThreshold &&
               energy > eCost + maxEnergy * cfg::tuning.reproEnergySafetyRatio &&
               health > bCost + cfg::tuning.reproHealthSafety;
    }
    bool canMateAsPartner() const {
        if(!sexuallyMature() || ageFertilityMult<=0.001f || reproCooldown>0.0f || dead()) return false;
        const float energyRatio=energy/std::max(0.01f,maxEnergy);
        const float healthRatio=health/std::max(0.01f,cfg::tuning.healthMax);
        const float partnerCost=maxEnergy*cfg::tuning.matingPartnerEnergyCostFraction;
        const float ease=std::max(1.0f,cfg::tuning.matingEaseFactor);
        const float minEnergy=cfg::tuning.matingPartnerMinEnergyRatio/(ease*1.35f);
        const float minHealth=cfg::tuning.matingPartnerMinHealthRatio/(ease*1.35f);
        return energyRatio>=minEnergy &&
               healthRatio>=minHealth &&
               energy>partnerCost+maxEnergy*0.005f;
    }
    bool dead() const { return health <= 0.0f; }

    void registerBite() {
        biteCooldown = biteCooldownCached;
        biteAnimation = 0.22f;
        energy = clampf(energy - cfg::BITE_COST * cfg::tuning.biteCost * clampf(bitePowerCached, cfg::tuning.biteCostPowerMin, cfg::tuning.biteCostPowerMax), 0.0f, maxEnergy);
    }

    float receiveDamage(float raw, DeathCause cause=DeathCause::Unknown) {
        float before = health;
        health = clampf(health - raw * damageMult, 0.0f, cfg::tuning.healthMax);
        const float lost=before-health;
        recentDamage=clampf(recentDamage+lost*cfg::tuning.damageFeedbackScale,0.0f,1.0f);
        if(lost>0.0f){
            const float units10=clampf((lost/std::max(0.01f,cfg::tuning.healthMax))*10.0f,0.0f,2.0f);
            reinforce(cfg::tuning.reinforcementDamage10Pct*units10);
        }
        if(before>0.0f && health<=0.0f && deathCause==DeathCause::Unknown) deathCause=cause;
        return lost;
    }

    void investInEgg(float& energyOut, float& bloodOut, float costScale=1.0f, float* paidEnergyOut=nullptr, float* paidBloodOut=nullptr) {
        // energyOut/bloodOut representam os recursos biologicos do ovo. Com parceiro,
        // a fecundacao torna o processo 70% mais eficiente: o ovo recebe a mesma reserva,
        // mas o corpo do genitor paga somente a fracao configurada.
        energyOut = eggEnergyInvestment();
        bloodOut = eggBloodInvestment();
        const float scale=clampf(costScale,0.0f,1.0f);
        const float paidEnergy=std::min(energy,energyOut*scale);
        const float paidBlood=std::min(health,bloodOut*scale);
        energy = clampf(energy - paidEnergy, 0.0f, maxEnergy);
        health = clampf(health - paidBlood, 0.0f, cfg::tuning.healthMax);
        if(paidEnergyOut)*paidEnergyOut=paidEnergy;
        if(paidBloodOut)*paidBloodOut=paidBlood;
        const float ageCd=1.0f+(cfg::tuning.oldAgeReproCooldownMaxMult-1.0f)*(1.0f-ageFertilityMult);
        reproCooldown = cfg::tuning.reproCooldown*std::max(0.01f,ageCd);
    }

    void wrap() {
        // Mundo toroidal: cruzar a borda preserva velocidade e impulso; apenas a
        // coordenada central e canonizada para o tile 0..W x 0..H.
        constrainCircleToWorld(pos,radius());
    }
};

struct HistoryEntry { float elapsed = 0.0f; std::string text; };
struct WatchAccumulator {
    float elapsed = 0, metabolism = 0, vision = 0, forward = 0, reverse = 0, turn = 0, lateral = 0, movement = 0, healthLoss = 0, healthGain = 0;
    float fSig = 0, rSig = 0, lSig = 0, dSig = 0, leSig = 0, ldSig = 0;
};
struct FinalSnapshot {
    int id = 0, generation = 0, parentId = -1;
    float energy = 0, maxEnergy = 0, health = 0, size = 0;
    Vector2 pos{};
};
struct WatchSession {
    int creatureId = -1;
    float startedAt = 0.0f;
    bool alive = false;
    std::vector<HistoryEntry> entries;
    WatchAccumulator acc{};
    FinalSnapshot final{};
    bool hasFinal = false;
};

struct MinimumLineageSeed {
    bool valid = false;
    CreatureGenome genome{};
    int generation = 1;
    int parentId = -1;
    Vector2 origin{};
    float parentSize = cfg::tuning.baseCreatureSize;
    std::vector<float> learnedWeights;
    std::array<float,cfg::REGISTER_MEMORY_SLOTS> memoryRegisters{};
    std::uint16_t sexualHeritageDepth=0;
};

struct PopulationLimits {
    bool maxEnabled = true;
    int minCreatures = 20;
    int maxCreatures = 400;
    int minPlants = 10;
    int maxPlants = 3000;
};

// [SEC-STATS] Estatisticas, recordes e genealogia
struct ReinforcementRecordBook {
    float longestLife = 0.0f; int longestLifeId = -1;
    float shortestLife = 0.0f; int shortestLifeId = -1; bool hasShortestLife = false;
    float longestHealthAbove50 = 0.0f; int healthAboveId = -1;
    float fastestHealth50ToZero = 0.0f; int healthCrashId = -1;
    float longestEnergyAbove50 = 0.0f; int energyAboveId = -1;
    float fastestEnergy50ToZero = 0.0f; int energyCrashId = -1;
    int mostEggs = 0; int mostEggsId = -1;
    int fewestEggs = 0; int fewestEggsId = -1; bool hasFewestEggs = false;
};
static_assert(std::is_trivially_copyable_v<ReinforcementRecordBook>);

struct EvolutionStats {
    long long naturalBirths = 0;
    long long deaths = 0;
    long long naturalAgeDeaths = 0;
    long long starvationDeaths = 0;
    long long attackDeaths = 0;
    long long eggsLaid = 0;
    long long eggsLaidSolo = 0;
    long long eggsLaidMated = 0;
    long long eggsHatched = 0;
    long long eggsDestroyed = 0;
    long long minimumReplacementEggs = 0;
    long long plantBirths = 0;

    int maxGeneration = 1;
    float avgGeneration = 1.0f;
    float avgAge = 0.0f;
    float oldestAge = 0.0f;
    float avgBodySize = 0.0f;
    float avgForwardGene = 0.0f;
    float avgHidden = 0.0f;
    float avgConnections = 0.0f;
    float avgLoops = 0.0f;
    float avgMemory = 0.0f;
    float legacyRemovedMetricA = 0.0f;
    float avgVisionFocus = 0.0f;
    float legacyRemovedMetricB = 0.0f;
    float avgEnergyRatio = 0.0f;
    float avgVisionRange = 0.0f;
    int speciesCount = 0;
    int detachedPlants = 0;
};


// Layout exato usado ate v0.122. v0.123 inseriu dois contadores no EvolutionStats;
// ler um save antigo diretamente no struct novo deslocaria todos os bytes seguintes.
struct EvolutionStatsV122 {
    long long naturalBirths=0,deaths=0,naturalAgeDeaths=0,starvationDeaths=0,attackDeaths=0,eggsLaid=0;
    long long eggsHatched=0,eggsDestroyed=0,minimumReplacementEggs=0,plantBirths=0;
    int maxGeneration=1;
    float avgGeneration=1.0f,avgAge=0.0f,oldestAge=0.0f,avgBodySize=0.0f,avgForwardGene=0.0f;
    float avgHidden=0.0f,avgConnections=0.0f,avgLoops=0.0f,avgMemory=0.0f,legacyRemovedMetricA=0.0f;
    float avgVisionFocus=0.0f,legacyRemovedMetricB=0.0f,avgEnergyRatio=0.0f,avgVisionRange=0.0f;
    int speciesCount=0,detachedPlants=0;
};
static_assert(std::is_trivially_copyable_v<EvolutionStatsV122>);

inline EvolutionStats upgradeEvolutionStats(const EvolutionStatsV122& o){
    EvolutionStats n{};
    n.naturalBirths=o.naturalBirths;n.deaths=o.deaths;n.naturalAgeDeaths=o.naturalAgeDeaths;
    n.starvationDeaths=o.starvationDeaths;n.attackDeaths=o.attackDeaths;n.eggsLaid=o.eggsLaid;
    n.eggsLaidSolo=0;n.eggsLaidMated=0;
    n.eggsHatched=o.eggsHatched;n.eggsDestroyed=o.eggsDestroyed;n.minimumReplacementEggs=o.minimumReplacementEggs;n.plantBirths=o.plantBirths;
    n.maxGeneration=o.maxGeneration;n.avgGeneration=o.avgGeneration;n.avgAge=o.avgAge;n.oldestAge=o.oldestAge;
    n.avgBodySize=o.avgBodySize;n.avgForwardGene=o.avgForwardGene;n.avgHidden=o.avgHidden;n.avgConnections=o.avgConnections;
    n.avgLoops=o.avgLoops;n.avgMemory=o.avgMemory;n.legacyRemovedMetricA=o.legacyRemovedMetricA;n.avgVisionFocus=o.avgVisionFocus;
    n.legacyRemovedMetricB=o.legacyRemovedMetricB;n.avgEnergyRatio=o.avgEnergyRatio;n.avgVisionRange=o.avgVisionRange;
    n.speciesCount=o.speciesCount;n.detachedPlants=o.detachedPlants;
    return n;
}

// Layout historico usado ate v0.125.
struct EvolutionHistoryPointV125 {
    float time=0.0f;
    int creatures=0,plants=0,carcasses=0,eggs=0,maxGeneration=0,speciesCount=0;
    long long starvationDeaths=0,attackDeaths=0;
    float avgGeneration=0,avgBodySize=0,avgForwardGene=0,avgHidden=0,avgConnections=0,avgMemory=0;
    float legacyRemovedMetricA=0,avgVisionFocus=0,legacyRemovedMetricB=0,avgEnergyRatio=0,avgAge=0,avgVisionRange=0;
};
static_assert(std::is_trivially_copyable_v<EvolutionHistoryPointV125>);

// v0.126 guardava duas series CUMULATIVAS de ovos produzidos.
struct EvolutionHistoryPointV126 {
    float time=0.0f;
    int creatures=0,plants=0,carcasses=0,eggs=0,maxGeneration=0,speciesCount=0;
    long long starvationDeaths=0,attackDeaths=0;
    float avgGeneration=0,avgBodySize=0,avgForwardGene=0,avgHidden=0,avgConnections=0,avgMemory=0;
    float legacyRemovedMetricA=0,avgVisionFocus=0,legacyRemovedMetricB=0,avgEnergyRatio=0,avgAge=0,avgVisionRange=0;
    long long eggsLaidSolo=0,eggsLaidMated=0;
};
static_assert(std::is_trivially_copyable_v<EvolutionHistoryPointV126>);

// v0.127: series de ovos passam a representar ESTOQUE ATUAL no mundo.
struct EvolutionHistoryPoint {
    float time=0.0f;
    int creatures=0,plants=0,carcasses=0,eggs=0,maxGeneration=0,speciesCount=0;
    long long starvationDeaths=0,attackDeaths=0;
    float avgGeneration=0,avgBodySize=0,avgForwardGene=0,avgHidden=0,avgConnections=0,avgMemory=0;
    float legacyRemovedMetricA=0,avgVisionFocus=0,legacyRemovedMetricB=0,avgEnergyRatio=0,avgAge=0,avgVisionRange=0;
    int eggsCurrentSolo=0,eggsCurrentMated=0;
};
static_assert(std::is_trivially_copyable_v<EvolutionHistoryPoint>);

inline EvolutionHistoryPoint upgradeEvolutionHistoryPoint(const EvolutionHistoryPointV125& o){
    EvolutionHistoryPoint n{};
    n.time=o.time;n.creatures=o.creatures;n.plants=o.plants;n.carcasses=o.carcasses;n.eggs=o.eggs;
    n.maxGeneration=o.maxGeneration;n.speciesCount=o.speciesCount;
    n.starvationDeaths=o.starvationDeaths;n.attackDeaths=o.attackDeaths;
    n.avgGeneration=o.avgGeneration;n.avgBodySize=o.avgBodySize;n.avgForwardGene=o.avgForwardGene;
    n.avgHidden=o.avgHidden;n.avgConnections=o.avgConnections;n.avgMemory=o.avgMemory;
    n.legacyRemovedMetricA=o.legacyRemovedMetricA;n.avgVisionFocus=o.avgVisionFocus;n.legacyRemovedMetricB=o.legacyRemovedMetricB;
    n.avgEnergyRatio=o.avgEnergyRatio;n.avgAge=o.avgAge;n.avgVisionRange=o.avgVisionRange;
    // Saves antigos nao registravam o estoque atual por origem. Nao inventar dados.
    n.eggsCurrentSolo=0;n.eggsCurrentMated=0;
    return n;
}

inline EvolutionHistoryPoint upgradeEvolutionHistoryPoint(const EvolutionHistoryPointV126& o){
    EvolutionHistoryPoint n{};
    n.time=o.time;n.creatures=o.creatures;n.plants=o.plants;n.carcasses=o.carcasses;n.eggs=o.eggs;
    n.maxGeneration=o.maxGeneration;n.speciesCount=o.speciesCount;
    n.starvationDeaths=o.starvationDeaths;n.attackDeaths=o.attackDeaths;
    n.avgGeneration=o.avgGeneration;n.avgBodySize=o.avgBodySize;n.avgForwardGene=o.avgForwardGene;
    n.avgHidden=o.avgHidden;n.avgConnections=o.avgConnections;n.avgMemory=o.avgMemory;
    n.legacyRemovedMetricA=o.legacyRemovedMetricA;n.avgVisionFocus=o.avgVisionFocus;n.legacyRemovedMetricB=o.legacyRemovedMetricB;
    n.avgEnergyRatio=o.avgEnergyRatio;n.avgAge=o.avgAge;n.avgVisionRange=o.avgVisionRange;
    // Os campos da v0.126 eram acumulados, logo nao podem ser convertidos para estoque atual.
    n.eggsCurrentSolo=0;n.eggsCurrentMated=0;
    return n;
}

struct GenealogyRecord {
    int id=0,parentA=-1,parentB=-1,generation=1,speciesId=0;
    float birthTime=0.0f,deathTime=-1.0f;
};

// Checklist persistente do pesquisador/usuario. Nao interfere na simulacao.
struct NoteTask {
    std::uint32_t id = 0;
    bool done = false;
    std::string text;
};


// [SEC-SAVE] Serializacao, migracao e persistencia
// =========================================================
// v0.072+: SAVE/LOAD PERSISTENTE (v0.084 adiciona anotacoes/checklist)
// =========================================================
namespace saveio {
constexpr char MAGIC[8] = {'V','A','E','S','A','V','E','3'};
constexpr std::uint32_t FORMAT_VERSION = 3;
constexpr std::uint32_t PROGRAM_VERSION = 137;
constexpr std::uint32_t MIN_COMPAT_PROGRAM_VERSION = 82;
constexpr std::uint32_t MAX_SAVE_ENTITIES = 2000000u;
constexpr std::uint32_t MAX_SAVE_VECTOR = 2000000u;
constexpr std::uint32_t MAX_SAVE_STRING = 1024u * 1024u;

struct SessionMeta {
    Vector2 cameraTarget{};
    float cameraZoom = 0.48f;
    float simSpeed = 1.0f;
    float plantOpacity = 0.60f;
    std::uint8_t paused = 0;
    std::uint8_t showHud = 1;
    std::uint8_t showPlants = 1;
    std::uint8_t legacyReservedViewFlag = 0;
    std::uint8_t showGraphs = 1;
    std::uint8_t followMode = 0;
    std::uint8_t inspectorOpen = 0;
    std::uint8_t inspectorMinimized = 0;
};

std::string executableDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH + 2]{};
    DWORD n = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (n > 0 && n < MAX_PATH) {
        std::string path(buffer, buffer + n);
        const std::size_t slash = path.find_last_of("\\/");
        if (slash != std::string::npos) return path.substr(0, slash);
    }
#endif
    return ".";
}

std::string pathBesideExecutable(const char* fileName) {
    return (std::filesystem::path(executableDirectory()) / fileName).string();
}

std::string defaultWorldSavePath() { return pathBesideExecutable("vida_reforco_mundo.dat"); }
std::string defaultConfigPath() { return pathBesideExecutable("vida_reforco_config.ini"); }
// Alias interno para codigo legado da camada binaria.
std::string defaultSavePath() { return defaultWorldSavePath(); }

struct Writer {
    std::ofstream out;
    bool ok = true;
    explicit Writer(const std::string& path) : out(path, std::ios::binary | std::ios::trunc) { ok = out.good(); }

    template<class T> void pod(const T& v) {
        static_assert(std::is_trivially_copyable_v<T>, "save pod precisa ser trivially copyable");
        if (!ok) return;
        out.write(reinterpret_cast<const char*>(&v), sizeof(T));
        ok = out.good();
    }
    void bytes(const void* p, std::size_t n) {
        if (!ok) return;
        out.write(reinterpret_cast<const char*>(p), (std::streamsize)n);
        ok = out.good();
    }
    void str(const std::string& s) {
        const std::uint32_t n = (std::uint32_t)std::min<std::size_t>(s.size(), MAX_SAVE_STRING);
        pod(n); bytes(s.data(), n);
    }
    template<class T> void podVector(const std::vector<T>& v) {
        static_assert(std::is_trivially_copyable_v<T>, "save vector precisa ser POD");
        const std::uint32_t n = (std::uint32_t)v.size();
        pod(n);
        if (n) bytes(v.data(), sizeof(T) * (std::size_t)n);
    }
};

struct Reader {
    std::ifstream in;
    bool ok = true;
    explicit Reader(const std::string& path) : in(path, std::ios::binary) { ok = in.good(); }

    template<class T> void pod(T& v) {
        static_assert(std::is_trivially_copyable_v<T>, "load pod precisa ser trivially copyable");
        if (!ok) return;
        in.read(reinterpret_cast<char*>(&v), sizeof(T));
        ok = in.good();
    }
    void bytes(void* p, std::size_t n) {
        if (!ok) return;
        in.read(reinterpret_cast<char*>(p), (std::streamsize)n);
        ok = in.good();
    }
    void str(std::string& s) {
        std::uint32_t n = 0; pod(n);
        if (!ok || n > MAX_SAVE_STRING) { ok = false; return; }
        s.resize(n); if (n) bytes(s.data(), n);
    }
    template<class T> void podVector(std::vector<T>& v, std::uint32_t maxN = MAX_SAVE_VECTOR) {
        static_assert(std::is_trivially_copyable_v<T>, "load vector precisa ser POD");
        std::uint32_t n = 0; pod(n);
        if (!ok || n > maxN) { ok = false; return; }
        v.resize(n);
        if (n) bytes(v.data(), sizeof(T) * (std::size_t)n);
    }
};

inline void writeGenome(Writer& w, const CreatureGenome& g) {
    w.pod(g.physical);
    w.pod(g.mouth);
    w.pod(g.vision);
    w.pod(g.legacyRemovedSensor);
    w.pod(g.legacyRemovedSocial);
    w.pod(g.reproduction);
    w.pod(g.development);
    w.pod(g.bodyColor);
    w.pod(g.bodyColorControl);

    w.pod(g.brain.inputCount);
    w.pod(g.brain.hiddenCount);
    w.pod(g.brain.outputCount);
    w.podVector(g.brain.hiddenNodes);
    w.podVector(g.brain.outputBiases);
    w.podVector(g.brain.connections);
    w.podVector(g.brain.activeInputs);
    w.podVector(g.brain.activeOutputs);

    const std::uint32_t ec = (std::uint32_t)g.emotions.emotions.size();
    w.pod(ec);
    for (const auto& e : g.emotions.emotions) {
        w.pod(e.bias); w.pod(e.gain); w.pod(e.threshold); w.pod(e.strength);
        w.pod(e.impact); w.pod(e.riseTime); w.pod(e.duration);
        w.podVector(e.triggers);
    }
}

inline void readGenome(Reader& r, CreatureGenome& g,std::uint32_t programVersion) {
    r.pod(g.physical);
    r.pod(g.mouth);
    r.pod(g.vision);
    r.pod(g.legacyRemovedSensor);
    r.pod(g.legacyRemovedSocial);
    r.pod(g.reproduction);
    r.pod(g.development);
    r.pod(g.bodyColor);
    r.pod(g.bodyColorControl);

    r.pod(g.brain.inputCount);
    r.pod(g.brain.hiddenCount);
    r.pod(g.brain.outputCount);
    r.podVector(g.brain.hiddenNodes, 10000);
    r.podVector(g.brain.outputBiases, 1000);
    if(programVersion>=135){
        r.podVector(g.brain.connections,500000);
        r.podVector(g.brain.activeInputs,2000);
        r.podVector(g.brain.activeOutputs,1000);
    }else{
        std::vector<LegacyNeuralConnectionGeneV134> oldConnections;
        r.podVector(oldConnections,500000);
        g.brain.connections.clear();g.brain.connections.reserve(oldConnections.size());
        for(const auto& oc:oldConnections){
            NeuralConnectionGene c;c.srcKind=oc.srcKind;c.src=oc.src;c.dstKind=oc.dstKind;c.dst=oc.dst;c.weight=oc.weight;c.enabled=oc.enabled;
            c.function=BrainLinkFunction::Linear;c.paramA=1.0f;c.paramB=0.0f;
            // O slot 7 era inerte antes da v0.135. Conexoes antigas para/dele nunca
            // tiveram efeito comportamental; desativa-las evita transformar lixo historico
            // em mordida arbitraria agora que o slot foi promovido a MORDE.
            if((c.srcKind==BrainNodeKind::Output&&c.src==cfg::BITE_INTENT_OUTPUT_INDEX) ||
               (c.dstKind==BrainNodeKind::Output&&c.dst==cfg::BITE_INTENT_OUTPUT_INDEX))
                setNeuralConnectionEnabled(c,false);
            g.brain.connections.push_back(c);
        }
        // Redes antigas mantem todos os sensores/outputs que realmente existiam.
        // Os novos canais de percepcao continuam dormentes, exceto contato de planta
        // + MORDE, adicionados para preservar a antiga mordida automatica.
        g.brain.activeInputs.assign(cfg::INPUT_COUNT,1);
        for(int i=cfg::PERCEPTION_INPUT_BASE;i<cfg::PERCEPTION_INPUT_BASE+cfg::LEGACY_SENSOR_PADDING_INPUTS;++i)g.brain.activeInputs[(std::size_t)i]=0;
        g.brain.activeInputs[(std::size_t)cfg::PERCEPT_PLANT_MOUTH_CONTACT]=1;
        g.brain.activeOutputs.assign(cfg::OUTPUT_COUNT,1);
        g.brain.activeOutputs[(std::size_t)cfg::LEGACY_RESERVED_OUTPUT_INDEX]=0;
        g.brain.activeOutputs[(std::size_t)cfg::BITE_INTENT_OUTPUT_INDEX]=1;
        ensureBrainOutputStorage(g.brain);
        if(!hasConnection(g.brain,BrainNodeKind::Input,cfg::PERCEPT_PLANT_MOUTH_CONTACT,BrainNodeKind::Output,cfg::BITE_INTENT_OUTPUT_INDEX))
            pushConnectionIfNew(g.brain,BrainNodeKind::Input,cfg::PERCEPT_PLANT_MOUTH_CONTACT,BrainNodeKind::Output,cfg::BITE_INTENT_OUTPUT_INDEX,7.0f);
        g.brain.outputBiases[(std::size_t)cfg::BITE_INTENT_OUTPUT_INDEX]=-3.2f;
    }

    std::uint32_t ec = 0; r.pod(ec);
    if (!r.ok || ec > (std::uint32_t)cfg::MAX_EMOTIONS) { r.ok = false; return; }
    g.emotions.emotions.resize(ec);
    for (auto& e : g.emotions.emotions) {
        r.pod(e.bias); r.pod(e.gain); r.pod(e.threshold); r.pod(e.strength);
        r.pod(e.impact); r.pod(e.riseTime); r.pod(e.duration);
        r.podVector(e.triggers, 10000);
    }
    // Novas entradas sao anexadas no final. Conexoes antigas continuam validas e
    // futuras mutacoes passam a poder conectar a memoria episodica.
    g.brain.inputCount=cfg::INPUT_COUNT;
    g.brain.outputCount=cfg::OUTPUT_COUNT;
    ensureBrainOutputStorage(g.brain);
    ensureConnectivity(g.brain);
}

struct CreatureState {
    int id = 0;
    Vector2 pos{};
    float angle = 0, speed = 0, lateralSpeed = 0;
    int generation = 1, parentId = -1, secondParentId = -1;
    CreatureGenome genome{};
    float size=0, energy=0, health=0, age=0, lifecycleTimer=0, developmentProgress=0, senescenceProgress=0;
    float ageSpeedMult=1, ageAccelMult=1, ageRotationMult=1, ageBiteMult=1, ageRegenMult=1, ageMetabolismMult=1, ageFertilityMult=1, longevityMaintenanceMult=1;
    bool naturalDeath=false; std::uint8_t deathCause=0;
    float reproCooldown=0, biteCooldown=0, biteAnimation=0, brainTimer=0;
    std::vector<float> hidden;
    std::array<float,cfg::BRAIN_STATE_COUNT> brainState{};
    std::array<float,cfg::OUTPUT_COUNT> netOut{};
    std::array<float,cfg::OUTPUT_COUNT> brainOut{};
    std::array<float,cfg::MAX_EMOTIONS> emotionState{};
    std::array<float,cfg::MAX_EMOTIONS> emotionDrive{};
    Color displayColor{};
    float vegetationFrictionMult=1, rootedPlantCover=0;
    // 272 bytes historicos mantidos somente para que saves antigos continuem alinhados.
    // Nao alimentam sensores, comportamento, energia, evolucao ou UI. Novos saves gravam zero.
    std::array<std::uint8_t,272> legacyRemovedRuntimePayload{};
    Vector2 pushVelocity{}; float angularVelocity=0; std::array<float,4> contactPressure{{0,0,0,0}};
    float recentImpact=0,recentDamage=0,recentEnergyGain=0;
    std::uint8_t grabbedKind=0; int grabbedTargetRef=-1; int grabbedByCreatureId=-1; float grabLoadRatio=0;
    int speciesId=0;
    // v0.104+: anexado no fim do registro para manter leitura dos campos antigos intacta.
    std::array<LongMemoryEvent,cfg::LONG_MEMORY_SLOTS> longMemory{};
    std::uint8_t longMemoryHead=0,longMemoryCount=0;
    // v0.109+: estado adquirido durante a vida, anexado no fim para retrocompatibilidade.
    std::array<float,cfg::REGISTER_MEMORY_SLOTS> memoryRegisters{};
    std::vector<float> learnedWeights;
    std::vector<float> eligibilityTraces;
    float lastPlasticReward=0.0f;
    std::uint64_t plasticUpdates=0;
    // v0.116+: diagnostico do aprendizado por reforco, anexado ao fim.
    float reinforcementPending=0.0f,reinforcementTotal=0.0f,reinforcementPositive=0.0f,reinforcementNegative=0.0f;
    std::uint64_t reinforcementEvents=0;
    float timeHealthAbove50=0.0f,timeHealthBelow50=0.0f,timeEnergyAbove50=0.0f,timeEnergyBelow50=0.0f;
    int eggsLaidLifetime=0,matingPartnerId=-1; float matingContactTime=0.0f;
    std::uint16_t sexualHeritageDepth=0; // v0.135+
};
static_assert(sizeof(std::array<std::uint8_t,272>)==272, "payload legado deve ter 272 bytes");

struct PlantState {
    Vector2 pos{}; float size=0,biomassSize=0; bool hasBeenEaten=false; float growthDuration=0, reproTimer=0, nextRepro=0;
    PlantGenes genes{}; Vector2 visualOffset{}; bool detached=false; Vector2 velocity{};
    float detachedAge=0, detachedDuration=0, detachedDecayPerSecond=0; int grabbedByCreatureId=-1;
};
struct CarcassState {
    Vector2 pos{}; float bodySize=0, age=0, duration=0, initialEnergy=0, energy=0, maxBite=0, sensorRadius=0, visualSize=0, glowRadius=0, decayPerSecond=0; Vector2 velocity{}; int grabbedByCreatureId=-1;
};
struct EggState {
    Vector2 pos{}; CreatureGenome genome{}; int generation=1,parentId=-1,secondParentId=-1; bool minimumReplacement=false;
    float childSize=0,childMaxEnergy=0,radius=0,age=0,incubationDuration=0,initialEnergy=0,energy=0,initialBlood=0,blood=0,maxBite=0; Vector2 velocity{};
    std::vector<float> inheritedLearnedWeights;
    std::array<float,cfg::REGISTER_MEMORY_SLOTS> inheritedMemoryRegisters{};
    std::array<LongMemoryEvent,cfg::LONG_MEMORY_SLOTS> inheritedLongMemory{};
    std::uint8_t inheritedLongMemoryHead=0,inheritedLongMemoryCount=0;
    bool hasInheritedLearning=false;
    // v0.124+: metadados reprodutivos que precisam sobreviver save/load.
    float qualityScore=0.0f;
    int plannedBirthCount=1;
    bool conceivedWithMate=false;
    std::uint16_t sexualHeritageDepth=0; // v0.135+
};
struct LegacyRemovedEventState {
    Vector2 legacyPos{}; LegacyRemovedBands legacyBands{}; float legacyAge=0,legacyDuration=0; int legacySource=-1; bool legacyFlag=true;
};
static_assert(sizeof(LegacyRemovedEventState)==36, "layout do evento legado precisa permanecer compativel");
struct LineageState {
    bool valid=false; CreatureGenome genome{}; int generation=1,parentId=-1; Vector2 origin{}; float parentSize=0;
    std::vector<float> learnedWeights;
    std::array<float,cfg::REGISTER_MEMORY_SLOTS> memoryRegisters{};
    std::uint16_t sexualHeritageDepth=0;
};

struct Snapshot {
    std::array<std::uint64_t,4> rng{};
    float worldW=2000,worldH=2000;
    PopulationLimits limits{};
    EvolutionStats evo{};
    ReinforcementRecordBook reinforcementRecords{};
    int selectedId=-1,nextCreatureId=1;
    float simTime=0,plantSwayAccumulator=0,statsAccumulator=0,historyAccumulator=0;
    int minimumLineageDebt=0;
    LineageState lineage{};
    WatchSession watch{};
    SessionMeta meta{};
    std::vector<CreatureState> creatures;
    std::vector<PlantState> plants;
    std::vector<CarcassState> carcasses;
    std::vector<EggState> eggs;
    std::vector<LegacyRemovedEventState> legacyRemovedEvents;
    std::vector<EvolutionHistoryPoint> evolutionHistory;
    std::vector<GenealogyRecord> genealogy;
    std::uint32_t nextNoteId = 1;
    std::vector<NoteTask> notes;
};

inline void writeWatch(Writer& w, const WatchSession& s) {
    w.pod(s.creatureId); w.pod(s.startedAt); w.pod(s.alive); w.pod(s.acc); w.pod(s.final); w.pod(s.hasFinal);
    const std::uint32_t n=(std::uint32_t)s.entries.size(); w.pod(n);
    for(const auto& e:s.entries){w.pod(e.elapsed);w.str(e.text);}
}
inline void readWatch(Reader& r, WatchSession& s) {
    r.pod(s.creatureId); r.pod(s.startedAt); r.pod(s.alive); r.pod(s.acc); r.pod(s.final); r.pod(s.hasFinal);
    std::uint32_t n=0;r.pod(n); if(!r.ok||n>50000){r.ok=false;return;} s.entries.resize(n);
    for(auto& e:s.entries){r.pod(e.elapsed);r.str(e.text);}
}

inline void writeNotes(Writer& w, const Snapshot& s) {
    w.pod(s.nextNoteId);
    const std::uint32_t n=(std::uint32_t)std::min<std::size_t>(s.notes.size(),100000);
    w.pod(n);
    for(std::uint32_t i=0;i<n;++i){w.pod(s.notes[i].id);w.pod(s.notes[i].done);w.str(s.notes[i].text);}
}
inline void readNotes(Reader& r, Snapshot& s) {
    r.pod(s.nextNoteId);
    std::uint32_t n=0;r.pod(n);
    if(!r.ok||n>100000){r.ok=false;return;}
    s.notes.resize(n);
    for(auto& note:s.notes){
        r.pod(note.id);r.pod(note.done);r.str(note.text);
        if(note.text.size()>4096) note.text.resize(4096);
    }
    s.nextNoteId=std::max<std::uint32_t>(1,s.nextNoteId);
}

inline void writeCreature(Writer& w,const CreatureState& c){
    w.pod(c.id);w.pod(c.pos);w.pod(c.angle);w.pod(c.speed);w.pod(c.lateralSpeed);w.pod(c.generation);w.pod(c.parentId);w.pod(c.secondParentId);writeGenome(w,c.genome);
    w.pod(c.size);w.pod(c.energy);w.pod(c.health);w.pod(c.age);w.pod(c.lifecycleTimer);w.pod(c.developmentProgress);w.pod(c.senescenceProgress);
    w.pod(c.ageSpeedMult);w.pod(c.ageAccelMult);w.pod(c.ageRotationMult);w.pod(c.ageBiteMult);w.pod(c.ageRegenMult);w.pod(c.ageMetabolismMult);w.pod(c.ageFertilityMult);w.pod(c.longevityMaintenanceMult);
    w.pod(c.naturalDeath);w.pod(c.deathCause);w.pod(c.reproCooldown);w.pod(c.biteCooldown);w.pod(c.biteAnimation);w.pod(c.brainTimer);
    w.podVector(c.hidden);w.pod(c.brainState);w.pod(c.netOut);w.pod(c.brainOut);w.pod(c.emotionState);w.pod(c.emotionDrive);w.pod(c.displayColor);
    w.pod(c.vegetationFrictionMult);w.pod(c.rootedPlantCover);w.pod(c.legacyRemovedRuntimePayload);w.pod(c.pushVelocity);w.pod(c.angularVelocity);w.pod(c.contactPressure);w.pod(c.recentImpact);w.pod(c.recentDamage);w.pod(c.recentEnergyGain);w.pod(c.grabbedKind);w.pod(c.grabbedTargetRef);w.pod(c.grabbedByCreatureId);w.pod(c.grabLoadRatio);w.pod(c.speciesId);
    w.pod(c.longMemory);w.pod(c.longMemoryHead);w.pod(c.longMemoryCount);
    w.pod(c.memoryRegisters);w.podVector(c.learnedWeights);w.podVector(c.eligibilityTraces);w.pod(c.lastPlasticReward);w.pod(c.plasticUpdates);
    w.pod(c.reinforcementPending);w.pod(c.reinforcementTotal);w.pod(c.reinforcementPositive);w.pod(c.reinforcementNegative);w.pod(c.reinforcementEvents);
    w.pod(c.timeHealthAbove50);w.pod(c.timeHealthBelow50);w.pod(c.timeEnergyAbove50);w.pod(c.timeEnergyBelow50);w.pod(c.eggsLaidLifetime);w.pod(c.matingPartnerId);w.pod(c.matingContactTime);
    w.pod(c.sexualHeritageDepth);
}
inline void readCreature(Reader& r,CreatureState& c,std::uint32_t programVersion){
    r.pod(c.id);r.pod(c.pos);r.pod(c.angle);r.pod(c.speed);r.pod(c.lateralSpeed);r.pod(c.generation);r.pod(c.parentId);r.pod(c.secondParentId);readGenome(r,c.genome,programVersion);
    r.pod(c.size);r.pod(c.energy);r.pod(c.health);r.pod(c.age);r.pod(c.lifecycleTimer);r.pod(c.developmentProgress);r.pod(c.senescenceProgress);
    r.pod(c.ageSpeedMult);r.pod(c.ageAccelMult);r.pod(c.ageRotationMult);r.pod(c.ageBiteMult);r.pod(c.ageRegenMult);r.pod(c.ageMetabolismMult);r.pod(c.ageFertilityMult);r.pod(c.longevityMaintenanceMult);
    r.pod(c.naturalDeath);r.pod(c.deathCause);r.pod(c.reproCooldown);r.pod(c.biteCooldown);r.pod(c.biteAnimation);r.pod(c.brainTimer);
    r.podVector(c.hidden,10000);
    c.brainState.fill(0.0f);c.netOut.fill(0.0f);c.brainOut.fill(0.0f);
    if(programVersion>=108){
        r.pod(c.brainState);r.pod(c.netOut);r.pod(c.brainOut);
    }else if(programVersion>=104){
        constexpr int OLD_INPUTS=cfg::REGISTER_MEMORY_INPUT_BASE;
        constexpr int OLD_OUTPUTS=cfg::LEGACY_OUTPUT_COUNT;
        constexpr int OLD_HIDDEN_BASE=OLD_INPUTS;
        constexpr int OLD_PREV_HIDDEN_BASE=OLD_HIDDEN_BASE+cfg::MAX_HIDDEN;
        constexpr int OLD_PREV_OUTPUT_BASE=OLD_PREV_HIDDEN_BASE+cfg::MAX_HIDDEN;
        constexpr int OLD_STATE_COUNT=OLD_PREV_OUTPUT_BASE+OLD_OUTPUTS;
        std::array<float,OLD_STATE_COUNT> oldState{};std::array<float,OLD_OUTPUTS> oldNet{},oldBrain{};
        r.pod(oldState);r.pod(oldNet);r.pod(oldBrain);
        for(int i=0;i<OLD_INPUTS;++i)c.brainState[cfg::BRAIN_INPUT_BASE+i]=oldState[i];
        for(int h=0;h<cfg::MAX_HIDDEN;++h){c.brainState[cfg::BRAIN_HIDDEN_BASE+h]=oldState[OLD_HIDDEN_BASE+h];c.brainState[cfg::BRAIN_PREV_HIDDEN_BASE+h]=oldState[OLD_PREV_HIDDEN_BASE+h];}
        for(int o=0;o<OLD_OUTPUTS;++o){c.brainState[cfg::BRAIN_PREV_OUTPUT_BASE+o]=oldState[OLD_PREV_OUTPUT_BASE+o];c.netOut[o]=oldNet[o];c.brainOut[o]=oldBrain[o];}
    }else{
        constexpr int OLD_INPUTS=cfg::MEMORY_INPUT_BASE;
        constexpr int OLD_OUTPUTS=cfg::LEGACY_OUTPUT_COUNT;
        constexpr int OLD_HIDDEN_BASE=OLD_INPUTS;
        constexpr int OLD_PREV_HIDDEN_BASE=OLD_HIDDEN_BASE+cfg::MAX_HIDDEN;
        constexpr int OLD_PREV_OUTPUT_BASE=OLD_PREV_HIDDEN_BASE+cfg::MAX_HIDDEN;
        constexpr int OLD_STATE_COUNT=OLD_PREV_OUTPUT_BASE+OLD_OUTPUTS;
        std::array<float,OLD_STATE_COUNT> oldState{};std::array<float,OLD_OUTPUTS> oldNet{},oldBrain{};
        r.pod(oldState);r.pod(oldNet);r.pod(oldBrain);
        for(int i=0;i<OLD_INPUTS;++i)c.brainState[cfg::BRAIN_INPUT_BASE+i]=oldState[i];
        for(int h=0;h<cfg::MAX_HIDDEN;++h){c.brainState[cfg::BRAIN_HIDDEN_BASE+h]=oldState[OLD_HIDDEN_BASE+h];c.brainState[cfg::BRAIN_PREV_HIDDEN_BASE+h]=oldState[OLD_PREV_HIDDEN_BASE+h];}
        for(int o=0;o<OLD_OUTPUTS;++o){c.brainState[cfg::BRAIN_PREV_OUTPUT_BASE+o]=oldState[OLD_PREV_OUTPUT_BASE+o];c.netOut[o]=oldNet[o];c.brainOut[o]=oldBrain[o];}
    }r.pod(c.emotionState);r.pod(c.emotionDrive);r.pod(c.displayColor);
    r.pod(c.vegetationFrictionMult);r.pod(c.rootedPlantCover);r.pod(c.legacyRemovedRuntimePayload);r.pod(c.pushVelocity);r.pod(c.angularVelocity);r.pod(c.contactPressure);r.pod(c.recentImpact);r.pod(c.recentDamage);r.pod(c.recentEnergyGain);r.pod(c.grabbedKind);r.pod(c.grabbedTargetRef);r.pod(c.grabbedByCreatureId);r.pod(c.grabLoadRatio);r.pod(c.speciesId);
    if(r.ok && programVersion>=104){
        r.pod(c.longMemory);r.pod(c.longMemoryHead);r.pod(c.longMemoryCount);
        c.longMemoryHead%=cfg::LONG_MEMORY_SLOTS;
        c.longMemoryCount=(std::uint8_t)std::min<int>(c.longMemoryCount,cfg::LONG_MEMORY_SLOTS);
    }else{
        c.longMemory={};c.longMemoryHead=0;c.longMemoryCount=0;
    }
    if(r.ok && programVersion>=108){
        r.pod(c.memoryRegisters);r.podVector(c.learnedWeights,500000);r.podVector(c.eligibilityTraces,500000);r.pod(c.lastPlasticReward);r.pod(c.plasticUpdates);
    }else{
        c.memoryRegisters={};c.learnedWeights.clear();c.eligibilityTraces.clear();c.lastPlasticReward=0.0f;c.plasticUpdates=0;
    }
    if(r.ok && programVersion>=111){
        r.pod(c.reinforcementPending);r.pod(c.reinforcementTotal);r.pod(c.reinforcementPositive);r.pod(c.reinforcementNegative);r.pod(c.reinforcementEvents);
    }else{
        c.reinforcementPending=0.0f;c.reinforcementTotal=0.0f;c.reinforcementPositive=0.0f;c.reinforcementNegative=0.0f;c.reinforcementEvents=0;
    }
    if(r.ok && programVersion>=115){
        r.pod(c.timeHealthAbove50);r.pod(c.timeHealthBelow50);r.pod(c.timeEnergyAbove50);r.pod(c.timeEnergyBelow50);r.pod(c.eggsLaidLifetime);r.pod(c.matingPartnerId);r.pod(c.matingContactTime);
    }else{
        c.timeHealthAbove50=c.timeHealthBelow50=c.timeEnergyAbove50=c.timeEnergyBelow50=0.0f;c.eggsLaidLifetime=0;c.matingPartnerId=-1;c.matingContactTime=0.0f;
    }
    if(r.ok && programVersion>=133) r.pod(c.sexualHeritageDepth);
    else c.sexualHeritageDepth=(c.secondParentId>=0 && c.secondParentId!=c.parentId)?1u:0u;
}

inline bool writeSnapshot(const std::string& path,const Snapshot& s,std::string& error){
    Writer w(path); if(!w.ok){error="Nao foi possivel criar o arquivo de save.";return false;}
    w.bytes(MAGIC,sizeof(MAGIC));w.pod(FORMAT_VERSION);w.pod(PROGRAM_VERSION);
    w.pod(s.rng);w.pod(s.worldW);w.pod(s.worldH);w.pod(s.limits);w.pod(s.evo);w.pod(s.selectedId);w.pod(s.nextCreatureId);
    w.pod(s.simTime);w.pod(s.plantSwayAccumulator);w.pod(s.statsAccumulator);w.pod(s.historyAccumulator);w.pod(s.minimumLineageDebt);w.pod(s.meta);
    w.pod(s.lineage.valid); if(s.lineage.valid){writeGenome(w,s.lineage.genome);w.pod(s.lineage.generation);w.pod(s.lineage.parentId);w.pod(s.lineage.origin);w.pod(s.lineage.parentSize);w.podVector(s.lineage.learnedWeights);w.pod(s.lineage.memoryRegisters);w.pod(s.lineage.sexualHeritageDepth);}
    writeWatch(w,s.watch);
    auto count=[&](std::size_t n){std::uint32_t x=(std::uint32_t)n;w.pod(x);};
    count(s.creatures.size());for(const auto& c:s.creatures)writeCreature(w,c);
    count(s.plants.size());for(const auto& p:s.plants){w.pod(p);}
    count(s.carcasses.size());for(const auto& k:s.carcasses){w.pod(k);}
    count(s.eggs.size());for(const auto& e:s.eggs){
        w.pod(e.pos);writeGenome(w,e.genome);w.pod(e.generation);w.pod(e.parentId);w.pod(e.secondParentId);w.pod(e.minimumReplacement);
        w.pod(e.childSize);w.pod(e.childMaxEnergy);w.pod(e.radius);w.pod(e.age);w.pod(e.incubationDuration);w.pod(e.initialEnergy);w.pod(e.energy);w.pod(e.initialBlood);w.pod(e.blood);w.pod(e.maxBite);w.pod(e.velocity);
        w.podVector(e.inheritedLearnedWeights);w.pod(e.inheritedMemoryRegisters);w.pod(e.hasInheritedLearning);
        w.pod(e.qualityScore);w.pod(e.plannedBirthCount);w.pod(e.conceivedWithMate);
        w.pod(e.inheritedLongMemory);w.pod(e.inheritedLongMemoryHead);w.pod(e.inheritedLongMemoryCount);
        w.pod(e.sexualHeritageDepth);
    }
    count(s.legacyRemovedEvents.size());for(const auto& e:s.legacyRemovedEvents)w.pod(e);
    w.podVector(s.evolutionHistory); w.podVector(s.genealogy);
    writeNotes(w,s);
    w.pod(s.reinforcementRecords);
    w.out.flush();
    if(!w.ok||!w.out.good()){error="Falha ao terminar a gravacao do save.";return false;}
    return true;
}

inline bool readSnapshot(const std::string& path,Snapshot& s,std::string& error){
    Reader r(path); if(!r.ok){error="Arquivo vida_mundo.dat nao encontrado ao lado do executavel.";return false;}
    char magic[8]{};r.bytes(magic,sizeof(magic));std::uint32_t fv=0,pv=0;r.pod(fv);r.pod(pv);
    if(!r.ok||std::memcmp(magic,MAGIC,sizeof(MAGIC))!=0){error="Arquivo de mundo invalido ou corrompido.";return false;}
    if(fv!=FORMAT_VERSION||pv<MIN_COMPAT_PROGRAM_VERSION||pv>PROGRAM_VERSION){error="Mundo salvo pertence a uma versao incompatível do programa.";return false;}
    r.pod(s.rng);r.pod(s.worldW);r.pod(s.worldH);r.pod(s.limits);
    if(pv>=123){r.pod(s.evo);}
    else{EvolutionStatsV122 oldEvo{};r.pod(oldEvo);s.evo=upgradeEvolutionStats(oldEvo);}
    r.pod(s.selectedId);r.pod(s.nextCreatureId);
    r.pod(s.simTime);r.pod(s.plantSwayAccumulator);r.pod(s.statsAccumulator);r.pod(s.historyAccumulator);r.pod(s.minimumLineageDebt);r.pod(s.meta);
    r.pod(s.lineage.valid);if(s.lineage.valid){readGenome(r,s.lineage.genome,pv);r.pod(s.lineage.generation);r.pod(s.lineage.parentId);r.pod(s.lineage.origin);r.pod(s.lineage.parentSize);if(pv>=110){r.podVector(s.lineage.learnedWeights,500000);r.pod(s.lineage.memoryRegisters);}else{s.lineage.learnedWeights.clear();s.lineage.memoryRegisters={};}if(pv>=133)r.pod(s.lineage.sexualHeritageDepth);else s.lineage.sexualHeritageDepth=0;}
    readWatch(r,s.watch);
    auto readCount=[&](std::uint32_t& n){r.pod(n);if(n>MAX_SAVE_ENTITIES)r.ok=false;};
    std::uint32_t n=0;
    readCount(n);if(r.ok){s.creatures.resize(n);for(auto& c:s.creatures)readCreature(r,c,pv);}
    readCount(n);if(r.ok){s.plants.resize(n);for(auto& p:s.plants)r.pod(p);}
    readCount(n);if(r.ok){s.carcasses.resize(n);for(auto& k:s.carcasses)r.pod(k);}
    readCount(n);if(r.ok){s.eggs.resize(n);for(auto& e:s.eggs){
        r.pod(e.pos);readGenome(r,e.genome,pv);r.pod(e.generation);r.pod(e.parentId);r.pod(e.secondParentId);r.pod(e.minimumReplacement);
        r.pod(e.childSize);r.pod(e.childMaxEnergy);r.pod(e.radius);r.pod(e.age);r.pod(e.incubationDuration);r.pod(e.initialEnergy);r.pod(e.energy);r.pod(e.initialBlood);r.pod(e.blood);r.pod(e.maxBite);r.pod(e.velocity);
        if(pv>=110){r.podVector(e.inheritedLearnedWeights,500000);r.pod(e.inheritedMemoryRegisters);r.pod(e.hasInheritedLearning);}else{e.inheritedLearnedWeights.clear();e.inheritedMemoryRegisters={};e.hasInheritedLearning=false;}
        if(pv>=124){
            r.pod(e.qualityScore);r.pod(e.plannedBirthCount);r.pod(e.conceivedWithMate);
            e.qualityScore=clampf(e.qualityScore,0.0f,1.0f);
            e.plannedBirthCount=std::clamp(e.plannedBirthCount,1,10);
        }else{
            // v0.123 criava estes dados apenas em memoria. Reconstruimos o que e seguro.
            const float eQ=clampf(e.initialEnergy/std::max(0.01f,e.childMaxEnergy*cfg::tuning.eggEnergyFullBirthFraction),0.0f,1.0f);
            const float bQ=clampf(e.initialBlood/(cfg::tuning.healthMax*cfg::tuning.eggBloodFullBirthFraction),0.0f,1.0f);
            const float ws=std::max(0.0001f,cfg::tuning.eggQualityEnergyWeight+cfg::tuning.eggQualityBloodWeight);
            e.qualityScore=clampf((eQ*cfg::tuning.eggQualityEnergyWeight+bQ*cfg::tuning.eggQualityBloodWeight)/ws,0.0f,1.0f);
            e.conceivedWithMate=e.secondParentId>=0 && e.secondParentId!=e.parentId;
            e.plannedBirthCount=1; // ninhadas antigas nao podem ser recuperadas sem rerrolar o save.
        }
        if(pv>=131){
            r.pod(e.inheritedLongMemory);r.pod(e.inheritedLongMemoryHead);r.pod(e.inheritedLongMemoryCount);
            e.inheritedLongMemoryHead%=cfg::LONG_MEMORY_SLOTS;
            e.inheritedLongMemoryCount=(std::uint8_t)std::min<int>(e.inheritedLongMemoryCount,cfg::LONG_MEMORY_SLOTS);
        }else{
            e.inheritedLongMemory={};e.inheritedLongMemoryHead=0;e.inheritedLongMemoryCount=0;
        }
        if(pv>=133) r.pod(e.sexualHeritageDepth);
        else e.sexualHeritageDepth=e.conceivedWithMate?1u:0u;
    }}
    readCount(n);if(r.ok){s.legacyRemovedEvents.resize(n);for(auto& e:s.legacyRemovedEvents)r.pod(e);}
    if(r.ok){
        if(pv>=127){
            r.podVector(s.evolutionHistory,cfg::MAX_EVOLUTION_HISTORY);
        }else if(pv>=126){
            std::vector<EvolutionHistoryPointV126> oldHistory;
            r.podVector(oldHistory,cfg::MAX_EVOLUTION_HISTORY);
            if(r.ok){
                s.evolutionHistory.clear();s.evolutionHistory.reserve(oldHistory.size());
                for(const auto& oldPoint:oldHistory)s.evolutionHistory.push_back(upgradeEvolutionHistoryPoint(oldPoint));
            }
        }else{
            std::vector<EvolutionHistoryPointV125> oldHistory;
            r.podVector(oldHistory,cfg::MAX_EVOLUTION_HISTORY);
            if(r.ok){
                s.evolutionHistory.clear();s.evolutionHistory.reserve(oldHistory.size());
                for(const auto& oldPoint:oldHistory)s.evolutionHistory.push_back(upgradeEvolutionHistoryPoint(oldPoint));
            }
        }
        r.podVector(s.genealogy,MAX_SAVE_ENTITIES);
    }
    // v0.082/v0.083 nao tinham checklist no final do arquivo.
    if(r.ok && pv>=84) readNotes(r,s);
    else { s.nextNoteId=1; s.notes.clear(); }
    if(r.ok && pv>=115) r.pod(s.reinforcementRecords);
    else s.reinforcementRecords={};
    // v0.118 mudou os antigos "tempo abaixo de 50%" para recordes de QUEDA 50%->0%.
    // Nao reutilizamos os valores antigos porque mediam outra coisa.
    if(pv<118){
        s.reinforcementRecords.fastestHealth50ToZero=0.0f;s.reinforcementRecords.healthCrashId=-1;
        s.reinforcementRecords.fastestEnergy50ToZero=0.0f;s.reinforcementRecords.energyCrashId=-1;
    }
    if(!r.ok){error="Save terminou inesperadamente ou contem dados invalidos.";return false;}
    return true;
}
} // namespace saveio


// =========================================================
// v0.060: armazenamento estavel + paralelismo persistente
// =========================================================
template <typename T, std::size_t BlockSize = 256>
class StableObjectPool {
    struct Slot {
        alignas(T) unsigned char storage[sizeof(T)];
        Slot* next = nullptr;
        std::uint32_t localityKey = 0;
        bool alive = false;
    };
    std::vector<std::unique_ptr<Slot[]>> blocks_;
    Slot* free_ = nullptr;
    std::size_t live_ = 0;

    void addBlock() {
        auto block = std::make_unique<Slot[]>(BlockSize);
        Slot* raw = block.get();
        const std::uint32_t blockOrdinal=(std::uint32_t)blocks_.size();
        for (std::size_t i=0;i<BlockSize;++i) {
            raw[i].next=free_;raw[i].alive=false;
            raw[i].localityKey=blockOrdinal*(std::uint32_t)BlockSize+(std::uint32_t)i;
            free_=&raw[i];
        }
        blocks_.push_back(std::move(block));
    }
public:
    StableObjectPool() = default;
    StableObjectPool(const StableObjectPool&) = delete;
    StableObjectPool& operator=(const StableObjectPool&) = delete;
    ~StableObjectPool(){ clear(); }
    template <typename... Args>
    T* create(Args&&... args) {
        if(!free_) addBlock();
        Slot* slot=free_; free_=slot->next; slot->next=nullptr;
        T* obj=new (slot->storage) T(std::forward<Args>(args)...);
        slot->alive=true; ++live_; return obj;
    }
    void destroy(T* obj) {
        if(!obj) return;
        Slot* slot=reinterpret_cast<Slot*>(obj);
        if(!slot->alive) return;
        obj->~T(); slot->alive=false; slot->next=free_; free_=slot; if(live_) --live_;
    }
    void clear() {
        free_=nullptr; live_=0;
        for(auto& block:blocks_) for(std::size_t i=0;i<BlockSize;++i) {
            Slot& slot=block[i];
            if(slot.alive){ reinterpret_cast<T*>(slot.storage)->~T(); slot.alive=false; }
        }
        for(auto& block:blocks_) for(std::size_t i=0;i<BlockSize;++i){ block[i].next=free_; free_=&block[i]; }
    }
    std::size_t live() const{return live_;}
    std::size_t capacity() const{return blocks_.size()*BlockSize;}
    std::uint32_t localityKey(const T* obj) const {
        if(!obj)return std::numeric_limits<std::uint32_t>::max();
        const Slot* slot=reinterpret_cast<const Slot*>(obj);
        return slot->localityKey;
    }
    std::size_t allocatedBytes() const{return sizeof(*this)+blocks_.capacity()*sizeof(std::unique_ptr<Slot[]>)+blocks_.size()*BlockSize*sizeof(Slot);}
};

class ParallelExecutor {
    std::vector<std::thread> workers_;
    std::mutex mutex_;
    std::condition_variable wakeCv_, doneCv_;
    std::atomic<std::size_t> next_{0};
    std::size_t count_=0, finishedWorkers_=0;
    std::function<void(std::size_t)> job_;
    std::uint64_t generation_=0;
    bool stopping_=false;
    // Medicao do custo de despacho/sincronizacao. O tempo residual de espera NAO e
    // chamado de overhead puro: ele tambem revela desequilibrio de carga entre workers.
    double perfSetupMs_=0.0, perfResidualWaitMs_=0.0;
    std::size_t perfCalls_=0;

    void runCurrentJob(){
        for(;;){ const std::size_t i=next_.fetch_add(1,std::memory_order_relaxed); if(i>=count_) break; job_(i); }
    }
    void workerLoop(){
        std::uint64_t seen=0;
        for(;;){
            {
                std::unique_lock<std::mutex> lock(mutex_);
                wakeCv_.wait(lock,[&]{return stopping_||generation_!=seen;});
                if(stopping_) return;
                seen=generation_;
            }
            runCurrentJob();
            {
                std::lock_guard<std::mutex> lock(mutex_);
                ++finishedWorkers_; if(finishedWorkers_==workers_.size()) doneCv_.notify_one();
            }
        }
    }
public:
    ParallelExecutor(){
        const unsigned hw=std::max(1u,std::thread::hardware_concurrency());
        const unsigned count=hw>2?std::min(8u,hw-1u):0u;
        workers_.reserve(count);
        for(unsigned i=0;i<count;++i) workers_.emplace_back([this]{workerLoop();});
    }
    ~ParallelExecutor(){
        { std::lock_guard<std::mutex> lock(mutex_); stopping_=true; ++generation_; }
        wakeCv_.notify_all();
        for(auto& t:workers_) if(t.joinable()) t.join();
    }
    std::size_t workerCount() const{return workers_.size();}
    struct PerfWindow { double setupMs=0.0, residualWaitMs=0.0; std::size_t calls=0; };
    void beginPerfWindow(){perfSetupMs_=0.0;perfResidualWaitMs_=0.0;perfCalls_=0;}
    PerfWindow perfWindow() const{return {perfSetupMs_,perfResidualWaitMs_,perfCalls_};}
    template<typename Fn>
    void parallelFor(std::size_t count, Fn&& fn){
        ++perfCalls_;
        if(count<24||workers_.empty()){ for(std::size_t i=0;i<count;++i) fn(i); return; }
        using PerfClock=std::chrono::high_resolution_clock;
        const auto setup0=PerfClock::now();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            count_=count; job_=std::forward<Fn>(fn); next_.store(0,std::memory_order_relaxed); finishedWorkers_=0; ++generation_;
        }
        wakeCv_.notify_all();
        const auto setup1=PerfClock::now();
        perfSetupMs_+=std::chrono::duration<double,std::milli>(setup1-setup0).count();
        runCurrentJob();
        const auto wait0=PerfClock::now();
        std::unique_lock<std::mutex> lock(mutex_);
        doneCv_.wait(lock,[&]{return finishedWorkers_==workers_.size();});
        const auto wait1=PerfClock::now();
        perfResidualWaitMs_+=std::chrono::duration<double,std::milli>(wait1-wait0).count();
        job_={};
    }
};

enum class PhysicalKind : std::uint8_t { Creature, Carcass, Plant, Egg };
struct PhysicalProxy {
    PhysicalKind kind=PhysicalKind::Creature;
    void* object=nullptr;
    Vector2* pos=nullptr;
    Vector2 velocity{};
    float radius=0.0f;
    float invMass=1.0f;
};

struct PerfMetric {
    double lastMs=0.0, avgMs=0.0, peakMs=0.0;
    std::uint64_t calls=0;
    void sample(double ms){
        lastMs=std::max(0.0,ms);
        avgMs=(calls==0)?lastMs:(avgMs*0.90+lastMs*0.10);
        peakMs=std::max(lastMs,peakMs*0.997); // pico recente decai devagar para continuar util
        ++calls;
    }
};

struct PerfStats {
    // Campos antigos preservados para o HUD compacto.
    double sensorsMs = 0.0, brainMs = 0.0, movementMs = 0.0, creaturesMs = 0.0;
    double plantsMs = 0.0, bitesMs = 0.0, ecologyMs = 0.0, physicsMs = 0.0;
    std::size_t brainBatch = 0;
    std::size_t substepsLastFrame = 0;

    // Diagnostico da relacao frame real -> tempo simulado. Quando um frame atrasa,
    // o excesso nao e mais convertido em centenas de subpassos de catch-up.
    double realDtLastFrame = 0.0;
    double simRequestedDtLastFrame = 0.0;   // quanto seria processado sem protecao
    double simProcessedDtLastFrame = 0.0;   // quanto foi efetivamente processado
    double simDroppedDtLastFrame = 0.0;     // tempo simulado descartado para impedir espiral
    float simSpeedLastFrame = 1.0f;
    std::size_t uncappedSubstepsLastFrame = 0;
    std::size_t preventedSubstepsLastFrame = 0;

    // Quantidade real de trabalho da ultima atualizacao sensorial. Estes contadores
    // nao usam atomicos no hot path: cada bicho escreve seu scratch e o main thread soma.
    std::uint64_t sensorRays = 0, sensorCells = 0;
    std::uint64_t sensorPlantChecks = 0, sensorCreatureChecks = 0;
    std::uint64_t sensorCarcassChecks = 0, sensorEggChecks = 0;
    // Visao GPU: o timer sensorVision mede somente trabalho CPU de preparar/colher/despachar.
    // O compute em si roda assincrono; estes campos mostram se o caminho GPU esta ativo.
    bool gpuVisionActive = false;
    bool gpuVisionAppliedLastPass = false;
    bool gpuVisionSubmittedLastPass = false;
    std::uint64_t gpuVisionDispatches = 0, gpuVisionReadbacks = 0, gpuVisionSkippedDispatches = 0;
    std::size_t gpuVisionObjectRefs = 0, gpuVisionPendingBatches = 0;

    // Diagnostico ecologico cumulativo desde o ultimo reset/load. Serve para detectar
    // se os bichos estao encontrando plantas mas gastando mordidas sem obter alimento.
    std::uint64_t plantBiteActions = 0;
    std::uint64_t plantUprootActions = 0;
    std::uint64_t plantFoodBites = 0;
    std::uint64_t plantZeroFoodBites = 0;
    double plantBiomassConsumed = 0.0;
    double plantEnergyGained = 0.0;

    // Fotografia do ultimo subpasso fisico. Plantas soltas sao contadas como
    // obstaculos estaticos, mas nao entram nos corpos do solver dinamico.
    std::size_t physicsBodyCreatures = 0, physicsBodyPlants = 0;
    std::size_t physicsBodyCarcasses = 0, physicsBodyEggs = 0;
    std::uint64_t physicsCandidatePairs = 0;
    std::uint64_t physicsContactPairs = 0;
    std::uint64_t physicsSkippedPlantPlantPairs = 0;
    std::uint64_t physicsCreatureCreatureContacts = 0;
    std::uint64_t physicsCreaturePlantContacts = 0;
    std::uint64_t physicsOtherContacts = 0;

    PerfMetric emotions, brains, grabIntent, movement, grabConstraints;
    PerfMetric plantSway, plants, bites, deaths, carcasses, eggs, physics, minimums;
    // Fisica detalhada: estes tres somados explicam quase todo o total de physics.
    PerfMetric physicsBuckets, physicsPairs, physicsStaticPlants, physicsOther;
    // Threads: setup e residual wait sao somados em cada subpasso, nao por chamada.
    PerfMetric parallelDispatch, parallelResidualWait;
    std::size_t parallelCallsLastSubstep=0;
    PerfMetric sensorCover, sensorVision, sensorInputs, sensing;
    PerfMetric stats, history, simSubstep, simFrame;
    PerfMetric renderWorld, renderUi, renderSubmit, frameTotal, saveIo, loadIo;
};

// =========================================================
// v0.104: VISAO GPU COMPUTE (Direct3D 11)
// =========================================================
// Estruturas alinhadas em blocos de 16 bytes para terem o mesmo stride no C++ e HLSL.
struct GpuVisionCellRange { std::uint32_t start=0,count=0,pad0=0,pad1=0; };
struct GpuVisionObject {
    float px=0,py=0,radius=0,coverRadius=0;
    float coverX=0,coverY=0; std::uint32_t type=0,color=0;
    std::int32_t creatureId=-1; std::uint32_t pad0=0,pad1=0,pad2=0;
};
struct GpuVisionRay {
    float ox=0,oy=0,dx=0,dy=0;
    float range=0,observerRadius=0,coverWeight=0,insidePass=0;
    std::int32_t observerId=-1; std::uint32_t raySlot=0,pad0=0,pad1=0;
};
struct GpuVisionResult {
    float distance=0,rangeUsed=1; std::uint32_t type=0,color=0;
    std::int32_t observerId=-1; std::uint32_t raySlot=0,cells=0,plantChecks=0;
    std::uint32_t creatureChecks=0,carcassChecks=0,eggChecks=0,pad=0;
    float targetX=0,targetY=0; std::int32_t targetCreatureId=-1; std::uint32_t pad2=0;
};
static_assert(sizeof(GpuVisionCellRange)==16);
static_assert(sizeof(GpuVisionObject)==48);
static_assert(sizeof(GpuVisionRay)==48);
static_assert(sizeof(GpuVisionResult)==64);

class GpuVisionEngine {
public:
    bool permanentlyDisabled=false;
    bool initialized=false;
    std::string disabledReason;
    std::uint64_t dispatches=0,readbacks=0,skippedDispatches=0;
    std::size_t lastObjectRefs=0;

#ifdef _WIN32
private:
    ID3D11ComputeShader* shader=nullptr;
    ID3D11Buffer *cellBuffer=nullptr,*objectBuffer=nullptr,*rayBuffer=nullptr,*constantBuffer=nullptr,*outputBuffer=nullptr;
    ID3D11ShaderResourceView *cellSrv=nullptr,*objectSrv=nullptr,*raySrv=nullptr;
    ID3D11UnorderedAccessView* outputUav=nullptr;
    ID3D11Buffer* staging[2]{nullptr,nullptr};
    ID3D11Query* query[2]{nullptr,nullptr};
    bool pending[2]{false,false};
    std::size_t pendingCount[2]{0,0};
    std::uint64_t pendingSerial[2]{0,0};
    std::size_t cellCapacity=0,objectCapacity=0,rayCapacity=0,outputCapacity=0;
    std::uint64_t serialCounter=0,lastCollectedSerial=0;

    template<class T> static void rel(T*& p){if(p){p->Release();p=nullptr;}}

    template<class T>
    bool ensureInput(ID3D11Buffer*& buf,ID3D11ShaderResourceView*& srv,std::size_t& capacity,std::size_t wanted){
        wanted=std::max<std::size_t>(1,wanted);
        if(buf&&srv&&capacity>=wanted)return true;
        rel(srv);rel(buf);capacity=1;while(capacity<wanted)capacity*=2;
        D3D11_BUFFER_DESC bd{};bd.ByteWidth=(UINT)(capacity*sizeof(T));bd.Usage=D3D11_USAGE_DYNAMIC;
        bd.BindFlags=D3D11_BIND_SHADER_RESOURCE;bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;bd.StructureByteStride=sizeof(T);
        if(FAILED(winshim::device->CreateBuffer(&bd,nullptr,&buf)))return false;
        D3D11_SHADER_RESOURCE_VIEW_DESC sd{};sd.Format=DXGI_FORMAT_UNKNOWN;sd.ViewDimension=D3D11_SRV_DIMENSION_BUFFER;
        sd.Buffer.FirstElement=0;sd.Buffer.NumElements=(UINT)capacity;
        if(FAILED(winshim::device->CreateShaderResourceView(buf,&sd,&srv))){rel(buf);return false;}
        return true;
    }

    template<class T>
    bool upload(ID3D11Buffer* buf,const std::vector<T>& src){
        if(!buf)return false;
        D3D11_MAPPED_SUBRESOURCE m{};
        if(FAILED(winshim::context->Map(buf,0,D3D11_MAP_WRITE_DISCARD,0,&m)))return false;
        if(!src.empty())std::memcpy(m.pData,src.data(),src.size()*sizeof(T));
        else std::memset(m.pData,0,sizeof(T));
        winshim::context->Unmap(buf,0);return true;
    }

    bool ensureOutput(std::size_t wanted){
        wanted=std::max<std::size_t>(1,wanted);
        if(outputBuffer&&outputUav&&outputCapacity>=wanted)return true;
        rel(outputUav);rel(outputBuffer);for(int i=0;i<2;++i){rel(query[i]);rel(staging[i]);pending[i]=false;pendingCount[i]=0;}
        outputCapacity=1;while(outputCapacity<wanted)outputCapacity*=2;
        D3D11_BUFFER_DESC bd{};bd.ByteWidth=(UINT)(outputCapacity*sizeof(GpuVisionResult));bd.Usage=D3D11_USAGE_DEFAULT;
        bd.BindFlags=D3D11_BIND_UNORDERED_ACCESS;bd.MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;bd.StructureByteStride=sizeof(GpuVisionResult);
        if(FAILED(winshim::device->CreateBuffer(&bd,nullptr,&outputBuffer)))return false;
        D3D11_UNORDERED_ACCESS_VIEW_DESC ud{};ud.Format=DXGI_FORMAT_UNKNOWN;ud.ViewDimension=D3D11_UAV_DIMENSION_BUFFER;
        ud.Buffer.FirstElement=0;ud.Buffer.NumElements=(UINT)outputCapacity;
        if(FAILED(winshim::device->CreateUnorderedAccessView(outputBuffer,&ud,&outputUav)))return false;
        for(int i=0;i<2;++i){
            D3D11_BUFFER_DESC sd{};sd.ByteWidth=bd.ByteWidth;sd.Usage=D3D11_USAGE_STAGING;sd.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
            if(FAILED(winshim::device->CreateBuffer(&sd,nullptr,&staging[i])))return false;
            if(!query[i]){D3D11_QUERY_DESC qd{};qd.Query=D3D11_QUERY_EVENT;if(FAILED(winshim::device->CreateQuery(&qd,&query[i])))return false;}
        }
        return true;
    }

    bool compile(){
        if(!winshim::device||!winshim::context||!winshim::D3DCompile_){disabledReason="D3D11 indisponivel";return false;}
        if(!winshim::hardwareDevice){disabledReason="renderer WARP/software";return false;}
        if(winshim::device->GetFeatureLevel()<D3D_FEATURE_LEVEL_11_0){disabledReason="feature level < 11_0";return false;}
        static const char* hlsl=R"HLSL(
struct CellRange { uint start; uint count; uint pad0; uint pad1; };
struct VObj { float px;float py;float radius;float coverRadius; float coverX;float coverY;uint type;uint color; int creatureId;uint pad0;uint pad1;uint pad2; };
struct VRay { float ox;float oy;float dx;float dy; float range;float observerRadius;float coverWeight;float insidePass; int observerId;uint raySlot;uint pad0;uint pad1; };
struct VResult { float distance;float rangeUsed;uint type;uint color; int observerId;uint raySlot;uint cells;uint plantChecks; uint creatureChecks;uint carcassChecks;uint eggChecks;uint pad; float targetX;float targetY;int targetCreatureId;uint pad2; };
cbuffer VisionCB : register(b0) { uint rayCount;uint cols;uint rows;float cellSize; float worldW;float worldH;float cbPad0;float cbPad1; };
StructuredBuffer<CellRange> gCells : register(t0);
StructuredBuffer<VObj> gObjects : register(t1);
StructuredBuffer<VRay> gRays : register(t2);
RWStructuredBuffer<VResult> gOut : register(u0);

float rayCircle(float2 origin,float2 dir,float2 center,float radius,float maxRange){
    float2 oc=center-origin;float proj=dot(oc,dir);float oc2=dot(oc,oc);float r2=radius*radius;
    if(proj<0.0 && oc2>r2)return -1.0;if(proj-radius>maxRange)return -1.0;
    float perp2=oc2-proj*proj;if(perp2>r2)return -1.0;
    float thc=sqrt(max(0.0,r2-perp2));float t=proj-thc;if(t<0.0)t=proj+thc;
    return (t>=0.0&&t<=maxRange)?t:-1.0;
}
float rayTrefoil(float2 origin,float2 dir,float2 center,float radius,float maxRange){
    float leafR=radius*0.55;float best=-1.0;float hit;
    hit=rayCircle(origin,dir,center+float2(0.0,-radius*0.30),leafR,maxRange);if(hit>=0.0)best=hit;
    hit=rayCircle(origin,dir,center+float2(-radius*0.27,radius*0.22),leafR,maxRange);if(hit>=0.0&&(best<0.0||hit<best))best=hit;
    hit=rayCircle(origin,dir,center+float2( radius*0.27,radius*0.22),leafR,maxRange);if(hit>=0.0&&(best<0.0||hit<best))best=hit;
    hit=rayCircle(origin,dir,center+float2(0.0,radius*0.02),radius*0.30,maxRange);if(hit>=0.0&&(best<0.0||hit<best))best=hit;
    return best;
}
int wrapCell(int v,int n){int m=v%n;return m<0?m+n:m;}
[numthreads(64,1,1)]
void CSMain(uint3 tid:SV_DispatchThreadID){
    uint id=tid.x;if(id>=rayCount)return;VRay r=gRays[id];float2 origin=float2(r.ox,r.oy);float2 dir=normalize(float2(r.dx,r.dy));
    float best=r.range;uint bestType=0,bestColor=0;float2 bestTarget=float2(0.0,0.0);int bestCreatureId=-1;
    uint cellsSeen=0,pc=0,cc=0,kc=0,ec=0;
    int2 cell=int2(floor(origin/cellSize));int sx=dir.x>0?1:(dir.x<0?-1:0);int sy=dir.y>0?1:(dir.y<0?-1:0);
    float inf=1e30;float tmx=inf,tmy=inf,tdx=inf,tdy=inf;
    if(sx!=0){float nb=(sx>0?(cell.x+1)*cellSize:cell.x*cellSize);tmx=(nb-origin.x)/dir.x;tdx=cellSize/abs(dir.x);if(tmx<0)tmx=0;}
    if(sy!=0){float nb=(sy>0?(cell.y+1)*cellSize:cell.y*cellSize);tmy=(nb-origin.y)/dir.y;tdy=cellSize/abs(dir.y);if(tmy<0)tmy=0;}
    float traveled=0.0;
    [loop]for(uint stepNo=0;stepNo<512 && traveled<=best;stepNo++){
        int wx=wrapCell(cell.x,(int)cols), wy=wrapCell(cell.y,(int)rows);
        uint key=(uint)wy*cols+(uint)wx;CellRange cr=gCells[key];cellsSeen++;
        float2 cellCenter=(float2(cell)+0.5)*cellSize;
        [loop]for(uint j=0;j<cr.count;j++){
            VObj o=gObjects[cr.start+j];
            if(o.type==2 && o.creatureId==r.observerId)continue;
            float2 baseCenter=float2(o.px,o.py);
            float2 worldSize=float2(worldW,worldH);
            float2 imageShift=round((cellCenter-baseCenter)/worldSize)*worldSize;
            float2 objCenter=baseCenter+imageShift;
            float2 coverCenter=float2(o.coverX,o.coverY)+imageShift;
            if(o.type==1){pc++;if(r.insidePass>0.5){float cover=o.radius+r.observerRadius*r.coverWeight;float2 d=objCenter-origin;if(dot(d,d)<=cover*cover)continue;}}
            else if(o.type==2)cc++;else if(o.type==3)kc++;else if(o.type==4)ec++;
            float hit=(o.type==1)?rayTrefoil(origin,dir,objCenter,o.radius,best):rayCircle(origin,dir,objCenter,o.radius,best);if(hit<0.0||hit>=best)continue;
            if(o.type==2 && o.coverRadius>0.0){
                float ownCover=o.coverRadius+r.observerRadius*r.coverWeight;float2 od=coverCenter-origin;
                bool sameCover=dot(od,od)<=ownCover*ownCover;
                if(!sameCover){float2 tc=objCenter-origin;float centerAlong=dot(tc,dir);float ch=rayCircle(origin,dir,coverCenter,o.coverRadius,centerAlong);if(ch>=0.0&&ch<centerAlong)continue;}
            }
            best=hit;bestType=o.type;bestColor=o.color;bestTarget=objCenter;bestCreatureId=o.creatureId;
        }
        if(tmx<tmy){traveled=tmx;tmx+=tdx;cell.x+=sx;}else{traveled=tmy;tmy+=tdy;cell.y+=sy;}
    }
    VResult z;z.distance=best;z.rangeUsed=max(0.0001,r.range);z.type=bestType;z.color=bestColor;z.observerId=r.observerId;z.raySlot=r.raySlot;z.cells=cellsSeen;z.plantChecks=pc;z.creatureChecks=cc;z.carcassChecks=kc;z.eggChecks=ec;z.pad=0;z.targetX=bestTarget.x;z.targetY=bestTarget.y;z.targetCreatureId=bestCreatureId;z.pad2=0;gOut[id]=z;
}
)HLSL";
        ID3DBlob* blob=nullptr;ID3DBlob* err=nullptr;UINT flags=D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3;
        HRESULT hr=winshim::D3DCompile_(hlsl,std::strlen(hlsl),"vision_compute",nullptr,nullptr,"CSMain","cs_5_0",flags,0,&blob,&err);
        if(FAILED(hr)){if(err){disabledReason=(const char*)err->GetBufferPointer();err->Release();}return false;}if(err)err->Release();
        hr=winshim::device->CreateComputeShader(blob->GetBufferPointer(),blob->GetBufferSize(),nullptr,&shader);blob->Release();
        if(FAILED(hr)){disabledReason="CreateComputeShader falhou";return false;}
        D3D11_BUFFER_DESC cb{};cb.ByteWidth=32;cb.Usage=D3D11_USAGE_DYNAMIC;cb.BindFlags=D3D11_BIND_CONSTANT_BUFFER;cb.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        if(FAILED(winshim::device->CreateBuffer(&cb,nullptr,&constantBuffer))){disabledReason="constant buffer falhou";return false;}
        initialized=true;return true;
    }

public:
    ~GpuVisionEngine(){shutdown();}
    void shutdown(){
        for(int i=0;i<2;++i){rel(query[i]);rel(staging[i]);pending[i]=false;}
        rel(outputUav);rel(outputBuffer);rel(cellSrv);rel(objectSrv);rel(raySrv);rel(cellBuffer);rel(objectBuffer);rel(rayBuffer);rel(constantBuffer);rel(shader);
        initialized=false;
    }

    bool ensureInitialized(){
        if(permanentlyDisabled)return false;if(initialized)return true;
        if(!compile()){permanentlyDisabled=true;return false;}return true;
    }
    void invalidatePending(){ if(initialized) shutdown(); }

    bool collect(std::vector<GpuVisionResult>& out){
        if(!initialized||!winshim::context)return false;
        bool completed[2]{false,false};int bestSlot=-1;std::uint64_t bestSerial=lastCollectedSerial;
        for(int i=0;i<2;++i){
            if(!pending[i])continue;
            BOOL done=FALSE;HRESULT q=winshim::context->GetData(query[i],&done,sizeof(done),D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if(q!=S_OK||!done)continue;
            completed[i]=true;
            if(pendingSerial[i]>=bestSerial){bestSerial=pendingSerial[i];bestSlot=i;}
        }
        bool got=false;
        if(bestSlot>=0){
            D3D11_MAPPED_SUBRESOURCE m{};
            if(SUCCEEDED(winshim::context->Map(staging[bestSlot],0,D3D11_MAP_READ,0,&m))){
                out.resize(pendingCount[bestSlot]);
                if(!out.empty())std::memcpy(out.data(),m.pData,out.size()*sizeof(GpuVisionResult));
                winshim::context->Unmap(staging[bestSlot],0);
                lastCollectedSerial=bestSerial;got=true;
            }
        }
        for(int i=0;i<2;++i)if(completed[i]){pending[i]=false;pendingCount[i]=0;++readbacks;}
        return got;
    }

    bool dispatch(const std::vector<GpuVisionCellRange>& cells,const std::vector<GpuVisionObject>& objects,const std::vector<GpuVisionRay>& rays,int cols,int rows,float worldW,float worldH){
        if(rays.empty())return true;if(!ensureInitialized())return false;
        int slot=-1;for(int i=0;i<2;++i)if(!pending[i]){slot=i;break;}if(slot<0){++skippedDispatches;return true;}
        if(!ensureInput<GpuVisionCellRange>(cellBuffer,cellSrv,cellCapacity,cells.size())||!ensureInput<GpuVisionObject>(objectBuffer,objectSrv,objectCapacity,objects.size())||!ensureInput<GpuVisionRay>(rayBuffer,raySrv,rayCapacity,rays.size())||!ensureOutput(rays.size())){permanentlyDisabled=true;disabledReason="falha ao criar buffers compute";return false;}
        if(!upload(cellBuffer,cells)||!upload(objectBuffer,objects)||!upload(rayBuffer,rays)){permanentlyDisabled=true;disabledReason="falha upload compute";return false;}
        struct CB{std::uint32_t rayCount,cols,rows;float cellSize;float worldW,worldH,p0,p1;} cb{(std::uint32_t)rays.size(),(std::uint32_t)cols,(std::uint32_t)rows,cfg::CELL,worldW,worldH,0,0};
        D3D11_MAPPED_SUBRESOURCE cm{};if(FAILED(winshim::context->Map(constantBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&cm)))return false;std::memcpy(cm.pData,&cb,sizeof(cb));winshim::context->Unmap(constantBuffer,0);
        ID3D11ShaderResourceView* srvs[3]{cellSrv,objectSrv,raySrv};winshim::context->CSSetShaderResources(0,3,srvs);winshim::context->CSSetUnorderedAccessViews(0,1,&outputUav,nullptr);winshim::context->CSSetConstantBuffers(0,1,&constantBuffer);winshim::context->CSSetShader(shader,nullptr,0);
        winshim::context->Dispatch((UINT)((rays.size()+63)/64),1,1);
        ID3D11ShaderResourceView* ns[3]{nullptr,nullptr,nullptr};ID3D11UnorderedAccessView* nu=nullptr;ID3D11Buffer* nb=nullptr;
        winshim::context->CSSetShaderResources(0,3,ns);winshim::context->CSSetUnorderedAccessViews(0,1,&nu,nullptr);winshim::context->CSSetConstantBuffers(0,1,&nb);winshim::context->CSSetShader(nullptr,nullptr,0);
        winshim::context->CopyResource(staging[slot],outputBuffer);winshim::context->End(query[slot]);pending[slot]=true;pendingCount[slot]=rays.size();pendingSerial[slot]=++serialCounter;++dispatches;lastObjectRefs=objects.size();return true;
    }

    std::size_t pendingBatches() const {return (pending[0]?1u:0u)+(pending[1]?1u:0u);}
    std::size_t estimatedGpuBytes() const {
        return cellCapacity*sizeof(GpuVisionCellRange)+objectCapacity*sizeof(GpuVisionObject)+rayCapacity*sizeof(GpuVisionRay)+
            outputCapacity*sizeof(GpuVisionResult)*3u+32u;
    }
#else
public:
    ~GpuVisionEngine()=default;
    bool ensureInitialized(){permanentlyDisabled=true;disabledReason="nao-Windows";return false;}
    void invalidatePending(){}
    bool collect(std::vector<GpuVisionResult>&){return false;}
    bool dispatch(const std::vector<GpuVisionCellRange>&,const std::vector<GpuVisionObject>&,const std::vector<GpuVisionRay>&,int,int,float,float){return false;}
    std::size_t pendingBatches() const{return 0;}
    std::size_t estimatedGpuBytes() const{return 0;}
#endif
};

// [SEC-SIMULATION] Orquestrador da simulacao e sistemas do mundo
class Simulation {
public:
    SpatialGrid<Creature> creatureGrid{cfg::CELL, cfg::WORLD_W, cfg::WORLD_H};
    SpatialGrid<Plant> plantGrid{cfg::CELL, cfg::WORLD_W, cfg::WORLD_H};
    // Grade separada apenas para plantas arrancadas. A grade geral continua
    // sendo usada por visao/alimentacao, mas a fisica nao precisa mais varrer
    // centenas de arvores enraizadas para descarta-las.
    SpatialGrid<Carcass> carcassGrid{cfg::CELL, cfg::WORLD_W, cfg::WORLD_H};
    SpatialGrid<Egg> eggGrid{cfg::CELL, cfg::WORLD_W, cfg::WORLD_H};
    // Compute shader da visao + buffers CPU reutilizados para montar a grade unificada.
    GpuVisionEngine gpuVision;
    std::vector<GpuVisionCellRange> gpuVisionCells;
    std::vector<GpuVisionObject> gpuVisionObjects;
    std::vector<GpuVisionRay> gpuVisionRays;
    std::vector<GpuVisionResult> gpuVisionResults;
    std::vector<std::uint32_t> gpuVisionCellCounts;
    std::vector<std::uint32_t> gpuVisionCellCursor;

    StableObjectPool<Creature,256> creaturePool;
    StableObjectPool<Plant,512> plantPool;
    StableObjectPool<Carcass,256> carcassPool;
    StableObjectPool<Egg,256> eggPool;
    std::vector<Creature*> creatures;
    std::vector<Plant*> plants;
    std::vector<Carcass*> carcasses;
    std::vector<Egg*> eggs;
    // Buffers de ponteiros para objetos ja reservados nos pools.
    std::vector<Plant*> plantNewbornScratch;
    std::vector<Creature*> creatureNewbornScratch;
    std::vector<Egg*> eggNewbornScratch;
    std::vector<Creature*> brainDueScratch;
    ParallelExecutor brainExecutor;
    std::vector<PhysicalProxy> physicalScratch;
    // Corpos que precisam colidir entre si ficam separados de plantas soltas.
    // Isso permite processar planta x corpo sem jamais enumerar planta x planta.
    std::vector<std::vector<int>> physicsBuckets;
    std::vector<int> activePhysicsBuckets;
    std::vector<int> physicsBucketActiveSlot;
    int physicsBucketCols=0,physicsBucketRows=0;
    std::vector<Creature*> byId;
    PopulationLimits limits{};
    WatchSession watch{};
    int selectedId = -1;
    // Controle manual e ferramenta de observacao, nao parte da biologia nem do save.
    // A rede neural continua calculada; apenas motores/mordida/agarrar sao substituidos.
    int manualControlCreatureId = -1;
    bool manualForward=false,manualReverse=false,manualTurnLeft=false,manualTurnRight=false;
    bool manualStrafeLeft=false,manualStrafeRight=false,manualBiteHeld=false;
    int nextCreatureId = 1;
    float simTime = 0.0f;
    double lastUpdateMs = 0.0;
    float plantSwayAccumulator = 0.0f;
    float statsAccumulator = 0.0f;
    float historyAccumulator = 0.0f;
    // Reordena apenas as LISTAS de ponteiros por proximidade fisica nos pools estaveis.
    // Os enderecos dos objetos nao mudam; grids/handles continuam validos.
    float localitySortAccumulator = 0.0f;
    EvolutionStats evo{};
    ReinforcementRecordBook reinforcementRecords{};
    std::vector<EvolutionHistoryPoint> evolutionHistory;
    std::vector<GenealogyRecord> genealogy;
    std::unordered_map<int,std::size_t> genealogyIndex;
    std::vector<NoteTask> notes;
    std::uint32_t nextNoteId = 1;
    PerfStats perf{};
    int lastMapResizeDestroyed = 0;
    MinimumLineageSeed minimumLineageSeed{};
    int minimumLineageDebt = 0; // vagas do minimo reservadas para descendentes de mortos

    Simulation() {
        creatures.reserve(2048);
        plants.reserve(4096);
        carcasses.reserve(1024);
        eggs.reserve(1024);
        plantNewbornScratch.reserve(64);
        creatureNewbornScratch.reserve(64);
        eggNewbornScratch.reserve(64);
        brainDueScratch.reserve(1024);
        physicalScratch.reserve(4096);
        evolutionHistory.reserve(cfg::MAX_EVOLUTION_HISTORY);
        genealogy.reserve(8192);
        activePhysicsBuckets.reserve(512);
        ensureMinimums();
        refreshEvolutionStats();
    }

    saveio::Snapshot makeSaveSnapshot(const saveio::SessionMeta& meta) const {
        saveio::Snapshot s;
        for(int i=0;i<4;++i) s.rng[(std::size_t)i]=gRng.s[i];
        s.worldW=cfg::WORLD_W; s.worldH=cfg::WORLD_H;
        s.limits=limits; s.evo=evo; s.reinforcementRecords=reinforcementRecords; s.selectedId=selectedId; s.nextCreatureId=nextCreatureId;
        s.simTime=simTime; s.plantSwayAccumulator=plantSwayAccumulator; s.statsAccumulator=statsAccumulator; s.historyAccumulator=historyAccumulator;
        s.minimumLineageDebt=minimumLineageDebt; s.watch=watch; s.meta=meta;
        s.lineage.valid=minimumLineageSeed.valid;
        if(s.lineage.valid){s.lineage.genome=minimumLineageSeed.genome;s.lineage.generation=minimumLineageSeed.generation;s.lineage.parentId=minimumLineageSeed.parentId;s.lineage.origin=minimumLineageSeed.origin;s.lineage.parentSize=minimumLineageSeed.parentSize;s.lineage.learnedWeights=minimumLineageSeed.learnedWeights;s.lineage.memoryRegisters=minimumLineageSeed.memoryRegisters;s.lineage.sexualHeritageDepth=minimumLineageSeed.sexualHeritageDepth;}

        s.creatures.reserve(creatures.size());
        for(const Creature* c:creatures){
            saveio::CreatureState x;
            x.id=c->id;x.pos=c->pos;x.angle=c->angle;x.speed=c->speed;x.lateralSpeed=c->lateralSpeed;x.generation=c->generation;x.parentId=c->parentId;x.secondParentId=c->secondParentId;x.genome=c->genome;
            x.size=c->size;x.energy=c->energy;x.health=c->health;x.age=c->age;x.lifecycleTimer=c->lifecycleTimer;x.developmentProgress=c->developmentProgress;x.senescenceProgress=c->senescenceProgress;
            x.ageSpeedMult=c->ageSpeedMult;x.ageAccelMult=c->ageAccelMult;x.ageRotationMult=c->ageRotationMult;x.ageBiteMult=c->ageBiteMult;x.ageRegenMult=c->ageRegenMult;x.ageMetabolismMult=c->ageMetabolismMult;x.ageFertilityMult=c->ageFertilityMult;x.longevityMaintenanceMult=c->longevityMaintenanceMult;
            x.naturalDeath=c->naturalDeath;x.deathCause=(std::uint8_t)c->deathCause;x.reproCooldown=c->reproCooldown;x.biteCooldown=c->biteCooldown;x.biteAnimation=c->biteAnimation;x.brainTimer=c->brainTimer;
            x.hidden=c->hidden;x.brainState=c->brainState;x.netOut=c->netOut;x.brainOut=c->brainOut;x.emotionState=c->emotionState;x.emotionDrive=c->emotionDrive;x.displayColor=c->displayColor;
            x.vegetationFrictionMult=c->vegetationFrictionMult;x.rootedPlantCover=c->rootedPlantCover;x.pushVelocity=c->pushVelocity;x.angularVelocity=c->angularVelocity;x.contactPressure=c->contactPressure;x.recentImpact=c->recentImpact;x.recentDamage=c->recentDamage;x.recentEnergyGain=c->recentEnergyGain;
            x.memoryRegisters=c->memoryRegisters;x.learnedWeights=c->learnedWeights;x.eligibilityTraces=c->eligibilityTraces;x.lastPlasticReward=c->lastPlasticReward;x.plasticUpdates=c->plasticUpdates;
            x.reinforcementPending=c->reinforcementPending;x.reinforcementTotal=c->reinforcementTotal;x.reinforcementPositive=c->reinforcementPositive;x.reinforcementNegative=c->reinforcementNegative;x.reinforcementEvents=c->reinforcementEvents;
            x.timeHealthAbove50=c->timeHealthAbove50;x.timeHealthBelow50=c->timeHealthBelow50;x.timeEnergyAbove50=c->timeEnergyAbove50;x.timeEnergyBelow50=c->timeEnergyBelow50;x.eggsLaidLifetime=c->eggsLaidLifetime;x.matingPartnerId=c->matingPartnerId;x.matingContactTime=c->matingContactTime;x.sexualHeritageDepth=c->sexualHeritageDepth;
            x.longMemory=c->longMemory;x.longMemoryHead=c->longMemoryHead;x.longMemoryCount=c->longMemoryCount;
            x.grabbedKind=(std::uint8_t)c->grabbedKind;x.grabbedByCreatureId=c->grabbedByCreatureId;x.grabLoadRatio=c->grabLoadRatio;
            if(c->grabbedKind==GrabKind::Creature && c->grabbedCreature) x.grabbedTargetRef=c->grabbedCreature->id;
            else if(c->grabbedKind==GrabKind::Plant && c->grabbedPlant){auto it=std::find(plants.begin(),plants.end(),c->grabbedPlant);x.grabbedTargetRef=it==plants.end()?-1:(int)std::distance(plants.begin(),it);}
            else if(c->grabbedKind==GrabKind::Carcass && c->grabbedCarcass){auto it=std::find(carcasses.begin(),carcasses.end(),c->grabbedCarcass);x.grabbedTargetRef=it==carcasses.end()?-1:(int)std::distance(carcasses.begin(),it);}
            x.speciesId=c->speciesId;
            s.creatures.push_back(std::move(x));
        }
        s.plants.reserve(plants.size());
        for(const Plant* p:plants){
            saveio::PlantState x; x.pos=p->pos;x.size=p->size;x.biomassSize=p->biomassSize;x.hasBeenEaten=p->hasBeenEaten;x.growthDuration=p->growthDuration;x.reproTimer=p->reproTimer;x.nextRepro=p->nextRepro;x.genes=p->genes;x.visualOffset={0.0f,0.0f};x.detached=false;x.velocity={0.0f,0.0f};x.detachedAge=0.0f;x.detachedDuration=0.0f;x.detachedDecayPerSecond=0.0f;x.grabbedByCreatureId=-1;s.plants.push_back(x);
        }
        s.carcasses.reserve(carcasses.size());
        for(const Carcass* k:carcasses){
            saveio::CarcassState x; x.pos=k->pos;x.bodySize=k->bodySize;x.age=k->age;x.duration=k->duration;x.initialEnergy=k->initialEnergy;x.energy=k->energy;x.maxBite=k->maxBite;x.sensorRadius=k->sensorRadius;x.visualSize=k->visualSize;x.glowRadius=k->glowRadius;x.decayPerSecond=k->decayPerSecond;x.velocity=k->velocity;x.grabbedByCreatureId=k->grabbedByCreatureId;s.carcasses.push_back(x);
        }
        s.eggs.reserve(eggs.size());
        for(const Egg* e:eggs){
            saveio::EggState x; x.pos=e->pos;x.genome=e->genome;x.generation=e->generation;x.parentId=e->parentId;x.secondParentId=e->secondParentId;x.minimumReplacement=e->minimumReplacement;x.childSize=e->childSize;x.childMaxEnergy=e->childMaxEnergy;x.radius=e->radius;x.age=e->age;x.incubationDuration=e->incubationDuration;x.initialEnergy=e->initialEnergy;x.energy=e->energy;x.initialBlood=e->initialBlood;x.blood=e->blood;x.maxBite=e->maxBite;x.velocity=e->velocity;x.inheritedLearnedWeights=e->inheritedLearnedWeights;x.inheritedMemoryRegisters=e->inheritedMemoryRegisters;x.inheritedLongMemory=e->inheritedLongMemory;x.inheritedLongMemoryHead=e->inheritedLongMemoryHead;x.inheritedLongMemoryCount=e->inheritedLongMemoryCount;x.hasInheritedLearning=e->hasInheritedLearning;x.qualityScore=e->qualityScore;x.plannedBirthCount=e->plannedBirthCount;x.conceivedWithMate=e->conceivedWithMate;x.sexualHeritageDepth=e->sexualHeritageDepth;s.eggs.push_back(std::move(x));
        }
        // Campo legado de eventos removidos permanece vazio em novos saves.
        s.evolutionHistory=evolutionHistory; s.genealogy=genealogy;
        s.notes=notes; s.nextNoteId=nextNoteId;
        return s;
    }

    bool saveToFile(const std::string& path,const saveio::SessionMeta& meta,std::string& error) const {
        return saveio::writeSnapshot(path,makeSaveSnapshot(meta),error);
    }

    bool applySaveSnapshot(const saveio::Snapshot& s,std::string& error) {
        gpuVision.invalidatePending();
        stopManualControl();
        if(s.worldW<cfg::WORLD_MIN_SIZE||s.worldW>cfg::WORLD_MAX_SIZE||s.worldH<cfg::WORLD_MIN_SIZE||s.worldH>cfg::WORLD_MAX_SIZE){error="Tamanho de mundo invalido no save.";return false;}

        creatureGrid.clear();plantGrid.clear();carcassGrid.clear();eggGrid.clear();
        creatures.clear();plants.clear();carcasses.clear();eggs.clear();plantNewbornScratch.clear();creatureNewbornScratch.clear();eggNewbornScratch.clear();brainDueScratch.clear();physicalScratch.clear();
        creaturePool.clear();plantPool.clear();carcassPool.clear();eggPool.clear();byId.clear();

        // As configuracoes sao carregadas separadamente de vida_config.ini antes deste snapshot.
        cfg::sanitizeTuning();cfg::WORLD_W=s.worldW;cfg::WORLD_H=s.worldH;
        limits=s.limits;evo=s.evo;reinforcementRecords=s.reinforcementRecords;selectedId=s.selectedId;nextCreatureId=s.nextCreatureId;simTime=s.simTime;plantSwayAccumulator=s.plantSwayAccumulator;statsAccumulator=s.statsAccumulator;historyAccumulator=s.historyAccumulator;minimumLineageDebt=s.minimumLineageDebt;evolutionHistory=s.evolutionHistory;genealogy=s.genealogy;genealogyIndex.clear();for(std::size_t gi=0;gi<genealogy.size();++gi)genealogyIndex[genealogy[gi].id]=gi;
        notes=s.notes; nextNoteId=std::max<std::uint32_t>(1,s.nextNoteId);
        for(const auto& note:notes) nextNoteId=std::max(nextNoteId,note.id+1);
        lastUpdateMs=0.0;perf={};lastMapResizeDestroyed=0;
        minimumLineageSeed={};
        if(s.lineage.valid){minimumLineageSeed.valid=true;minimumLineageSeed.genome=s.lineage.genome;minimumLineageSeed.generation=s.lineage.generation;minimumLineageSeed.parentId=s.lineage.parentId;minimumLineageSeed.origin=s.lineage.origin;minimumLineageSeed.parentSize=s.lineage.parentSize;minimumLineageSeed.learnedWeights=s.lineage.learnedWeights;minimumLineageSeed.memoryRegisters=s.lineage.memoryRegisters;minimumLineageSeed.sexualHeritageDepth=s.lineage.sexualHeritageDepth;}

        int maxId=0;
        for(const auto& x:s.creatures){
            Creature* c=creaturePool.create(x.id,x.pos,x.genome,x.generation,x.parentId,cfg::tuning.energyInitialBase);
            c->angle=x.angle;c->refreshDirection();c->speed=x.speed;c->lateralSpeed=x.lateralSpeed;c->secondParentId=x.secondParentId;c->size=x.size;c->age=x.age;c->lifecycleTimer=x.lifecycleTimer;c->developmentProgress=x.developmentProgress;c->senescenceProgress=x.senescenceProgress;
            c->ageSpeedMult=x.ageSpeedMult;c->ageAccelMult=x.ageAccelMult;c->ageRotationMult=x.ageRotationMult;c->ageBiteMult=x.ageBiteMult;c->ageRegenMult=x.ageRegenMult;c->ageMetabolismMult=x.ageMetabolismMult;c->ageFertilityMult=x.ageFertilityMult;c->longevityMaintenanceMult=x.longevityMaintenanceMult;
            c->naturalDeath=x.naturalDeath;c->deathCause=(DeathCause)x.deathCause;c->reproCooldown=x.reproCooldown;c->biteCooldown=x.biteCooldown;c->biteAnimation=x.biteAnimation;c->brainTimer=x.brainTimer;
            c->compiled=compileBrain(c->genome.brain);c->hidden=x.hidden;if((int)c->hidden.size()!=c->genome.brain.hiddenCount)c->hidden.resize(c->genome.brain.hiddenCount,0.0f);
            c->brainState=x.brainState;c->netOut=x.netOut;c->brainOut=x.brainOut;c->emotionState=x.emotionState;c->emotionDrive=x.emotionDrive;
            c->vegetationFrictionMult=x.vegetationFrictionMult;c->rootedPlantCover=x.rootedPlantCover;c->pushVelocity=x.pushVelocity;c->angularVelocity=x.angularVelocity;c->contactPressure=x.contactPressure;c->recentImpact=x.recentImpact;c->recentDamage=x.recentDamage;c->recentEnergyGain=x.recentEnergyGain;c->longMemory=x.longMemory;c->longMemoryHead=x.longMemoryHead;c->longMemoryCount=x.longMemoryCount;c->grabbedByCreatureId=x.grabbedByCreatureId;c->grabLoadRatio=x.grabLoadRatio;c->speciesId=x.speciesId;
            c->memoryRegisters=x.memoryRegisters;c->lastPlasticReward=x.lastPlasticReward;c->plasticUpdates=x.plasticUpdates;
            c->reinforcementPending=x.reinforcementPending;c->reinforcementTotal=x.reinforcementTotal;c->reinforcementPositive=x.reinforcementPositive;c->reinforcementNegative=x.reinforcementNegative;c->reinforcementEvents=x.reinforcementEvents;
            c->timeHealthAbove50=x.timeHealthAbove50;c->timeHealthBelow50=x.timeHealthBelow50;c->timeEnergyAbove50=x.timeEnergyAbove50;c->timeEnergyBelow50=x.timeEnergyBelow50;c->eggsLaidLifetime=x.eggsLaidLifetime;c->matingPartnerId=x.matingPartnerId;c->matingContactTime=x.matingContactTime;c->sexualHeritageDepth=x.sexualHeritageDepth;
            if(x.learnedWeights.size()==c->genome.brain.connections.size())c->learnedWeights=x.learnedWeights;
            if(x.eligibilityTraces.size()==c->genome.brain.connections.size())c->eligibilityTraces=x.eligibilityTraces;
            c->cachePhenotype();c->energy=clampf(x.energy,0.0f,c->maxEnergy);c->health=clampf(x.health,0.0f,cfg::tuning.healthMax);c->speed=clampf(c->speed,-c->maxReverseEffective,c->maxForwardEffective);c->lateralSpeed=clampf(c->lateralSpeed,-c->maxLateralEffective,c->maxLateralEffective);c->displayColor=x.displayColor;
            creatures.push_back(c);indexCreature(c);maxId=std::max(maxId,c->id);
        }
        for(const auto& x:s.plants){
            Plant* p=plantPool.create(x.pos,x.genes);p->size=std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,x.size);p->biomassSize=std::max(0.0f,x.biomassSize);p->hasBeenEaten=x.hasBeenEaten||x.detached||(p->biomassSize+0.001f<p->size);p->growthDuration=x.growthDuration;p->reproTimer=x.reproTimer;p->nextRepro=x.nextRepro;p->visualOffset={0.0f,0.0f};p->detached=false;p->velocity={0.0f,0.0f};p->detachedAge=0.0f;p->detachedDuration=0.0f;p->detachedDecayPerSecond=0.0f;p->grabbedByCreatureId=-1;plants.push_back(p);
        }
        for(const auto& x:s.carcasses){
            Carcass* k=carcassPool.create(x.pos,x.bodySize);k->age=x.age;k->duration=x.duration;k->initialEnergy=x.initialEnergy;k->energy=x.energy;k->maxBite=x.maxBite;k->sensorRadius=x.sensorRadius;k->visualSize=x.visualSize;k->glowRadius=x.glowRadius;k->decayPerSecond=x.decayPerSecond;k->velocity=x.velocity;k->grabbedByCreatureId=x.grabbedByCreatureId;carcasses.push_back(k);
        }
        for(const auto& x:s.eggs){
            Egg* e=eggPool.create(x.pos,x.genome,x.generation,x.parentId,x.initialEnergy,x.initialBlood);e->secondParentId=x.secondParentId;e->minimumReplacement=x.minimumReplacement;e->childSize=x.childSize;e->childMaxEnergy=x.childMaxEnergy;e->radius=0.50f*std::max(cfg::tuning.eggRadiusMin,cfg::tuning.eggRadiusBase+e->childSize*cfg::tuning.eggRadiusSizeScale);e->age=x.age;e->incubationDuration=x.incubationDuration;e->initialEnergy=x.initialEnergy;e->energy=x.energy;e->initialBlood=x.initialBlood;e->blood=x.blood;e->maxBite=std::max(cfg::tuning.eggBiteMin,e->radius*cfg::tuning.eggBiteRadiusScale);e->velocity=x.velocity;e->inheritedLearnedWeights=x.inheritedLearnedWeights;e->inheritedMemoryRegisters=x.inheritedMemoryRegisters;e->inheritedLongMemory=x.inheritedLongMemory;e->inheritedLongMemoryHead=x.inheritedLongMemoryHead%cfg::LONG_MEMORY_SLOTS;e->inheritedLongMemoryCount=(std::uint8_t)std::min<int>(x.inheritedLongMemoryCount,cfg::LONG_MEMORY_SLOTS);e->hasInheritedLearning=x.hasInheritedLearning;e->qualityScore=x.qualityScore;e->plannedBirthCount=std::clamp(x.plannedBirthCount,1,10);e->conceivedWithMate=x.conceivedWithMate;e->sexualHeritageDepth=x.sexualHeritageDepth;eggs.push_back(e);
        }
        // Reconecta referencias de agarrar somente depois que todos os objetos existem novamente.
        for(std::size_t i=0;i<s.creatures.size()&&i<creatures.size();++i){
            Creature* c=creatures[i];const auto& x=s.creatures[i];c->grabbedKind=(GrabKind)x.grabbedKind;
            if(c->grabbedKind==GrabKind::Creature){c->grabbedCreature=creatureById(x.grabbedTargetRef);if(!c->grabbedCreature)c->grabbedKind=GrabKind::None;}
            else if(c->grabbedKind==GrabKind::Plant){c->grabbedKind=GrabKind::None;c->grabbedPlant=nullptr;}
            else if(c->grabbedKind==GrabKind::Carcass){if(x.grabbedTargetRef>=0&&x.grabbedTargetRef<(int)carcasses.size())c->grabbedCarcass=carcasses[(std::size_t)x.grabbedTargetRef];else c->grabbedKind=GrabKind::None;}
            if(c->grabbedKind==GrabKind::None)c->grabLoadRatio=0.0f;
        }
        // Eventos acusticos de saves antigos sao lidos pelo formato, mas descartados.
        nextCreatureId=std::max(nextCreatureId,maxId+1);
        for(int i=0;i<4;++i)gRng.s[i]=s.rng[(std::size_t)i];
        watch=s.watch;
        if(selectedId>=0&&!creatureById(selectedId))selectedId=-1;
        if(watch.alive&&!creatureById(watch.creatureId))watch.alive=false;
        rebuildSpatialStructures();
        updateSensors();
        refreshEvolutionStats();
        return true;
    }

    bool loadFromFile(const std::string& path,saveio::SessionMeta& meta,std::string& error) {
        saveio::Snapshot s;
        if(!saveio::readSnapshot(path,s,error))return false;
        if(!applySaveSnapshot(s,error))return false;
        meta=s.meta;
        return true;
    }


    void rebuildSpatialStructures() {
        creatureGrid.reconfigure(cfg::WORLD_W, cfg::WORLD_H);
        plantGrid.reconfigure(cfg::WORLD_W, cfg::WORLD_H);
        carcassGrid.reconfigure(cfg::WORLD_W, cfg::WORLD_H);
        eggGrid.reconfigure(cfg::WORLD_W, cfg::WORLD_H);

        for (Creature* c : creatures) { c->wrap(); creatureGrid.insert(c); }
        for (Plant* p : plants) {
            constrainCircleToWorld(p->pos,p->radius());
            plantGrid.insert(p);
            p->detached=false;p->velocity={0.0f,0.0f};p->grabbedByCreatureId=-1;
            p->detachedGridCell = INVALID_GRID_KEY; p->detachedGridSlot = -1;
        }
        for (Carcass* k : carcasses) { resolveCircleWorldBarrier(k->pos,k->velocity,k->radius()); carcassGrid.insert(k); }
        for (Egg* e : eggs) { resolveCircleWorldBarrier(e->pos,e->velocity,e->radius); eggGrid.insert(e); }

        physicsBuckets.clear();
        activePhysicsBuckets.clear();
        physicsBucketActiveSlot.clear();
        physicsBucketCols=physicsBucketRows=0;
    }

    int resizeWorld(float requestedSize) {
        // Mundo finito: mudar o tamanho move para dentro qualquer entidade que tenha
        // ficado alem da nova borda. Nada e destruido apenas por redimensionar.
        float newSize=clampf(requestedSize,cfg::WORLD_MIN_SIZE,cfg::WORLD_MAX_SIZE);
        newSize=std::round(newSize/cfg::WORLD_SIZE_STEP)*cfg::WORLD_SIZE_STEP;
        newSize=clampf(newSize,cfg::WORLD_MIN_SIZE,cfg::WORLD_MAX_SIZE);
        if(std::fabs(newSize-cfg::WORLD_W)<0.5f)return 0;
        cfg::WORLD_W=newSize;cfg::WORLD_H=newSize;lastMapResizeDestroyed=0;
        gpuVision.invalidatePending();
        rebuildSpatialStructures();
        ensureMinimums();refreshEvolutionStats();updateSensors();
        return 0;
    }

    void optimizeEntityIterationLocality(){
        auto byCreatureLocality=[&](Creature* a,Creature* b){return creaturePool.localityKey(a)<creaturePool.localityKey(b);};
        auto byPlantLocality=[&](Plant* a,Plant* b){return plantPool.localityKey(a)<plantPool.localityKey(b);};
        auto byCarcassLocality=[&](Carcass* a,Carcass* b){return carcassPool.localityKey(a)<carcassPool.localityKey(b);};
        auto byEggLocality=[&](Egg* a,Egg* b){return eggPool.localityKey(a)<eggPool.localityKey(b);};
        std::sort(creatures.begin(),creatures.end(),byCreatureLocality);
        std::sort(plants.begin(),plants.end(),byPlantLocality);
        std::sort(carcasses.begin(),carcasses.end(),byCarcassLocality);
        std::sort(eggs.begin(),eggs.end(),byEggLocality);
    }

    Creature* creatureById(int id) const {
        return id>=0 && id<(int)byId.size()?byId[(std::size_t)id]:nullptr;
    }
    void indexCreature(Creature* c){
        if(!c||c->id<0)return;
        if((std::size_t)c->id>=byId.size())byId.resize((std::size_t)c->id+1,nullptr);
        byId[(std::size_t)c->id]=c;
    }
    void unindexCreature(int id){if(id>=0&&id<(int)byId.size())byId[(std::size_t)id]=nullptr;}
    Creature* selected() const { return creatureById(selectedId); }
    Creature* manualControlled() const { return creatureById(manualControlCreatureId); }
    bool manualControlActive() const { return manualControlled()!=nullptr; }
    bool isManualControlled(const Creature& c) const { return manualControlCreatureId==c.id; }
    void stopManualControl(){
        manualControlCreatureId=-1;
        manualForward=manualReverse=manualTurnLeft=manualTurnRight=false;
        manualStrafeLeft=manualStrafeRight=manualBiteHeld=false;
    }
    void setManualControl(Creature* c){
        if(!c||c->dead()){stopManualControl();return;}
        manualControlCreatureId=c->id;
        manualForward=manualReverse=manualTurnLeft=manualTurnRight=false;
        manualStrafeLeft=manualStrafeRight=manualBiteHeld=false;
        log(c->id,"Controle MANUAL ativado. Cerebro continua sendo calculado, mas motores sao comandados pelo usuario.");
    }
    void setManualInput(bool f,bool r,bool l,bool rr,bool sl,bool sr,bool bite){
        if(!manualControlActive()){stopManualControl();return;}
        manualForward=f;manualReverse=r;manualTurnLeft=l;manualTurnRight=rr;
        manualStrafeLeft=sl;manualStrafeRight=sr;manualBiteHeld=bite;
    }
    void toggleManualGrab(){
        Creature* c=manualControlled(); if(!c||c->dead()){stopManualControl();return;}
        if(c->grabbedKind!=GrabKind::None) releaseGrab(*c);
        else tryAcquireGrab(*c);
    }

    bool memoryEventRecent(const Creature& c,MemoryEventType type,float seconds) const {
        for(int i=0;i<(int)c.longMemoryCount;++i){
            const int slot=(int(c.longMemoryHead)-1-i+cfg::LONG_MEMORY_SLOTS)%cfg::LONG_MEMORY_SLOTS;
            const auto& e=c.longMemory[(std::size_t)slot];
            if(e.type==type && e.time>=0.0f && simTime-e.time<=seconds) return true;
        }
        return false;
    }

    void rememberEventDir(Creature& c,MemoryEventType type,float intensity,Vector2 worldDir){
        if(type==MemoryEventType::None || c.dead()) return;
        const float d2=length2(worldDir);
        if(d2>1e-8f) worldDir=mul(worldDir,1.0f/std::sqrt(d2)); else worldDir={0.0f,0.0f};
        LongMemoryEvent e; e.type=type;e.time=simTime;e.intensity=clampf(intensity,0.0f,1.0f);e.worldDir=worldDir;
        c.longMemory[(std::size_t)c.longMemoryHead]=e;
        c.longMemoryHead=(std::uint8_t)((c.longMemoryHead+1)%cfg::LONG_MEMORY_SLOTS);
        c.longMemoryCount=(std::uint8_t)std::min<int>(cfg::LONG_MEMORY_SLOTS,int(c.longMemoryCount)+1);
    }

    void rememberEventAt(Creature& c,MemoryEventType type,float intensity,Vector2 worldPos){
        rememberEventDir(c,type,intensity,toroidalDelta(c.pos,worldPos));
    }

    Creature* highestGenerationCreature() const {
        Creature* best = nullptr;
        for (Creature* c : creatures) {
            if (!best || c->generation > best->generation ||
                (c->generation == best->generation && c->age > best->age))
                best = c;
        }
        return best;
    }

    void registerGenealogy(Creature* c){
        if(!c || genealogyIndex.find(c->id)!=genealogyIndex.end())return;
        GenealogyRecord r; r.id=c->id;r.parentA=c->parentId;r.parentB=c->secondParentId;r.generation=c->generation;r.speciesId=c->speciesId;r.birthTime=simTime;r.deathTime=-1.0f;
        genealogyIndex[c->id]=genealogy.size();genealogy.push_back(r);
        if(genealogy.size()>200000){
            // Mantem o arquivo controlavel sem fazer milhares de erase O(n): remove em uma passada
            // somente os registros mortos mais antigos ate voltar para perto de 180 mil.
            std::size_t need=genealogy.size()-180000, dropped=0;
            std::vector<GenealogyRecord> kept; kept.reserve(genealogy.size()-need);
            for(const auto& oldRec:genealogy){
                if(dropped<need && oldRec.deathTime>=0.0f){++dropped;continue;}
                kept.push_back(oldRec);
            }
            genealogy.swap(kept);
            genealogyIndex.clear();for(std::size_t i=0;i<genealogy.size();++i)genealogyIndex[genealogy[i].id]=i;
        }
    }
    void markGenealogyDeath(const Creature& c){
        auto it=genealogyIndex.find(c.id);if(it==genealogyIndex.end())return;
        auto& r=genealogy[it->second];r.deathTime=simTime;r.speciesId=c.speciesId;
    }

    std::array<float,10> speciesSignature(const Creature& c) const {
        return {
            normalizeGene(c.genome.physical.bodySize,cfg::tuning.minCreatureSize,cfg::tuning.maxCreatureSize),
            normalizeGene(c.genome.physical.maxForwardSpeed,cfg::tuning.geneForwardMin,cfg::tuning.geneForwardMax),
            clampf(c.genome.physical.hardness,0.0f,1.0f),
            normalizeGene(c.genome.vision.range,cfg::tuning.visionMinRange,cfg::tuning.visionMaxRange),
            normalizeGene(c.genome.vision.focusExponent,cfg::tuning.visionFocusMin,cfg::tuning.visionFocusMax),
            normalizeGene(c.genome.reproduction.mateRange,cfg::tuning.mateRangeMin,cfg::tuning.mateRangeMax),
            normalizeGene(c.genome.mouth.biteForce,cfg::tuning.mouthBiteGeneMin,cfg::tuning.mouthBiteGeneMax),
            clampf((c.genome.reproduction.visualPreference+1.0f)*0.5f,0.0f,1.0f),
            clampf(float(c.genome.bodyColor.r)/255.0f,0.0f,1.0f),
            clampf(float(c.genome.bodyColor.b)/255.0f,0.0f,1.0f)
        };
    }
    void classifySpecies(){
        struct Cluster{std::array<float,10> center{};int n=0;};
        std::vector<Cluster> clusters;clusters.reserve(cfg::MAX_SPECIES);
        auto distance=[](const std::array<float,10>&a,const std::array<float,10>&b){float d=0;for(int i=0;i<10;++i){float x=a[i]-b[i];d+=x*x;}return std::sqrt(d/10.0f);};
        std::vector<Creature*> ordered=creatures;std::sort(ordered.begin(),ordered.end(),[](Creature*a,Creature*b){return a->id<b->id;});
        for(Creature* c:ordered){
            const auto sig=speciesSignature(*c);int best=-1;float bestD=1e9f;
            for(int i=0;i<(int)clusters.size();++i){float d=distance(sig,clusters[i].center);if(d<bestD){bestD=d;best=i;}}
            if((best<0 || bestD>cfg::tuning.speciesDistanceThreshold) && (int)clusters.size()<cfg::MAX_SPECIES){Cluster cl;cl.center=sig;cl.n=1;clusters.push_back(cl);best=(int)clusters.size()-1;}
            else if(best>=0){auto&cl=clusters[best];++cl.n;for(int i=0;i<10;++i)cl.center[i]+=(sig[i]-cl.center[i])/float(cl.n);}
            if(best<0) best=0;
            c->speciesId=best+1;
            auto it=genealogyIndex.find(c->id);if(it!=genealogyIndex.end())genealogy[it->second].speciesId=c->speciesId;
        }
        evo.speciesCount=(int)clusters.size();
    }

    void countCurrentEggOrigins(int& solo,int& mated) const {
        solo=0;mated=0;
        auto countOne=[&](const Egg* e){
            if(!e || e->destroyed() || e->minimumReplacement) return;
            if(e->conceivedWithMate || (e->secondParentId>=0 && e->secondParentId!=e->parentId)) ++mated;
            else ++solo;
        };
        for(const Egg* e:eggs) countOne(e);
        for(const Egg* e:eggNewbornScratch) countOne(e);
    }

    void recordEvolutionHistory(){
        EvolutionHistoryPoint h;
        h.time=simTime;
        h.creatures=(int)creatures.size();h.plants=(int)plants.size();h.carcasses=(int)carcasses.size();h.eggs=(int)eggs.size();
        h.maxGeneration=evo.maxGeneration;h.speciesCount=evo.speciesCount;h.starvationDeaths=evo.starvationDeaths;h.attackDeaths=evo.attackDeaths;h.avgGeneration=evo.avgGeneration;
        h.avgBodySize=evo.avgBodySize;h.avgForwardGene=evo.avgForwardGene;h.avgHidden=evo.avgHidden;h.avgConnections=evo.avgConnections;h.avgMemory=evo.avgMemory;
        h.legacyRemovedMetricA=evo.legacyRemovedMetricA;h.avgVisionFocus=evo.avgVisionFocus;h.legacyRemovedMetricB=evo.legacyRemovedMetricB;h.avgEnergyRatio=evo.avgEnergyRatio;h.avgAge=evo.avgAge;h.avgVisionRange=evo.avgVisionRange;
        countCurrentEggOrigins(h.eggsCurrentSolo,h.eggsCurrentMated);
        evolutionHistory.push_back(h);if(evolutionHistory.size()>cfg::MAX_EVOLUTION_HISTORY)evolutionHistory.erase(evolutionHistory.begin());
    }

    void refreshEvolutionStats() {
        evo.detachedPlants = 0; // v0.098: plantas soltas nao existem mais

        if (creatures.empty()) {
            evo.maxGeneration = 0;
            evo.avgGeneration = evo.avgAge = evo.oldestAge = 0.0f;
            evo.avgBodySize = evo.avgForwardGene = 0.0f;
            evo.avgHidden = evo.avgConnections = evo.avgLoops = evo.avgMemory = 0.0f;
            evo.legacyRemovedMetricA=evo.avgVisionFocus=evo.legacyRemovedMetricB=evo.avgEnergyRatio=evo.avgVisionRange=0.0f;evo.speciesCount=0;
            return;
        }

        double generation=0.0, age=0.0, body=0.0, speed=0.0;
        double hidden=0.0, connections=0.0, loops=0.0, memory=0.0, focus=0.0, energyRatio=0.0, visionRange=0.0;
        long long memoryNodes = 0;
        int maxGen = 1;
        float oldest = 0.0f;
        for (Creature* cp : creatures) {
            const Creature& c = *cp;
            generation += c.generation;
            age += c.age;
            body += c.size;
            speed += c.genome.physical.maxForwardSpeed;
            hidden += c.genome.brain.hiddenCount;
            connections += c.compiled.activeConnections;
            loops += c.compiled.recurrentConnections;
            focus += c.genome.vision.focusExponent;
            energyRatio += c.maxEnergy>0.0f ? c.energy/c.maxEnergy : 0.0f;
            visionRange += c.genome.vision.range;
            maxGen = std::max(maxGen, c.generation);
            oldest = std::max(oldest, c.age);
            for (const auto& n : c.genome.brain.hiddenNodes) { memory += n.memory; ++memoryNodes; }
        }
        const double n = double(creatures.size());
        evo.maxGeneration = maxGen;
        evo.avgGeneration = float(generation/n);
        evo.avgAge = float(age/n);
        evo.oldestAge = oldest;
        evo.avgBodySize = float(body/n);
        evo.avgForwardGene = float(speed/n);
        evo.avgHidden = float(hidden/n);
        evo.avgConnections = float(connections/n);
        evo.avgLoops = float(loops/n);
        evo.avgMemory = memoryNodes > 0 ? float(memory/double(memoryNodes)) : 0.0f;
        evo.legacyRemovedMetricA=0.0f;evo.avgVisionFocus=float(focus/n);evo.legacyRemovedMetricB=0.0f;evo.avgEnergyRatio=float(energyRatio/n);evo.avgVisionRange=float(visionRange/n);
        classifySpecies();
    }

    bool isWatching(int id) const { return watch.alive && watch.creatureId == id; }

    void log(int id, const std::string& text) {
        if (!isWatching(id)) return;
        watch.entries.push_back({simTime - watch.startedAt, text});
        if (watch.entries.size() > 1200) watch.entries.erase(watch.entries.begin(), watch.entries.begin() + 200);
    }

    void startWatch(Creature* c) {
        if (!c) return;
        if(manualControlCreatureId>=0 && manualControlCreatureId!=c->id) stopManualControl();
        if (watch.alive && watch.creatureId == c->id) { selectedId = c->id; return; }
        watch = {};
        watch.creatureId = c->id;
        watch.startedAt = simTime;
        watch.alive = true;
        selectedId = c->id;
        log(c->id, "Acompanhamento iniciado. Energia " + f2(c->energy) + "/" + f2(c->maxEnergy) + ", vida " + f2(c->health) + ", tamanho " + f2(c->size) + ".");
    }

    void closeWatch() { stopManualControl(); selectedId = -1; watch = {}; }

    static std::string f2(float v) {
        char buf[64]; std::snprintf(buf, sizeof(buf), "%.2f", v); return buf;
    }

    Creature* addCreature(Vector2 p, CreatureGenome g = randomGenome(), int generation = 1, int parent = -1, float initialEnergyBase = cfg::tuning.energyInitialBase) {
        if (limits.maxEnabled && (int)creatures.size() >= limits.maxCreatures) return nullptr;
        Creature* raw = creaturePool.create(nextCreatureId++, p, std::move(g), generation, parent, initialEnergyBase);
        raw->wrap();
        creatures.push_back(raw);
        indexCreature(raw);
        creatureGrid.insert(raw);
        registerGenealogy(raw);
        return raw;
    }

    Plant* addPlant(Vector2 p, PlantGenes g = randomPlantGenes()) {
        if (limits.maxEnabled && (int)plants.size() >= limits.maxPlants) return nullptr;
        Plant* raw = plantPool.create(p,g);
        constrainCircleToWorld(raw->pos,raw->radius());
        plants.push_back(raw);
        plantGrid.insert(raw);
        return raw;
    }

    Carcass* addCarcass(Vector2 p, float bodySize = cfg::tuning.baseCreatureSize) {
        Carcass* raw = carcassPool.create(p,clampf(bodySize,cfg::tuning.minCreatureSize,cfg::tuning.maxCreatureSize));
        constrainCircleToWorld(raw->pos,raw->radius());
        carcasses.push_back(raw);
        carcassGrid.insert(raw);
        return raw;
    }

    Egg* addManualEgg(Vector2 p) {
        CreatureGenome g = randomGenome();
        const float childMax = creatureMaxEnergyForSize(g.physical.bodySize);
        const float energy = childMax * cfg::tuning.eggEnergyFullBirthFraction;
        const float blood = cfg::tuning.healthMax * cfg::tuning.eggBloodFullBirthFraction;
        Egg* raw = eggPool.create(p,std::move(g),1,-1,energy,blood);
        constrainCircleToWorld(raw->pos,raw->radius);
        eggs.push_back(raw);
        eggGrid.insert(raw);
        return raw;
    }


    int futureCreaturePopulation() const {
        return (int)creatures.size() + (int)eggs.size() +
               (int)eggNewbornScratch.size() + (int)creatureNewbornScratch.size();
    }

    int pendingMinimumReplacementEggs() const {
        int pending = 0;
        for (const Egg* e : eggs) if (e && !e->destroyed() && e->minimumReplacement) ++pending;
        for (const Egg* e : eggNewbornScratch) if (e && !e->destroyed() && e->minimumReplacement) ++pending;
        return pending;
    }

    int minimumCoveredPopulation() const {
        return (int)creatures.size() + pendingMinimumReplacementEggs();
    }

    bool findSystemEggPosition(Vector2 origin, float parentSize, float childSize, Vector2& out) const {
        const float base = parentSize*0.5f + childSize*0.5f + cfg::tuning.creatureBirthGap + 1.0f;
        const float phase = rf(0.0f,2.0f*PI);
        for (int attempt=0; attempt<72; ++attempt) {
            const float ring = 1.0f + float(attempt/18)*0.75f;
            const float a = phase + 2.0f*PI*float(attempt%18)/18.0f;
            Vector2 p{origin.x + std::cos(a)*base*ring, origin.y + std::sin(a)*base*ring};
            if (creatureBirthPositionFree(p,childSize)) { out=p; return true; }
        }
        // Em mundos congestionados tenta qualquer espaco livre, sem mudar a linhagem.
        for (int attempt=0; attempt<160; ++attempt) {
            Vector2 p{rf(0.0f,cfg::WORLD_W),rf(0.0f,cfg::WORLD_H)};
            if (creatureBirthPositionFree(p,childSize)) { out=p; return true; }
        }
        return false;
    }

    Egg* spawnMinimumReplacementEgg(const MinimumLineageSeed& seed) {
        if (!seed.valid) return nullptr;
        if (limits.maxEnabled && futureCreaturePopulation() >= limits.maxCreatures) return nullptr;

        CreatureGenome childGenome = mutateGenome(seed.genome);
        const float childSize = clampf(childGenome.physical.bodySize,cfg::tuning.minCreatureSize,cfg::tuning.maxCreatureSize);
        Vector2 eggPos{};
        if (!findSystemEggPosition(seed.origin,seed.parentSize,childSize,eggPos)) return nullptr;

        // Como o progenitor ja morreu, o mecanismo de manutencao do minimo fornece
        // os recursos de referencia do ovo. DNA/geracao/parentId continuam vindo do morto.
        const float childMax = creatureMaxEnergyForSize(childSize);
        const float energy = childMax * cfg::tuning.eggEnergyFullBirthFraction;
        const float blood = cfg::tuning.healthMax * cfg::tuning.eggBloodFullBirthFraction;
        Egg* egg = eggPool.create(
            eggPos,std::move(childGenome),seed.generation+1,seed.parentId,energy,blood);
        prepareInheritedLearning(*egg,seed);
        egg->sexualHeritageDepth=seed.sexualHeritageDepth;
        egg->minimumReplacement = true;
        eggs.push_back(egg);
        eggGrid.insert(egg);
        ++evo.eggsLaid;
        ++evo.minimumReplacementEggs;
        return egg;
    }

    void resetWorld() {
        gpuVision.invalidatePending();
        closeWatch();
        creatureGrid.clear(); plantGrid.clear(); carcassGrid.clear(); eggGrid.clear(); 
        creatures.clear(); plants.clear(); carcasses.clear(); eggs.clear(); 
        plantNewbornScratch.clear(); creatureNewbornScratch.clear(); eggNewbornScratch.clear();
        brainDueScratch.clear(); physicalScratch.clear();
        creaturePool.clear(); plantPool.clear(); carcassPool.clear(); eggPool.clear(); 
        byId.clear();
        nextCreatureId = 1;
        simTime = 0.0f;
        lastUpdateMs = 0.0;
        plantSwayAccumulator = 0.0f;
        statsAccumulator = 0.0f;
        historyAccumulator = 0.0f;
        evolutionHistory.clear();genealogy.clear();genealogyIndex.clear();
        evo = {};
        perf = {};
        lastMapResizeDestroyed = 0;
        minimumLineageSeed = {};
        minimumLineageDebt = 0;
        ensureMinimums();
        refreshEvolutionStats();
    }

    int killEntities(int type, int amount) {
        amount = std::max(0, amount);
        int killed = 0;
        if (type == 0) { // bichos: morte real, gera carne
            const int n = std::min(amount, (int)creatures.size());
            int activeEnd = (int)creatures.size();
            for (int k = 0; k < n; ++k) {
                const int idx = ri(0, activeEnd - 1);
                creatures[idx]->health = 0.0f;
                std::swap(creatures[idx], creatures[activeEnd - 1]);
                --activeEnd;
            }
            processDeaths();
            killed = n;
        } else if (type == 1) { // plantas
            for (int k=0; k<amount && !plants.empty(); ++k) {
                int idx=ri(0,(int)plants.size()-1);
                Plant* victim=plants[idx];
                if(victim->grabbedByCreatureId>=0) releaseCarrierById(victim->grabbedByCreatureId);
                plantGrid.remove(victim);
                if (idx!=(int)plants.size()-1) std::swap(plants[idx],plants.back());
                plants.pop_back(); plantPool.destroy(victim); ++killed;
            }
        } else if (type == 2) { // carne
            for (int k=0; k<amount && !carcasses.empty(); ++k) {
                int idx=ri(0,(int)carcasses.size()-1);
                Carcass* victim=carcasses[idx];
                carcassGrid.remove(victim);
                if (idx!=(int)carcasses.size()-1) std::swap(carcasses[idx],carcasses.back());
                carcasses.pop_back(); carcassPool.destroy(victim); ++killed;
            }
        } else if (type == 3) { // ovos
            for (int k=0; k<amount && !eggs.empty(); ++k) {
                int idx=ri(0,(int)eggs.size()-1);
                Egg* victim=eggs[idx];
                eggGrid.remove(victim);
                if (idx!=(int)eggs.size()-1) std::swap(eggs[idx],eggs.back());
                eggs.pop_back(); eggPool.destroy(victim); ++killed;
            }
        }
        ensureMinimums();
        return killed;
    }

    Vector2 minimumFounderSpawnPosition() const {
        // O minimo e uma semente artificial do experimento. Em um mapa 2000x2000 com
        // poucas plantas, sortear fundadores totalmente ao acaso criava uma loteria de fome
        // antes mesmo da maturidade sexual. Preferimos iniciar perto de alimento existente,
        // sem teleportar comida nem alterar descendentes naturais.
        if(!plants.empty()){
            for(int attempt=0;attempt<24;++attempt){
                Plant* p=plants[(std::size_t)ri(0,(int)plants.size()-1)];
                if(!p || p->dead()) continue;
                const float a=rf(0.0f,2.0f*PI);
                const float d=rf(cfg::MINIMUM_FOUNDER_NEAR_FOOD_MIN,cfg::MINIMUM_FOUNDER_NEAR_FOOD_MAX);
                Vector2 out{p->pos.x+std::cos(a)*d,p->pos.y+std::sin(a)*d};
                out.x=clampf(out.x,2.0f,std::max(2.0f,cfg::WORLD_W-2.0f));
                out.y=clampf(out.y,2.0f,std::max(2.0f,cfg::WORLD_H-2.0f));
                return out;
            }
        }
        return {rf(0,cfg::WORLD_W),rf(0,cfg::WORLD_H)};
    }

    void ensureMinimums() {
        // Mortes abaixo do minimo criam uma divida de linhagem: essas vagas devem
        // ser preenchidas por ovos mutados dos mortos. Aumentar o minimo manualmente
        // continua criando fundadores novos em vez de clonar a ultima linhagem morta.
        int pending = pendingMinimumReplacementEggs();
        const int liveDeficit = std::max(0, limits.minCreatures - (int)creatures.size());
        const int lineageSlotsWanted = std::min(minimumLineageDebt, liveDeficit);
        int guard = 0;
        while (pending < lineageSlotsWanted &&
               (!limits.maxEnabled || futureCreaturePopulation() < limits.maxCreatures) &&
               guard++ < std::max(8,limits.minCreatures*2)) {
            if (!minimumLineageSeed.valid || !spawnMinimumReplacementEgg(minimumLineageSeed)) break;
            ++pending;
        }

        // Garante primeiro a base vegetal. Isso permite que fundadores artificiais do
        // minimo sejam semeados em regioes habitaveis em vez de no vazio do mapa.
        int minimumPlacementGuard = 0;
        while ((int)plants.size() < limits.minPlants && (!limits.maxEnabled || (int)plants.size() < limits.maxPlants) && minimumPlacementGuard++ < 200) {
            PlantGenes genes=randomPlantGenes();
            const float adultSize=cfg::tuning.plantMaxSize*genes.sizeScale;
            Vector2 p{rf(0, cfg::WORLD_W), rf(0, cfg::WORLD_H)};
            bool placed = plants.empty();
            for (int attempt = 0; attempt < 36 && !placed; ++attempt) {
                p = {rf(0, cfg::WORLD_W), rf(0, cfg::WORLD_H)};
                placed = plantPositionFree(p,adultSize);
            }
            if (!placed) break;
            addPlant(p,genes);
        }

        // Vagas adicionais, por exemplo ao aumentar o slider de minimo, sao fundadores.
        // Eles recebem uma reserva inicial maior apenas porque sao criados artificialmente
        // pelo piso populacional; descendentes naturais continuam obedecendo integralmente
        // aos recursos do ovo.
        guard = 0;
        const float founderEnergyBase=std::max(cfg::tuning.energyInitialBase,
            cfg::tuning.energyMaxBase*cfg::MINIMUM_FOUNDER_ENERGY_RATIO);
        while ((int)creatures.size() + pending < limits.minCreatures &&
               (!limits.maxEnabled || futureCreaturePopulation() < limits.maxCreatures) &&
               guard++ < std::max(8,limits.minCreatures*2)) {
            if (!addCreature(minimumFounderSpawnPosition(),randomGenome(),1,-1,founderEnergyBase)) break;
        }
    }

    bool rootedPlantCoversCreature(const Creature& c, const Plant& p) const {
        if (p.dead()) return false;
        const float bodyR = c.size * 0.5f;
        const float coverR = p.radius() + bodyR * cfg::tuning.plantCoverBodyRadiusWeight;
        return toroidalDist2(c.pos,p.pos) <= coverR*coverR;
    }

    void updateRootedPlantCover(Creature& c) const {
        c.rootedPlantCover = 0.0f;
        c.rootedCoverPlantPos={};
        c.rootedCoverPlantRadius=0.0f;
        const float bodyR = c.size * 0.5f;
        const float maxPlantR = std::max(cfg::tuning.plantRadiusMin,
            cfg::tuning.plantMaxSize * cfg::tuning.plantSizeScaleMax * cfg::tuning.plantRadiusScale);
        const float coverBroad = maxPlantR + bodyR * cfg::tuning.plantCoverBodyRadiusWeight;
        float bestMargin=-1e30f;
        plantGrid.forCircle(c.pos.x,c.pos.y,coverBroad,[&](Plant* p){
            if(!p || p->dead()) return;
            const float pr=p->radius();
            const float coverR=pr+bodyR*cfg::tuning.plantCoverBodyRadiusWeight;
            const float d2=toroidalDist2(c.pos,p->pos);
            if(d2>coverR*coverR) return;
            c.rootedPlantCover=1.0f;
            // A copa com maior margem de cobertura representa o esconderijo dominante.
            const float margin=coverR-std::sqrt(std::max(0.0f,d2));
            if(margin>bestMargin){bestMargin=margin;c.rootedCoverPlantPos=p->pos;c.rootedCoverPlantRadius=pr;}
        });
    }

    bool targetHiddenByRootedPlant(const Creature& observer, const Creature& target, Vector2 rayDir) const {
        if(cfg::tuning.rootedPlantHideOccupants <= 0.5f) return false;
        // A fase de cobertura roda para TODOS os bichos antes da visao. Se o alvo
        // nao esta sob copa alguma, evita uma segunda consulta espacial por hit visual.
        if(target.rootedPlantCover <= 0.5f) return false;
        const float targetR = target.size * 0.5f;
        const float maxPlantR = std::max(cfg::tuning.plantRadiusMin,
            cfg::tuning.plantMaxSize * cfg::tuning.plantSizeScaleMax * cfg::tuning.plantRadiusScale);
        const float broad = maxPlantR + targetR * cfg::tuning.plantCoverBodyRadiusWeight;
        bool hidden=false;
        const Vector2 targetImage=nearestToroidalImage(observer.pos,target.pos);
        const float centerAlongRay = dot(sub(targetImage,observer.pos),rayDir);
        if(centerAlongRay <= 0.0f) return false;
        plantGrid.forCircle(target.pos.x,target.pos.y,broad,[&](Plant* p){
            if(hidden || !p || !rootedPlantCoversCreature(target,*p)) return;
            // Se os dois estao sob a mesma copa, ela nao os separa visualmente.
            if(cfg::tuning.rootedPlantInsideVisionPass > 0.5f && rootedPlantCoversCreature(observer,*p)) return;
            const Vector2 plantImage=nearestToroidalImage(observer.pos,p->visualCenter());
            const float hit = rayTrefoil(observer.pos,rayDir,plantImage,p->visualRadius(),centerAlongRay);
            if(hit >= 0.0f && hit < centerAlongRay) hidden=true;
        });
        return hidden;
    }



    static std::uint32_t packVisionColor(Color c){
        return (std::uint32_t)c.r | ((std::uint32_t)c.g<<8) | ((std::uint32_t)c.b<<16) | ((std::uint32_t)c.a<<24);
    }
    static Color unpackVisionColor(std::uint32_t c){
        return Color{(unsigned char)(c&255u),(unsigned char)((c>>8)&255u),(unsigned char)((c>>16)&255u),(unsigned char)((c>>24)&255u)};
    }

    bool applyGpuVisionReadback(){
        gpuVisionResults.clear();
        if(!gpuVision.collect(gpuVisionResults)) return false;
        for(Creature* c:creatures){
            if(!c) continue;
            c->perfVisionRays=0;c->perfVisionCells=0;c->perfVisionPlantChecks=0;c->perfVisionCreatureChecks=0;c->perfVisionCarcassChecks=0;c->perfVisionEggChecks=0;
        }
        for(const auto& z:gpuVisionResults){
            Creature* observed=creatureById(z.observerId);if(!observed)continue;Creature& c=*observed;
            if(z.raySlot>=c.rays.size())continue;
            RayReading& rr=c.rays[z.raySlot];rr.distance=z.distance;rr.normalizedDistance=clampf(z.distance/std::max(0.0001f,z.rangeUsed),0.0f,1.0f);
            rr.type=(z.type<=4u)?(RayType)z.type:RayType::None;rr.targetColor=unpackVisionColor(z.color);rr.plantDetached=0.0f;rr.plantSway=0.0f;
            rr.hasTargetWorldPos=rr.type!=RayType::None;rr.targetWorldPos={z.targetX,z.targetY};
            if(rr.type==RayType::Plant||rr.type==RayType::Carcass)rr.colorAffinity=colorSimilarity(c.genome.mouth.color,rr.targetColor);
            else if(rr.type==RayType::Creature)rr.colorAffinity=colorSimilarity(c.displayColor,rr.targetColor);else rr.colorAffinity=0.0f;
            ++c.perfVisionRays;c.perfVisionCells+=z.cells;c.perfVisionPlantChecks+=z.plantChecks;c.perfVisionCreatureChecks+=z.creatureChecks;c.perfVisionCarcassChecks+=z.carcassChecks;c.perfVisionEggChecks+=z.eggChecks;
        }
        return true;
    }

    bool dispatchGpuVision(){
        const int cols=std::max(1,(int)std::ceil(cfg::WORLD_W/cfg::CELL));
        const int rows=std::max(1,(int)std::ceil(cfg::WORLD_H/cfg::CELL));
        const std::size_t cellN=(std::size_t)cols*(std::size_t)rows;
        gpuVisionCells.resize(cellN); std::fill(gpuVisionCells.begin(),gpuVisionCells.end(),GpuVisionCellRange{});
        gpuVisionCellCounts.resize(cellN); std::fill(gpuVisionCellCounts.begin(),gpuVisionCellCounts.end(),0u);
        gpuVisionCellCursor.resize(cellN); std::fill(gpuVisionCellCursor.begin(),gpuVisionCellCursor.end(),0u);

        auto wrapIndex=[](int v,int n){int m=v%n;return m<0?m+n:m;};
        // Um objeto que cruza a costura precisa existir tambem nos buckets da borda
        // oposta. Iteramos coordenadas de celula sem clamp e embrulhamos apenas a chave.
        auto forObjectCells=[&](Vector2 pos,float radius,auto&& fn){
            pos=wrappedPoint(pos);
            const int sx=(int)std::floor((pos.x-radius)/cfg::CELL);
            const int sy=(int)std::floor((pos.y-radius)/cfg::CELL);
            const int ex=(int)std::floor((pos.x+radius)/cfg::CELL);
            const int ey=(int)std::floor((pos.y+radius)/cfg::CELL);
            const int nx=std::min(cols,std::max(0,ex-sx+1));
            const int ny=std::min(rows,std::max(0,ey-sy+1));
            for(int iy=0;iy<ny;++iy){
                const int wy=wrapIndex(sy+iy,rows);
                for(int ix=0;ix<nx;++ix){
                    const int wx=wrapIndex(sx+ix,cols);
                    fn((std::size_t)wy*(std::size_t)cols+(std::size_t)wx);
                }
            }
        };
        auto countObj=[&](Vector2 pos,float radius){forObjectCells(pos,radius,[&](std::size_t key){++gpuVisionCellCounts[key];});};
        for(Plant* p:plants)if(p&&!p->dead())countObj(p->visualCenter(),p->visualRadius());
        for(Carcass* k:carcasses)if(k)countObj(k->pos,k->sensorRadius);
        for(Egg* e:eggs)if(e&&!e->destroyed())countObj(e->pos,e->radius);
        for(Creature* c:creatures)if(c)countObj(c->pos,c->size*0.5f);

        std::size_t total=0;for(std::size_t i=0;i<cellN;++i){gpuVisionCells[i].start=(std::uint32_t)total;gpuVisionCells[i].count=gpuVisionCellCounts[i];gpuVisionCellCursor[i]=(std::uint32_t)total;total+=gpuVisionCellCounts[i];}
        gpuVisionObjects.resize(total);
        auto fillObj=[&](const GpuVisionObject& o){
            forObjectCells({o.px,o.py},o.radius,[&](std::size_t key){gpuVisionObjects[gpuVisionCellCursor[key]++]=o;});
        };
        for(Plant* p:plants)if(p&&!p->dead()){const Vector2 vc=p->visualCenter();GpuVisionObject o;o.px=vc.x;o.py=vc.y;o.radius=p->visualRadius();o.type=1;o.color=packVisionColor(p->genes.color);fillObj(o);}
        for(Carcass* k:carcasses)if(k){GpuVisionObject o;o.px=k->pos.x;o.py=k->pos.y;o.radius=k->sensorRadius;o.type=3;o.color=packVisionColor(cfg::CARCASS_COLOR);fillObj(o);}
        for(Egg* e:eggs)if(e&&!e->destroyed()){GpuVisionObject o;o.px=e->pos.x;o.py=e->pos.y;o.radius=e->radius;o.type=4;o.color=packVisionColor(Color{238,218,150,255});fillObj(o);}
        for(Creature* c:creatures)if(c){GpuVisionObject o;o.px=c->pos.x;o.py=c->pos.y;o.radius=c->size*0.5f;o.type=2;o.color=packVisionColor(c->displayColor);o.creatureId=c->id;
            if(cfg::tuning.rootedPlantHideOccupants>0.5f&&c->rootedPlantCover>0.5f){o.coverRadius=c->rootedCoverPlantRadius;Vector2 cd=toroidalDelta(c->pos,c->rootedCoverPlantPos);o.coverX=c->pos.x+cd.x;o.coverY=c->pos.y+cd.y;}fillObj(o);}

        std::size_t rayTotal=0;for(Creature* c:creatures)if(c)rayTotal+=c->rays.size();gpuVisionRays.clear();gpuVisionRays.reserve(rayTotal);
        for(Creature* c:creatures){if(!c)continue;for(std::size_t r=0;r<c->rays.size();++r){
            const float sinO=c->raySin[r],cosO=c->rayCos[r];Vector2 dir{c->sinA*cosO+c->cosA*sinO,-c->cosA*cosO+c->sinA*sinO};
            GpuVisionRay q;q.ox=c->pos.x;q.oy=c->pos.y;q.dx=dir.x;q.dy=dir.y;q.range=c->genome.vision.range;q.observerRadius=c->size*0.5f;q.coverWeight=cfg::tuning.plantCoverBodyRadiusWeight;q.insidePass=cfg::tuning.rootedPlantInsideVisionPass;q.observerId=c->id;q.raySlot=(std::uint32_t)r;gpuVisionRays.push_back(q);
        }}
        return gpuVision.dispatch(gpuVisionCells,gpuVisionObjects,gpuVisionRays,cols,rows,cfg::WORLD_W,cfg::WORLD_H);
    }

    bool senseVisionGpu(){
        if(!gpuVision.ensureInitialized())return false;
        perf.gpuVisionAppliedLastPass=applyGpuVisionReadback();
        const std::uint64_t before=gpuVision.dispatches;
        if(!dispatchGpuVision())return false;
        perf.gpuVisionSubmittedLastPass=gpuVision.dispatches>before;
        perf.gpuVisionActive=true;
        perf.gpuVisionDispatches=gpuVision.dispatches;
        perf.gpuVisionReadbacks=gpuVision.readbacks;
        perf.gpuVisionSkippedDispatches=gpuVision.skippedDispatches;
        perf.gpuVisionObjectRefs=gpuVision.lastObjectRefs;
        perf.gpuVisionPendingBatches=gpuVision.pendingBatches();
        return true;
    }

    float rayWorldBoundaryDistance(Vector2 origin, Vector2 dir) const {
        (void)origin;(void)dir;
        return std::numeric_limits<float>::infinity();
    }

    void senseVision(Creature& c) {
        c.perfVisionRays = (std::uint32_t)std::min<std::size_t>(c.rays.size(),std::numeric_limits<std::uint32_t>::max());
        c.perfVisionCells = c.perfVisionPlantChecks = c.perfVisionCreatureChecks = 0;
        c.perfVisionCarcassChecks = c.perfVisionEggChecks = 0;
        for (size_t r = 0; r < c.rays.size(); ++r) {
            float sinO = c.raySin[r], cosO = c.rayCos[r];
            Vector2 dir{c.sinA * cosO + c.cosA * sinO, -c.cosA * cosO + c.sinA * sinO};
            float best = c.genome.vision.range;
            RayType type = RayType::None;
            Color targetColor{0,0,0,255};
            float plantDetached = 0.0f;
            float plantSway = 0.0f;
            Vector2 targetWorldPos{};bool hasTargetWorldPos=false;
            // Mundo toroidal: o raio nao encontra parede. Ao cruzar uma borda,
            // RayCellCollector continua nos tiles periodicos seguintes.
            thread_local RayCellCollector localRayCells;
            const auto& entries = localRayCells.collect(c.pos, dir, c.genome.vision.range);
            for (const auto& entry : entries) {
                if (entry.minDistance > best) break;
                ++c.perfVisionCells;
                auto imageInEntry=[&](Vector2 center){
                    center=wrappedPoint(center);
                    center.x += (float)entry.tileX*cfg::WORLD_W;
                    center.y += (float)entry.tileY*cfg::WORLD_H;
                    return center;
                };
                plantGrid.forCellKey(entry.key, [&](Plant* p){
                    ++c.perfVisionPlantChecks;
                    if(!p || p->dead()) return;
                    // Regra assimetrica de copa: a planta enraizada que cobre o proprio
                    // observador fica transparente somente para ele. Para quem esta fora,
                    // ela continua sendo um obstaculo visual normal. Plantas soltas nunca
                    // recebem essa excecao.
                    if(c.rootedPlantCover > 0.5f && cfg::tuning.rootedPlantInsideVisionPass > 0.5f && rootedPlantCoversCreature(c,*p)) return;
                    const float hit=rayTrefoil(c.pos,dir,imageInEntry(p->visualCenter()),p->visualRadius(),best);
                    if(hit>=0.0f&&hit<best){
                        best=hit;type=RayType::Plant;targetColor=p->genes.color;plantDetached=0.0f;
                        plantSway=0.0f;targetWorldPos=imageInEntry(p->visualCenter());hasTargetWorldPos=true;
                    }
                });
                carcassGrid.forCellKey(entry.key, [&](Carcass* k){
                    ++c.perfVisionCarcassChecks;
                    const float hit=rayCircle(c.pos,dir,imageInEntry(k->pos),k->sensorRadius,best);
                    if(hit>=0.0f&&hit<best){best=hit;type=RayType::Carcass;targetColor=cfg::CARCASS_COLOR;plantDetached=0.0f;plantSway=0.0f;targetWorldPos=imageInEntry(k->pos);hasTargetWorldPos=true;}
                });
                eggGrid.forCellKey(entry.key, [&](Egg* egg){
                    ++c.perfVisionEggChecks;
                    if(!egg||egg->destroyed())return;
                    const float hit=rayCircle(c.pos,dir,imageInEntry(egg->pos),egg->radius,best);
                    if(hit>=0.0f&&hit<best){best=hit;type=RayType::Egg;targetColor=Color{238,218,150,255};plantDetached=0.0f;plantSway=0.0f;targetWorldPos=imageInEntry(egg->pos);hasTargetWorldPos=true;}
                });
                creatureGrid.forCellKey(entry.key, [&](Creature* other){
                    ++c.perfVisionCreatureChecks;
                    if(other==&c)return;
                    const float hit=rayCircle(c.pos,dir,imageInEntry(other->pos),other->size*0.5f,best);
                    if(hit<0.0f || hit>=best) return;
                    // Garante a semantica de esconderijo: se o alvo esta sob uma copa
                    // enraizada e o observador esta fora dela, a copa oculta o alvo inteiro,
                    // mesmo quando um corpo grande ultrapassa um pouco a borda visual.
                    if(targetHiddenByRootedPlant(c,*other,dir)) return;
                    best=hit;type=RayType::Creature;targetColor=other->displayColor;plantDetached=0.0f;plantSway=0.0f;targetWorldPos=imageInEntry(other->pos);hasTargetWorldPos=true;
                });
            }
            c.rays[r].distance = best;
            c.rays[r].normalizedDistance = clampf(best / c.genome.vision.range, 0.0f, 1.0f);
            c.rays[r].type = type;
            c.rays[r].targetColor = targetColor;
            c.rays[r].plantDetached = type == RayType::Plant ? plantDetached : 0.0f;
            c.rays[r].plantSway = (type == RayType::Plant && plantDetached<0.5f) ? plantSway : 0.0f;
            c.rays[r].targetWorldPos=targetWorldPos;c.rays[r].hasTargetWorldPos=hasTargetWorldPos;
            if(type==RayType::Plant || type==RayType::Carcass)
                c.rays[r].colorAffinity=colorSimilarity(c.genome.mouth.color,targetColor);
            else if(type==RayType::Creature)
                c.rays[r].colorAffinity=colorSimilarity(c.displayColor,targetColor);
            else
                c.rays[r].colorAffinity=0.0f;
        }

    }

    void packSensorInputs(Creature& c) {
        int idx = 0;
        for (int slot = 0; slot < cfg::MAX_RAYS; ++slot) {
            if (slot < (int)c.rays.size()) {
                const RayReading& rr = c.rays[slot];
                c.inputs[idx++] = rr.normalizedDistance;
                c.inputs[idx++] = rr.type == RayType::Plant ? 1.0f : 0.0f;
                c.inputs[idx++] = rr.type == RayType::Creature ? 1.0f : 0.0f;
                c.inputs[idx++] = rr.type == RayType::Carcass ? 1.0f : 0.0f;
                c.inputs[idx++] = rr.type == RayType::Egg ? 1.0f : 0.0f;
                c.inputs[idx++] = rr.plantDetached;
                c.inputs[idx++] = clampf(float(rr.targetColor.r) / 255.0f * c.genome.vision.sensitivityR,0.0f,1.0f);
                c.inputs[idx++] = clampf(float(rr.targetColor.g) / 255.0f * c.genome.vision.sensitivityG,0.0f,1.0f);
                c.inputs[idx++] = clampf(float(rr.targetColor.b) / 255.0f * c.genome.vision.sensitivityB,0.0f,1.0f);
                c.inputs[idx++] = rr.colorAffinity;
                c.inputs[idx++] = rr.plantSway;
            } else {
                c.inputs[idx++] = 1.0f;
                for(int z=0;z<10;++z) c.inputs[idx++] = 0.0f;
            }
        }
        // rootedPlantCover ja foi calculado na fase anterior de cobertura, pois
        // ele tambem participa da regra de oclusao dos proprios raios.

        c.inputs[idx++] = c.energy / c.maxEnergy;
        c.inputs[idx++] = c.health / cfg::tuning.healthMax;
        c.inputs[idx++] = clampf(c.speed >= 0 ? c.speed / std::max(0.01f,c.maxForwardEffective) : c.speed / std::max(0.01f,c.maxReverseEffective), -1.0f, 1.0f);
        c.inputs[idx++] = clampf(c.lateralSpeed / std::max(0.01f,c.maxLateralEffective), -1.0f, 1.0f);
        c.inputs[idx++] = c.canReproduce() ? 1.0f : 0.0f;
        // Feedback proprio: o cerebro sabe qual cor do CORPO esta emitindo agora.
        c.inputs[idx++] = float(c.displayColor.r) / 255.0f;
        c.inputs[idx++] = float(c.displayColor.g) / 255.0f;
        c.inputs[idx++] = float(c.displayColor.b) / 255.0f;
        c.inputs[idx++] = normalizeGene(c.genome.vision.range, cfg::tuning.visionMinRange, cfg::tuning.visionMaxRange);
        c.inputs[idx++] = normalizeGene((float)c.genome.vision.rayCount, (float)cfg::MIN_RAYS, (float)cfg::MAX_RAYS);
        c.inputs[idx++] = clampf(c.genome.mouth.biteForce / std::max(0.01f,cfg::tuning.mouthBiteGeneMax), 0.0f, 1.0f);
        c.inputs[idx++] = clampf(c.genome.mouth.maxOpening / std::max(0.01f,cfg::tuning.mouthOpeningGeneMax), 0.0f, 1.0f);
        c.inputs[idx++] = clampf(c.genome.mouth.movementSpeed / std::max(0.01f,cfg::tuning.mouthSpeedGeneMax), 0.0f, 1.0f);
        c.inputs[idx++] = c.genome.physical.hardness;
        c.inputs[idx++] = normalizeGene(c.size, cfg::tuning.minCreatureSize, cfg::tuning.maxCreatureSize);
        c.inputs[idx++] = c.rootedPlantCover;
        c.inputs[idx++] = c.developmentProgress;
        c.inputs[idx++] = c.sexuallyMature()?1.0f:0.0f;
        c.inputs[idx++] = c.senescenceProgress;
        c.inputs[idx++] = clampf(c.age/std::max(0.01f,c.genome.development.longevity),0.0f,1.0f);
        c.inputs[idx++] = clampf(c.angularVelocity/std::max(1.0f,c.rotEffective),-1.0f,1.0f);
        c.inputs[idx++] = clampf(c.recentImpact,0.0f,1.0f);
        c.inputs[idx++] = clampf(c.recentDamage,0.0f,1.0f);
        c.inputs[idx++] = clampf(c.recentEnergyGain,0.0f,1.0f);
        c.inputs[idx++] = clampf(std::sqrt(length2(c.pushVelocity))/std::max(0.01f,cfg::PHYSICS_MAX_PUSH_SPEED*cfg::tuning.maxPushSpeed),0.0f,1.0f);
        c.inputs[idx++] = clampf(c.contactPressure[0],0.0f,1.0f);
        c.inputs[idx++] = clampf(c.contactPressure[1],0.0f,1.0f);
        c.inputs[idx++] = clampf(c.contactPressure[2],0.0f,1.0f);
        c.inputs[idx++] = clampf(c.contactPressure[3],0.0f,1.0f);
        c.inputs[idx++] = c.grabbedKind!=GrabKind::None ? 1.0f : 0.0f;
        c.inputs[idx++] = clampf(c.grabLoadRatio/std::max(0.01f,cfg::tuning.grabMaxLoadMassRatio),0.0f,1.0f);
        // v0.135 - PERCEPCAO CONTINUA. Os raycasts continuam sendo a verdade bruta,
        // mas o cerebro pode evoluir sensores que condensam o objeto visivel mais proximo
        // em presenca/direcao/distancia. O objeto precisa ter sido atingido por um raio.
        struct Perceived { bool seen=false; float hitDistance=0.0f; Vector2 target{}; };
        Perceived pp,pc,pk,pe;
        auto take=[&](Perceived& dst,const RayReading& rr){
            if(!rr.hasTargetWorldPos)return;
            if(!dst.seen || rr.distance<dst.hitDistance){dst.seen=true;dst.hitDistance=rr.distance;dst.target=rr.targetWorldPos;}
        };
        for(const RayReading& rr:c.rays){
            if(rr.type==RayType::Plant)take(pp,rr);
            else if(rr.type==RayType::Creature)take(pc,rr);
            else if(rr.type==RayType::Carcass)take(pk,rr);
            else if(rr.type==RayType::Egg)take(pe,rr);
        }
        auto direction=[&](const Perceived& q){
            if(!q.seen)return 0.0f;
            Vector2 d=sub(q.target,c.pos);
            if(length2(d)<1e-8f)return 0.0f;
            const float side=dot(d,c.right()),front=dot(d,c.forward());
            return clampf(std::atan2(side,front)/PI,-1.0f,1.0f);
        };
        auto near01=[&](const Perceived& q){return q.seen?1.0f-clampf(q.hitDistance/std::max(0.01f,c.genome.vision.range),0.0f,1.0f):0.0f;};
        bool plantMouthContact=false;
        {
            const Vector2 bp=c.bitePoint();const float br=std::max(0.35f,c.biteRadius()*0.45f);
            const float maxPr=std::max(cfg::tuning.plantRadiusMin,cfg::tuning.plantMaxSize*cfg::tuning.plantSizeScaleMax*cfg::tuning.plantRadiusScale);
            plantGrid.forCircle(bp.x,bp.y,br+maxPr,[&](Plant* p){
                if(plantMouthContact||!p||p->dead()||p->biomassSize<=0.001f)return;
                const float req=br+p->radius();if(toroidalDist2(bp,p->pos)<=req*req)plantMouthContact=true;
            });
        }
        std::array<float,cfg::LEGACY_SENSOR_PADDING_INPUTS> percept{};
        percept[0]=pp.seen?1.0f:0.0f;percept[1]=direction(pp);percept[2]=near01(pp);percept[3]=plantMouthContact?1.0f:0.0f;
        percept[4]=pc.seen?1.0f:0.0f;percept[5]=direction(pc);percept[6]=near01(pc);
        percept[7]=pk.seen?1.0f:0.0f;percept[8]=direction(pk);percept[9]=near01(pk);
        percept[10]=pe.seen?1.0f:0.0f;percept[11]=direction(pe);percept[12]=near01(pe);
        for(float v:percept)c.inputs[idx++]=v;

        // Memoria episodica fica APOS as emocoes para preservar indices de saves antigos.
        // Cada slot: 5 tipos one-hot + intensidade + idade normalizada + direcao atual X/Y + valido.
        constexpr float MEMORY_TIME_HORIZON=60.0f; // segundos simulados; evento continua no buffer, idade satura em 1.
        int mi=cfg::MEMORY_INPUT_BASE;
        for(int recent=0;recent<cfg::LONG_MEMORY_SLOTS;++recent){
            LongMemoryEvent e{}; bool valid=recent<(int)c.longMemoryCount;
            if(valid){
                const int slot=(int(c.longMemoryHead)-1-recent+cfg::LONG_MEMORY_SLOTS)%cfg::LONG_MEMORY_SLOTS;
                e=c.longMemory[(std::size_t)slot]; valid=e.type!=MemoryEventType::None && e.time>=0.0f;
            }
            for(int type=1;type<=cfg::MEMORY_EVENT_TYPES;++type)
                c.inputs[mi++]=valid && (int)e.type==type ? 1.0f : 0.0f;
            c.inputs[mi++]=valid?clampf(e.intensity,0.0f,1.0f):0.0f;
            c.inputs[mi++]=valid?clampf((simTime-e.time)/MEMORY_TIME_HORIZON,0.0f,1.0f):1.0f;
            c.inputs[mi++]=valid?clampf(dot(e.worldDir,c.right()),-1.0f,1.0f):0.0f;
            c.inputs[mi++]=valid?clampf(dot(e.worldDir,c.forward()),-1.0f,1.0f):0.0f;
            c.inputs[mi++]=valid?1.0f:0.0f;
        }

        // Registradores persistentes: slots acima da capacidade genetica existem no
        // layout apenas para estabilidade dos indices, mas leem zero e nao podem ser escritos.
        const int regCap=brainRegisterCapacity(c.genome.brain);
        for(int slot=0;slot<cfg::REGISTER_MEMORY_SLOTS;++slot)
            c.inputs[cfg::REGISTER_MEMORY_INPUT_BASE+slot]=slot<regCap?clampf(c.memoryRegisters[slot],0.0f,1.0f):0.0f;
    }

    float currentSynapticWeight(const Creature& c,const CompiledConnection& cn) const {
        return cn.geneIndex>=0 && cn.geneIndex<(int)c.learnedWeights.size()?c.learnedWeights[(std::size_t)cn.geneIndex]:cn.weight;
    }

    void updateLifetimePlasticity(Creature& c){
        if(c.compiled.plasticConnections<=0 || c.genome.brain.connections.empty())return;
        if(c.learnedWeights.size()!=c.genome.brain.connections.size()){
            c.learnedWeights.resize(c.genome.brain.connections.size());c.eligibilityTraces.assign(c.genome.brain.connections.size(),0.0f);
            for(std::size_t i=0;i<c.genome.brain.connections.size();++i)c.learnedWeights[i]=c.genome.brain.connections[i].weight;
        }
        if(c.eligibilityTraces.size()!=c.genome.brain.connections.size())c.eligibilityTraces.assign(c.genome.brain.connections.size(),0.0f);
        const float tick=std::max(0.001f,cfg::tuning.brainInterval);
        const float eligDecay=std::exp(-tick/std::max(0.05f,cfg::tuning.brainPlasticEligibilitySeconds));

        float rawReward = 0.0f;
        if(cfg::tuning.reinforcementEnabled>0.5f){
            rawReward += c.reinforcementPending;
            rawReward += cfg::tuning.reinforcementSurvivalPerSecond*tick;
            const float energyRatio=c.maxEnergy>0.0001f?c.energy/c.maxEnergy:0.0f;
            if(energyRatio<cfg::tuning.starvationThreshold)
                rawReward += cfg::tuning.reinforcementStarvingPerSecond*tick;
        }
        c.reinforcementPending=0.0f;
        const float reward=clampf(rawReward,-cfg::tuning.reinforcementRewardClip,cfg::tuning.reinforcementRewardClip);
        c.lastPlasticReward=reward;
        c.reinforcementTotal += reward;
        if(reward>0.0f)c.reinforcementPositive += reward;
        else c.reinforcementNegative += -reward;
        for(std::size_t gi=0;gi<c.genome.brain.connections.size();++gi){
            const auto& gene=c.genome.brain.connections[gi];
            if(!neuralConnectionEnabled(gene)||!neuralConnectionPlastic(gene)||gene.dstKind==BrainNodeKind::Input)continue;
            if(!validEndpoint(c.genome.brain,gene.srcKind,gene.src)||!validEndpoint(c.genome.brain,gene.dstKind,gene.dst))continue;
            if(gene.srcKind==BrainNodeKind::Input&&!brainInputActive(c.genome.brain,gene.src))continue;
            if(gene.srcKind==BrainNodeKind::Output&&!brainOutputActive(c.genome.brain,gene.src))continue;
            if(gene.dstKind==BrainNodeKind::Output&&!brainOutputActive(c.genome.brain,gene.dst))continue;
            const bool recurrent=connectionIsRecurrent(c.genome.brain,gene);
            int srcIndex=0;
            if(gene.srcKind==BrainNodeKind::Input)srcIndex=cfg::BRAIN_INPUT_BASE+gene.src;
            else if(gene.srcKind==BrainNodeKind::Hidden)srcIndex=(recurrent?cfg::BRAIN_PREV_HIDDEN_BASE:cfg::BRAIN_HIDDEN_BASE)+gene.src;
            else srcIndex=cfg::BRAIN_PREV_OUTPUT_BASE+gene.src;
            const float pre=clampf(applyBrainLinkFunction(gene.function,c.brainState[srcIndex],gene.paramA,gene.paramB),-1.0f,1.0f);
            float post=0.0f;
            if(gene.dstKind==BrainNodeKind::Hidden)post=clampf(c.hidden[(std::size_t)gene.dst],-1.0f,1.0f);
            else post=clampf(c.netOut[(std::size_t)gene.dst]*2.0f-1.0f,-1.0f,1.0f);
            float& elig=c.eligibilityTraces[gi];
            // Primeiro recompensa o passado; so depois registra a atividade DESTE tick.
            // Isso evita dar credito causal para uma acao que ocorreu depois da comida/dano.
            elig=clampf(elig*eligDecay,-1.0f,1.0f);
            float& learned=c.learnedWeights[gi];
            if(std::fabs(reward)>1e-4f){
                const float lr=cfg::tuning.brainPlasticLearningRate*neuralConnectionPlasticFactor(gene);
                learned+=lr*reward*elig;
                ++c.plasticUpdates;
            }
            if(cfg::tuning.brainPlasticReturnPerSecond>0.0f)
                learned=moveToward(learned,gene.weight,cfg::tuning.brainPlasticReturnPerSecond*tick);
            const float range=cfg::tuning.brainPlasticWeightRange*(0.35f+0.65f*neuralConnectionPlasticFactor(gene));
            learned=clampf(learned,std::max(-cfg::tuning.brainWeightLimit,gene.weight-range),std::min(cfg::tuning.brainWeightLimit,gene.weight+range));
            elig=clampf(elig+(1.0f-eligDecay)*(pre*post),-1.0f,1.0f);
        }
    }

    void evaluateBrain(Creature& c) {
        // Copia somente os estados necessarios para a memoria do tick anterior.
        // A topologia ja foi traduzida para indices globais em compileBrain().
        for (int i = 0; i < cfg::INPUT_COUNT; ++i)
            c.brainState[cfg::BRAIN_INPUT_BASE + i] = brainInputActive(c.genome.brain,i)?c.inputs[i]:0.0f;
        for (int h = 0; h < c.genome.brain.hiddenCount; ++h)
            c.brainState[cfg::BRAIN_PREV_HIDDEN_BASE + h] = c.hidden[h];
        for (int o = 0; o < cfg::OUTPUT_COUNT; ++o)
            c.brainState[cfg::BRAIN_PREV_OUTPUT_BASE + o] = c.netOut[o];

        auto linkSignal=[&](const CompiledConnection& cn){
            return applyBrainLinkFunction(cn.function,c.brainState[cn.sourceIndex],cn.paramA,cn.paramB)*currentSynapticWeight(c,cn);
        };

        // Loop critico agora e MAC puro: leitura indexada + multiplicacao + soma.
        // Ligacoes feedforward leem BRAIN_HIDDEN_BASE, que e atualizado conforme
        // cada camada e processada; loops leem BRAIN_PREV_HIDDEN_BASE.
        for (int layer = 0; layer < cfg::MAX_BRAIN_LAYERS; ++layer) {
            for (int h : c.compiled.hiddenByLayer[layer]) {
                const auto& node = c.genome.brain.hiddenNodes[h];
                float sum = node.bias;
                for (const auto& cn : c.compiled.hiddenIncoming[h]) sum += linkSignal(cn);
                float gateSum=0.0f;for(const auto& cn:c.compiled.hiddenGateIncoming[h])gateSum+=linkSignal(cn);
                if(!c.compiled.hiddenGateIncoming[h].empty())sum*=2.0f*sigmoid(gateSum);
                float modSum=0.0f;for(const auto& cn:c.compiled.hiddenModIncoming[h])modSum+=linkSignal(cn);
                float dynGain=node.gain;if(!c.compiled.hiddenModIncoming[h].empty())dynGain*=0.35f+1.30f*sigmoid(modSum);
                float shunt=0.0f;for(const auto& cn:c.compiled.hiddenShuntIncoming[h])shunt+=std::fabs(linkSignal(cn));
                if(shunt>0.0f)sum/=1.0f+shunt;
                const float candidate = applyHiddenActivation(hiddenNodeActivation(node),sum * dynGain);
                const float mem = clampf(node.memory, 0.0f, 0.96f);
                const float previous = c.brainState[cfg::BRAIN_PREV_HIDDEN_BASE + h];
                const float state = previous * mem + candidate * (1.0f - mem);
                c.hidden[h] = state;
                c.brainState[cfg::BRAIN_HIDDEN_BASE + h] = state;
            }
        }

        for (int o = 0; o < cfg::OUTPUT_COUNT; ++o) {
            if(!brainOutputActive(c.genome.brain,o)){c.netOut[o]=0.0f;continue;}
            float sum = c.genome.brain.outputBiases[o];
            for (const auto& cn : c.compiled.outputIncoming[o]) sum += linkSignal(cn);
            float gateSum=0.0f;for(const auto& cn:c.compiled.outputGateIncoming[o])gateSum+=linkSignal(cn);
            if(!c.compiled.outputGateIncoming[o].empty())sum*=2.0f*sigmoid(gateSum);
            float modSum=0.0f;for(const auto& cn:c.compiled.outputModIncoming[o])modSum+=linkSignal(cn);
            if(!c.compiled.outputModIncoming[o].empty())sum*=0.35f+1.30f*sigmoid(modSum);
            float shunt=0.0f;for(const auto& cn:c.compiled.outputShuntIncoming[o])shunt+=std::fabs(linkSignal(cn));
            if(shunt>0.0f)sum/=1.0f+shunt;
            c.netOut[o] = sigmoid(sum);
        }
        c.brainOut.fill(0.0f);
        float drive = c.netOut[0] - c.netOut[1];
        float turn = c.netOut[3] - c.netOut[2];
        float strafe = c.netOut[5] - c.netOut[4];
        c.brainOut[0] = std::max(0.0f, drive);
        c.brainOut[1] = std::max(0.0f, -drive);
        c.brainOut[2] = std::max(0.0f, -turn);
        c.brainOut[3] = std::max(0.0f, turn);
        c.brainOut[4] = std::max(0.0f, -strafe);
        c.brainOut[5] = std::max(0.0f, strafe);
        for (int o=0;o<cfg::MOTOR_OUTPUT_COUNT;++o) if (c.brainOut[o] < 0.10f) c.brainOut[o] = 0.0f;
        c.brainOut[cfg::BITE_INTENT_OUTPUT_INDEX]=brainOutputActive(c.genome.brain,cfg::BITE_INTENT_OUTPUT_INDEX)?c.netOut[cfg::BITE_INTENT_OUTPUT_INDEX]:0.0f;
        c.brainOut[cfg::LEGACY_RESERVED_OUTPUT_INDEX]=0.0f;
        c.brainOut[cfg::MATE_INTENT_OUTPUT_INDEX]=brainOutputActive(c.genome.brain,cfg::MATE_INTENT_OUTPUT_INDEX)?c.netOut[cfg::MATE_INTENT_OUTPUT_INDEX]:0.0f;
        for (int k=0;k<cfg::COLOR_OUTPUT_COUNT;++k) c.brainOut[cfg::COLOR_OUTPUT_BASE+k]=brainOutputActive(c.genome.brain,cfg::COLOR_OUTPUT_BASE+k)?c.netOut[cfg::COLOR_OUTPUT_BASE+k]:0.0f;
        const int regCap=brainRegisterCapacity(c.genome.brain);
        for(int slot=0;slot<cfg::REGISTER_MEMORY_SLOTS;++slot){
            c.brainOut[cfg::MEMORY_WRITE_OUTPUT_BASE+slot]=brainOutputActive(c.genome.brain,cfg::MEMORY_WRITE_OUTPUT_BASE+slot)?c.netOut[cfg::MEMORY_WRITE_OUTPUT_BASE+slot]:0.0f;
            c.brainOut[cfg::MEMORY_GATE_OUTPUT_BASE+slot]=brainOutputActive(c.genome.brain,cfg::MEMORY_GATE_OUTPUT_BASE+slot)?c.netOut[cfg::MEMORY_GATE_OUTPUT_BASE+slot]:0.0f;
            if(slot<regCap && brainOutputActive(c.genome.brain,cfg::MEMORY_GATE_OUTPUT_BASE+slot) && c.netOut[cfg::MEMORY_GATE_OUTPUT_BASE+slot]>=cfg::tuning.brainRegisterWriteGateThreshold)
                c.memoryRegisters[slot]=clampf(c.netOut[cfg::MEMORY_WRITE_OUTPUT_BASE+slot],0.0f,1.0f);
        }
        // Plasticidade altera apenas o fenotipo sinaptico para o PROXIMO tick.
        updateLifetimePlasticity(c);
    }

    void updateSensors(float simulatedDt=0.0f,bool forceLegacyCompatibility=true) {
        (void)simulatedDt; (void)forceLegacyCompatibility;
        using Clock=std::chrono::high_resolution_clock;
        const auto ms=[](auto a,auto b){return std::chrono::duration<double,std::milli>(b-a).count();};
        const auto totalStart=Clock::now();

        auto a=Clock::now();
        brainExecutor.parallelFor(creatures.size(), [&](std::size_t i) { updateRootedPlantCover(*creatures[i]); });
        auto b=Clock::now();
        perf.sensorCover.sample(ms(a,b));

        a=Clock::now();
        perf.gpuVisionAppliedLastPass=false; perf.gpuVisionSubmittedLastPass=false;
        const bool gpuVisionOk=senseVisionGpu();
        if(!gpuVisionOk){
            // CPU e fallback apenas quando a GPU realmente nao esta disponivel ou falha.
            // No primeiro dispatch assincrono mantemos a leitura anterior/default por um tick,
            // evitando um pico caro de raycast CPU durante o aquecimento da GPU.
            perf.gpuVisionActive=false;
            brainExecutor.parallelFor(creatures.size(), [&](std::size_t i) { senseVision(*creatures[i]); });
        }
        b=Clock::now();
        perf.sensorVision.sample(ms(a,b));

        a=Clock::now();
        brainExecutor.parallelFor(creatures.size(), [&](std::size_t i) { packSensorInputs(*creatures[i]); });
        b=Clock::now();
        perf.sensorInputs.sample(ms(a,b));
        perf.sensing.sample(ms(totalStart,Clock::now()));
        perf.sensorsMs=perf.sensorCover.avgMs+perf.sensorVision.avgMs+perf.sensorInputs.avgMs;

        perf.sensorRays=perf.sensorCells=0;
        perf.sensorPlantChecks=perf.sensorCreatureChecks=0;
        perf.sensorCarcassChecks=perf.sensorEggChecks=0;
        for(const Creature* c:creatures){
            perf.sensorRays+=c->perfVisionRays;
            perf.sensorCells+=c->perfVisionCells;
            perf.sensorPlantChecks+=c->perfVisionPlantChecks;
            perf.sensorCreatureChecks+=c->perfVisionCreatureChecks;
            perf.sensorCarcassChecks+=c->perfVisionCarcassChecks;
            perf.sensorEggChecks+=c->perfVisionEggChecks;
        }
    }

    float emotionSourceValue(const Creature& c,const EmotionTriggerGene& t,const std::array<float,cfg::MAX_EMOTIONS>& previous) const {
        if(t.sourceKind==EmotionSourceKind::Input){
            return (t.source>=0&&t.source<cfg::BASE_INPUT_COUNT)?c.inputs[t.source]:0.0f;
        }
        if(t.sourceKind==EmotionSourceKind::Hidden){
            return (t.source>=0&&t.source<(int)c.hidden.size())?c.hidden[t.source]:0.0f;
        }
        if(t.sourceKind==EmotionSourceKind::Output){
            return (t.source>=0&&t.source<cfg::OUTPUT_COUNT)?c.netOut[t.source]:0.0f;
        }
        if(t.source>=0&&t.source<(int)c.genome.emotions.emotions.size()){
            const float maxS=std::max(0.01f,c.genome.emotions.emotions[t.source].strength);
            return clampf(previous[t.source]/maxS,0.0f,1.0f);
        }
        return 0.0f;
    }

    void updateEmotion(Creature& c,float dt){
        const auto previous=c.emotionState; // impede ordem artificial entre E0/E1/E2
        const int count=std::min<int>((int)c.genome.emotions.emotions.size(),cfg::MAX_EMOTIONS);
        for(int i=0;i<count;++i){
            const EmotionGene& e=c.genome.emotions.emotions[i];
            float sum=e.bias;
            for(const auto& t:e.triggers)sum+=emotionSourceValue(c,t,previous)*t.weight;
            const float drive=sigmoid(sum*e.gain);
            c.emotionDrive[i]=drive;
            float state=previous[i];
            if(drive>e.threshold){
                const float normalized=clampf((drive-e.threshold)/std::max(0.001f,1.0f-e.threshold),0.0f,1.0f);
                const float target=e.strength*normalized;
                const float riseSpeed=e.strength/std::max(0.01f,e.riseTime);
                state=moveToward(state,target,riseSpeed*dt);
            }else{
                const float fallSpeed=e.strength/std::max(0.01f,e.duration);
                state=std::max(0.0f,state-fallSpeed*dt);
            }
            c.emotionState[i]=clampf(state,0.0f,std::max(0.0f,e.strength));
            c.inputs[cfg::EMOTION_INPUT_BASE+i]=c.emotionState[i]*e.impact;
        }
        for(int i=count;i<cfg::MAX_EMOTIONS;++i){
            c.emotionState[i]=0.0f;c.emotionDrive[i]=0.0f;c.inputs[cfg::EMOTION_INPUT_BASE+i]=0.0f;
        }
    }

    void updateEmotions(float dt){
        brainExecutor.parallelFor(creatures.size(),[&](std::size_t i){updateEmotion(*creatures[i],dt);});
    }

    void updateBrains(float dt) {
        brainDueScratch.clear();
        for (Creature* c : creatures) {
            c->brainTimer += dt;
            while (c->brainTimer >= cfg::tuning.brainInterval) {
                c->brainTimer -= cfg::tuning.brainInterval;
                brainDueScratch.push_back(c);
                break; // um disparo por subpasso; MAX_SUBSTEP << BRAIN_INTERVAL
            }
        }
        perf.brainBatch=brainDueScratch.size();
        brainExecutor.parallelFor(brainDueScratch.size(),[&](std::size_t i){ evaluateBrain(*brainDueScratch[i]); });
    }

    void recordContinuous(Creature& c, float dt, float metabolism, float vision, float fwd, float rev, float turn, float lateral, float movement, float healthLoss, float healthGain) {
        if (!isWatching(c.id)) return;
        auto& a = watch.acc;
        a.elapsed += dt; a.metabolism += metabolism; a.vision += vision; a.forward += fwd; a.reverse += rev; a.turn += turn; a.lateral += lateral; a.movement += movement; a.healthLoss += healthLoss; a.healthGain += healthGain;
        a.fSig += c.brainOut[0] * dt; a.rSig += c.brainOut[1] * dt; a.lSig += c.brainOut[2] * dt; a.dSig += c.brainOut[3] * dt;
        a.leSig += c.brainOut[4] * dt; a.ldSig += c.brainOut[5] * dt;
        if (a.elapsed < 2.0f) return;
        float total = a.metabolism + a.vision + a.forward + a.reverse + a.turn + a.lateral + a.movement;
        std::string text = "Gastou " + f2(total) + " energia em " + f2(a.elapsed) + "s (met " + f2(a.metabolism);
        if (a.vision > 0.01f) text += ", visao " + f2(a.vision);
        if (a.forward > 0.01f) text += ", frente " + f2(a.forward);
        if (a.reverse > 0.01f) text += ", re " + f2(a.reverse);
        if (a.turn > 0.01f) text += ", giro " + f2(a.turn);
        if (a.lateral > 0.01f) text += ", lateral " + f2(a.lateral);
        if (a.movement > 0.01f) text += ", movimento " + f2(a.movement);
        text += "). Energia " + f2(c.energy) + "/" + f2(c.maxEnergy) + ".";
        log(c.id, text);
        if (a.healthLoss > 0.01f) log(c.id, "Perdeu " + f2(a.healthLoss) + " de vida por energia baixa. Vida " + f2(c.health) + ".");
        if (a.healthGain > 0.01f) log(c.id, "Regenerou " + f2(a.healthGain) + " de sangue por energia alta. Vida " + f2(c.health) + ".");
        watch.acc = {};
    }

    float grabbedTargetMass(const Creature& c) const {
        if(c.grabbedKind==GrabKind::Plant && c.grabbedPlant && !c.grabbedPlant->dead()) return c.grabbedPlant->physicalMass();
        if(c.grabbedKind==GrabKind::Carcass && c.grabbedCarcass && !c.grabbedCarcass->finished()) return c.grabbedCarcass->mass();
        if(c.grabbedKind==GrabKind::Creature && c.grabbedCreature && !c.grabbedCreature->dead()) return c.grabbedCreature->mass();
        return 0.0f;
    }

    float grabbedTargetRadius(const Creature& c) const {
        if(c.grabbedKind==GrabKind::Plant && c.grabbedPlant) return c.grabbedPlant->physicalRadius();
        if(c.grabbedKind==GrabKind::Carcass && c.grabbedCarcass) return c.grabbedCarcass->radius();
        if(c.grabbedKind==GrabKind::Creature && c.grabbedCreature) return c.grabbedCreature->radius();
        return 0.0f;
    }

    void releaseGrab(Creature& c){
        if(c.grabbedKind==GrabKind::Plant && c.grabbedPlant && c.grabbedPlant->grabbedByCreatureId==c.id){
            c.grabbedPlant->grabbedByCreatureId=-1;
            c.grabbedPlant->velocity={0.0f,0.0f};
        }
        if(c.grabbedKind==GrabKind::Carcass && c.grabbedCarcass && c.grabbedCarcass->grabbedByCreatureId==c.id) c.grabbedCarcass->grabbedByCreatureId=-1;
        if(c.grabbedKind==GrabKind::Creature && c.grabbedCreature && c.grabbedCreature->grabbedByCreatureId==c.id) c.grabbedCreature->grabbedByCreatureId=-1;
        c.grabbedKind=GrabKind::None;c.grabbedPlant=nullptr;c.grabbedCarcass=nullptr;c.grabbedCreature=nullptr;c.grabLoadRatio=0.0f;
    }

    void releaseCarrierById(int carrierId){
        if(carrierId<0)return;
        if(Creature* carrier=creatureById(carrierId)) releaseGrab(*carrier);
    }

    bool grabStillValid(Creature& c){
        if(c.grabbedKind==GrabKind::None) return false;
        if(c.grabbedByCreatureId>=0) return false; // evita correntes/ciclos de agarrar enquanto ja esta sendo levado
        const float m=grabbedTargetMass(c);
        if(m<=0.0f) return false;
        const float ratio=m/std::max(0.01f,c.mass());
        c.grabLoadRatio=ratio;
        if(ratio>cfg::tuning.grabMaxLoadMassRatio) return false;
        if(c.grabbedKind==GrabKind::Plant) return false;
        if(c.grabbedKind==GrabKind::Carcass) return c.grabbedCarcass && c.grabbedCarcass->grabbedByCreatureId==c.id;
        if(c.grabbedKind==GrabKind::Creature) return c.grabbedCreature && c.grabbedCreature!=&c && c.grabbedCreature->grabbedByCreatureId==c.id;
        return false;
    }

    void tryAcquireGrab(Creature& c){
        if(c.grabbedKind!=GrabKind::None || c.grabbedByCreatureId>=0) return;
        const float reach=std::max(0.5f,c.radius()*cfg::tuning.grabReachBodyScale+cfg::tuning.grabReachExtra);
        const Vector2 center=add(c.pos,mul(c.forward(),c.radius()+reach*0.45f));
        const float carrierMass=std::max(0.01f,c.mass());
        float bestScore=1e30f; GrabKind kind=GrabKind::None; Plant* bp=nullptr;Carcass* bk=nullptr;Creature* bc=nullptr;
        auto consider=[&](float d,float mass,GrabKind k,Plant* p,Carcass* carc,Creature* cr){
            const float ratio=mass/carrierMass;
            if(ratio>cfg::tuning.grabMaxLoadMassRatio) return;
            // Distancia domina; cargas pesadas recebem pequena penalidade na escolha.
            const float score=d+reach*0.18f*ratio;
            if(score<bestScore){bestScore=score;kind=k;bp=p;bk=carc;bc=cr;}
        };
        // v0.098: plantas sao sempre enraizadas e nao podem ser agarradas.
        const float maxCarcassR=std::max(1.0f,cfg::tuning.maxCreatureSize*cfg::tuning.carcassVisualScale);
        carcassGrid.forCircle(center.x,center.y,reach+maxCarcassR,[&](Carcass* k){
            if(!k||k->finished()||k->grabbedByCreatureId>=0)return;
            const float d=std::max(0.0f,std::sqrt(std::max(0.0f,toroidalDist2(center,k->pos)))-k->radius());
            if(d<=reach)consider(d,k->mass(),GrabKind::Carcass,nullptr,k,nullptr);
        });
        creatureGrid.forCircle(center.x,center.y,reach+cfg::tuning.maxCreatureSize*0.5f,[&](Creature* other){
            if(!other||other==&c||other->dead()||other->grabbedByCreatureId>=0)return;
            // Evita ciclo direto A segura B enquanto B segura A.
            if(other->grabbedKind==GrabKind::Creature && other->grabbedCreature==&c)return;
            const float d=std::max(0.0f,std::sqrt(std::max(0.0f,toroidalDist2(center,other->pos)))-other->radius());
            if(d<=reach)consider(d,other->mass(),GrabKind::Creature,nullptr,nullptr,other);
        });
        if(kind==GrabKind::None)return;
        c.grabbedKind=kind;c.grabbedPlant=bp;c.grabbedCarcass=bk;c.grabbedCreature=bc;
        if(bp) bp->grabbedByCreatureId=c.id;
        if(bk) bk->grabbedByCreatureId=c.id;
        if(bc) bc->grabbedByCreatureId=c.id;
        c.grabLoadRatio=grabbedTargetMass(c)/carrierMass;
        log(c.id,std::string("Agarrou ")+(kind==GrabKind::Plant?"planta solta":kind==GrabKind::Carcass?"carne":"bicho #"+std::to_string(bc?bc->id:-1))+"; carga/massa "+f2(c.grabLoadRatio)+"x.");
    }

    void updateGrabIntents(){
        for(Creature* c:creatures){
            if(!c||c->dead())continue;
            if(isManualControlled(*c)) continue; // G controla agarrar/soltar manualmente.
            const float out=clampf(c->brainOut[cfg::GRAB_OUTPUT_INDEX],0.0f,1.0f);
            if(c->grabbedKind!=GrabKind::None){
                if(out<cfg::tuning.grabOutputThreshold*0.55f || !grabStillValid(*c)) releaseGrab(*c);
            }else if(out>=cfg::tuning.grabOutputThreshold){
                tryAcquireGrab(*c);
            }
        }
    }

    void applyGrabConstraints(){
        for(Creature* c:creatures){
            if(!c||c->dead()||c->grabbedKind==GrabKind::None)continue;
            if(!grabStillValid(*c)){releaseGrab(*c);continue;}
            const float tr=grabbedTargetRadius(*c);
            Vector2 anchor=add(c->pos,mul(c->forward(),c->radius()+tr+cfg::tuning.grabGap));
            wrapCircle(anchor,tr);
            const Vector2 v=c->worldVelocity();
            if(c->grabbedKind==GrabKind::Plant){releaseGrab(*c);continue;}
            else if(c->grabbedKind==GrabKind::Carcass && c->grabbedCarcass){c->grabbedCarcass->pos=anchor;c->grabbedCarcass->velocity=v;carcassGrid.update(c->grabbedCarcass);}
            else if(c->grabbedKind==GrabKind::Creature && c->grabbedCreature){c->grabbedCreature->pos=anchor;c->grabbedCreature->setWorldVelocity(v);creatureGrid.update(c->grabbedCreature);}
        }
    }

    void updateCreature(Creature& c, float dt) {
        c.age += dt;
        const bool wasNaturalDeath=c.naturalDeath;
        const float growthCost=c.updateLifecycle(dt);
        if(!wasNaturalDeath && c.naturalDeath)
            log(c.id,"Falha biologica por velhice aos "+f2(c.age)+"s (longevidade DNA "+f2(c.genome.development.longevity)+"s).");
        c.reproCooldown = std::max(0.0f, c.reproCooldown - dt);
        const float colorDisplayCost=c.updateBodyColor(dt);
        // Feedbacks fisicos recentes desaparecem gradualmente, criando memoria corporal curta.
        const float feedbackDecay=std::exp(-dt/std::max(0.01f,cfg::tuning.tactileMemorySeconds));
        c.recentImpact*=feedbackDecay; c.recentDamage*=feedbackDecay; c.recentEnergyGain*=feedbackDecay;
        c.biteCooldown = std::max(0.0f, c.biteCooldown - dt);
        c.biteAnimation = std::max(0.0f, c.biteAnimation - dt);
        // vegetationFrictionMult e calculado antes do movimento pelo passe de copas.
        // Plantas continuam atravessaveis: isto e apenas arrasto, nunca colisao/empurrao.

        float fwd = c.brainOut[0], rev = c.brainOut[1], left = c.brainOut[2], right = c.brainOut[3];
        float strafeLeft = c.brainOut[4], strafeRight = c.brainOut[5];
        if(isManualControlled(c)){
            fwd=manualForward?1.0f:0.0f; rev=manualReverse?1.0f:0.0f;
            left=manualTurnLeft?1.0f:0.0f; right=manualTurnRight?1.0f:0.0f;
            strafeLeft=manualStrafeLeft?1.0f:0.0f; strafeRight=manualStrafeRight?1.0f:0.0f;
        }
        const float longitudinalCommand=std::max(fwd,rev);
        const float lateralCommand=std::max(strafeLeft,strafeRight);
        const float multiAxis=clampf(longitudinalCommand*lateralCommand,0.0f,1.0f);
        const float load=clampf(c.grabLoadRatio,0.0f,cfg::tuning.grabMaxLoadMassRatio);
        // Dois eixos ativos dividem a capacidade muscular. Carregar massa tambem reduz a aceleracao.
        const float accelAvailable=c.accelEffective/(1.0f+cfg::tuning.multiAxisForcePenalty*multiAxis+cfg::tuning.grabAccelerationPenalty*load);
        const float loadSpeedMult=1.0f/(1.0f+cfg::tuning.grabSpeedPenalty*load);
        const float frictionEff=c.genome.physical.friction*cfg::tuning.globalFriction;
        if (fwd > 0) c.speed += accelAvailable * fwd * dt;
        if (rev > 0) c.speed -= accelAvailable * rev * dt;
        c.speed = clampf(c.speed, -c.maxReverseEffective*loadSpeedMult, c.maxForwardEffective*loadSpeedMult);
        if (fwd == 0 && rev == 0)
            c.speed = moveToward(c.speed, 0.0f, frictionEff * dt);

        // Movimento lateral independente: desloca para esquerda/direita sem alterar
        // a orientacao. Usa a mesma aceleracao corporal, mas tem limite genetico proprio.
        if (strafeRight > 0) c.lateralSpeed += accelAvailable * strafeRight * dt;
        if (strafeLeft > 0) c.lateralSpeed -= accelAvailable * strafeLeft * dt;
        c.lateralSpeed = clampf(c.lateralSpeed, -c.maxLateralEffective*loadSpeedMult, c.maxLateralEffective*loadSpeedMult);
        if (strafeLeft == 0 && strafeRight == 0)
            c.lateralSpeed = moveToward(c.lateralSpeed, 0.0f, frictionEff * dt);

        // Arrasto passivo existe mesmo com o motor ativo. Assim a aceleracao disputa
        // com a friccao e a velocidade de cruzeiro fica menor, sem remover a
        // influencia genetica de tamanho, aceleracao e velocidade maxima.
        if (c.speed != 0.0f || c.lateralSpeed != 0.0f) {
            const float dragFactor = std::exp(-frictionEff * cfg::PASSIVE_DRAG_SCALE * cfg::tuning.passiveDrag * dt);
            c.speed *= dragFactor;
            c.lateralSpeed *= dragFactor;
        }

        // Vegetacao oferece resistencia mesmo quando o animal esta acelerando.
        // O multiplicador e atualizado no mesmo passe que ja calcula o balanco das plantas,
        // evitando uma nova consulta espacial em todo subpasso.
        if (c.vegetationFrictionMult > 1.0f) {
            const float extraDrag = frictionEff * (c.vegetationFrictionMult - 1.0f);
            c.speed = moveToward(c.speed, 0.0f, extraDrag * dt);
            c.lateralSpeed = moveToward(c.lateralSpeed, 0.0f, extraDrag * dt);
        }

        // Impulsos de colisao dissipam separadamente da friccao do motor.
        if (length2(c.pushVelocity) > 1e-8f) {
            const float pushDamp = std::exp(-cfg::PHYSICS_CREATURE_PUSH_DAMPING * cfg::tuning.creaturePushDamping * dt);
            c.pushVelocity = mul(c.pushVelocity, pushDamp);
        }

        float deltaAngle = (right - left) * c.rotEffective * dt;
        c.angularVelocity = dt>1e-8f ? deltaAngle/dt : 0.0f;
        if (deltaAngle != 0.0f) { c.angle = std::fmod(c.angle + deltaAngle + 3600.0f, 360.0f); c.refreshDirection(); }
        Vector2 worldVel = add(add(mul(c.forward(), c.speed), mul(c.right(), c.lateralSpeed)), c.pushVelocity);
        c.pos = add(c.pos, mul(worldVel, dt));
        c.wrap();
        creatureGrid.update(&c);

        const float brainComplexityCost=(
            float(c.genome.brain.hiddenCount)*cfg::tuning.brainNeuronEnergyCostPerSecond +
            float(c.compiled.activeConnections)*cfg::tuning.brainConnectionEnergyCostPerSecond +
            float(c.compiled.recurrentConnections)*cfg::tuning.brainRecurrentConnectionEnergyCostPerSecond +
            float(c.compiled.statefulHiddenNodes)*cfg::tuning.brainStatefulNeuronEnergyCostPerSecond +
            float(brainRegisterCapacity(c.genome.brain))*cfg::tuning.brainRegisterEnergyCostPerSecond +
            float(c.compiled.plasticConnections)*cfg::tuning.brainPlasticConnectionEnergyCostPerSecond)*dt;
        float m = cfg::METABOLISM * cfg::tuning.metabolismCost * c.sizeEnergyCostMult *
            c.ageMetabolismMult * c.longevityMaintenanceMult * dt + growthCost + colorDisplayCost + brainComplexityCost;
        // Custo sensorial depende do numero de raios ativos. Um animal pode economizar
        // energia geneticamente reduzindo resolucao angular, inclusive ate 1 raio.
        float vc = c.visionEnergyCostPerSecond() * dt;
        float fc = fwd * cfg::FORWARD_COST * cfg::tuning.forwardCost * c.sizeEnergyCostMult * dt;
        float rc = rev * cfg::REVERSE_COST * cfg::tuning.reverseCost * c.sizeEnergyCostMult * dt;
        float tc = (left + right) * cfg::TURN_COST * cfg::tuning.turnCost * c.sizeEnergyCostMult * dt;
        float lc = (strafeLeft + strafeRight) * cfg::LATERAL_COST * cfg::tuning.lateralCost * c.sizeEnergyCostMult * dt;
        // A combinacao frente/re + lateral tem sobretaxa proporcional a quanto os dois eixos sao exigidos.
        const float multiAxisExtra=(fc+rc+lc)*cfg::tuning.multiAxisEnergyPenalty*multiAxis;
        const float speedDen = std::max(0.01f, c.speed >= 0.0f ? c.maxForwardEffective : c.maxReverseEffective);
        const float longitudinalLevel = std::fabs(c.speed) / speedDen;
        const float lateralLevel = std::fabs(c.lateralSpeed) / std::max(0.01f,c.maxLateralEffective);
        const float motionLevel = clampf(std::sqrt(longitudinalLevel*longitudinalLevel + lateralLevel*lateralLevel), 0.0f, 1.75f);
        // Movimento combinado frente+lado custa mais porque aumenta a velocidade corporal total.
        float mc = motionLevel * motionLevel * cfg::MOTION_COST * cfg::tuning.motionCost * c.sizeEnergyCostMult * dt;
        const float grabCost=(c.grabbedKind!=GrabKind::None)
            ? (cfg::tuning.grabHoldEnergyCost*load + cfg::tuning.grabMoveEnergyCost*load*motionLevel)*dt : 0.0f;
        float req = m + vc + fc + rc + tc + lc + multiAxisExtra + mc + grabCost;
        float beforeE = c.energy;
        c.energy = clampf(c.energy - req, 0.0f, c.maxEnergy);
        float actual = beforeE - c.energy;
        float scale = req > 0 ? actual / req : 0.0f;
        float beforeH = c.health;
        float threshold = c.maxEnergy * cfg::tuning.starvationThreshold;
        if (c.energy < threshold) {
            float severity = 1.0f - c.energy / threshold;
            const float hpBeforeStarvation=c.health;
            c.health = clampf(c.health - cfg::tuning.starvationDamage * severity * dt, 0.0f, cfg::tuning.healthMax);
            if(hpBeforeStarvation>0.0f && c.health<=0.0f && c.deathCause==DeathCause::Unknown) c.deathCause=DeathCause::Starvation;
        }

        // Sangue/vida se regenera quando a reserva energetica passa do limiar configurado.
        // Padrao v0.063: 50% -> 0 cura/s
        // 90% -> 25% da taxa maxima
        // 95% -> 56.25%
        // 100% -> 100% da taxa maxima
        float healthBeforeRegen = c.health;
        const float energyRatio = c.maxEnergy > 0.0f ? clampf(c.energy / c.maxEnergy, 0.0f, 1.0f) : 0.0f;
        if (energyRatio > cfg::tuning.healthRegenThreshold && c.health > 0.0f && c.health < cfg::tuning.healthMax) {
            const float regenT = clampf(
                (energyRatio - cfg::tuning.healthRegenThreshold) /
                (1.0f - cfg::tuning.healthRegenThreshold),
                0.0f, 1.0f);
            const float regenRate = cfg::tuning.healthRegenMaxPerSecond * cfg::tuning.healthRegen * c.ageRegenMult * regenT * regenT;
            c.health = clampf(c.health + regenRate * dt, 0.0f, cfg::tuning.healthMax);
        }
        const float starvationLoss = std::max(0.0f, beforeH - healthBeforeRegen);
        const float regenGain = std::max(0.0f, c.health - healthBeforeRegen);
        recordContinuous(c, dt, m * scale, vc * scale, fc * scale, rc * scale, tc * scale, lc * scale, mc * scale, starvationLoss, regenGain);
    }

    bool plantPositionFree(Vector2 p,float candidateAdultSize=-1.0f) const {
        const float candidate=candidateAdultSize>0.0f?candidateAdultSize:cfg::tuning.plantMaxSize;
        const float candidateRadius=std::max(cfg::tuning.plantRadiusMin,std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,candidate)*cfg::tuning.plantRadiusScale);
        if(!circleInsideWorld(p,candidateRadius)) return false;
        const float broad=std::max(candidate,cfg::tuning.plantMaxSize*cfg::tuning.plantSizeScaleMax);
        return !plantGrid.anyCircle(p.x, p.y, broad, [&](Plant* other){
            const float otherAdult = other->hasBeenEaten ? other->size : other->adultSizeTarget();
            const float required = std::max(cfg::tuning.plantMinSpacing, ((otherAdult + candidate) * 0.5f) * cfg::tuning.plantOverlapFactor);
            return toroidalDist2(p, other->pos) < required * required;
        });
    }

    // O ovo e colocado apenas onde caberia o futuro filhote.
    // A posicao rejeita bicho, planta, carcaca, ovo e outro ovo pendente.
    bool creatureBirthPositionFree(Vector2 p, float childSize) const {
        const float childR = childSize * 0.5f;
        if(!circleInsideWorld(p,childR)) return false;
        const float gap = cfg::tuning.creatureBirthGap;
        const float creatureBroad = childR + cfg::tuning.maxCreatureSize * 0.5f + gap;
        if (creatureGrid.anyCircle(p.x, p.y, creatureBroad, [&](Creature* other){
            if (!other || other->health <= 0.0f) return false;
            const float required = childR + other->size * 0.5f + gap;
            return toroidalDist2(p, other->pos) < required * required;
        })) return false;

        // Ovos preparados neste mesmo subpasso tambem reservam espaco.
        for (const auto& pending : eggNewbornScratch) {
            if (!pending) continue;
            const float required = childR + pending->radius + gap;
            if (toroidalDist2(p, pending->pos) < required * required) return false;
        }

        // Plantas sao copas atravessaveis: nao reservam/impedem local de ovo ou nascimento.

        const float maxCarcassOcc = cfg::tuning.maxCreatureSize * cfg::tuning.carcassVisualScale * 0.62f;
        const float carcassBroad = childR + maxCarcassOcc + gap;
        if (carcassGrid.anyCircle(p.x, p.y, carcassBroad, [&](Carcass* carcass){
            if (!carcass || carcass->finished()) return false;
            const float occupied = std::max(carcass->sensorRadius, carcass->visualSize);
            const float required = childR + occupied + gap;
            return toroidalDist2(p, carcass->pos) < required * required;
        })) return false;

        const float maxEggRadius = 0.50f*(cfg::tuning.eggRadiusBase + cfg::tuning.maxCreatureSize * cfg::tuning.eggRadiusSizeScale);
        const float eggBroad = childR + maxEggRadius + gap;
        if (eggGrid.anyCircle(p.x, p.y, eggBroad, [&](Egg* egg){
            if (!egg || egg->destroyed()) return false;
            const float required = childR + egg->radius + gap;
            return toroidalDist2(p, egg->pos) < required * required;
        })) return false;

        return true;
    }

    bool findCreatureBirthPosition(const Creature& parent, float childSize, Vector2& out) {
        const float base = parent.size * 0.5f + childSize * 0.5f + cfg::tuning.creatureBirthGap;
        // Variamos angulo e um pequeno raio extra. Continua sendo nascimento
        // ao lado do pai, mas consegue encontrar uma fresta livre quando existe.
        const float phase = rf(0.0f, 2.0f * PI);
        for (int attempt = 0; attempt < (int)std::lround(cfg::tuning.creatureReproAttempts); ++attempt) {
            const float ring = float(attempt / 12) / 2.0f; // 0, 0.5, 1.0
            const float angle = phase + (2.0f * PI * float(attempt % 12) / 12.0f) + rf(-0.10f, 0.10f);
            const float d = base + cfg::tuning.creatureBirthExtraRadius * ring;
            Vector2 p{parent.pos.x + std::cos(angle) * d, parent.pos.y + std::sin(angle) * d};
            if (creatureBirthPositionFree(p, childSize)) { out = p; return true; }
        }
        return false;
    }

    Creature* chooseCreaturePartner(Creature& parent) {
        const float range=clampf(parent.genome.reproduction.mateRange,cfg::tuning.mateRangeMin,cfg::tuning.mateRangeMax);
        Creature* best=nullptr; float bestScore=-1e30f;
        const float strength=clampf(parent.genome.reproduction.mateChoiceStrength,0.0f,1.0f);
        creatureGrid.forCircle(parent.pos.x,parent.pos.y,range,[&](Creature* other){
            if(!other || other==&parent || other->dead() || !other->canMateAsPartner())return;
            const float d=std::sqrt(std::max(0.0f,toroidalDist2(parent.pos,other->pos)));
            if(d>range)return;
            const float proximity=1.0f-clampf(d/std::max(0.01f,range),0.0f,1.0f);
            const float colorSim=colorSimilarity(parent.displayColor,other->displayColor);
            const float vp=clampf(parent.genome.reproduction.visualPreference,-1.0f,1.0f);
            const float visualDesired=vp>=0?colorSim:(1.0f-colorSim);
            const float weighted=proximity*cfg::tuning.mateSpatialWeight+visualDesired*std::fabs(vp)*cfg::tuning.mateVisualWeight;
            const float score=(1.0f-strength)*proximity+strength*weighted+rf(-cfg::tuning.mateChoiceNoise,cfg::tuning.mateChoiceNoise);
            if(score>bestScore){bestScore=score;best=other;}
        });
        return best;
    }

    bool sameLearningConnection(const NeuralConnectionGene& a,const NeuralConnectionGene& b) const {
        return a.srcKind==b.srcKind && a.src==b.src && a.dstKind==b.dstKind && a.dst==b.dst &&
               neuralConnectionMode(a)==neuralConnectionMode(b);
    }

    bool learnedDeltaFrom(const NeuralGenome& sourceGenome,const std::vector<float>& sourceLearned,
                          const NeuralConnectionGene& childGene,float& deltaOut) const {
        for(std::size_t i=0;i<sourceGenome.connections.size();++i){
            const auto& src=sourceGenome.connections[i];
            if(!sameLearningConnection(src,childGene)) continue;
            const float actual=i<sourceLearned.size()?sourceLearned[i]:src.weight;
            deltaOut=actual-src.weight;
            return true;
        }
        return false;
    }

    void prepareInheritedLearning(Egg& egg,const Creature& parent,const Creature* mate) const {
        const auto& childBrain=egg.genome.brain;
        egg.inheritedLearnedWeights.resize(childBrain.connections.size());
        bool inheritedAny=false;
        for(std::size_t i=0;i<childBrain.connections.size();++i){
            const auto& cg=childBrain.connections[i];
            float d1=0.0f,d2=0.0f;
            const bool h1=learnedDeltaFrom(parent.genome.brain,parent.learnedWeights,cg,d1);
            const bool h2=mate?learnedDeltaFrom(mate->genome.brain,mate->learnedWeights,cg,d2):false;
            float delta=0.0f;
            if(h1&&h2){
                // v0.135 - fusao de conhecimento: se os dois aprenderam na mesma direcao,
                // preserva a associacao mais forte; se aprenderam coisas opostas, combina
                // proporcionalmente a forca de cada experiencia em vez de apagar ambos.
                if((d1>=0.0f)==(d2>=0.0f)) delta=(std::fabs(d1)>=std::fabs(d2)?d1:d2);
                else{
                    const float w1=std::max(1e-5f,std::fabs(d1));
                    const float w2=std::max(1e-5f,std::fabs(d2));
                    delta=(d1*w1+d2*w2)/(w1+w2);
                }
            }else if(h1) delta=d1;
            else if(h2) delta=d2;
            if(h1||h2) inheritedAny=inheritedAny || std::fabs(delta)>1e-6f;
            const float range=std::max(0.0f,cfg::tuning.brainPlasticWeightRange);
            egg.inheritedLearnedWeights[i]=clampf(cg.weight+delta,
                std::max(-cfg::tuning.brainWeightLimit,cg.weight-range),
                std::min( cfg::tuning.brainWeightLimit,cg.weight+range));
        }

        // Memoria persistente: cada registro do filho recebe a consolidacao dos dois pais.
        egg.inheritedMemoryRegisters.fill(0.0f);
        const int childCap=brainRegisterCapacity(childBrain);
        const int pCap=brainRegisterCapacity(parent.genome.brain);
        const int mCap=mate?brainRegisterCapacity(mate->genome.brain):0;
        for(int slot=0;slot<childCap;++slot){
            float sum=0.0f;int n=0;
            if(slot<pCap){sum+=parent.memoryRegisters[slot];++n;}
            if(mate&&slot<mCap){sum+=mate->memoryRegisters[slot];++n;}
            if(n>0){
                egg.inheritedMemoryRegisters[slot]=clampf(sum/float(n),0.0f,1.0f);
                inheritedAny=inheritedAny||std::fabs(egg.inheritedMemoryRegisters[slot])>1e-6f;
            }
        }

        egg.inheritedLongMemory={};
        egg.inheritedLongMemoryHead=0;
        egg.inheritedLongMemoryCount=0;
        if(mate){
            // Memoria episodica dos DOIS pais. O cerebro possui quatro slots episodicos;
            // consolidamos as experiencias mais relevantes/recentes dos dois em conjunto.
            // Isso preserva conhecimento dos dois sem alterar INPUT_COUNT nem quebrar saves antigos.
            struct Candidate{LongMemoryEvent e{};float score=0.0f;};
            std::array<Candidate,cfg::LONG_MEMORY_SLOTS*2> candidates{};
            int count=0;
            auto collect=[&](const Creature& src){
                for(int recent=0;recent<(int)src.longMemoryCount && count<(int)candidates.size();++recent){
                    const int slot=(int(src.longMemoryHead)-1-recent+cfg::LONG_MEMORY_SLOTS)%cfg::LONG_MEMORY_SLOTS;
                    const LongMemoryEvent& e=src.longMemory[(std::size_t)slot];
                    if(e.type==MemoryEventType::None || e.time<0.0f) continue;
                    const float age=std::max(0.0f,simTime-e.time);
                    const float recency=1.0f/(1.0f+age/60.0f);
                    candidates[(std::size_t)count++]={e,clampf(e.intensity,0.0f,1.5f)*0.65f+recency*0.35f};
                }
            };
            collect(parent);collect(*mate);
            std::stable_sort(candidates.begin(),candidates.begin()+count,[](const Candidate& a,const Candidate& b){return a.score>b.score;});
            const int keep=std::min(count,cfg::LONG_MEMORY_SLOTS);
            // Mantem a ordem cronologica no ring final para a tela/inputs tratarem como memoria normal.
            std::array<LongMemoryEvent,cfg::LONG_MEMORY_SLOTS> chosen{};
            for(int i=0;i<keep;++i) chosen[(std::size_t)i]=candidates[(std::size_t)i].e;
            std::sort(chosen.begin(),chosen.begin()+keep,[](const LongMemoryEvent& a,const LongMemoryEvent& b){return a.time<b.time;});
            for(int i=0;i<keep;++i){
                egg.inheritedLongMemory[(std::size_t)i]=chosen[(std::size_t)i];
                egg.inheritedLongMemoryHead=(std::uint8_t)((i+1)%cfg::LONG_MEMORY_SLOTS);
            }
            egg.inheritedLongMemoryCount=(std::uint8_t)keep;
            if(keep>0) inheritedAny=true;
        }
        egg.hasInheritedLearning=inheritedAny;
    }

    void prepareInheritedLearning(Egg& egg,const MinimumLineageSeed& seed) const {
        const auto& childBrain=egg.genome.brain;
        egg.inheritedLearnedWeights.resize(childBrain.connections.size());
        bool inheritedAny=false;
        for(std::size_t i=0;i<childBrain.connections.size();++i){
            const auto& cg=childBrain.connections[i];
            float delta=0.0f;const bool found=learnedDeltaFrom(seed.genome.brain,seed.learnedWeights,cg,delta);
            if(found) inheritedAny=inheritedAny||std::fabs(delta)>1e-6f;
            const float range=std::max(0.0f,cfg::tuning.brainPlasticWeightRange);
            egg.inheritedLearnedWeights[i]=clampf(cg.weight+(found?delta:0.0f),
                std::max(-cfg::tuning.brainWeightLimit,cg.weight-range),
                std::min( cfg::tuning.brainWeightLimit,cg.weight+range));
        }
        egg.inheritedMemoryRegisters.fill(0.0f);
        egg.inheritedLongMemory={};egg.inheritedLongMemoryHead=0;egg.inheritedLongMemoryCount=0;
        const int childCap=brainRegisterCapacity(childBrain),sourceCap=brainRegisterCapacity(seed.genome.brain);
        for(int slot=0;slot<std::min(childCap,sourceCap);++slot){
            egg.inheritedMemoryRegisters[slot]=clampf(seed.memoryRegisters[slot],0.0f,1.0f);
            inheritedAny=inheritedAny||std::fabs(egg.inheritedMemoryRegisters[slot])>1e-6f;
        }
        egg.hasInheritedLearning=inheritedAny;
    }

    // Ferramenta manual de observacao: cria um ovo viavel do bicho seguido/selecionado
    // sem retirar energia, vida nem aplicar cooldown ao pai. Tambem ignora o teto populacional,
    // pois e uma acao explicita do pesquisador. Se nao houver fresta livre, usa uma posicao
    // segura ao lado do pai e deixa a fisica resolver uma eventual sobreposicao.
    bool forceFreeEgg(Creature& parent) {
        if(parent.dead()) return false;

        Creature* mate=chooseCreaturePartner(parent);
        CreatureGenome childGenome = breedGenome(parent.genome,mate?&mate->genome:nullptr);
        const float childSize = clampf(childGenome.physical.bodySize, cfg::tuning.minCreatureSize, cfg::tuning.maxCreatureSize);
        Vector2 eggPos{};
        if(!findCreatureBirthPosition(parent,childSize,eggPos)){
            const float childR=std::max(0.5f,childSize*0.5f);
            const float d=parent.radius()+childR+std::max(0.5f,cfg::tuning.creatureBirthGap);
            eggPos=add(parent.pos,mul(parent.forward(),d));
            constrainCircleToWorld(eggPos,childR);
        }

        // O ovo recebe os recursos que normalmente seriam investidos, mas eles sao clonados
        // para a acao manual: o pai permanece exatamente com a mesma energia/vida/cooldown.
        const float freeEnergy=std::max(0.01f,parent.eggEnergyInvestment());
        const float freeBlood=std::max(0.01f,parent.eggBloodInvestment());
        Egg* egg=eggPool.create(eggPos,std::move(childGenome),parent.generation+1,parent.id,freeEnergy,freeBlood);
        egg->secondParentId=mate?mate->id:-1;
        egg->conceivedWithMate=mate!=nullptr;
        egg->sexualHeritageDepth=mate?(std::uint16_t)std::min<int>(65535,std::max<int>(parent.sexualHeritageDepth,mate->sexualHeritageDepth)+1):parent.sexualHeritageDepth;
        prepareInheritedLearning(*egg,parent,mate);
        eggs.push_back(egg);
        eggGrid.insert(egg);
        ++evo.eggsLaid;
        rememberEventAt(parent,MemoryEventType::EggLaid,1.0f,eggPos);

        log(parent.id,std::string("Ovo GRATIS criado manualmente")+
            (mate?" com parceiro #"+std::to_string(mate->id):" sem parceiro")+
            "; nenhum custo de energia/vida/cooldown para o pai.");
        return true;
    }

    // [SEC-REINFORCEMENT] Qualidade do ovo e uma recompensa de POSTURA, separada
    // da recompensa extrema de FILHO NASCIDO. Aplica-se a ovo solo e acasalado.
    void rewardEggQuality(const Egg& egg, Creature& parent, Creature* mate=nullptr){
        if(cfg::tuning.reinforcementEnabled<=0.5f) return;
        const float q=clampf(egg.qualityScore,0.0f,1.0f);
        float reward=0.0f;
        if(q>=0.9995f) reward=cfg::tuning.reinforcementReproduction*0.40f;
        else if(q>=0.90f) reward=cfg::tuning.reinforcementReproduction*0.18f;
        if(reward==0.0f) return;
        parent.reinforce(reward);
        if(mate && mate!=&parent) mate->reinforce(reward);
    }

    bool trySoloReproduction(Creature& parent) {
        // Sem parceiro o bicho ainda pode botar ovo, pagando 100% do custo atual.
        // A viabilidade sera decidida na eclosao: 10% viram carne, 90% nascem.
        if(!parent.canReproduce(1.0f)) return false;
        // O limite de bichos vivos nao bloqueia a postura. O ovo pode esperar
        // no mundo ate abrir uma vaga; limitamos apenas a fila de ovos para evitar
        // crescimento sem limite quando a populacao viva esta no teto.
        if(limits.maxEnabled &&
           (int)eggs.size() + (int)eggNewbornScratch.size() >= std::max(8,limits.maxCreatures))
            return false;

        CreatureGenome childGenome=breedGenome(parent.genome,nullptr);
        const float childSize=clampf(childGenome.physical.bodySize,cfg::tuning.minCreatureSize,cfg::tuning.maxCreatureSize);
        Vector2 eggPos{};
        if(!findCreatureBirthPosition(parent,childSize,eggPos)){
            const float childR=std::max(0.5f,childSize*0.5f);
            const float d=parent.radius()+childR+std::max(0.5f,cfg::tuning.creatureBirthGap);
            eggPos=add(parent.pos,mul(parent.forward(),d));
            constrainCircleToWorld(eggPos,childR);
        }

        float eggEnergy=0.0f,eggBlood=0.0f,paidEnergy=0.0f,paidBlood=0.0f;
        parent.investInEgg(eggEnergy,eggBlood,1.0f,&paidEnergy,&paidBlood);
        Egg* egg=eggPool.create(eggPos,std::move(childGenome),parent.generation+1,parent.id,eggEnergy,eggBlood);
        egg->secondParentId=-1;
        egg->conceivedWithMate=false;
        egg->plannedBirthCount=1;
        // Heranca sexual nao e apagada por um nascimento solo excepcional.
        egg->sexualHeritageDepth=parent.sexualHeritageDepth;
        prepareInheritedLearning(*egg,parent,nullptr);
        rewardEggQuality(*egg,parent,nullptr);
        eggNewbornScratch.push_back(egg);
        ++evo.eggsLaid; ++evo.eggsLaidSolo;
        ++parent.eggsLaidLifetime;
        rememberEventAt(parent,MemoryEventType::EggLaid,1.0f,eggPos);
        parent.matingPartnerId=-1;
        parent.matingContactTime=0.0f;
        parent.soloFallbackTimer=0.0f;
        log(parent.id,"Botou ovo SOZINHO; custo integral energia -"+f2(paidEnergy)+
            ", sangue -"+f2(paidBlood)+". Chance: 90% nascer / 10% virar carne.");
        return true;
    }

    bool tryCreatureReproduction(Creature& parent, Creature& mate, float headContactSeconds) {
        const float costScale=clampf(cfg::tuning.partneredEggCostScale,0.0f,1.0f);
        if (!parent.canReproduce(costScale) || !mate.canMateAsPartner() || parent.id==mate.id) return false;
        // Igual ao ovo solo: maxCreatures limita NASCIDOS, nao ovos incubando.
        if (limits.maxEnabled &&
            (int)eggs.size() + (int)eggNewbornScratch.size() >= std::max(8,limits.maxCreatures))
            return false;

        CreatureGenome childGenome = breedGenome(parent.genome,&mate.genome);
        const float childSize = clampf(childGenome.physical.bodySize, cfg::tuning.minCreatureSize, cfg::tuning.maxCreatureSize);
        Vector2 eggPos{};
        if (!findCreatureBirthPosition(parent, childSize, eggPos)) {
            Vector2 mid=mul(add(parent.pos,mate.pos),0.5f);
            Vector2 side=parent.right();
            const float d=std::max(2.0f,0.35f*(parent.radius()+mate.radius()));
            eggPos=add(mid,mul(side,d));
            constrainCircleToWorld(eggPos,std::max(0.5f,childSize*0.5f));
        }

        float eggEnergy=0.0f,eggBlood=0.0f,paidEnergy=0.0f,paidBlood=0.0f;
        parent.investInEgg(eggEnergy,eggBlood,costScale,&paidEnergy,&paidBlood);
        // Qualidade de contato: uma batida curta ja produz ovo bom (~90%). Manter as
        // cabecas encostadas ate o tempo-alvo eleva progressivamente para 100%.
        const float minContact=std::max(0.03f,(cfg::tuning.matingContactSeconds/std::max(1.0f,cfg::tuning.matingEaseFactor))*0.25f);
        const float fullQualityContact=std::max(minContact+0.01f,cfg::tuning.matingContactSeconds*2.0f);
        const float contact01=clampf((headContactSeconds-minContact)/(fullQualityContact-minContact),0.0f,1.0f);
        const float contactQuality=0.90f+0.10f*contact01;
        eggEnergy *= (1.18f + 0.12f*contact01);
        eggBlood  *= (1.18f + 0.12f*contact01);
        const float mateEnergyCost=mate.maxEnergy*cfg::tuning.matingPartnerEnergyCostFraction*costScale;
        mate.energy=std::max(0.0f,mate.energy-mateEnergyCost);
        const float mateAgeCd=1.0f+(cfg::tuning.oldAgeReproCooldownMaxMult-1.0f)*(1.0f-mate.ageFertilityMult);
        mate.reproCooldown=cfg::tuning.reproCooldown*std::max(0.01f,mateAgeCd);

        Egg* egg = eggPool.create(eggPos,std::move(childGenome),std::max(parent.generation,mate.generation)+1,parent.id,eggEnergy,eggBlood);
        egg->secondParentId=mate.id;
        egg->conceivedWithMate=true;
        egg->sexualHeritageDepth=(std::uint16_t)std::min<int>(65535,std::max<int>(parent.sexualHeritageDepth,mate.sexualHeritageDepth)+1);
        egg->incubationDuration*=0.5f;
        egg->qualityScore=clampf(std::max(egg->qualityScore,contactQuality),0.0f,1.0f);
        // Ninhada sexual: 1 filho e o normal. A chance configurada de gemeos abre
        // uma ninhada multipla; quanto melhor o ovo/contato, maior o teto possivel,
        // chegando a 10 em qualidade maxima. O sorteio e enviesado para ninhadas menores.
        const float broodQuality=clampf(egg->qualityScore,0.0f,1.0f);
        const int maxBrood=std::clamp(1+(int)std::floor(clampf((broodQuality-0.88f)/0.12f,0.0f,1.0f)*9.999f),1,10);
        egg->plannedBirthCount=1;
        if(maxBrood>=2 && deterministicEggRoll(*egg,0x7A11u)<cfg::tuning.partneredEggTwinChance){
            const float roll=deterministicEggRoll(*egg,0xB00Du);
            const float biased=roll*roll; // favorece 2-4, mas 10 continua possivel
            egg->plannedBirthCount=2+(int)std::floor(biased*(float)(maxBrood-1));
            egg->plannedBirthCount=std::clamp(egg->plannedBirthCount,2,maxBrood);
        }
        prepareInheritedLearning(*egg,parent,&mate);
        eggNewbornScratch.push_back(egg);
        ++evo.eggsLaid; ++evo.eggsLaidMated;
        ++parent.eggsLaidLifetime;
        rememberEventAt(parent,MemoryEventType::EggLaid,1.0f,eggPos);
        rememberEventAt(mate,MemoryEventType::EggLaid,0.7f,eggPos);
        parent.matingPartnerId=mate.matingPartnerId=-1;
        parent.matingContactTime=mate.matingContactTime=0.0f;
        parent.soloFallbackTimer=mate.soloFallbackTimer=0.0f;
        log(parent.id,"Acasalou com #"+std::to_string(mate.id)+
            "; ovo 100% viavel, incubacao metade, " + std::to_string(egg->plannedBirthCount) + " filho(s) planejado(s); custo energia -"+f2(paidEnergy)+
            ", sangue -"+f2(paidBlood)+" (70% menos).");
        log(mate.id,"Acasalou com #"+std::to_string(parent.id)+"; custo energia -"+f2(mateEnergyCost)+".");

        rewardEggQuality(*egg,parent,&mate);
        return true;
    }

    void evaluateFast50ToZeroRecord(Creature& c,float elapsed,float& record,int& holder,float penalty){
        if(elapsed<=0.0001f) return;
        if(holder<0 || record<=0.0001f){
            // O primeiro colapso estabelece a referencia e ja recebe a punicao-base.
            record=elapsed; holder=c.id; c.reinforce(penalty);
            return;
        }
        if(elapsed<record){
            const float old=std::max(0.0001f,record);
            const float improvement=clampf((old-elapsed)/old,0.0f,1.0f);
            record=elapsed; holder=c.id;
            // Quanto mais rapido que o recorde anterior, mais negativa fica a punicao.
            c.reinforce(penalty*(1.0f+improvement));
        }
    }

    void updateRecordTracking(Creature& c,float dt){
        if(c.dead() || cfg::tuning.reinforcementEnabled<=0.5f) return;
        const float hr=clampf(c.health/std::max(0.01f,cfg::tuning.healthMax),0.0f,1.0f);
        const float er=clampf(c.energy/std::max(0.01f,c.maxEnergy),0.0f,1.0f);

        // Marcadores bons continuam acumulando TEMPO TOTAL acima de 50%.
        if(hr>=0.5f){ c.timeHealthAbove50+=dt; c.timeHealthBelow50=0.0f; }
        else if(c.timeHealthBelow50>=0.0f) c.timeHealthBelow50+=dt;
        if(er>=0.5f){ c.timeEnergyAbove50+=dt; c.timeEnergyBelow50=0.0f; }
        else if(c.timeEnergyBelow50>=0.0f) c.timeEnergyBelow50+=dt;

        auto high=[&](float value,float& record,int& holder,float unitReward){
            if(value>record){float delta=value-record;record=value;holder=c.id;c.reinforce(unitReward*delta);}
        };
        high(c.age,reinforcementRecords.longestLife,reinforcementRecords.longestLifeId,cfg::tuning.reinforcementRecordLongestLife);
        high(c.timeHealthAbove50,reinforcementRecords.longestHealthAbove50,reinforcementRecords.healthAboveId,cfg::tuning.reinforcementRecordHealthAbove50);
        high(c.timeEnergyAbove50,reinforcementRecords.longestEnergyAbove50,reinforcementRecords.energyAboveId,cfg::tuning.reinforcementRecordEnergyAbove50);

        // Marcador ruim de energia = quem foi MAIS RAPIDO de 50% ate 0%, nao quem
        // passou mais tempo abaixo de 50%. Valor negativo congela a tentativa ja concluida
        // para ela nao ser contada novamente enquanto continuar em energia zero.
        if(er<=0.0001f && c.timeEnergyBelow50>0.0001f){
            const float elapsed=c.timeEnergyBelow50;
            evaluateFast50ToZeroRecord(c,elapsed,reinforcementRecords.fastestEnergy50ToZero,reinforcementRecords.energyCrashId,cfg::tuning.reinforcementRecordEnergyBelow50);
            c.timeEnergyBelow50=-elapsed;
        }

        if(c.eggsLaidLifetime>reinforcementRecords.mostEggs){
            const int delta=c.eggsLaidLifetime-reinforcementRecords.mostEggs;
            reinforcementRecords.mostEggs=c.eggsLaidLifetime;reinforcementRecords.mostEggsId=c.id;
            c.reinforce(cfg::tuning.reinforcementRecordMostEggs*(float)delta);
        }
    }

    void evaluateInverseTerminalRecords(Creature& c){
        // MORRER e deliberadamente a pior consequencia do treinamento. Ela entra
        // antes da ultima atualizacao plastica para punir os circuitos recentemente ativos.
        c.reinforce(cfg::tuning.reinforcementDeath);

        // Sangue em 0% encerra a vida; aqui fechamos o cronometro 50% -> 0%.
        if(c.timeHealthBelow50>0.0001f){
            evaluateFast50ToZeroRecord(c,c.timeHealthBelow50,reinforcementRecords.fastestHealth50ToZero,reinforcementRecords.healthCrashId,cfg::tuning.reinforcementRecordHealthBelow50);
            c.timeHealthBelow50=-c.timeHealthBelow50;
        }

        // Menor vida e menor quantidade de ovos so fazem sentido ao encerrar uma vida.
        if(!reinforcementRecords.hasShortestLife){
            reinforcementRecords.shortestLife=c.age;reinforcementRecords.shortestLifeId=c.id;reinforcementRecords.hasShortestLife=true;
        }else if(c.age<reinforcementRecords.shortestLife){
            const float old=std::max(0.01f,reinforcementRecords.shortestLife);
            const float frac=(old-c.age)/old;
            reinforcementRecords.shortestLife=c.age;reinforcementRecords.shortestLifeId=c.id;
            c.reinforce(cfg::tuning.reinforcementRecordShortestLife*frac);
        }
        if(!reinforcementRecords.hasFewestEggs){
            reinforcementRecords.fewestEggs=c.eggsLaidLifetime;reinforcementRecords.fewestEggsId=c.id;reinforcementRecords.hasFewestEggs=true;
        }else if(c.eggsLaidLifetime<reinforcementRecords.fewestEggs){
            const float old=(float)std::max(1,reinforcementRecords.fewestEggs);
            const float frac=((float)reinforcementRecords.fewestEggs-(float)c.eggsLaidLifetime)/old;
            reinforcementRecords.fewestEggs=c.eggsLaidLifetime;reinforcementRecords.fewestEggsId=c.id;
            c.reinforce(cfg::tuning.reinforcementRecordFewestEggs*frac);
        }
        // Garante que morte + recordes terminais alterem os pesos ANTES de copiar
        // a linhagem para a geracao seguinte.
        if(std::fabs(c.reinforcementPending)>1e-6f) updateLifetimePlasticity(c);
    }

    // [REGRA-CRITICA] eggNewbornScratch e uma fila transacional: criar ovo -> cobrar custo
// -> manter na fila -> commit em eggs/eggGrid no final de processBites(). Nunca limpar
// essa fila entre updateMating() e o commit.
// [SEC-REPRODUCTION] Acasalamento por sessao cabeca-com-cabeca.
    // Uma batida valida ja cria uma sessao; o ovo e finalizado quando as cabecas se
    // separam ou quando a qualidade chega a 100%. Isso torna possivel contato prolongado
    // melhorar o ovo sem exigir que os bichos "esperem" por um parceiro distante.
    void updateMating(float dt){
        const float ease=std::max(0.25f,cfg::tuning.matingEaseFactor);
        const float minContact=std::max(0.02f,(cfg::tuning.matingContactSeconds/std::max(1.0f,ease))*0.20f);
        const float fullQualityContact=std::max(minContact+0.01f,cfg::tuning.matingContactSeconds*1.35f);
        const float partnerCostScale=clampf(cfg::tuning.partneredEggCostScale,0.0f,1.0f);
        auto effectiveMateIntent=[&](const Creature& c)->float{
            const float neural=clampf(c.brainOut[cfg::MATE_INTENT_OUTPUT_INDEX],0.0f,1.0f);
            // v0.135: para o acasalamento finalmente acontecer, existe um drive biologico
            // minimo quando o individuo ja esta pronto para reproduzir. O output neural
            // continua podendo reforcar/modular essa vontade.
            float innate=0.0f;
            if(c.canReproduce(partnerCostScale)) innate=0.85f;
            else if(c.canMateAsPartner()) innate=0.65f;
            return std::max(neural,innate);
        };
        auto findNearbyMateCandidate=[&](Creature& p,float radius)->Creature*{
            Creature* best=nullptr; float bestScore=-1e30f;
            creatureGrid.forCircle(p.pos.x,p.pos.y,radius,[&](Creature* o){
                if(!o || o==&p || o->dead() || !o->canMateAsPartner()) return;
                if(o->matingPartnerId>=0 && o->matingPartnerId!=p.id) return;
                const float intentSelf=effectiveMateIntent(p);
                const float intentOther=effectiveMateIntent(*o);
                if(intentSelf<0.35f || intentOther<0.35f) return;
                const float d2=toroidalDist2(p.pos,o->pos);
                const float score=(intentSelf+intentOther)*0.5f - std::sqrt(d2)*0.01f;
                if(score>bestScore){bestScore=score;best=o;}
            });
            return best;
        };

        for(Creature* c:creatures) if(c) c->matingTouchThisFrame=false;

        // 1) Detecta pares com intencao mutua e cabeca realmente encostada.
        for(Creature* p:creatures){
            if(!p || p->dead() || !p->canReproduce(partnerCostScale)) continue;
            if(effectiveMateIntent(*p)<0.35f) continue;
            Creature* best=nullptr; float bestD2=1e30f;
            const float detectRange=std::max(28.0f,p->size*3.0f+cfg::tuning.matingContactExtraRadius*4.0f);
            creatureGrid.forCircle(p->pos.x,p->pos.y,detectRange,[&](Creature* o){
                if(!o || o==p || o->dead() || !o->canMateAsPartner()) return;
                if(o->matingPartnerId>=0 && o->matingPartnerId!=p->id) return;
                if(effectiveMateIntent(*o)<0.35f) return;
                const Vector2 hp=add(p->pos,mul(p->forward(),p->size*0.56f));
                const Vector2 ho=add(o->pos,mul(o->forward(),o->size*0.56f));
                const float req=std::max(1.2f,(p->size+o->size)*0.32f+cfg::tuning.matingContactExtraRadius+2.0f);
                const float d2=toroidalDist2(hp,ho);
                if(d2<=req*req && d2<bestD2){best=o;bestD2=d2;}
            });
            if(!best) continue;
            Creature* a=p; Creature* b=best;
            if(a->id>b->id) std::swap(a,b);
            if(p->id!=a->id) continue;
            const float t=std::max(a->matingContactTime,b->matingContactTime)+dt;
            a->matingPartnerId=b->id; b->matingPartnerId=a->id;
            a->matingContactTime=t; b->matingContactTime=t;
            a->matingTouchThisFrame=b->matingTouchThisFrame=true;
            a->matingSearchGrace=b->matingSearchGrace=0.0f;
            if(t>=fullQualityContact && a->canReproduce(partnerCostScale) && b->canMateAsPartner())
                tryCreatureReproduction(*a,*b,t);
        }

        // 2) Finaliza sessoes ao separar; sem sessao, segura um pouco a postura solo
        // quando existe parceiro promissor por perto. Isso faz o sexual ter prioridade
        // real sobre o solo, em vez de o bicho botar sozinho imediatamente.
        for(Creature* p:creatures){
            if(!p || p->dead()) continue;
            if(p->matingTouchThisFrame) continue;
            if(p->matingPartnerId>=0 && p->matingContactTime>0.0f){
                Creature* mate=creatureById(p->matingPartnerId);
                if(mate && !mate->dead() && p->id<mate->id){
                    const float contact=std::max(p->matingContactTime,mate->matingContactTime);
                    if(contact>=minContact && p->canReproduce(partnerCostScale) && mate->canMateAsPartner()){
                        if(tryCreatureReproduction(*p,*mate,contact)) continue;
                    }
                    p->matingPartnerId=mate->matingPartnerId=-1;
                    p->matingContactTime=mate->matingContactTime=0.0f;
                }else if(!mate || mate->dead()){
                    p->matingPartnerId=-1; p->matingContactTime=0.0f;
                }
                continue;
            }

            // O solo agora e FALLBACK, nao a primeira escolha. A procura usa o alcance
            // genetico do proprio bicho ampliado por um multiplicador global.
            if(!p->canReproduce(1.0f) || p->reproCooldown>0.0f){
                p->soloFallbackTimer=0.0f;
                p->matingSearchGrace=0.0f;
                continue;
            }
            const float geneticSearch=clampf(p->genome.reproduction.mateRange,cfg::tuning.mateRangeMin,cfg::tuning.mateRangeMax);
            const float searchRadius=std::min(0.45f*std::min(cfg::WORLD_W,cfg::WORLD_H),std::max(70.0f,geneticSearch*std::max(1.0f,cfg::tuning.matingSearchRangeScale)));
            Creature* candidate = p->canReproduce(partnerCostScale) ? findNearbyMateCandidate(*p,searchRadius) : nullptr;
            const float fallbackTarget=p->sexualHeritageDepth>0?cfg::tuning.sexualHeritageSoloFallbackSeconds:cfg::tuning.soloFallbackSeconds;
            if(candidate){
                p->matingPartnerId=candidate->id;
                // Com parceiro plausivel por perto, o relogio de desistir do sexo anda muito devagar.
                p->soloFallbackTimer+=dt*0.10f;
            }else{
                p->matingPartnerId=-1;
                p->soloFallbackTimer+=dt;
            }
            p->matingSearchGrace=clampf(p->soloFallbackTimer/std::max(0.01f,fallbackTarget),0.0f,1.0f);
            if(p->soloFallbackTimer+1e-6f<fallbackTarget) continue;
            if(trySoloReproduction(*p)) p->soloFallbackTimer=0.0f;
        }
    }

    // [SEC-REPRODUCTION] Reproducao vegetal
    Plant* choosePartner(Plant& mother) {
        if (mother.dead() || mother.biomassSize < std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,mother.adultSizeTarget()*0.45f)) return nullptr;
        Plant* best = nullptr;
        float bestScore = -1e30f;
        const float radius = std::max(0.01f,mother.partnerRadius());
        const float colorPref = clampf(mother.genes.colorMatePreference,-1.0f,1.0f);
        const float colorStrength = std::fabs(colorPref);

        plantGrid.forCircle(mother.pos.x, mother.pos.y, radius, [&](Plant* p){
            if (p == &mother || p->dead() || p->biomassSize < std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,p->adultSizeTarget()*0.45f)) return;
            const float d = std::sqrt(std::max(0.0f,toroidalDist2(mother.pos,p->pos)));
            const float proximity = 1.0f - clampf(d/radius,0.0f,1.0f);
            // matePreference 1 quer perto; 0 quer longe.
            const float spatialScore =
                mother.genes.matePreference*proximity +
                (1.0f-mother.genes.matePreference)*(1.0f-proximity);

            const float similarity = colorSimilarity(mother.genes.color,p->genes.color);
            const float desiredColor = colorPref >= 0.0f ? similarity : (1.0f-similarity);
            // Com preferencia cromatica zero, cor nao pesa. Nos extremos, pode competir
            // fortemente com a preferencia espacial e criar isolamento/recombinacao por cor.
            const float colorScore = (1.0f-colorStrength)*0.5f + colorStrength*desiredColor;
            const float score = spatialScore*cfg::tuning.plantMateSpatialWeight + colorScore*cfg::tuning.plantMateColorWeight + rf(-cfg::tuning.plantMateNoise,cfg::tuning.plantMateNoise);
            if (score > bestScore) { bestScore=score; best=p; }
        });
        return best;
    }

    Plant* tryPlantChild(Plant& mother) {
        if (mother.dead() || mother.biomassSize < std::max(cfg::PLANT_MIN_FRAGMENT_SIZE,mother.adultSizeTarget()*0.45f)) return nullptr;
        if (limits.maxEnabled && (int)plants.size() >= limits.maxPlants) return nullptr;
        Plant* partner = choosePartner(mother);
        PlantGenes genes = inheritPlantGenes(mother.genes, partner ? &partner->genes : nullptr);
        Vector2 center = mother.pos;
        if (partner) center = mul(add(mother.pos, partner->pos), 0.5f);
        for (int a = 0; a < std::max(1,(int)std::lround(cfg::tuning.plantReproAttempts)); ++a) {
            float angle = rf(0.0f, 2.0f * PI);
            float d = rf(cfg::tuning.plantChildMinDist, cfg::tuning.plantChildMaxDist);
            Vector2 p{center.x + std::cos(angle) * d, center.y + std::sin(angle) * d};
            if (plantPositionFree(p,cfg::tuning.plantMaxSize*genes.sizeScale)) return plantPool.create(p,genes);
        }
        return nullptr;
    }

    void updatePlants(float dt) {
        const size_t initial = plants.size();
        plantNewbornScratch.clear();
        for (size_t i = 0; i < initial; ++i) {
            const bool wantsChild = plants[i]->update(dt);
            plantGrid.update(plants[i]);
            if (wantsChild) {
                Plant* child=tryPlantChild(*plants[i]);
                if(child) plantNewbornScratch.push_back(child);
            }
        }
        for (Plant* child : plantNewbornScratch) {
            if(!child) continue;
            if(limits.maxEnabled&&(int)plants.size()>=limits.maxPlants){plantPool.destroy(child);continue;}
            if(!plantPositionFree(child->pos,child->adultSizeTarget())){plantPool.destroy(child);continue;}
            plants.push_back(child);
            plantGrid.insert(child);
            ++evo.plantBirths;
        }
        plantNewbornScratch.clear();
    }

    void plantSway() {
        // Planta continua 100% enraizada; so a IMAGEM recebe um deslocamento
        // temporario quando os bichos atravessam a copa, simulando mato sendo
        // empurrado/arrastado. O ponto central biologico da planta permanece no lugar.
        for(Plant* p:plants){
            if(!p) continue;
            p->pendingSway={0.0f,0.0f};
            p->swaySuppressedThisPass=false;
        }

        const float maxPlantRadius=std::max(
            cfg::tuning.plantRadiusMin,
            cfg::tuning.plantMaxSize*cfg::tuning.plantSizeScaleMax*cfg::tuning.plantRadiusScale);
        const float maxOffset=std::max(0.0f,cfg::tuning.plantSwayMaxOffset);

        for(Creature* up:creatures){
            if(!up || up->dead()) continue;
            Creature& c=*up;
            c.vegetationFrictionMult=1.0f;
            const float cr=c.radius();
            int overlaps=0;

            plantGrid.forCircle(c.pos.x,c.pos.y,cr+maxPlantRadius,[&](Plant* p){
                if(!p || p->dead()) return;
                const float pr=p->radius();
                const float required=cr+pr;
                Vector2 diff=toroidalDelta(c.pos,p->pos);
                const float d2=length2(diff);
                if(d2>required*required) return;
                ++overlaps;

                float d=std::sqrt(std::max(0.0001f,d2));
                Vector2 away = d>0.0001f ? mul(diff,1.0f/d) : c.forward();
                const float overlap = std::max(0.0f, required-d);
                const float normOverlap = clampf(overlap/std::max(0.001f,required),0.0f,1.0f);

                // Parte do deslocamento aponta para fora do corpo do bicho; outra parte
                // segue o sentido instantaneo do movimento para parecer que o mato esta
                // sendo varrido enquanto ele atravessa a planta.
                Vector2 motion = {c.speed*c.forward().x + c.lateralSpeed*c.right().x,
                                  c.speed*c.forward().y + c.lateralSpeed*c.right().y};
                const float motionLen2=length2(motion);
                if(motionLen2>0.0001f){
                    const float inv=1.0f/std::sqrt(motionLen2);
                    motion=mul(motion,inv);
                }else motion={0.0f,0.0f};

                const float basePush = (0.35f + 0.85f*normOverlap) * cfg::tuning.plantSwayVelocityScale;
                Vector2 impulse = add(mul(away, basePush * (2.1f + 1.4f*normOverlap)),
                                      mul(motion, basePush * 1.35f));
                p->pendingSway = add(p->pendingSway, impulse);
            });

            // Todas as plantas realmente tocadas contam para a resistencia.
            c.vegetationFrictionMult=1.0f +
                float(overlaps)*cfg::VEGETATION_FRICTION_PER_PLANT*cfg::tuning.vegetationFriction;
        }

        // Relaxa gradualmente de volta ao centro quando nao ha bichos passando.
        for(Plant* p:plants){
            if(!p) continue;
            p->visualOffset = add(mul(p->visualOffset,0.82f), p->pendingSway);
            const float m2=length2(p->visualOffset);
            if(m2>maxOffset*maxOffset && maxOffset>0.0f){
                const float m=std::sqrt(m2);
                p->visualOffset=mul(p->visualOffset,maxOffset/m);
            }
            if(length2(p->pendingSway)<0.00001f && length2(p->visualOffset)<0.0004f)
                p->visualOffset={0.0f,0.0f};
        }
    }

    void removeDeadPlants() {
        for (int i = (int)plants.size() - 1; i >= 0; --i) {
            if (!plants[i]->dead()) continue;
            Plant* dead=plants[i];
            if(dead->grabbedByCreatureId>=0) releaseCarrierById(dead->grabbedByCreatureId);
            plantGrid.remove(dead);
            if (i != (int)plants.size() - 1) std::swap(plants[i], plants.back());
            plants.pop_back();
            plantPool.destroy(dead);
        }
    }

    void processBites() {
        // [INVARIANT-PENDING-EGGS]
        // NAO limpar eggNewbornScratch aqui. updateMating() cria ovos nesta fila antes
        // de processBites(); ela funciona como transacao pendente e deve sobreviver ate
        // o bloco de commit no final desta funcao, onde cada ovo entra em eggs + eggGrid.
        // Limpar a fila aqui fazia o genitor pagar sangue/energia e registrar a postura,
        // mas o ovo desaparecia antes de existir no mundo.
        for (Creature* up : creatures) {
            Creature& c = *up;
            if(isManualControlled(c)){if(!manualBiteHeld)continue;}
            else{
                // v0.135: morder e uma acao neural evolutiva. Se o gene de output
                // desapareceu, o individuo literalmente perdeu essa acao.
                if(!brainOutputActive(c.genome.brain,cfg::BITE_INTENT_OUTPUT_INDEX))continue;
                if(c.brainOut[cfg::BITE_INTENT_OUTPUT_INDEX]<0.55f)continue;
            }
            if (!c.canBite()) continue;
            Vector2 bp = c.bitePoint();
            float br = c.biteRadius();
            Plant* bestPlant = nullptr;
            Creature* bestCreature = nullptr;
            Carcass* bestCarcass = nullptr;
            Egg* bestEgg = nullptr;
            float bestD = 1e30f;

            // v0.099: a mordida em planta usa uma zona de contato pequena ao redor
            // da ponta da boca, em vez do raio generico inteiro da mordida. O raio
            // generico representa uma boca aberta e era largo demais para comida:
            // uma planta que parecia longe podia ser escolhida apenas porque sua copa
            // encostava na grande esfera de busca.
            //
            // O desenho da planta e a colisao da mordida agora usam exatamente p->radius(),
            // portanto alterar "Raio planta x tamanho" na configuracao nao cria mais
            // divergencia entre o que aparece na tela e o que pode ser comido.
            const float plantBiteContactRadius = std::max(0.35f, br * 0.45f);
            const float maxPlantVisualRadius = std::max(
                cfg::tuning.plantRadiusMin,
                cfg::tuning.plantMaxSize * cfg::tuning.plantSizeScaleMax * cfg::tuning.plantRadiusScale);

            plantGrid.forCircle(bp.x, bp.y, plantBiteContactRadius + maxPlantVisualRadius, [&](Plant* p){
                if (!p || p->dead() || p->biomassSize <= 0.001f) return;

                const float plantR = p->radius();
                const float d = toroidalDist2(bp, p->pos);
                const bool coveringSelf = rootedPlantCoversCreature(c,*p);

                if(!coveringSelf){
                    // Planta que NAO cobre o proprio bicho continua precisando tocar
                    // fisicamente a pequena zona frontal da boca. Isso preserva a correcao
                    // que impediu comer plantas distantes/laterais.
                    const float req = plantBiteContactRadius + plantR;
                    if (d > req * req) return;

                    const Vector2 rel = toroidalDelta(c.pos,p->pos);
                    const float forwardCenter = dot(rel,c.forward());
                    if (forwardCenter + plantR < c.radius() * 0.80f) return;
                }
                // Se a copa cobre o bicho, ele esta fisicamente dentro da vegetacao e pode
                // comer essa propria planta, independentemente de o centro dela estar
                // exatamente na pequena esfera desenhada na ponta da boca.

                if (d < bestD) {
                    bestD = d; bestPlant = p; bestCreature = nullptr; bestCarcass = nullptr; bestEgg = nullptr;
                }
            });
            creatureGrid.forCircle(bp.x, bp.y, br + cfg::tuning.maxCreatureSize * 0.5f, [&](Creature* t){
                if (t == &c || t->health <= 0) return;
                float req = br + t->size * 0.5f; float d = toroidalDist2(bp, t->pos);
                if (d <= req * req && d < bestD) { bestD = d; bestCreature = t; bestPlant = nullptr; bestCarcass = nullptr; bestEgg = nullptr; }
            });
            carcassGrid.forCircle(bp.x, bp.y, br + cfg::tuning.maxCreatureSize * cfg::tuning.carcassSensorScale, [&](Carcass* k){
                if (k->finished()) return;
                float req = br + k->sensorRadius; float d = toroidalDist2(bp, k->pos);
                if (d <= req * req && d < bestD) { bestD = d; bestCarcass = k; bestPlant = nullptr; bestCreature = nullptr; bestEgg = nullptr; }
            });
            const float maxEggRadius = 0.50f*(cfg::tuning.eggRadiusBase + cfg::tuning.maxCreatureSize * cfg::tuning.eggRadiusSizeScale);
            eggGrid.forCircle(bp.x, bp.y, br + maxEggRadius, [&](Egg* egg){
                if (!egg || egg->destroyed()) return;
                const float req = br + egg->radius;
                const float d = toroidalDist2(bp, egg->pos);
                if (d <= req * req && d < bestD) {
                    bestD = d;
                    bestEgg = egg;
                    bestPlant = nullptr;
                    bestCreature = nullptr;
                    bestCarcass = nullptr;
                }
            });

            if (!bestPlant && !bestCreature && !bestCarcass && !bestEgg) continue;
            float energyBeforeBite = c.energy;
            c.registerBite();
            log(c.id, "Mordeu. Potencia " + f2(c.bitePowerCached) + "; custo " + f2(energyBeforeBite - c.energy) + " energia.");

            if (bestPlant) {
                ++perf.plantBiteActions;
                // A potencia da mordida ja combina forca, abertura, velocidade da boca,
                // tamanho corporal e idade. v0.100 deixa plantas usarem toda essa faixa,
                // sem cortar artificialmente potencias acima de 1.0.
                const float plantSizeBefore=bestPlant->size;
                const float plantBiomassBefore=bestPlant->biomassSize;
                float removed = bestPlant->consume(cfg::tuning.plantBiteBiomassBase *
                    clampf(c.bitePowerCached, 0.0f, cfg::tuning.bitePowerMax));
                const float similarity = colorSimilarity(c.genome.mouth.color, bestPlant->genes.color);
                const float potentialEnergy = bestPlant->energyFor(removed);
                const float efficiency = cfg::tuning.plantMinColorEfficiency +
                    (1.0f-cfg::tuning.plantMinColorEfficiency)*similarity;
                const float gain = potentialEnergy * efficiency;
                const float poisonThreshold = cfg::tuning.plantPoisonSimilarity;
                const float incompatibility = clampf((poisonThreshold-similarity)/poisonThreshold,0.0f,1.0f);
                const float biomassFraction = clampf(removed/cfg::tuning.plantMaxSize,0.0f,1.0f);
                const float poisonDamage = cfg::tuning.plantMaxPoisonDamage * biomassFraction *
                    std::pow(incompatibility,cfg::tuning.plantPoisonExponent);
                const float before = c.energy;
                const float hpBefore = c.health;
                c.energy = clampf(c.energy + gain, 0.0f, c.maxEnergy);
                const float actualEnergyGain = c.energy - before;
                if(removed>0.0f){
                    ++perf.plantFoodBites;
                    perf.plantBiomassConsumed += removed;
                    perf.plantEnergyGained += std::max(0.0f,actualEnergyGain);
                } else {
                    ++perf.plantZeroFoodBites;
                }
                c.recentEnergyGain=clampf(c.recentEnergyGain+actualEnergyGain*cfg::tuning.energyGainFeedbackScale,0.0f,1.0f);
                if(actualEnergyGain>0.001f){
                    const float energy10=clampf((actualEnergyGain/std::max(0.01f,c.maxEnergy))*10.0f,0.0f,2.0f);
                    const float usefulBite=clampf(actualEnergyGain/std::max(0.01f,c.maxEnergy*0.05f),0.0f,1.0f);
                    c.reinforce(cfg::tuning.reinforcementEnergy10Pct*energy10 + cfg::tuning.reinforcementPlantFood*usefulBite);
                    rememberEventAt(c,MemoryEventType::Food,actualEnergyGain/std::max(0.01f,c.maxEnergy),bestPlant->pos);
                }
                c.health = clampf(c.health - poisonDamage,0.0f,cfg::tuning.healthMax);
                if(hpBefore>0.0f && c.health<=0.0f && c.deathCause==DeathCause::Unknown) c.deathCause=DeathCause::Poison;
                const float eatenPct=plantBiomassBefore>0.0001f
                    ?clampf(removed/plantBiomassBefore,0.0f,1.0f)*100.0f:0.0f;
                log(c.id, "Comeu planta enraizada NO ALCANCE DA BOCA: biomassa " + f2(removed) +
                    " (" + f2(eatenPct) + "%), tamanho " + f2(plantSizeBefore) + " -> " + f2(bestPlant->size) +
                    ", afinidade " + f2(similarity*100.0f) + "%, energia +" + f2(c.energy-before) + ".");
                if (hpBefore-c.health > 0.01f)
                    log(c.id,"Planta causou " + f2(hpBefore-c.health) + " de dano por baixa afinidade de cor.");
            } else if (bestCarcass) {
                const float before = c.energy;
                const float rawGain = bestCarcass->bite(c.bitePowerCached);
                const float similarity = colorSimilarity(c.genome.mouth.color,cfg::CARCASS_COLOR);
                const float efficiency = cfg::tuning.carcassMinColorEfficiency +
                    (1.0f-cfg::tuning.carcassMinColorEfficiency)*similarity;
                const float gain = rawGain*efficiency;
                c.energy = clampf(c.energy + gain, 0.0f, c.maxEnergy);
                c.recentEnergyGain=clampf(c.recentEnergyGain+(c.energy-before)*cfg::tuning.energyGainFeedbackScale,0.0f,1.0f);
                if(c.energy-before>0.001f){
                    const float actualGain=c.energy-before;
                    const float energy10=clampf((actualGain/std::max(0.01f,c.maxEnergy))*10.0f,0.0f,2.0f);
                    const float usefulBite=clampf(actualGain/std::max(0.01f,c.maxEnergy*0.05f),0.0f,1.0f);
                    c.reinforce(cfg::tuning.reinforcementEnergy10Pct*energy10 + cfg::tuning.reinforcementMeatFood*usefulBite);
                    rememberEventAt(c,MemoryEventType::Food,actualGain/std::max(0.01f,c.maxEnergy),bestCarcass->pos);
                }
                log(c.id, "Comeu carne: afinidade " + f2(similarity*100.0f) +
                    "%, energia +" + f2(c.energy-before) + ", restam " +
                    f2(bestCarcass->fraction()*100.0f) + "%.");
            } else if (bestEgg) {
                float before = c.energy;
                const int eggParent = bestEgg->parentId;
                float gain = bestEgg->bite(c.bitePowerCached);
                c.energy = clampf(c.energy + gain, 0.0f, c.maxEnergy);
                c.recentEnergyGain=clampf(c.recentEnergyGain+(c.energy-before)*cfg::tuning.energyGainFeedbackScale,0.0f,1.0f);
                if(c.energy-before>0.001f){
                    const float actualGain=c.energy-before;
                    const float energy10=clampf((actualGain/std::max(0.01f,c.maxEnergy))*10.0f,0.0f,2.0f);
                    const float usefulBite=clampf(actualGain/std::max(0.01f,c.maxEnergy*0.05f),0.0f,1.0f);
                    c.reinforce(cfg::tuning.reinforcementEnergy10Pct*energy10 + cfg::tuning.reinforcementEggFood*usefulBite);
                    rememberEventAt(c,MemoryEventType::Food,actualGain/std::max(0.01f,c.maxEnergy),bestEgg->pos);
                }
                log(c.id,
                    "Comeu ovo: energia +" + f2(c.energy - before) +
                    ", recursos do ovo " + f2(bestEgg->resourceFraction() * 100.0f) + "%.");
                if (eggParent >= 0)
                    log(eggParent,
                        "Seu ovo foi mordido por #" + std::to_string(c.id) +
                        "; restam " + f2(bestEgg->resourceFraction() * 100.0f) + "% dos recursos.");
            } else if (bestCreature) {
                const float similarity = colorSimilarity(c.displayColor,bestCreature->displayColor);
                float damage = bestCreature->receiveDamage(cfg::tuning.creatureBiteDamageBase * c.bitePowerCached, DeathCause::Attack);
                if(damage>0.001f){
                    const float damage10=clampf((damage/std::max(0.01f,cfg::tuning.healthMax))*10.0f,0.0f,2.0f);
                    c.reinforce(cfg::tuning.reinforcementAttack10Pct*damage10);
                    bestCreature->reinforce(cfg::tuning.reinforcementBittenEvent);
                    rememberEventAt(c,MemoryEventType::AttackHit,damage/std::max(0.01f,cfg::tuning.healthMax),bestCreature->pos);
                    rememberEventAt(*bestCreature,MemoryEventType::Attacked,damage/std::max(0.01f,cfg::tuning.healthMax),c.pos);
                }
                log(c.id, "Mordeu bicho #" + std::to_string(bestCreature->id) +
                    ": dano " + f2(damage) + ", afinidade de cor " + f2(similarity*100.0f) + "%.");
                log(bestCreature->id, "Foi mordido por #" + std::to_string(c.id) + ": perdeu " + f2(damage) + " de vida.");
            }
        }
        removeDeadPlants();

        // Ovos destruidos por mordidas somem antes de inserir novos ovos.
        for (int i = (int)eggs.size() - 1; i >= 0; --i) {
            if (!eggs[i]->destroyed()) continue;
            log(eggs[i]->parentId, "Ovo destruido antes de eclodir.");
            ++evo.eggsDestroyed;
            Egg* deadEgg=eggs[i];
            eggGrid.remove(deadEgg);
            if (i != (int)eggs.size() - 1) std::swap(eggs[i], eggs.back());
            eggs.pop_back();
            eggPool.destroy(deadEgg);
        }

        // Commit atomico dos ovos criados neste subpasso reprodutivo. So depois de
        // entrar em eggs + eggGrid a fila temporaria pode ser limpa.
        for (Egg* egg : eggNewbornScratch) {
            if(!egg) continue;
            eggs.push_back(egg);
            eggGrid.insert(egg);
        }
        eggNewbornScratch.clear();
    }

    void processDeaths() {
        for (int i = (int)creatures.size() - 1; i >= 0; --i) {
            Creature* c = creatures[i];
            if (!c->dead()) continue;

            if(manualControlCreatureId==c->id) stopManualControl();
            if (isWatching(c->id)) {
                if (watch.acc.elapsed > 0.05f) {
                    log(c->id, "Periodo final incompleto antes da morte.");
                    watch.acc = {};
                }
                if(c->deathCause==DeathCause::NaturalAge)
                    log(c->id, "Morreu de velhice/falha biologica. Idade " + f2(c->age) + "s; longevidade DNA " + f2(c->genome.development.longevity) + "s.");
                else if(c->deathCause==DeathCause::Starvation)
                    log(c->id, "Morreu de fome. Energia final " + f2(c->energy) + "/" + f2(c->maxEnergy) + ".");
                else if(c->deathCause==DeathCause::Attack)
                    log(c->id, "Morreu por ataque de outro bicho.");
                else if(c->deathCause==DeathCause::Poison)
                    log(c->id, "Morreu por efeito toxico de alimento vegetal.");
                else
                    log(c->id, "Morreu. Energia final " + f2(c->energy) + "/" + f2(c->maxEnergy) + ", vida " + f2(c->health) + ".");
                watch.alive = false; watch.hasFinal = true;
                watch.final = {c->id, c->generation, c->parentId, c->energy, c->maxEnergy, c->health, c->size, c->pos};
                selectedId = -1;
            }

            evaluateInverseTerminalRecords(*c);
            ++evo.deaths;
            if(c->naturalDeath) ++evo.naturalAgeDeaths;
            if(c->deathCause==DeathCause::Starvation) ++evo.starvationDeaths;
            if(c->deathCause==DeathCause::Attack) ++evo.attackDeaths;

            // Guarda a linhagem ANTES de destruir o objeto. Esse DNA e a fonte
            // da reposicao do minimo, sempre via mutacao + ovo + geracao seguinte.
            minimumLineageSeed.valid = true;
            minimumLineageSeed.genome = c->genome;
            minimumLineageSeed.generation = c->generation;
            minimumLineageSeed.parentId = c->id;
            minimumLineageSeed.origin = c->pos;
            minimumLineageSeed.parentSize = c->size;
            minimumLineageSeed.learnedWeights = c->learnedWeights;
            minimumLineageSeed.memoryRegisters = c->memoryRegisters;
            minimumLineageSeed.sexualHeritageDepth = c->sexualHeritageDepth;

            if(c->grabbedByCreatureId>=0) releaseCarrierById(c->grabbedByCreatureId);
            if(c->grabbedKind!=GrabKind::None) releaseGrab(*c);
            markGenealogyDeath(*c);
            creatureGrid.remove(c);
            unindexCreature(c->id);
            if (i != (int)creatures.size() - 1) std::swap(creatures[i], creatures.back());
            creatures.pop_back();

            // Se esta morte deixou os VIVOS abaixo do minimo, esta vaga pertence
            // a linhagem do morto. Se o ovo for destruido, a divida continua ativa.
            if ((int)creatures.size() < limits.minCreatures) {
                ++minimumLineageDebt;
                if (Egg* replacement = spawnMinimumReplacementEgg(minimumLineageSeed)) {
                    log(c->id,
                        "Minimo populacional: deixou descendente em ovo, geracao " +
                        std::to_string(c->generation+1) + ".");
                    (void)replacement;
                }
            }

            Carcass* rawK=carcassPool.create(c->pos,c->size);
            carcasses.push_back(rawK);
            carcassGrid.insert(rawK);
            creaturePool.destroy(c);
        }
    }

    void updateCarcasses(float dt) {
        for (int i = (int)carcasses.size() - 1; i >= 0; --i) {
            carcasses[i]->update(dt);
            carcassGrid.update(carcasses[i]);
            if (carcasses[i]->finished()) {
                Carcass* done=carcasses[i];
                if(done->grabbedByCreatureId>=0) releaseCarrierById(done->grabbedByCreatureId);
                carcassGrid.remove(done);
                if (i != (int)carcasses.size() - 1) std::swap(carcasses[i], carcasses.back());
                carcasses.pop_back();
                carcassPool.destroy(done);
            }
        }
    }


    void resolvePhysicalBodies() {
        using PhysClock=std::chrono::high_resolution_clock;
        const auto physMs=[](auto a,auto b){return std::chrono::duration<double,std::milli>(b-a).count();};
        const auto other0=PhysClock::now();
        for(Creature* c:creatures) if(c) c->contactPressure.fill(0.0f);
        physicalScratch.clear();
        perf.physicsBodyCreatures=perf.physicsBodyPlants=perf.physicsBodyCarcasses=perf.physicsBodyEggs=0;
        perf.physicsCandidatePairs=perf.physicsContactPairs=perf.physicsSkippedPlantPlantPairs=0;
        perf.physicsCreatureCreatureContacts=perf.physicsCreaturePlantContacts=perf.physicsOtherContacts=0;
        for(Creature* c:creatures) if(c&&!c->dead()){
            physicalScratch.push_back({PhysicalKind::Creature,c,&c->pos,c->worldVelocity(),c->radius(),1.0f/c->mass()});
            ++perf.physicsBodyCreatures;
        }
        for(Carcass* k:carcasses)if(k&&!k->finished()){physicalScratch.push_back({PhysicalKind::Carcass,k,&k->pos,k->velocity,k->radius(),1.0f/k->mass()});++perf.physicsBodyCarcasses;}
        // v0.098: plantas nao participam da fisica em nenhuma forma.
        perf.physicsBodyPlants=0;
        for(Egg* e:eggs)if(e&&!e->destroyed()){physicalScratch.push_back({PhysicalKind::Egg,e,&e->pos,e->velocity,e->radius,1.0f/e->mass()});++perf.physicsBodyEggs;}

        const int physCols=std::max(1,(int)std::ceil(cfg::WORLD_W/cfg::CELL));
        const int physRows=std::max(1,(int)std::ceil(cfg::WORLD_H/cfg::CELL));
        const int physCellCount=physCols*physRows;
        if(physicsBucketCols!=physCols||physicsBucketRows!=physRows||(int)physicsBuckets.size()!=physCellCount){
            physicsBucketCols=physCols;physicsBucketRows=physRows;
            physicsBuckets.clear();physicsBuckets.resize((std::size_t)physCellCount);
            physicsBucketActiveSlot.assign((std::size_t)physCellCount,-1);
            activePhysicsBuckets.clear();activePhysicsBuckets.reserve(std::min(physCellCount,4096));
        }
        auto clearBuckets=[&]{
            for(int idx:activePhysicsBuckets){physicsBuckets[(std::size_t)idx].clear();physicsBucketActiveSlot[(std::size_t)idx]=-1;}
            activePhysicsBuckets.clear();
        };
        auto fillBuckets=[&]{
            clearBuckets();
            for(int i=0;i<(int)physicalScratch.size();++i){
                const Vector2 p=wrappedPoint(*physicalScratch[(std::size_t)i].pos);
                const int cx=std::clamp((int)std::floor(p.x/cfg::CELL),0,physCols-1);
                const int cy=std::clamp((int)std::floor(p.y/cfg::CELL),0,physRows-1);
                const int idx=cy*physCols+cx;
                auto& bucket=physicsBuckets[(std::size_t)idx];
                if(bucket.empty()){physicsBucketActiveSlot[(std::size_t)idx]=(int)activePhysicsBuckets.size();activePhysicsBuckets.push_back(idx);}
                bucket.push_back(i);
            }
        };
        auto solvePair=[&](int ia,int ib,bool emitImpact){
            ++perf.physicsCandidatePairs;
            PhysicalProxy& a=physicalScratch[ia]; PhysicalProxy& b=physicalScratch[ib];
            auto carrierHolds=[&](PhysicalProxy& carrier,PhysicalProxy& target){
                if(carrier.kind!=PhysicalKind::Creature)return false;
                Creature* c=static_cast<Creature*>(carrier.object); if(!c)return false;
                if(target.kind==PhysicalKind::Creature)return c->grabbedKind==GrabKind::Creature && c->grabbedCreature==target.object;
                if(target.kind==PhysicalKind::Carcass)return c->grabbedKind==GrabKind::Carcass && c->grabbedCarcass==target.object;
                if(target.kind==PhysicalKind::Plant)return c->grabbedKind==GrabKind::Plant && c->grabbedPlant==target.object;
                return false;
            };
            if(carrierHolds(a,b)||carrierHolds(b,a)) return; // a mao segura a carga; nao colide com o proprio portador
            Vector2 delta=toroidalDelta(*a.pos,*b.pos);
            const float d2=length2(delta);
            const float minDist=a.radius+b.radius;
            // Broadphase ja reduziu bastante os pares, mas ainda ha muitos corpos na mesma
            // celula/vizinhas. Antes calculavamos sqrt/normal para TODO par candidato e so
            // depois solveCircleCollision descobria que a maioria nem encostava. Fazemos o
            // teste quadratico barato primeiro; sqrt/impulso/feedback so existem em contato real.
            if(d2>=minDist*minDist) return;
            ++perf.physicsContactPairs;
            const bool aCreature=a.kind==PhysicalKind::Creature,bCreature=b.kind==PhysicalKind::Creature;
            const bool aPlant=a.kind==PhysicalKind::Plant,bPlant=b.kind==PhysicalKind::Plant;
            if(aCreature&&bCreature)++perf.physicsCreatureCreatureContacts;
            else if((aCreature&&bPlant)||(bCreature&&aPlant))++perf.physicsCreaturePlantContacts;
            else ++perf.physicsOtherContacts;
            Vector2 normal=d2>1e-10f?mul(delta,1.0f/std::sqrt(d2)):Vector2{1.0f,0.0f};
            const float closing=std::max(0.0f,-dot(sub(b.velocity,a.velocity),normal));
            const bool hit=solveCircleCollision(*a.pos,a.velocity,a.radius,a.invMass,*b.pos,b.velocity,b.radius,b.invMass);
            if(emitImpact && hit){
                const float massA=a.invMass>1e-8f?1.0f/a.invMass:1e6f;
                const float massB=b.invMass>1e-8f?1.0f/b.invMass:1e6f;
                const float effectiveMass=(massA*massB)/std::max(1e-6f,massA+massB);
                const float pressure=clampf((0.08f+closing*std::sqrt(std::max(0.01f,effectiveMass))*cfg::tuning.tactilePressureScale),0.0f,1.0f);
                auto touch=[&](PhysicalProxy& body,Vector2 toward){
                    if(body.kind!=PhysicalKind::Creature)return;
                    Creature* c=static_cast<Creature*>(body.object); if(!c)return;
                    const float f=dot(toward,c->forward()),r=dot(toward,c->right());
                    int sector=0;
                    if(std::fabs(f)>=std::fabs(r)) sector=f>=0?0:2; else sector=r>=0?1:3;
                    c->contactPressure[sector]=clampf(c->contactPressure[sector]+pressure,0.0f,1.0f);
                    c->recentImpact=std::max(c->recentImpact,clampf(closing*cfg::tuning.impactFeedbackScale,0.0f,1.0f));
                };
                touch(a,normal); touch(b,mul(normal,-1.0f));
            }
        };
        const auto other1=PhysClock::now();
        double bucketMsTotal=0.0,pairMsTotal=0.0;
        constexpr int nb[4][2]={{1,0},{0,1},{1,1},{-1,1}};
        for(int iteration=0;iteration<(int)std::lround(cfg::tuning.physicsSolverIterations);++iteration){
            const auto bucket0=PhysClock::now();
            fillBuckets();
            bucketMsTotal+=physMs(bucket0,PhysClock::now());
            const auto pair0=PhysClock::now();
            const bool emitImpact=(iteration==0);
            for(int cellIdx:activePhysicsBuckets){
                const int cx=cellIdx%physCols,cy=cellIdx/physCols;
                const auto& local=physicsBuckets[(std::size_t)cellIdx];
                for(int i=0;i<(int)local.size();++i)for(int j=i+1;j<(int)local.size();++j)solvePair(local[(std::size_t)i],local[(std::size_t)j],emitImpact);
                for(const auto& off:nb){
                    const int nx=(cx+off[0]+physCols)%physCols;
                    const int ny=(cy+off[1]+physRows)%physRows;
                    const int otherIdx=ny*physCols+nx;
                    if(otherIdx==cellIdx)continue;
                    const auto& other=physicsBuckets[(std::size_t)otherIdx];
                    if(other.empty())continue;
                    for(int ia:local)for(int ib:other)solvePair(ia,ib,emitImpact);
                }
            }
            pairMsTotal+=physMs(pair0,PhysClock::now());
        }
        const auto cleanup0=PhysClock::now();
        clearBuckets();

        for(PhysicalProxy& body:physicalScratch){
            switch(body.kind){
                case PhysicalKind::Creature:{auto* c=static_cast<Creature*>(body.object);c->setWorldVelocity(body.velocity);c->wrap();creatureGrid.update(c);break;}
                case PhysicalKind::Carcass:{auto* k=static_cast<Carcass*>(body.object);k->velocity=clampMagnitude(body.velocity,cfg::PHYSICS_MAX_PUSH_SPEED * cfg::tuning.maxPushSpeed);resolveCircleWorldBarrier(k->pos,k->velocity,k->radius());carcassGrid.update(k);break;}
                case PhysicalKind::Plant: break; // plantas soltas nao entram no solver dinamico
                case PhysicalKind::Egg:{auto* e=static_cast<Egg*>(body.object);e->velocity=clampMagnitude(body.velocity,cfg::PHYSICS_MAX_PUSH_SPEED * cfg::tuning.maxPushSpeed);resolveCircleWorldBarrier(e->pos,e->velocity,e->radius);eggGrid.update(e);break;}
            }
        }

        // v0.098: plantas sao totalmente atravessaveis e nao entram no solver.
        perf.physicsStaticPlants.sample(0.0);
        const auto cleanup1=PhysClock::now();
        perf.physicsBuckets.sample(bucketMsTotal);
        perf.physicsPairs.sample(pairMsTotal);
        perf.physicsOther.sample(physMs(other0,other1)+physMs(cleanup0,cleanup1));
    }

    float deterministicEggRoll(const Egg& egg,std::uint32_t salt) const {
        // Resultado estavel mesmo se salvar/carregar no meio da incubacao.
        std::uint32_t h=2166136261u ^ salt;
        auto mix=[&](std::uint32_t v){h^=v;h*=16777619u;h^=h>>13;h*=0x85ebca6bu;};
        mix((std::uint32_t)(egg.parentId+1000003));
        mix((std::uint32_t)(egg.secondParentId+2000003));
        mix((std::uint32_t)egg.generation);
        mix((std::uint32_t)std::lround(egg.pos.x*32.0f));
        mix((std::uint32_t)std::lround(egg.pos.y*32.0f));
        mix((std::uint32_t)std::lround(egg.initialEnergy*100.0f));
        h^=h>>16; h*=0x7feb352du; h^=h>>15; h*=0x846ca68bu; h^=h>>16;
        return float(h & 0x00ffffffu)/float(0x01000000u);
    }

    void updateEggs(float dt) {
        creatureNewbornScratch.clear();

        for (int i = (int)eggs.size() - 1; i >= 0; --i) {
            Egg* egg = eggs[i];
            egg->update(dt);
            eggGrid.update(egg);

            if (egg->destroyed()) {
                log(egg->parentId, "Ovo ficou sem recursos e morreu.");
                ++evo.eggsDestroyed;
                eggGrid.remove(egg);
                if (i != (int)eggs.size() - 1) std::swap(eggs[i], eggs.back());
                eggs.pop_back();
                eggPool.destroy(egg);
                continue;
            }

            if (!egg->readyToHatch()) continue;

            const bool partnered=egg->secondParentId>=0 && egg->secondParentId!=egg->parentId;
            const bool protectedReplacement=egg->minimumReplacement;
            // Ovo solo: 10% falham biologicamente e viram carne ao fim da incubacao.
            if(!partnered && !protectedReplacement &&
               deterministicEggRoll(*egg,0x51A0u)<cfg::tuning.soloEggFailureChance){
                const Vector2 meatPos=egg->pos;
                const float meatSize=egg->childSize;
                const float eggFood=std::max(0.1f,egg->edibleValue());
                Carcass* meat=addCarcass(meatPos,meatSize);
                if(meat){
                    meat->initialEnergy=meat->energy=eggFood;
                    meat->decayPerSecond=meat->duration>0.0f?meat->initialEnergy/meat->duration:meat->initialEnergy;
                }
                log(egg->parentId,"Ovo SOLO nao desenvolveu bicho e virou carne (regra 10% falha? nao, aqui falhou no sorteio biologico restante)." );
                ++evo.eggsDestroyed;
                eggGrid.remove(egg);
                if(i!=(int)eggs.size()-1)std::swap(eggs[i],eggs.back());
                eggs.pop_back();
                eggPool.destroy(egg);
                continue;
            }

            int available=10;
            if(limits.maxEnabled){
                available=limits.maxCreatures-(int)creatures.size()-(int)creatureNewbornScratch.size();
                if(available<=0) continue;
            }

            const int planned=std::max(1,egg->plannedBirthCount);
            const bool twins=partnered && planned>=2;
            const int birthCount=std::min(planned,available);
            const float birthEnergyBase=egg->childEnergyBase();
            const float birthHealth=egg->childHealth();
            const int parentId=egg->parentId;
            const int secondParentId=egg->secondParentId;
            const int generation=egg->generation;
            const Vector2 basePos=egg->pos;



            for(int born=0;born<birthCount;++born){
                const int childId=nextCreatureId++;
                CreatureGenome childGenome=egg->genome; // gemeos sao geneticamente identicos neste ovo
                Vector2 childPos=basePos;
                if(birthCount>1){
                    const float childR=std::max(0.5f,egg->childSize*0.5f);
                    const float ring=std::max(childR*1.15f,egg->radius+childR*0.65f);
                    const float a=2.0f*PI*(float)born/(float)birthCount;
                    childPos={basePos.x+std::cos(a)*ring,basePos.y+std::sin(a)*ring};
                }
                constrainCircleToWorld(childPos,std::max(0.5f,egg->childSize*0.5f));
                Creature* child=creaturePool.create(
                    childId,childPos,std::move(childGenome),generation,parentId,birthEnergyBase);
                child->health=birthHealth;
                child->secondParentId=secondParentId;
                child->sexualHeritageDepth=egg->sexualHeritageDepth;
                if(egg->hasInheritedLearning && egg->inheritedLearnedWeights.size()==child->genome.brain.connections.size()){
                    child->learnedWeights=egg->inheritedLearnedWeights;
                    child->memoryRegisters=egg->inheritedMemoryRegisters;
                    if(partnered && egg->inheritedLongMemoryCount>0){
                        child->longMemory=egg->inheritedLongMemory;
                        child->longMemoryHead=egg->inheritedLongMemoryHead%cfg::LONG_MEMORY_SLOTS;
                        child->longMemoryCount=(std::uint8_t)std::min<int>(egg->inheritedLongMemoryCount,cfg::LONG_MEMORY_SLOTS);
                    }
                    child->eligibilityTraces.assign(child->genome.brain.connections.size(),0.0f);
                    child->lastPlasticReward=0.0f;
                    child->plasticUpdates=0;
                }

                log(parentId,
                    std::string(partnered?(birthCount>=3?"Ovo fecundado eclodiu TRIPLO: filho #":(twins?"Ovo fecundado eclodiu GEMEO: filho #":"Ovo fecundado eclodiu: filho #")):"Ovo SOLO eclodiu: filho #")+
                    std::to_string(childId)+", energia "+f2(child->energy)+"/"+f2(child->maxEnergy)+
                    ", vida "+f2(child->health)+(egg->hasInheritedLearning?(partnered?", conhecimento dos dois pais herdado.":", aprendizado herdado."):"."));

                creatureNewbornScratch.push_back(child);
                ++evo.naturalBirths;

                // Recompensa extrema somente quando um FILHO realmente nasce.
                auto rewardParent=[&](int id){
                    if(Creature* parent=creatureById(id);parent && !parent->dead())
                        parent->reinforce(cfg::tuning.reinforcementReproduction);
                };
                rewardParent(parentId);
                if(secondParentId>=0 && secondParentId!=parentId) rewardParent(secondParentId);
            }

            if(egg->minimumReplacement && minimumLineageDebt>0) --minimumLineageDebt;
            ++evo.eggsHatched;

            eggGrid.remove(egg);
            if(i!=(int)eggs.size()-1)std::swap(eggs[i],eggs.back());
            eggs.pop_back();
            eggPool.destroy(egg);
        }

        for (Creature* child : creatureNewbornScratch) {
            if (!child) continue;
            indexCreature(child);
            creatures.push_back(child);
            creatureGrid.insert(child);
            registerGenealogy(child);
        }
        creatureNewbornScratch.clear();
    }

    void update(float dt) {
        using Clock=std::chrono::high_resolution_clock;
        const auto ms=[](auto a,auto b){return std::chrono::duration<double,std::milli>(b-a).count();};
        const auto t0=Clock::now(); simTime+=dt;
        brainExecutor.beginPerfWindow();

        auto a=Clock::now();auto b=a;
        a=Clock::now();updateEmotions(dt);b=Clock::now();perf.emotions.sample(ms(a,b));
        a=Clock::now();updateBrains(dt);b=Clock::now();perf.brains.sample(ms(a,b));
        a=Clock::now();updateGrabIntents();b=Clock::now();perf.grabIntent.sample(ms(a,b));

        // Resistencia vegetal e calculada na posicao atual ANTES do movimento.
        // Nao move a planta e nao resolve colisao; apenas prepara o multiplicador de arrasto do bicho.
        plantSwayAccumulator=0.0f;
        a=Clock::now();plantSway();b=Clock::now();perf.plantSway.sample(ms(a,b));

        a=Clock::now();for(Creature* c:creatures){ updateCreature(*c,dt); updateRecordTracking(*c,dt); }b=Clock::now();perf.movement.sample(ms(a,b));
        a=Clock::now();applyGrabConstraints();b=Clock::now();perf.grabConstraints.sample(ms(a,b));
        updateMating(dt);

        a=Clock::now();updatePlants(dt);b=Clock::now();perf.plants.sample(ms(a,b));
        a=Clock::now();processBites();b=Clock::now();perf.bites.sample(ms(a,b));
        a=Clock::now();processDeaths();b=Clock::now();perf.deaths.sample(ms(a,b));
        a=Clock::now();updateCarcasses(dt);b=Clock::now();perf.carcasses.sample(ms(a,b));
        a=Clock::now();updateEggs(dt);b=Clock::now();perf.eggs.sample(ms(a,b));

        a=Clock::now();resolvePhysicalBodies();b=Clock::now();perf.physics.sample(ms(a,b));
        a=Clock::now();applyGrabConstraints();b=Clock::now();perf.grabConstraints.sample(ms(a,b));
        a=Clock::now();ensureMinimums();b=Clock::now();perf.minimums.sample(ms(a,b));
        updateSensors(dt,false);

        // Inspirado no StableIndexVector: manter a iteracao densa/local sem sacrificar
        // ponteiros estaveis. A ordenacao e rara e so mexe nas listas de visita.
        localitySortAccumulator+=dt;
        if(localitySortAccumulator>=10.0f){localitySortAccumulator=std::fmod(localitySortAccumulator,10.0f);optimizeEntityIterationLocality();}

        statsAccumulator+=dt;
        if(statsAccumulator>=0.50f){a=Clock::now();statsAccumulator=std::fmod(statsAccumulator,0.50f);refreshEvolutionStats();b=Clock::now();perf.stats.sample(ms(a,b));}
        historyAccumulator+=dt;
        if(historyAccumulator>=cfg::tuning.evolutionHistoryInterval){a=Clock::now();historyAccumulator=std::fmod(historyAccumulator,cfg::tuning.evolutionHistoryInterval);recordEvolutionHistory();b=Clock::now();perf.history.sample(ms(a,b));}

        const auto threadPerf=brainExecutor.perfWindow();
        perf.parallelDispatch.sample(threadPerf.setupMs);
        perf.parallelResidualWait.sample(threadPerf.residualWaitMs);
        perf.parallelCallsLastSubstep=threadPerf.calls;

        const auto t1=Clock::now();
        perf.simSubstep.sample(ms(t0,t1));
        lastUpdateMs=perf.simSubstep.avgMs;
        perf.brainMs=perf.emotions.avgMs+perf.brains.avgMs+perf.grabIntent.avgMs;
        perf.movementMs=perf.movement.avgMs+perf.grabConstraints.avgMs;
        perf.creaturesMs=perf.brainMs+perf.movementMs;
        perf.plantsMs=perf.plants.avgMs+perf.plantSway.avgMs;
        perf.bitesMs=perf.bites.avgMs;
        perf.ecologyMs=perf.deaths.avgMs+perf.carcasses.avgMs+perf.eggs.avgMs;
        perf.physicsMs=perf.physics.avgMs;
    }

    static std::size_t genomeHeapBytes(const CreatureGenome& g){
        std::size_t b=0;
        b+=g.brain.hiddenNodes.capacity()*sizeof(HiddenNodeGene);
        b+=g.brain.outputBiases.capacity()*sizeof(float);
        b+=g.brain.connections.capacity()*sizeof(NeuralConnectionGene);
        b+=g.brain.activeInputs.capacity()*sizeof(uint8_t);
        b+=g.brain.activeOutputs.capacity()*sizeof(uint8_t);
        b+=g.emotions.emotions.capacity()*sizeof(EmotionGene);
        for(const auto& e:g.emotions.emotions)b+=e.triggers.capacity()*sizeof(EmotionTriggerGene);
        return b;
    }

    static std::size_t compiledBrainHeapBytes(const CompiledBrain& c){
        std::size_t b=0;
        auto add=[&](const auto& vv){b+=vv.capacity()*sizeof(std::vector<CompiledConnection>);for(const auto& v:vv)b+=v.capacity()*sizeof(CompiledConnection);};
        add(c.hiddenIncoming);add(c.hiddenGateIncoming);add(c.hiddenModIncoming);add(c.hiddenShuntIncoming);
        add(c.outputIncoming);add(c.outputGateIncoming);add(c.outputModIncoming);add(c.outputShuntIncoming);
        for(const auto& v:c.hiddenByLayer)b+=v.capacity()*sizeof(int);
        return b;
    }

    std::size_t estimatedEntityPoolBytes() const{
        return creaturePool.allocatedBytes()+plantPool.allocatedBytes()+carcassPool.allocatedBytes()+eggPool.allocatedBytes()+
            creatures.capacity()*sizeof(Creature*)+plants.capacity()*sizeof(Plant*)+carcasses.capacity()*sizeof(Carcass*)+eggs.capacity()*sizeof(Egg*);
    }

    std::size_t estimatedNeuralBytes() const{
        std::size_t b=0;
        for(const Creature* c:creatures){
            if(!c)continue;
            b+=genomeHeapBytes(c->genome)+compiledBrainHeapBytes(c->compiled)+c->hidden.capacity()*sizeof(float)+c->learnedWeights.capacity()*sizeof(float)+c->eligibilityTraces.capacity()*sizeof(float);
        }
        for(const Egg* e:eggs)if(e)b+=genomeHeapBytes(e->genome);
        if(minimumLineageSeed.valid)b+=genomeHeapBytes(minimumLineageSeed.genome);
        return b;
    }

    std::size_t estimatedSensorIndexBytes() const{
        std::size_t b=creatureGrid.allocatedBytes()+plantGrid.allocatedBytes()+carcassGrid.allocatedBytes()+eggGrid.allocatedBytes();
        for(const Creature* c:creatures){if(!c)continue;b+=c->rayOffsets.capacity()*sizeof(float)+c->raySin.capacity()*sizeof(float)+c->rayCos.capacity()*sizeof(float)+c->rays.capacity()*sizeof(RayReading);}
        b+=gpuVisionCells.capacity()*sizeof(GpuVisionCellRange)+gpuVisionObjects.capacity()*sizeof(GpuVisionObject)+gpuVisionRays.capacity()*sizeof(GpuVisionRay)+gpuVisionResults.capacity()*sizeof(GpuVisionResult)+gpuVisionCellCounts.capacity()*sizeof(std::uint32_t)+gpuVisionCellCursor.capacity()*sizeof(std::uint32_t)+gpuVision.estimatedGpuBytes();
        return b;
    }

    std::size_t estimatedPhysicsBytes() const{
        std::size_t b=physicalScratch.capacity()*sizeof(PhysicalProxy)+physicsBuckets.capacity()*sizeof(std::vector<int>)+
            activePhysicsBuckets.capacity()*sizeof(int)+physicsBucketActiveSlot.capacity()*sizeof(int);
        for(int idx:activePhysicsBuckets)if(idx>=0&&idx<(int)physicsBuckets.size())b+=physicsBuckets[(std::size_t)idx].capacity()*sizeof(int);
        return b;
    }

    std::size_t estimatedHistoryBytes() const{
        std::size_t b=evolutionHistory.capacity()*sizeof(EvolutionHistoryPoint)+genealogy.capacity()*sizeof(GenealogyRecord)+notes.capacity()*sizeof(NoteTask);
        for(const auto& n:notes)b+=n.text.capacity()+1;
        b+=byId.capacity()*sizeof(Creature*);
        b+=genealogyIndex.size()*(sizeof(int)+sizeof(std::size_t)+sizeof(void*)*2);
        return b;
    }

    std::size_t estimatedKnownBytes() const{
        return estimatedEntityPoolBytes()+estimatedNeuralBytes()+estimatedSensorIndexBytes()+estimatedPhysicsBytes()+estimatedHistoryBytes()+GetRenderCpuBufferBytes()+GetRenderGpuVertexBufferBytes();
    }

    Creature* hitCreature(Vector2 world, float screenHitRadius, float zoom) const {
        Creature* best = nullptr; float bestD = 1e30f;
        float broad = std::max(cfg::tuning.maxCreatureSize * 0.5f, screenHitRadius / zoom);
        creatureGrid.forCircle(world.x, world.y, broad, [&](Creature* c){
            float radius = std::max(c->size * 0.5f, screenHitRadius / zoom);
            float d = dist2(world, c->pos);
            if (d <= radius * radius && d < bestD) { best = c; bestD = d; }
        });
        return best;
    }


    Plant* hitPlant(Vector2 world, float screenHitRadius, float zoom) const {
        Plant* best = nullptr; float bestD = 1e30f;
        const float screenR = screenHitRadius / std::max(0.001f, zoom);
        const float broad = std::max(cfg::tuning.plantMaxSize * cfg::tuning.plantSizeScaleMax * 0.5f, screenR);
        plantGrid.forCircle(world.x, world.y, broad, [&](Plant* p){
            if (!p || p->dead()) return;
            const float radius = std::max(p->radius(), screenR);
            const float d = dist2(world, p->pos);
            if (d <= radius * radius && d < bestD) { best = p; bestD = d; }
        });
        return best;
    }

    Carcass* hitCarcass(Vector2 world, float screenHitRadius, float zoom) const {
        Carcass* best = nullptr; float bestD = 1e30f;
        const float screenR = screenHitRadius / std::max(0.001f, zoom);
        const float broad = std::max(cfg::tuning.maxCreatureSize * cfg::tuning.carcassVisualScale, screenR);
        carcassGrid.forCircle(world.x, world.y, broad, [&](Carcass* k){
            if (!k || k->finished()) return;
            const float radius = std::max(k->radius(), screenR);
            const float d = dist2(world, k->pos);
            if (d <= radius * radius && d < bestD) { best = k; bestD = d; }
        });
        return best;
    }

    Egg* hitEgg(Vector2 world, float screenHitRadius, float zoom) const {
        Egg* best = nullptr; float bestD = 1e30f;
        const float screenR = screenHitRadius / std::max(0.001f, zoom);
        const float maxEggR = cfg::tuning.eggRadiusBase + cfg::tuning.maxCreatureSize * cfg::tuning.eggRadiusSizeScale;
        const float broad = std::max(maxEggR, screenR);
        eggGrid.forCircle(world.x, world.y, broad, [&](Egg* e){
            if (!e || e->destroyed()) return;
            const float radius = std::max(e->radius, screenR);
            const float d = dist2(world, e->pos);
            if (d <= radius * radius && d < bestD) { best = e; bestD = d; }
        });
        return best;
    }

    Creature* randomCreatureDifferent() const {
        if (creatures.empty()) return nullptr;
        if (creatures.size() == 1) return creatures[0];
        Creature* cur = selected();
        for (int tries = 0; tries < 12; ++tries) {
            Creature* c = creatures[ri(0, (int)creatures.size() - 1)];
            if (!cur || c->id != cur->id) return c;
        }
        return creatures[0];
    }
};

enum class FollowMode : uint8_t { None, Random, TopGeneration };

// [SEC-UI] Estado e componentes de interface/HUD

// v0.125 OBSERVACAO DO INDIVIDUO:
// - Memorias e uma tela SOMENTE de leitura: episodios, registradores e estado recorrente.
// - Visao do bicho nao revela o mundo real: reconstrui exclusivamente RayReading.
// - Emocoes pertencem ao laboratorio Cerebro como aba; nao devem voltar a um modal paralelo.
struct UIState {
    FollowMode followMode = FollowMode::None;
    bool followPopup = false;
    bool inspectorOpen = false;
    bool inspectorMinimized = false;
    float inspectorScroll = 0.0f;
    bool mouseConsumed = false;

    int activePopulationField = -1;
    std::string populationEdit;
    bool populationMinimized = false;

    // Dashboard de graficos: rolagem independente para nunca disputar espaco
    // com o controle de populacao.
    float liveGraphsScroll = 0.0f;

    bool createPopup = false;
    Vector2 createPopupPos{};
    Vector2 createWorldPos{};

    bool killPopup = false;
    int killType = 0; // 0 bichos, 1 plantas, 2 carne, 3 ovos
    int killAmount = 10;
    int lastKilled = 0;

    bool configPopup = false;
    bool systemPopup = false;
    int systemTab = 0; // 0 mapa,1 causalidade,2 desempenho,3 historico,4 genealogia
    bool checkpointLoadRequested = false;
    std::string checkpointWorldPath;
    std::string checkpointConfigPath;
    int configSlider = -1;
    int configCategory = 0;
    float configScroll = 0.0f;

    // Laboratorio neural do bicho selecionado.
    bool brainLabOpen = false;
    int brainLabTab = 0; // 0 rede neural, 1 emocoes
    int brainHiddenIndex = 0;
    int brainConnectionIndex = 0;
    int brainSliderId = -1;

    // Memorias do individuo observado: episodica, registradores persistentes e
    // estado recorrente da rede. E observacao apenas; nao altera a simulacao.
    bool memoryLabOpen = false;
    float memoryLabScroll = 0.0f;

    // Retina 1D observacional: cada raio vira uma faixa vertical com cor, tipo e distancia.
    // Nao altera sensores nem o mundo.
    bool visionLabOpen = false;

    // Selecao/rolagem da aba Emocoes que agora vive dentro do Cerebro.
    int emotionLabIndex = 0;
    float emotionLabScroll = 0.0f;

    // Editor completo de caracteristicas do individuo selecionado.
    // Fica separado do Config global: aqui alteramos somente este bicho/DNA.
    bool traitEditorOpen = false;
    int traitEditorCategory = 0;
    float traitEditorScroll = 0.0f;
    int traitSliderId = -1;
    int traitEmotionIndex = 0;

    float plantOpacity = 0.60f;
    bool draggingPlantOpacity = false;
    bool draggingMapSize = false;

    // Painel de anotacoes/checklist. Conteudo vive em Simulation e vai no save;
    // aqui ficam apenas abertura, rolagem e estado temporario de edicao.
    bool notesOpen = false;
    float notesScroll = 0.0f;
    bool noteInputActive = false;
    int noteEditTargetId = -1;
    std::string noteEdit;

    // Feedback curto de salvar/carregar.
    std::string ioToast;
    double ioToastUntil = 0.0;
};

inline bool fullScreenModalOpen(const UIState& ui) {
    return ui.brainLabOpen || ui.memoryLabOpen || ui.visionLabOpen || ui.traitEditorOpen || ui.configPopup || ui.systemPopup;
}

enum class DragEntityKind { None, Creature, Plant, Carcass, Egg };

struct DragEntityState {
    DragEntityKind kind = DragEntityKind::None;
    void* ptr = nullptr;
    Vector2 grabOffset{};
    Vector2 pressScreen{};
    bool moved = false;

    void clear() {
        kind = DragEntityKind::None;
        ptr = nullptr;
        grabOffset = {};
        pressScreen = {};
        moved = false;
    }

    bool active() const { return kind != DragEntityKind::None && ptr != nullptr; }
};

bool pointIn(RectF r, Vector2 p) { return CheckCollisionPointRec(p, r); }

bool button(RectF r, const char* text, bool active = false) {
    Vector2 m = GetMousePosition();
    bool hover = pointIn(r, m);
    Color bg = active ? Color{80, 110, 170, 235} : (hover ? Color{75,75,75,235} : Color{45,45,45,225});
    DrawRectangleRounded(r, 0.22f, 6, bg);
    DrawRectangleRoundedLines(r, 0.22f, 6, 1.0f, Color{130,130,130,180});
    int fs = 14;
    int tw = MeasureText(text, fs);
    DrawText(text, (int)(r.x + (r.width - tw) * 0.5f), (int)(r.y + (r.height - fs) * 0.5f), fs, RAYWHITE);
    return hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

struct TopControlRects {
    RectF plants{},pause{},kill{},config{},reinforcement{},system{},checkpoint{},save{},load{},restart{},follow{},fullscreen{},hud{};
};

TopControlRects topControlRects(int sw) {
    constexpr float gap=5.0f,y0=10.0f,h=34.0f,left=8.0f;
    TopControlRects r;
    r.hud={(float)sw-113.0f,y0,105.0f,h};
    r.fullscreen={r.hud.x-gap-92.0f,y0,92.0f,h};
    const float rightLimit=std::max(left+120.0f,r.fullscreen.x-gap);
    float x=left,y=y0;
    auto place=[&](RectF& out,float w){ if(x+w>rightLimit && x>left){x=left;y+=h+gap;} out={x,y,w,h};x+=w+gap; };
    place(r.plants,100); place(r.pause,82); place(r.kill,70);
    place(r.system,76); place(r.reinforcement,82); place(r.config,70); place(r.checkpoint,70); place(r.save,64); place(r.load,64); place(r.restart,78); place(r.follow,82);
    return r;
}

float topControlsBottom(const TopControlRects& r,bool showHud){
    float b=std::max(r.hud.y+r.hud.height,r.fullscreen.y+r.fullscreen.height);
    if(!showHud) return b;
    const RectF* all[]={&r.plants,&r.pause,&r.kill,&r.system,&r.reinforcement,&r.config,&r.checkpoint,&r.save,&r.load,&r.restart,&r.follow};
    for(const RectF* q:all) b=std::max(b,q->y+q->height);
    return b;
}

bool topControlHit(const TopControlRects& r, Vector2 p, bool showHud) {
    if(pointIn(r.hud,p)||pointIn(r.fullscreen,p)) return true;
    if(!showHud) return false;
    return pointIn(r.plants,p)||pointIn(r.pause,p)||
           pointIn(r.kill,p)||pointIn(r.system,p)||pointIn(r.reinforcement,p)||pointIn(r.config,p)||pointIn(r.checkpoint,p)||pointIn(r.save,p)||pointIn(r.load,p)||pointIn(r.restart,p)||pointIn(r.follow,p);
}

RectF followPopupRect(const TopControlRects& controls,int sw,int sh){
    const float w=430.0f,h=390.0f;
    float x=controls.follow.x+controls.follow.width-w;
    float y=controls.follow.y+controls.follow.height+6.0f;
    x=clampf(x,8.0f,std::max(8.0f,(float)sw-w-8.0f));
    y=clampf(y,8.0f,std::max(8.0f,(float)sh-h-8.0f));
    return {x,y,w,h};
}

void openSpecificFollow(Simulation& sim,UIState& ui,Creature* c){
    if(!c || c->dead()) return;
    ui.followMode=FollowMode::None;
    sim.startWatch(c);
    ui.inspectorOpen=true;ui.inspectorMinimized=false;ui.inspectorScroll=0.0f;
    ui.followPopup=false;
}

void drawFollowPopup(Simulation& sim,UIState& ui,const TopControlRects& controls,int sw,int sh){
    if(!ui.followPopup) return;
    RectF p=followPopupRect(controls,sw,sh);
    DrawRectangleRounded(p,0.035f,6,Color{10,11,14,250});
    DrawRectangleRoundedLines(p,0.035f,6,1.0f,Color{95,110,140,235});
    DrawText("SEGUIR",(int)p.x+14,(int)p.y+12,18,RAYWHITE);
    DrawText("Modos automaticos e atuais recordistas do reforco",(int)p.x+14,(int)p.y+34,10,Color{165,180,205,255});
    const float bx=p.x+14,bw=p.width-28,bh=28;
    float y=p.y+55;
    if(button({bx,y,bw,bh},"Aleatorio",ui.followMode==FollowMode::Random)){
        if(ui.followMode==FollowMode::Random){ui.followMode=FollowMode::None;sim.closeWatch();ui.inspectorOpen=false;}
        else{ui.followMode=FollowMode::Random;Creature* c=sim.randomCreatureDifferent();if(c){sim.startWatch(c);ui.inspectorOpen=true;ui.inspectorMinimized=false;ui.inspectorScroll=0;}}
        ui.followPopup=false; return;
    }
    y+=32;
    if(button({bx,y,bw,bh},"Top geracao",ui.followMode==FollowMode::TopGeneration)){
        if(ui.followMode==FollowMode::TopGeneration){ui.followMode=FollowMode::None;sim.closeWatch();ui.inspectorOpen=false;}
        else{ui.followMode=FollowMode::TopGeneration;Creature* c=sim.highestGenerationCreature();if(c){sim.startWatch(c);ui.inspectorOpen=true;ui.inspectorMinimized=false;ui.inspectorScroll=0;}}
        ui.followPopup=false; return;
    }
    y+=39;
    DrawText("RECORDISTAS",(int)bx,(int)y,11,Color{235,198,105,255}); y+=18;
    const auto& rr=sim.reinforcementRecords;
    struct Row{const char* name;int id;float value;bool integer;bool valid;};
    Row rows[]={
        {"Maior tempo vivo",rr.longestLifeId,rr.longestLife,false,rr.longestLifeId>=0},
        {"Menor tempo vivo",rr.shortestLifeId,rr.shortestLife,false,rr.hasShortestLife&&rr.shortestLifeId>=0},
        {"Sangue > 50%",rr.healthAboveId,rr.longestHealthAbove50,false,rr.healthAboveId>=0},
        {"Sangue 50->0 MAIS RAPIDO",rr.healthCrashId,rr.fastestHealth50ToZero,false,rr.healthCrashId>=0},
        {"Energia > 50%",rr.energyAboveId,rr.longestEnergyAbove50,false,rr.energyAboveId>=0},
        {"Energia 50->0 MAIS RAPIDO",rr.energyCrashId,rr.fastestEnergy50ToZero,false,rr.energyCrashId>=0},
        {"Mais ovos",rr.mostEggsId,(float)rr.mostEggs,true,rr.mostEggsId>=0},
        {"Menos ovos",rr.fewestEggsId,(float)rr.fewestEggs,true,rr.hasFewestEggs&&rr.fewestEggsId>=0}
    };
    for(const Row& row:rows){
        Creature* alive=nullptr;
        if(row.valid){Creature* found=sim.creatureById(row.id);if(found&&!found->dead())alive=found;}
        std::string label=row.name;
        if(row.valid){label+="  #"+std::to_string(row.id)+"  ";label+=row.integer?std::to_string((int)row.value):(Simulation::f2(row.value)+"s");label+=alive?"":"  (morto)";}
        else label+="  --";
        if(button({bx,y,bw,24},label.c_str(),sim.selected()&&row.valid&&sim.selected()->id==row.id)){
            if(alive) openSpecificFollow(sim,ui,alive);
            else{ui.ioToast=row.valid?("Recordista #"+std::to_string(row.id)+" ja morreu; o recorde continua salvo."):"Ainda nao existe recordista nessa categoria.";ui.ioToastUntil=GetTime()+3.0;}
            return;
        }
        y+=27;
    }
}

struct GameSpriteAssets {
    Texture2D plant{};
    Texture2D meat{};
    Texture2D creature{};
    Texture2D egg{};
    std::string plantPath,meatPath,creaturePath,eggPath;
};
static GameSpriteAssets gSprites;

std::string locateSpritePng(const std::vector<std::string>& exactNames,const std::string& timestampToken){
    try{
        const std::filesystem::path dir(saveio::executableDirectory());
        for(const auto& name:exactNames){const auto p=dir/name;if(std::filesystem::exists(p))return p.string();}
        for(const auto& e:std::filesystem::directory_iterator(dir)){
            if(!e.is_regular_file())continue;
            std::string fn=e.path().filename().string();std::string ext=e.path().extension().string();
            std::transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return (char)std::tolower(c);});
            if(ext==".png" && fn.find(timestampToken)!=std::string::npos)return e.path().string();
        }
    }catch(...){}
    return {};
}

void loadGameSpriteAssets(){
    gSprites.plantPath=locateSpritePng({"ChatGPT Image 13 de ago. de 2026, 21_31_09 (1).png","Planta.png","planta.png"},"21_31_09");
    gSprites.meatPath=locateSpritePng({"ChatGPT Image 13 de ago. de 2026, 21_34_21.png","Carne.png","carne.png"},"21_34_21");
    gSprites.creaturePath=locateSpritePng({"ChatGPT Image 13 de ago. de 2026, 21_34_52.png","Bicho.png","bicho.png"},"21_34_52");
    gSprites.eggPath=locateSpritePng({"ovo.png"},"__ovo_exato_nao_varrer__");
    if(!gSprites.plantPath.empty())gSprites.plant=LoadTexture(gSprites.plantPath.c_str());
    if(!gSprites.meatPath.empty())gSprites.meat=LoadTexture(gSprites.meatPath.c_str());
    if(!gSprites.creaturePath.empty())gSprites.creature=LoadTexture(gSprites.creaturePath.c_str());
    if(!gSprites.eggPath.empty())gSprites.egg=LoadTexture(gSprites.eggPath.c_str());
    std::fprintf(stderr,"Sprites v0.138: planta=%s | carne=%s | bicho=%s | ovo=%s\n",IsTextureReady(gSprites.plant)?"OK":"fallback",IsTextureReady(gSprites.meat)?"OK":"fallback",IsTextureReady(gSprites.creature)?"OK":"fallback",IsTextureReady(gSprites.egg)?"OK":"fallback");
}
void unloadGameSpriteAssets(){UnloadTexture(gSprites.plant);UnloadTexture(gSprites.meat);UnloadTexture(gSprites.creature);UnloadTexture(gSprites.egg);}
int loadedGameSpriteCount(){return (IsTextureReady(gSprites.plant)?1:0)+(IsTextureReady(gSprites.meat)?1:0)+(IsTextureReady(gSprites.creature)?1:0)+(IsTextureReady(gSprites.egg)?1:0);}

int worldLOD(float zoom) {
    if (zoom < 0.32f) return 0; // longe: minimo de primitivas
    if (zoom < 0.78f) return 1; // medio
    return 2;                   // perto: detalhes completos
}

// [SEC-RENDER] Renderizacao do mundo e sprites
void drawPlant(const Plant& p, float zoom, float rootedOpacity) {
    const int lod = worldLOD(zoom);
    Vector2 c = p.visualCenter();
    const float r = p.visualRadius();
    const Color fill = p.genes.color;
    const Color outline = darkenRgb(p.genes.color,0.42f);

    // Em LOD medio/perto, usa o PNG cinza e o pixel shader aplica a cor genetica.
    // LOD distante conserva o quadrado barato para mundos com milhares de plantas.
    if(IsTextureReady(gSprites.plant) && lod>=1){
        Color tint=fill;
        tint.a=(unsigned char)std::lround(255.0f*(lod>=2?clampf(rootedOpacity,0.0f,1.0f):1.0f));
        DrawTextureCentered(gSprites.plant,c,r*2.0f,r*2.0f,0.0f,tint);
        return;
    }
    if (lod >= 2) {
        DrawCircleAlphaV(c, r, fill, outline, rootedOpacity);
    } else if (lod == 1) {
        DrawCircleV(c, r, fill);
    } else {
        const float d = std::max(0.8f, r * 1.65f);
        DrawRectangleV({c.x-d*0.5f,c.y-d*0.5f},{d,d},fill);
    }
}

Vector2 localToWorld(const Creature& c, float x, float y) {
    return add(c.pos, add(mul(c.right(), x), mul(c.forward(), -y)));
}

void drawCreature(const Creature& c, bool selected, float zoom) {
    const int lod = selected ? 2 : worldLOD(zoom);
    if (selected) DrawCircleLinesV(c.pos, c.size + 5.0f / zoom, Color{255,255,255,230});

    if (lod == 0) {
        const float d = std::max(1.0f, c.size * 0.88f);
        DrawRectangleV({c.pos.x-d*0.5f,c.pos.y-d*0.5f},{d,d},c.displayColor);
        return;
    }

    const bool spriteBody=IsTextureReady(gSprites.creature);
    if(spriteBody){
        // O PNG fornecido aponta a cabeca para BAIXO; no simulador angle=0 aponta para CIMA.
        // +180 alinha a cabeca visual ao mesmo forward() usado por boca, sensores e movimento.
        DrawTextureCentered(gSprites.creature,c.pos,c.size*1.08f,c.size*1.38f,c.angle+180.0f,c.displayColor);
    }else{
        RectF body{c.pos.x, c.pos.y, c.size, c.size};
        DrawRectanglePro(body, {c.size*0.5f,c.size*0.5f}, c.angle, c.displayColor);

        float physicalOpening = c.genome.mouth.maxOpening;
        float biteProgress = c.biteAnimation > 0 ? clampf(1.0f - c.biteAnimation / 0.22f, 0, 1) : 0;
        float closePulse = c.biteAnimation > 0 ? std::sin(biteProgress * PI) : 0;
        float visualOpening = physicalOpening * (1.0f - closePulse * 0.90f);
        float jawSpread = (1.1f + visualOpening * 4.2f) * c.mouthSizeFactor;
        float jawLength = (3.2f + physicalOpening * 4.2f) * c.mouthSizeFactor;
        Vector2 base = add(c.pos, mul(c.forward(), c.size*0.5f));

        if (lod == 1) {
            Vector2 tip = add(base, mul(c.forward(), jawLength * 0.82f));
            DrawLineEx(base, tip, std::max(0.55f, 0.75f*c.mouthSizeFactor), c.genome.mouth.color);
        } else {
            Vector2 tipL = add(base, add(mul(c.forward(), jawLength), mul(c.right(), -jawSpread)));
            Vector2 tipR = add(base, add(mul(c.forward(), jawLength), mul(c.right(), jawSpread)));
            Vector2 innerL = add(base, add(mul(c.forward(), jawLength*0.62f), mul(c.right(), -0.15f*c.mouthSizeFactor)));
            Vector2 innerR = add(base, add(mul(c.forward(), jawLength*0.62f), mul(c.right(), 0.15f*c.mouthSizeFactor)));
            DrawTriangle(base, tipL, innerL, c.genome.mouth.color);
            DrawTriangle(base, innerR, tipR, c.genome.mouth.color);
            if (closePulse > 0.55f) DrawCircleLinesV(add(base,mul(c.forward(),jawLength*0.7f)), 1.5f + closePulse*1.5f, Fade(c.genome.mouth.color,0.7f));
        }
    }

    // No sprite novo, a boca ja esta desenhada. Para o bicho selecionado, um pequeno
    // marcador genetico mostra a cor real da boca sem duplicar mandíbulas em todos os bichos.
    if(spriteBody && selected){
        const Vector2 mouthPos=add(c.pos,mul(c.forward(),c.size*0.47f));
        DrawCircleV(mouthPos,std::max(0.65f,c.mouthSizeFactor*1.15f),Fade(c.genome.mouth.color,0.82f));
    }

    if(selected && c.grabbedKind!=GrabKind::None){
        Vector2 target=c.pos; bool valid=false;
        if(c.grabbedKind==GrabKind::Plant && c.grabbedPlant){target=c.grabbedPlant->pos;valid=true;}
        else if(c.grabbedKind==GrabKind::Carcass && c.grabbedCarcass){target=c.grabbedCarcass->pos;valid=true;}
        else if(c.grabbedKind==GrabKind::Creature && c.grabbedCreature){target=c.grabbedCreature->pos;valid=true;}
        if(valid){
            DrawLineEx(c.pos,target,std::max(0.7f,1.2f/zoom),Color{80,205,255,230});
            DrawCircleLinesV(target,std::max(1.0f,2.7f/zoom),Color{80,205,255,230});
        }
    }

    if (selected) {
        float w = std::max(10.0f, c.size * 1.25f), h = 1.4f;
        Vector2 top{c.pos.x-w*0.5f,c.pos.y-c.size*0.5f-6.0f};
        DrawRectangleV(top,{w,h},Color{255,255,255,45}); DrawRectangleV(top,{w*(c.energy/c.maxEnergy),h},YELLOW);
        Vector2 hp{top.x,top.y+h+0.9f}; DrawRectangleV(hp,{w,h},Color{255,255,255,45}); DrawRectangleV(hp,{w*(c.health/cfg::tuning.healthMax),h},RED);
    }
}

void drawCarcass(const Carcass& k, float zoom) {
    const int lod=worldLOD(zoom);
    const float r = std::max(0.5f, k.radius());
    if(IsTextureReady(gSprites.meat) && lod>=1){
        Color tint=cfg::CARCASS_COLOR;
        tint.a=255;
        DrawTextureCentered(gSprites.meat,k.pos,r*2.15f,r*2.15f,0.0f,tint);
        return;
    }
    const Color outline = Color{105,18,18,255};
    const Color fill = Color{220,45,40,255};
    DrawCircleV(k.pos, r, outline);
    DrawCircleV(k.pos, std::max(0.30f, r - std::max(0.22f, r * 0.14f)), fill);
}


void drawEgg(const Egg& egg, float zoom) {
    const int lod = worldLOD(zoom);
    const float r = egg.radius;
    const float resources = egg.resourceFraction();
    const Color shell = Fade(Color{238,218,150,255}, 0.45f + 0.55f * resources);
    if(IsTextureReady(gSprites.egg) && lod>=1){
        Color tint=shell;
        DrawTextureCentered(gSprites.egg,egg.pos,r*2.35f,r*2.60f,0.0f,tint);
        return;
    }

    if (lod == 0) {
        const float d = std::max(1.0f, r * 0.9f);
        DrawRectangleV({egg.pos.x-d*0.5f,egg.pos.y-d*0.5f},{d,d},shell);
        return;
    }

    if (lod == 1) {
        DrawCircleV(egg.pos, std::max(0.8f,r*0.82f), shell);
        return;
    }

    // Ovo levemente oval, com nucleo vermelho representando o sangue investido.
    DrawCircleV(egg.pos, r, Color{130,105,55,220});
    DrawCircleV({egg.pos.x,egg.pos.y-r*0.10f}, r*0.86f, shell);

    const float bloodQ = clampf(
        egg.blood / (cfg::tuning.healthMax * cfg::tuning.eggBloodFullBirthFraction),
        0.0f, 1.0f);
    DrawCircleV(
        {egg.pos.x,egg.pos.y+r*0.05f},
        std::max(0.45f,r*0.28f),
        Fade(Color{185,45,45,255}, 0.30f + 0.65f*bloodQ)
    );
}

std::string timeStamp(float t) {
    int totalTenths = std::max(0, (int)std::floor(t * 10.0f));
    int min = totalTenths / 600;
    int sec = (totalTenths % 600) / 10;
    int tenth = totalTenths % 10;
    char b[32]; std::snprintf(b,sizeof(b),"+%02d:%02d.%d",min,sec,tenth); return b;
}

std::string globalSimulationClock(float t) {
    const long long totalTenths = std::max(0LL,(long long)std::floor((double)t*10.0));
    const long long days = totalTenths / 864000LL;
    const int hours = (int)((totalTenths / 36000LL) % 24LL);
    const int minutes = (int)((totalTenths / 600LL) % 60LL);
    const int seconds = (int)((totalTenths / 10LL) % 60LL);
    const int tenth = (int)(totalTenths % 10LL);
    char b[64];
    std::snprintf(b,sizeof(b),"%03lldd %02d:%02d:%02d.%d",days,hours,minutes,seconds,tenth);
    return b;
}

float drawLabelValue(float x, float y, const char* label, const std::string& value) {
    DrawText(label,(int)x,(int)y,12,Color{170,170,170,255});
    DrawText(value.c_str(),(int)x,(int)(y+14),15,RAYWHITE);
    return y + 34;
}

float averageBrainMemory(const Creature& c) {
    if (c.genome.brain.hiddenNodes.empty()) return 0.0f;
    double total = 0.0;
    for (const auto& n : c.genome.brain.hiddenNodes) total += n.memory;
    return float(total / double(c.genome.brain.hiddenNodes.size()));
}

int brainInputGroup(int inputIndex) {
    if (inputIndex < cfg::MAX_RAYS * cfg::INPUTS_PER_RAY)
        return inputIndex / cfg::INPUTS_PER_RAY;
    return cfg::MAX_RAYS + (inputIndex - cfg::MAX_RAYS * cfg::INPUTS_PER_RAY);
}
bool brainInputGroupActive(const NeuralGenome& g,int group){
    if(group<0)return false;
    if(group<cfg::MAX_RAYS){
        const int base=group*cfg::INPUTS_PER_RAY;
        for(int k=0;k<cfg::INPUTS_PER_RAY;++k)if(brainInputActive(g,base+k))return true;
        return false;
    }
    const int idx=cfg::MAX_RAYS*cfg::INPUTS_PER_RAY+(group-cfg::MAX_RAYS);
    return brainInputActive(g,idx);
}

std::string brainInputLabel(int inputIndex) {
    static const char* rayFields[cfg::INPUTS_PER_RAY] = {
        "dist", "planta", "bicho", "carne", "ovo", "planta solta LEGADO=0", "R alvo", "G alvo", "B alvo", "afinidade", "copa movimento"
    };
    if(inputIndex>=0 && inputIndex<cfg::MAX_RAYS*cfg::INPUTS_PER_RAY){
        const int ray=inputIndex/cfg::INPUTS_PER_RAY;
        const int field=inputIndex%cfg::INPUTS_PER_RAY;
        return "Raio "+std::to_string(ray)+" "+rayFields[field];
    }
    const int internal=inputIndex-cfg::MAX_RAYS*cfg::INPUTS_PER_RAY;
    static const char* baseInternals[cfg::BASE_INTERNAL_INPUTS]={
        "Energia", "Vida/sangue", "Velocidade frente/re", "Velocidade lateral", "Reproducao pronta", "Meu corpo R", "Meu corpo G", "Meu corpo B",
        "Alcance visao", "Qtd raios", "Forca mordida", "Abertura boca", "Velocidade boca",
        "Dureza", "Tamanho", "Sob planta enraizada", "Desenvolvimento", "Maturidade sexual",
        "Senescencia", "Idade/longevidade", "Velocidade angular", "Impacto recente", "Dano recente", "Energia ganha recente", "Empurrao externo",
        "Pressao frente", "Pressao direita", "Pressao tras", "Pressao esquerda", "Esta agarrando", "Peso carga/corpo"
    };
    if(internal>=0 && internal<cfg::BASE_INTERNAL_INPUTS) return baseInternals[internal];
    if(internal>=cfg::BASE_INTERNAL_INPUTS && internal<cfg::INTERNAL_INPUTS){
        const int si=internal-cfg::BASE_INTERNAL_INPUTS;
        static const char* percept[cfg::PERCEPTION_USED_INPUTS]={
            "Planta presente","Planta direcao","Planta proximidade","Planta contato boca",
            "Bicho presente","Bicho direcao","Bicho proximidade",
            "Carne presente","Carne direcao","Carne proximidade",
            "Ovo presente","Ovo direcao","Ovo proximidade"};
        if(si>=0&&si<cfg::PERCEPTION_USED_INPUTS)return percept[si];
        return std::string("Reservado legado ")+std::to_string(si);
    }
    if(inputIndex>=cfg::EMOTION_INPUT_BASE && inputIndex<cfg::EMOTION_INPUT_BASE+cfg::MAX_EMOTIONS)
        return "Emocao E"+std::to_string(inputIndex-cfg::EMOTION_INPUT_BASE);
    if(inputIndex>=cfg::MEMORY_INPUT_BASE && inputIndex<cfg::REGISTER_MEMORY_INPUT_BASE){
        const int local=inputIndex-cfg::MEMORY_INPUT_BASE;
        const int slot=local/cfg::MEMORY_FEATURES_PER_SLOT;
        const int f=local%cfg::MEMORY_FEATURES_PER_SLOT;
        static const char* fields[cfg::MEMORY_FEATURES_PER_SLOT]={"comida","atacado","ataque acertou","ovo","legado removido","intensidade","idade","dir X","dir Y","valido"};
        return "Mem evento "+std::to_string(slot)+" "+fields[f];
    }
    if(inputIndex>=cfg::REGISTER_MEMORY_INPUT_BASE && inputIndex<cfg::INPUT_COUNT)
        return "Registro persistente M"+std::to_string(inputIndex-cfg::REGISTER_MEMORY_INPUT_BASE);
    return "Entrada "+std::to_string(inputIndex);
}

std::string emotionSourceLabel(const EmotionTriggerGene& t){
    if(t.sourceKind==EmotionSourceKind::Input) return brainInputLabel(t.source);
    if(t.sourceKind==EmotionSourceKind::Hidden) return "Neuronio H"+std::to_string(t.source);
    if(t.sourceKind==EmotionSourceKind::Output){
        static const char* names[cfg::OUTPUT_COUNT]={"Saida frente","Saida re","Girar esquerda","Girar direita","Lateral esquerda","Lateral direita","Agarrar","Morder","Reservado 1","Acasalar","Cor corpo R","Cor corpo G","Cor corpo B","Mem escreve 0","Mem escreve 1","Mem escreve 2","Mem escreve 3","Mem gate 0","Mem gate 1","Mem gate 2","Mem gate 3"};
        return (t.source>=0&&t.source<cfg::OUTPUT_COUNT)?names[t.source]:"Saida "+std::to_string(t.source);
    }
    return "Emocao E"+std::to_string(t.source);
}

std::string brainDestinationLabel(BrainNodeKind kind,int index){
    if(kind==BrainNodeKind::Hidden) return "H"+std::to_string(index);
    if(kind==BrainNodeKind::Output){
        static const char* names[cfg::OUTPUT_COUNT]={"Frente","Re","Girar E","Girar D","Lateral E","Lateral D","Agarrar","Morder","Rsv1","Acasalar","Cor R","Cor G","Cor B","MemW0","MemW1","MemW2","MemW3","MemG0","MemG1","MemG2","MemG3"};
        return (index>=0&&index<cfg::OUTPUT_COUNT)?names[index]:"O"+std::to_string(index);
    }
    return brainInputLabel(index);
}
std::string brainOutputLabel(int index){
    static const char* names[cfg::OUTPUT_COUNT]={"Frente","Re","Girar esquerda","Girar direita","Lateral esquerda","Lateral direita","Agarrar","Morder","Reservado","Acasalar","Cor corpo R","Cor corpo G","Cor corpo B","Mem escreve 0","Mem escreve 1","Mem escreve 2","Mem escreve 3","Mem gate 0","Mem gate 1","Mem gate 2","Mem gate 3"};
    return (index>=0&&index<cfg::OUTPUT_COUNT)?names[index]:"Saida "+std::to_string(index);
}

float brainInputGroupDisplayValue(const Creature& c,const NeuralGenome& g,int group){
    if(group<0) return 0.0f;
    if(group<cfg::MAX_RAYS){
        const int base=group*cfg::INPUTS_PER_RAY;
        float best=0.0f;
        for(int k=0;k<cfg::INPUTS_PER_RAY;++k){
            const int idx=base+k;
            if(idx>=cfg::INPUT_COUNT || !brainInputActive(g,idx)) continue;
            const float v=c.inputs[(std::size_t)idx];
            if(std::fabs(v)>std::fabs(best)) best=v;
        }
        return best;
    }
    const int idx=cfg::MAX_RAYS*cfg::INPUTS_PER_RAY+(group-cfg::MAX_RAYS);
    return (idx>=0 && idx<cfg::INPUT_COUNT && brainInputActive(g,idx))?c.inputs[(std::size_t)idx]:0.0f;
}

std::string brainInputGroupShortLabel(int group){
    if(group<0) return "?";
    if(group<cfg::MAX_RAYS) return "R"+std::to_string(group);
    const int idx=cfg::MAX_RAYS*cfg::INPUTS_PER_RAY+(group-cfg::MAX_RAYS);
    if(idx>=0 && idx<cfg::BASE_INPUT_COUNT){
        static const char* shortNames[cfg::BASE_INTERNAL_INPUTS]={
            "EN","HP","VF","VL","RP","CR","CG","CB","AV","QR","FM","AB","VB","DU","TM","CO","DV","MS","SN","ID","VA","IM","DM","EG","EX","PF","PD","PT","PE","AG","CG"
        };
        if(idx<cfg::BASE_INTERNAL_INPUTS) return shortNames[idx];
        const int p=idx-cfg::PERCEPTION_INPUT_BASE;
        static const char* perceptShort[cfg::PERCEPTION_USED_INPUTS]={"PP","PD","PN","PM","BP","BD","BN","CP","CD","CN","OP","OD","ON"};
        if(p>=0 && p<cfg::PERCEPTION_USED_INPUTS) return perceptShort[p];
        return "I"+std::to_string(idx);
    }
    if(idx>=cfg::EMOTION_INPUT_BASE && idx<cfg::EMOTION_INPUT_BASE+cfg::MAX_EMOTIONS) return "E"+std::to_string(idx-cfg::EMOTION_INPUT_BASE);
    if(idx>=cfg::MEMORY_INPUT_BASE && idx<cfg::REGISTER_MEMORY_INPUT_BASE){
        const int local=idx-cfg::MEMORY_INPUT_BASE;
        return "L"+std::to_string(local/cfg::MEMORY_FEATURES_PER_SLOT);
    }
    if(idx>=cfg::REGISTER_MEMORY_INPUT_BASE && idx<cfg::INPUT_COUNT) return "M"+std::to_string(idx-cfg::REGISTER_MEMORY_INPUT_BASE);
    return "I"+std::to_string(idx);
}

void drawBrainHoverCard(RectF area,Vector2 anchor,const std::vector<std::string>& lines){
    if(lines.empty()) return;
    int maxW=0;
    for(const auto& s:lines) maxW=std::max(maxW,MeasureText(s.c_str(),12));
    const float pad=8.0f;
    const float h=pad*2.0f+lines.size()*15.0f;
    float w=std::max(170.0f,float(maxW)+pad*2.0f+6.0f);
    float x=anchor.x+14.0f;
    float y=anchor.y+14.0f;
    if(x+w>area.x+area.width-6.0f) x=anchor.x-w-14.0f;
    if(y+h>area.y+area.height-6.0f) y=area.y+area.height-h-6.0f;
    if(x<area.x+6.0f) x=area.x+6.0f;
    if(y<area.y+6.0f) y=area.y+6.0f;
    RectF box{x,y,w,h};
    DrawRectangleRounded(box,0.08f,5,Color{8,10,15,245});
    DrawRectangleRoundedLines(box,0.08f,5,1.0f,Color{145,165,205,230});
    for(std::size_t i=0;i<lines.size();++i)
        DrawText(lines[i].c_str(),(int)(x+pad),(int)(y+pad+i*15.0f),12,i==0?RAYWHITE:Color{200,210,225,255});
}


// Diagnostico contrafactual: recalcula UMA decisao usando o estado atual como
// memoria, mas sem modificar o bicho. Quando muteEmotion>=0, zera somente a
// entrada atual dessa emocao. Assim obtemos a influencia motora imediata dela.
std::array<float,cfg::OUTPUT_COUNT> brainSnapshotMotor(const Creature& c,int muteEmotion=-1){
    std::array<float,cfg::BRAIN_STATE_COUNT> state{};
    auto synWeight=[&](const CompiledConnection& cn){return cn.geneIndex>=0&&cn.geneIndex<(int)c.learnedWeights.size()?c.learnedWeights[(std::size_t)cn.geneIndex]:cn.weight;};
    auto sig=[&](const CompiledConnection& cn){return applyBrainLinkFunction(cn.function,state[cn.sourceIndex],cn.paramA,cn.paramB)*synWeight(cn);};
    for(int i=0;i<cfg::INPUT_COUNT;++i){
        float v=c.inputs[i];
        if(muteEmotion>=0 && i==cfg::EMOTION_INPUT_BASE+muteEmotion) v=0.0f;
        state[cfg::BRAIN_INPUT_BASE+i]=brainInputActive(c.genome.brain,i)?v:0.0f;
    }
    for(int h=0;h<c.genome.brain.hiddenCount;++h)
        state[cfg::BRAIN_PREV_HIDDEN_BASE+h]=(h<(int)c.hidden.size()?c.hidden[h]:0.0f);
    for(int o=0;o<cfg::OUTPUT_COUNT;++o)
        state[cfg::BRAIN_PREV_OUTPUT_BASE+o]=c.netOut[o];

    for(int layer=0;layer<cfg::MAX_BRAIN_LAYERS;++layer){
        for(int h:c.compiled.hiddenByLayer[layer]){
            const auto& node=c.genome.brain.hiddenNodes[h];
            float sum=node.bias;for(const auto& cn:c.compiled.hiddenIncoming[h])sum+=sig(cn);
            float gs=0.0f;for(const auto& cn:c.compiled.hiddenGateIncoming[h])gs+=sig(cn);if(!c.compiled.hiddenGateIncoming[h].empty())sum*=2.0f*sigmoid(gs);
            float ms=0.0f;for(const auto& cn:c.compiled.hiddenModIncoming[h])ms+=sig(cn);float dynGain=node.gain;if(!c.compiled.hiddenModIncoming[h].empty())dynGain*=0.35f+1.30f*sigmoid(ms);
            float sh=0.0f;for(const auto& cn:c.compiled.hiddenShuntIncoming[h])sh+=std::fabs(sig(cn));if(sh>0.0f)sum/=1.0f+sh;
            const float candidate=applyHiddenActivation(hiddenNodeActivation(node),sum*dynGain);
            const float mem=clampf(node.memory,0.0f,0.96f);
            const float prev=state[cfg::BRAIN_PREV_HIDDEN_BASE+h];
            state[cfg::BRAIN_HIDDEN_BASE+h]=prev*mem+candidate*(1.0f-mem);
        }
    }
    std::array<float,cfg::OUTPUT_COUNT> net{};
    for(int o=0;o<cfg::OUTPUT_COUNT;++o){
        if(!brainOutputActive(c.genome.brain,o)){net[o]=0.0f;continue;}
        float sum=c.genome.brain.outputBiases[o];for(const auto& cn:c.compiled.outputIncoming[o])sum+=sig(cn);
        float gs=0;for(const auto& cn:c.compiled.outputGateIncoming[o])gs+=sig(cn);if(!c.compiled.outputGateIncoming[o].empty())sum*=2.0f*sigmoid(gs);
        float ms=0;for(const auto& cn:c.compiled.outputModIncoming[o])ms+=sig(cn);if(!c.compiled.outputModIncoming[o].empty())sum*=0.35f+1.30f*sigmoid(ms);
        float sh=0;for(const auto& cn:c.compiled.outputShuntIncoming[o])sh+=std::fabs(sig(cn));if(sh>0)sum/=1.0f+sh;
        net[o]=sigmoid(sum);
    }
    std::array<float,cfg::OUTPUT_COUNT> result{};
    const float drive=net[0]-net[1];
    const float turn=net[3]-net[2];
    const float strafe=net[5]-net[4];
    result[0]=std::max(0.0f,drive); result[1]=std::max(0.0f,-drive);
    result[2]=std::max(0.0f,-turn); result[3]=std::max(0.0f,turn);
    result[4]=std::max(0.0f,-strafe); result[5]=std::max(0.0f,strafe);
    for(int o=0;o<cfg::MOTOR_OUTPUT_COUNT;++o)if(result[o]<0.10f)result[o]=0.0f;
    result[cfg::BITE_INTENT_OUTPUT_INDEX]=brainOutputActive(c.genome.brain,cfg::BITE_INTENT_OUTPUT_INDEX)?net[cfg::BITE_INTENT_OUTPUT_INDEX]:0.0f;
    result[cfg::LEGACY_RESERVED_OUTPUT_INDEX]=0.0f;
    result[cfg::MATE_INTENT_OUTPUT_INDEX]=brainOutputActive(c.genome.brain,cfg::MATE_INTENT_OUTPUT_INDEX)?net[cfg::MATE_INTENT_OUTPUT_INDEX]:0.0f;
    for(int k=0;k<cfg::COLOR_OUTPUT_COUNT;++k)result[cfg::COLOR_OUTPUT_BASE+k]=brainOutputActive(c.genome.brain,cfg::COLOR_OUTPUT_BASE+k)?net[cfg::COLOR_OUTPUT_BASE+k]:0.0f;
    for(int slot=0;slot<cfg::REGISTER_MEMORY_SLOTS;++slot){
        const int w=cfg::MEMORY_WRITE_OUTPUT_BASE+slot,gx=cfg::MEMORY_GATE_OUTPUT_BASE+slot;
        result[w]=brainOutputActive(c.genome.brain,w)?net[w]:0.0f;
        result[gx]=brainOutputActive(c.genome.brain,gx)?net[gx]:0.0f;
    }
    return result;
}

void drawBrainGraph(const Creature& c, RectF area) {
    DrawRectangleRounded(area,0.04f,4,Color{22,22,22,245});

    const auto& g = c.genome.brain;
    const int leftCount = cfg::MAX_RAYS + cfg::INTERNAL_INPUTS + cfg::MAX_EMOTIONS + cfg::MEMORY_INPUTS + cfg::REGISTER_MEMORY_SLOTS;
    const int oCount = cfg::OUTPUT_COUNT;

    auto yFor = [&](int i,int count){
        return area.y + 26.0f + (i+0.5f)*(area.height-42.0f)/float(std::max(1,count));
    };

    const float lx = area.x + 22.0f;
    const float ox = area.x + area.width - 22.0f;
    const float hiddenLeft = area.x + 74.0f;
    const float hiddenRight = area.x + area.width - 72.0f;

    std::array<int,cfg::MAX_BRAIN_LAYERS> layerCounts{};
    std::array<int,cfg::MAX_BRAIN_LAYERS> layerSeen{};
    int highestLayer = 0;
    for (int h=0; h<g.hiddenCount; ++h) {
        int l=hiddenNodeLayer(g.hiddenNodes[h]);
        ++layerCounts[l];
        highestLayer=std::max(highestLayer,l);
    }

    std::vector<Vector2> hiddenPos(g.hiddenCount);
    for (int h=0; h<g.hiddenCount; ++h) {
        int l=hiddenNodeLayer(g.hiddenNodes[h]);
        float t = highestLayer > 0 ? float(l)/float(highestLayer) : 0.5f;
        float hx = hiddenLeft + (hiddenRight-hiddenLeft)*t;
        int ordinal = layerSeen[l]++;
        hiddenPos[h] = {hx, yFor(ordinal, std::max(1,layerCounts[l]))};
    }

    auto endpoint = [&](BrainNodeKind kind,int index)->Vector2 {
        if(kind==BrainNodeKind::Input)
            return {lx,yFor(brainInputGroup(index),leftCount)};
        if(kind==BrainNodeKind::Hidden && index>=0 && index<(int)hiddenPos.size())
            return hiddenPos[index];
        return {ox,yFor(index,oCount)};
    };

    // Ligacoes primeiro. Recorrentes ficam alaranjadas; plasticas SOMA ficam azuladas.
    for(int ci=0;ci<(int)g.connections.size();++ci){
        const auto& cn=g.connections[(std::size_t)ci];
        if(!neuralConnectionEnabled(cn)) continue;
        Vector2 a=endpoint(cn.srcKind,cn.src);
        Vector2 b=endpoint(cn.dstKind,cn.dst);
        bool rec=connectionIsRecurrent(g,cn);
        const float w=ci<(int)c.learnedWeights.size()?c.learnedWeights[(std::size_t)ci]:cn.weight;
        float alpha=clampf(std::fabs(w)/3.2f,0.035f,0.24f);
        const auto mode=neuralConnectionMode(cn);
        Color base=mode==BrainConnectionMode::Gate?Color{190,100,240,255}:mode==BrainConnectionMode::Modulate?Color{80,220,155,255}:mode==BrainConnectionMode::Shunt?Color{235,90,90,255}:(neuralConnectionPlastic(cn)?Color{80,195,245,255}:(rec?ORANGE:WHITE));
        Color lc=Fade(base,std::max(rec?0.07f:0.035f,alpha));
        if(cn.srcKind==BrainNodeKind::Hidden && cn.dstKind==BrainNodeKind::Hidden && cn.src==cn.dst){
            DrawCircleLinesV(a,5.5f,Fade(ORANGE,0.48f));
        }else{
            DrawLineEx(a,b,rec?0.85f:0.55f,lc);
        }
    }

    for(int i=0;i<leftCount;++i){
        const bool active=brainInputGroupActive(g,i);
        DrawCircleV({lx,yFor(i,leftCount)},active?2.4f:1.6f,active?Color{135,185,235,245}:Color{55,58,65,170});
    }
    for(int h=0;h<g.hiddenCount;++h){BrainArea a=hiddenNodeArea(g.hiddenNodes[h]);Color col=a==BrainArea::Vision?Color{100,180,255,255}:a==BrainArea::Reserved?Color{95,95,105,255}:a==BrainArea::Memory?Color{190,120,240,255}:a==BrainArea::Emotion?Color{240,110,180,255}:a==BrainArea::Motor?Color{255,170,80,255}:a==BrainArea::Social?Color{120,230,145,255}:a==BrainArea::Internal?Color{220,210,110,255}:SKYBLUE;DrawCircleV(hiddenPos[h],2.8f,Fade(col,0.92f));}
    for(int o=0;o<oCount;++o){
        const bool active=brainOutputActive(g,o);
        DrawCircleV({ox,yFor(o,oCount)},active?4.2f:2.8f,active?Fade(ORANGE,0.95f):Color{65,65,70,180});
    }

    std::string stats="I "+std::to_string(brainActiveInputCount(g))+"/"+std::to_string(cfg::INPUT_COUNT)+
        " | O "+std::to_string(brainActiveOutputCount(g))+"/"+std::to_string(cfg::OUTPUT_COUNT)+
        " | H "+std::to_string(g.hiddenCount)+
        " | camadas "+std::to_string(c.compiled.usedLayers)+
        " | lig "+std::to_string(c.compiled.activeConnections)+
        " | gate "+std::to_string(c.compiled.gateConnections)+" mod "+std::to_string(c.compiled.modConnections)+
        " | plast "+std::to_string(c.compiled.plasticConnections)+" | reg "+std::to_string(brainRegisterCapacity(g))+
        " | loops "+std::to_string(c.compiled.recurrentConnections);
    DrawText(stats.c_str(),(int)area.x+8,(int)area.y+6,10,Color{170,170,170,255});
}


const char* brainKindLabel(BrainNodeKind kind) {
    if (kind == BrainNodeKind::Input) return "I";
    if (kind == BrainNodeKind::Hidden) return "H";
    return "O";
}

std::string brainEndpointText(BrainNodeKind kind, int index) {
    return std::string(brainKindLabel(kind)) + std::to_string(index);
}

void refreshEditedBrain(Creature& c) {
    c.genome.brain.hiddenCount = std::clamp(
        c.genome.brain.hiddenCount,
        0,
        std::min<int>(cfg::MAX_HIDDEN, (int)c.genome.brain.hiddenNodes.size())
    );
    ensureBrainOutputStorage(c.genome.brain);
    c.compiled = compileBrain(c.genome.brain);
    if ((int)c.hidden.size() != c.genome.brain.hiddenCount)
        c.hidden.resize(c.genome.brain.hiddenCount, 0.0f);
    // Edicao manual altera o DNA-base; reinicia somente o fenotipo aprendido
    // para evitar indices/pesos obsoletos depois de mudancas estruturais.
    c.learnedWeights.resize(c.genome.brain.connections.size());
    c.eligibilityTraces.assign(c.genome.brain.connections.size(),0.0f);
    for(std::size_t i=0;i<c.genome.brain.connections.size();++i)c.learnedWeights[i]=c.genome.brain.connections[i].weight;
}

bool brainFloatSlider(
    UIState& ui,
    int id,
    RectF r,
    const char* label,
    float& value,
    float minimum,
    float maximum,
    const char* suffix = ""
) {
    Vector2 m = GetMousePosition();
    const float t = clampf((value - minimum) / std::max(0.0001f, maximum - minimum), 0.0f, 1.0f);

    DrawText(TextFormat("%s: %.3f%s",label,value,suffix),(int)r.x,(int)r.y-18,12,Color{215,215,220,255});
    DrawRectangleRounded(r,0.40f,6,Color{35,35,42,245});
    DrawRectangleRounded(
        {r.x,r.y,r.width*t,r.height},
        0.40f,6,Color{75,115,185,245}
    );
    DrawCircleV({r.x+r.width*t,r.y+r.height*0.5f},6.0f,Color{235,235,240,255});

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        pointIn({r.x-6,r.y-8,r.width+12,r.height+16},m))
        ui.brainSliderId=id;

    bool changed=false;
    if (ui.brainSliderId==id && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        const float nt=clampf((m.x-r.x)/r.width,0.0f,1.0f);
        const float nv=minimum + (maximum-minimum)*nt;
        if (std::fabs(nv-value)>0.00001f) {
            value=nv;
            changed=true;
        }
    }
    if (ui.brainSliderId==id && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        ui.brainSliderId=-1;
    return changed;
}


// -----------------------------------------------------------------------------
// v0.103 - editor completo de caracteristicas do bicho selecionado
// -----------------------------------------------------------------------------
bool traitFloatSlider(
    UIState& ui,
    int id,
    RectF r,
    const RectF& viewport,
    const char* label,
    float& value,
    float minimum,
    float maximum,
    const char* suffix = ""
) {
    Vector2 m=GetMousePosition();
    if(maximum<minimum) std::swap(minimum,maximum);
    value=clampf(value,minimum,maximum);
    const float t=clampf((value-minimum)/std::max(0.0001f,maximum-minimum),0.0f,1.0f);

    DrawText(TextFormat("%s: %.3f%s",label,value,suffix),(int)r.x,(int)r.y-18,12,Color{215,215,220,255});
    DrawRectangleRounded(r,0.40f,6,Color{34,35,42,245});
    DrawRectangleRounded({r.x,r.y,r.width*t,r.height},0.40f,6,Color{74,125,190,245});
    DrawCircleV({r.x+r.width*t,r.y+r.height*0.5f},6.0f,Color{238,238,242,255});

    const RectF hit{r.x-6,r.y-8,r.width+12,r.height+16};
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && pointIn(viewport,m) && pointIn(hit,m))
        ui.traitSliderId=id;

    bool changed=false;
    if(ui.traitSliderId==id && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        const float nt=clampf((m.x-r.x)/std::max(1.0f,r.width),0.0f,1.0f);
        const float nv=minimum+(maximum-minimum)*nt;
        if(std::fabs(nv-value)>0.00001f){ value=nv; changed=true; }
    }
    if(ui.traitSliderId==id && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) ui.traitSliderId=-1;
    return changed;
}

bool traitIntSlider(UIState& ui,int id,RectF r,const RectF& viewport,const char* label,int& value,int minimum,int maximum){
    float f=(float)value;
    const bool changed=traitFloatSlider(ui,id,r,viewport,label,f,(float)minimum,(float)maximum);
    const int nv=std::clamp((int)std::lround(f),minimum,maximum);
    if(nv!=value){value=nv;return true;}
    return changed;
}

bool traitByteSlider(UIState& ui,int id,RectF r,const RectF& viewport,const char* label,unsigned char& value){
    int v=(int)value;
    const bool changed=traitIntSlider(ui,id,r,viewport,label,v,0,255);
    value=(unsigned char)std::clamp(v,0,255);
    return changed;
}

void refreshEditedVision(Creature& c){
    c.genome.vision.rayCount=normalizeRayCount(c.genome.vision.rayCount);
    c.genome.vision.range=clampf(c.genome.vision.range,cfg::tuning.visionMinRange,cfg::tuning.visionMaxRange);
    c.rayOffsets=visionOffsets(c.genome.vision);
    c.rays.assign(c.rayOffsets.size(),RayReading{});
    c.raySin.resize(c.rayOffsets.size());
    c.rayCos.resize(c.rayOffsets.size());
    for(std::size_t i=0;i<c.rayOffsets.size();++i){
        const float rad=c.rayOffsets[i]*DEG2RAD;
        c.raySin[i]=std::sin(rad); c.rayCos[i]=std::cos(rad);
        c.rays[i].angleOffset=c.rayOffsets[i];
        c.rays[i].distance=c.genome.vision.range;
        c.rays[i].normalizedDistance=1.0f;
    }
}

// Recalcula imediatamente tudo que depende de idade/desenvolvimento/tamanho sem cobrar
// energia de crescimento nem sortear morte natural. A porcentagem de energia e preservada.
void refreshEditedLifecyclePhenotype(Simulation& sim,Creature& c){
    const float oldMax=std::max(0.001f,c.maxEnergy);
    const float energyRatio=clampf(c.energy/oldMax,0.0f,1.0f);

    const float growthDuration=std::max(0.01f,c.genome.development.growthDuration);
    c.developmentProgress=clampf(c.age/growthDuration,0.0f,1.0f);
    const float shaped=std::pow(c.developmentProgress,std::max(0.01f,c.genome.development.growthCurveExponent));
    const float birth=clampf(c.genome.development.birthSizeFraction,0.01f,1.0f);
    c.size=std::max(0.05f,c.adultSize()*(birth+(1.0f-birth)*shaped));

    const float senStart=c.senescenceStartAge();
    const float senSpan=std::max(0.01f,c.genome.development.longevity-senStart);
    c.senescenceProgress=clampf(std::max(0.0f,(c.age-senStart)/senSpan),0.0f,1.0f);
    const float a=std::pow(c.senescenceProgress,std::max(0.01f,cfg::tuning.oldAgeCurveExponent));
    auto fromOne=[&](float target){return 1.0f+(target-1.0f)*a;};
    c.ageSpeedMult=fromOne(cfg::tuning.oldAgeSpeedMin);
    c.ageAccelMult=fromOne(cfg::tuning.oldAgeAccelMin);
    c.ageRotationMult=fromOne(cfg::tuning.oldAgeRotationMin);
    c.ageBiteMult=fromOne(cfg::tuning.oldAgeBiteMin);
    c.ageRegenMult=fromOne(cfg::tuning.oldAgeRegenMin);
    c.ageMetabolismMult=fromOne(cfg::tuning.oldAgeMetabolismMax);
    c.ageFertilityMult=clampf(fromOne(cfg::tuning.oldAgeFertilityMin),0.0f,1.0f);

    const float longRatio=std::max(0.01f,c.genome.development.longevity/std::max(1.0f,cfg::tuning.longevityReference));
    const float rawLong=std::pow(longRatio,cfg::tuning.longevityMetabolismExponent);
    c.longevityMaintenanceMult=clampf(1.0f+cfg::tuning.longevityMetabolismWeight*(rawLong-1.0f),
        cfg::tuning.longevityMetabolismMin,cfg::tuning.longevityMetabolismMax);

    c.cachePhenotype();
    c.energy=clampf(energyRatio*c.maxEnergy,0.0f,c.maxEnergy);
    c.health=clampf(c.health,0.0f,cfg::tuning.healthMax);
    c.speed=clampf(c.speed,-c.maxReverseEffective,c.maxForwardEffective);
    c.lateralSpeed=clampf(c.lateralSpeed,-c.maxLateralEffective,c.maxLateralEffective);
    sim.creatureGrid.update(&c);
}

void drawCreatureTraitEditor(Simulation& sim,UIState& ui,int sw,int sh){
    if(!ui.traitEditorOpen) return;
    DrawRectangle(0,0,sw,sh,Color{0,0,0,210});

    RectF panel{20.0f,28.0f,std::max(760.0f,(float)sw-40.0f),std::max(520.0f,(float)sh-56.0f)};
    if(panel.width>sw-16.0f)panel.width=sw-16.0f;
    if(panel.height>sh-16.0f)panel.height=sh-16.0f;
    panel.x=((float)sw-panel.width)*0.5f; panel.y=((float)sh-panel.height)*0.5f;
    DrawRectangleRounded(panel,0.018f,5,Color{8,9,12,253});
    DrawRectangleRoundedLines(panel,0.018f,5,1.0f,Color{70,105,145,235});

    Creature* c=sim.selected();
    const std::string title=c?"Editar caracteristicas - Criatura #"+std::to_string(c->id):"Editar caracteristicas - sem criatura viva";
    DrawText(title.c_str(),(int)panel.x+18,(int)panel.y+14,20,RAYWHITE);
    DrawText("Altera somente este individuo. Genes editados ficam no save e podem passar aos descendentes.",
        (int)panel.x+18,(int)panel.y+39,10,Color{150,175,205,255});
    if(button({panel.x+panel.width-104,panel.y+10,86,30},"Fechar")){
        ui.traitEditorOpen=false;ui.traitSliderId=-1;return;
    }
    if(!c){
        DrawText("O individuo observado nao esta mais vivo.",(int)panel.x+22,(int)panel.y+82,15,Color{200,200,210,255});
        return;
    }

    static const char* tabs[]={"Estado","Corpo","Boca","Visao","Reproducao","Desenvolv.","Cores","Emocoes","Cerebro"};
    constexpr int tabCount=(int)(sizeof(tabs)/sizeof(tabs[0]));
    ui.traitEditorCategory=std::clamp(ui.traitEditorCategory,0,tabCount-1);
    const float leftW=154.0f;
    RectF left{panel.x+14,panel.y+66,leftW,panel.height-82};
    RectF viewport{left.x+left.width+14,left.y,panel.width-left.width-42,left.height};
    DrawRectangleRounded(left,0.035f,5,Color{14,15,20,248});
    DrawRectangleRounded(viewport,0.018f,5,Color{13,14,18,248});
    DrawRectangleRoundedLines(left,0.035f,5,1.0f,Color{58,67,85,220});
    DrawRectangleRoundedLines(viewport,0.018f,5,1.0f,Color{58,67,85,220});

    float ty=left.y+10.0f;
    for(int i=0;i<tabCount;++i){
        RectF tr{left.x+8,ty,left.width-16,34};
        if(button(tr,tabs[i],i==ui.traitEditorCategory)){
            ui.traitEditorCategory=i;ui.traitEditorScroll=0.0f;ui.traitSliderId=-1;
        }
        ty+=39.0f;
    }

    const int cat=ui.traitEditorCategory;
    float contentH=520.0f;
    if(cat==0)contentH=470.0f;
    else if(cat==1)contentH=500.0f;
    else if(cat==2)contentH=390.0f;
    else if(cat==3)contentH=500.0f;
    else if(cat==4)contentH=420.0f;
    else if(cat==5)contentH=470.0f;
    else if(cat==6)contentH=650.0f;
    else if(cat==7){
        const int idx=std::clamp(ui.traitEmotionIndex,0,std::max(0,(int)c->genome.emotions.emotions.size()-1));
        const std::size_t nt=c->genome.emotions.emotions.empty()?0:c->genome.emotions.emotions[(std::size_t)idx].triggers.size();
        contentH=500.0f+(float)nt*44.0f;
    } else contentH=260.0f;
    const float maxScroll=std::max(0.0f,contentH-viewport.height+24.0f);
    ui.traitEditorScroll=clampf(ui.traitEditorScroll,0.0f,maxScroll);
    if(pointIn(viewport,GetMousePosition())){
        const float wheel=GetMouseWheelMove();
        if(wheel!=0.0f)ui.traitEditorScroll=clampf(ui.traitEditorScroll-wheel*52.0f,0.0f,maxScroll);
    }

    BeginScissorMode((int)viewport.x+2,(int)viewport.y+2,(int)viewport.width-4,(int)viewport.height-4);
    float x=viewport.x+18.0f,y=viewport.y+30.0f-ui.traitEditorScroll;
    const float sliderW=std::max(240.0f,viewport.width-38.0f);
    int sid=1000+cat*100;
    auto section=[&](const char* text){DrawText(text,(int)x,(int)y,17,Color{225,230,240,255});y+=34.0f;};
    auto slider=[&](const char* label,float& v,float lo,float hi,const char* suffix=""){
        const bool ch=traitFloatSlider(ui,sid++,{x,y,sliderW,12},viewport,label,v,lo,hi,suffix);y+=46.0f;return ch;
    };
    auto intSlider=[&](const char* label,int& v,int lo,int hi){
        const bool ch=traitIntSlider(ui,sid++,{x,y,sliderW,12},viewport,label,v,lo,hi);y+=46.0f;return ch;
    };
    auto byteSlider=[&](const char* label,unsigned char& v){
        const bool ch=traitByteSlider(ui,sid++,{x,y,sliderW,12},viewport,label,v);y+=46.0f;return ch;
    };
    auto contentButton=[&](RectF r,const char* text,bool active=false){
        if(r.y+r.height<viewport.y||r.y>viewport.y+viewport.height)return false;
        return button(r,text,active);
    };

    if(cat==0){
        section("Estado atual (nao genetico)");
        slider("Energia atual",c->energy,0.0f,std::max(0.01f,c->maxEnergy));
        slider("Vida / sangue",c->health,0.0f,cfg::tuning.healthMax);
        bool ageChanged=slider("Idade simulada",c->age,0.0f,std::max(cfg::tuning.longevityMax,c->genome.development.longevity*2.0f)," s");
        if(ageChanged)refreshEditedLifecyclePhenotype(sim,*c);
        bool angleChanged=slider("Angulo",c->angle,0.0f,360.0f," graus");
        if(angleChanged)c->refreshDirection();
        slider("Velocidade frente/re atual",c->speed,-std::max(0.01f,c->maxReverseEffective),std::max(0.01f,c->maxForwardEffective));
        slider("Velocidade lateral atual",c->lateralSpeed,-std::max(0.01f,c->maxLateralEffective),std::max(0.01f,c->maxLateralEffective));
        slider("Cooldown reproducao",c->reproCooldown,0.0f,std::max(1.0f,cfg::tuning.reproCooldown*3.0f)," s");
        slider("Cooldown mordida",c->biteCooldown,0.0f,std::max(0.5f,cfg::tuning.biteCooldownMax)," s");
    }else if(cat==1){
        section("Corpo e movimento - DNA");
        bool changed=false,lifecycle=false;
        changed|=slider("Velocidade frontal max",c->genome.physical.maxForwardSpeed,cfg::tuning.geneForwardMin,cfg::tuning.geneForwardMax);
        changed|=slider("Velocidade re max",c->genome.physical.maxReverseSpeed,cfg::tuning.geneReverseMin,cfg::tuning.geneReverseMax);
        changed|=slider("Razao velocidade lateral",c->genome.physical.lateralSpeedRatio,cfg::tuning.geneLateralRatioMin,cfg::tuning.geneLateralRatioMax);
        changed|=slider("Aceleracao",c->genome.physical.acceleration,cfg::tuning.geneAccelerationMin,cfg::tuning.geneAccelerationMax);
        changed|=slider("Friccao corporal",c->genome.physical.friction,cfg::tuning.geneFrictionMin,cfg::tuning.geneFrictionMax);
        changed|=slider("Velocidade de giro",c->genome.physical.rotationSpeed,cfg::tuning.geneRotationMin,cfg::tuning.geneRotationMax);
        lifecycle|=slider("Tamanho adulto",c->genome.physical.bodySize,cfg::tuning.minCreatureSize,cfg::tuning.maxCreatureSize);
        changed|=slider("Dureza",c->genome.physical.hardness,cfg::tuning.geneHardnessMin,cfg::tuning.geneHardnessMax);
        if(lifecycle)refreshEditedLifecyclePhenotype(sim,*c);else if(changed)c->cachePhenotype();
    }else if(cat==2){
        section("Boca e alimentacao - DNA");
        bool changed=false;
        changed|=slider("Forca da mordida",c->genome.mouth.biteForce,cfg::tuning.mouthBiteGeneMin,cfg::tuning.mouthBiteGeneMax);
        changed|=slider("Abertura maxima",c->genome.mouth.maxOpening,cfg::tuning.mouthOpeningGeneMin,cfg::tuning.mouthOpeningGeneMax);
        changed|=slider("Velocidade da boca",c->genome.mouth.movementSpeed,cfg::tuning.mouthSpeedGeneMin,cfg::tuning.mouthSpeedGeneMax);
        section("Cor digestiva / afinidade");
        byteSlider("Boca R",c->genome.mouth.color.r);byteSlider("Boca G",c->genome.mouth.color.g);byteSlider("Boca B",c->genome.mouth.color.b);
        c->genome.mouth.color.a=255;
        if(changed)c->cachePhenotype();
    }else if(cat==3){
        section("Visao - DNA");
        bool changed=false;
        changed|=intSlider("Quantidade de raios",c->genome.vision.rayCount,(int)std::lround(cfg::tuning.minRaysEffective),(int)std::lround(cfg::tuning.maxRaysEffective));
        changed|=slider("Alcance",c->genome.vision.range,cfg::tuning.visionMinRange,cfg::tuning.visionMaxRange);
        changed|=slider("Foco dos raios",c->genome.vision.focusExponent,cfg::tuning.visionFocusMin,cfg::tuning.visionFocusMax);
        changed|=slider("Assimetria",c->genome.vision.asymmetry,-cfg::tuning.visionAsymmetryMax,cfg::tuning.visionAsymmetryMax);
        changed|=slider("Sensibilidade R",c->genome.vision.sensitivityR,cfg::tuning.visionColorSensitivityMin,cfg::tuning.visionColorSensitivityMax);
        changed|=slider("Sensibilidade G",c->genome.vision.sensitivityG,cfg::tuning.visionColorSensitivityMin,cfg::tuning.visionColorSensitivityMax);
        changed|=slider("Sensibilidade B",c->genome.vision.sensitivityB,cfg::tuning.visionColorSensitivityMin,cfg::tuning.visionColorSensitivityMax);
        changed|=slider("Eficiencia energetica",c->genome.vision.efficiency,cfg::tuning.visionEfficiencyMin,cfg::tuning.visionEfficiencyMax);
        if(changed)refreshEditedVision(*c);
        DrawText(TextFormat("FOV derivado atual: %.1f graus",visionFov(c->genome.vision.range)),(int)x,(int)y,12,Color{155,190,225,255});
    }else if(cat==4){
        section("Reproducao e escolha de parceiro - DNA");
        slider("Energia investida no ovo",c->genome.reproduction.eggEnergyFraction,cfg::tuning.eggEnergyGeneMin,cfg::tuning.eggEnergyGeneMax);
        slider("Sangue investido no ovo",c->genome.reproduction.eggBloodFraction,cfg::tuning.eggBloodGeneMin,cfg::tuning.eggBloodGeneMax);
        slider("Alcance para parceiro",c->genome.reproduction.mateRange,cfg::tuning.mateRangeMin,cfg::tuning.mateRangeMax);
        slider("Preferencia visual",c->genome.reproduction.visualPreference,-1.0f,1.0f);
        slider("Forca da escolha",c->genome.reproduction.mateChoiceStrength,0.0f,1.0f);
    }else if(cat==5){
        section("Desenvolvimento e ciclo de vida - DNA");
        bool changed=false;
        changed|=slider("Tamanho ao nascer",c->genome.development.birthSizeFraction,cfg::tuning.birthSizeFractionMin,cfg::tuning.birthSizeFractionMax);
        changed|=slider("Duracao do crescimento",c->genome.development.growthDuration,cfg::tuning.growthDurationMin,cfg::tuning.growthDurationMax," s");
        changed|=slider("Curva de crescimento",c->genome.development.growthCurveExponent,cfg::tuning.growthCurveMin,cfg::tuning.growthCurveMax);
        changed|=slider("Inicio adolescencia",c->genome.development.adolescenceStartFraction,cfg::tuning.adolescenceStartMin,cfg::tuning.adolescenceStartMax);
        changed|=slider("Maturidade sexual",c->genome.development.sexualMaturityFraction,cfg::tuning.maturityFractionMin,cfg::tuning.maturityFractionMax);
        changed|=slider("Longevidade",c->genome.development.longevity,cfg::tuning.longevityMin,cfg::tuning.longevityMax," s");
        changed|=slider("Inicio senescencia",c->genome.development.senescenceStartFraction,cfg::tuning.senescenceFractionMin,cfg::tuning.senescenceFractionMax);
        if(changed)refreshEditedLifecyclePhenotype(sim,*c);
    }else if(cat==6){
        section("Cor-base do corpo - DNA");
        bool colorChanged=false;
        colorChanged|=byteSlider("Corpo R",c->genome.bodyColor.r);colorChanged|=byteSlider("Corpo G",c->genome.bodyColor.g);colorChanged|=byteSlider("Corpo B",c->genome.bodyColor.b);
        c->genome.bodyColor.a=255;
        if(colorChanged)c->displayColor=c->genome.bodyColor;
        section("Canais controlados pelo cerebro");
        RectF rb{x,y,100,30},gb{x+108,y,100,30},bb{x+216,y,100,30};
        if(contentButton(rb,(c->genome.bodyColorControl.channelMask&1)?"R: ON":"R: OFF",(c->genome.bodyColorControl.channelMask&1)!=0))c->genome.bodyColorControl.channelMask^=1;
        if(contentButton(gb,(c->genome.bodyColorControl.channelMask&2)?"G: ON":"G: OFF",(c->genome.bodyColorControl.channelMask&2)!=0))c->genome.bodyColorControl.channelMask^=2;
        if(contentButton(bb,(c->genome.bodyColorControl.channelMask&4)?"B: ON":"B: OFF",(c->genome.bodyColorControl.channelMask&4)!=0))c->genome.bodyColorControl.channelMask^=4;
        y+=52.0f;
        slider("Velocidade de mudanca",c->genome.bodyColorControl.changeSpeed,cfg::tuning.bodyColorSpeedMin,cfg::tuning.bodyColorSpeedMax,"x/s");
        bool mn=slider("R minimo",c->genome.bodyColorControl.rMin,0.0f,1.0f);bool mx=slider("R maximo",c->genome.bodyColorControl.rMax,0.0f,1.0f);if(mn&&c->genome.bodyColorControl.rMin>c->genome.bodyColorControl.rMax)c->genome.bodyColorControl.rMax=c->genome.bodyColorControl.rMin;if(mx&&c->genome.bodyColorControl.rMax<c->genome.bodyColorControl.rMin)c->genome.bodyColorControl.rMin=c->genome.bodyColorControl.rMax;
        mn=slider("G minimo",c->genome.bodyColorControl.gMin,0.0f,1.0f);mx=slider("G maximo",c->genome.bodyColorControl.gMax,0.0f,1.0f);if(mn&&c->genome.bodyColorControl.gMin>c->genome.bodyColorControl.gMax)c->genome.bodyColorControl.gMax=c->genome.bodyColorControl.gMin;if(mx&&c->genome.bodyColorControl.gMax<c->genome.bodyColorControl.gMin)c->genome.bodyColorControl.gMin=c->genome.bodyColorControl.gMax;
        mn=slider("B minimo",c->genome.bodyColorControl.bMin,0.0f,1.0f);mx=slider("B maximo",c->genome.bodyColorControl.bMax,0.0f,1.0f);if(mn&&c->genome.bodyColorControl.bMin>c->genome.bodyColorControl.bMax)c->genome.bodyColorControl.bMax=c->genome.bodyColorControl.bMin;if(mx&&c->genome.bodyColorControl.bMax<c->genome.bodyColorControl.bMin)c->genome.bodyColorControl.bMin=c->genome.bodyColorControl.bMax;
    }else if(cat==7){
        section("Emocoes - DNA");
        auto& eg=c->genome.emotions;
        const int maxE=(int)std::lround(cfg::tuning.emotionMaxEffective);
        const int minE=(int)std::lround(cfg::tuning.emotionMinEffective);
        RectF addB{x,y,140,30},removeB{x+148,y,140,30};
        if((int)eg.emotions.size()<maxE && contentButton(addB,"Adicionar emocao")){
            eg.emotions.push_back(randomEmotionGene(c->genome.brain,(int)eg.emotions.size()));
            ui.traitEmotionIndex=(int)eg.emotions.size()-1;ui.traitEditorScroll=0.0f;
        }
        if((int)eg.emotions.size()>minE && !eg.emotions.empty() && contentButton(removeB,"Remover atual")){
            removeEmotionAt(eg,std::clamp(ui.traitEmotionIndex,0,(int)eg.emotions.size()-1));
            ui.traitEmotionIndex=std::clamp(ui.traitEmotionIndex,0,std::max(0,(int)eg.emotions.size()-1));
            sanitizeEmotionTriggers(eg,c->genome.brain);
        }
        y+=48.0f;
        if(eg.emotions.empty()){
            DrawText("Este bicho nao possui emocoes no DNA.",(int)x,(int)y,13,Color{180,180,195,255});
        }else{
            ui.traitEmotionIndex=std::clamp(ui.traitEmotionIndex,0,(int)eg.emotions.size()-1);
            RectF prev{x,y,40,28},next{x+48,y,40,28};
            if(contentButton(prev,"<"))ui.traitEmotionIndex=(ui.traitEmotionIndex+(int)eg.emotions.size()-1)%(int)eg.emotions.size();
            if(contentButton(next,">"))ui.traitEmotionIndex=(ui.traitEmotionIndex+1)%(int)eg.emotions.size();
            DrawText(TextFormat("E%d de %d",ui.traitEmotionIndex,(int)eg.emotions.size()),(int)x+104,(int)y+7,13,RAYWHITE);y+=48.0f;
            auto& e=eg.emotions[(std::size_t)ui.traitEmotionIndex];
            slider("Bias",e.bias,cfg::tuning.emotionBiasMin,cfg::tuning.emotionBiasMax);
            slider("Ganho / sensibilidade",e.gain,cfg::tuning.emotionGainMin,cfg::tuning.emotionGainMax);
            slider("Limiar",e.threshold,cfg::tuning.emotionThresholdMin,cfg::tuning.emotionThresholdMax);
            slider("Forca maxima",e.strength,cfg::tuning.emotionStrengthMin,cfg::tuning.emotionStrengthMax);
            slider("Impacto no cerebro",e.impact,cfg::tuning.emotionImpactMin,cfg::tuning.emotionImpactMax);
            slider("Tempo de subida",e.riseTime,cfg::tuning.emotionRiseTimeMin,cfg::tuning.emotionRiseTimeMax," s");
            slider("Duracao",e.duration,cfg::tuning.emotionDurationMin,cfg::tuning.emotionDurationMax," s");
            section("Pesos dos gatilhos");
            const int maxT=(int)std::lround(cfg::tuning.emotionMaxTriggersPerEmotion);
            RectF addT{x,y,150,28},remT{x+158,y,150,28};
            if((int)e.triggers.size()<maxT && contentButton(addT,"Adicionar gatilho"))e.triggers.push_back(randomEmotionTrigger(c->genome.brain,(int)eg.emotions.size()));
            if(!e.triggers.empty() && contentButton(remT,"Remover ultimo"))e.triggers.pop_back();
            y+=44.0f;
            for(std::size_t ti=0;ti<e.triggers.size();++ti){
                auto& t=e.triggers[ti];
                const std::string label="T"+std::to_string(ti)+" "+emotionSourceLabel(t);
                slider(label.c_str(),t.weight,-cfg::tuning.emotionTriggerWeightLimit,cfg::tuning.emotionTriggerWeightLimit);
            }
            sanitizeEmotionTriggers(eg,c->genome.brain);
        }
    }else{
        section("Cerebro evolutivo");
        DrawText(TextFormat("%d/%d inputs ativos | %d/%d outputs | %d ocultos | %d ligacoes | %d loops",brainActiveInputCount(c->genome.brain),cfg::INPUT_COUNT,brainActiveOutputCount(c->genome.brain),cfg::OUTPUT_COUNT,c->genome.brain.hiddenCount,c->compiled.activeConnections,c->compiled.recurrentConnections),(int)x,(int)y,13,RAYWHITE);y+=22.0f;
        DrawText(TextFormat("gates %d | moduladoras %d | shunts %d | plasticas %d",c->compiled.gateConnections,c->compiled.modConnections,c->compiled.shuntConnections,c->compiled.plasticConnections),(int)x,(int)y,11,Color{165,205,190,255});y+=20.0f;
        DrawText(TextFormat("registradores persistentes %d/%d | recompensa plastica %.2f | updates %llu",brainRegisterCapacity(c->genome.brain),cfg::REGISTER_MEMORY_SLOTS,c->lastPlasticReward,(unsigned long long)c->plasticUpdates),(int)x,(int)y,10,Color{220,190,115,255});y+=22.0f;
        DrawText("Editor: areas, ativacoes, registradores e conexoes soma/gate/mod/shunt/plasticas.",(int)x,(int)y,11,Color{165,175,195,255});y+=32.0f;
        RectF open{x,y,220,34};
        if(contentButton(open,"Abrir editor do cerebro")){
            ui.traitEditorOpen=false;ui.traitSliderId=-1;ui.brainLabOpen=true;ui.brainLabTab=0;ui.brainHiddenIndex=0;ui.brainConnectionIndex=0;ui.brainSliderId=-1;
            EndScissorMode();return;
        }
    }
    EndScissorMode();

    if(maxScroll>0.0f){
        const float trackX=viewport.x+viewport.width-6.0f,trackY=viewport.y+8.0f,trackH=viewport.height-16.0f;
        DrawRectangle((int)trackX,(int)trackY,3,(int)trackH,Color{45,48,58,220});
        const float thumbH=std::max(26.0f,trackH*(viewport.height/std::max(viewport.height,contentH)));
        const float t=maxScroll>0.0f?ui.traitEditorScroll/maxScroll:0.0f;
        DrawRectangle((int)trackX,(int)(trackY+t*(trackH-thumbH)),3,(int)thumbH,Color{105,155,210,245});
    }
}

void drawBrainLabGraph(Creature& c, UIState& ui, RectF area) {
    DrawRectangleRounded(area,0.025f,5,Color{13,14,18,248});
    DrawRectangleRoundedLines(area,0.025f,5,1.0f,Color{72,82,105,220});

    const auto& g=c.genome.brain;
    const int leftCount=cfg::MAX_RAYS+cfg::INTERNAL_INPUTS+cfg::MAX_EMOTIONS+cfg::MEMORY_INPUTS+cfg::REGISTER_MEMORY_SLOTS;
    const int oCount=cfg::OUTPUT_COUNT;
    Vector2 mouse=GetMousePosition();

    auto yFor=[&](int i,int count){
        return area.y+56.0f+(i+0.5f)*(area.height-116.0f)/float(std::max(1,count));
    };

    const float lx=area.x+42.0f;
    const float ox=area.x+area.width-58.0f;
    const float hiddenLeft=area.x+150.0f;
    const float hiddenRight=area.x+area.width-164.0f;

    std::array<int,cfg::MAX_BRAIN_LAYERS> layerCounts{};
    std::array<int,cfg::MAX_BRAIN_LAYERS> layerSeen{};
    int highestLayer=0;
    for(int h=0;h<g.hiddenCount;++h){
        int l=hiddenNodeLayer(g.hiddenNodes[h]);
        ++layerCounts[l];
        highestLayer=std::max(highestLayer,l);
    }

    std::vector<Vector2> hiddenPos(g.hiddenCount);
    for(int h=0;h<g.hiddenCount;++h){
        int l=hiddenNodeLayer(g.hiddenNodes[h]);
        float t=highestLayer>0?float(l)/float(highestLayer):0.5f;
        float hx=hiddenLeft+(hiddenRight-hiddenLeft)*t;
        int ordinal=layerSeen[l]++;
        hiddenPos[h]={hx,yFor(ordinal,std::max(1,layerCounts[l]))};
    }

    auto endpoint=[&](BrainNodeKind kind,int index)->Vector2{
        if(kind==BrainNodeKind::Input) return {lx,yFor(brainInputGroup(index),leftCount)};
        if(kind==BrainNodeKind::Hidden && index>=0 && index<(int)hiddenPos.size()) return hiddenPos[index];
        return {ox,yFor(index,oCount)};
    };

    auto groupColor=[&](int group)->Color{
        const bool ray=group<cfg::MAX_RAYS;
        const int emotionStart=cfg::MAX_RAYS+cfg::INTERNAL_INPUTS;
        const int memoryStart=emotionStart+cfg::MAX_EMOTIONS;
        const int registerStart=memoryStart+cfg::MEMORY_INPUTS;
        const bool emotion=group>=emotionStart&&group<memoryStart;
        const bool memory=group>=memoryStart&&group<registerStart;
        const bool reg=group>=registerStart;
        return emotion?Color{190,105,230,245}:(reg?Color{255,190,85,245}:(memory?Color{90,205,175,245}:(ray?Color{105,130,175,240}:Color{110,110,120,220})));
    };

    auto sourceState=[&](const NeuralConnectionGene& cn)->float{
        if(cn.srcKind==BrainNodeKind::Input) return (cn.src>=0 && cn.src<cfg::INPUT_COUNT && brainInputActive(g,cn.src))?c.inputs[(std::size_t)cn.src]:0.0f;
        if(cn.srcKind==BrainNodeKind::Hidden){
            const int idx=connectionIsRecurrent(g,cn)?cfg::BRAIN_PREV_HIDDEN_BASE+cn.src:cfg::BRAIN_HIDDEN_BASE+cn.src;
            return (idx>=0 && idx<cfg::BRAIN_STATE_COUNT)?c.brainState[(std::size_t)idx]:0.0f;
        }
        const int idx=cfg::BRAIN_PREV_OUTPUT_BASE+cn.src;
        return (idx>=0 && idx<cfg::BRAIN_STATE_COUNT)?c.brainState[(std::size_t)idx]:0.0f;
    };

    auto connectionContribution=[&](int ci)->float{
        if(ci<0 || ci>=(int)g.connections.size()) return 0.0f;
        const auto& cn=g.connections[(std::size_t)ci];
        const float src=sourceState(cn);
        const float transformed=applyBrainLinkFunction(cn.function,src,cn.paramA,cn.paramB);
        const float w=ci<(int)c.learnedWeights.size()?c.learnedWeights[(std::size_t)ci]:cn.weight;
        return transformed*w;
    };

    auto shortAreaName=[&](BrainArea a)->const char*{
        switch(a){
            case BrainArea::Vision:return "VIS";
            case BrainArea::Reserved:return "RSV";
            case BrainArea::Internal:return "INT";
            case BrainArea::Memory:return "MEM";
            case BrainArea::Emotion:return "EMO";
            case BrainArea::Motor:return "MOT";
            case BrainArea::Social:return "SOC";
            default:return "GER";
        }
    };

    constexpr int BRAIN_GRAPH_CONNECTION_BUDGET = 900;
    const int activeTotal = std::max(0, c.compiled.activeConnections);
    const int sampleStride = std::max(1, activeTotal / BRAIN_GRAPH_CONNECTION_BUDGET);
    int activeOrdinal = 0;
    int drawnConnections = 0;
    int hoverConnection = -1;
    float hoverConnectionDist = 8.0f;
    int hoverInputGroup=-1, hoverHidden=-1, hoverOutput=-1;

    for(int ci=0;ci<(int)g.connections.size();++ci){
        const auto& cn=g.connections[(std::size_t)ci];
        if(!neuralConnectionEnabled(cn)) continue;

        const bool selected=ci==ui.brainConnectionIndex;
        const bool touchesSelectedHidden=g.hiddenCount>0&&((cn.srcKind==BrainNodeKind::Hidden&&cn.src==ui.brainHiddenIndex)||(cn.dstKind==BrainNodeKind::Hidden&&cn.dst==ui.brainHiddenIndex));
        const bool rec=connectionIsRecurrent(g,cn);
        const bool sampled=(activeOrdinal % sampleStride)==0;
        ++activeOrdinal;

        if(!selected && !touchesSelectedHidden && !rec && !sampled) continue;
        if(drawnConnections>=BRAIN_GRAPH_CONNECTION_BUDGET && !selected && !touchesSelectedHidden) continue;

        Vector2 a=endpoint(cn.srcKind,cn.src);
        Vector2 b=endpoint(cn.dstKind,cn.dst);
        const float displayWeight=ci<(int)c.learnedWeights.size()?c.learnedWeights[(std::size_t)ci]:cn.weight;
        const float flow=std::fabs(connectionContribution(ci));
        const float alpha=selected?0.95f:clampf(std::max(std::fabs(displayWeight)/3.2f,flow/2.3f),0.05f,0.85f);
        const auto mode=neuralConnectionMode(cn);
        Color base=mode==BrainConnectionMode::Gate?Color{195,100,245,255}:mode==BrainConnectionMode::Modulate?Color{80,225,155,255}:mode==BrainConnectionMode::Shunt?Color{235,85,90,255}:(neuralConnectionPlastic(cn)?Color{80,195,245,255}:(rec?ORANGE:WHITE));
        Color lc=selected?Color{255,220,90,245}:Fade(base,std::max(rec?0.12f:0.05f,alpha));
        const bool selfLoop=(cn.srcKind==BrainNodeKind::Hidden && cn.dstKind==BrainNodeKind::Hidden && cn.src==cn.dst);
        if(selfLoop) DrawCircleLinesV(a,selected?9.0f:6.5f,lc);
        else DrawLineEx(a,b,selected?2.2f:(rec?1.2f:0.8f)+clampf(flow,0.0f,1.0f)*0.8f,lc);

        float hitDist=selfLoop?pointCircleStrokeDistance(mouse,a,selected?9.0f:6.5f):pointSegmentDistance(mouse,a,b);
        const float hitThreshold=selfLoop?7.0f:6.5f;
        if(hitDist<=hitThreshold && hitDist<hoverConnectionDist){ hoverConnectionDist=hitDist; hoverConnection=ci; }
        ++drawnConnections;
    }

    // pequeno resumo ao vivo do fluxo mental mais forte.
    std::vector<std::pair<float,int>> topInputs, topOutputs;
    topInputs.reserve(cfg::INPUT_COUNT); topOutputs.reserve(cfg::OUTPUT_COUNT);
    for(int i=0;i<cfg::INPUT_COUNT;++i) if(brainInputActive(g,i)) topInputs.push_back({std::fabs(c.inputs[(std::size_t)i]),i});
    for(int o=0;o<cfg::OUTPUT_COUNT;++o) if(brainOutputActive(g,o)) topOutputs.push_back({std::fabs(c.netOut[(std::size_t)o]),o});
    std::sort(topInputs.begin(),topInputs.end(),[](const auto& a,const auto& b){return a.first>b.first;});
    std::sort(topOutputs.begin(),topOutputs.end(),[](const auto& a,const auto& b){return a.first>b.first;});

    DrawText(TextFormat("%d/%d entradas | %d/%d saidas | %d ocultos | %d conexoes | %d loops",
        brainActiveInputCount(g),cfg::INPUT_COUNT,brainActiveOutputCount(g),cfg::OUTPUT_COUNT,g.hiddenCount,c.compiled.activeConnections,c.compiled.recurrentConnections),
        (int)area.x+12,(int)area.y+10,12,Color{185,190,205,255});
    DrawText("Passe o mouse para ver nome, valor e fluxo. Clique em um neuronio/ligacao para fixar no editor.",(int)area.x+12,(int)area.y+26,10,Color{145,155,175,255});

    std::string flowInputs="Entradas fortes: ";
    for(int k=0;k<std::min<int>(3,(int)topInputs.size());++k){
        const int idx=topInputs[(std::size_t)k].second;
        if(k) flowInputs+=" | ";
        flowInputs+=brainInputLabel(idx)+" "+Simulation::f2(c.inputs[(std::size_t)idx]);
    }
    if(topInputs.empty()) flowInputs+="nenhuma";
    DrawText(flowInputs.c_str(),(int)area.x+12,(int)area.y+42,10,Color{155,195,225,255});

    std::string flowOutputs="Saidas fortes: ";
    for(int k=0;k<std::min<int>(3,(int)topOutputs.size());++k){
        const int idx=topOutputs[(std::size_t)k].second;
        if(k) flowOutputs+=" | ";
        flowOutputs+=brainOutputLabel(idx)+" "+Simulation::f2(c.netOut[(std::size_t)idx]);
    }
    if(topOutputs.empty()) flowOutputs+="nenhuma";
    DrawText(flowOutputs.c_str(),(int)area.x+12,(int)area.y+54,10,Color{235,190,145,255});

    // Entradas agregadas: um ponto por raio + entradas internas/novas percepcoes.
    for(int group=0;group<leftCount;++group){
        Vector2 pos{lx,yFor(group,leftCount)};
        const bool active=brainInputGroupActive(g,group);
        const float v=brainInputGroupDisplayValue(c,g,group);
        const float av=std::fabs(v);
        const Color semantic=groupColor(group);
        const float radius=active?(3.2f+av*3.1f):1.8f;
        DrawCircleV(pos,radius,active?semantic:Color{50,52,58,180});
        if(pointIn({pos.x-8,pos.y-8,16,16},mouse)){ hoverInputGroup=group; DrawCircleLinesV(pos,8.5f,Color{255,255,255,210}); }

        const std::string shortLabel=brainInputGroupShortLabel(group);
        DrawText(shortLabel.c_str(),(int)pos.x-34,(int)pos.y-5,9,active?Color{205,215,230,235}:Color{95,100,110,185});
        const float barW=20.0f;
        RectF bar{pos.x+8.0f,pos.y-2.5f,barW,5.0f};
        DrawRectangleRounded(bar,0.5f,4,Color{25,28,34,220});
        DrawRectangleRounded({bar.x,bar.y,barW*clampf(av,0.0f,1.0f),bar.height},0.5f,4,active?Fade(semantic,0.95f):Color{70,70,75,150});
    }

    bool clickedHidden=false;
    for(int h=0;h<g.hiddenCount;++h){
        const bool selected=h==ui.brainHiddenIndex;
        const float state=h<(int)c.hidden.size()?c.hidden[(std::size_t)h]:0.0f;
        const float activation=std::fabs(state);
        const float radius=selected?8.0f:4.3f+activation*2.3f;
        BrainArea ba=hiddenNodeArea(g.hiddenNodes[(std::size_t)h]);
        Color ac=ba==BrainArea::Vision?Color{100,180,255,255}:ba==BrainArea::Reserved?Color{95,95,105,255}:ba==BrainArea::Memory?Color{190,120,240,255}:ba==BrainArea::Emotion?Color{240,110,180,255}:ba==BrainArea::Motor?Color{255,170,80,255}:ba==BrainArea::Social?Color{120,230,145,255}:ba==BrainArea::Internal?Color{220,210,110,255}:SKYBLUE;
        DrawCircleV(hiddenPos[h],radius,selected?Color{255,220,90,255}:Fade(ac,0.94f));
        const Color signCol=state>=0.0f?Color{110,235,155,220}:Color{245,120,120,220};
        DrawCircleLinesV(hiddenPos[h],radius+1.2f,Fade(signCol,0.45f+0.25f*activation));
        DrawText(TextFormat("H%d",h),(int)hiddenPos[h].x-9,(int)hiddenPos[h].y-16,9,Color{220,225,235,225});
        if(pointIn({hiddenPos[h].x-10,hiddenPos[h].y-10,20,20},mouse)){
            hoverHidden=h;
            DrawCircleLinesV(hiddenPos[h],10.0f,Color{255,255,255,210});
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){ ui.brainHiddenIndex=h; clickedHidden=true; }
        }
    }

    if(hoverConnection>=0){
        const auto& cn=g.connections[(std::size_t)hoverConnection];
        Vector2 a=endpoint(cn.srcKind,cn.src), b=endpoint(cn.dstKind,cn.dst);
        const bool selfLoop=(cn.srcKind==BrainNodeKind::Hidden && cn.dstKind==BrainNodeKind::Hidden && cn.src==cn.dst);
        if(selfLoop) DrawCircleLinesV(a,11.5f,Color{255,255,255,210});
        else DrawLineEx(a,b,3.2f,Color{255,255,255,85});
    }
    if(hoverConnection>=0 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !clickedHidden){
        ui.brainConnectionIndex=hoverConnection;
        const auto& cn=g.connections[(std::size_t)hoverConnection];
        if(cn.srcKind==BrainNodeKind::Hidden) ui.brainHiddenIndex=std::clamp(cn.src,0,std::max(0,g.hiddenCount-1));
        else if(cn.dstKind==BrainNodeKind::Hidden) ui.brainHiddenIndex=std::clamp(cn.dst,0,std::max(0,g.hiddenCount-1));
    }

    const char* outNames[cfg::OUTPUT_COUNT]={"F","R","GE","GD","LE","LD","AG","MO","--","AC","cR","cG","cB","W0","W1","W2","W3","G0","G1","G2","G3"};
    for(int o=0;o<oCount;++o){
        Vector2 pos{ox,yFor(o,oCount)};
        const bool active=brainOutputActive(g,o);
        const float v=active?c.netOut[(std::size_t)o]:0.0f;
        const float av=std::fabs(v);
        DrawCircleV(pos,active?(5.0f+av*2.4f):3.6f,active?Fade(ORANGE,0.96f):Color{58,58,64,190});
        DrawText(outNames[o],(int)pos.x+10,(int)pos.y-6,11,active?Color{235,220,185,255}:Color{90,92,100,180});
        RectF bar{pos.x-28.0f,pos.y-2.5f,20.0f,5.0f};
        DrawRectangleRounded(bar,0.5f,4,Color{25,28,34,220});
        DrawRectangleRounded({bar.x,bar.y,bar.width*clampf(av,0.0f,1.0f),bar.height},0.5f,4,active?Color{255,175,90,235}:Color{70,70,75,150});
        if(pointIn({pos.x-9,pos.y-9,18,18},mouse)){ hoverOutput=o; DrawCircleLinesV(pos,9.5f,Color{255,255,255,210}); }
    }

    if(!c.genome.emotions.emotions.empty()){
        std::string emotional="Emocoes: ";
        for(int i=0;i<(int)c.genome.emotions.emotions.size()&&i<cfg::MAX_EMOTIONS;++i){
            const auto& e=c.genome.emotions.emotions[(std::size_t)i];
            const float pct=e.strength>0?clampf(c.emotionState[(std::size_t)i]/e.strength,0,1)*100.0f:0.0f;
            emotional += "E"+std::to_string(i)+" "+Simulation::f2(pct)+"%  ";
        }
        DrawText(emotional.c_str(),(int)area.x+12,(int)(area.y+area.height-38),10,Color{205,145,235,255});
    }
    DrawText(TextFormat("Legenda: cor = area/tipo | tamanho = intensidade atual | linhas = influencia | exibindo %d/%d lig.",drawnConnections,activeTotal),
        (int)area.x+12,(int)(area.y+area.height-22),10,Color{145,155,175,255});

    std::vector<std::string> hoverLines;
    if(hoverConnection>=0){
        const auto& cn=g.connections[(std::size_t)hoverConnection];
        const float src=sourceState(cn);
        const float transformed=applyBrainLinkFunction(cn.function,src,cn.paramA,cn.paramB);
        const float w=hoverConnection<(int)c.learnedWeights.size()?c.learnedWeights[(std::size_t)hoverConnection]:cn.weight;
        const float contrib=transformed*w;
        hoverLines.push_back("Ligacao "+std::to_string(hoverConnection+1)+": "+brainDestinationLabel(cn.srcKind,cn.src)+" -> "+brainDestinationLabel(cn.dstKind,cn.dst));
        hoverLines.push_back(std::string("Modo ")+brainConnectionModeName(neuralConnectionMode(cn))+" | Funcao "+brainLinkFunctionName(cn.function)+(connectionIsRecurrent(g,cn)?" | LOOP":""));
        hoverLines.push_back("src "+Simulation::f2(src)+" -> func "+Simulation::f2(transformed)+" x peso "+Simulation::f2(w));
        hoverLines.push_back("contribuicao atual: "+Simulation::f2(contrib)+(neuralConnectionPlastic(cn)?" | plastica":" | fixa"));
    }else if(hoverHidden>=0){
        const auto& n=g.hiddenNodes[(std::size_t)hoverHidden];
        hoverLines.push_back("Neuronio H"+std::to_string(hoverHidden));
        hoverLines.push_back(std::string("Area ")+brainAreaName(hiddenNodeArea(n))+" | ativacao "+brainActivationName(hiddenNodeActivation(n)));
        hoverLines.push_back("estado atual "+Simulation::f2(hoverHidden<(int)c.hidden.size()?c.hidden[(std::size_t)hoverHidden]:0.0f)+" | bias "+Simulation::f2(n.bias));
        hoverLines.push_back("ganho "+Simulation::f2(n.gain)+" | memoria "+Simulation::f2(n.memory)+" | camada "+std::to_string(hiddenNodeLayer(n)));
        float bestIn=0.0f; int bestInIdx=-1; float bestOut=0.0f; int bestOutIdx=-1;
        for(int ci=0;ci<(int)g.connections.size();++ci){
            const auto& cn=g.connections[(std::size_t)ci]; if(!neuralConnectionEnabled(cn)) continue;
            if(cn.dstKind==BrainNodeKind::Hidden && cn.dst==hoverHidden){ const float v=std::fabs(connectionContribution(ci)); if(v>bestIn){ bestIn=v; bestInIdx=ci; } }
            if(cn.srcKind==BrainNodeKind::Hidden && cn.src==hoverHidden){ const float v=std::fabs(connectionContribution(ci)); if(v>bestOut){ bestOut=v; bestOutIdx=ci; } }
        }
        if(bestInIdx>=0){ const auto& cn=g.connections[(std::size_t)bestInIdx]; hoverLines.push_back("entrada mais forte: "+brainDestinationLabel(cn.srcKind,cn.src)+" -> H"+std::to_string(hoverHidden)+" ("+Simulation::f2(connectionContribution(bestInIdx))+")"); }
        if(bestOutIdx>=0){ const auto& cn=g.connections[(std::size_t)bestOutIdx]; hoverLines.push_back("saida mais forte: H"+std::to_string(hoverHidden)+" -> "+brainDestinationLabel(cn.dstKind,cn.dst)+" ("+Simulation::f2(connectionContribution(bestOutIdx))+")"); }
    }else if(hoverOutput>=0){
        hoverLines.push_back("Saida "+brainOutputLabel(hoverOutput)+" ["+std::string(outNames[hoverOutput])+"]");
        hoverLines.push_back("valor atual: "+Simulation::f2(c.netOut[(std::size_t)hoverOutput])+(brainOutputActive(g,hoverOutput)?" | gene ativo":" | gene OFF"));
        float bestIn=0.0f; int bestInIdx=-1;
        for(int ci=0;ci<(int)g.connections.size();++ci){
            const auto& cn=g.connections[(std::size_t)ci];
            if(!neuralConnectionEnabled(cn) || cn.dstKind!=BrainNodeKind::Output || cn.dst!=hoverOutput) continue;
            const float v=std::fabs(connectionContribution(ci)); if(v>bestIn){ bestIn=v; bestInIdx=ci; }
        }
        if(bestInIdx>=0){ const auto& cn=g.connections[(std::size_t)bestInIdx]; hoverLines.push_back("maior influencia: "+brainDestinationLabel(cn.srcKind,cn.src)+" -> "+brainOutputLabel(hoverOutput)+" ("+Simulation::f2(connectionContribution(bestInIdx))+")"); }
    }else if(hoverInputGroup>=0){
        hoverLines.push_back("Entrada/grupo "+brainInputGroupShortLabel(hoverInputGroup));
        if(hoverInputGroup<cfg::MAX_RAYS){
            hoverLines.push_back("Raio visual "+std::to_string(hoverInputGroup)+" | valor exibido "+Simulation::f2(brainInputGroupDisplayValue(c,g,hoverInputGroup)));
            const int base=hoverInputGroup*cfg::INPUTS_PER_RAY;
            std::vector<std::pair<float,int>> fields;
            for(int k=0;k<cfg::INPUTS_PER_RAY;++k){ int idx=base+k; if(idx<cfg::INPUT_COUNT && brainInputActive(g,idx)) fields.push_back({std::fabs(c.inputs[(std::size_t)idx]),idx}); }
            std::sort(fields.begin(),fields.end(),[](const auto& a,const auto& b){return a.first>b.first;});
            for(int i=0;i<std::min<int>(4,(int)fields.size());++i){ int idx=fields[(std::size_t)i].second; hoverLines.push_back(brainInputLabel(idx)+": "+Simulation::f2(c.inputs[(std::size_t)idx])); }
        }else{
            const int idx=cfg::MAX_RAYS*cfg::INPUTS_PER_RAY+(hoverInputGroup-cfg::MAX_RAYS);
            hoverLines.push_back(brainInputLabel(idx));
            hoverLines.push_back("valor atual: "+Simulation::f2(idx>=0&&idx<cfg::INPUT_COUNT?c.inputs[(std::size_t)idx]:0.0f)+(brainInputActive(g,idx)?" | gene ativo":" | gene OFF"));
            hoverLines.push_back(std::string("area ")+shortAreaName(brainAreaForInput(idx)));
        }
    }
    if(!hoverLines.empty()) drawBrainHoverCard(area,mouse,hoverLines);
}


std::string memoryEventName(MemoryEventType t){
    switch(t){
        case MemoryEventType::Food:return "Comida";
        case MemoryEventType::Attacked:return "Foi atacado";
        case MemoryEventType::AttackHit:return "Ataque acertou";
        case MemoryEventType::EggLaid:return "Botou ovo";
        case MemoryEventType::LegacyRemoved:return "Legado removido";
        default:return "Vazia";
    }
}

const char* rayTypeName(RayType t){
    switch(t){case RayType::Plant:return "Planta";case RayType::Creature:return "Bicho";case RayType::Carcass:return "Carne";case RayType::Egg:return "Ovo";default:return "Nada";}
}
const char* rayTypeShort(RayType t){
    switch(t){case RayType::Plant:return "PL";case RayType::Creature:return "BI";case RayType::Carcass:return "CA";case RayType::Egg:return "OV";default:return "--";}
}

void drawVisionLab(Simulation& sim,UIState& ui,int sw,int sh){
    if(!ui.visionLabOpen)return;
    Creature* c=sim.selected();
    DrawRectangle(0,0,sw,sh,Color{0,0,0,185});
    const float pw=std::min(1120.0f,(float)sw-50.0f),ph=std::min(520.0f,(float)sh-60.0f);
    RectF panel{(sw-pw)*0.5f,(sh-ph)*0.5f,pw,ph};
    DrawRectangleRounded(panel,0.025f,6,Color{10,11,15,250});
    DrawRectangleRoundedLines(panel,0.025f,6,1.2f,Color{105,130,175,230});
    RectF closeB{panel.x+panel.width-42,panel.y+10,30,28};
    if(button(closeB,"X")){ui.visionLabOpen=false;return;}
    if(!c||c->dead()){DrawText("Criatura indisponivel",(int)panel.x+20,(int)panel.y+55,15,RAYWHITE);return;}
    DrawText(TextFormat("RETINA 1D - Criatura #%d",c->id),(int)panel.x+18,(int)panel.y+14,16,RAYWHITE);
    DrawText(TextFormat("%d raios | alcance %.0f | esquerda -> direita | cor perde intensidade com a distancia",(int)c->rays.size(),c->genome.vision.range),(int)panel.x+18,(int)panel.y+38,11,Color{165,185,215,255});
    DrawText(TextFormat("Percepcao continua PLANTA: presente %.0f%% | direcao %+.3f | proximidade %.0f%% | boca %.0f%% | genes %s/%s/%s/%s",
        c->inputs[cfg::PERCEPT_PLANT_PRESENT]*100.0f,c->inputs[cfg::PERCEPT_PLANT_DIRECTION],c->inputs[cfg::PERCEPT_PLANT_NEARNESS]*100.0f,c->inputs[cfg::PERCEPT_PLANT_MOUTH_CONTACT]*100.0f,
        brainInputActive(c->genome.brain,cfg::PERCEPT_PLANT_PRESENT)?"ON":"--",brainInputActive(c->genome.brain,cfg::PERCEPT_PLANT_DIRECTION)?"ON":"--",brainInputActive(c->genome.brain,cfg::PERCEPT_PLANT_NEARNESS)?"ON":"--",brainInputActive(c->genome.brain,cfg::PERCEPT_PLANT_MOUTH_CONTACT)?"ON":"--"),
        (int)panel.x+18,(int)panel.y+59,10,Color{175,205,175,255});

    const float rx=panel.x+18.0f,ry=panel.y+102.0f,rw=panel.width-36.0f,rh=panel.height-170.0f;
    DrawRectangle((int)rx,(int)ry,(int)rw,(int)rh,Color{0,0,0,255});
    const int n=std::max(1,(int)c->rays.size());
    const float stripeW=rw/(float)n;
    Vector2 mouse=GetMousePosition();int hover=-1;
    for(int i=0;i<n;++i){
        const RayReading& rr=c->rays[(std::size_t)i];
        const float nearStrength=rr.type==RayType::None?0.0f:std::pow(clampf(1.0f-rr.normalizedDistance,0.0f,1.0f),0.62f);
        Color base=rr.targetColor;
        if(rr.type==RayType::Carcass)base=cfg::CARCASS_COLOR;
        else if(rr.type==RayType::Egg)base=Color{238,218,150,255};
        if(rr.type==RayType::None)base=Color{0,0,0,255};
        Color seen{(unsigned char)std::lround((float)base.r*nearStrength),(unsigned char)std::lround((float)base.g*nearStrength),(unsigned char)std::lround((float)base.b*nearStrength),255};
        RectF stripe{rx+stripeW*i,ry,stripeW+0.5f,rh};
        DrawRectangle((int)stripe.x,(int)stripe.y,(int)std::ceil(stripe.width),(int)stripe.height,seen);
        DrawRectangleLinesEx(stripe,std::max(0.5f,1.0f),Color{255,255,255,45});
        const char* label=stripeW>=48.0f?rayTypeName(rr.type):rayTypeShort(rr.type);
        const int fs=stripeW>=48.0f?10:9;const int tw=MeasureText(label,fs);
        DrawText(label,(int)(stripe.x+stripe.width*0.5f-tw*0.5f),(int)ry-17,fs,Color{210,215,225,255});
        if(pointIn(stripe,mouse))hover=i;
    }
    DrawRectangleLinesEx({rx,ry,rw,rh},1.2f,Color{150,165,195,210});
    DrawText("ESQUERDA",(int)rx,(int)(ry+rh+10),10,Color{145,155,175,255});
    const char* right="DIREITA";DrawText(right,(int)(rx+rw-MeasureText(right,10)),(int)(ry+rh+10),10,Color{145,155,175,255});
    if(hover>=0){
        const RayReading& rr=c->rays[(std::size_t)hover];
        const float pct=100.0f*(1.0f-rr.normalizedDistance);
        DrawText(TextFormat("Raio %d | %s | distancia %.1f / %.1f | intensidade visual %.0f%% | RGB %d,%d,%d",hover+1,rayTypeName(rr.type),rr.distance,c->genome.vision.range,rr.type==RayType::None?0.0f:pct,(int)rr.targetColor.r,(int)rr.targetColor.g,(int)rr.targetColor.b),(int)panel.x+18,(int)(panel.y+panel.height-34),11,Color{220,225,235,255});
    }else{
        DrawText("Passe o mouse sobre uma faixa para ver tipo, distancia e cor recebida pelo raio.",(int)panel.x+18,(int)(panel.y+panel.height-34),11,Color{175,185,205,255});
    }
}

void drawMemoryLab(Simulation& sim, UIState& ui, int sw, int sh){
    if(!ui.memoryLabOpen) return;
    DrawRectangle(0,0,sw,sh,Color{0,0,0,210});
    RectF panel{28.0f,42.0f,std::max(620.0f,(float)sw-56.0f),std::max(440.0f,(float)sh-84.0f)};
    if(panel.width>sw-18.0f)panel.width=sw-18.0f;
    if(panel.height>sh-18.0f)panel.height=sh-18.0f;
    panel.x=((float)sw-panel.width)*0.5f;panel.y=((float)sh-panel.height)*0.5f;
    DrawRectangleRounded(panel,0.018f,5,Color{8,10,14,253});
    DrawRectangleRoundedLines(panel,0.018f,5,1.0f,Color{105,90,155,235});
    Creature* c=sim.selected();
    const std::string title=c?"Memorias - Criatura #"+std::to_string(c->id):"Memorias - sem criatura viva";
    DrawText(title.c_str(),(int)panel.x+18,(int)panel.y+14,20,RAYWHITE);
    if(button({panel.x+panel.width-104,panel.y+10,86,30},"Fechar")){ui.memoryLabOpen=false;return;}
    if(!c){DrawText("O individuo observado morreu ou deixou de ser seguido.",(int)panel.x+22,(int)panel.y+72,14,Color{190,190,205,255});return;}

    RectF view{panel.x+14,panel.y+54,panel.width-28,panel.height-68};
    const float contentH=680.0f;
    const float maxScroll=std::max(0.0f,contentH-view.height);
    if(pointIn(view,GetMousePosition())){const float wheel=GetMouseWheelMove();if(wheel!=0.0f)ui.memoryLabScroll=clampf(ui.memoryLabScroll-wheel*48.0f,0.0f,maxScroll);}
    ui.memoryLabScroll=clampf(ui.memoryLabScroll,0.0f,maxScroll);
    BeginScissorMode((int)view.x,(int)view.y,(int)view.width,(int)view.height);
    float x=view.x+10,y=view.y+8-ui.memoryLabScroll;

    DrawText("MEMORIA PERSISTENTE - registradores que o proprio cerebro escreve",(int)x,(int)y,15,Color{205,165,245,255});y+=26;
    const int cap=brainRegisterCapacity(c->genome.brain);
    for(int i=0;i<cfg::REGISTER_MEMORY_SLOTS;++i){
        const float v=clampf(c->memoryRegisters[(std::size_t)i],0.0f,1.0f);
        RectF bar{x,y,260,13};DrawRectangleRounded(bar,0.35f,5,Color{34,34,43,245});
        if(i<cap)DrawRectangleRounded({bar.x,bar.y,bar.width*v,bar.height},0.35f,5,Color{155,100,220,245});
        DrawText(TextFormat("R%d  %.3f  %s",i,v,i<cap?"ativo":"fora da capacidade genetica"),(int)x+274,(int)y-1,11,i<cap?RAYWHITE:Color{120,120,130,255});
        y+=24;
    }
    y+=10;

    DrawText("MEMORIA EPISODICA - eventos concretos recentes",(int)x,(int)y,15,Color{145,195,250,255});y+=25;
    DrawText(TextFormat("%d/%d eventos armazenados",(int)c->longMemoryCount,cfg::LONG_MEMORY_SLOTS),(int)x,(int)y,11,Color{165,175,195,255});y+=20;
    if(c->longMemoryCount==0){DrawText("Nenhuma memoria episodica registrada ainda.",(int)x+8,(int)y,12,Color{150,150,165,255});y+=34;}
    for(int recent=0;recent<(int)c->longMemoryCount;++recent){
        const int slot=(int(c->longMemoryHead)-1-recent+cfg::LONG_MEMORY_SLOTS)%cfg::LONG_MEMORY_SLOTS;
        const auto& e=c->longMemory[(std::size_t)slot];
        const float age=std::max(0.0f,sim.simTime-e.time);
        const float front=e.worldDir.x*c->forward().x+e.worldDir.y*c->forward().y;
        const float right=e.worldDir.x*c->right().x+e.worldDir.y*c->right().y;
        RectF card{x,y,view.width-30,72};
        DrawRectangleRounded(card,0.025f,5,Color{18,21,29,245});
        DrawRectangleRoundedLines(card,0.025f,5,1.0f,Color{62,78,105,220});
        DrawText(TextFormat("%d. %s",recent+1,memoryEventName(e.type).c_str()),(int)card.x+10,(int)card.y+8,13,RAYWHITE);
        DrawText(TextFormat("ha %.2fs | intensidade %.3f",age,e.intensity),(int)card.x+10,(int)card.y+29,11,Color{180,190,210,255});
        DrawText(TextFormat("direcao lembrada agora: frente %+.2f | direita %+.2f",front,right),(int)card.x+10,(int)card.y+47,10,Color{150,170,195,255});
        y+=82;
    }
    y+=8;

    DrawText("MEMORIA RECORRENTE - estado interno dos neuronios",(int)x,(int)y,15,Color{240,180,110,255});y+=25;
    int recurrentNodes=0;
    for(int h=0;h<c->genome.brain.hiddenCount;++h)if(c->genome.brain.hiddenNodes[(std::size_t)h].memory>0.01f)++recurrentNodes;
    DrawText(TextFormat("%d neuronios com memoria > 0 | %d loops neurais ativos",recurrentNodes,c->compiled.recurrentConnections),(int)x,(int)y,11,Color{180,185,200,255});y+=22;
    int shown=0;
    for(int h=0;h<c->genome.brain.hiddenCount && shown<16;++h){
        const auto& n=c->genome.brain.hiddenNodes[(std::size_t)h];
        if(n.memory<=0.01f)continue;
        const float state=h<(int)c->hidden.size()?c->hidden[(std::size_t)h]:0.0f;
        DrawText(TextFormat("H%-2d  memoria %.2f | estado atual %+.3f | area %s",h,n.memory,state,brainAreaName(hiddenNodeArea(n))),(int)x+8,(int)y,10,Color{195,185,165,255});y+=16;++shown;
    }
    if(shown==0)DrawText("Nenhum neuronio oculto mantem estado recorrente relevante.",(int)x+8,(int)y,10,Color{145,145,155,255});
    EndScissorMode();
}

void drawBrainEmotionTab(Creature& c, UIState& ui, RectF body){
    const int count=std::min<int>((int)c.genome.emotions.emotions.size(),cfg::MAX_EMOTIONS);
    DrawRectangleRounded(body,0.018f,5,Color{12,12,18,248});
    DrawRectangleRoundedLines(body,0.018f,5,1.0f,Color{100,65,120,220});
    if(count<=0){
        DrawText("Este individuo nao possui emocoes no DNA.",(int)body.x+18,(int)body.y+22,16,Color{205,205,215,255});
        DrawText("Ausencia de modulo emocional tambem e uma estrategia evolutiva valida.",(int)body.x+18,(int)body.y+50,11,Color{155,160,175,255});
        return;
    }
    ui.emotionLabIndex=std::clamp(ui.emotionLabIndex,0,count-1);
    const float listW=std::clamp(body.width*0.25f,190.0f,250.0f);
    RectF list{body.x+10,body.y+10,listW,body.height-20};
    RectF detail{list.x+list.width+12,list.y,body.width-list.width-32,list.height};
    DrawRectangleRounded(list,0.025f,5,Color{18,15,23,245});
    DrawRectangleRounded(detail,0.018f,5,Color{15,16,22,245});
    const float cardStep=clampf((list.height-20.0f)/float(std::max(1,count)),42.0f,66.0f);
    for(int i=0;i<count;++i){
        const auto& e=c.genome.emotions.emotions[(std::size_t)i];
        const float active=e.strength>0?clampf(c.emotionState[(std::size_t)i]/e.strength,0.0f,1.0f):0.0f;
        RectF card{list.x+8,list.y+8+i*cardStep,list.width-16,std::max(36.0f,cardStep-6.0f)};
        const bool sel=i==ui.emotionLabIndex;
        DrawRectangleRounded(card,0.10f,5,sel?Color{58,34,72,248}:Color{27,24,32,245});
        DrawText(TextFormat("E%d  ativa %.0f%%",i,active*100.0f),(int)card.x+9,(int)card.y+7,12,sel?Color{240,195,255,255}:RAYWHITE);
        RectF bar{card.x+9,card.y+26,card.width-18,6};DrawRectangleRounded(bar,0.4f,5,Color{39,34,45,240});
        DrawRectangleRounded({bar.x,bar.y,bar.width*active,bar.height},0.4f,5,Color{175,92,216,248});
        if(pointIn(card,GetMousePosition())&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){ui.emotionLabIndex=i;ui.emotionLabScroll=0.0f;}
    }
    const int ei=ui.emotionLabIndex;const auto& e=c.genome.emotions.emotions[(std::size_t)ei];
    const float active=e.strength>0?clampf(c.emotionState[(std::size_t)ei]/e.strength,0.0f,1.0f):0.0f;
    const float neuralSignal=c.inputs[cfg::EMOTION_INPUT_BASE+ei];
    const float contentH=720.0f,maxScroll=std::max(0.0f,contentH-detail.height+20.0f);
    if(pointIn(detail,GetMousePosition())){const float wheel=GetMouseWheelMove();if(wheel!=0.0f)ui.emotionLabScroll=clampf(ui.emotionLabScroll-wheel*45.0f,0.0f,maxScroll);}
    BeginScissorMode((int)detail.x+2,(int)detail.y+2,(int)detail.width-4,(int)detail.height-4);
    float x=detail.x+14,y=detail.y+14-ui.emotionLabScroll;
    DrawText(TextFormat("E%d - estado emocional",ei),(int)x,(int)y,18,Color{235,190,250,255});y+=28;
    DrawText(TextFormat("ativa %.1f%% | estado %.3f / %.3f | drive %.1f%% | limiar %.1f%%",active*100.0f,c.emotionState[(std::size_t)ei],e.strength,c.emotionDrive[(std::size_t)ei]*100.0f,e.threshold*100.0f),(int)x,(int)y,11,RAYWHITE);y+=19;
    DrawText(TextFormat("sinal ao cerebro %.3f | ganho %.2f | impacto %.2f | subida %.2fs | duracao %.2fs | bias %.2f",neuralSignal,e.gain,e.impact,e.riseTime,e.duration,e.bias),(int)x,(int)y,10,Color{170,175,195,255});y+=30;
    DrawText("Gatilhos mais fortes agora",(int)x,(int)y,14,Color{225,210,235,255});y+=20;
    struct TriggerView{float absC,con,value,weight;std::string label;};std::vector<TriggerView> tv;tv.reserve(e.triggers.size());
    const auto previous=c.emotionState;
    auto sourceValue=[&](const EmotionTriggerGene& t){
        if(t.sourceKind==EmotionSourceKind::Input)return (t.source>=0&&t.source<cfg::BASE_INPUT_COUNT&&brainInputActive(c.genome.brain,t.source))?c.inputs[(std::size_t)t.source]:0.0f;
        if(t.sourceKind==EmotionSourceKind::Hidden)return (t.source>=0&&t.source<(int)c.hidden.size())?c.hidden[(std::size_t)t.source]:0.0f;
        if(t.sourceKind==EmotionSourceKind::Output)return (t.source>=0&&t.source<cfg::OUTPUT_COUNT&&brainOutputActive(c.genome.brain,t.source))?c.netOut[(std::size_t)t.source]:0.0f;
        if(t.source>=0&&t.source<count){const float m=std::max(0.01f,c.genome.emotions.emotions[(std::size_t)t.source].strength);return clampf(previous[(std::size_t)t.source]/m,0.0f,1.0f);}return 0.0f;};
    for(const auto& t:e.triggers){const float v=sourceValue(t),con=v*t.weight;tv.push_back({std::fabs(con),con,v,t.weight,emotionSourceLabel(t)});}std::sort(tv.begin(),tv.end(),[](const auto&a,const auto&b){return a.absC>b.absC;});
    for(int i=0;i<std::min<int>(8,(int)tv.size());++i){const auto&t=tv[(std::size_t)i];DrawText(TextFormat("%+.3f  %s (%.2f x %+.2f)",t.con,t.label.c_str(),t.value,t.weight),(int)x+6,(int)y,10,t.con>=0?Color{170,225,180,255}:Color{230,165,165,255});y+=15;}y+=12;
    DrawText("Efeito motor imediato estimado",(int)x,(int)y,14,Color{225,210,235,255});y+=20;
    const auto full=brainSnapshotMotor(c,-1),muted=brainSnapshotMotor(c,ei);
    static const char* names[cfg::OUTPUT_COUNT]={"Frente","Re","Girar E","Girar D","Lateral E","Lateral D","Agarrar","Morder","Rsv1","Acasalar","Cor R","Cor G","Cor B","MemW0","MemW1","MemW2","MemW3","MemG0","MemG1","MemG2","MemG3"};
    for(int o=0;o<cfg::OUTPUT_COUNT;++o){const float d=full[(std::size_t)o]-muted[(std::size_t)o];DrawText(TextFormat("%-8s efeito %+.3f | com %.3f | sem %.3f",names[o],d,full[(std::size_t)o],muted[(std::size_t)o]),(int)x+6,(int)y,10,d>0.001f?Color{170,225,180,255}:(d<-0.001f?Color{230,165,165,255}:Color{160,160,170,255}));y+=15;}
    y+=12;DrawText("Ligacoes neurais diretas desta emocao",(int)x,(int)y,14,Color{225,210,235,255});y+=20;
    int shown=0;const int emotionInput=cfg::EMOTION_INPUT_BASE+ei;
    for(int ci=0;ci<(int)c.genome.brain.connections.size()&&shown<10;++ci){const auto&cn=c.genome.brain.connections[(std::size_t)ci];if(!neuralConnectionEnabled(cn)||cn.srcKind!=BrainNodeKind::Input||cn.src!=emotionInput)continue;const float w=ci<(int)c.learnedWeights.size()?c.learnedWeights[(std::size_t)ci]:cn.weight;DrawText(TextFormat("%s  peso %+.3f | contrib %+.3f",brainDestinationLabel(cn.dstKind,cn.dst).c_str(),w,neuralSignal*w),(int)x+6,(int)y,10,Color{180,205,190,255});y+=15;++shown;}
    if(shown==0)DrawText("Nenhuma ligacao direta ativa.",(int)x+6,(int)y,10,Color{150,150,165,255});
    EndScissorMode();
}

void drawBrainLab(Simulation& sim, UIState& ui, int sw, int sh) {
    if(!ui.brainLabOpen) return;

    // Modal verdadeiro: escurece tudo que esta atras e impede a sensacao de
    // varias telas empilhadas. O mundo continua visivel apenas como contexto.
    DrawRectangle(0,0,sw,sh,Color{0,0,0,205});

    RectF panel{
        24.0f,
        42.0f,
        std::max(640.0f,(float)sw-48.0f),
        std::max(460.0f,(float)sh-84.0f)
    };
    if(panel.width>(float)sw-20.0f) panel.width=(float)sw-20.0f;
    if(panel.height>(float)sh-20.0f) panel.height=(float)sh-20.0f;
    panel.x=((float)sw-panel.width)*0.5f;
    panel.y=((float)sh-panel.height)*0.5f;

    DrawRectangleRounded(panel,0.018f,5,Color{7,8,11,253});
    DrawRectangleRoundedLines(panel,0.018f,5,1.0f,Color{82,95,125,235});

    Creature* c=sim.selected();
    std::string title=c
        ?"Laboratorio do cerebro - Criatura #"+std::to_string(c->id)
        :"Laboratorio do cerebro - sem criatura viva selecionada";
    DrawText(title.c_str(),(int)panel.x+18,(int)panel.y+14,20,RAYWHITE);

    RectF tabNeural{panel.x+18,panel.y+46,112,28};
    RectF tabEmotion{panel.x+136,panel.y+46,112,28};
    if(button(tabNeural,"Rede neural",ui.brainLabTab==0)){ui.brainLabTab=0;ui.brainSliderId=-1;}
    if(button(tabEmotion,"Emocoes",ui.brainLabTab==1)){ui.brainLabTab=1;ui.brainSliderId=-1;ui.emotionLabScroll=0.0f;}

    RectF closeB{panel.x+panel.width-104,panel.y+10,86,30};
    if(button(closeB,"Fechar")){
        ui.brainLabOpen=false;
        ui.brainSliderId=-1;
        return;
    }

    if(!c){
        DrawText("O individuo observado morreu. Se R/G estiver ativo, a tela acompanha o proximo automaticamente.",
                 (int)panel.x+22,(int)panel.y+70,14,Color{190,190,200,255});
        return;
    }

    RectF tabBody{panel.x+16,panel.y+82,panel.width-32,panel.height-98};
    if(ui.brainLabTab==1){
        drawBrainEmotionTab(*c,ui,tabBody);
        return;
    }

    const float editorW=std::clamp(panel.width*0.31f,280.0f,340.0f);
    RectF graph{
        tabBody.x,
        tabBody.y,
        tabBody.width-editorW-16,
        tabBody.height
    };
    RectF editor{
        graph.x+graph.width+16,
        graph.y,
        editorW,
        graph.height
    };

    drawBrainLabGraph(*c,ui,graph);

    DrawRectangleRounded(editor,0.035f,5,Color{16,17,22,248});
    DrawRectangleRoundedLines(editor,0.035f,5,1.0f,Color{72,82,105,220});

    float x=editor.x+14;
    float y=editor.y+14;
    DrawText("Editor neural",(int)x,(int)y,17,RAYWHITE);
    y+=26;

    auto& g=c->genome.brain;
    const int regCap=brainRegisterCapacity(g);
    DrawText(TextFormat("Registradores %d/%d | plasticas %d | RL tick %+.2f | RL total %+.1f",regCap,cfg::REGISTER_MEMORY_SLOTS,c->compiled.plasticConnections,c->lastPlasticReward,c->reinforcementTotal),(int)x,(int)y,9,Color{175,195,215,255});
    y+=18;
    if(button({x,y,editor.width-28,25},TextFormat("Capacidade memoria persistente: %d (clicar)",regCap))){setBrainRegisterCapacity(g,(regCap+1)%(cfg::REGISTER_MEMORY_SLOTS+1));refreshEditedBrain(*c);}
    y+=34;

    if(g.hiddenCount>0){
        ui.brainHiddenIndex=std::clamp(ui.brainHiddenIndex,0,g.hiddenCount-1);
        RectF prev{x,y,34,28}, next{x+editor.width-62,y,34,28};
        if(button(prev,"<")) ui.brainHiddenIndex=(ui.brainHiddenIndex+g.hiddenCount-1)%g.hiddenCount;
        if(button(next,">")) ui.brainHiddenIndex=(ui.brainHiddenIndex+1)%g.hiddenCount;
        DrawText(TextFormat("H%d | %s | %s",ui.brainHiddenIndex,brainAreaName(hiddenNodeArea(g.hiddenNodes[ui.brainHiddenIndex])),brainActivationName(hiddenNodeActivation(g.hiddenNodes[ui.brainHiddenIndex]))),(int)x+52,(int)y+7,12,Color{225,225,230,255});
        y+=38;

        auto& n=g.hiddenNodes[ui.brainHiddenIndex];
        bool changed=false;
        changed|=brainFloatSlider(ui,200,{x,y,editor.width-28,12},"Bias",n.bias,-3.0f,3.0f); y+=36;
        changed|=brainFloatSlider(ui,201,{x,y,editor.width-28,12},"Ganho",n.gain,0.30f,3.00f); y+=36;
        changed|=brainFloatSlider(ui,202,{x,y,editor.width-28,12},"Memoria",n.memory,0.00f,0.96f); y+=36;

        float layerFloat=(float)hiddenNodeLayer(n);
        if(brainFloatSlider(ui,203,{x,y,editor.width-28,12},"Camada",layerFloat,0.0f,(float)(cfg::MAX_BRAIN_LAYERS-1))){setHiddenNodeLayer(n,(int)std::lround(layerFloat));changed=true;}
        y+=36;
        const float traitHalf=(editor.width-34)*0.5f;
        if(button({x,y,traitHalf,28},(std::string("Area: ")+brainAreaName(hiddenNodeArea(n))).c_str())){setHiddenNodeArea(n,BrainArea((uint8_t(hiddenNodeArea(n))+1)%8));changed=true;}
        if(button({x+traitHalf+6,y,traitHalf,28},(std::string("Ativ: ")+brainActivationName(hiddenNodeActivation(n))).c_str())){setHiddenNodeActivation(n,BrainActivation((uint8_t(hiddenNodeActivation(n))+1)%4));changed=true;}
        y+=36;
        if(changed) refreshEditedBrain(*c);
    }

    DrawLineEx({x,y},{x+editor.width-28,y},1.0f,Color{65,68,80,220});
    y+=12;

    if(!g.connections.empty()){
        ui.brainConnectionIndex=std::clamp(ui.brainConnectionIndex,0,(int)g.connections.size()-1);
        RectF prev{x,y,34,28}, next{x+editor.width-62,y,34,28};
        if(button(prev,"<")) ui.brainConnectionIndex=(ui.brainConnectionIndex+(int)g.connections.size()-1)%(int)g.connections.size();
        if(button(next,">")) ui.brainConnectionIndex=(ui.brainConnectionIndex+1)%(int)g.connections.size();

        auto& cn=g.connections[ui.brainConnectionIndex];
        std::string cnTitle="Lig "+std::to_string(ui.brainConnectionIndex+1)+"/"+std::to_string(g.connections.size());
        DrawText(cnTitle.c_str(),(int)x+48,(int)y+1,12,Color{220,220,225,255});
        DrawText(
            (brainEndpointText(cn.srcKind,cn.src)+" -> "+brainEndpointText(cn.dstKind,cn.dst)).c_str(),
            (int)x+48,(int)y+15,10,Color{155,165,185,255}
        );
        y+=38;

        bool changed=brainFloatSlider(ui,210,{x,y,editor.width-28,12},"Peso",cn.weight,-4.0f,4.0f);
        y+=34;
        const float connHalf=(editor.width-34)*0.5f;
        if(button({x,y,connHalf,30},neuralConnectionEnabled(cn)?"ATIVA":"DESLIGADA",neuralConnectionEnabled(cn))){setNeuralConnectionEnabled(cn,!neuralConnectionEnabled(cn));changed=true;}
        if(button({x+connHalf+6,y,connHalf,30},(std::string("Tipo: ")+brainConnectionModeName(neuralConnectionMode(cn))).c_str())){setNeuralConnectionMode(cn,BrainConnectionMode((uint8_t(neuralConnectionMode(cn))+1)%4));changed=true;}
        y+=34;
        if(button({x,y,editor.width-28,28},(std::string("Funcao: ")+brainLinkFunctionName(cn.function)).c_str())){cn.function=BrainLinkFunction((uint8_t(cn.function)+1)%8);changed=true;}
        y+=32;
        changed|=brainFloatSlider(ui,211,{x,y,editor.width-28,11},"Func param A",cn.paramA,-2.0f,8.0f);y+=28;
        changed|=brainFloatSlider(ui,212,{x,y,editor.width-28,11},"Func param B",cn.paramB,-2.0f,2.0f);y+=30;
        if(button({x,y,connHalf,28},neuralConnectionPlastic(cn)?"PLASTICA":"Fixa",neuralConnectionPlastic(cn))){setNeuralConnectionPlastic(cn,!neuralConnectionPlastic(cn));if(neuralConnectionPlastic(cn)&&neuralConnectionPlasticLevel(cn)==0)setNeuralConnectionPlasticLevel(cn,6);changed=true;}
        if(button({x+connHalf+6,y,connHalf,28},TextFormat("Taxa %d/15",neuralConnectionPlasticLevel(cn)))){setNeuralConnectionPlasticLevel(cn,(neuralConnectionPlasticLevel(cn)+1)%16);changed=true;}
        y+=31;
        if(ui.brainConnectionIndex<(int)c->learnedWeights.size()){
            DrawText(TextFormat("DNA %.3f | aprendido %.3f | elig %.3f",cn.weight,c->learnedWeights[(std::size_t)ui.brainConnectionIndex],ui.brainConnectionIndex<(int)c->eligibilityTraces.size()?c->eligibilityTraces[(std::size_t)ui.brainConnectionIndex]:0.0f),(int)x,(int)y,9,Color{170,190,170,255});
            y+=16;
        }
        if(changed) refreshEditedBrain(*c);
    }

    DrawLineEx({x,y},{x+editor.width-28,y},1.0f,Color{65,68,80,220});
    y+=10;
    DrawText("Bias das saidas",(int)x,(int)y,13,Color{210,210,220,255});
    y+=24;
    const char* outNames[cfg::OUTPUT_COUNT]={"Frente","Re","Girar E","Girar D","Lateral E","Lateral D","Agarrar","Morder","Rsv1","Acasalar","Cor R","Cor G","Cor B","Mem escreve0","Mem escreve1","Mem escreve2","Mem escreve3","Mem gate0","Mem gate1","Mem gate2","Mem gate3"};
    for(int o=0;o<cfg::OUTPUT_COUNT;++o){
        if(y+38>editor.y+editor.height-24) break;
        if(brainFloatSlider(ui,220+o,{x,y,editor.width-28,11},outNames[o],g.outputBiases[o],-3.0f,3.0f))
            refreshEditedBrain(*c);
        y+=30;
    }

    DrawText("Edicoes afetam somente este individuo e passam aos descendentes pelo DNA.",
             (int)x,(int)(editor.y+editor.height-28),9,Color{145,150,165,255});
}

void drawInspector(Simulation& sim, UIState& ui, int sw, int sh, bool& paused) {
    if (!ui.inspectorOpen && !ui.inspectorMinimized) return;
    const float inspectorTop=topControlsBottom(topControlRects(sw),true)+8.0f;
    if (ui.inspectorMinimized) {
        RectF r{(float)sw-258.0f,inspectorTop,246.0f,42.0f};
        DrawRectangleRounded(r,0.20f,6,Color{18,20,25,248});
        DrawRectangleRoundedLines(r,0.20f,6,1.0f,Color{95,110,145,225});
        std::string title = sim.watch.hasFinal
            ? "Criatura #"+std::to_string(sim.watch.creatureId)+" - morreu"
            : "Criatura #"+std::to_string(sim.watch.creatureId);
        DrawText(title.c_str(),(int)r.x+12,(int)r.y+7,12,RAYWHITE);
        DrawText("Clique para restaurar",(int)r.x+12,(int)r.y+24,10,Color{155,170,205,255});
        if (pointIn(r,GetMousePosition()) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            ui.inspectorMinimized=false;
            ui.inspectorOpen=true;
        }
        return;
    }
    float w = std::min(390.0f, sw-20.0f); RectF panel{sw-w-10.0f,inspectorTop,w,std::max(220.0f,(float)sh-inspectorTop-67.0f)};
    DrawRectangleRounded(panel,0.03f,5,Color{12,12,12,245});
    DrawRectangleRoundedLines(panel,0.03f,5,1.0f,Color{90,90,90,210});
    RectF minB{panel.x+panel.width-72,panel.y+8,28,28}, closeB{panel.x+panel.width-38,panel.y+8,28,28};
    const float rowGap=6.0f;
    const float topBtnW=(panel.width-24.0f-rowGap*3.0f)/4.0f;
    RectF brainB{panel.x+12,panel.y+42,topBtnW,28};
    RectF memoryB{brainB.x+topBtnW+rowGap,panel.y+42,topBtnW,28};
    RectF visionB{memoryB.x+topBtnW+rowGap,panel.y+42,topBtnW,28};
    RectF freeEggB{visionB.x+topBtnW+rowGap,panel.y+42,topBtnW,28};
    RectF controlB{panel.x+12,panel.y+76,112,28};
    RectF editTraitsB{panel.x+130,panel.y+76,panel.width-142,28};
    if(sim.selected() && button(brainB,"Cerebro")){
        ui.brainLabOpen=true;
        ui.brainLabTab=0;
        ui.memoryLabOpen=false;
        ui.visionLabOpen=false;
        ui.traitEditorOpen=false;
        ui.brainHiddenIndex=0;
        ui.brainConnectionIndex=0;
        ui.brainSliderId=-1;
        ui.configPopup=false;
        ui.killPopup=false;
        ui.createPopup=false;
        // Nao continua desenhando o inspetor no mesmo frame em que o modal abre.
        return;
    }
    if(sim.selected() && button(memoryB,"Memorias")){
        ui.memoryLabOpen=true;
        ui.memoryLabScroll=0.0f;
        ui.brainLabOpen=false;
        ui.visionLabOpen=false;
        ui.traitEditorOpen=false;
        ui.configPopup=false;
        ui.systemPopup=false;
        ui.killPopup=false;
        ui.createPopup=false;
        return;
    }
    if(sim.selected() && button(visionB,"Visao")){
        ui.visionLabOpen=true;
        ui.brainLabOpen=false;ui.memoryLabOpen=false;ui.traitEditorOpen=false;
        ui.configPopup=false;ui.systemPopup=false;ui.killPopup=false;ui.createPopup=false;
        return;
    }
    if(sim.selected() && button(freeEggB,"Ovo gratis")){
        const int id=sim.selected()->id;
        if(sim.forceFreeEgg(*sim.selected())){
            ui.ioToast="Ovo gratis criado para #"+std::to_string(id);
            ui.ioToastUntil=GetTime()+2.6;
        }else{
            ui.ioToast="Nao foi possivel criar ovo para #"+std::to_string(id);
            ui.ioToastUntil=GetTime()+2.6;
        }
    }
    if(sim.selected()){
        const bool controlling=sim.manualControlCreatureId==sim.selected()->id;
        if(button(controlB,controlling?"Soltar ctrl":"Controlar",controlling)){
            if(controlling){
                sim.stopManualControl();
                ui.ioToast="Controle manual desligado; IA reassumiu #"+std::to_string(sim.selected()->id);
            }else{
                ui.followMode=FollowMode::None;
                // Controle manual assume o teclado: nenhum campo de texto/numerico
                // pode continuar roubando W/A/S/D. Tambem sai do pause automaticamente,
                // pois um animal pausado nao recebe subpassos de movimento.
                ui.noteInputActive=false;
                ui.noteEditTargetId=-1;
                ui.activePopulationField=-1;
                ui.populationEdit.clear();
                paused=false;
                sim.setManualControl(sim.selected());
                ui.ioToast="Controle #"+std::to_string(sim.selected()->id)+": W/S ou setas | A/D ou <-/-> | Q/E lateral | ESPACO/F morder | G agarrar";
            }
            ui.ioToastUntil=GetTime()+4.5;
        }
    }
    if(sim.selected() && button(editTraitsB,"Editar caracteristicas")){
        ui.traitEditorOpen=true;
        ui.traitEditorCategory=0;
        ui.traitEditorScroll=0.0f;
        ui.traitSliderId=-1;
        ui.traitEmotionIndex=0;
        ui.brainLabOpen=false;
        ui.memoryLabOpen=false;
        
        ui.configPopup=false;
        ui.systemPopup=false;
        ui.killPopup=false;
        ui.createPopup=false;
        return;
    }
    if(button(minB,"-")){ui.inspectorOpen=false;ui.inspectorMinimized=true;}
    if(button(closeB,"X")){
        // Fechar o inspetor tambem encerra qualquer modo persistente R/G.
        // Sem isso, maintainFollowMode() escolhe outra criatura no frame seguinte.
        ui.followMode = FollowMode::None;
        ui.brainLabOpen=false;
        ui.memoryLabOpen=false;
        ui.visionLabOpen=false;
        
        ui.traitEditorOpen=false;
        ui.traitSliderId=-1;
        ui.brainSliderId=-1;
        sim.closeWatch();
        ui.inspectorOpen=false;
        ui.inspectorMinimized=false;
        return;
    }
    if(pointIn(panel,GetMousePosition())) { float wheel=GetMouseWheelMove(); if(wheel!=0) ui.inspectorScroll=std::max(0.0f,ui.inspectorScroll-wheel*35.0f); }
    std::string title="Criatura #"+std::to_string(sim.watch.creatureId)+(sim.watch.hasFinal?" - morreu":"");
    DrawText(title.c_str(),(int)panel.x+12,(int)panel.y+14,18,RAYWHITE);
    BeginScissorMode((int)panel.x+8,(int)panel.y+112,(int)panel.width-16,(int)panel.height-120);
    float x=panel.x+12,y=panel.y+118-ui.inspectorScroll;
    Creature* c=sim.selected();
    if(c){
        y=drawLabelValue(x,y,"Controle manual",sim.manualControlCreatureId==c->id?"ATIVO | W/S ou setas | A/D ou setas | Q/E lateral | ESPACO/F morder | G agarrar":"desligado (IA no comando)");
        if(sim.manualControlCreatureId==c->id){
            const bool kW=IsKeyDown('W')||IsKeyDown(KEY_UP), kS=IsKeyDown('S')||IsKeyDown(KEY_DOWN);
            const bool kA=IsKeyDown('A')||IsKeyDown(KEY_LEFT), kD=IsKeyDown('D')||IsKeyDown(KEY_RIGHT);
            const bool kQ=IsKeyDown('Q'), kE=IsKeyDown('E'), kB=IsKeyDown(KEY_SPACE)||IsKeyDown(KEY_F);
            y=drawLabelValue(x,y,"Teclas detectadas",std::string("F ")+(kW?"1":"0")+" | R "+(kS?"1":"0")+" | GE "+(kA?"1":"0")+" | GD "+(kD?"1":"0")+" | LE "+(kQ?"1":"0")+" | LD "+(kE?"1":"0")+" | M "+(kB?"1":"0"));
        }
        y=drawLabelValue(x,y,"Energia",Simulation::f2(c->energy)+" / "+Simulation::f2(c->maxEnergy));
        y=drawLabelValue(x,y,"Vida",Simulation::f2(c->health));
        y=drawLabelValue(x,y,"Geracao",std::to_string(c->generation));
        y=drawLabelValue(x,y,"Especie analitica","S"+std::to_string(c->speciesId)+" (classificacao, nao gene)");
        y=drawLabelValue(x,y,"Pais","#"+std::to_string(c->parentId)+" / #"+std::to_string(c->secondParentId));
        y=drawLabelValue(x,y,"Heranca sexual",c->sexualHeritageDepth>0?(std::to_string(c->sexualHeritageDepth)+" nivel(is); ovo solo e apenas fallback"):"nenhuma ainda");
        y=drawLabelValue(x,y,"Idade",Simulation::f2(c->age)+" s simulados");
        y=drawLabelValue(x,y,"Fase da vida",c->lifeStageName());
        y=drawLabelValue(x,y,"Desenvolvimento",Simulation::f2(c->developmentProgress*100.0f)+"% | adulto "+Simulation::f2(c->adultSize()));
        y=drawLabelValue(x,y,"Maturidade sexual",std::string(c->sexuallyMature()?"SIM":"nao")+" | aos "+Simulation::f2(c->maturityAge())+"s");
        y=drawLabelValue(x,y,"Velhice / senescencia",Simulation::f2(c->senescenceStartAge())+"s | "+Simulation::f2(c->senescenceProgress*100.0f)+"%");
        y=drawLabelValue(x,y,"Longevidade DNA",Simulation::f2(c->genome.development.longevity)+"s | risco atual "+Simulation::f2(c->naturalDeathHazard()*100.0f)+"%/s");
        y=drawLabelValue(x,y,"Idade: vel/met/regen/fert",Simulation::f2(c->ageSpeedMult)+"x / "+Simulation::f2(c->ageMetabolismMult*c->longevityMaintenanceMult)+"x / "+Simulation::f2(c->ageRegenMult)+"x / "+Simulation::f2(c->ageFertilityMult)+"x");
        y=drawLabelValue(x,y,"Tamanho",Simulation::f2(c->size));
        y=drawLabelValue(x,y,"Velocidade frente/re",Simulation::f2(c->speed)+" | max F "+Simulation::f2(c->maxForwardEffective)+" / R "+Simulation::f2(c->maxReverseEffective));
        y=drawLabelValue(x,y,"Velocidade lateral",Simulation::f2(c->lateralSpeed)+" | max "+Simulation::f2(c->maxLateralEffective));
        y=drawLabelValue(x,y,"Velocidade angular",Simulation::f2(c->angularVelocity)+" graus/s");
        y=drawLabelValue(x,y,"Pressao tato F/D/T/E",Simulation::f2(c->contactPressure[0])+" / "+Simulation::f2(c->contactPressure[1])+" / "+Simulation::f2(c->contactPressure[2])+" / "+Simulation::f2(c->contactPressure[3]));
        y=drawLabelValue(x,y,"Feedback recente","impacto "+Simulation::f2(c->recentImpact)+" | dano "+Simulation::f2(c->recentDamage)+" | energia+ "+Simulation::f2(c->recentEnergyGain));
        y=drawLabelValue(x,y,"Memoria persistente",TextFormat("%d slots | [%.2f %.2f %.2f %.2f]",brainRegisterCapacity(c->genome.brain),c->memoryRegisters[0],c->memoryRegisters[1],c->memoryRegisters[2],c->memoryRegisters[3]));
        y=drawLabelValue(x,y,"Plasticidade",TextFormat("%d conexoes | recompensa %.2f | updates %llu",c->compiled.plasticConnections,c->lastPlasticReward,(unsigned long long)c->plasticUpdates));
        y=drawLabelValue(x,y,"Aprendizado com reforco",TextFormat("total %+.2f | +%.2f / -%.2f | eventos %llu",c->reinforcementTotal,c->reinforcementPositive,c->reinforcementNegative,(unsigned long long)c->reinforcementEvents));
        y=drawLabelValue(x,y,"Marcadores pessoais",TextFormat("vida %.1fs | sangue >50 %.1fs / queda50->0 %.1fs | energia >50 %.1fs / queda50->0 %.1fs | ovos %d",c->age,c->timeHealthAbove50,std::fabs(c->timeHealthBelow50),c->timeEnergyAbove50,std::fabs(c->timeEnergyBelow50),c->eggsLaidLifetime));
        y=drawLabelValue(x,y,"Acasalamento",TextFormat("parceiro #%d | contato %.2fs | fallback solo %.0f%% | facilidade %.1fx",c->matingPartnerId,c->matingContactTime,c->matingSearchGrace*100.0f,cfg::tuning.matingEaseFactor));
        y=drawLabelValue(x,y,"Reproducao pronta",TextFormat("solo %s | com parceiro %s | maduro %s | energia %.0f%% | sangue %.0f%%",
            c->canReproduce(1.0f)?"SIM":"nao",
            c->canReproduce(clampf(cfg::tuning.partneredEggCostScale,0.0f,1.0f))?"SIM":"nao",
            c->sexuallyMature()?"SIM":"nao",
            100.0f*c->energy/std::max(0.01f,c->maxEnergy),
            100.0f*c->health/std::max(0.01f,cfg::tuning.healthMax)));
        {
            auto memName=[](MemoryEventType t){switch(t){case MemoryEventType::Food:return "comida";case MemoryEventType::Attacked:return "atacado";case MemoryEventType::AttackHit:return "ataque";case MemoryEventType::EggLaid:return "ovo";case MemoryEventType::LegacyRemoved:return "legado";default:return "-";}};
            std::string mem=std::to_string((int)c->longMemoryCount)+"/"+std::to_string(cfg::LONG_MEMORY_SLOTS)+" | ";
            for(int i=0;i<(int)c->longMemoryCount;++i){
                const int slot=(int(c->longMemoryHead)-1-i+cfg::LONG_MEMORY_SLOTS)%cfg::LONG_MEMORY_SLOTS;
                const auto&e=c->longMemory[(std::size_t)slot];
                if(i) mem+=" > ";
                mem+=memName(e.type)+std::string(" ")+Simulation::f2(std::max(0.0f,sim.simTime-e.time))+"s";
            }
            if(c->longMemoryCount==0)mem+="vazia";
            y=drawLabelValue(x,y,"Memoria longa",mem);
        }
        {
            std::string held="nenhum";
            if(c->grabbedKind==GrabKind::Plant && c->grabbedPlant) held="planta (LEGADO - controle invalido)";
            else if(c->grabbedKind==GrabKind::Carcass && c->grabbedCarcass) held="carne (massa "+Simulation::f2(c->grabbedCarcass->mass())+")";
            else if(c->grabbedKind==GrabKind::Creature && c->grabbedCreature) held="bicho #"+std::to_string(c->grabbedCreature->id)+" (massa "+Simulation::f2(c->grabbedCreature->mass())+")";
            y=drawLabelValue(x,y,"Agarrando",held);
            y=drawLabelValue(x,y,"Carga / propria massa",Simulation::f2(c->grabLoadRatio)+"x | limite "+Simulation::f2(cfg::tuning.grabMaxLoadMassRatio)+"x");
            y=drawLabelValue(x,y,"Output agarrar",Simulation::f2(c->brainOut[cfg::GRAB_OUTPUT_INDEX])+" | limiar "+Simulation::f2(cfg::tuning.grabOutputThreshold));
        }
        y=drawLabelValue(x,y,"Dureza",Simulation::f2(c->genome.physical.hardness));
        y=drawLabelValue(x,y,"Sob planta enraizada",c->rootedPlantCover>0.5f?"SIM":"nao");
        y=drawLabelValue(x,y,"Cor corpo atual RGB",std::to_string((int)c->displayColor.r)+" / "+std::to_string((int)c->displayColor.g)+" / "+std::to_string((int)c->displayColor.b));
        y=drawLabelValue(x,y,"Cor corpo base DNA",std::to_string((int)c->genome.bodyColor.r)+" / "+std::to_string((int)c->genome.bodyColor.g)+" / "+std::to_string((int)c->genome.bodyColor.b));
        {
            std::string channels; const auto mask=c->genome.bodyColorControl.channelMask;
            if(mask&1) channels+="R ";
            if(mask&2) channels+="G ";
            if(mask&4) channels+="B ";
            if(channels.empty()) channels="nenhum (cor fixa)";
            y=drawLabelValue(x,y,"Canais corpo controlaveis",channels+" | vel "+Simulation::f2(c->genome.bodyColorControl.changeSpeed)+"x/s");
            y=drawLabelValue(x,y,"Saidas cor cerebro",Simulation::f2(c->netOut[cfg::COLOR_OUTPUT_BASE])+" / "+Simulation::f2(c->netOut[cfg::COLOR_OUTPUT_BASE+1])+" / "+Simulation::f2(c->netOut[cfg::COLOR_OUTPUT_BASE+2]));
            y=drawLabelValue(x,y,"Faixas RGB DNA",
                "R "+Simulation::f2(c->genome.bodyColorControl.rMin)+"-"+Simulation::f2(c->genome.bodyColorControl.rMax)+
                " | G "+Simulation::f2(c->genome.bodyColorControl.gMin)+"-"+Simulation::f2(c->genome.bodyColorControl.gMax)+
                " | B "+Simulation::f2(c->genome.bodyColorControl.bMin)+"-"+Simulation::f2(c->genome.bodyColorControl.bMax));
        }
        y=drawLabelValue(x,y,"Cor boca RGB",std::to_string((int)c->genome.mouth.color.r)+" / "+std::to_string((int)c->genome.mouth.color.g)+" / "+std::to_string((int)c->genome.mouth.color.b)+" (afinidade alimento)");
        y=drawLabelValue(x,y,"Boca: forca / abertura / vel",Simulation::f2(c->genome.mouth.biteForce)+" / "+Simulation::f2(c->genome.mouth.maxOpening)+" / "+Simulation::f2(c->genome.mouth.movementSpeed));
        y=drawLabelValue(x,y,"Visao",std::to_string(c->genome.vision.rayCount)+" raios / "+
            Simulation::f2(c->genome.vision.range)+" alcance / "+
            Simulation::f2(visionFov(c->genome.vision.range))+" graus");
        y=drawLabelValue(x,y,"Visao distribuicao","foco "+Simulation::f2(c->genome.vision.focusExponent)+" | assimetria "+Simulation::f2(c->genome.vision.asymmetry));
        y=drawLabelValue(x,y,"Visao sens RGB",Simulation::f2(c->genome.vision.sensitivityR)+" / "+Simulation::f2(c->genome.vision.sensitivityG)+" / "+Simulation::f2(c->genome.vision.sensitivityB)+" | efic "+Simulation::f2(c->genome.vision.efficiency));
        y=drawLabelValue(x,y,"Custo visao",Simulation::f2(c->visionEnergyCostPerSecond())+" energia/s");
        {
            const float er = c->maxEnergy > 0.0f ? clampf(c->energy/c->maxEnergy,0.0f,1.0f) : 0.0f;
            float rr = 0.0f;
            if(er > cfg::tuning.healthRegenThreshold){
                const float t=clampf((er-cfg::tuning.healthRegenThreshold)/(1.0f-cfg::tuning.healthRegenThreshold),0.0f,1.0f);
                rr=cfg::tuning.healthRegenMaxPerSecond*cfg::tuning.healthRegen*t*t;
            }
            y=drawLabelValue(x,y,"Regeneracao sangue",Simulation::f2(rr)+" vida/s (limiar configuravel)");
        }
        y=drawLabelValue(
            x,y,"Ovo: energia / sangue",
            Simulation::f2(c->genome.reproduction.eggEnergyFraction*100.0f)+"% / "+
            Simulation::f2(c->genome.reproduction.eggBloodFraction*100.0f)+"%");
        y=drawLabelValue(x,y,"Escolha de parceiro","alcance "+Simulation::f2(c->genome.reproduction.mateRange)+" | visual "+Simulation::f2(c->genome.reproduction.visualPreference)+" | forca "+Simulation::f2(c->genome.reproduction.mateChoiceStrength));
        y=drawLabelValue(
            x,y,"Cerebro",
            std::to_string(brainActiveInputCount(c->genome.brain))+" inputs / "+
            std::to_string(brainActiveOutputCount(c->genome.brain))+" outputs / "+
            std::to_string(c->genome.brain.hiddenCount)+" ocultos / "+
            std::to_string(c->compiled.usedLayers)+" camadas / "+
            std::to_string(c->compiled.activeConnections)+" lig. / "+
            std::to_string(c->compiled.recurrentConnections)+" loops / mem "+
            Simulation::f2(averageBrainMemory(*c)*100.0f)+"%");
        y=drawLabelValue(x,y,"Emocoes DNA",std::to_string(c->genome.emotions.emotions.size())+" estados | Cerebro > Emocoes");
    } else if(sim.watch.hasFinal){
        auto& f=sim.watch.final;
        y=drawLabelValue(x,y,"Estado","MORTO"); y=drawLabelValue(x,y,"Energia final",Simulation::f2(f.energy)+" / "+Simulation::f2(f.maxEnergy)); y=drawLabelValue(x,y,"Vida final",Simulation::f2(f.health)); y=drawLabelValue(x,y,"Tamanho",Simulation::f2(f.size));
    }
    const float visibleTop = panel.y + 112.0f;
    const float visibleBottom = panel.y + panel.height - 8.0f;

    // Rede neural primeiro.
    if(c){
        y+=6;
        if (y + 18.0f >= visibleTop && y <= visibleBottom)
            DrawText("Rede neural",(int)x,(int)y,16,Color{220,220,220,255});
        y+=24;
        RectF graph{x,y,panel.width-24,230};
        if (graph.y + graph.height >= visibleTop && graph.y <= visibleBottom)
            drawBrainGraph(*c,graph);
        y+=246;
    }

    // Historico abaixo da rede. O evento mais novo fica no topo,
    // e os eventos antigos descem conforme a lista cresce.
    if (y + 18.0f >= visibleTop && y <= visibleBottom)
        DrawText("Historico",(int)x,(int)y,16,Color{220,220,220,255});
    y+=24;

    const int firstKept = std::max(0,(int)sim.watch.entries.size()-220);
    bool historyPastBottom = false;
    for(int i=(int)sim.watch.entries.size()-1; i>=firstKept && !historyPastBottom; --i){
        const auto& e=sim.watch.entries[i];
        std::string line=timeStamp(e.elapsed)+"  "+e.text;
        const int maxChars=54;
        for(size_t p=0;p<line.size();p+=maxChars){
            if (y > visibleBottom) { historyPastBottom = true; break; }
            if (y + 14.0f >= visibleTop) {
                std::string part=line.substr(p,maxChars);
                DrawText(part.c_str(),(int)x,(int)y,11,Color{215,215,215,255});
            }
            y+=14;
        }
        y+=3;
    }
    EndScissorMode();
}


bool numericField(UIState& ui, int fieldId, RectF r, int& value, bool enabled = true) {
    Vector2 m = GetMousePosition();
    const bool hover = enabled && pointIn(r,m);
    const bool active = ui.activePopulationField == fieldId;

    Color bg = !enabled
        ? Color{26,26,26,210}
        : active
            ? Color{34,42,58,245}
            : hover
                ? Color{38,38,38,240}
                : Color{28,28,28,235};

    DrawRectangleRounded(r,0.16f,5,bg);
    DrawRectangleRoundedLines(
        r,0.16f,5,1.0f,
        active
            ? Color{190,205,235,240}
            : Color{95,95,95,190}
    );

    if (enabled && hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        ui.activePopulationField = fieldId;
        ui.populationEdit = std::to_string(value);
    }

    if (active && enabled) {
        for (int key='0'; key<='9'; ++key) {
            if (IsKeyPressed(key)) {
                if (ui.populationEdit == "0") ui.populationEdit.clear();
                if (ui.populationEdit.size() < 7) {
                    ui.populationEdit.push_back(static_cast<char>(key));
                }
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !ui.populationEdit.empty()) {
            ui.populationEdit.pop_back();
        }

        if (!ui.populationEdit.empty()) {
            try {
                long long parsed = std::stoll(ui.populationEdit);
                parsed = std::max<long long>(0, std::min<long long>(1000000, parsed));
                value = static_cast<int>(parsed);
            } catch (...) {}
        } else {
            value = 0;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            ui.activePopulationField = -1;
            ui.populationEdit.clear();
        }
    }

    const std::string display =
        active ? ui.populationEdit : std::to_string(value);

    int tw = MeasureText(display.c_str(),14);
    DrawText(
        display.c_str(),
        (int)(r.x + r.width - tw - 10),
        (int)(r.y + 7),
        14,
        enabled ? RAYWHITE : Color{125,125,125,255}
    );

    if (active && enabled) {
        const int caretX = (int)(r.x + r.width - 8);
        DrawLineEx(
            {(float)caretX,r.y+6},
            {(float)caretX,r.y+r.height-6},
            1.0f,
            Color{230,230,230,230}
        );
    }

    return hover || active;
}

RectF populationPanelRect(int sh,bool minimized) {
    return minimized
        ? RectF{10.0f,(float)sh-48.0f,300.0f,36.0f}
        : RectF{10.0f,(float)sh-190.0f,300.0f,178.0f};
}

bool drawPopulationUI(Simulation& sim, UIState& ui, int sh) {
    RectF p=populationPanelRect(sh,ui.populationMinimized);
    DrawRectangleRounded(p,0.06f,5,Color{12,12,12,238});
    DrawRectangleRoundedLines(p,0.06f,5,1.0f,Color{72,72,72,205});

    if(ui.populationMinimized){
        DrawText("Populacao",20,(int)p.y+10,14,RAYWHITE);
        DrawText(TextFormat("B %d | P %d",(int)sim.creatures.size(),(int)sim.plants.size()),96,(int)p.y+11,10,Color{165,180,195,255});
        RectF openB{p.x+p.width-78.0f,p.y+5.0f,68.0f,26.0f};
        if(button(openB,"Abrir")) ui.populationMinimized=false;
        return pointIn(p,GetMousePosition());
    }

    DrawText("Populacao",20,(int)p.y+11,16,RAYWHITE);
    RectF minB{148,p.y+8,28,28};
    if(button(minB,"-")){
        ui.populationMinimized=true;
        ui.activePopulationField=-1;
        ui.populationEdit.clear();
        return true;
    }

    RectF toggle{184,p.y+8,108,28};
    if(button(toggle,sim.limits.maxEnabled?"Maximos: ON":"Maximos: OFF",sim.limits.maxEnabled)) {
        sim.limits.maxEnabled=!sim.limits.maxEnabled;
    }

    struct Row { const char* label; int* value; bool maximum; };
    Row rows[] = {
        {"Min. bichos",&sim.limits.minCreatures,false},
        {"Max. bichos",&sim.limits.maxCreatures,true},
        {"Min. plantas",&sim.limits.minPlants,false},
        {"Max. plantas",&sim.limits.maxPlants,true}
    };

    float y=p.y+46;
    bool anyField=false;
    for(int i=0;i<4;++i) {
        DrawText(rows[i].label,20,(int)y+7,13,Color{205,205,205,255});
        RectF field{174,y,118,28};
        const bool enabled = !rows[i].maximum || sim.limits.maxEnabled;
        anyField = numericField(ui,i,field,*rows[i].value,enabled) || anyField;
        y+=32;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && pointIn(p,GetMousePosition()) && !anyField) {
        ui.activePopulationField=-1;
        ui.populationEdit.clear();
    }

    sim.limits.maxCreatures=std::max(sim.limits.maxCreatures,sim.limits.minCreatures);
    sim.limits.maxPlants=std::max(sim.limits.maxPlants,sim.limits.minPlants);
    sim.ensureMinimums();
    return pointIn(p,GetMousePosition());
}

RectF notesCollapsedRect(int sw,float uiTop) {
    const float w=190.0f;
    return {(float)sw*0.5f-w*0.5f,uiTop,w,36.0f};
}

RectF notesPanelRect(int sw,int sh,float uiTop) {
    const float w=std::min(460.0f,std::max(330.0f,(float)sw-32.0f));
    const float h=std::min(455.0f,std::max(260.0f,(float)sh-150.0f));
    return {(float)sw*0.5f-w*0.5f,uiTop+42.0f,w,h};
}

std::string notePreview(const std::string& text,int maxChars){
    std::string one=text;
    for(char& ch:one) if(ch=='\n'||ch=='\r'||ch=='\t') ch=' ';
    if((int)one.size()<=maxChars) return one;
    if(maxChars<=3) return one.substr(0,(std::size_t)std::max(0,maxChars));
    return one.substr(0,(std::size_t)maxChars-3)+"...";
}

bool drawNotesPanel(Simulation& sim,UIState& ui,int sw,int sh,float uiTop){
    Vector2 mouse=GetMousePosition();
    int pending=0,done=0;
    for(const auto& n:sim.notes) n.done?++done:++pending;

    if(!ui.notesOpen){
        RectF tab=notesCollapsedRect(sw,uiTop);
        DrawRectangleRounded(tab,0.18f,6,Color{18,20,25,244});
        DrawRectangleRoundedLines(tab,0.18f,6,1.0f,Color{95,110,145,225});
        const std::string label="Anotacoes  "+std::to_string(pending)+" pendentes";
        DrawText(label.c_str(),(int)tab.x+12,(int)tab.y+10,12,RAYWHITE);
        if(pointIn(tab,mouse)&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            ui.notesOpen=true;
            ui.notesScroll=0.0f;
        }
        return pointIn(tab,mouse);
    }

    RectF panel=notesPanelRect(sw,sh,uiTop);
    DrawRectangleRounded(panel,0.035f,6,Color{12,12,12,248});
    DrawRectangleRoundedLines(panel,0.035f,6,1.0f,Color{88,96,118,225});
    DrawText("Anotacoes / checklist",(int)panel.x+14,(int)panel.y+13,17,RAYWHITE);
    DrawText(TextFormat("%d pendentes | %d concluidas",pending,done),(int)panel.x+14,(int)panel.y+36,10,Color{160,175,205,255});

    RectF minB{panel.x+panel.width-42.0f,panel.y+9.0f,30.0f,28.0f};
    if(button(minB,"-")){
        ui.notesOpen=false;
        ui.noteInputActive=false;
        ui.noteEditTargetId=-1;
        ui.noteEdit.clear();
        return true;
    }

    const float inputY=panel.y+62.0f;
    RectF input{panel.x+12.0f,inputY,panel.width-126.0f,34.0f};
    RectF commit{input.x+input.width+6.0f,inputY,96.0f,34.0f};
    const bool inputHover=pointIn(input,mouse);
    if(inputHover&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) ui.noteInputActive=true;

    Color inputBg=ui.noteInputActive?Color{32,39,54,245}:(inputHover?Color{35,35,35,242}:Color{27,27,27,238});
    DrawRectangleRounded(input,0.12f,5,inputBg);
    DrawRectangleRoundedLines(input,0.12f,5,1.0f,ui.noteInputActive?Color{170,195,235,240}:Color{82,82,82,205});

    if(ui.noteInputActive){
        const std::string& chars=GetTextInputChars();
        for(char ch:chars){
            if(ui.noteEdit.size()<500) ui.noteEdit.push_back(ch);
        }
        if(IsKeyPressed(KEY_BACKSPACE)&&!ui.noteEdit.empty()) ui.noteEdit.pop_back();
    }

    auto commitEdit=[&](){
        // Remove espacos simples das pontas para evitar tarefas vazias acidentais.
        auto a=ui.noteEdit.find_first_not_of(' ');
        auto b=ui.noteEdit.find_last_not_of(' ');
        std::string text=(a==std::string::npos)?std::string():ui.noteEdit.substr(a,b-a+1);
        if(text.empty()) return;
        if(ui.noteEditTargetId>=0){
            for(auto& n:sim.notes) if((int)n.id==ui.noteEditTargetId){n.text=text;break;}
        }else{
            NoteTask n;n.id=sim.nextNoteId++;n.text=text;n.done=false;sim.notes.push_back(std::move(n));
            ui.notesScroll=0.0f;
        }
        ui.noteEdit.clear();ui.noteEditTargetId=-1;ui.noteInputActive=true;
    };

    const std::string placeholder=ui.noteEditTargetId>=0?"Edite a tarefa...":"Digite uma nova tarefa...";
    const std::string shown=ui.noteEdit.empty()?placeholder:notePreview(ui.noteEdit,50);
    DrawText(shown.c_str(),(int)input.x+10,(int)input.y+9,12,ui.noteEdit.empty()?Color{125,125,135,255}:RAYWHITE);
    if(ui.noteInputActive){
        int tw=MeasureText(notePreview(ui.noteEdit,50).c_str(),12);
        float cx=std::min(input.x+input.width-8.0f,input.x+10.0f+(float)tw+2.0f);
        DrawLineEx({cx,input.y+7.0f},{cx,input.y+input.height-7.0f},1.0f,Color{230,230,230,230});
    }

    if(button(commit,ui.noteEditTargetId>=0?"Salvar":"Adicionar")) commitEdit();
    if(ui.noteInputActive&&IsKeyPressed(KEY_ENTER)) commitEdit();

    if(ui.noteEditTargetId>=0){
        RectF cancel{commit.x,commit.y+38.0f,96.0f,24.0f};
        if(button(cancel,"Cancelar")){
            ui.noteEditTargetId=-1;ui.noteEdit.clear();ui.noteInputActive=false;
        }
    }

    const float listTop=panel.y+110.0f;
    const float listBottom=panel.y+panel.height-12.0f;
    RectF listRect{panel.x+8.0f,listTop,panel.width-16.0f,listBottom-listTop};
    if(pointIn(listRect,mouse)){
        const float wheel=GetMouseWheelMove();
        if(wheel!=0.0f) ui.notesScroll=std::max(0.0f,ui.notesScroll-wheel*44.0f);
    }

    constexpr float rowH=48.0f;
    const float contentH=(float)sim.notes.size()*rowH;
    const float maxScroll=std::max(0.0f,contentH-listRect.height);
    ui.notesScroll=clampf(ui.notesScroll,0.0f,maxScroll);

    BeginScissorMode((int)listRect.x,(int)listRect.y,(int)listRect.width,(int)listRect.height);
    int eraseIndex=-1;
    for(int i=0;i<(int)sim.notes.size();++i){
        NoteTask& n=sim.notes[(std::size_t)i];
        float y=listRect.y+(float)i*rowH-ui.notesScroll;
        if(y+rowH<listRect.y||y>listRect.y+listRect.height) continue;
        RectF row{listRect.x+4.0f,y+3.0f,listRect.width-8.0f,rowH-6.0f};
        DrawRectangleRounded(row,0.08f,4,n.done?Color{24,29,27,230}:Color{24,24,27,230});
        DrawRectangleRoundedLines(row,0.08f,4,1.0f,n.done?Color{66,110,82,190}:Color{64,64,70,180});

        RectF check{row.x+8.0f,row.y+9.0f,24.0f,24.0f};
        DrawRectangleRounded(check,0.18f,4,n.done?Color{55,125,82,245}:Color{35,35,38,245});
        DrawRectangleRoundedLines(check,0.18f,4,1.0f,n.done?Color{125,220,155,230}:Color{105,105,112,210});
        if(n.done) DrawText("OK",(int)check.x+4,(int)check.y+6,9,RAYWHITE);
        if(pointIn(check,mouse)&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) n.done=!n.done;

        RectF editB{row.x+row.width-72.0f,row.y+8.0f,28.0f,26.0f};
        RectF delB{row.x+row.width-38.0f,row.y+8.0f,28.0f,26.0f};
        if(button(editB,"E")){
            ui.noteEdit=n.text;ui.noteEditTargetId=(int)n.id;ui.noteInputActive=true;
        }
        if(button(delB,"X")) eraseIndex=i;

        const int chars=std::max(16,(int)((row.width-126.0f)/7.0f));
        std::string text=notePreview(n.text,chars);
        DrawText(text.c_str(),(int)row.x+40,(int)row.y+8,12,n.done?Color{145,155,148,255}:Color{220,220,225,255});
        DrawText(n.done?"Concluida":"Pendente",(int)row.x+40,(int)row.y+25,9,n.done?Color{105,180,130,255}:Color{190,165,95,255});
        if(n.done){
            const float lineEnd=std::min(editB.x-8.0f,row.x+40.0f+(float)MeasureText(text.c_str(),12));
            DrawLineEx({row.x+40.0f,row.y+15.0f},{lineEnd,row.y+15.0f},1.0f,Color{130,145,135,180});
        }
    }
    EndScissorMode();

    if(eraseIndex>=0&&eraseIndex<(int)sim.notes.size()){
        const int erasedId=(int)sim.notes[(std::size_t)eraseIndex].id;
        sim.notes.erase(sim.notes.begin()+eraseIndex);
        if(ui.noteEditTargetId==erasedId){ui.noteEditTargetId=-1;ui.noteEdit.clear();ui.noteInputActive=false;}
    }

    // Clique no fundo do painel tira o foco do campo sem fechar o painel.
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)&&pointIn(panel,mouse)&&!inputHover&&!pointIn(commit,mouse)&&!pointIn(minB,mouse)){
        // Mantem foco quando o usuario acabou de clicar em Editar; esse caso define active=true acima.
        if(ui.noteEditTargetId<0 && !pointIn(listRect,mouse)) ui.noteInputActive=false;
    }
    return pointIn(panel,mouse);
}

void drawSpeedUI(float& speed, bool& paused, int sw, int sh) {
    constexpr float buttonW = 50.0f;
    constexpr float gap = 6.0f;
    constexpr float pauseW = 48.0f;
    constexpr int count = 6;
    const float vals[count] = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
    const char* labels[count] = {"0.1x","0.5x","1x","2x","5x","10x"};

    const float w = pauseW + gap + count * buttonW + (count - 1) * gap;
    const float x = sw * 0.5f - w * 0.5f;
    const float y = sh - 52.0f;

    if(button({x,y,pauseW,38},paused?">":"||",paused)) paused=!paused;

    float bx = x + pauseW + gap;
    for(int i=0;i<count;++i){
        RectF r{bx,y,buttonW,38};
        if(button(r,labels[i],std::fabs(speed-vals[i])<0.0001f)) speed=vals[i];
        bx += buttonW + gap;
    }
}


RectF clampPopup(RectF r, int sw, int sh) {
    r.x = clampf(r.x, 8.0f, std::max(8.0f, (float)sw - r.width - 8.0f));
    r.y = clampf(r.y, 52.0f, std::max(52.0f, (float)sh - r.height - 8.0f));
    return r;
}

bool drawCreatePopup(Simulation& sim, UIState& ui, int sw, int sh) {
    if (!ui.createPopup) return false;
    RectF p = clampPopup({ui.createPopupPos.x, ui.createPopupPos.y, 210.0f, 218.0f}, sw, sh);
    DrawRectangleRounded(p,0.07f,6,Color{12,12,12,248});
    DrawRectangleRoundedLines(p,0.07f,6,1.0f,Color{100,100,100,220});
    DrawText("Criar aqui",(int)p.x+12,(int)p.y+10,16,RAYWHITE);
    DrawText("Escolha o ente",(int)p.x+12,(int)p.y+31,11,Color{175,175,175,255});
    float y=p.y+54;
    auto action=[&](const char* label,int type){
        RectF r{p.x+10,y,p.width-20,30}; y+=34;
        if(button(r,label)){
            const Vector2 w=ui.createWorldPos;
            if(type==0) sim.addCreature(w);
            else if(type==1) sim.addPlant(w);
            else if(type==2) sim.addCarcass(w,rf(cfg::tuning.founderMinSize,cfg::tuning.founderMaxSize));
            else sim.addManualEgg(w);
            ui.createPopup=false;
        }
    };
    action("Criar bicho",0);
    action("Criar planta",1);
    action("Criar carne",2);
    action("Criar ovo",3);
    if(button({p.x+10,p.y+p.height-36,p.width-20,26},"Cancelar")) ui.createPopup=false;
    return pointIn(p,GetMousePosition());
}

bool drawKillPopup(Simulation& sim, UIState& ui, int sw, int sh) {
    if (!ui.killPopup) return false;
    RectF p=clampPopup({(float)sw-320.0f,54.0f,300.0f,264.0f},sw,sh);
    DrawRectangleRounded(p,0.06f,6,Color{12,12,12,248});
    DrawRectangleRoundedLines(p,0.06f,6,1.0f,Color{100,100,100,220});
    DrawText("Matar quantidade",(int)p.x+12,(int)p.y+10,16,RAYWHITE);
    DrawText("Tipo",(int)p.x+12,(int)p.y+42,12,Color{190,190,190,255});
    const char* names[4]={"Bichos","Plantas","Carne","Ovos"};
    for(int i=0;i<4;++i){
        RectF r{p.x+12+(i%2)*137.0f,p.y+60+(i/2)*36.0f,128.0f,30.0f};
        if(button(r,names[i],ui.killType==i)) ui.killType=i;
    }
    DrawText("Quantidade",(int)p.x+12,(int)p.y+137,12,Color{190,190,190,255});
    numericField(ui,100,{p.x+112,p.y+129,174,30},ui.killAmount,true);
    ui.killAmount=std::clamp(ui.killAmount,0,1000000);
    if(button({p.x+12,p.y+174,132,34},"Matar")){
        ui.lastKilled=sim.killEntities(ui.killType,ui.killAmount);
        ui.activePopulationField=-1;
        ui.populationEdit.clear();
    }
    if(button({p.x+154,p.y+174,132,34},"Fechar")){
        ui.killPopup=false;
        ui.activePopulationField=-1;
        ui.populationEdit.clear();
    }
    DrawText(TextFormat("Ultima acao: %d removidos",ui.lastKilled),(int)p.x+12,(int)p.y+222,11,Color{165,165,165,255});
    DrawText("Minimos configurados podem repor entidades.",(int)p.x+12,(int)p.y+240,10,Color{145,145,145,255});
    return pointIn(p,GetMousePosition());
}


RectF configPopupRect(int sw, int sh) {
    const float w = 720.0f;
    const float h = std::min(590.0f, std::max(480.0f, (float)sh - 90.0f));
    return clampPopup({(float)sw * 0.5f - w * 0.5f, 56.0f, w, h}, sw, sh);
}

struct TuningSliderDef {
    const char* label;
    float* value;
    float minValue;
    float maxValue;
    bool percent = false;
    bool integer = false;
};

enum class ConfigCategory : int {
    Energia=0, Movimento, Visao, Corpo, Cores, CicloVida, Reproducao, Plantas, OvosCarne, Fisica, Emocoes, CerebroMutacao, Reforco, Sistema, Count
};

static const char* CONFIG_CATEGORY_NAMES[] = {
    "Energia", "Movimento", "Visao", "Corpo", "Cores", "Vida", "Reproducao",
    "Plantas", "Ovos/Carne", "Fisica", "Emocoes", "Cerebro/Mutacao", "Reforco", "Sistema"
};

std::vector<TuningSliderDef> configDefsForCategory(int cat) {
    using namespace cfg;
    switch((ConfigCategory)cat) {
        case ConfigCategory::Energia: return {
            {"Energia max base",&tuning.energyMaxBase,10,500,false,false},
            {"Energia inicial",&tuning.energyInitialBase,0,500,false,false},
            {"Vida/sangue max",&tuning.healthMax,10,500,false,false},
            {"Metabolismo basal",&tuning.metabolismCost,0,3,true,false},
            {"Fome comeca em",&tuning.starvationThreshold,0,0.8,true,false},
            {"Dano de fome /s",&tuning.starvationDamage,0,50,false,false},
            {"Regeneracao comeca",&tuning.healthRegenThreshold,0,0.99,true,false},
            {"Regeneracao max /s",&tuning.healthRegenMaxPerSecond,0,20,false,false},
            {"Multiplicador regeneracao",&tuning.healthRegen,0,5,true,false},
        };
        case ConfigCategory::Movimento: return {
            {"Friccao global",&tuning.globalFriction,0,3,true,false},
            {"Arrasto passivo",&tuning.passiveDrag,0,3,true,false},
            {"Friccao das plantas",&tuning.vegetationFriction,0,4,true,false},
            {"Custo frente",&tuning.forwardCost,0,3,true,false},
            {"Custo re",&tuning.reverseCost,0,3,true,false},
            {"Custo giro",&tuning.turnCost,0,3,true,false},
            {"Custo lateral",&tuning.lateralCost,0,3,true,false},
            {"Penalidade energia multi-eixo",&tuning.multiAxisEnergyPenalty,0,4,false,false},
            {"Penalidade forca multi-eixo",&tuning.multiAxisForcePenalty,0,4,false,false},
            {"Custo velocidade",&tuning.motionCost,0,3,true,false},
            {"Custo mordida",&tuning.biteCost,0,3,true,false},
            {"Custo visao",&tuning.visionCost,0,3,true,false},
        };
        case ConfigCategory::Visao: return {
            {"Min raios efetivo",&tuning.minRaysEffective,1,(float)MAX_RAYS,false,true},
            {"Max raios efetivo",&tuning.maxRaysEffective,1,(float)MAX_RAYS,false,true},
            {"Fundador min raios",&tuning.founderMinRays,1,(float)MAX_RAYS,false,true},
            {"Fundador max raios",&tuning.founderMaxRays,1,(float)MAX_RAYS,false,true},
            {"Alcance minimo",&tuning.visionMinRange,10,5000,false,false},
            {"Alcance maximo",&tuning.visionMaxRange,50,10000,false,false},
            {"FOV no alcance curto",&tuning.visionMaxFov,10,360,false,false},
            {"FOV no alcance longo",&tuning.visionMinFov,1,180,false,false},
            {"Fundador alcance min",&tuning.founderVisionMinRange,10,5000,false,false},
            {"Fundador alcance max",&tuning.founderVisionMaxRange,50,10000,false,false},
            {"Curva alcance -> FOV",&tuning.visionFovCurveExponent,0.05,4,false,false},
            {"Custo base por raio /s",&tuning.visionRayEnergyCost,0,1,false,false},
            {"Fund foco raios min",&tuning.founderVisionFocusMin,0.1,4,false,false},
            {"Fund foco raios max",&tuning.founderVisionFocusMax,0.1,4,false,false},
            {"Gene foco raios min",&tuning.visionFocusMin,0.05,5,false,false},
            {"Gene foco raios max",&tuning.visionFocusMax,0.1,8,false,false},
            {"Assimetria visual max",&tuning.visionAsymmetryMax,0,1,true,false},
            {"Sensibilidade RGB min",&tuning.visionColorSensitivityMin,0,2,false,false},
            {"Sensibilidade RGB max",&tuning.visionColorSensitivityMax,0.1,4,false,false},
            {"Eficiencia visual min",&tuning.visionEfficiencyMin,0.05,3,false,false},
            {"Eficiencia visual max",&tuning.visionEfficiencyMax,0.1,4,false,false},
            {"Mutar tracos visao",&tuning.visionTraitMutationChance,0,1,true,false},
            {"Forca mut tracos visao",&tuning.visionTraitMutationStrength,0,1,true,false},
        };
        case ConfigCategory::Corpo: return {
            {"Tamanho referencia",&tuning.baseCreatureSize,1,30,false,false},
            {"Tamanho genetico min",&tuning.minCreatureSize,0.25,30,false,false},
            {"Tamanho genetico max",&tuning.maxCreatureSize,1,50,false,false},
            {"Fundador tamanho min",&tuning.founderMinSize,0.25,30,false,false},
            {"Fundador tamanho max",&tuning.founderMaxSize,0.25,30,false,false},
            {"Fundador vel frente",&tuning.founderForwardSpeed,1,100,false,false},
            {"Fundador vel re",&tuning.founderReverseSpeed,1,80,false,false},
            {"Fundador lateral/frente",&tuning.founderLateralSpeedRatio,0.10f,0.99f,true,false},
            {"Fundador aceleracao",&tuning.founderAcceleration,1,500,false,false},
            {"Fundador friccao",&tuning.founderFriction,0,100,false,false},
            {"Fundador rotacao",&tuning.founderRotation,1,500,false,false},
            {"Variacao fundador min",&tuning.founderVariationMin,0.1,1.5,false,false},
            {"Variacao fundador max",&tuning.founderVariationMax,0.5,2.5,false,false},
            {"Gene frente min",&tuning.geneForwardMin,0,100,false,false},
            {"Gene frente max",&tuning.geneForwardMax,1,200,false,false},
            {"Gene re min",&tuning.geneReverseMin,0,100,false,false},
            {"Gene re max",&tuning.geneReverseMax,1,150,false,false},
            {"Gene lateral ratio min",&tuning.geneLateralRatioMin,0.05f,0.99f,true,false},
            {"Gene lateral ratio max",&tuning.geneLateralRatioMax,0.05f,0.999f,true,false},
            {"Gene aceleracao min",&tuning.geneAccelerationMin,0,500,false,false},
            {"Gene aceleracao max",&tuning.geneAccelerationMax,1,1000,false,false},
            {"Gene friccao min",&tuning.geneFrictionMin,0,100,false,false},
            {"Gene friccao max",&tuning.geneFrictionMax,1,200,false,false},
            {"Gene rotacao min",&tuning.geneRotationMin,0,500,false,false},
            {"Gene rotacao max",&tuning.geneRotationMax,1,1000,false,false},
            {"Dureza min",&tuning.geneHardnessMin,0,1,false,false},
            {"Dureza max",&tuning.geneHardnessMax,0,1,false,false},
            {"Agarrar limiar output",&tuning.grabOutputThreshold,0,1,true,false},
            {"Agarrar alcance x corpo",&tuning.grabReachBodyScale,0,3,false,false},
            {"Agarrar alcance extra",&tuning.grabReachExtra,0,20,false,false},
            {"Carga max / massa corpo",&tuning.grabMaxLoadMassRatio,0.05,5,false,false},
            {"Custo segurar carga",&tuning.grabHoldEnergyCost,0,3,false,false},
            {"Custo mover carga",&tuning.grabMoveEnergyCost,0,5,false,false},
            {"Carga penaliza velocidade",&tuning.grabSpeedPenalty,0,4,false,false},
            {"Carga penaliza aceleracao",&tuning.grabAccelerationPenalty,0,4,false,false},
            {"Espaco carga a frente",&tuning.grabGap,0,5,false,false},
            {"Tamanho -> energia expoente",&tuning.energySizeExponent,-4,4,false,false},
            {"Tamanho -> vel expoente",&tuning.sizeSpeedExponent,-3,3,false,false},
            {"Mult vel tamanho min",&tuning.sizeSpeedMin,0,3,false,false},
            {"Mult vel tamanho max",&tuning.sizeSpeedMax,0,4,false,false},
            {"Tamanho -> aceleracao exp",&tuning.sizeAccelExponent,-3,3,false,false},
            {"Mult aceleracao min",&tuning.sizeAccelMin,0,3,false,false},
            {"Mult aceleracao max",&tuning.sizeAccelMax,0,4,false,false},
            {"Tamanho -> giro expoente",&tuning.sizeRotationExponent,-3,3,false,false},
            {"Mult giro min",&tuning.sizeRotationMin,0,3,false,false},
            {"Mult giro max",&tuning.sizeRotationMax,0,4,false,false},
            {"Tamanho -> custo energia exp",&tuning.sizeEnergyCostExponent,-3,3,false,false},
            {"Mult custo tamanho min",&tuning.sizeEnergyCostMin,0,3,false,false},
            {"Mult custo tamanho max",&tuning.sizeEnergyCostMax,0,5,false,false},
            {"Tamanho -> mordida expoente",&tuning.sizeBiteExponent,-3,3,false,false},
            {"Mult mordida tamanho min",&tuning.sizeBiteMin,0,3,false,false},
            {"Mult mordida tamanho max",&tuning.sizeBiteMax,0,4,false,false},
            {"Escala boca tamanho min",&tuning.mouthSizeFactorMin,0,3,false,false},
            {"Escala boca tamanho max",&tuning.mouthSizeFactorMax,0,4,false,false},
            {"Dureza penaliza velocidade",&tuning.hardnessSpeedPenalty,0,2,true,false},
            {"Vel min por dureza",&tuning.hardnessSpeedMinMult,0,1,true,false},
            {"Dureza reduz dano",&tuning.hardnessDamageReduction,0,2,true,false},
            {"Dano min por dureza",&tuning.hardnessDamageMinMult,0,1,true,false},
            {"Dureza aumenta massa",&tuning.hardnessMassBonus,0,2,true,false},
            {"Expoente massa bicho",&tuning.creatureMassSizeExponent,-4,4,false,false},
            {"Massa minima bicho",&tuning.creatureMassMin,0,10,false,false},
            {"Fund boca forca min",&tuning.founderMouthBiteMin,0,3,false,false},
            {"Fund boca forca max",&tuning.founderMouthBiteMax,0,3,false,false},
            {"Fund abertura min",&tuning.founderMouthOpeningMin,0,3,false,false},
            {"Fund abertura max",&tuning.founderMouthOpeningMax,0,3,false,false},
            {"Fund boca velocidade min",&tuning.founderMouthSpeedMin,0,6,false,false},
            {"Fund boca velocidade max",&tuning.founderMouthSpeedMax,0,6,false,false},
            {"Gene mordida min",&tuning.mouthBiteGeneMin,0,3,false,false},
            {"Gene mordida max",&tuning.mouthBiteGeneMax,0,4,false,false},
            {"Gene abertura min",&tuning.mouthOpeningGeneMin,0,3,false,false},
            {"Gene abertura max",&tuning.mouthOpeningGeneMax,0,4,false,false},
            {"Gene vel boca min",&tuning.mouthSpeedGeneMin,0,6,false,false},
            {"Gene vel boca max",&tuning.mouthSpeedGeneMax,0,8,false,false},
            {"Mutacao boca chance",&tuning.mouthMutationChance,0,1,true,false},
            {"Mutacao boca forca",&tuning.mouthMutationStrength,0,1,true,false},
            {"Vel boca referencia",&tuning.mouthSpeedReference,0.1,8,false,false},
            {"Fator vel boca min",&tuning.mouthSpeedFactorMin,0,3,false,false},
            {"Fator vel boca max",&tuning.mouthSpeedFactorMax,0,4,false,false},
            {"Mordida abertura base",&tuning.biteOpeningBase,0,2,false,false},
            {"Mordida peso abertura",&tuning.biteOpeningWeight,0,3,false,false},
            {"Mordida velocidade base",&tuning.biteSpeedBase,0,2,false,false},
            {"Mordida peso velocidade",&tuning.biteSpeedWeight,0,3,false,false},
            {"Potencia mordida max",&tuning.bitePowerMax,0.1,5,false,false},
            {"Cooldown vel piso",&tuning.biteCooldownSpeedFloor,0.01,3,false,false},
            {"Cooldown mordida min",&tuning.biteCooldownMin,0.01,5,false,false},
            {"Cooldown mordida max",&tuning.biteCooldownMax,0.01,10,false,false},
            {"Alcance mordida base",&tuning.biteReachBase,0,10,false,false},
            {"Alcance por abertura",&tuning.biteReachOpeningScale,0,20,false,false},
            {"Raio mordida base",&tuning.biteRadiusBase,0,10,false,false},
            {"Raio por abertura",&tuning.biteRadiusOpeningScale,0,15,false,false},
            {"Biomassa planta/mordida",&tuning.plantBiteBiomassBase,0,100,false,false},
            {"Dano base bicho/mordida",&tuning.creatureBiteDamageBase,0,200,false,false},
            {"Custo mordida pot min",&tuning.biteCostPowerMin,0,5,false,false},
            {"Custo mordida pot max",&tuning.biteCostPowerMax,0,5,false,false},
        };
        case ConfigCategory::Cores: return {
            {"Boca: mutacao cor chance",&tuning.mouthColorMutationChance,0,1,true,false},
            {"Boca: mutacao RGB delta",&tuning.mouthColorMutationDelta,0,128,false,true},
            {"Corpo: canal fundador chance (padrao 0)",&tuning.founderBodyColorChannelChance,0,1,true,false},
            {"Corpo: cor base mut chance",&tuning.bodyColorBaseMutationChance,0,1,true,false},
            {"Corpo: cor base mut delta",&tuning.bodyColorBaseMutationDelta,0,128,false,true},
            {"Corpo: mutar canal output (raro)",&tuning.bodyColorChannelToggleChance,0,1,true,false},
            {"Corpo: faixa mut chance",&tuning.bodyColorRangeMutationChance,0,1,true,false},
            {"Corpo: faixa mut forca",&tuning.bodyColorRangeMutationStrength,0,1,true,false},
            {"Fund cor velocidade min",&tuning.founderBodyColorSpeedMin,0.01,10,false,false},
            {"Fund cor velocidade max",&tuning.founderBodyColorSpeedMax,0.01,10,false,false},
            {"Gene cor velocidade min",&tuning.bodyColorSpeedMin,0.001,10,false,false},
            {"Gene cor velocidade max",&tuning.bodyColorSpeedMax,0.01,20,false,false},
            {"Cor velocidade mut chance",&tuning.bodyColorSpeedMutationChance,0,1,true,false},
            {"Cor velocidade mut forca",&tuning.bodyColorSpeedMutationStrength,0,1,true,false},
            {"Custo / canal ativo /s",&tuning.bodyColorActiveChannelCost,0,2,false,false},
            {"Custo por mudanca RGB",&tuning.bodyColorChangeEnergyCost,0,5,false,false},
        };
        case ConfigCategory::CicloVida: return {
            {"Fund nasc tamanho min",&tuning.founderBirthSizeFractionMin,0.01,1,true,false},
            {"Fund nasc tamanho max",&tuning.founderBirthSizeFractionMax,0.01,1,true,false},
            {"Gene nasc tamanho min",&tuning.birthSizeFractionMin,0.01,1,true,false},
            {"Gene nasc tamanho max",&tuning.birthSizeFractionMax,0.01,1,true,false},
            {"Fund crescimento min s",&tuning.founderGrowthDurationMin,0.1,300,false,false},
            {"Fund crescimento max s",&tuning.founderGrowthDurationMax,0.1,300,false,false},
            {"Gene crescimento min s",&tuning.growthDurationMin,0.1,600,false,false},
            {"Gene crescimento max s",&tuning.growthDurationMax,0.1,1200,false,false},
            {"Fund curva cresc min",&tuning.founderGrowthCurveMin,0.05,6,false,false},
            {"Fund curva cresc max",&tuning.founderGrowthCurveMax,0.05,6,false,false},
            {"Gene curva cresc min",&tuning.growthCurveMin,0.01,8,false,false},
            {"Gene curva cresc max",&tuning.growthCurveMax,0.01,10,false,false},
            {"Fund adolescencia min",&tuning.founderAdolescenceStartMin,0,1,true,false},
            {"Fund adolescencia max",&tuning.founderAdolescenceStartMax,0,1,true,false},
            {"Gene adolescencia min",&tuning.adolescenceStartMin,0,1,true,false},
            {"Gene adolescencia max",&tuning.adolescenceStartMax,0,1,true,false},
            {"Fund maturidade min",&tuning.founderMaturityFractionMin,0.01,1.5,true,false},
            {"Fund maturidade max",&tuning.founderMaturityFractionMax,0.01,1.5,true,false},
            {"Gene maturidade min",&tuning.maturityFractionMin,0.01,2,true,false},
            {"Gene maturidade max",&tuning.maturityFractionMax,0.01,2,true,false},
            {"Fund longevidade min s",&tuning.founderLongevityMin,1,1200,false,false},
            {"Fund longevidade max s",&tuning.founderLongevityMax,1,1200,false,false},
            {"Gene longevidade min s",&tuning.longevityMin,1,2400,false,false},
            {"Gene longevidade max s",&tuning.longevityMax,1,5000,false,false},
            {"Fund velhice min",&tuning.founderSenescenceFractionMin,0,0.99,true,false},
            {"Fund velhice max",&tuning.founderSenescenceFractionMax,0,0.99,true,false},
            {"Gene velhice min",&tuning.senescenceFractionMin,0,0.99,true,false},
            {"Gene velhice max",&tuning.senescenceFractionMax,0,0.99,true,false},
            {"Mutacao ciclo chance",&tuning.lifecycleMutationChance,0,1,true,false},
            {"Mutacao ciclo forca",&tuning.lifecycleMutationStrength,0,1,true,false},
            {"Atualizacao ciclo s",&tuning.lifecycleUpdateInterval,0.01,1,false,false},
            {"Custo crescimento / tamanho",&tuning.growthEnergyCostPerSize,0,20,false,false},
            {"Mult custo crescimento",&tuning.growthEnergyCostMultiplier,0,5,true,false},
            {"Curva da velhice",&tuning.oldAgeCurveExponent,0.05,6,false,false},
            {"Velocidade idoso min",&tuning.oldAgeSpeedMin,0,2,true,false},
            {"Aceleracao idoso min",&tuning.oldAgeAccelMin,0,2,true,false},
            {"Giro idoso min",&tuning.oldAgeRotationMin,0,2,true,false},
            {"Mordida idoso min",&tuning.oldAgeBiteMin,0,2,true,false},
            {"Regeneracao idoso min",&tuning.oldAgeRegenMin,0,2,true,false},
            {"Metabolismo idoso max",&tuning.oldAgeMetabolismMax,0,4,true,false},
            {"Fertilidade idoso min",&tuning.oldAgeFertilityMin,0,1,true,false},
            {"Velhice penaliza energia repro",&tuning.oldAgeReproThresholdPenalty,0,0.8,true,false},
            {"Cooldown repro idoso max",&tuning.oldAgeReproCooldownMaxMult,0.1,10,false,false},
            {"Longevidade referencia s",&tuning.longevityReference,1,1200,false,false},
            {"Custo manut longevidade",&tuning.longevityMetabolismWeight,0,3,true,false},
            {"Expoente custo longevidade",&tuning.longevityMetabolismExponent,-3,3,false,false},
            {"Metab longevidade min",&tuning.longevityMetabolismMin,0,3,true,false},
            {"Metab longevidade max",&tuning.longevityMetabolismMax,0,5,true,false},
            {"Risco morte na longevidade /s",&tuning.naturalDeathHazardAtLongevity,0,2,false,false},
            {"Expoente risco morte",&tuning.naturalDeathHazardExponent,0.05,8,false,false},
            {"Risco morte max /s",&tuning.naturalDeathHazardMax,0,5,false,false},
        };
        case ConfigCategory::Reproducao: return {
            {"Energia min p/ reproduzir",&tuning.reproMinRatio,0.1,1,true,false},
            {"Cooldown reproducao",&tuning.reproCooldown,0,120,false,false},
            {"Gap nascimento",&tuning.creatureBirthGap,0,3,false,false},
            {"Raio extra nascimento",&tuning.creatureBirthExtraRadius,0,30,false,false},
            {"Tentativas posicionar ovo",&tuning.creatureReproAttempts,1,100,false,true},
            {"Gene ovo energia min",&tuning.eggEnergyGeneMin,0,1,true,false},
            {"Gene ovo energia max",&tuning.eggEnergyGeneMax,0,1,true,false},
            {"Gene ovo sangue min",&tuning.eggBloodGeneMin,0,1,true,false},
            {"Gene ovo sangue max",&tuning.eggBloodGeneMax,0,1,true,false},
            {"Referencia energia ovo",&tuning.eggEnergyFullBirthFraction,0.01,1,true,false},
            {"Referencia sangue ovo",&tuning.eggBloodFullBirthFraction,0.01,1,true,false},
            {"Reserva energia apos ovo",&tuning.reproEnergySafetyRatio,0,0.5,true,false},
            {"Reserva sangue apos ovo",&tuning.reproHealthSafety,0,50,false,false},
            {"Qualidade peso energia",&tuning.eggQualityEnergyWeight,0,2,false,false},
            {"Qualidade peso sangue",&tuning.eggQualityBloodWeight,0,2,false,false},
            {"Fund parceiro alcance min",&tuning.founderMateRangeMin,1,800,false,false},
            {"Fund parceiro alcance max",&tuning.founderMateRangeMax,1,800,false,false},
            {"Gene parceiro alcance min",&tuning.mateRangeMin,1,1000,false,false},
            {"Gene parceiro alcance max",&tuning.mateRangeMax,1,3000,false,false},
            {"Mutar preferencia parceiro",&tuning.mateTraitMutationChance,0,1,true,false},
            {"Forca mut preferencia",&tuning.mateTraitMutationStrength,0,1,true,false},
            {"Ruido escolha parceiro",&tuning.mateChoiceNoise,0,1,false,false},
            {"Peso proximidade parceiro",&tuning.mateSpatialWeight,0,3,false,false},
            {"Peso visual parceiro",&tuning.mateVisualWeight,0,3,false,false},
            {"Acasalamento obrigatorio 0/1",&tuning.matingRequired,0,1,false,true},
            {"Tempo contato acasalamento",&tuning.matingContactSeconds,0.05,30,false,false},
            {"Folga contato acasalamento",&tuning.matingContactExtraRadius,0,50,false,false},
            {"Custo parceiro acasalamento",&tuning.matingPartnerEnergyCostFraction,0,0.5,true,false},
            {"Energia min parceiro",&tuning.matingPartnerMinEnergyRatio,0,1,true,false},
            {"Sangue min parceiro",&tuning.matingPartnerMinHealthRatio,0,1,true,false},
            {"Facilidade acasalamento",&tuning.matingEaseFactor,0.25,6,false,false},
            {"Progresso cortejo proximo",&tuning.matingNearProgressRate,0,2,true,false},
            {"Ovo solo chance virar carne",&tuning.soloEggFailureChance,0,1,true,false},
            {"Ovo com parceiro chance gemeos",&tuning.partneredEggTwinChance,0,1,true,false},
            {"Ovo com parceiro custo relativo",&tuning.partneredEggCostScale,0,1,true,false},
            {"Espera solo sem heranca sexual",&tuning.soloFallbackSeconds,0,600,false,false},
            {"Espera solo linhagem sexual",&tuning.sexualHeritageSoloFallbackSeconds,0,1800,false,false},
            {"Alcance busca acasalamento x",&tuning.matingSearchRangeScale,0.25,10,false,false},
        };
        case ConfigCategory::Plantas: return {
            {"Tamanho nascimento (min 1)",&tuning.plantBirthSize,1,20,false,false},
            {"Tamanho adulto medio",&tuning.plantMaxSize,0.1,40,false,false},
            {"Escala planta pequena min",&tuning.plantSizeScaleMin,0.05,1,false,false},
            {"Escala planta grande max",&tuning.plantSizeScaleMax,1,4,false,false},
            {"Variacao tamanho fundadora",&tuning.plantFounderSizeVariation,0,0.8,false,false},
            {"Mutacao tamanho planta chance",&tuning.plantSizeMutationChance,0,1,true,false},
            {"Mutacao tamanho planta forca",&tuning.plantSizeMutationStrength,0,1,false,false},
            {"Dificuldade extremos tamanho",&tuning.plantSizeExtremeResistance,0,8,false,false},
            {"Tempo crescimento",&tuning.plantGrowth,0.1,120,false,false},
            {"Jitter crescimento",&tuning.plantGrowthJitter,0,10,false,false},
            {"Tempo reproducao",&tuning.plantRepro,0.1,120,false,false},
            {"Jitter reproducao",&tuning.plantReproJitter,0,15,false,false},
            {"Espacamento minimo",&tuning.plantMinSpacing,0,50,false,false},
            {"Fator sobreposicao",&tuning.plantOverlapFactor,0,2,false,false},
            {"Filho distancia min",&tuning.plantChildMinDist,0,100,false,false},
            {"Filho distancia max",&tuning.plantChildMaxDist,0,200,false,false},
            {"Tentativas reproducao",&tuning.plantReproAttempts,1,100,false,true},
            {"Energia planta adulta",&tuning.plantFullEnergy,0,200,false,false},
            {"Resistencia tamanho ao comer exp",&tuning.plantBiteSizeResistanceExponent,0,2,false,false},
            {"Eficiencia cor minima",&tuning.plantMinColorEfficiency,0,1,true,false},
            {"Veneno abaixo similaridade",&tuning.plantPoisonSimilarity,0,1,true,false},
            {"Dano veneno max",&tuning.plantMaxPoisonDamage,0,100,false,false},
            {"Expoente veneno",&tuning.plantPoisonExponent,0.1,6,false,false},
            {"Cobertura usa raio corpo",&tuning.plantCoverBodyRadiusWeight,0,2,false,false},
            {"Copa deixa visao sair",&tuning.rootedPlantInsideVisionPass,0,1,true,false},
            {"Copa oculta ocupantes",&tuning.rootedPlantHideOccupants,0,1,true,false},
            {"Mutacao preferencia espacial",&tuning.plantMateMutationChance,0,1,true,false},
            {"Mutacao preferencia cor",&tuning.plantColorMateMutationChance,0,1,true,false},
            {"Mutacao RGB chance",&tuning.plantRgbMutationChance,0,1,true,false},
            {"Mutacao RGB delta",&tuning.plantRgbMutationDelta,0,64,false,true},
            {"Raio busca parceiro x tamanho",&tuning.plantPartnerRadiusScale,0,50,false,false},
            {"Peso parceiro espacial",&tuning.plantMateSpatialWeight,0,2,false,false},
            {"Peso parceiro cor",&tuning.plantMateColorWeight,0,2,false,false},
            {"Ruido escolha parceiro",&tuning.plantMateNoise,0,0.5,false,false},
            {"Delta mut pref espacial",&tuning.plantMatePreferenceMutationDelta,0,1,false,false},
            {"Delta mut pref cor",&tuning.plantColorPreferenceMutationDelta,0,1,false,false},
            {"Raio planta x tamanho",&tuning.plantRadiusScale,0.05,2,false,false},
            {"Raio minimo planta",&tuning.plantRadiusMin,0.01,5,false,false},
            {"Expoente massa planta",&tuning.plantMassSizeExponent,-4,4,false,false},
            {"Massa minima planta",&tuning.plantMassMin,0,10,false,false},
            {"LEGADO planta solta duracao (sem uso)",&tuning.plantDetachedDecayMultiplier,0,100,false,false},
            {"Vegetacao sway empurrao",&tuning.plantSwayVelocityScale,0,1.0,false,false},
            {"Vegetacao sway max offset",&tuning.plantSwayMaxOffset,0,30,false,false},
        };
        case ConfigCategory::OvosCarne: return {
            {"Incubacao min",&tuning.eggIncubationMin,0.1,60,false,false},
            {"Incubacao max",&tuning.eggIncubationMax,0.1,120,false,false},
            {"Sangue ovo -> comida",&tuning.eggBloodFoodValue,0,2,false,false},
            {"Recurso minimo ovo",&tuning.eggMinResource,0,1,true,false},
            {"Carne afinidade minima",&tuning.carcassMinColorEfficiency,0,1,true,false},
            {"Carne tamanho referencia",&tuning.carcassRefSize,0.1,30,false,false},
            {"Carne energia base",&tuning.carcassBaseEnergy,0,300,false,false},
            {"Carne duracao",&tuning.carcassBaseDuration,1,600,false,false},
            {"Carne mordida max",&tuning.carcassBaseBite,0.1,100,false,false},
            {"Carne escala sensor",&tuning.carcassSensorScale,0.1,3,false,false},
            {"Carne escala visual",&tuning.carcassVisualScale,0.1,3,false,false},
            {"Ovo raio base",&tuning.eggRadiusBase,0,10,false,false},
            {"Ovo raio por tamanho",&tuning.eggRadiusSizeScale,0,2,false,false},
            {"Ovo raio minimo",&tuning.eggRadiusMin,0.1,10,false,false},
            {"Ovo mordida x raio",&tuning.eggBiteRadiusScale,0,3,false,false},
            {"Ovo mordida minima",&tuning.eggBiteMin,0,20,false,false},
            {"Ovo potencia mordida max",&tuning.eggBitePowerMax,0,5,false,false},
            {"Ovo massa tamanho coef",&tuning.eggMassSizeCoeff,0,3,false,false},
            {"Ovo massa tamanho expoente",&tuning.eggMassSizeExponent,-4,4,false,false},
            {"Ovo massa por energia",&tuning.eggMassEnergyCoeff,0,2,false,false},
            {"Ovo massa por sangue",&tuning.eggMassBloodCoeff,0,2,false,false},
            {"Ovo massa minima",&tuning.eggMassMin,0,10,false,false},
            {"Carne energia x tamanho exp",&tuning.carcassEnergySizeExponent,-4,4,false,false},
            {"Carne duracao x tamanho exp",&tuning.carcassDurationSizeExponent,-4,4,false,false},
            {"Carne mordida x tamanho exp",&tuning.carcassBiteSizeExponent,-4,4,false,false},
            {"Carne massa x tamanho exp",&tuning.carcassMassSizeExponent,-4,4,false,false},
            {"Carne massa minima",&tuning.carcassMassMin,0,10,false,false},
            {"Carne potencia mordida max",&tuning.carcassBitePowerMax,0,5,false,false},
        };
        case ConfigCategory::Fisica: return {
            {"Amortecimento empurrao",&tuning.creaturePushDamping,0,3,true,false},
            {"Amortecimento objetos",&tuning.objectDamping,0,3,true,false},
            {"Elasticidade",&tuning.restitution,0,3,true,false},
            {"Correcao penetracao",&tuning.positionCorrection,0,1.2,true,false},
            {"Vel max empurrao",&tuning.maxPushSpeed,0,3,true,false},
            {"Slop colisao",&tuning.physicsSlop,0,0.2,false,false},
            {"Iteracoes solver",&tuning.physicsSolverIterations,1,8,false,true},
            {"Densidade bicho",&tuning.creatureMassDensity,0.05,3,false,false},
            {"Densidade carne",&tuning.carcassMassDensity,0.05,3,false,false},
            {"LEGADO massa de planta (sem uso)",&tuning.plantMassDensity,0.01,2,false,false},
            {"Substep max",&tuning.maxSubstep,0.002,0.1,false,false},
            {"Escala pressao tato",&tuning.tactilePressureScale,0.001,0.5,false,false},
            {"Memoria tato segundos",&tuning.tactileMemorySeconds,0.05,10,false,false},
            {"Escala feedback impacto",&tuning.impactFeedbackScale,0.001,2,false,false},
            {"Escala feedback dano",&tuning.damageFeedbackScale,0.001,2,false,false},
            {"Escala feedback energia",&tuning.energyGainFeedbackScale,0.001,2,false,false},
        };
        case ConfigCategory::Emocoes: return {
            {"Emocoes min efetivo",&tuning.emotionMinEffective,0,(float)MAX_EMOTIONS,false,true},
            {"Emocoes max efetivo",&tuning.emotionMaxEffective,0,(float)MAX_EMOTIONS,false,true},
            {"Fundador emocoes min",&tuning.founderEmotionMin,0,(float)MAX_EMOTIONS,false,true},
            {"Fundador emocoes max",&tuning.founderEmotionMax,0,(float)MAX_EMOTIONS,false,true},
            {"Gatilhos fundador min",&tuning.founderEmotionTriggerMin,0,32,false,true},
            {"Gatilhos fundador max",&tuning.founderEmotionTriggerMax,0,64,false,true},
            {"Limiar DNA min",&tuning.emotionThresholdMin,0,0.99,true,false},
            {"Limiar DNA max",&tuning.emotionThresholdMax,0,0.99,true,false},
            {"Sensibilidade min",&tuning.emotionGainMin,0.05,5,false,false},
            {"Sensibilidade max",&tuning.emotionGainMax,0.05,8,false,false},
            {"Forca emocao min",&tuning.emotionStrengthMin,0.01,4,false,false},
            {"Forca emocao max",&tuning.emotionStrengthMax,0.01,6,false,false},
            {"Impacto cerebral min",&tuning.emotionImpactMin,0,4,false,false},
            {"Impacto cerebral max",&tuning.emotionImpactMax,0,6,false,false},
            {"Tempo subida min",&tuning.emotionRiseTimeMin,0.01,10,false,false},
            {"Tempo subida max",&tuning.emotionRiseTimeMax,0.01,20,false,false},
            {"Duracao min",&tuning.emotionDurationMin,0.05,60,false,false},
            {"Duracao max",&tuning.emotionDurationMax,0.05,120,false,false},
            {"Bias emocao min",&tuning.emotionBiasMin,-5,5,false,false},
            {"Bias emocao max",&tuning.emotionBiasMax,-5,5,false,false},
            {"Peso gatilho limite",&tuning.emotionTriggerWeightLimit,0.1,10,false,false},
            {"Fonte gatilho: input",&tuning.emotionFounderInputSourceChance,0,1,true,false},
            {"Fonte gatilho: oculto",&tuning.emotionFounderHiddenSourceChance,0,1,true,false},
            {"Fonte gatilho: saida",&tuning.emotionFounderOutputSourceChance,0,1,true,false},
            {"Adicionar emocao",&tuning.emotionAddChance,0,1,true,false},
            {"Remover emocao",&tuning.emotionRemoveChance,0,1,true,false},
            {"Mutar gene emocao",&tuning.emotionGeneMutationChance,0,1,true,false},
            {"Forca mut gene emocao",&tuning.emotionGeneMutationStrength,0,1,true,false},
            {"Mutar peso gatilho",&tuning.emotionTriggerWeightMutationChance,0,1,true,false},
            {"Forca mut peso gatilho",&tuning.emotionTriggerWeightMutationStrength,0,3,false,false},
            {"Adicionar gatilho",&tuning.emotionAddTriggerChance,0,1,true,false},
            {"Remover gatilho",&tuning.emotionRemoveTriggerChance,0,1,true,false},
            {"Max gatilhos/emocao",&tuning.emotionMaxTriggersPerEmotion,1,128,false,true},
        };
        case ConfigCategory::CerebroMutacao: return {
            {"Intervalo decisao cerebro",&tuning.brainInterval,0.02,1,false,false},
            {"Fundador recebe vies",&tuning.founderSeekBiasChance,0,1,true,false},
            {"Vies: planta",&tuning.founderSeekPlantChance,0,1,true,false},
            {"Vies: carne",&tuning.founderSeekCarcassChance,0,1,true,false},
            {"Mutar qtd raios",&tuning.visionRayMutationChance,0,1,true,false},
            {"Delta duplo de raio",&tuning.visionDoubleRayDeltaChance,0,1,true,false},
            {"Mutar alcance",&tuning.visionRangeMutationChance,0,1,true,false},
            {"Forca mutacao alcance",&tuning.visionRangeMutationStrength,0,1,true,false},
            {"Mutacao corporal chance",&tuning.bodyMutationChance,0,1,true,false},
            {"Mutacao corporal forca",&tuning.bodyMutationStrength,0,0.5,true,false},
            {"Mutacao tamanho chance",&tuning.sizeMutationChance,0,1,true,false},
            {"Mutacao tamanho forca",&tuning.sizeMutationStrength,0,0.5,true,false},
            {"Mutacao ovo chance",&tuning.eggMutationChance,0,1,true,false},
            {"Mutacao ovo forca",&tuning.eggMutationStrength,0,0.5,true,false},
            {"Peso: chance mutacao",&tuning.brainWeightMutationChance,0,1,true,false},
            {"Peso: forca mutacao",&tuning.brainWeightMutationStrength,0,2,false,false},
            {"Bias: chance mutacao",&tuning.brainBiasMutationChance,0,1,true,false},
            {"Bias: forca mutacao",&tuning.brainBiasMutationStrength,0,2,false,false},
            {"Ganho: chance mutacao",&tuning.brainGainMutationChance,0,1,true,false},
            {"Ganho: forca mutacao",&tuning.brainGainMutationStrength,0,1,true,false},
            {"Memoria: chance mutacao",&tuning.brainMemoryMutationChance,0,1,true,false},
            {"Memoria: forca mutacao",&tuning.brainMemoryMutationStrength,0,1,true,false},
            {"Bias saida: chance",&tuning.brainOutputBiasMutationChance,0,1,true,false},
            {"Adicionar neuronio",&tuning.brainAddNeuronChance,0,1,true,false},
            {"Remover neuronio",&tuning.brainRemoveNeuronChance,0,1,true,false},
            {"Mudar camada",&tuning.brainLayerMutationChance,0,1,true,false},
            {"Adicionar conexao",&tuning.brainAddConnectionChance,0,1,true,false},
            {"Remover conexao",&tuning.brainRemoveConnectionChance,0,1,true,false},
            {"Reativar conexao",&tuning.brainReactivateConnectionChance,0,1,true,false},
            {"Adicionar input gene",&tuning.brainAddInputChance,0,1,true,false},
            {"Remover input gene",&tuning.brainRemoveInputChance,0,1,true,false},
            {"Adicionar output gene",&tuning.brainAddOutputChance,0,1,true,false},
            {"Remover output gene",&tuning.brainRemoveOutputChance,0,1,true,false},
            {"Neuronio novo nasce isolado",&tuning.brainNewNeuronIsolatedChance,0,1,true,false},
            {"Conexao mesma area x",&tuning.brainSameAreaConnectionMult,0.1,5,false,false},
            {"Conexao outra area x",&tuning.brainCrossAreaConnectionMult,0.01,2,false,false},
            {"Conexao para tras x",&tuning.brainBackwardConnectionMult,0.01,2,false,false},
            {"Conexao mesma camada x",&tuning.brainSameLayerConnectionMult,0.01,2,false,false},
            {"Feedback de output x",&tuning.brainOutputFeedbackConnectionMult,0.001,1,false,false},
            {"Penalidade fanout",&tuning.brainFanoutPenalty,0,2,false,false},
            {"Mutar funcao ligacao",&tuning.brainLinkFunctionMutationChance,0,1,true,false},
            {"Mutar params ligacao",&tuning.brainLinkParameterMutationChance,0,1,true,false},
            {"Forca params ligacao",&tuning.brainLinkParameterMutationStrength,0,2,false,false},
            {"Mutar ativacao neural",&tuning.brainActivationMutationChance,0,1,true,false},
            {"Mutar area neural",&tuning.brainAreaMutationChance,0,1,true,false},
            {"Mutar tipo conexao",&tuning.brainConnectionModeMutationChance,0,1,true,false},
            {"Dividir conexao",&tuning.brainSplitConnectionChance,0,1,true,false},
            {"Duplicar neuronio",&tuning.brainDuplicateNeuronChance,0,1,true,false},
            {"Duplicar circuito",&tuning.brainDuplicateCircuitChance,0,1,true,false},
            {"Vies conexao especializada",&tuning.brainSpecializedConnectionBias,0,1,true,false},
            {"LEGADO fundador area (ignorado)",&tuning.founderSpecializedAreaChance,0,1,true,false},
            {"LEGADO fundador ativacao (ignorado)",&tuning.founderAltActivationChance,0,1,true,false},
            {"LEGADO fundador gate (ignorado)",&tuning.founderGateConnectionChance,0,1,true,false},
            {"Ancestral registradores (fixo 0)",&tuning.founderRegisterMin,0,0,false,true},
            {"Ancestral registradores max (fixo 0)",&tuning.founderRegisterMax,0,0,false,true},
            {"Mutar qtd registradores",&tuning.brainRegisterCountMutationChance,0,1,true,false},
            {"Custo energia/registrador",&tuning.brainRegisterEnergyCostPerSecond,0,0.01,false,false},
            {"LEGADO fundador plastica (ignorado)",&tuning.founderPlasticConnectionChance,0,1,true,false},
            {"Alternar plasticidade",&tuning.brainPlasticToggleMutationChance,0,1,true,false},
            {"Mutar taxa plasticidade",&tuning.brainPlasticRateMutationChance,0,1,true,false},
            {"Taxa aprendizado plastico",&tuning.brainPlasticLearningRate,0,0.5,false,false},
            {"Elegibilidade segundos",&tuning.brainPlasticEligibilitySeconds,0.1,20,false,false},
            {"Faixa peso aprendido",&tuning.brainPlasticWeightRange,0,4,false,false},
            {"Retorno peso ao DNA/s",&tuning.brainPlasticReturnPerSecond,0,0.1,false,false},
            {"LEGADO reward energia",&tuning.brainPlasticRewardGain,0,4,false,false},
            {"LEGADO reward dano",&tuning.brainPlasticDamagePenalty,0,4,false,false},
            {"Custo energia/conexao plastica",&tuning.brainPlasticConnectionEnergyCostPerSecond,0,0.005,false,false},
            {"Gate escrita memoria",&tuning.brainRegisterWriteGateThreshold,0,1,true,false},
            {"Custo energia/neuronio",&tuning.brainNeuronEnergyCostPerSecond,0,0.02,false,false},
            {"Custo energia/conexao",&tuning.brainConnectionEnergyCostPerSecond,0,0.005,false,false},
            {"Custo extra ligacao recorrente",&tuning.brainRecurrentConnectionEnergyCostPerSecond,0,0.005,false,false},
            {"Custo extra neuronio com memoria",&tuning.brainStatefulNeuronEnergyCostPerSecond,0,0.005,false,false},
            {"Fundador ocultos (ancestral fixa 0)",&tuning.founderHiddenCount,0,0,false,true},
            {"LEGADO fundador camada min",&tuning.founderLayerMin,1,(float)MAX_BRAIN_LAYERS,false,true},
            {"LEGADO fundador camada max",&tuning.founderLayerMax,1,(float)MAX_BRAIN_LAYERS,false,true},
            {"Novo neuronio ganho min",&tuning.founderGainMin,0.1,5,false,false},
            {"Novo neuronio ganho max",&tuning.founderGainMax,0.1,5,false,false},
            {"LEGADO fundador memoria chance",&tuning.founderMemoryChance,0,1,true,false},
            {"LEGADO fundador memoria min",&tuning.founderMemoryMin,0,1,true,false},
            {"LEGADO fundador memoria max",&tuning.founderMemoryMax,0,1,true,false},
            {"LEGADO Fund I->H",&tuning.founderInputHiddenChance,0,1,true,false},
            {"LEGADO Fund H->H self",&tuning.founderHiddenSelfChance,0,1,true,false},
            {"LEGADO Fund H->H",&tuning.founderHiddenHiddenChance,0,1,true,false},
            {"LEGADO Fund H->saida",&tuning.founderHiddenOutputChance,0,1,true,false},
            {"LEGADO Fund entrada->saida",&tuning.founderInputOutputChance,0,1,true,false},
            {"LEGADO Fund saida->H",&tuning.founderOutputHiddenChance,0,1,true,false},
            {"LEGADO Fund saida->saida",&tuning.founderOutputOutputChance,0,1,true,false},
            {"Limite absoluto peso",&tuning.brainWeightLimit,0.1,20,false,false},
            {"Limite absoluto bias",&tuning.brainBiasLimit,0.1,20,false,false},
            {"Ganho neural min",&tuning.brainGainMin,0,5,false,false},
            {"Ganho neural max",&tuning.brainGainMax,0,10,false,false},
            {"Memoria neural max",&tuning.brainMemoryMax,0,1,true,false},
            {"Ocultos min efetivo",&tuning.brainMinHiddenEffective,0,(float)MAX_HIDDEN,false,true},
            {"Ocultos max efetivo",&tuning.brainMaxHiddenEffective,1,(float)MAX_HIDDEN,false,true},
            {"Mudar camada passo chance",&tuning.brainLayerStepChance,0,1,true,false},
            {"Novo neuro memoria chance",&tuning.newNeuronMemoryChance,0,1,true,false},
            {"Novo neuro memoria min",&tuning.newNeuronMemoryMin,0,1,true,false},
            {"Novo neuro memoria max",&tuning.newNeuronMemoryMax,0,1,true,false},
            {"Novo neuro ligacao extra",&tuning.newNeuronExtraConnectionChance,0,1,true,false},
            {"Nova lig origem entrada",&tuning.randomConnectionSrcInputChance,0,1,true,false},
            {"Nova lig origem oculto",&tuning.randomConnectionSrcHiddenChance,0,1,true,false},
            {"Nova lig destino oculto",&tuning.randomConnectionDstHiddenChance,0,1,true,false},
            {"Tentativas nova conexao",&tuning.randomConnectionAttempts,1,200,false,true},
            {"Min conexoes ativas",&tuning.minimumActiveConnections,0,50,false,true},
        };
        case ConfigCategory::Reforco: return {
            {"RL ativo 0/1",&tuning.reinforcementEnabled,0,1,false,true},
            {"Ganhar 10% energia",&tuning.reinforcementEnergy10Pct,-5,5,false,false},
            {"Comer planta",&tuning.reinforcementPlantFood,-5,5,false,false},
            {"Comer carne",&tuning.reinforcementMeatFood,-5,5,false,false},
            {"Comer ovo",&tuning.reinforcementEggFood,-5,5,false,false},
            {"Receber 10% dano",&tuning.reinforcementDamage10Pct,-5,5,false,false},
            {"Causar 10% dano",&tuning.reinforcementAttack10Pct,-5,5,false,false},
            {"TER FILHO (recompensa maxima)",&tuning.reinforcementReproduction,-50,50,false,false},
            {"MORRER (punicao maxima)",&tuning.reinforcementDeath,-50,50,false,false},
            {"Sobreviver por segundo",&tuning.reinforcementSurvivalPerSecond,-2,2,false,false},
            {"Fome por segundo",&tuning.reinforcementStarvingPerSecond,-2,2,false,false},
            {"Limite reward por tick",&tuning.reinforcementRewardClip,0.05,50,false,false},
            {"Ser mordido evento",&tuning.reinforcementBittenEvent,-10,10,false,false},
            {"Recorde maior tempo vivo /s",&tuning.reinforcementRecordLongestLife,-10,10,false,false},
            {"Recorde menor tempo vivo",&tuning.reinforcementRecordShortestLife,-10,10,false,false},
            {"Recorde sangue acima 50 /s",&tuning.reinforcementRecordHealthAbove50,-10,10,false,false},
            {"Recorde MAIS RAPIDO sangue 50->0",&tuning.reinforcementRecordHealthBelow50,-20,20,false,false},
            {"Recorde energia acima 50 /s",&tuning.reinforcementRecordEnergyAbove50,-10,10,false,false},
            {"Recorde MAIS RAPIDO energia 50->0",&tuning.reinforcementRecordEnergyBelow50,-20,20,false,false},
            {"Recorde mais ovos /ovo",&tuning.reinforcementRecordMostEggs,-10,10,false,false},
            {"Recorde menos ovos",&tuning.reinforcementRecordFewestEggs,-10,10,false,false},
            {"Taxa aprendizado plastico",&tuning.brainPlasticLearningRate,0,0.5,false,false},
            {"Elegibilidade segundos",&tuning.brainPlasticEligibilitySeconds,0.1,20,false,false},
            {"Faixa peso aprendido",&tuning.brainPlasticWeightRange,0,4,false,false},
            {"LEGADO fundador plastica (ignorado)",&tuning.founderPlasticConnectionChance,0,1,true,false},
            {"Alternar plasticidade",&tuning.brainPlasticToggleMutationChance,0,1,true,false},
            {"Mutar taxa plasticidade",&tuning.brainPlasticRateMutationChance,0,1,true,false},
        };
        case ConfigCategory::Sistema: return {
            {"Autosave intervalo s (0=OFF)",&tuning.autosaveIntervalSeconds,0,1800,false,false},
        };
        default: return {};
    }
}

// =========================================================
// v0.079: ajuda contextual didatica + arquivo de configuracao humano
// =========================================================
struct ConfigHelpText {
    std::string summary; // o que e, em linguagem leiga
    std::string effect;  // o que muda quando aumenta/diminui
    std::string example; // exemplo concreto
};

std::string lowerAscii(std::string s) {
    for(char& ch:s) ch=(char)std::tolower((unsigned char)ch);
    return s;
}

bool textHas(const std::string& haystackLower,const char* needleLower) {
    return haystackLower.find(needleLower)!=std::string::npos;
}

std::string trimAscii(const std::string& s) {
    std::size_t a=0,b=s.size();
    while(a<b && std::isspace((unsigned char)s[a])) ++a;
    while(b>a && std::isspace((unsigned char)s[b-1])) --b;
    return s.substr(a,b-a);
}

std::string configKeyFromLabel(const char* label) {
    std::string in=label?label:"";
    std::string out;
    bool underscore=false;
    for(unsigned char uc:in){
        char c=(char)std::tolower(uc);
        if(std::isalnum(uc)){
            out.push_back(c); underscore=false;
        }else if(!out.empty() && !underscore){
            out.push_back('_'); underscore=true;
        }
    }
    while(!out.empty() && out.back()=='_') out.pop_back();
    return out.empty()?"parametro":out;
}

std::string configValueText(const TuningSliderDef& d,float value) {
    if(d.integer) return std::to_string((int)std::lround(value));
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    std::string out=ss.str();
    while(out.size()>1 && out.back()=='0') out.pop_back();
    if(!out.empty() && out.back()=='.') out.pop_back();
    if(out=="-0") out="0";
    return out;
}

std::string configDisplayValue(const TuningSliderDef& d,float value) {
    char buf[64];
    if(d.percent) std::snprintf(buf,sizeof(buf),"%.1f%%",value*100.0f);
    else if(d.integer) std::snprintf(buf,sizeof(buf),"%d",(int)std::lround(value));
    else if(std::fabs(value)<0.1f) std::snprintf(buf,sizeof(buf),"%.4f",value);
    else std::snprintf(buf,sizeof(buf),"%.3f",value);
    return buf;
}

std::string categoryPurpose(int cat) {
    switch((ConfigCategory)cat){
        case ConfigCategory::Energia:return "energia, fome e regeneracao";
        case ConfigCategory::Movimento:return "movimento e seus custos";
        case ConfigCategory::Visao:return "alcance, precisao e custo da visao";
        case ConfigCategory::Corpo:return "anatomia, massa, boca e capacidades fisicas";
        case ConfigCategory::Cores:return "cores da boca e sinalizacao corporal";
        case ConfigCategory::CicloVida:return "crescimento, maturidade, velhice e longevidade";
        case ConfigCategory::Reproducao:return "condicoes e recursos da reproducao";
        case ConfigCategory::Plantas:return "crescimento, reproducao e fisica das plantas";
        case ConfigCategory::OvosCarne:return "ovos, carcasas, alimento e decomposicao";
        case ConfigCategory::Fisica:return "colisoes, massas e resolucao fisica";
        case ConfigCategory::Emocoes:return "estados emocionais evolutiveis e seus gatilhos";
        case ConfigCategory::CerebroMutacao:return "cerebro, conectividade e mutacoes";
        case ConfigCategory::Reforco:return "funcao de recompensa do aprendizado por reforco";
        case ConfigCategory::Sistema:return "salvamento automatico e comportamento geral do programa";
        default:return "esta regra da simulacao";
    }
}

ConfigHelpText configHelpFor(int cat,const TuningSliderDef& d) {
    const std::string label=d.label?d.label:"Parametro";
    const std::string l=lowerAscii(label);
    ConfigHelpText h;

    // Explicacoes exatas para os controles que costumam gerar mais duvida.
    if(l=="rl ativo 0/1") h.summary="Liga/desliga o sinal de recompensa externo. Em zero, a plasticidade nao recebe reward do painel.";
    else if(l=="ganhar 10% energia") h.summary="Reward aplicado a cada bloco equivalente a 10% da energia maxima realmente recuperada. Pode ser positivo ou negativo.";
    else if(l=="comer planta") h.summary="Reward extra por uma mordida de planta que realmente recuperou energia. O valor e escalado pela utilidade da mordida.";
    else if(l=="comer carne") h.summary="Reward extra por uma mordida de carne que realmente recuperou energia.";
    else if(l=="comer ovo") h.summary="Reward extra por obter energia de um ovo. Use negativo se quiser ensinar a evitar canibalismo de ovos.";
    else if(l=="receber 10% dano") h.summary="Reward por perder 10% da vida maxima. Normalmente e negativo, mas voce pode inverter a regra experimentalmente.";
    else if(l=="causar 10% dano") h.summary="Reward por tirar 10% da vida de outro bicho. Padrao zero: agressao nao e premiada nem punida diretamente.";
    else if(l=="botar ovo natural") h.summary="Reward dado quando o bicho consegue colocar um ovo natural com custo real de energia/vida.";
    else if(l=="sobreviver por segundo") h.summary="Reward continuo enquanto o individuo esta vivo. Use valores pequenos para nao dominar eventos mais informativos.";
    else if(l=="fome por segundo") h.summary="Reward continuo enquanto a energia esta abaixo do limiar de fome. Padrao negativo para ensinar a evitar esse estado.";
    else if(l=="limite reward por tick") h.summary="Satura a soma de recompensas recebidas entre dois ticks cerebrais, evitando uma atualizacao sinaptica explosiva.";
    else if(l=="energia max base") h.summary="Energia maxima de referencia. O tamanho corporal pode multiplicar esse valor pelas leis de tamanho.";
    else if(l=="energia inicial") h.summary="Energia de referencia com que um bicho novo comeca. O valor final acompanha a capacidade energetica do corpo.";
    else if(l=="metabolismo basal") h.summary="Multiplica o gasto de energia que existe mesmo quando o bicho esta parado.";
    else if(l=="fome comeca em") h.summary="Fracao da energia maxima abaixo da qual a falta de energia passa a causar dano.";
    else if(l=="regeneracao comeca") h.summary="Fracao de energia acima da qual o sangue/vida pode comecar a se regenerar.";
    else if(l=="friccao global") h.summary="Multiplica a friccao genetica dos bichos. Quanto maior, mais rapidamente eles perdem velocidade.";
    else if(l=="arrasto passivo") h.summary="Resistencia adicional aplicada ao movimento, mesmo sem comando contrario do cerebro.";
    else if(l=="custo lateral") h.summary="Multiplica o gasto ativo de energia ao andar de lado para esquerda ou direita sem girar o corpo.";
    else if(l=="fundador lateral/frente") h.summary="Fracao da velocidade frontal usada como velocidade lateral maxima nos fundadores. Mantem andar de lado mais lento que ir para frente.";
    else if(l=="gene lateral ratio min") h.summary="Menor fracao genetica entre velocidade lateral e frontal permitida pela evolucao.";
    else if(l=="gene lateral ratio max") h.summary="Define ate que ponto a evolucao pode deixar um bicho andar de lado quase tao rapido quanto anda para frente.";
    else if(l=="penalidade energia multi-eixo") h.summary="Cobra energia extra quando o bicho tenta andar para frente/tras e para o lado ao mesmo tempo. Diagonal continua possivel, mas deixa de ser gratis.";
    else if(l=="penalidade forca multi-eixo") h.summary="Faz o corpo dividir sua capacidade de aceleracao quando usa o eixo frente/tras e o eixo lateral juntos.";
    else if(l=="agarrar limiar output") h.summary="E o quanto a saida neural 'Agarrar' precisa estar ativada para o bicho tentar pegar algo que esteja a sua frente.";
    else if(l=="agarrar alcance x corpo") h.summary="Define quanto o proprio tamanho do bicho aumenta a distancia em que ele consegue alcancar uma carga.";
    else if(l=="agarrar alcance extra") h.summary="Adiciona uma pequena distancia fixa ao alcance de agarrar, independentemente do tamanho do bicho.";
    else if(l=="carga max / massa corpo") h.summary="Define o peso maximo que um bicho consegue segurar comparado com a propria massa. O sistema compara massas reais, nao apenas o tamanho visual.";
    else if(l=="custo segurar carga") h.summary="Energia gasta apenas para continuar sustentando algo agarrado, mesmo que o bicho esteja quase parado.";
    else if(l=="custo mover carga") h.summary="Energia extra gasta para transportar uma carga. Quanto mais pesada a carga e mais rapido o bicho se move, maior o gasto.";
    else if(l=="carga penaliza velocidade") h.summary="Controla quanto uma carga pesada reduz a velocidade maxima do bicho enquanto ele a carrega.";
    else if(l=="carga penaliza aceleracao") h.summary="Controla quanto uma carga pesada deixa mais dificil ganhar velocidade ou mudar o movimento.";
    else if(l=="espaco carga a frente") h.summary="E o pequeno espaco deixado entre o corpo do carregador e o objeto que ele esta segurando.";
    else if(l=="tamanho adulto medio") h.summary="E o tamanho adulto de referencia das plantas. Esse e o tamanho que corresponde a uma planta 'media' (gene de escala 1.0).";
    else if(l=="escala planta pequena min") h.summary="Define quao pequena uma linhagem de planta pode evoluir em relacao a planta media.";
    else if(l=="escala planta grande max") h.summary="Define quao grande uma linhagem de planta pode evoluir em relacao a planta media.";
    else if(l=="variacao tamanho fundadora") h.summary="Da uma pequena diferenca de tamanho entre as primeiras plantas do mundo, para a evolucao nao comecar com todas exatamente iguais.";
    else if(l=="mutacao tamanho planta chance") h.summary="Chance de o gene de tamanho de uma planta descendente sofrer alguma mudanca ao nascer.";
    else if(l=="mutacao tamanho planta forca") h.summary="Define o tamanho do passo que uma mutacao pode dar para deixar a nova planta um pouco maior ou menor.";
    else if(l=="dificuldade extremos tamanho") h.summary="Faz ficar progressivamente mais dificil evoluir para tamanhos muito distantes do medio. Quanto maior, mais raras ficam plantas extremamente pequenas ou grandes."; 
    else if(l=="alcance minimo") h.summary="Menor alcance de visao que a evolucao pode produzir para um bicho.";
    else if(l=="alcance maximo") h.summary="Maior alcance de visao permitido geneticamente.";
    else if(l=="curva alcance -> fov") h.summary="Define como aumentar o alcance estreita o campo de visao. Muda a curva do trade-off alcance versus abertura.";
    else if(l=="copa deixa visao sair") h.summary="Permite que o bicho coberto por uma copa enraizada enxergue atraves daquela propria copa para fora.";
    else if(l=="copa oculta ocupantes") h.summary="Faz a copa enraizada esconder de observadores externos os bichos que estao cobertos por ela.";
    else if(l=="cobertura usa raio corpo") h.summary="Define quanto o raio do corpo conta para decidir se o bicho esta sob uma copa, em vez de usar apenas seu centro.";
    else if(l=="eficiencia cor minima") h.summary="Energia minima aproveitada de uma planta mesmo quando a cor da boca tem afinidade muito baixa.";
    else if(l=="veneno abaixo similaridade") h.summary="Afinidade de cor abaixo da qual comer planta passa a causar dano por incompatibilidade.";
    else if(l=="corpo: canal fundador chance (padrao 0)") h.summary="Chance de um fundador ja nascer capaz de controlar R, G ou B do corpo. O padrao zero faz essa habilidade surgir apenas por evolucao.";
    else if(l=="corpo: mutar canal output (raro)") h.summary="Chance por canal de um descendente ganhar ou perder controle neural sobre R, G ou B do corpo.";
    else if(l=="intervalo decisao cerebro") h.summary="Tempo simulado entre duas avaliacoes do cerebro. Sensores continuam sendo atualizados entre as decisoes.";
    else if(l=="substep max") h.summary="Maior passo de tempo permitido na fisica/sensores. Menor valor aumenta fidelidade e tambem o custo computacional.";
    else if(textHas(l,"expoente") || textHas(l," exp")) h.summary="Expoente que controla a curvatura desta relacao em "+categoryPurpose(cat)+". Valores maiores costumam reforcar diferencas; valores menores suavizam.";
    else if(textHas(l,"chance") || textHas(l,"mutar ") || textHas(l,"adicionar ") || textHas(l,"remover ") || textHas(l,"reativar ")) h.summary="Probabilidade de esta mudanca acontecer quando a regra correspondente e avaliada durante "+categoryPurpose(cat)+".";
    else if(textHas(l,"fundador") || l.rfind("fund ",0)==0) h.summary="Valor usado para criar os bichos fundadores. Depois disso, descendentes herdam e mutam seus proprios genes.";
    else if(textHas(l,"gene ") || l.rfind("gene",0)==0) h.summary="Limite ou propriedade genetica disponivel para a evolucao em "+categoryPurpose(cat)+".";
    else if(textHas(l,"ganho") || textHas(l,"peso ")) h.summary="Define quanto esta contribuicao pesa no resultado de "+categoryPurpose(cat)+". Maior valor aumenta sua influencia.";
    else if(textHas(l,"custo")) h.summary="Controla quanto esta atividade ou caracteristica contribui para o gasto de energia do bicho.";
    else if(textHas(l,"min")) h.summary="Limite minimo permitido para esta propriedade de "+categoryPurpose(cat)+".";
    else if(textHas(l,"max")) h.summary="Limite maximo permitido para esta propriedade de "+categoryPurpose(cat)+".";
    else if(textHas(l,"duracao") || textHas(l,"tempo") || textHas(l,"cooldown") || textHas(l,"intervalo")) h.summary="Controla o tempo associado a esta regra de "+categoryPurpose(cat)+".";
    else if(textHas(l,"massa") || textHas(l,"densidade")) h.summary="Controla como massa/densidade entra na fisica e nos efeitos ligados a "+categoryPurpose(cat)+".";
    else if(textHas(l,"raio") || textHas(l,"alcance") || textHas(l,"distancia")) h.summary="Controla uma distancia ou area usada por "+categoryPurpose(cat)+".";
    else if((textHas(l,"mutacao")||textHas(l," mut ")) && textHas(l,"forca")) h.summary="Aqui 'forca' nao e forca fisica: e o tamanho da alteracao genetica quando essa mutacao acontece. A chance de acontecer e configurada separadamente.";
    else if(textHas(l,"dano") || textHas(l,"mordida") || textHas(l,"forca")) h.summary="Controla a intensidade desta interacao fisica dentro de "+categoryPurpose(cat)+".";
    else if(textHas(l,"multiplicador")) h.summary="E um numero que multiplica outro efeito. Pense em 1 como normal, 0.5 como metade e 2 como o dobro, aplicado a "+categoryPurpose(cat)+".";
    else if(textHas(l,"referencia")) h.summary="E o valor usado como ponto de comparacao para os calculos de "+categoryPurpose(cat)+". Outros valores sao medidos em relacao a ele; nao significa necessariamente um limite.";
    else if(textHas(l,"escala")) h.summary="E um fator de escala: ele aumenta ou reduz proporcionalmente esta parte de "+categoryPurpose(cat)+" sem mudar sozinho as outras regras.";
    else if(textHas(l,"jitter") || textHas(l,"ruido")) h.summary="Adiciona variacao aleatoria para que os resultados nao sejam todos identicos. Zero remove essa variacao; valores maiores tornam os resultados mais espalhados.";
    else if(textHas(l,"elasticidade")) h.summary="Define quanto os corpos tendem a quicar/se afastar depois de uma colisao, em vez de simplesmente parar encostados.";
    else if(textHas(l,"amortecimento")) h.summary="Define quanto movimento e perdido com o tempo. E como um freio automatico aplicado a empurroes ou objetos soltos.";
    else if(textHas(l,"correcao penetracao")) h.summary="Quando dois corpos ficam sobrepostos por causa do passo da simulacao, este valor define com que firmeza a fisica os separa novamente.";
    else if(textHas(l,"slop colisao")) h.summary="E uma pequena tolerancia de sobreposicao nas colisoes. Evita que a fisica fique tentando corrigir contatos minusculos o tempo todo.";
    else if(textHas(l,"iteracoes solver")) h.summary="E quantas vezes a fisica revisa as colisoes no mesmo passo. Mais iteracoes resolvem pilhas/empurroes melhor, mas usam mais CPU.";
    else if(textHas(l,"fonte gatilho")) h.summary="Define de qual tipo de sinal uma emocao pode receber gatilhos: sensores de entrada, neuronios ocultos ou saidas do proprio cerebro.";
    else if(l.rfind("vies:",0)==0) h.summary="Define para qual tipo de alvo o pequeno circuito inicial dos fundadores tende a apontar. E apenas um empurrao inicial na rede, nao uma regra permanente de comportamento.";
    else if(textHas(l,"delta")) h.summary="Define o tamanho maximo aproximado de uma mudanca por mutacao. Nao muda a chance de mutar; muda o quanto o valor pode andar quando a mutacao acontece.";
    else if(textHas(l,"reserva")) h.summary="E a quantidade que deve continuar sobrando depois da acao. Funciona como uma margem de seguranca para o organismo nao gastar tudo.";
    else if(textHas(l,"tentativas")) h.summary="E quantas tentativas o sistema pode fazer para encontrar uma solucao valida, como uma posicao livre ou uma nova conexao. Mais tentativas aumentam a chance de sucesso e custam mais processamento.";
    else if(textHas(l,"curva")) h.summary="Muda o formato de como um efeito cresce ou diminui. Serve para tornar a resposta mais suave no inicio ou mais forte perto dos extremos.";
    else if(textHas(l,"atualizacao")) h.summary="Define de quanto em quanto tempo simulado esta parte e recalculada. Intervalos menores respondem mais rapido, mas exigem mais processamento.";
    else if(textHas(l,"risco")) h.summary="Controla a probabilidade/intensidade de um evento indesejado nesta parte da simulacao. Quanto maior, mais forte ou frequente tende a ser o risco.";
    else if(textHas(l,"mutacao preferencia")) h.summary="Define a chance de a preferencia herdada mudar em um descendente, permitindo que novas estrategias de escolha aparecam ao longo das geracoes.";
    else if(textHas(l,"gap")) h.summary="E um espaco de seguranca deixado entre corpos/posicoes para evitar que nascam ou sejam colocados ja sobrepostos.";
    else if(textHas(l,"sway")) h.summary="Controla o balanco visual/fisico da copa quando algo a empurra pela borda. Bichos realmente embaixo da copa agora a mantem parada.";
    else if(textHas(l,"tamanho")) h.summary="Controla um tamanho usado nesta parte de "+categoryPurpose(cat)+". Tamanho tambem pode alterar massa, contato e outros efeitos quando essas leis estao ligadas.";
    else h.summary="Em linguagem simples: \""+label+"\" e um ajuste da parte de "+categoryPurpose(cat)+". O bloco abaixo diz exatamente o que muda quando voce aumenta ou diminui o numero.";

    // Segunda camada: explica a consequencia sem exigir que o usuario saiba programacao ou matematica.
    if(l=="penalidade energia multi-eixo") h.effect="Aumentar faz diagonais gastarem mais energia; zerar deixa frente+lado sem sobretaxa alem do custo normal do movimento.";
    else if(l=="penalidade forca multi-eixo") h.effect="Aumentar deixa o bicho acelerar menos quando combina duas direcoes; zerar permite usar a aceleracao cheia nos dois eixos.";
    else if(textHas(l,"carga max / massa")) h.effect="Se aumentar, bichos conseguem levantar coisas mais pesadas. Se diminuir, so cargas leves em relacao ao proprio corpo podem ser levadas.";
    else if(textHas(l,"carga penaliza")) h.effect="Quanto maior este valor, mais uma carga pesada atrapalha o movimento. Cargas leves quase nao sentem a penalidade.";
    else if(textHas(l,"agarrar alcance")) h.effect="Aumentar permite pegar objetos um pouco mais longe; diminuir obriga o bicho a encostar mais perto antes de conseguir segurar.";
    else if(textHas(l,"custo segurar")||textHas(l,"custo mover carga")) h.effect="Aumentar torna carregar uma estrategia energeticamente cara; diminuir facilita transportar objetos por longos periodos.";
    else if(l=="tamanho adulto medio") h.effect="Aumentar torna toda a referencia vegetal maior. O gene individual continua podendo produzir plantas menores ou maiores que esse medio.";
    else if(textHas(l,"escala planta pequena")) h.effect="Valores menores abrem espaco evolutivo para plantas bem pequenas. Perto de 1, quase nenhuma planta consegue ficar muito abaixo do tamanho medio.";
    else if(textHas(l,"escala planta grande")) h.effect="Valores maiores permitem plantas gigantes. Perto de 1, o teto fica proximo do tamanho medio.";
    else if(textHas(l,"dificuldade extremos")) h.effect="Aumentar cria uma barreira evolutiva mais forte conforme o tamanho se afasta de 1.0; diminuir facilita chegar aos extremos.";
    else if(textHas(l,"chance")||textHas(l,"mutar ")||textHas(l,"adicionar ")||textHas(l,"remover ")) h.effect="Quanto maior, mais vezes esse evento tende a aparecer ao longo das geracoes. Nao garante que aconteca; apenas muda a probabilidade.";
    else if(textHas(l,"expoente")||textHas(l," exp")) h.effect="Esse numero muda o formato da relacao: acima de 1 costuma fazer valores altos crescerem mais depressa; entre 0 e 1 costuma suavizar as diferencas.";
    else if(textHas(l,"min")) h.effect="Esse e um piso: a evolucao ou a regra nao pode passar abaixo dele. Aumentar o piso elimina resultados menores.";
    else if(textHas(l,"max")) h.effect="Esse e um teto: a evolucao ou a regra nao pode passar acima dele. Aumentar o teto permite resultados maiores.";
    else if(textHas(l,"custo")) h.effect="Aumentar faz essa caracteristica consumir mais energia; diminuir a torna mais barata e, portanto, potencialmente mais facil de manter pela selecao.";
    else if(textHas(l,"friccao")||textHas(l,"arrasto")) h.effect="Aumentar aumenta a resistencia ao movimento; diminuir deixa corpos manterem movimento por mais tempo.";
    else if(textHas(l,"ganho")) h.effect="Aumentar amplifica esse efeito; diminuir enfraquece. Em zero, essa contribuicao deixa de ter efeito.";
    else if(textHas(l,"peso ")||textHas(l,"peso_")) h.effect="Aumentar faz este fator contar mais quando o sistema combina varias informacoes; diminuir faz ele contar menos.";
    else if(textHas(l,"duracao")||textHas(l,"tempo")||textHas(l,"intervalo")||textHas(l,"cooldown")) h.effect="Aumentar normalmente faz o processo levar mais tempo ou acontecer com menos frequencia; diminuir encurta o intervalo.";
    else if(textHas(l,"alcance")||textHas(l,"distancia")||textHas(l,"raio")) h.effect="Aumentar amplia a area ou distancia afetada; diminuir torna a regra mais local.";
    else if((textHas(l,"mutacao")||textHas(l," mut ")) && textHas(l,"forca")) h.effect="Aumentar permite que cada mutacao mude o gene mais de uma vez; diminuir faz mudancas pequenas e graduais. Isso nao aumenta a frequencia das mutacoes.";
    else if(textHas(l,"dano")||textHas(l,"mordida")||textHas(l,"forca")) h.effect="Aumentar deixa essa interacao fisica mais intensa; diminuir a torna mais fraca.";
    else if(textHas(l,"multiplicador")||textHas(l,"escala")) h.effect="1.0 costuma significar efeito normal. Abaixo de 1 reduz proporcionalmente; acima de 1 aumenta. O resultado final ainda pode ser limitado por outras leis.";
    else if(textHas(l,"referencia")) h.effect="Mudar a referencia muda a base usada na comparacao. Isso pode alterar varios resultados derivados mesmo sem mudar os genes dos individuos.";
    else if(textHas(l,"jitter")||textHas(l,"ruido")) h.effect="Aumentar cria mais variacao aleatoria entre resultados parecidos; diminuir deixa o sistema mais previsivel e repetitivo.";
    else if(textHas(l,"elasticidade")) h.effect="Aumentar faz colisoes devolverem mais movimento e parecerem mais 'quicantes'; diminuir deixa os contatos mais amortecidos.";
    else if(textHas(l,"amortecimento")) h.effect="Aumentar faz o movimento externo desaparecer mais rapido; diminuir deixa empurroes e objetos deslizando persistirem por mais tempo.";
    else if(textHas(l,"correcao penetracao")) h.effect="Aumentar separa corpos sobrepostos mais agressivamente; diminuir corrige de forma mais suave, mas pode deixar sobreposicao visivel por mais tempo.";
    else if(textHas(l,"slop colisao")) h.effect="Aumentar tolera um pouco mais de sobreposicao antes de corrigir; diminuir torna a deteccao de contato mais rigorosa.";
    else if(textHas(l,"iteracoes solver")) h.effect="Aumentar melhora a estabilidade quando muitos corpos se empurram ao mesmo tempo, com custo maior de CPU.";
    else if(textHas(l,"fonte gatilho")) h.effect="Aumentar essa proporcao faz esse tipo de fonte aparecer mais na criacao/mutacao dos gatilhos emocionais em relacao as outras fontes.";
    else if(l.rfind("vies:",0)==0) h.effect="Aumentar torna esse alvo mais comum no vies inicial dos fundadores que recebem o circuito; depois a evolucao pode alterar ou perder essas conexoes.";
    else if(textHas(l,"delta")) h.effect="Aumentar permite saltos maiores quando a mutacao acontece; diminuir faz a evolucao caminhar em passos menores.";
    else if(textHas(l,"reserva")) h.effect="Aumentar obriga o organismo a guardar mais recurso antes/depois da acao; diminuir permite gastar mais agressivamente.";
    else if(textHas(l,"tentativas")) h.effect="Aumentar reduz falhas por azar na busca, mas faz o sistema tentar mais vezes; diminuir economiza processamento e aceita mais falhas.";
    else if(textHas(l,"curva")) h.effect="O valor nao soma diretamente uma quantidade: ele muda a forma da resposta, fazendo a diferenca aparecer mais cedo, mais tarde ou mais forte nos extremos.";
    else if(textHas(l,"atualizacao")) h.effect="Menor intervalo = resposta mais frequente. Maior intervalo = menos recalculos e resposta mais espaçada.";
    else if(textHas(l,"risco")) h.effect="Aumentar torna o risco mais relevante; diminuir o torna raro/fraco. Zero normalmente remove essa contribuicao quando a formula permite.";
    else if(textHas(l,"gap")) h.effect="Aumentar deixa mais espaco vazio entre os corpos; diminuir permite posicionamentos mais apertados.";
    else if(textHas(l,"sway")) h.effect="Aumentar faz um mesmo contato lateral deslocar mais a copa; diminuir deixa a vegetacao visualmente mais firme.";
    else h.effect="Aumentar ou diminuir este valor altera diretamente \""+label+"\" dentro de "+categoryPurpose(cat)+". Use o exemplo abaixo como referencia e mude aos poucos para observar o efeito.";

    if(l=="fundador lateral/frente"){
        h.example="Ex.: 0.75 significa que um bicho com velocidade frontal maxima 20 tera velocidade lateral maxima 15.";
    }else if(l=="gene lateral ratio min" || l=="gene lateral ratio max"){
        h.example="Ex.: ratio 0.60 = lateral a 60% da velocidade frontal; 0.95 = quase tao rapido quanto para frente.";
    }else if(l=="penalidade energia multi-eixo"){
        h.example="Ex.: com valor 1.0, usar frente e lateral ambos perto de 100% adiciona aproximadamente mais um bloco de custo ativo de translacao.";
    }else if(l=="penalidade forca multi-eixo"){
        h.example="Ex.: valor 0.5 reduz a aceleracao disponivel quando os dois eixos estao muito ativos; andando so para frente, essa penalidade fica praticamente zero.";
    }else if(l=="carga max / massa corpo"){
        h.example="Ex.: 1.35 significa que um bicho de massa 10 pode segurar uma carga de ate aproximadamente 13.5. Uma carga de massa 20 seria pesada demais.";
    }else if(l=="tamanho adulto medio"){
        h.example="Ex.: se o medio for 10, gene 0.6 tende a uma planta adulta de tamanho 6; gene 1.5 tende a tamanho 15.";
    }else if(l=="dificuldade extremos tamanho"){
        h.example="Ex.: com resistencia alta, passar de escala 1.8 para 1.9 e muito mais raro do que passar de 1.0 para 1.1.";
    }else if(textHas(l,"expoente") || textHas(l," exp")){
        h.example="Ex.: 1 tende a uma relacao linear; 2 reforca valores altos; 0 pode neutralizar a dependencia quando a formula permite.";
    }else if((textHas(l,"mutacao")||textHas(l," mut ")) && textHas(l,"forca")){
        h.example="Ex.: forca 0.05 produz alteracoes pequenas quando a mutacao ocorre; 0.50 permite saltos geneticos bem maiores. A chance fica em outro controle.";
    }else if(textHas(l,"elasticidade")){
        h.example="Ex.: perto de 0 quase nao ha quique; aumentar devolve mais movimento depois da batida. Nao significa que 1 seja obrigatoriamente o melhor valor.";
    }else if(textHas(l,"amortecimento")){
        h.example="Ex.: amortecimento alto faz um objeto empurrado perder velocidade rapidamente; baixo faz ele deslizar por mais tempo.";
    }else if(d.percent){
        h.example="Ex.: 0.10 = 10%, 0.50 = 50% e 1.00 = 100%.";
    }else if(d.integer){
        h.example="Ex.: "+configValueText(d,d.minValue)+" fica no limite inferior e "+configValueText(d,d.maxValue)+" no limite superior permitido por este controle.";
    }else if(textHas(l,"multiplicador")||textHas(l,"escala")){
        h.example="Ex.: 1.0 = referencia normal; 0.5 = aproximadamente metade da contribuicao; 2.0 = aproximadamente o dobro antes de outros limites.";
    }else if(textHas(l,"jitter")||textHas(l,"ruido")){
        h.example="Ex.: 0 = sem variacao aleatoria adicionada; um valor maior faz dois casos iguais poderem terminar com resultados um pouco diferentes.";
    }else if(textHas(l,"elasticidade")){
        h.example="Ex.: perto de 0, dois corpos tendem a nao quicar; aumentando, parte maior do impacto volta como movimento de separacao.";
    }else if(textHas(l,"amortecimento")){
        h.example="Ex.: valor alto faz um objeto empurrado parar cedo; valor baixo deixa ele continuar deslizando por mais tempo.";
    }else if(textHas(l,"delta")){
        h.example="Ex.: delta 0.05 permite mudancas pequenas por mutacao; delta 0.50 permite saltos muito maiores quando ela ocorre.";
    }else if(textHas(l,"tentativas")){
        h.example="Ex.: 1 tentativa desiste logo se falhar; 20 tentativas procuram varias alternativas antes de desistir.";
    }else if(textHas(l,"transmissao")){
        h.example="Ex.: 1.0 deixa passar tudo; 0.5 deixa aproximadamente metade; 0 bloqueia.";
    }else{
        h.example="Ex.: valores perto de "+configValueText(d,d.minValue)+" ficam no extremo baixo do controle; perto de "+configValueText(d,d.maxValue)+" ficam no extremo alto.";
    }
    return h;
}

std::vector<std::string> wrapConfigText(const std::string& text,int fontSize,int maxWidth) {
    std::vector<std::string> lines;
    std::istringstream ss(text);
    std::string word,line;
    while(ss>>word){
        std::string test=line.empty()?word:line+" "+word;
        if(!line.empty() && MeasureText(test.c_str(),fontSize)>maxWidth){lines.push_back(line);line=word;}
        else line=test;
    }
    if(!line.empty()) lines.push_back(line);
    if(lines.empty()) lines.push_back("");
    return lines;
}

void drawConfigInfoTooltip(int cat,const TuningSliderDef& d,Vector2 mouse,int sw,int sh) {
    const ConfigHelpText h=configHelpFor(cat,d);
    const float w=470.0f;
    const int fs=13;
    const int inner=(int)w-30;
    auto what=wrapConfigText(h.summary,fs,inner);
    auto effect=wrapConfigText(h.effect,fs,inner);
    auto example=wrapConfigText(h.example,fs,inner);
    const float hgt=80.0f+(what.size()+effect.size()+example.size())*18.0f+72.0f;
    float x=mouse.x+18.0f,y=mouse.y+15.0f;
    if(x+w>sw-10) x=mouse.x-w-18.0f;
    if(y+hgt>sh-10) y=sh-hgt-10.0f;
    x=clampf(x,10.0f,std::max(10.0f,(float)sw-w-10.0f));
    y=clampf(y,10.0f,std::max(10.0f,(float)sh-hgt-10.0f));
    RectF box{x,y,w,hgt};
    // Fundo totalmente opaco: nenhum titulo/slider desenhado atras pode atravessar a ajuda.
    DrawRectangle((int)x-3,(int)y-3,(int)w+6,(int)hgt+6,Color{0,0,0,220});
    DrawRectangleRounded(box,0.028f,7,Color{8,13,23,255});
    DrawRectangleRoundedLines(box,0.028f,7,1.4f,Color{64,143,244,255});
    DrawCircleV({x+19.0f,y+20.0f},10.0f,Color{47,128,237,255});
    DrawText("i",(int)x+17,(int)y+12,13,WHITE);
    DrawText(d.label,(int)x+38,(int)y+11,16,RAYWHITE);
    const std::string current="Valor atual: "+configDisplayValue(d,*d.value);
    DrawText(current.c_str(),(int)x+38,(int)y+33,12,Color{120,185,255,255});
    float yy=y+58.0f;
    auto section=[&](const char* title,const std::vector<std::string>& lines,Color c){
        DrawText(title,(int)x+14,(int)yy,12,c);yy+=17.0f;
        for(const auto& ln:lines){DrawText(ln.c_str(),(int)x+14,(int)yy,fs,Color{222,228,239,255});yy+=18.0f;}
        yy+=7.0f;
    };
    section("O que e?",what,Color{105,181,255,255});
    section("Na pratica, o que muda?",effect,Color{128,211,166,255});
    section("Exemplo simples",example,Color{244,190,105,255});
}

struct ConfigFileLoadReport { int applied=0, unknown=0; };

bool writeConfigTextFile(const std::string& path,std::string& error) {
    std::ofstream out(path,std::ios::trunc);
    if(!out.good()){error="Nao foi possivel criar vida_config.ini.";return false;}
    out << "# Vida Artificial Evolutiva v0.082 - configuracoes editaveis\n";
    out << "# Edite somente o valor depois de '='. Linhas com # sao comentarios.\n";
    out << "# Percentuais usam valor real: 0.25 = 25%, 1.0 = 100%.\n";
    out << "# As alteracoes sao aplicadas ao usar Carregar/F9.\n\n";
    for(int cat=0;cat<(int)ConfigCategory::Count;++cat){
        out << '[' << CONFIG_CATEGORY_NAMES[cat] << "]\n";
        const auto defs=configDefsForCategory(cat);
        for(const auto& d:defs){
            const auto help=configHelpFor(cat,d);
            out << "# O que e: " << help.summary << "\n";
            out << "# Na pratica: " << help.effect << "\n";
            out << "# Exemplo: " << help.example << "\n";
            out << configKeyFromLabel(d.label) << " = " << configValueText(d,*d.value) << "\n\n";
        }
    }
    out.flush();
    if(!out.good()){error="Falha ao terminar vida_config.ini.";return false;}
    return true;
}

bool readConfigTextFile(const std::string& path,ConfigFileLoadReport& report,std::string& error) {
    std::ifstream in(path);
    if(!in.good()){error="Arquivo vida_config.ini nao encontrado ao lado do executavel.";return false;}

    std::unordered_map<std::string,TuningSliderDef> lookup;
    for(int cat=0;cat<(int)ConfigCategory::Count;++cat){
        const auto defs=configDefsForCategory(cat);
        const std::string section=lowerAscii(CONFIG_CATEGORY_NAMES[cat]);
        for(const auto& d:defs) lookup[section+"."+configKeyFromLabel(d.label)]=d;
    }

    std::string section,line;
    int lineNo=0;
    while(std::getline(in,line)){
        ++lineNo;
        if(!line.empty()&&line.back()=='\r') line.pop_back();
        const std::string t=trimAscii(line);
        if(t.empty()||t[0]=='#'||t[0]==';') continue;
        if(t.front()=='['&&t.back()==']'){
            section=lowerAscii(trimAscii(t.substr(1,t.size()-2)));
            continue;
        }
        const std::size_t eq=t.find('=');
        if(eq==std::string::npos){++report.unknown;continue;}
        const std::string key=lowerAscii(trimAscii(t.substr(0,eq)));
        std::string valueText=trimAscii(t.substr(eq+1));
        const std::size_t hash=valueText.find('#'); if(hash!=std::string::npos) valueText=trimAscii(valueText.substr(0,hash));
        const auto it=lookup.find(section+"."+key);
        if(it==lookup.end()){++report.unknown;continue;}
        char* end=nullptr;
        const float raw=std::strtof(valueText.c_str(),&end);
        if(end==valueText.c_str() || !std::isfinite(raw)){
            error="Valor invalido em vida_config.ini, linha "+std::to_string(lineNo)+": "+valueText;
            return false;
        }
        const TuningSliderDef& d=it->second;
        float v=clampf(raw,d.minValue,d.maxValue);
        if(d.integer) v=std::round(v);
        *d.value=v;
        ++report.applied;
    }
    if(report.applied==0){error="vida_config.ini nao contem nenhuma configuracao reconhecida.";return false;}
    cfg::sanitizeTuning();
    return true;
}

void drawConfigPopup(Simulation& sim, UIState& ui, int sw, int sh) {
    if (!ui.configPopup) return;
    RectF p = clampPopup({20.0f,54.0f,std::max(760.0f,(float)sw-40.0f),std::max(520.0f,(float)sh-74.0f)}, sw, sh);
    DrawRectangleRounded(p,0.018f,6,Color{8,8,10,253});
    DrawRectangleRoundedLines(p,0.018f,6,1.0f,Color{105,115,140,235});
    const bool reinforcementTab=ui.configCategory==(int)ConfigCategory::Reforco;
    DrawText(reinforcementTab?"APRENDIZADO COM REFORCO - funcao de recompensa":"Configuracao completa da simulacao",(int)p.x+18,(int)p.y+14,20,RAYWHITE);
    DrawText(reinforcementTab?"Positivo reforca; negativo pune. O reward modifica conexoes plasticas usando o traco de elegibilidade e e herdado pelos descendentes.":"Valores e REGRAS alteram a simulacao. Exponentes/pesos mudam as leis do mundo; limites estruturais continuam protegidos.",
             (int)p.x+18,(int)p.y+40,11,Color{170,180,200,255});
    const float reinforcementHeaderExtra=reinforcementTab?30.0f:0.0f;
    if(reinforcementTab){
        const auto& rr=sim.reinforcementRecords;
        DrawText(TextFormat("RECORDES: vida %.1fs (#%d) | menor %.1fs (#%d) | ovos %d (#%d) | menos %d (#%d)",
            rr.longestLife,rr.longestLifeId,rr.hasShortestLife?rr.shortestLife:0.0f,rr.shortestLifeId,rr.mostEggs,rr.mostEggsId,rr.hasFewestEggs?rr.fewestEggs:0,rr.fewestEggsId),
            (int)p.x+18,(int)p.y+58,9,Color{235,198,105,255});
        DrawText(TextFormat("Sangue >50 %.1fs (#%d) | queda 50->0 %.1fs (#%d) || Energia >50 %.1fs (#%d) | queda 50->0 %.1fs (#%d)",
            rr.longestHealthAbove50,rr.healthAboveId,rr.fastestHealth50ToZero,rr.healthCrashId,rr.longestEnergyAbove50,rr.energyAboveId,rr.fastestEnergy50ToZero,rr.energyCrashId),
            (int)p.x+18,(int)p.y+72,9,Color{175,200,220,255});
    }

    // abas
    const int catCount=(int)ConfigCategory::Count;
    const float tabGap=5.0f;
    const float tabW=(p.width-36.0f-tabGap*(catCount-1))/catCount;
    float tabY=p.y+64.0f+reinforcementHeaderExtra;
    for(int i=0;i<catCount;++i){
        RectF r{p.x+18.0f+i*(tabW+tabGap),tabY,tabW,30.0f};
        if(button(r,CONFIG_CATEGORY_NAMES[i],ui.configCategory==i)){
            ui.configCategory=i; ui.configScroll=0; ui.configSlider=-1;
        }
    }

    RectF content{p.x+18,p.y+104+reinforcementHeaderExtra,p.width-36,p.height-166-reinforcementHeaderExtra};
    DrawRectangleRounded(content,0.018f,5,Color{14,15,19,245});
    DrawRectangleRoundedLines(content,0.018f,5,1.0f,Color{55,62,78,220});

    auto defs=configDefsForCategory(ui.configCategory);
    const int columns = content.width > 980.0f ? 3 : 2;
    const float gap=18.0f;
    const float colW=(content.width-28.0f-gap*(columns-1))/columns;
    const float rowH=55.0f;
    const int rows=(int)((defs.size()+columns-1)/columns);
    const float fullH=rows*rowH+12.0f;
    const float maxScroll=std::max(0.0f,fullH-content.height+16.0f);

    Vector2 mouse=GetMousePosition();
    if(pointIn(content,mouse)){
        float wheel=GetMouseWheelMove();
        if(wheel!=0) ui.configScroll=clampf(ui.configScroll-wheel*50.0f,0.0f,maxScroll);
    }

    int hoveredInfo=-1;
    BeginScissorMode((int)content.x+2,(int)content.y+2,(int)content.width-4,(int)content.height-4);
    for(int i=0;i<(int)defs.size();++i){
        int col=i%columns, row=i/columns;
        float x=content.x+14.0f+col*(colW+gap);
        float y=content.y+12.0f+row*rowH-ui.configScroll;
        if(y+45<content.y || y>content.y+content.height) continue;
        auto& d=defs[i];
        float v=*d.value;
        const std::string val=configDisplayValue(d,v);

        // Nome a esquerda, valor a direita e um pequeno "i" azul sempre no mesmo ponto.
        const float infoCx=x+colW-8.0f;
        const float valueRight=infoCx-16.0f;
        std::string shown=d.label;
        const int maxLabelW=(int)std::max(40.0f,colW-105.0f);
        while(shown.size()>4 && MeasureText(shown.c_str(),12)>maxLabelW) shown.resize(shown.size()-1);
        if(shown!=d.label){while(shown.size()>3 && shown.back()==' ')shown.pop_back();shown+="...";}
        DrawText(shown.c_str(),(int)x,(int)y,12,Color{220,222,230,255});
        DrawText(val.c_str(),(int)(valueRight-MeasureText(val.c_str(),12)),(int)y,12,Color{165,190,232,255});
        const RectF infoRect{infoCx-9.0f,y-3.0f,18.0f,18.0f};
        const bool infoHover=pointIn(infoRect,mouse);
        DrawCircleV({infoCx,y+6.0f},7.0f,infoHover?Color{75,158,255,255}:Color{47,128,237,245});
        DrawText("i",(int)infoCx-2,(int)y,11,WHITE);
        if(infoHover) hoveredInfo=i;

        RectF s{x,y+20,colW,13};
        DrawRectangleRounded(s,0.45f,6,Color{34,36,43,240});
        float t=(d.maxValue>d.minValue)?clampf((v-d.minValue)/(d.maxValue-d.minValue),0,1):0;
        DrawRectangleRounded({s.x,s.y,s.width*t,s.height},0.45f,6,Color{83,118,190,245});
        DrawCircleV({s.x+s.width*t,s.y+s.height*0.5f},6.2f,Color{235,235,240,255});
        int sliderId=ui.configCategory*1000+i;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && pointIn({s.x-5,s.y-7,s.width+10,s.height+14},mouse)) ui.configSlider=sliderId;
        if(ui.configSlider==sliderId && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            float pos=clampf((mouse.x-s.x)/s.width,0,1);
            float nv=d.minValue+(d.maxValue-d.minValue)*pos;
            if(d.integer) nv=std::round(nv);
            *d.value=nv;
            cfg::sanitizeTuning();
        }
    }
    EndScissorMode();
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) ui.configSlider=-1;
    const float footerY=p.y+p.height-48.0f;
    if(button({p.x+18,footerY,168,32},"Restaurar tudo")){
        cfg::tuning.reset(); cfg::sanitizeTuning(); ui.configScroll=0; ui.configSlider=-1;
    }
    DrawText(TextFormat("%d parametros nesta aba | role o mouse para ver todos",(int)defs.size()),
             (int)p.x+204,(int)footerY+10,11,Color{155,165,185,255});
    if(button({p.x+p.width-118,footerY,100,32},"Fechar")){
        ui.configPopup=false; ui.configSlider=-1;
    }
    // Sempre por ultimo: garante que nenhuma aba, titulo, valor ou botao cubra a explicacao.
    if(hoveredInfo>=0 && hoveredInfo<(int)defs.size()) drawConfigInfoTooltip(ui.configCategory,defs[hoveredInfo],mouse,sw,sh);
}



// =========================================================
// v0.079 - MAPA DO SISTEMA / CAUSALIDADE / HISTORICO / GENEALOGIA
// =========================================================
std::string systemOutputLabel(int o){
    static const char* names[cfg::OUTPUT_COUNT]={
        "Frente","Re","Girar E","Girar D","Lateral E","Lateral D","Agarrar",
        "Morder","Reservado 1","Acasalar","Cor R","Cor G","Cor B",
        "Mem escreve0","Mem escreve1","Mem escreve2","Mem escreve3","Mem gate0","Mem gate1","Mem gate2","Mem gate3"
    };
    return (o>=0&&o<cfg::OUTPUT_COUNT)?names[o]:"Saida "+std::to_string(o);
}

std::string brainStateSourceLabel(int idx){
    if(idx>=cfg::BRAIN_INPUT_BASE && idx<cfg::BRAIN_INPUT_BASE+cfg::INPUT_COUNT)
        return brainInputLabel(idx-cfg::BRAIN_INPUT_BASE);
    if(idx>=cfg::BRAIN_HIDDEN_BASE && idx<cfg::BRAIN_HIDDEN_BASE+cfg::MAX_HIDDEN)
        return "H"+std::to_string(idx-cfg::BRAIN_HIDDEN_BASE)+" atual";
    if(idx>=cfg::BRAIN_PREV_HIDDEN_BASE && idx<cfg::BRAIN_PREV_HIDDEN_BASE+cfg::MAX_HIDDEN)
        return "H"+std::to_string(idx-cfg::BRAIN_PREV_HIDDEN_BASE)+" memoria";
    if(idx>=cfg::BRAIN_PREV_OUTPUT_BASE && idx<cfg::BRAIN_PREV_OUTPUT_BASE+cfg::OUTPUT_COUNT)
        return systemOutputLabel(idx-cfg::BRAIN_PREV_OUTPUT_BASE)+" anterior";
    return "Estado "+std::to_string(idx);
}

void drawSystemArrow(Vector2 a,Vector2 b,Color c){
    DrawLineEx(a,b,2.0f,c);
    Vector2 d=sub(b,a);float m=std::sqrt(length2(d));if(m<1e-3f)return;d=mul(d,1.0f/m);
    Vector2 n{-d.y,d.x};
    Vector2 p1=add(b,add(mul(d,-9.0f),mul(n,5.0f)));
    Vector2 p2=add(b,add(mul(d,-9.0f),mul(n,-5.0f)));
    DrawTriangle(b,p1,p2,c);
}

void drawSystemNode(RectF r,const char* title,const char* detail,Color border){
    DrawRectangleRounded(r,0.08f,6,Color{20,23,30,248});
    DrawRectangleRoundedLines(r,0.08f,6,1.5f,border);
    DrawText(title,(int)r.x+10,(int)r.y+8,14,RAYWHITE);
    if(detail) DrawText(detail,(int)r.x+10,(int)r.y+29,10,Color{170,183,205,255});
}

void drawHistoryChart(RectF r,const char* title,const std::vector<float>& values,Color lineColor,bool percent=false){
    DrawRectangleRounded(r,0.035f,5,Color{15,17,22,245});
    DrawRectangleRoundedLines(r,0.035f,5,1.0f,Color{68,74,88,210});
    DrawText(title,(int)r.x+8,(int)r.y+7,12,Color{220,224,232,255});
    if(values.empty()){DrawText("Sem historico ainda",(int)r.x+8,(int)r.y+30,11,Color{130,135,145,255});return;}
    float mn=*std::min_element(values.begin(),values.end()),mx=*std::max_element(values.begin(),values.end());
    if(percent){mn=0;mx=1;} else if(std::fabs(mx-mn)<1e-6f){mn-=0.5f;mx+=0.5f;}
    const float lx=r.x+8,rx=r.x+r.width-8,ty=r.y+25,by=r.y+r.height-18;
    DrawLineEx({lx,by},{rx,by},1.0f,Color{55,60,70,180});
    if(values.size()==1){float t=clampf((values[0]-mn)/(mx-mn),0,1);DrawCircleV({lx,(by-(by-ty)*t)},2.5f,lineColor);}
    else for(std::size_t i=1;i<values.size();++i){
        float x0=lx+(rx-lx)*float(i-1)/float(values.size()-1),x1=lx+(rx-lx)*float(i)/float(values.size()-1);
        float y0=by-(by-ty)*clampf((values[i-1]-mn)/(mx-mn),0,1),y1=by-(by-ty)*clampf((values[i]-mn)/(mx-mn),0,1);
        DrawLineEx({x0,y0},{x1,y1},1.8f,lineColor);
    }
    std::string range=percent?TextFormat("0%% .. 100%% | atual %.1f%%",values.back()*100.0f):TextFormat("%.2f .. %.2f | atual %.2f",mn,mx,values.back());
    DrawText(range.c_str(),(int)r.x+8,(int)(r.y+r.height-14),9,Color{135,145,160,255});
}

void drawSystemMapTab(const Simulation& sim,RectF a){
    (void)sim;
    const float w=(a.width-60.0f)/4.0f,h=66.0f,gap=16.0f;
    const float x0=a.x+12,y0=a.y+18;
    RectF laws{x0,y0,w,h},dna{x0+w+gap,y0,w,h},phen{x0+2*(w+gap),y0,w,h},state{x0+3*(w+gap),y0,w,h};
    RectF world{x0,y0+115,w,h},sense{x0+w+gap,y0+115,w,h},brain{x0+2*(w+gap),y0+115,w,h},outputs{x0+3*(w+gap),y0+115,w,h};
    RectF interact{x0+3*(w+gap),y0+230,w,h},conseq{x0+2*(w+gap),y0+230,w,h},select{x0+w+gap,y0+230,w,h},next{x0,y0+230,w,h};
    Color blue{72,148,245,255},green{80,190,130,255},amber{230,170,70,255},purple{165,105,230,255};
    drawSystemNode(laws,"CONFIG / LEIS","~400 regras editaveis",blue);
    drawSystemNode(dna,"DNA","corpo, sentidos, cerebro",purple);
    drawSystemNode(phen,"FENOTIPO","massa, limites, custos",green);
    drawSystemNode(state,"ESTADO","energia, vida, idade, memoria",green);
    drawSystemNode(world,"MUNDO","plantas, carne, ovos, borda",blue);
    drawSystemNode(sense,"SENSORES","visao + tato",amber);
    drawSystemNode(brain,"CEREBRO + EMOCOES","rede, recorrencia, persistencia",purple);
    drawSystemNode(outputs,"OUTPUTS","movimento + acasalar + cor",amber);
    drawSystemNode(interact,"INTERACOES","colisao, mordida, arrasto",amber);
    drawSystemNode(conseq,"CONSEQUENCIAS","energia, dano, ovo, morte",green);
    drawSystemNode(select,"SELECAO","sobrevive / reproduz mais",purple);
    drawSystemNode(next,"PROXIMA GERACAO","heranca + mutacao",purple);
    drawSystemArrow({laws.x+laws.width,laws.y+33},{dna.x,dna.y+33},blue);
    drawSystemArrow({dna.x+dna.width,dna.y+33},{phen.x,phen.y+33},purple);
    drawSystemArrow({phen.x+phen.width,phen.y+33},{state.x,state.y+33},green);
    drawSystemArrow({world.x+world.width,world.y+33},{sense.x,sense.y+33},blue);
    drawSystemArrow({state.x+state.width*.5f,state.y+state.height},{outputs.x+outputs.width*.5f,outputs.y-10},green);
    drawSystemArrow({sense.x+sense.width,sense.y+33},{brain.x,brain.y+33},amber);
    drawSystemArrow({brain.x+brain.width,brain.y+33},{outputs.x,outputs.y+33},purple);
    drawSystemArrow({outputs.x+outputs.width*.5f,outputs.y+outputs.height},{interact.x+interact.width*.5f,interact.y},amber);
    drawSystemArrow({interact.x,interact.y+33},{conseq.x+conseq.width,conseq.y+33},amber);
    drawSystemArrow({conseq.x,conseq.y+33},{select.x+select.width,select.y+33},green);
    drawSystemArrow({select.x,select.y+33},{next.x+next.width,next.y+33},purple);
    drawSystemArrow({next.x+next.width*.5f,next.y},{dna.x+dna.width*.5f,dna.y+dna.height+8},purple);

    float fy=a.y+330;
    DrawText("Leis atuais (exemplos reais):",(int)a.x+14,(int)fy,14,RAYWHITE);fy+=24;
    DrawText(TextFormat("Massa bicho = densidade x (1 + dureza x %.2f) x tamanho ^ %.2f",cfg::tuning.hardnessMassBonus,cfg::tuning.creatureMassSizeExponent),(int)a.x+20,(int)fy,11,Color{185,195,210,255});fy+=18;
    DrawText("Clique no Config para alterar as leis; nenhuma seta deste mapa cria significado semantico para o cerebro.",(int)a.x+20,(int)fy,11,Color{145,165,195,255});
}

void drawCausalityTab(const Simulation& sim,RectF a){
    Creature* c=sim.selected();
    if(!c){DrawText("Selecione um bicho para ver a causalidade neural real.",(int)a.x+20,(int)a.y+30,16,Color{205,210,220,255});return;}
    const float col=(a.width-54)/3.0f;
    float x1=a.x+14,x2=x1+col+13,x3=x2+col+13;
    DrawText(TextFormat("Bicho #%d | especie analitica S%d | geracao %d | pais #%d / #%d",c->id,c->speciesId,c->generation,c->parentId,c->secondParentId),(int)x1,(int)a.y+14,14,RAYWHITE);
    DrawText(TextFormat("Energia %.1f/%.1f | vida %.1f | massa %.2f | visao %.3f e/s",c->energy,c->maxEnergy,c->health,c->mass(),c->visionEnergyCostPerSecond()),(int)x1,(int)a.y+35,11,Color{170,185,205,255});

    std::vector<std::pair<float,int>> inputs;inputs.reserve(cfg::INPUT_COUNT);
    for(int i=0;i<cfg::INPUT_COUNT;++i)inputs.push_back({std::fabs(c->inputs[i]),i});
    std::sort(inputs.begin(),inputs.end(),[](auto&a,auto&b){return a.first>b.first;});
    std::vector<std::pair<float,int>> outputs;outputs.reserve(cfg::OUTPUT_COUNT);
    for(int o=0;o<cfg::OUTPUT_COUNT;++o)outputs.push_back({std::fabs(c->brainOut[o]),o});
    std::sort(outputs.begin(),outputs.end(),[](auto&a,auto&b){return a.first>b.first;});

    float y=a.y+72;
    DrawText("INPUTS MAIS FORTES",(int)x1,(int)y,13,Color{100,175,255,255});y+=23;
    for(int k=0;k<std::min<int>(10,inputs.size());++k){int i=inputs[k].second;DrawText(TextFormat("%2d. %-25s %+.3f",k+1,brainInputLabel(i).c_str(),c->inputs[i]),(int)x1,(int)y,10,Color{205,210,218,255});y+=18;}

    y=a.y+72;DrawText("OUTPUTS ATUAIS",(int)x2,(int)y,13,Color{236,174,84,255});y+=23;
    for(int k=0;k<cfg::OUTPUT_COUNT;++k){int o=outputs[k].second;DrawText(TextFormat("%2d. %-16s %.3f",k+1,systemOutputLabel(o).c_str(),c->brainOut[o]),(int)x2,(int)y,10,Color{205,210,218,255});y+=18;}

    const int dom=outputs.empty()?0:outputs.front().second;
    y=a.y+72;DrawText(("CONTRIBUICOES -> "+systemOutputLabel(dom)).c_str(),(int)x3,(int)y,13,Color{185,120,245,255});y+=23;
    struct CnVal{float mag,val;std::string label;};std::vector<CnVal> cvs;
    if(dom>=0&&dom<(int)c->compiled.outputIncoming.size())for(const auto& cn:c->compiled.outputIncoming[dom]){
        const float w=(cn.geneIndex>=0&&cn.geneIndex<(int)c->learnedWeights.size())?c->learnedWeights[(std::size_t)cn.geneIndex]:cn.weight;
        const float val=(cn.sourceIndex>=0&&cn.sourceIndex<cfg::BRAIN_STATE_COUNT)?c->brainState[cn.sourceIndex]*w:0.0f;
        cvs.push_back({std::fabs(val),val,brainStateSourceLabel(cn.sourceIndex)});
    }
    std::sort(cvs.begin(),cvs.end(),[](const CnVal&a,const CnVal&b){return a.mag>b.mag;});
    if(cvs.empty()){DrawText("Sem conexoes diretas ativas.",(int)x3,(int)y,10,Color{160,165,175,255});y+=18;}
    for(int k=0;k<std::min<int>(10,cvs.size());++k){DrawText(TextFormat("%2d. %-21s %+.3f",k+1,cvs[k].label.c_str(),cvs[k].val),(int)x3,(int)y,10,cvs[k].val>=0?Color{145,215,165,255}:Color{235,135,135,255});y+=18;}

    const float by=a.y+a.height-118;
    DrawText("FEEDBACK CORPORAL",(int)x1,(int)by,12,Color{90,195,185,255});
    DrawText(TextFormat("pressao F/D/T/E %.2f %.2f %.2f %.2f | impacto %.2f | dano %.2f | energia+ %.2f",c->contactPressure[0],c->contactPressure[1],c->contactPressure[2],c->contactPressure[3],c->recentImpact,c->recentDamage,c->recentEnergyGain),(int)x1,(int)by+20,10,Color{190,205,205,255});
    DrawText("SENTIDOS / SINALIZACAO",(int)x1,(int)by+44,12,Color{100,175,255,255});
    DrawText(TextFormat("visao foco %.2f assim %.2f RGB %.2f/%.2f/%.2f efic %.2f",c->genome.vision.focusExponent,c->genome.vision.asymmetry,c->genome.vision.sensitivityR,c->genome.vision.sensitivityG,c->genome.vision.sensitivityB,c->genome.vision.efficiency),(int)x1,(int)by+64,10,Color{190,205,220,255});
}

void drawHistoryTab(const Simulation& sim,UIState& ui,RectF a){
    const auto& hs=sim.evolutionHistory;
    std::vector<float> pop,species,size,speed,connections,energy;
    pop.reserve(hs.size());species.reserve(hs.size());size.reserve(hs.size());speed.reserve(hs.size());connections.reserve(hs.size());energy.reserve(hs.size());
    for(const auto& h:hs){pop.push_back((float)h.creatures);species.push_back((float)h.speciesCount);size.push_back(h.avgBodySize);speed.push_back(h.avgForwardGene);connections.push_back(h.avgConnections);energy.push_back(h.avgEnergyRatio*100.0f);}
    const float gap=12,w=(a.width-gap*3)/2.0f,h=std::max(70.0f,(a.height-150.0f-gap*4)/3.0f);
    drawHistoryChart({a.x+gap,a.y+gap,w,h},"Populacao de bichos",pop,Color{95,185,125,255});
    drawHistoryChart({a.x+gap*2+w,a.y+gap,w,h},"Especies analiticas",species,Color{180,115,235,255});
    drawHistoryChart({a.x+gap,a.y+gap*2+h,w,h},"Tamanho medio",size,Color{230,170,80,255});
    drawHistoryChart({a.x+gap*2+w,a.y+gap*2+h,w,h},"Velocidade frontal genetica",speed,Color{90,155,240,255});
    drawHistoryChart({a.x+gap,a.y+gap*3+h*2,w,h},"Ligacoes neurais medias",connections,Color{90,200,205,255});
    drawHistoryChart({a.x+gap*2+w,a.y+gap*3+h*2,w,h},"Energia media",energy,Color{235,105,155,255},true);

    const float my=a.y+a.height-122.0f;
    DrawText("CHECKPOINTS / MARCOS",(int)a.x+12,(int)my,12,Color{120,180,245,255});
    DrawText("O botao Marco cria pares .dat + .ini em /saves. Os mais recentes podem ser carregados aqui.",(int)a.x+12,(int)my+18,9,Color{150,160,180,255});
    std::vector<std::filesystem::path> checkpoints;
    try{
        std::filesystem::path dir(saveio::executableDirectory());dir/="saves";
        if(std::filesystem::exists(dir)) for(const auto& ent:std::filesystem::directory_iterator(dir)){
            if(ent.is_regular_file() && ent.path().extension()==".dat"){
                std::filesystem::path ini=ent.path();ini.replace_extension(".ini");if(std::filesystem::exists(ini))checkpoints.push_back(ent.path());
            }
        }
        std::sort(checkpoints.begin(),checkpoints.end(),[](const auto&a,const auto&b){
            std::error_code ea,eb;auto ta=std::filesystem::last_write_time(a,ea),tb=std::filesystem::last_write_time(b,eb);
            if(ea||eb) return a.filename().string()>b.filename().string();
            return ta>tb;
        });
    }catch(...){checkpoints.clear();}
    float bx=a.x+12,by=my+42;
    if(checkpoints.empty())DrawText("Nenhum marco salvo ainda.",(int)bx,(int)by,10,Color{135,140,155,255});
    for(int i=0;i<std::min<int>(4,checkpoints.size());++i){
        const auto& world=checkpoints[i];std::filesystem::path ini=world;ini.replace_extension(".ini");
        std::string name=world.stem().string();
        DrawText(name.c_str(),(int)bx,(int)by+7,10,Color{200,205,215,255});
        if(button({a.x+a.width-112,by,92,25},"Carregar")){ui.checkpointWorldPath=world.string();ui.checkpointConfigPath=ini.string();ui.checkpointLoadRequested=true;ui.systemPopup=false;}
        by+=29;
    }
}

void drawGenealogyTab(const Simulation& sim,RectF a){
    Creature* c=sim.selected();
    if(!c){DrawText("Selecione um bicho para explorar sua genealogia.",(int)a.x+20,(int)a.y+28,16,Color{205,210,220,255});return;}
    auto rec=[&](int id)->const GenealogyRecord*{auto it=sim.genealogyIndex.find(id);return it==sim.genealogyIndex.end()?nullptr:&sim.genealogy[it->second];};
    DrawText(TextFormat("Genealogia de #%d | geracao %d | especie analitica S%d",c->id,c->generation,c->speciesId),(int)a.x+18,(int)a.y+15,15,RAYWHITE);
    DrawText("A especie e somente uma classificacao por distancia genetica; nao e um gene nem um sensor.",(int)a.x+18,(int)a.y+38,10,Color{150,165,190,255});
    struct Node{int id,depth;float x;};
    std::vector<Node> level{{c->id,0,a.x+a.width*0.5f}};
    float y=a.y+85;
    for(int depth=0;depth<5 && !level.empty();++depth){
        std::vector<Node> next;
        const float spacing=a.width/float(level.size()+1);
        for(std::size_t i=0;i<level.size();++i){
            int id=level[i].id;float x=a.x+spacing*float(i+1);const GenealogyRecord* r=rec(id);
            RectF box{x-82,y,164,52};
            DrawRectangleRounded(box,0.08f,5,Color{20,23,31,245});
            DrawRectangleRoundedLines(box,0.08f,5,1.2f,depth==0?Color{90,165,250,255}:Color{115,100,180,230});
            if(r){DrawText(TextFormat("#%d | G%d | S%d",r->id,r->generation,r->speciesId),(int)box.x+8,(int)box.y+7,11,RAYWHITE);DrawText(TextFormat("pais %d / %d",r->parentA,r->parentB),(int)box.x+8,(int)box.y+25,9,Color{170,180,198,255});}
            else DrawText(TextFormat("#%d (fora do arquivo)",id),(int)box.x+8,(int)box.y+16,10,Color{160,165,175,255});
            if(r && depth<4){
                if(r->parentA>=0)next.push_back({r->parentA,depth+1,0});
                if(r->parentB>=0 && r->parentB!=r->parentA)next.push_back({r->parentB,depth+1,0});
            }
        }
        if(next.size()>16)next.resize(16);
        // Linhas sao simplificadas para manter a arvore legivel quando ha muitos ancestrais.
        if(!next.empty())DrawText("↑ pais / ancestrais",(int)a.x+18,(int)(y+56),9,Color{130,145,170,255});
        level.swap(next);y+=88;
    }
    DrawText(TextFormat("Arquivo genealogico: %d individuos registrados | historico: %d amostras",(int)sim.genealogy.size(),(int)sim.evolutionHistory.size()),(int)a.x+18,(int)(a.y+a.height-22),10,Color{145,155,175,255});
}


std::string perfMb(std::size_t bytes){
    char b[64]{};const double mb=double(bytes)/(1024.0*1024.0);
    if(mb<0.10)std::snprintf(b,sizeof(b),"%.1f KB",double(bytes)/1024.0);
    else std::snprintf(b,sizeof(b),"%.2f MB",mb);
    return b;
}

Color perfLoadColor(double frameMs,double budgetMs,bool waiting=false){
    if(waiting)return Color{145,175,220,255};
    const double r=budgetMs>0?frameMs/budgetMs:0.0;
    if(r>=0.50)return Color{245,90,85,255};
    if(r>=0.25)return Color{240,170,75,255};
    if(r>=0.10)return Color{220,205,90,255};
    return Color{105,205,135,255};
}

void drawPerformanceTab(Simulation& sim,RectF a,bool& paused){
    const ProcessTelemetry os=readProcessTelemetry();
    const double budget=1000.0/60.0;
    const std::size_t steps=std::max<std::size_t>(1,sim.perf.substepsLastFrame);
    const double simFrame=sim.perf.simFrame.avgMs;
    const double frame=sim.perf.frameTotal.avgMs;
    const double headroom=std::max(0.0,budget-simFrame-sim.perf.renderWorld.avgMs-sim.perf.renderUi.avgMs);

    const float pad=10.0f, gap=7.0f;
    const float cardW=(a.width-pad*2-gap*5)/6.0f;
    const float cy=a.y+10.0f;
    auto card=[&](int i,const char* label,const std::string& value,const std::string& detail,Color c){
        RectF r{a.x+pad+i*(cardW+gap),cy,cardW,64.0f};
        DrawRectangleRounded(r,0.06f,6,Color{20,23,30,248});DrawRectangleRoundedLines(r,0.06f,6,1.0f,c);
        DrawText(label,(int)r.x+8,(int)r.y+7,9,Color{160,170,190,255});
        DrawText(value.c_str(),(int)r.x+8,(int)r.y+23,14,RAYWHITE);
        DrawText(detail.c_str(),(int)r.x+8,(int)r.y+45,8,Color{135,145,165,255});
    };
    card(0,"FPS / frame",TextFormat("%d / %.2f ms",GetFPS(),frame),"alvo 60 FPS = 16.67 ms",perfLoadColor(frame,budget));
    card(1,"CPU processo",TextFormat("%.1f%%",os.cpuPercent),TextFormat("%u processadores logicos",os.logicalProcessors),perfLoadColor(os.cpuPercent/100.0*budget,budget));
    card(2,"RAM residente",TextFormat("%.1f MB",os.workingSetMB),"Working Set real do Windows",Color{105,170,235,255});
    const double effectiveSpeed=sim.perf.realDtLastFrame>1e-9?sim.perf.simProcessedDtLastFrame/sim.perf.realDtLastFrame:0.0;
    card(3,"Velocidade efetiva",TextFormat("%.2fx real | alvo %.1fx",effectiveSpeed,sim.perf.simSpeedLastFrame),
        TextFormat("real %.1f ms -> sim %.1f ms",sim.perf.realDtLastFrame*1000.0,sim.perf.simProcessedDtLastFrame*1000.0),
        perfLoadColor(std::max(0.0,sim.perf.simSpeedLastFrame-effectiveSpeed)/std::max(0.1f,sim.perf.simSpeedLastFrame)*budget,budget));
    card(4,"Subpassos",TextFormat("%zu usados",sim.perf.substepsLastFrame),TextFormat("%zu evitados por atraso",sim.perf.preventedSubstepsLastFrame),sim.perf.preventedSubstepsLastFrame?Color{240,170,75,255}:Color{105,205,135,255});
    card(5,"Simulacao / folga",TextFormat("%.2f / %.2f ms",simFrame,headroom),TextFormat("descartou %.1f ms simulados",sim.perf.simDroppedDtLastFrame*1000.0),perfLoadColor(simFrame,budget));

    float y=cy+74.0f;
    DrawText("CPU POR PROCESSO INTERNO - custo real por frame, do mais pesado para o mais leve",(int)a.x+pad,(int)y,11,RAYWHITE);
    const float pauseW=148.0f;
    if(button({a.x+a.width-pad-pauseW,y-5.0f,pauseW,25.0f},paused?"Continuar simulacao":"Pausar simulacao",paused)) paused=!paused;
    y+=22;
    DrawText(TextFormat("Subpassos: %zu executados | sem protecao seriam %zu | anti-espiral evitou %zu | dt sim processado %.1f ms | descartado %.1f ms",
        sim.perf.substepsLastFrame,sim.perf.uncappedSubstepsLastFrame,sim.perf.preventedSubstepsLastFrame,
        sim.perf.simProcessedDtLastFrame*1000.0,sim.perf.simDroppedDtLastFrame*1000.0),
        (int)a.x+pad,(int)y,8,sim.perf.preventedSubstepsLastFrame?Color{235,185,90,255}:Color{145,155,175,255});
    y+=24;

    const float tableL=a.x+pad;
    const float tableR=a.x+a.width-pad;
    const float nameW=std::clamp(a.width*0.42f,320.0f,500.0f);
    const float metricsL=tableL+nameW;
    const float metricW=std::max(54.0f,(tableR-metricsL)/6.0f);
    const float hx[6]={metricsL,metricsL+metricW,metricsL+metricW*2,metricsL+metricW*3,metricsL+metricW*4,metricsL+metricW*5};
    DrawText("Processo / funcao",(int)tableL+7,(int)y,9,Color{150,160,180,255});
    DrawText("ult ms",(int)hx[0]+5,(int)y,9,Color{150,160,180,255});
    DrawText("media",(int)hx[1]+5,(int)y,9,Color{150,160,180,255});
    DrawText("pico",(int)hx[2]+5,(int)y,9,Color{150,160,180,255});
    DrawText("custo/frame",(int)hx[3]+5,(int)y,9,Color{150,160,180,255});
    DrawText("% 16.7ms",(int)hx[4]+5,(int)y,9,Color{150,160,180,255});
    DrawText("estado",(int)hx[5]+5,(int)y,9,Color{150,160,180,255});
    y+=15;

    enum class CostMode : std::uint8_t { PerSubstep, PerFrame, PerSimInterval, PerRealInterval, EventOnly, Custom };
    enum class NodeKind : std::uint8_t { Leaf, Sensors, Physics };
    struct Row{
        std::string name;
        double last=0.0,avg=0.0,peak=0.0;
        CostMode mode=CostMode::PerSubstep;
        double factor=1.0;
        bool wait=false,eventual=false;
        NodeKind kind=NodeKind::Leaf;
        int depth=0,rank=0;
        double est=0.0,pct=0.0;
    };
    auto fromMetric=[&](const char* name,const PerfMetric& m,CostMode mode,double factor=1.0,bool wait=false,NodeKind kind=NodeKind::Leaf,int depth=0){
        Row r;r.name=name;r.last=m.lastMs;r.avg=m.avgMs;r.peak=m.peakMs;r.mode=mode;r.factor=factor;r.wait=wait;r.kind=kind;r.depth=depth;return r;
    };
    auto calc=[&](Row& r){
        if(r.mode!=CostMode::Custom){
            switch(r.mode){
                case CostMode::PerSubstep:r.est=r.avg*double(steps)*r.factor;break;
                case CostMode::PerFrame:r.est=r.avg*r.factor;break;
                case CostMode::PerSimInterval:r.est=r.avg*(sim.perf.simProcessedDtLastFrame/std::max(0.000001,r.factor));break;
                case CostMode::PerRealInterval:r.est=r.avg/(std::max(0.001,r.factor)*60.0);r.eventual=true;break;
                case CostMode::EventOnly:r.est=0.0;r.eventual=true;break;
                case CostMode::Custom:break;
            }
        }
        r.pct=r.est/budget*100.0;
    };


    // Nivel 0: somente processos macro/folhas independentes. Processos macro PESADOS
    // ou CRITICOS sao expandidos automaticamente logo abaixo, preservando a relacao pai->filho.
    std::vector<Row> top={
        fromMetric("Sensores TOTAL",sim.perf.sensing,CostMode::PerSubstep,1,false,NodeKind::Sensors),
        fromMetric("Fisica TOTAL",sim.perf.physics,CostMode::PerSubstep,1,false,NodeKind::Physics),
        fromMetric("Emocoes: atualizar estados internos",sim.perf.emotions,CostMode::PerSubstep),
        fromMetric("Cerebro: executar redes neurais",sim.perf.brains,CostMode::PerSubstep),
        fromMetric("Agarrar: decidir intencao",sim.perf.grabIntent,CostMode::PerSubstep),
        fromMetric("Movimento: motores + metabolismo",sim.perf.movement,CostMode::PerSubstep),
        fromMetric("Agarrar: manter carga fisica",sim.perf.grabConstraints,CostMode::PerSubstep,2),
        fromMetric("Plantas: resistencia ao atravessar copas",sim.perf.plantSway,CostMode::PerSimInterval,0.10),
        fromMetric("Plantas: crescer/comer/decompor",sim.perf.plants,CostMode::PerSubstep),
        fromMetric("Mordidas: busca + dano + alimento",sim.perf.bites,CostMode::PerSubstep),
        fromMetric("Mortes: detectar + gerar carcaca",sim.perf.deaths,CostMode::PerSubstep),
        fromMetric("Carne: movimento + decomposicao",sim.perf.carcasses,CostMode::PerSubstep),
        fromMetric("Ovos: incubacao + eclosao",sim.perf.eggs,CostMode::PerSubstep),
        fromMetric("Threads: despacho + acordar workers",sim.perf.parallelDispatch,CostMode::PerSubstep),
        fromMetric("Threads: espera residual dos workers",sim.perf.parallelResidualWait,CostMode::PerSubstep),
        fromMetric("Populacao: garantir minimos",sim.perf.minimums,CostMode::PerSubstep),
        fromMetric("Estatisticas evolutivas (a cada 0.5s sim)",sim.perf.stats,CostMode::PerSimInterval,0.50),
        fromMetric("Historico: gravar amostra",sim.perf.history,CostMode::PerSimInterval,std::max(0.01f,cfg::tuning.evolutionHistoryInterval)),
        fromMetric("Render mundo: culling + geometria",sim.perf.renderWorld,CostMode::PerFrame),
        fromMetric("Render HUD/UI: paineis + texto",sim.perf.renderUi,CostMode::PerFrame),
        fromMetric("GPU/Present: enviar + VSync",sim.perf.renderSubmit,CostMode::PerFrame,1,true),
        fromMetric("Salvar/autosave: I/O eventual",sim.perf.saveIo,cfg::tuning.autosaveIntervalSeconds>0?CostMode::PerRealInterval:CostMode::EventOnly,std::max(0.001f,cfg::tuning.autosaveIntervalSeconds)),
        fromMetric("Carregar: I/O eventual",sim.perf.loadIo,CostMode::EventOnly)
    };
    for(auto& r:top)calc(r);
    std::stable_sort(top.begin(),top.end(),[](const Row& lhs,const Row& rhs){if(std::fabs(lhs.est-rhs.est)>0.000001)return lhs.est>rhs.est;return lhs.peak>rhs.peak;});

    std::vector<Row> rows;rows.reserve(top.size()+16);
    int rankCounter=0;
    std::string hotPath="Nenhum processo pesado no momento";
    double hotCost=0.0;
    for(auto parent:top){
        parent.rank=++rankCounter;rows.push_back(parent);
        if(parent.est>hotCost){hotCost=parent.est;hotPath=parent.name;}
        const bool expand=parent.pct>=25.0 && !parent.wait && !parent.eventual;
        if(parent.kind==NodeKind::Sensors && expand){
            std::vector<Row> children={
                fromMetric("Cobertura por copa",sim.perf.sensorCover,CostMode::PerSubstep,1,false,NodeKind::Leaf,1),
                fromMetric(sim.perf.gpuVisionActive?"Visao GPU: grade + readback + dispatch":"Visao CPU: raios + celulas + oclusao",sim.perf.sensorVision,CostMode::PerSubstep,1,false,NodeKind::Leaf,1),
                fromMetric("Inputs: visao + tato + estado -> rede",sim.perf.sensorInputs,CostMode::PerSubstep,1,false,NodeKind::Leaf,1)
            };
            for(auto& c:children)calc(c);
            std::stable_sort(children.begin(),children.end(),[](const Row& l,const Row& r){return l.est>r.est;});
            if(!children.empty() && children.front().est>0.0) hotPath="Sensores > "+children.front().name;
            for(auto child:children){
                rows.push_back(child);

            }
        }else if(parent.kind==NodeKind::Physics && expand){
            std::vector<Row> children={
                fromMetric("Montar/reindexar buckets",sim.perf.physicsBuckets,CostMode::PerSubstep,1,false,NodeKind::Leaf,1),
                fromMetric("Resolver pares/colisoes dinamicos",sim.perf.physicsPairs,CostMode::PerSubstep,1,false,NodeKind::Leaf,1),
                fromMetric("Plantas: sem colisao (desativado)",sim.perf.physicsStaticPlants,CostMode::PerSubstep,1,false,NodeKind::Leaf,1),
                fromMetric("Preparar corpos + atualizar grids",sim.perf.physicsOther,CostMode::PerSubstep,1,false,NodeKind::Leaf,1)
            };
            for(auto& c:children)calc(c);
            std::stable_sort(children.begin(),children.end(),[](const Row& l,const Row& r){return l.est>r.est;});
            if(!children.empty()) hotPath="Fisica > "+children.front().name;
            for(auto c:children)rows.push_back(c);
        }
    }

    DrawText(TextFormat("PROFILER ADAPTATIVO: PESADO/CRITICO expande automaticamente | caminho quente: %s",hotPath.c_str()),
        (int)tableL+7,(int)(cy+109.0f),8,Color{205,190,125,255});

    const float memY=a.y+a.height-111.0f;
    const float diagY=memY-88.0f;
    const float availableRows=std::max(1.0f,diagY-y-3.0f);
    const float rowH=std::clamp(availableRows/std::max<std::size_t>(1,rows.size()),9.2f,15.5f);
    const int rowFont=rowH<11.0f?7:(rowH<12.3f?8:9);
    for(const Row& r:rows){
        Color c;const char* st=nullptr;
        if(r.wait){c=Color{145,175,220,255};st="ESPERA";}
        else if(r.eventual){c=Color{150,145,220,255};st="EVENTUAL";}
        else{c=perfLoadColor(r.est,budget,false);st=r.pct>=50?"CRITICO":(r.pct>=25?"PESADO":(r.pct>=10?"ATENCAO":"OK"));}
        Color bg=c;bg.a=r.wait?24:(r.eventual?22:(r.depth?24:34));
        Color border=c;border.a=r.wait?80:(r.eventual?75:(r.depth?72:105));
        RectF rr{tableL,y-1.5f,tableR-tableL,rowH-0.7f};
        DrawRectangleRounded(rr,0.025f,4,bg);DrawRectangleRoundedLines(rr,0.025f,4,1.0f,border);
        for(int ci=0;ci<6;++ci)DrawLineEx({hx[ci],rr.y+1},{hx[ci],rr.y+rr.height-1},1.0f,Color{95,105,120,65});
        std::string prefix;
        if(r.depth==1)prefix="    > ";else if(r.depth>=2)prefix="        >> ";
        const std::string label=r.depth==0?std::string(TextFormat("%02d  %s",r.rank,r.name.c_str())):prefix+r.name;
        DrawText(label.c_str(),(int)tableL+7,(int)y,rowFont,c);
        DrawText(TextFormat("%.3f",r.last),(int)hx[0]+5,(int)y,rowFont,c);
        DrawText(TextFormat("%.3f",r.avg),(int)hx[1]+5,(int)y,rowFont,c);
        DrawText(TextFormat("%.3f",r.peak),(int)hx[2]+5,(int)y,rowFont,c);
        DrawText(TextFormat("%.3f",r.est),(int)hx[3]+5,(int)y,rowFont,c);
        DrawText(TextFormat("%.0f%%",r.pct),(int)hx[4]+5,(int)y,rowFont,c);
        DrawText(st,(int)hx[5]+5,(int)y,rowFont,c);
        y+=rowH;
    }

    DrawLineEx({a.x+pad,diagY-4},{a.x+a.width-pad,diagY-4},1.0f,Color{60,66,78,210});
    DrawText(TextFormat("SENSORES ultimo subpasso: %zu bichos | %llu raios | %llu celulas | candidatos: plantas %llu, bichos %llu, carnes %llu, ovos %llu",
        sim.creatures.size(),(unsigned long long)sim.perf.sensorRays,(unsigned long long)sim.perf.sensorCells,
        (unsigned long long)sim.perf.sensorPlantChecks,(unsigned long long)sim.perf.sensorCreatureChecks,
        (unsigned long long)sim.perf.sensorCarcassChecks,(unsigned long long)sim.perf.sensorEggChecks),
        (int)a.x+pad,(int)diagY,8,Color{170,185,210,255});
    DrawText(TextFormat("VISAO %s: dispatch %llu | readback %llu | pendentes %zu | refs grade %zu | %s / %s",
        sim.perf.gpuVisionActive?"GPU COMPUTE":"CPU fallback",
        (unsigned long long)sim.perf.gpuVisionDispatches,(unsigned long long)sim.perf.gpuVisionReadbacks,
        sim.perf.gpuVisionPendingBatches,sim.perf.gpuVisionObjectRefs,
        sim.perf.gpuVisionAppliedLastPass?"resultado aplicado":"cache visual",
        sim.perf.gpuVisionSubmittedLastPass?"novo lote enviado":"sem novo lote"),
        (int)a.x+pad,(int)diagY+11,8,sim.perf.gpuVisionActive?Color{120,210,160,255}:Color{205,165,120,255});
    DrawText(TextFormat("THREADS ultimo subpasso: %zu parallelFor | despacho %.3f ms | espera residual %.3f ms",
        sim.perf.parallelCallsLastSubstep,sim.perf.parallelDispatch.lastMs,sim.perf.parallelResidualWait.lastMs),
        (int)a.x+pad,(int)diagY+44,8,Color{135,150,180,255});
    const double foodPct=sim.perf.plantBiteActions>0?100.0*double(sim.perf.plantFoodBites)/double(sim.perf.plantBiteActions):0.0;
    DrawText(TextFormat("ALIMENTACAO: %llu mordidas planta | %llu com alimento (%.1f%%) | biomassa %.1f | energia +%.1f | zero %llu | enraizadas",
        (unsigned long long)sim.perf.plantBiteActions,(unsigned long long)sim.perf.plantFoodBites,foodPct,
        sim.perf.plantBiomassConsumed,sim.perf.plantEnergyGained,(unsigned long long)sim.perf.plantZeroFoodBites),
        (int)a.x+pad,(int)diagY+55,8,Color{155,205,155,255});
    DrawText(TextFormat("FISICA: dinamicos %zu = bichos %zu + carnes %zu + ovos %zu | pares %llu | contatos %llu (BB %llu, outros %llu)",
        sim.perf.physicsBodyCreatures+sim.perf.physicsBodyCarcasses+sim.perf.physicsBodyEggs,
        sim.perf.physicsBodyCreatures,sim.perf.physicsBodyCarcasses,sim.perf.physicsBodyEggs,
        (unsigned long long)sim.perf.physicsCandidatePairs,(unsigned long long)sim.perf.physicsContactPairs,
        (unsigned long long)sim.perf.physicsCreatureCreatureContacts,(unsigned long long)sim.perf.physicsOtherContacts),
        (int)a.x+pad,(int)diagY+66,8,Color{205,175,125,255});

    DrawLineEx({a.x+pad,memY-7},{a.x+a.width-pad,memY-7},1.0f,Color{60,66,78,210});
    DrawText("MEMORIA - RAM total acima e real; abaixo sao estimativas atribuiveis e podem nao somar o processo inteiro",(int)a.x+pad,(int)memY,9,Color{175,185,205,255});
    const std::size_t mems[]={sim.estimatedEntityPoolBytes(),sim.estimatedNeuralBytes(),sim.estimatedSensorIndexBytes(),sim.estimatedPhysicsBytes(),sim.estimatedHistoryBytes(),GetRenderCpuBufferBytes(),GetRenderGpuVertexBufferBytes()};
    const char* labels[]={"Entidades/pools","Cerebros/DNA","Sensores/grades","Fisica buffers","Historico/notas","Render CPU","Vertex buffer GPU"};
    const float mw=(a.width-pad*2)/4.0f;
    for(int i=0;i<7;++i){
        const int col=i%4,row=i/4;const float x=a.x+pad+col*mw,yy=memY+18+row*24;
        DrawText(labels[i],(int)x,(int)yy,8,Color{145,155,175,255});
        const std::string v=perfMb(mems[i]);DrawText(v.c_str(),(int)x,(int)yy+10,10,Color{205,215,230,255});
    }
    DrawText(TextFormat("Conhecido/atribuido CPU+GPU ~%s | handles Windows %lu | draw calls %u | vertices %u",perfMb(sim.estimatedKnownBytes()).c_str(),os.handleCount,GetRenderDrawCalls(),GetRenderVertexCount()),
        (int)(a.x+a.width*0.50f),(int)(a.y+a.height-17),8,Color{135,145,165,255});
}

void drawSystemPopup(Simulation& sim,UIState& ui,int sw,int sh,bool& paused){
    if(!ui.systemPopup)return;
    RectF p{26.0f,42.0f,std::max(760.0f,(float)sw-52.0f),std::max(520.0f,(float)sh-68.0f)};
    if(p.x+p.width>sw-10) p.width=sw-p.x-10;
    if(p.y+p.height>sh-10) p.height=sh-p.y-10;
    DrawRectangleRounded(p,0.025f,8,Color{9,11,16,252});
    DrawRectangleRoundedLines(p,0.025f,8,1.5f,Color{67,102,155,240});
    DrawText("Sistemas / Diagnostico em tempo real",(int)p.x+18,(int)p.y+13,18,RAYWHITE);
    const char* tabs[5]={"Mapa","Causalidade","Desempenho","Historico","Genealogia"};
    float tx=p.x+18,ty=p.y+42;
    for(int i=0;i<5;++i){if(button({tx,ty,118,29},tabs[i],ui.systemTab==i))ui.systemTab=i;tx+=125;}
    if(button({p.x+p.width-96,ty,78,29},"Fechar"))ui.systemPopup=false;
    RectF content{p.x+12,p.y+80,p.width-24,p.height-92};
    DrawRectangleRounded(content,0.02f,5,Color{12,14,19,235});
    if(ui.systemTab==0)drawSystemMapTab(sim,content);
    else if(ui.systemTab==1)drawCausalityTab(sim,content);
    else if(ui.systemTab==2)drawPerformanceTab(sim,content,paused);
    else if(ui.systemTab==3)drawHistoryTab(sim,ui,content);
    else drawGenealogyTab(sim,content);
}

RectF plantOpacitySliderRect(float uiTop){ return RectF{382.0f,uiTop+62.0f,220.0f,14.0f}; }

void drawPlantOpacitySlider(const UIState& ui,float uiTop){
    const RectF r=plantOpacitySliderRect(uiTop);
    DrawText(TextFormat("Opacidade enraizadas: %d%%",(int)std::lround(ui.plantOpacity*100.0f)),(int)r.x,(int)r.y-17,12,Color{200,200,200,230});
    DrawRectangleRounded(r,0.40f,6,Color{35,35,35,235});
    DrawRectangleRounded({r.x,r.y,r.width*ui.plantOpacity,r.height},0.40f,6,Color{110,150,110,235});
    const float knobX=r.x+r.width*ui.plantOpacity;
    DrawCircleV({knobX,r.y+r.height*0.5f},7.0f,Color{225,225,225,255});
    DrawCircleLinesV({knobX,r.y+r.height*0.5f},7.0f,Color{65,65,65,255});
}

RectF mapSizeSliderRect(float uiTop){ return RectF{382.0f,uiTop+102.0f,220.0f,14.0f}; }

float mapSizeToSlider(float size){
    const float minS=cfg::WORLD_MIN_SIZE, maxS=cfg::WORLD_MAX_SIZE;
    const float ratio=std::log(std::max(minS,size)/minS)/std::log(maxS/minS);
    return clampf(ratio,0.0f,1.0f);
}

float mapSliderToSize(float t){
    const float minS=cfg::WORLD_MIN_SIZE, maxS=cfg::WORLD_MAX_SIZE;
    const float raw=minS*std::pow(maxS/minS,clampf(t,0.0f,1.0f));
    const float stepped=std::round(raw/cfg::WORLD_SIZE_STEP)*cfg::WORLD_SIZE_STEP;
    return clampf(stepped,minS,maxS);
}

void drawMapSizeSlider(const Simulation& sim,float uiTop){
    (void)sim;
    const RectF r=mapSizeSliderRect(uiTop);
    const float t=mapSizeToSlider(cfg::WORLD_W);
    DrawText(TextFormat("Mapa: %.0fx%.0f | toroidal (bordas conectadas)",cfg::WORLD_W,cfg::WORLD_H),
        (int)r.x,(int)r.y-17,12,Color{200,200,200,230});
    DrawRectangleRounded(r,0.40f,6,Color{35,35,35,235});
    DrawRectangleRounded({r.x,r.y,r.width*t,r.height},0.40f,6,Color{115,125,165,235});
    const float knobX=r.x+r.width*t;
    DrawCircleV({knobX,r.y+r.height*0.5f},7.0f,Color{225,225,225,255});
    DrawCircleLinesV({knobX,r.y+r.height*0.5f},7.0f,Color{65,65,65,255});
}

RectF liveGraphsPanelRect(int sh,bool populationMinimized,float uiTop) {
    const float top=uiTop+144.0f;
    const RectF population=populationPanelRect(sh,populationMinimized);
    // O dashboard termina sempre antes do controle de populacao: nenhum pixel se sobrepoe.
    const float bottom=population.y-8.0f;
    return {8.0f,top,566.0f,std::max(90.0f,bottom-top)};
}
RectF liveGraphsToggleRect(bool showGraphs,int sh,bool populationMinimized,float uiTop) {
    if(showGraphs){ const RectF p=liveGraphsPanelRect(sh,populationMinimized,uiTop); return {p.x+p.width-112.0f,p.y+8.0f,102.0f,24.0f}; }
    return {8.0f,146.0f,142.0f,28.0f};
}

std::string liveMetricValue(float value,bool percent=false,int decimals=0){
    char b[64]{};
    if(percent){
        if(decimals>0) std::snprintf(b,sizeof(b),"%.*f%%",decimals,(double)value);
        else std::snprintf(b,sizeof(b),"%.0f%%",(double)value);
    }else if(decimals<=0){
        std::snprintf(b,sizeof(b),"%.0f",(double)value);
    }else{
        std::snprintf(b,sizeof(b),"%.*f",decimals,(double)value);
    }
    return b;
}

template <typename Accessor>
void drawLiveMetricCard(RectF r,const char* title,float current,const std::vector<EvolutionHistoryPoint>& hs,
                        Accessor get,Color accent,bool percent=false,int decimals=0){
    DrawRectangleRounded(r,0.06f,5,Color{18,18,20,242});
    DrawRectangleRoundedLines(r,0.06f,5,1.0f,Color{64,66,74,205});
    DrawText(title,(int)r.x+8,(int)r.y+6,10,Color{185,190,205,255});
    const std::string value=liveMetricValue(current,percent,decimals);
    DrawText(value.c_str(),(int)r.x+8,(int)r.y+20,16,RAYWHITE);

    RectF chart{r.x+7.0f,r.y+42.0f,r.width-14.0f,r.height-49.0f};
    DrawLineEx({chart.x,chart.y+chart.height},{chart.x+chart.width,chart.y+chart.height},1.0f,Color{55,57,65,180});

    const std::size_t n=hs.size();
    const std::size_t start=(n>150)?n-150:0;
    float mn=current,mx=current;
    for(std::size_t i=start;i<n;++i){const float v=get(hs[i]);mn=std::min(mn,v);mx=std::max(mx,v);}
    if(std::fabs(mx-mn)<1e-5f){const float pad=std::max(1.0f,std::fabs(mx)*0.08f);mn-=pad;mx+=pad;}
    else {const float pad=(mx-mn)*0.08f;mn-=pad;mx+=pad;}
    auto py=[&](float v){return chart.y+chart.height-(clampf((v-mn)/(mx-mn),0.0f,1.0f)*chart.height);};

    const std::size_t count=n-start;
    Vector2 prev{}; bool have=false;
    for(std::size_t k=0;k<count;++k){
        const std::size_t i=start+k;
        const float x=chart.x+(count<=1?0.0f:(float(k)/float(count))*chart.width);
        const Vector2 pt{x,py(get(hs[i]))};
        if(have)DrawLineEx(prev,pt,1.55f,Fade(accent,0.88f));
        prev=pt;have=true;
    }
    const Vector2 now{chart.x+chart.width,py(current)};
    if(have)DrawLineEx(prev,now,1.7f,accent);
    DrawCircleV(now,2.2f,accent);
}

template <typename AccessorA, typename AccessorB>
void drawDualLiveMetricCard(RectF r,const char* title,
                            float currentA,float currentB,const char* labelA,const char* labelB,
                            const std::vector<EvolutionHistoryPoint>& hs,
                            AccessorA getA,AccessorB getB,Color accentA,Color accentB){
    DrawRectangleRounded(r,0.06f,5,Color{18,18,20,242});
    DrawRectangleRoundedLines(r,0.06f,5,1.0f,Color{64,66,74,205});
    DrawText(title,(int)r.x+8,(int)r.y+5,10,Color{185,190,205,255});

    const std::string aValue=std::string(labelA)+" "+liveMetricValue(currentA,false,0);
    const std::string bValue=std::string(labelB)+" "+liveMetricValue(currentB,false,0);
    DrawText(aValue.c_str(),(int)r.x+8,(int)r.y+18,10,accentA);
    const int aw=MeasureText(aValue.c_str(),10);
    const int bx=(int)std::min(r.x+r.width-8.0f-(float)MeasureText(bValue.c_str(),10),r.x+14.0f+(float)aw);
    DrawText(bValue.c_str(),bx,(int)r.y+18,10,accentB);

    RectF chart{r.x+7.0f,r.y+34.0f,r.width-14.0f,r.height-41.0f};
    DrawLineEx({chart.x,chart.y+chart.height},{chart.x+chart.width,chart.y+chart.height},1.0f,Color{55,57,65,180});
    const std::size_t n=hs.size();
    const std::size_t start=(n>150)?n-150:0;
    float mn=std::min(currentA,currentB),mx=std::max(currentA,currentB);
    for(std::size_t i=start;i<n;++i){
        const float a=getA(hs[i]),b=getB(hs[i]);
        mn=std::min(mn,std::min(a,b));mx=std::max(mx,std::max(a,b));
    }
    if(std::fabs(mx-mn)<1e-5f){const float pad=std::max(1.0f,std::fabs(mx)*0.08f);mn-=pad;mx+=pad;}
    else{const float pad=(mx-mn)*0.08f;mn-=pad;mx+=pad;}
    auto py=[&](float v){return chart.y+chart.height-clampf((v-mn)/(mx-mn),0.0f,1.0f)*chart.height;};
    const std::size_t count=n-start;
    auto drawSeries=[&](auto get,float current,Color accent){
        Vector2 prev{};bool have=false;
        for(std::size_t k=0;k<count;++k){
            const std::size_t i=start+k;
            const float x=chart.x+(count<=1?0.0f:(float(k)/float(count))*chart.width);
            const Vector2 pt{x,py(get(hs[i]))};
            if(have)DrawLineEx(prev,pt,1.45f,Fade(accent,0.88f));
            prev=pt;have=true;
        }
        const Vector2 now{chart.x+chart.width,py(current)};
        if(have)DrawLineEx(prev,now,1.65f,accent);
        DrawCircleV(now,2.0f,accent);
    };
    drawSeries(getA,currentA,accentA);
    drawSeries(getB,currentB,accentB);
}

void drawLiveGraphsDashboard(const Simulation& sim,UIState& ui,bool showGraphs,int sh,float uiTop){
    const RectF toggle=liveGraphsToggleRect(showGraphs,sh,ui.populationMinimized,uiTop);
    if(!showGraphs){
        DrawRectangleRounded(toggle,0.18f,5,Color{25,55,88,242});
        DrawRectangleRoundedLines(toggle,0.18f,5,1.0f,Color{80,165,235,230});
        DrawText("Mostrar graficos",(int)toggle.x+12,(int)toggle.y+7,11,Color{190,225,255,255});
        return;
    }

    const RectF panel=liveGraphsPanelRect(sh,ui.populationMinimized,uiTop);
    DrawRectangleRounded(panel,0.035f,5,Color{8,8,10,238});
    DrawRectangleRoundedLines(panel,0.035f,5,1.0f,Color{72,74,82,210});
    DrawText("Indicadores ao vivo",(int)panel.x+12,(int)panel.y+10,15,RAYWHITE);
    DrawText(TextFormat("28 graficos | %d amostras | %.1fs simulados",(int)sim.evolutionHistory.size(),cfg::tuning.evolutionHistoryInterval),
        (int)panel.x+148,(int)panel.y+13,9,Color{145,155,175,255});
    DrawRectangleRounded(toggle,0.18f,5,Color{38,42,50,245});
    DrawRectangleRoundedLines(toggle,0.18f,5,1.0f,Color{90,100,120,220});
    DrawText("Ocultar",(int)toggle.x+28,(int)toggle.y+6,10,Color{205,210,220,255});

    constexpr float gap=6.0f,pad=8.0f,header=38.0f,cardH=78.0f;
    constexpr int rows=10;
    const float cardW=(panel.width-pad*2.0f-gap*2.0f)/3.0f;
    const RectF viewport{panel.x+pad,panel.y+header,panel.width-pad*2.0f-8.0f,std::max(30.0f,panel.height-header-pad)};
    const float contentH=rows*cardH+(rows-1)*gap;
    const float maxScroll=std::max(0.0f,contentH-viewport.height);
    ui.liveGraphsScroll=clampf(ui.liveGraphsScroll,0.0f,maxScroll);
    if(pointIn(panel,GetMousePosition())){
        const float wheel=GetMouseWheelMove();
        if(wheel!=0.0f) ui.liveGraphsScroll=clampf(ui.liveGraphsScroll-wheel*72.0f,0.0f,maxScroll);
    }
    auto card=[&](int col,int row){return RectF{viewport.x+col*(cardW+gap),viewport.y+row*(cardH+gap)-ui.liveGraphsScroll,cardW,cardH};};
    const auto& e=sim.evo; const auto& h=sim.evolutionHistory;
    auto safeDiv=[](float a,float b){return std::fabs(b)>1e-6f?a/b:0.0f;};

    BeginScissorMode((int)viewport.x,(int)viewport.y,(int)viewport.width,(int)viewport.height);
    drawLiveMetricCard(card(0,0),"Bichos",(float)sim.creatures.size(),h,[](const EvolutionHistoryPoint& x){return (float)x.creatures;},Color{95,205,130,255});
    drawLiveMetricCard(card(1,0),"Plantas totais",(float)sim.plants.size(),h,[](const EvolutionHistoryPoint& x){return (float)x.plants;},Color{95,195,95,255});
    drawLiveMetricCard(card(2,0),"Carnes",(float)sim.carcasses.size(),h,[](const EvolutionHistoryPoint& x){return (float)x.carcasses;},Color{225,95,90,255});

    int currentSoloEggs=0,currentMatedEggs=0;sim.countCurrentEggOrigins(currentSoloEggs,currentMatedEggs);
    drawDualLiveMetricCard(card(0,1),"Ovos atuais",
        (float)currentSoloEggs,(float)currentMatedEggs,"Natural","Acasal.",h,
        [](const EvolutionHistoryPoint& x){return (float)x.eggsCurrentSolo;},
        [](const EvolutionHistoryPoint& x){return (float)x.eggsCurrentMated;},
        Color{225,205,125,255},Color{115,205,245,255});
    drawLiveMetricCard(card(1,1),"Neuronios H medios",e.avgHidden,h,[](const EvolutionHistoryPoint& x){return x.avgHidden;},Color{160,125,235,255},false,1);
    drawLiveMetricCard(card(2,1),"Ligacoes medias",e.avgConnections,h,[](const EvolutionHistoryPoint& x){return x.avgConnections;},Color{120,145,245,255},false,1);

    drawLiveMetricCard(card(0,2),"Memoria neural media",e.avgMemory*100.0f,h,[](const EvolutionHistoryPoint& x){return x.avgMemory*100.0f;},Color{190,120,220,255},true,0);
    drawLiveMetricCard(card(1,2),"Geracao media",e.avgGeneration,h,[](const EvolutionHistoryPoint& x){return x.avgGeneration;},Color{235,165,85,255},false,1);
    drawLiveMetricCard(card(2,2),"Geracao maxima",(float)e.maxGeneration,h,[](const EvolutionHistoryPoint& x){return (float)x.maxGeneration;},Color{245,125,75,255});

    drawLiveMetricCard(card(0,3),"Tamanho medio",e.avgBodySize,h,[](const EvolutionHistoryPoint& x){return x.avgBodySize;},Color{90,195,205,255},false,2);
    drawLiveMetricCard(card(1,3),"Especies",(float)e.speciesCount,h,[](const EvolutionHistoryPoint& x){return (float)x.speciesCount;},Color{205,120,235,255});
    drawLiveMetricCard(card(2,3),"Memoria x ligacoes",e.avgMemory*e.avgConnections,h,[](const EvolutionHistoryPoint& x){return x.avgMemory*x.avgConnections;},Color{235,100,155,255},false,2);

    drawLiveMetricCard(card(0,4),"Energia media",e.avgEnergyRatio*100.0f,h,[](const EvolutionHistoryPoint& x){return x.avgEnergyRatio*100.0f;},Color{120,210,150,255},true,0);
    drawLiveMetricCard(card(1,4),"Idade media",e.avgAge,h,[](const EvolutionHistoryPoint& x){return x.avgAge;},Color{200,185,110,255},false,1);
    drawLiveMetricCard(card(2,4),"Alcance visao medio",e.avgVisionRange,h,[](const EvolutionHistoryPoint& x){return x.avgVisionRange;},Color{100,180,235,255},false,1);

    drawLiveMetricCard(card(0,5),"Mortes por fome",(float)e.starvationDeaths,h,[](const EvolutionHistoryPoint& x){return (float)x.starvationDeaths;},Color{230,160,80,255});
    drawLiveMetricCard(card(1,5),"Mortes por ataque",(float)e.attackDeaths,h,[](const EvolutionHistoryPoint& x){return (float)x.attackDeaths;},Color{235,90,90,255});

    // +11 indicadores derivados usando campos ja presentes no historico, preservando compatibilidade dos saves.
    drawLiveMetricCard(card(2,5),"Velocidade frontal gene",e.avgForwardGene,h,[](const EvolutionHistoryPoint& x){return x.avgForwardGene;},Color{90,150,240,255},false,2);
    drawLiveMetricCard(card(0,6),"Alcance/foco visual",safeDiv(e.avgVisionRange,std::max(0.05f,e.avgVisionFocus)),h,[](const EvolutionHistoryPoint& x){return x.avgVisionRange/std::max(0.05f,x.avgVisionFocus);},Color{90,205,205,255},false,2);
    drawLiveMetricCard(card(1,6),"Foco visual gene",e.avgVisionFocus,h,[](const EvolutionHistoryPoint& x){return x.avgVisionFocus;},Color{100,180,240,255},false,2);
    drawLiveMetricCard(card(2,6),"Plantas por bicho",safeDiv((float)sim.plants.size(),(float)sim.creatures.size()),h,[](const EvolutionHistoryPoint& x){return x.creatures>0?(float)x.plants/(float)x.creatures:0.0f;},Color{110,205,110,255},false,2);

    drawLiveMetricCard(card(0,7),"Ovos por 100 bichos",100.0f*safeDiv((float)sim.eggs.size(),(float)sim.creatures.size()),h,[](const EvolutionHistoryPoint& x){return x.creatures>0?100.0f*(float)x.eggs/(float)x.creatures:0.0f;},Color{225,205,125,255},false,1);
    drawLiveMetricCard(card(1,7),"Carnes por 100 bichos",100.0f*safeDiv((float)sim.carcasses.size(),(float)sim.creatures.size()),h,[](const EvolutionHistoryPoint& x){return x.creatures>0?100.0f*(float)x.carcasses/(float)x.creatures:0.0f;},Color{225,110,95,255},false,1);
    drawLiveMetricCard(card(2,7),"Ligacoes por neuronio H",safeDiv(e.avgConnections,std::max(1.0f,e.avgHidden)),h,[](const EvolutionHistoryPoint& x){return x.avgConnections/std::max(1.0f,x.avgHidden);},Color{145,135,240,255},false,2);

    drawLiveMetricCard(card(0,8),"Gap geracao max-media",std::max(0.0f,(float)e.maxGeneration-e.avgGeneration),h,[](const EvolutionHistoryPoint& x){return std::max(0.0f,(float)x.maxGeneration-x.avgGeneration);},Color{240,145,80,255},false,1);
    drawLiveMetricCard(card(1,8),"Especies por 100 bichos",100.0f*safeDiv((float)e.speciesCount,(float)sim.creatures.size()),h,[](const EvolutionHistoryPoint& x){return x.creatures>0?100.0f*(float)x.speciesCount/(float)x.creatures:0.0f;},Color{195,125,235,255},false,2);
    const float deathPair=(float)(e.starvationDeaths+e.attackDeaths);
    drawLiveMetricCard(card(2,8),"Fome entre fome+ataque",deathPair>0?100.0f*(float)e.starvationDeaths/deathPair:0.0f,h,[](const EvolutionHistoryPoint& x){const float d=(float)(x.starvationDeaths+x.attackDeaths);return d>0?100.0f*(float)x.starvationDeaths/d:0.0f;},Color{230,165,80,255},true,1);

    drawLiveMetricCard(card(0,9),"Bichos por especie",safeDiv((float)sim.creatures.size(),(float)e.speciesCount),h,[](const EvolutionHistoryPoint& x){return x.speciesCount>0?(float)x.creatures/(float)x.speciesCount:0.0f;},Color{170,135,225,255},false,1);
    EndScissorMode();

    if(maxScroll>0.0f){
        const float trackX=panel.x+panel.width-6.0f;
        const float trackY=viewport.y,trackH=viewport.height;
        DrawRectangle((int)trackX,(int)trackY,3,(int)trackH,Color{45,48,58,220});
        const float thumbH=std::max(24.0f,trackH*(viewport.height/contentH));
        const float t=maxScroll>0.0f?ui.liveGraphsScroll/maxScroll:0.0f;
        const float thumbY=trackY+t*(trackH-thumbH);
        DrawRectangle((int)trackX,(int)thumbY,3,(int)thumbH,Color{125,150,190,245});
    }
}

struct WorldRenderStats { int creatures=0, plants=0, carcasses=0, eggs=0; };


WorldRenderStats drawWorld(Simulation& sim, Camera2D& cam, int sw, int sh, bool showPlants, float plantOpacity) {
    WorldRenderStats rs{};
    ClearBackground(cfg::OUTSIDE_BG);

    // O retangulo desenhado e o tile fundamental do mundo toroidal. Entidades
    // reaparecem na borda oposta e sensores atravessam a costura periodica.
    const float worldLeftScreen = cam.offset.x + (0.0f - cam.target.x) * cam.zoom;
    const float worldTopScreen = cam.offset.y + (0.0f - cam.target.y) * cam.zoom;
    const float worldRightScreen = cam.offset.x + (cfg::WORLD_W - cam.target.x) * cam.zoom;
    const float worldBottomScreen = cam.offset.y + (cfg::WORLD_H - cam.target.y) * cam.zoom;
    const int clipL = std::max(0, (int)std::floor(std::min(worldLeftScreen,worldRightScreen)));
    const int clipT = std::max(0, (int)std::floor(std::min(worldTopScreen,worldBottomScreen)));
    const int clipR = std::min(sw, (int)std::ceil(std::max(worldLeftScreen,worldRightScreen)));
    const int clipB = std::min(sh, (int)std::ceil(std::max(worldTopScreen,worldBottomScreen)));

    if (clipR > clipL && clipB > clipT) BeginScissorMode(clipL,clipT,clipR-clipL,clipB-clipT);
    BeginMode2D(cam);
    DrawRectangle(0,0,(int)cfg::WORLD_W,(int)cfg::WORLD_H,cfg::WORLD_BG);

    // Grade de celulas do mundo: ajuda a visualizar o tile espacial usado
    // pelos grids e pela percepcao/fisica. Desenhamos de forma adaptativa
    // para nao poluir demais quando o zoom esta muito afastado.
    {
        const float baseCell = std::max(8.0f,cfg::CELL);
        float step = baseCell;
        while(step * cam.zoom < 14.0f) step *= 2.0f;
        const float minorThickness = 1.0f / std::max(0.05f,cam.zoom);
        const Color minor = Color{90,105,135,60};
        const Color major = Color{125,150,190,105};
        const int majorEvery = std::max(1,(int)std::round((baseCell*4.0f)/step));
        int ix=0;
        for(float x=0.0f; x<=cfg::WORLD_W+0.1f; x+=step,++ix){
            const bool majorLine = (ix % majorEvery)==0;
            DrawLineEx({x,0.0f},{x,cfg::WORLD_H},majorLine?minorThickness*1.15f:minorThickness,majorLine?major:minor);
        }
        int iy=0;
        for(float y=0.0f; y<=cfg::WORLD_H+0.1f; y+=step,++iy){
            const bool majorLine = (iy % majorEvery)==0;
            DrawLineEx({0.0f,y},{cfg::WORLD_W,y},majorLine?minorThickness*1.15f:minorThickness,majorLine?major:minor);
        }
    }

    Vector2 tl=GetScreenToWorld2D({0,0},cam), br=GetScreenToWorld2D({(float)sw,(float)sh},cam);
    const float viewMinX=std::min(tl.x,br.x);
    const float viewMaxX=std::max(tl.x,br.x);
    const float viewMinY=std::min(tl.y,br.y);
    const float viewMaxY=std::max(tl.y,br.y);

    auto visibleCircle = [&](Vector2 p, float radius) {
        return p.x + radius >= viewMinX && p.x - radius <= viewMaxX &&
               p.y + radius >= viewMinY && p.y - radius <= viewMaxY;
    };

    const float margin = 32.0f;
    const float minX=viewMinX-margin, maxX=viewMaxX+margin;
    const float minY=viewMinY-margin, maxY=viewMaxY+margin;

    Creature* sel=sim.selected();
    auto drawToroidalRay=[&](Vector2 start,Vector2 dir,float distance,float thickness,Color color){
        Vector2 p=wrappedPoint(start);
        float remaining=std::max(0.0f,distance);
        constexpr float eps=1e-5f,inf=1e30f;
        int guard=0;
        while(remaining>eps && guard++<256){
            float tx=inf,ty=inf;
            if(dir.x>eps) tx=(cfg::WORLD_W-p.x)/dir.x; else if(dir.x<-eps) tx=(0.0f-p.x)/dir.x;
            if(dir.y>eps) ty=(cfg::WORLD_H-p.y)/dir.y; else if(dir.y<-eps) ty=(0.0f-p.y)/dir.y;
            float seg=std::min(remaining,std::min(tx,ty));
            if(!std::isfinite(seg) || seg<eps) seg=std::min(remaining,0.001f);
            const Vector2 q=add(p,mul(dir,seg));
            DrawLineEx(p,q,thickness,color);
            remaining-=seg;
            if(remaining<=eps) break;
            p=wrappedPoint(add(q,mul(dir,0.002f)));
        }
    };
    if(sel && visibleCircle(sel->pos, sel->genome.vision.range)){
        const int lod = worldLOD(cam.zoom);
        const size_t step = lod == 0 ? 2u : 1u;
        for(size_t i=0;i<sel->rays.size();i+=step){
            float sinO=sel->raySin[i],cosO=sel->rayCos[i];
            Vector2 d{sel->sinA*cosO+sel->cosA*sinO,-sel->cosA*cosO+sel->sinA*sinO};
            Color col=Color{120,160,255,(unsigned char)(lod==0?55:90)};
            if(lod>0){
                if(sel->rays[i].type==RayType::Plant || sel->rays[i].type==RayType::Creature) {
                    col=sel->rays[i].targetColor;
                    col.a=220;
                }
                else if(sel->rays[i].type==RayType::Carcass)col=Color{255,110,80,225};
                else if(sel->rays[i].type==RayType::Egg)col=Color{245,210,110,230};
            }
            drawToroidalRay(sel->pos,d,sel->rays[i].distance,0.7f/cam.zoom,col);
        }
    }

    sim.creatureGrid.forAABB(minX,minY,maxX,maxY,[&](Creature* c){
        const float radius = c->size + (sel==c ? 8.0f/std::max(0.05f,cam.zoom) : 0.0f);
        if (!visibleCircle(c->pos,radius)) return;
        ++rs.creatures;
        drawCreature(*c,sel==c,cam.zoom);
    });
    sim.carcassGrid.forAABB(minX,minY,maxX,maxY,[&](Carcass* k){
        if (!visibleCircle(k->pos,k->visualSize)) return;
        ++rs.carcasses;
        drawCarcass(*k,cam.zoom);
    });
    sim.eggGrid.forAABB(minX,minY,maxX,maxY,[&](Egg* egg){
        if (!visibleCircle(egg->pos,egg->radius*1.15f)) return;
        ++rs.eggs;
        drawEgg(*egg,cam.zoom);
    });
    if (showPlants) {
        sim.plantGrid.forAABB(minX,minY,maxX,maxY,[&](Plant* p){
            if (!visibleCircle(p->pos,p->size*0.75f+5.0f)) return;
            ++rs.plants;
            drawPlant(*p,cam.zoom,plantOpacity);
        });
    }
    EndMode2D();
    if (clipR > clipL && clipB > clipT) EndScissorMode();

    // A linha marca apenas a costura visual do tile fundamental; nao e uma parede.
    BeginMode2D(cam);
    DrawRectangleLinesEx({0,0,cfg::WORLD_W,cfg::WORLD_H},1.4f/cam.zoom,Color{150,175,215,165});
    EndMode2D();
    return rs;
}

// [SEC-MAIN] Inicializacao e loop principal
namespace headless_runtime {
volatile std::sig_atomic_t stopRequested = 0;

void requestStop(int) {
    stopRequested = 1;
}

bool saveHeadlessSession(const Simulation& sim, const char* reason) {
    saveio::SessionMeta meta{};
    meta.cameraTarget = {cfg::WORLD_W * 0.5f, cfg::WORLD_H * 0.5f};
    meta.cameraZoom = 0.48f;
    meta.simSpeed = 1.0f;
    meta.plantOpacity = 0.60f;
    meta.paused = 0;
    meta.showHud = 1;
    meta.showPlants = 1;
    meta.showGraphs = 1;

    std::string error;
    const bool worldOk = sim.saveToFile(saveio::defaultWorldSavePath(), meta, error);
    const bool configOk = worldOk ? writeConfigTextFile(saveio::defaultConfigPath(), error) : false;

    if (worldOk && configOk) {
        std::fprintf(stderr,
            "[HEADLESS] save %s: OK -> %s\n",
            reason ? reason : "",
            saveio::defaultWorldSavePath().c_str());
        return true;
    }

    std::fprintf(stderr,
        "[HEADLESS] save %s: ERRO: %s\n",
        reason ? reason : "",
        error.c_str());
    return false;
}

int run() {
    using Clock = std::chrono::steady_clock;

    std::signal(SIGINT, requestStop);
#ifdef SIGTERM
    std::signal(SIGTERM, requestStop);
#endif

    std::fprintf(stderr,
        "[HEADLESS] Vida Artificial v0.138 iniciada sem janela/renderizacao.\n"
        "[HEADLESS] Limites: bichos=%d..%d | plantas=%d..%d\n"
        "[HEADLESS] Ctrl+C encerra com save. Autosave real: %.0f s.\n",
        20, 400, 10, 3000,
        (double)cfg::tuning.autosaveIntervalSeconds);
    std::fflush(stderr);

    Simulation sim;
    sim.updateSensors();

    const float step = std::max(0.000001f, cfg::tuning.maxSubstep);
    const auto realStart = Clock::now();
    auto lastStatus = realStart;
    auto lastAutosave = realStart;
    unsigned long long steps = 0;

    while (!stopRequested) {
        sim.update(step);
        ++steps;

        // Consultar o relogio a cada 32 subpassos reduz overhead no caminho quente.
        if ((steps & 31ull) != 0ull) continue;

        const auto now = Clock::now();
        const double statusElapsed = std::chrono::duration<double>(now - lastStatus).count();

        if (statusElapsed >= 1.0) {
            const double realSeconds = std::max(0.000001,
                std::chrono::duration<double>(now - realStart).count());
            const double simMultiplier = (double)sim.simTime / realSeconds;

            std::fprintf(stderr,
                "[HEADLESS] real=%7.1fs | sim=%9.1fs | %.2fx | "
                "bichos=%zu plantas=%zu carne=%zu ovos=%zu | "
                "geracao=%d | nascimentos=%lld mortes=%lld | %.3f ms/step\n",
                realSeconds,
                (double)sim.simTime,
                simMultiplier,
                sim.creatures.size(),
                sim.plants.size(),
                sim.carcasses.size(),
                sim.eggs.size(),
                sim.evo.maxGeneration,
                sim.evo.naturalBirths,
                sim.evo.deaths,
                sim.lastUpdateMs);
            std::fflush(stderr);
            lastStatus = now;
        }

        const float autosaveSeconds = cfg::tuning.autosaveIntervalSeconds;
        if (autosaveSeconds > 0.0f &&
            std::chrono::duration<double>(now - lastAutosave).count() >= autosaveSeconds) {
            saveHeadlessSession(sim, "automatico");
            lastAutosave = now;
        }
    }

    std::fprintf(stderr, "\n[HEADLESS] Encerramento solicitado. Salvando mundo...\n");
    saveHeadlessSession(sim, "final");
    std::fprintf(stderr, "[HEADLESS] Finalizado.\n");
    std::fflush(stderr);
    return 0;
}
} // namespace headless_runtime

int main(int argc, char** argv) {
    bool headless = false;

#ifndef _WIN32
    // O shim grafico nao abre janela fora do Windows; Linux/VM entra em headless automaticamente.
    headless = true;
#endif

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--headless") == 0) {
            headless = true;
        } else if (std::strcmp(argv[i], "--help") == 0 ||
                   std::strcmp(argv[i], "-h") == 0) {
            std::fprintf(stderr,
                "Uso: %s [--headless]\n"
                "  --headless  roda somente a simulacao, sem janela/renderizacao.\n"
                "No Linux o modo headless e ativado automaticamente.\n",
                argv[0]);
            return 0;
        }
    }

    if (headless) {
        return headless_runtime::run();
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 760, "Vida Artificial - APRENDIZADO COM REFORCO v0.138");
    SetTargetFPS(60);

    Simulation sim;
    sim.updateSensors(); // raios ja nascem com leitura espacial valida
    UIState ui;
    loadGameSpriteAssets();
    ui.ioToast="Sprites carregados: "+std::to_string(loadedGameSpriteCount())+"/4 (planta/carne/bicho/ovo)";
    ui.ioToastUntil=GetTime()+5.0;
    Camera2D cam{};
    cam.offset={GetScreenWidth()*0.5f,GetScreenHeight()*0.5f};
    cam.target={cfg::WORLD_W*0.5f,cfg::WORLD_H*0.5f};
    cam.zoom=0.48f;

    float simSpeed=1.0f;
    bool paused=false;
    bool showHud=true;
    bool showPlants=true;
    bool showGraphs=true; // dashboard inicial de indicadores; somente visualizacao
    bool dragging=false;
    Vector2 pressPos{};
    double pressTime=0.0;
    DragEntityState entityDrag{};
    double autosaveElapsedReal=0.0;

    while(!WindowShouldClose()) {
        using FrameClock=std::chrono::high_resolution_clock;
        const auto frameWorkStart=FrameClock::now();
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        cam.offset={sw*0.5f,sh*0.5f};
        Creature* tracked=sim.selected();
        const bool draggingTracked =
            entityDrag.active() && entityDrag.kind == DragEntityKind::Creature &&
            entityDrag.ptr == tracked;
        if(tracked && !draggingTracked) cam.target=tracked->pos;

        Vector2 mouse=GetMousePosition();
        const TopControlRects controls = topControlRects(sw);
        const float uiTop = topControlsBottom(controls,showHud)+8.0f;
        const bool editingPopulation = ui.activePopulationField >= 0;
        const bool editingText = editingPopulation || ui.noteInputActive;

        auto openTrackedCreature = [&](Creature* c){
            if(!c) return;
            sim.startWatch(c);
            ui.inspectorOpen=true;
            ui.inspectorMinimized=false;
            ui.inspectorScroll=0;
        };
        auto followRandom = [&](){
            // R e G sao exclusivos. Acionar novamente o modo que ja esta ativo
            // funciona como toggle e libera o acompanhamento.
            if (ui.followMode == FollowMode::Random) {
                ui.followMode = FollowMode::None;
                sim.closeWatch();
                ui.inspectorOpen = false;
                ui.inspectorMinimized = false;
                return;
            }
            ui.followMode=FollowMode::Random; // ativa R e desativa G automaticamente
            openTrackedCreature(sim.randomCreatureDifferent());
        };
        auto followTopGeneration = [&](){
            if (ui.followMode == FollowMode::TopGeneration) {
                ui.followMode = FollowMode::None;
                sim.closeWatch();
                ui.inspectorOpen = false;
                ui.inspectorMinimized = false;
                return;
            }
            ui.followMode=FollowMode::TopGeneration; // ativa G e desativa R
            openTrackedCreature(sim.highestGenerationCreature());
        };
        auto maintainFollowMode = [&](){
            if(sim.selected() || sim.creatures.empty()) return;
            if(ui.followMode==FollowMode::Random)
                openTrackedCreature(sim.randomCreatureDifferent());
            else if(ui.followMode==FollowMode::TopGeneration)
                openTrackedCreature(sim.highestGenerationCreature());
        };
        auto toggleHud = [&](){
            showHud=!showHud;
            if(!showHud){ui.activePopulationField=-1;ui.populationEdit.clear();ui.noteInputActive=false;ui.noteEditTargetId=-1;ui.noteEdit.clear();ui.draggingPlantOpacity=false;ui.draggingMapSize=false;}
        };

        auto saveSession = [&](){
            const auto ioStart=std::chrono::high_resolution_clock::now();
            saveio::SessionMeta meta;
            meta.cameraTarget=cam.target; meta.cameraZoom=cam.zoom; meta.simSpeed=simSpeed; meta.plantOpacity=ui.plantOpacity;
            meta.paused=paused?1:0; meta.showHud=showHud?1:0; meta.showPlants=showPlants?1:0; meta.legacyReservedViewFlag=0; meta.showGraphs=showGraphs?1:0;
            meta.followMode=(std::uint8_t)ui.followMode; meta.inspectorOpen=ui.inspectorOpen?1:0; meta.inspectorMinimized=ui.inspectorMinimized?1:0;
            std::string error;
            const bool worldOk=sim.saveToFile(saveio::defaultWorldSavePath(),meta,error);
            const bool configOk=worldOk ? writeConfigTextFile(saveio::defaultConfigPath(),error) : false;
            if(worldOk && configOk) ui.ioToast="Salvo: mundo + configs + "+std::to_string(sim.notes.size())+" anotacoes";
            else ui.ioToast="ERRO AO SALVAR: "+error;
            ui.ioToastUntil=GetTime()+3.2;
            if(worldOk && configOk) autosaveElapsedReal=0.0;
            sim.perf.saveIo.sample(std::chrono::duration<double,std::milli>(std::chrono::high_resolution_clock::now()-ioStart).count());
        };

        auto saveCheckpoint = [&](){
            const auto ioStart=std::chrono::high_resolution_clock::now();
            try{
                std::filesystem::path dir(saveio::executableDirectory());dir/="saves";std::filesystem::create_directories(dir);
                std::ostringstream stem;stem<<"reforco_mundo_t"<<(long long)std::llround(sim.simTime)<<"_g"<<sim.evo.maxGeneration;
                const std::filesystem::path world=dir/(stem.str()+".dat");
                const std::filesystem::path conf=dir/(stem.str()+".ini");
                saveio::SessionMeta meta;meta.cameraTarget=cam.target;meta.cameraZoom=cam.zoom;meta.simSpeed=simSpeed;meta.plantOpacity=ui.plantOpacity;
                meta.paused=paused?1:0;meta.showHud=showHud?1:0;meta.showPlants=showPlants?1:0;meta.legacyReservedViewFlag=0;meta.showGraphs=showGraphs?1:0;meta.followMode=(std::uint8_t)ui.followMode;meta.inspectorOpen=ui.inspectorOpen?1:0;meta.inspectorMinimized=ui.inspectorMinimized?1:0;
                std::string error;const bool a=sim.saveToFile(world.string(),meta,error);const bool b=a?writeConfigTextFile(conf.string(),error):false;
                ui.ioToast=(a&&b)?"Marco salvo (+ anotacoes) em /saves: "+stem.str():"ERRO NO MARCO: "+error;
            }catch(const std::exception& e){ui.ioToast=std::string("ERRO NO MARCO: ")+e.what();}
            ui.ioToastUntil=GetTime()+3.8;
            sim.perf.saveIo.sample(std::chrono::duration<double,std::milli>(std::chrono::high_resolution_clock::now()-ioStart).count());
        };

        auto loadSession = [&](){
            const auto ioStart=std::chrono::high_resolution_clock::now();
            std::string error;
            saveio::Snapshot snapshot;
            saveio::SessionMeta meta;
            const cfg::RuntimeTuning previousTuning=cfg::tuning;
            ConfigFileLoadReport configReport;

            bool ok=saveio::readSnapshot(saveio::defaultWorldSavePath(),snapshot,error);
            if(ok) ok=readConfigTextFile(saveio::defaultConfigPath(),configReport,error);
            if(ok) ok=sim.applySaveSnapshot(snapshot,error);
            if(ok){
                meta=snapshot.meta;
                cam.target=meta.cameraTarget; cam.zoom=clampf(meta.cameraZoom,0.015f,30.0f); simSpeed=std::max(0.01f,meta.simSpeed);
                ui.plantOpacity=clampf(meta.plantOpacity,0.0f,1.0f); paused=meta.paused!=0; showHud=meta.showHud!=0; showPlants=meta.showPlants!=0;  showGraphs=meta.showGraphs!=0;
                ui.followMode=(meta.followMode<=2)?(FollowMode)meta.followMode:FollowMode::None;
                ui.inspectorOpen=(meta.inspectorOpen!=0)&&sim.selected()!=nullptr; ui.inspectorMinimized=(meta.inspectorMinimized!=0)&&ui.inspectorOpen; ui.inspectorScroll=0.0f;
                ui.activePopulationField=-1;ui.populationEdit.clear();ui.noteInputActive=false;ui.noteEditTargetId=-1;ui.noteEdit.clear();ui.createPopup=false;ui.killPopup=false;ui.configPopup=false;ui.followPopup=false;ui.configSlider=-1;ui.brainLabOpen=false;ui.memoryLabOpen=false;ui.traitEditorOpen=false;ui.traitSliderId=-1;ui.brainSliderId=-1;
                entityDrag.clear();dragging=false;
                ui.ioToast="Carregado: mundo + "+std::to_string(configReport.applied)+" configs";
                if(configReport.unknown>0) ui.ioToast+=" ("+std::to_string(configReport.unknown)+" linhas ignoradas)";
            }else{
                cfg::tuning=previousTuning; cfg::sanitizeTuning();
                ui.ioToast="ERRO AO CARREGAR: "+error;
            }
            ui.ioToastUntil=GetTime()+3.8;
            sim.perf.loadIo.sample(std::chrono::duration<double,std::milli>(std::chrono::high_resolution_clock::now()-ioStart).count());
        };


        auto loadCheckpointFiles = [&](const std::string& worldPath,const std::string& configPath){
            const auto ioStart=std::chrono::high_resolution_clock::now();
            std::string error;saveio::Snapshot snapshot;saveio::SessionMeta meta;
            const cfg::RuntimeTuning previousTuning=cfg::tuning;ConfigFileLoadReport configReport;
            bool ok=saveio::readSnapshot(worldPath,snapshot,error);
            if(ok) ok=readConfigTextFile(configPath,configReport,error);
            if(ok) ok=sim.applySaveSnapshot(snapshot,error);
            if(ok){
                meta=snapshot.meta;cam.target=meta.cameraTarget;cam.zoom=clampf(meta.cameraZoom,0.015f,30.0f);simSpeed=std::max(0.01f,meta.simSpeed);
                ui.plantOpacity=clampf(meta.plantOpacity,0.0f,1.0f);paused=meta.paused!=0;showHud=meta.showHud!=0;showPlants=meta.showPlants!=0;showGraphs=meta.showGraphs!=0;
                ui.followMode=(meta.followMode<=2)?(FollowMode)meta.followMode:FollowMode::None;
                ui.inspectorOpen=(meta.inspectorOpen!=0)&&sim.selected()!=nullptr;ui.inspectorMinimized=(meta.inspectorMinimized!=0)&&ui.inspectorOpen;ui.inspectorScroll=0;
                ui.activePopulationField=-1;ui.populationEdit.clear();ui.noteInputActive=false;ui.noteEditTargetId=-1;ui.noteEdit.clear();ui.createPopup=false;ui.killPopup=false;ui.configPopup=false;ui.systemPopup=false;ui.configSlider=-1;ui.brainLabOpen=false;ui.memoryLabOpen=false;ui.traitEditorOpen=false;ui.traitSliderId=-1;ui.brainSliderId=-1;
                entityDrag.clear();dragging=false;ui.ioToast="Marco carregado: "+std::filesystem::path(worldPath).stem().string()+" + "+std::to_string(configReport.applied)+" configs";
            }else{cfg::tuning=previousTuning;cfg::sanitizeTuning();ui.ioToast="ERRO AO CARREGAR MARCO: "+error;}
            ui.ioToastUntil=GetTime()+4.0;
            sim.perf.loadIo.sample(std::chrono::duration<double,std::milli>(std::chrono::high_resolution_clock::now()-ioStart).count());
        };
        if(ui.checkpointLoadRequested){
            const std::string wp=ui.checkpointWorldPath,cp=ui.checkpointConfigPath;
            ui.checkpointLoadRequested=false;ui.checkpointWorldPath.clear();ui.checkpointConfigPath.clear();
            loadCheckpointFiles(wp,cp);
        }

        auto beginPausedEntityDrag = [&](Vector2 screenPos)->bool {
            if (!paused) return false;
            Vector2 world = GetScreenToWorld2D(screenPos, cam);
            Creature* hc = sim.hitCreature(world, 14.0f, cam.zoom);
            Plant* hp = sim.hitPlant(world, 14.0f, cam.zoom);
            Carcass* hk = sim.hitCarcass(world, 14.0f, cam.zoom);
            Egg* he = sim.hitEgg(world, 14.0f, cam.zoom);

            float bestD = 1e30f;
            DragEntityKind bestKind = DragEntityKind::None;
            void* bestPtr = nullptr;
            Vector2 bestPos{};

            auto consider = [&](DragEntityKind kind, void* ptr, Vector2 pos) {
                if (!ptr) return;
                float d = dist2(world, pos);
                if (d < bestD) { bestD = d; bestKind = kind; bestPtr = ptr; bestPos = pos; }
            };
            if (hc) consider(DragEntityKind::Creature, hc, hc->pos);
            if (hp) consider(DragEntityKind::Plant, hp, hp->pos);
            if (hk) consider(DragEntityKind::Carcass, hk, hk->pos);
            if (he) consider(DragEntityKind::Egg, he, he->pos);

            if (!bestPtr) return false;
            entityDrag.kind = bestKind;
            entityDrag.ptr = bestPtr;
            entityDrag.grabOffset = sub(bestPos, world);
            entityDrag.pressScreen = screenPos;
            entityDrag.moved = false;
            return true;
        };

        auto updatePausedEntityDrag = [&]() {
            if (!paused || !entityDrag.active()) return;
            Vector2 world = add(GetScreenToWorld2D(GetMousePosition(), cam), entityDrag.grabOffset);
            Vector2 screenDelta = sub(GetMousePosition(), entityDrag.pressScreen);
            if (length2(screenDelta) > 9.0f) entityDrag.moved = true;

            if (entityDrag.kind == DragEntityKind::Creature) {
                auto* c = static_cast<Creature*>(entityDrag.ptr);
                c->pos = world;
                c->wrap();
                c->speed = 0.0f;
                c->pushVelocity = {};
                sim.creatureGrid.update(c);
            } else if (entityDrag.kind == DragEntityKind::Plant) {
                auto* p = static_cast<Plant*>(entityDrag.ptr);
                p->pos = world;
                constrainCircleToWorld(p->pos,p->radius());
                p->velocity = {};
                p->visualOffset = {};
                sim.plantGrid.update(p);
            } else if (entityDrag.kind == DragEntityKind::Carcass) {
                auto* k = static_cast<Carcass*>(entityDrag.ptr);
                k->pos = world;
                constrainCircleToWorld(k->pos,k->radius());
                k->velocity = {};
                sim.carcassGrid.update(k);
            } else if (entityDrag.kind == DragEntityKind::Egg) {
                auto* e = static_cast<Egg*>(entityDrag.ptr);
                e->pos = world;
                constrainCircleToWorld(e->pos,e->radius);
                e->velocity = {};
                sim.eggGrid.update(e);
            }
        };

        const bool uiModal = fullScreenModalOpen(ui);

        // Teclas agora usam eventos Win32 travados por frame, entao pressionamentos
        // rapidos nao se perdem mesmo se DOWN e UP chegarem juntos na fila.
        if(!editingText && !uiModal && IsKeyPressed(KEY_R)) followRandom();
        if(!editingText && !uiModal && IsKeyPressed('G')) {
            if(sim.manualControlActive()) sim.toggleManualGrab();
            else followTopGeneration();
        }
        if(!editingText && !uiModal && IsKeyPressed(KEY_H)) toggleHud();
        if(!editingText && !uiModal && IsKeyPressed(KEY_P)) showPlants=!showPlants;
        if(!editingText && !uiModal && IsKeyPressed(KEY_F5)) saveSession();
        if(!editingText && !uiModal && IsKeyPressed(KEY_F9)) loadSession();
        if(!editingText && !uiModal && IsKeyPressed(KEY_F11)) ToggleFullscreen();
        if(!editingText && !uiModal && IsKeyPressed(KEY_X)){
            ui.followMode=FollowMode::None;
            ui.brainLabOpen=false;
            ui.memoryLabOpen=false;
            
            ui.visionLabOpen=false;
            ui.traitEditorOpen=false;
            ui.traitSliderId=-1;
            ui.brainSliderId=-1;
            sim.closeWatch();
            ui.inspectorOpen=false;
            ui.inspectorMinimized=false;
        }
        if(!editingText && !uiModal && IsKeyPressed(KEY_M)&&sim.watch.creatureId>=0){ui.inspectorMinimized=!ui.inspectorMinimized;ui.inspectorOpen=!ui.inspectorMinimized;}
        if(!editingText && !uiModal && IsKeyPressed(KEY_SPACE) && !sim.manualControlActive()) {
            paused=!paused;
            if(!paused) entityDrag.clear();
        }
        // Estado continuo do teclado para o bicho controlado. Quando ha modal/campo de
        // texto, zeramos motores para as teclas da interface nao moverem o animal.
        const bool manualKeys=!editingText && !uiModal && sim.manualControlActive();
        // Duas familias de teclas para notebooks/layouts diferentes. As setas usam
        // VK_* e o polling tem fallback GetAsyncKeyState, entao nao dependemos apenas
        // da repeticao de WM_KEYDOWN da janela.
        sim.setManualInput(
            manualKeys&&(IsKeyDown('W')||IsKeyDown(KEY_UP)),
            manualKeys&&(IsKeyDown('S')||IsKeyDown(KEY_DOWN)),
            manualKeys&&(IsKeyDown('A')||IsKeyDown(KEY_LEFT)),
            manualKeys&&(IsKeyDown('D')||IsKeyDown(KEY_RIGHT)),
            manualKeys&&IsKeyDown('Q'), manualKeys&&IsKeyDown('E'),
            manualKeys&&(IsKeyDown(KEY_SPACE)||IsKeyDown(KEY_F)));
        if(!editingPopulation && !uiModal){
            if(IsKeyPressed(KEY_ONE)) simSpeed=1;
            if(IsKeyPressed(KEY_TWO)) simSpeed=2;
            if(IsKeyPressed(KEY_FIVE)) simSpeed=5;
            if(IsKeyPressed(KEY_ZERO)) simSpeed=10;
        }

        // Os mesmos comandos tambem existem como botoes. O botao de HUD nunca some.
        const bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        const RectF opacitySlider = plantOpacitySliderRect(uiTop);
        const bool overOpacitySlider = showHud && !uiModal && pointIn(opacitySlider,mouse);
        if(showHud && !uiModal && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && overOpacitySlider) ui.draggingPlantOpacity=true;
        if(ui.draggingPlantOpacity && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            ui.plantOpacity=clampf((mouse.x-opacitySlider.x)/opacitySlider.width,0.0f,1.0f);
        if(ui.draggingPlantOpacity && mouseReleased) {
            ui.plantOpacity=clampf((mouse.x-opacitySlider.x)/opacitySlider.width,0.0f,1.0f);
            ui.draggingPlantOpacity=false;
        }
        const RectF mapSlider = mapSizeSliderRect(uiTop);
        const bool overMapSlider = showHud && !uiModal && pointIn(mapSlider,mouse);
        if(showHud && !uiModal && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && overMapSlider) {
            ui.draggingMapSize=true;
            entityDrag.clear();
        }
        if(ui.draggingMapSize && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            const float t=clampf((mouse.x-mapSlider.x)/mapSlider.width,0.0f,1.0f);
            const float wanted=mapSliderToSize(t);
            if(std::fabs(wanted-cfg::WORLD_W)>=0.5f) {
                sim.resizeWorld(wanted);
                ui.createPopup=false;
            }
        }
        if(ui.draggingMapSize && mouseReleased) {
            const float t=clampf((mouse.x-mapSlider.x)/mapSlider.width,0.0f,1.0f);
            sim.resizeWorld(mapSliderToSize(t));
            ui.draggingMapSize=false;
        }
        const RectF liveGraphToggle=liveGraphsToggleRect(showGraphs,sh,ui.populationMinimized,uiTop);
        const RectF liveGraphPanel=liveGraphsPanelRect(sh,ui.populationMinimized,uiTop);
        const bool overLiveGraphs=!uiModal && showHud && (showGraphs?pointIn(liveGraphPanel,mouse):pointIn(liveGraphToggle,mouse));
        const RectF notesHitRect=ui.notesOpen?notesPanelRect(sw,sh,uiTop):notesCollapsedRect(sw,uiTop);
        const bool overNotes=!uiModal && showHud && pointIn(notesHitRect,mouse);
        if(!uiModal && showHud && mouseReleased && pointIn(liveGraphToggle,mouse)) {
            showGraphs=!showGraphs;
        } else if(!uiModal && mouseReleased && pointIn(controls.fullscreen,mouse)) {
            ToggleFullscreen();
        } else if(!uiModal && mouseReleased && pointIn(controls.hud,mouse)) {
            toggleHud();
        } else if(!uiModal && showHud && mouseReleased) {
            if(pointIn(controls.follow,mouse)) { ui.followPopup=!ui.followPopup; ui.killPopup=false; ui.createPopup=false; }
            else if(pointIn(controls.plants,mouse)) showPlants=!showPlants;
            else if(pointIn(controls.pause,mouse)) {
                paused=!paused;
                if(!paused) entityDrag.clear();
            }
            else if(pointIn(controls.kill,mouse)) { ui.killPopup=!ui.killPopup; ui.followPopup=false; ui.createPopup=false; ui.configPopup=false; ui.systemPopup=false; ui.configSlider=-1; }
            else if(pointIn(controls.system,mouse)) { ui.systemPopup=!ui.systemPopup; ui.followPopup=false; ui.configPopup=false; ui.killPopup=false; ui.createPopup=false; ui.brainLabOpen=false; ui.memoryLabOpen=false;  ui.traitEditorOpen=false; ui.traitSliderId=-1; }
            else if(pointIn(controls.reinforcement,mouse)) {
                ui.configPopup=true;ui.followPopup=false;
                ui.configCategory=(int)ConfigCategory::Reforco;
                ui.configScroll=0.0f;ui.configSlider=-1;
                ui.systemPopup=false;ui.killPopup=false;ui.createPopup=false;ui.brainLabOpen=false;ui.memoryLabOpen=false;ui.traitEditorOpen=false;ui.traitSliderId=-1;ui.brainSliderId=-1;
            }
            else if(pointIn(controls.config,mouse)) {
                ui.configPopup=!ui.configPopup;ui.followPopup=false;
                ui.systemPopup=false;
                ui.killPopup=false;
                ui.createPopup=false;
                ui.brainLabOpen=false;
                
                ui.traitEditorOpen=false;
                ui.traitSliderId=-1;
                ui.brainSliderId=-1;
                ui.configSlider=-1;
            }
            else if(pointIn(controls.checkpoint,mouse)) saveCheckpoint();
            else if(pointIn(controls.save,mouse)) saveSession();
            else if(pointIn(controls.load,mouse)) loadSession();
            else if(pointIn(controls.restart,mouse)) {
                sim.resetWorld();
                ui.inspectorOpen=false; ui.inspectorMinimized=false; ui.inspectorScroll=0;
                ui.brainLabOpen=false; ui.memoryLabOpen=false;  ui.traitEditorOpen=false; ui.traitSliderId=-1; ui.brainSliderId=-1;
                ui.killPopup=false; ui.createPopup=false; ui.configPopup=false; ui.systemPopup=false; ui.followPopup=false; ui.configSlider=-1;
                ui.noteInputActive=false; ui.noteEditTargetId=-1; ui.noteEdit.clear();
            }
        }

        bool overInspector=!uiModal && showHud && ui.inspectorOpen && mouse.x>sw-400 && mouse.y>uiTop;
        bool overPopulation=!uiModal && showHud && mouse.x<310 && mouse.y>sh-205;
        bool overSpeed=!uiModal && showHud && mouse.y>sh-65 && mouse.x>sw*0.5f-205 && mouse.x<sw*0.5f+205;
        bool overTopControls=!uiModal && topControlHit(controls,mouse,showHud);
        const RectF followPopupArea=followPopupRect(controls,sw,sh);
        const bool overFollowPopup=!uiModal && showHud && ui.followPopup && pointIn(followPopupArea,mouse);
        bool overPopup = ui.createPopup || ui.killPopup || ui.configPopup || ui.systemPopup || ui.brainLabOpen || ui.memoryLabOpen || ui.followPopup;
        bool overUI=uiModal || overPopup || overTopControls || overFollowPopup || overOpacitySlider || overMapSlider || overLiveGraphs || overNotes ||
                    (showHud&&(overInspector||overPopulation||overSpeed));

        // Botao direito abre um menu de criacao na posicao do mundo.
        if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && !uiModal && !ui.killPopup && !ui.followPopup && !overTopControls && !overInspector && !overPopulation && !overSpeed && !overOpacitySlider && !overMapSlider && !overLiveGraphs && !overNotes){
            Vector2 world=GetScreenToWorld2D(mouse,cam);
            ui.createPopup=true; ui.killPopup=false;
            ui.createPopupPos=mouse; ui.createWorldPos=world;
            ui.activePopulationField=-1; ui.populationEdit.clear();
        }

        float wheel=GetMouseWheelMove();
        if(wheel!=0 && !overUI){ Vector2 before=GetScreenToWorld2D(mouse,cam); cam.zoom=clampf(cam.zoom*(1.0f+wheel*0.12f),0.015f,30.0f); Vector2 after=GetScreenToWorld2D(mouse,cam); cam.target=add(cam.target,sub(before,after)); }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !overUI) {
            if (paused && beginPausedEntityDrag(mouse)) {
                dragging = false;
            } else {
                dragging=true;
                pressPos=mouse;
                pressTime=GetTime();
            }
        }

        if (paused && entityDrag.active() && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            updatePausedEntityDrag();
            // No pause o mundo nao avanca, mas a percepcao continua reagindo
            // ao reposicionamento manual das entidades em tempo real.
            sim.updateSensors();
        }

        if (paused && entityDrag.active() && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // Um clique curto em um bicho pausado ainda o seleciona;
            // ao mover, a acao e tratada somente como arraste.
            if (!entityDrag.moved && entityDrag.kind == DragEntityKind::Creature) {
                auto* c = static_cast<Creature*>(entityDrag.ptr);
                ui.followMode=FollowMode::None;
                sim.startWatch(c);
                ui.inspectorOpen=true;
                ui.inspectorMinimized=false;
                ui.inspectorScroll=0;
            }
            entityDrag.clear();
        } else {
            if(dragging&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)&&!sim.selected()){
                Vector2 d=GetMouseDelta();
                cam.target.x-=d.x/cam.zoom;
                cam.target.y-=d.y/cam.zoom;
            }
            if(dragging&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                double duration=GetTime()-pressTime;
                Vector2 delta=sub(mouse,pressPos);
                float moved=std::sqrt(length2(delta));
                if(duration<=0.32 && moved<=10.0f){
                    Vector2 world=GetScreenToWorld2D(mouse,cam);
                    Creature* hit=sim.hitCreature(world,18.0f,cam.zoom);
                    if(hit){
                        ui.followMode=FollowMode::None;
                        sim.startWatch(hit);
                        ui.inspectorOpen=true;
                        ui.inspectorMinimized=false;
                        ui.inspectorScroll=0;
                    } else if (!paused) {
                        sim.addCreature(world);
                    }
                }
                dragging=false;
            }
        }

        float realDt=std::min(GetFrameTime(),0.25f);
        // Ao pausar pela tela de desempenho, preservamos a ultima fotografia ativa
        // do profiler em vez de zerar/reordenar a tabela enquanto o usuario analisa.
        if(!paused){
            sim.perf.substepsLastFrame=0;
            sim.perf.realDtLastFrame=realDt;
            sim.perf.simSpeedLastFrame=simSpeed;
            sim.perf.simRequestedDtLastFrame=double(realDt)*double(simSpeed);
            sim.perf.simProcessedDtLastFrame=0.0;
            sim.perf.simDroppedDtLastFrame=0.0;
            sim.perf.uncappedSubstepsLastFrame=0;
            sim.perf.preventedSubstepsLastFrame=0;
            if(cfg::tuning.autosaveIntervalSeconds>0.0f){
                autosaveElapsedReal+=realDt;
                if(autosaveElapsedReal>=cfg::tuning.autosaveIntervalSeconds){ saveSession(); autosaveElapsedReal=0.0; }
            }else autosaveElapsedReal=0.0;

            const auto simFrameStart=FrameClock::now();
            const float safeStep=std::max(0.000001f,cfg::tuning.maxSubstep);
            // Anti-espiral: se o frame anterior demorou mais que 1/60 s, nao tentamos
            // recuperar todo esse atraso dentro do proximo frame. A simulacao passa a
            // rodar em best-effort em vez de transformar lag em ainda mais subpassos.
            constexpr float PERF_TARGET_REAL_DT=1.0f/60.0f;
            const float stableRealDt=std::min(realDt,PERF_TARGET_REAL_DT);
            float remaining=stableRealDt*simSpeed;
            sim.perf.simProcessedDtLastFrame=remaining;
            sim.perf.simDroppedDtLastFrame=std::max(0.0,double(realDt*simSpeed-remaining));
            sim.perf.uncappedSubstepsLastFrame=(std::size_t)std::ceil(std::max(0.0,double(realDt*simSpeed))/double(safeStep));
            while(remaining>0.000001f){
                const float step=std::min(safeStep,remaining);
                sim.update(step);
                remaining-=step;
                ++sim.perf.substepsLastFrame;
            }
            sim.perf.preventedSubstepsLastFrame=sim.perf.uncappedSubstepsLastFrame>sim.perf.substepsLastFrame
                ?sim.perf.uncappedSubstepsLastFrame-sim.perf.substepsLastFrame:0;
            sim.perf.simFrame.sample(std::chrono::duration<double,std::milli>(FrameClock::now()-simFrameStart).count());
        }
        maintainFollowMode();

        const double renderStart = GetTime();

        BeginDrawing();
        const auto renderWorldStart=FrameClock::now();
        if(ui.visionLabOpen && !sim.selected()) ui.visionLabOpen=false;
        const WorldRenderStats visible=drawWorld(sim,cam,sw,sh,showPlants,ui.plantOpacity);
        sim.perf.renderWorld.sample(std::chrono::duration<double,std::milli>(FrameClock::now()-renderWorldStart).count());
        const auto renderUiStart=FrameClock::now();

        // Controles/HUD de base nao sao desenhados por baixo de modais grandes.
        // Isso evita sobreposicao visual e tambem reduz trabalho ao abrir o cerebro.
        const bool renderModal = fullScreenModalOpen(ui);

        // Controles de tela. Quando o HUD esta oculto, somente o botao para
        // restaura-lo permanece visivel. Em modal, ate essa barra fica escondida.
        if(showHud && !renderModal){
            button(controls.plants,showPlants?"P: Plantas ON":"P: Plantas OFF",showPlants);
            button(controls.pause,paused?"Continuar":"Pausar",paused);
            button(controls.kill,"Matar X",ui.killPopup);
            button(controls.system,"Sistema",ui.systemPopup);
            button(controls.reinforcement,"Reforco",ui.configPopup && ui.configCategory==(int)ConfigCategory::Reforco);
            button(controls.config,"Config",ui.configPopup && ui.configCategory!=(int)ConfigCategory::Reforco);
            button(controls.checkpoint,"Marco");
            button(controls.save,"Salvar");
            button(controls.load,"Carregar");
            button(controls.restart,"Reiniciar");
            button(controls.follow,"Seguir",ui.followPopup || ui.followMode!=FollowMode::None);
        }
        if(!renderModal){
            button(controls.fullscreen,IsWindowFullscreen()?"Janela":"Fullscreen",IsWindowFullscreen());
            button(controls.hud,showHud?"H: Ocultar HUD":"Mostrar HUD",!showHud);
        }
        if(showHud && !renderModal) drawFollowPopup(sim,ui,controls,sw,sh);

        if(showHud && !renderModal){
            // HUD compacto no estilo da versao web.
            RectF hud{8,uiTop,360,136};
            DrawRectangleRounded(hud,0.06f,5,Color{10,10,10,235});
            DrawRectangleRoundedLines(hud,0.06f,5,1.0f,Color{72,72,72,190});

            DrawText(TextFormat("FPS %d",GetFPS()),18,(int)hud.y+9,16,RAYWHITE);
            {
                const std::string clockText="Tempo sim "+globalSimulationClock(sim.simTime)+(paused?"  [PAUSADO]":"");
                DrawText(clockText.c_str(),92,(int)hud.y+10,12,paused?Color{255,190,100,255}:Color{150,205,255,255});
            }
            DrawText(TextFormat("Bichos %d | vis %d",(int)sim.creatures.size(),visible.creatures),18,(int)hud.y+34,12,Color{220,220,220,255});
            DrawText(TextFormat("Plantas %d | visual %s",(int)sim.plants.size(),showPlants?"ON":"OFF"),176,(int)hud.y+34,12,Color{220,220,220,255});
            DrawText(TextFormat("Carne %d | vis %d",(int)sim.carcasses.size(),visible.carcasses),18,(int)hud.y+51,11,Color{190,190,190,255});
            DrawText(TextFormat("Ovos %d | vis %d",(int)sim.eggs.size(),visible.eggs),176,(int)hud.y+51,11,Color{190,190,190,255});

            Creature* hudCreature=sim.selected();
            if(hudCreature){
                DrawText(
                    TextFormat("Cerebro %d in | %d H | %d cam | %d lig | %d loops",
                        cfg::INPUT_COUNT,
                        hudCreature->genome.brain.hiddenCount,
                        hudCreature->compiled.usedLayers,
                        hudCreature->compiled.activeConnections,
                        hudCreature->compiled.recurrentConnections),
                    18,(int)hud.y+69,11,Color{190,190,190,255});
            }else{
                DrawText(TextFormat("Cerebro evolutivo: catalogo %d inputs | genes aparecem por mutacao",cfg::INPUT_COUNT),18,(int)hud.y+69,11,Color{190,190,190,255});
            }

            DrawText(TextFormat("Sim %.3f ms | sense %.2f | brain %.2f | workers %u",sim.lastUpdateMs,sim.perf.sensorsMs,sim.perf.brainMs,(unsigned)sim.brainExecutor.workerCount()),18,(int)hud.y+89,10,Color{180,180,180,255});

            const double renderMsBeforeUi = (GetTime()-renderStart)*1000.0;
            DrawText(TextFormat("Render CPU %.2f ms | GPU %u draws / %u verts",renderMsBeforeUi,GetRenderDrawCalls(),GetRenderVertexCount()),18,(int)hud.y+107,10,Color{155,155,155,255});
            DrawText(cfg::tuning.reinforcementEnabled>0.5f?"RL ON":"RL OFF",294,(int)hud.y+122,9,
                cfg::tuning.reinforcementEnabled>0.5f?Color{105,220,150,255}:Color{210,120,120,255});
            drawPlantOpacitySlider(ui,uiTop);
            drawMapSizeSlider(sim,uiTop);
            drawLiveGraphsDashboard(sim,ui,showGraphs,sh,uiTop);
            drawNotesPanel(sim,ui,sw,sh,uiTop);

            drawPopulationUI(sim,ui,sh);
            drawSpeedUI(simSpeed,paused,sw,sh);
            drawInspector(sim,ui,sw,sh,paused);
        }

        if(!renderModal && !ui.ioToast.empty() && GetTime()<ui.ioToastUntil){
            const int fs=13; const int tw=MeasureText(ui.ioToast.c_str(),fs);
            RectF tr{sw*0.5f-tw*0.5f-12.0f,uiTop,(float)tw+24.0f,28.0f};
            DrawRectangleRounded(tr,0.25f,6,Color{12,12,12,238});
            DrawRectangleRoundedLines(tr,0.25f,6,1.0f,Color{115,150,210,220});
            DrawText(ui.ioToast.c_str(),(int)(tr.x+12),(int)(tr.y+7),fs,RAYWHITE);
        }

        // Popups pequenos nao ficam por baixo de modais grandes.
        if(!renderModal) {
            drawCreatePopup(sim,ui,sw,sh);
            if(showHud) drawKillPopup(sim,ui,sw,sh);
        }

        // Modais completos sao mutuamente exclusivos e sempre a ultima camada.
        if(showHud && ui.configPopup) {
            DrawRectangle(0,0,sw,sh,Color{0,0,0,190});
            drawConfigPopup(sim,ui,sw,sh);
        }
        if(showHud && ui.systemPopup) {
            DrawRectangle(0,0,sw,sh,Color{0,0,0,190});
            drawSystemPopup(sim,ui,sw,sh,paused);
        }
        drawCreatureTraitEditor(sim,ui,sw,sh);
        drawMemoryLab(sim,ui,sw,sh);
        drawBrainLab(sim,ui,sw,sh);
        drawVisionLab(sim,ui,sw,sh);
        sim.perf.renderUi.sample(std::chrono::duration<double,std::milli>(FrameClock::now()-renderUiStart).count());
        const auto submitStart=FrameClock::now();
        EndDrawing();
        sim.perf.renderSubmit.sample(std::chrono::duration<double,std::milli>(FrameClock::now()-submitStart).count());
        sim.perf.frameTotal.sample(std::chrono::duration<double,std::milli>(FrameClock::now()-frameWorkStart).count());
    }

    unloadGameSpriteAssets();
    CloseWindow();
    return 0;
}
