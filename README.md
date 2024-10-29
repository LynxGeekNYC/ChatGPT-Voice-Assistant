# ChatGPT-Voice-Assistant
ChatGPT Voice Assistant that uses ChatGPT and Google TTS API. Launching apps (Notepad, Calculator, etc.), Web searches System commands, (Restart, Shutdown), ChatGPT queries with TTS playback.

# How to compile
g++ VoiceAssistant.cpp -o VoiceAssistant -lcurl -lole32 -loleaut32

# How to add more commands:

Windows API (WinAPI) Documentation
To create commands that control system functions such as opening applications, shutting down the system, or searching files, the Windows API is essential.

Microsoft Docs – Windows API Reference
https://learn.microsoft.com/en-us/windows/win32/api/

This is the official documentation for all the WinAPI functions. Look for functions like:
ShellExecute() – To open files, folders, and URLs
FindFirstFile() and FindNextFile() – To search files
CreateProcess() – To launch external applications
ExitWindowsEx() – For shutdown and restart commands

Key Topics:

Shell and System Commands:
https://learn.microsoft.com/en-us/windows/win32/shell/shell-functions

# Was this script helpful to you? Please donate:

I put a lot of work into these scripts so please donate if you can. Even $1 helps!

PayPal: alex@alexandermirvis.com

CashApp / Venmo: LynxGeekNYC

BitCoin: bc1q8sthd96c7chhq5kr3u80xrxs26jna9d8c0mjh7
