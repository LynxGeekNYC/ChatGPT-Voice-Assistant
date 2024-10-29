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
