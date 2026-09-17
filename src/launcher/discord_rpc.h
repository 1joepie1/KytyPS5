#pragma once
#include <windows.h>
#include <string>

class DiscordRPC {
public:
    static void UpdatePresence(const std::string& gameTitle, const std::string& titleId) {
        HANDLE hPipe = CreateFileA("\\\\.\\pipe\\discord-ipc-0", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe == INVALID_HANDLE_VALUE) return;

        std::string json = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"activity\":{\"details\":\"" + gameTitle + "\",\"state\":\"" + titleId + "\"}},\"nonce\":\"1\"}";
        uint32_t opcode = 1;
        uint32_t len = static_cast<uint32_t>(json.length());
        DWORD written = 0;

        WriteFile(hPipe, &opcode, 4, &written, NULL);
        WriteFile(hPipe, &len, 4, &written, NULL);
        WriteFile(hPipe, json.c_str(), len, &written, NULL);

        CloseHandle(hPipe);
    }
};
