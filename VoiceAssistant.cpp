#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include <sapi.h>  // Speech API for voice recognition
#include <string>
#include <curl/curl.h>  // libcurl for API requests
#include <nlohmann/json.hpp>  // JSON parsing (install via vcpkg)
#include <fstream>
#include <thread>

// API Keys (replace with your actual keys)
const std::string CHATGPT_API_KEY = "your_chatgpt_api_key";
const std::string GOOGLE_TTS_API_KEY = "your_google_tts_api_key";

// Function prototypes
void MinimizeToTray();
void ShowNotification(const std::string& message);
std::string QueryChatGPT(const std::string& query);
void SpeakResponse(const std::string& text);
void VoiceCommandListener();
void ExecuteCommand(const std::string& command);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
std::string GetGoogleTTS(const std::string& text);

// System Tray Variables
NOTIFYICONDATA nid;
HWND hwnd;
HINSTANCE hInstance;
bool isRunning = true;

int main() {
    std::thread voiceThread(VoiceCommandListener);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "VoiceAssistant";

    RegisterClass(&wc);
    hwnd = CreateWindow(wc.lpszClassName, "VoiceAssistant", WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, wc.hInstance, NULL);

    MinimizeToTray();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    isRunning = false;
    voiceThread.join();
    return 0;
}

void MinimizeToTray() {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_APP;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy(nid.szTip, "VoiceAssistant Running");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void ShowNotification(const std::string& message) {
    nid.uFlags = NIF_INFO;
    strcpy(nid.szInfo, message.c_str());
    strcpy(nid.szInfoTitle, "VoiceAssistant");
    nid.dwInfoFlags = NIIF_INFO;

    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

std::string QueryChatGPT(const std::string& query) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/completions");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // This variable controls the JSON data that is sent and received from OpenAI. Default is 100 tokens but if your script requires more token usage, change it here. 
        std::string jsonData = R"({
            "model": "text-davinci-003",
            "prompt": ")" + query + R"(",
            "max_tokens": 100 
        })";

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + CHATGPT_API_KEY).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        auto writeCallback = [](char* ptr, size_t size, size_t nmemb, std::string* data) {
            data->append(ptr, size * nmemb);
            return size * nmemb;
        };
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    nlohmann::json jsonResponse = nlohmann::json::parse(response);
    return jsonResponse["choices"][0]["text"].get<std::string>();
}

void SpeakResponse(const std::string& text) {
    std::string audioFile = GetGoogleTTS(text);
    if (!audioFile.empty()) {
        std::string command = "start " + audioFile;
        system(command.c_str());
    } else {
        std::cerr << "Failed to generate speech." << std::endl;
    }
}

std::string GetGoogleTTS(const std::string& text) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://texttospeech.googleapis.com/v1/text:synthesize");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        std::string jsonData = R"({
            "input": {"text": ")" + text + R"("},
            "voice": {"languageCode": "en-US", "name": "en-US-Wavenet-D"},
            "audioConfig": {"audioEncoding": "LINEAR16"}
        })";

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + GOOGLE_TTS_API_KEY).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        auto writeCallback = [](char* ptr, size_t size, size_t nmemb, std::string* data) {
            data->append(ptr, size * nmemb);
            return size * nmemb;
        };
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    nlohmann::json jsonResponse = nlohmann::json::parse(response);
    std::string audioContent = jsonResponse["audioContent"].get<std::string>();

    std::ofstream outFile("response.wav", std::ios::binary);
    outFile << std::string(reinterpret_cast<const char*>(audioContent.data()), audioContent.size());
    outFile.close();

    return "response.wav";
}

// Commands you can incorporate into the system

void ExecuteCommand(const std::string& command) {
    if (command == "open notepad") {
        system("start notepad");
    } else if (command == "open calculator") {
        system("start calc");
    } else if (command.find("search for") != std::string::npos) {
        std::string query = command.substr(11);
        system(("start chrome \"https://www.google.com/search?q=" + query + "\"").c_str());
    } else if (command == "restart computer") {
        system("shutdown /r /t 0");
    } else if (command == "shutdown computer") {
        system("shutdown /s /t 0");
    } else {
        std::string response = QueryChatGPT(command);
        SpeakResponse(response);
        ShowNotification(response);
    }
}

void VoiceCommandListener() {
    ISpRecognizer* recognizer;
    ISpRecoContext* context;
    ISpRecoGrammar* grammar;

    CoInitialize(NULL);
    CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, IID_ISpRecognizer, (void**)&recognizer);
    recognizer->CreateRecoContext(&context);
    context->CreateGrammar(1, &grammar);
    grammar->LoadDictation(NULL, SPLO_STATIC);
    grammar->SetDictationState(SPRS_ACTIVE);

    while (isRunning) {
        SPEVENT event;
        ULONG fetched;
        context->GetEvents(1, &event, &fetched);
        if (event.eEventId == SPEI_RECOGNITION) {
            ISpPhrase* phrase = event.RecoResult;
            SPPHRASE* pPhrase;
            phrase->GetPhrase(&pPhrase);
            std::wstring command = pPhrase->pProperties->pszValue;
            ExecuteCommand(std::string(command.begin(), command.end()));
        }
    }

    CoUninitialize();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_APP && lParam == WM_LBUTTONDOWN) {
        ShowNotification("VoiceAssistant is running.");
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
