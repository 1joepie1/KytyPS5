#pragma once

#include <cstdint>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

class DiscordRPC {
public:
    static void UpdatePresence([[maybe_unused]] const std::string& gameTitle, [[maybe_unused]] const std::string& titleId) {
#ifdef _WIN32
        HANDLE hPipe = INVALID_HANDLE_VALUE;

        // Probeer poorten discord-ipc-0 t/m discord-ipc-9
        for (int i = 0; i < 10; ++i) {
            std::string pipePath = "\\\\.\\pipe\\discord-ipc-" + std::to_string(i);
            hPipe = CreateFileA(pipePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (hPipe != INVALID_HANDLE_VALUE) {
                break;
            }
        }

        if (hPipe == INVALID_HANDLE_VALUE) {
            return;
        }

        DWORD bytesWritten = 0;

        // 1. Stuur Discord Handshake (Opcode 0)
        // TODO: Vervang client_id door het officiële KytyPS5 Discord App ID zodra aangemaakt.
        std::string handshakeJson = "{\"v\":1,\"client_id\":\"123456789012345678\"}";
        uint32_t handshakeOpcode = 0;
        uint32_t handshakeLength = static_cast<uint32_t>(handshakeJson.size());

        WriteFile(hPipe, &handshakeOpcode, sizeof(handshakeOpcode), &bytesWritten, NULL);
        WriteFile(hPipe, &handshakeLength, sizeof(handshakeLength), &bytesWritten, NULL);
        WriteFile(hPipe, handshakeJson.c_str(), handshakeLength, &bytesWritten, NULL);

        // 2. Stuur Activity Frame met Process ID (Opcode 1)
        DWORD pid = GetCurrentProcessId();
        std::string activityJson = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + std::to_string(pid) +
                                    ",\"activity\":{\"details\":\"" + gameTitle + "\",\"state\":\"" + titleId + "\"}},\"nonce\":\"1\"}";
        uint32_t frameOpcode = 1;
        uint32_t frameLength = static_cast<uint32_t>(activityJson.size());

        WriteFile(hPipe, &frameOpcode, sizeof(frameOpcode), &bytesWritten, NULL);
        WriteFile(hPipe, &frameLength, sizeof(frameLength), &bytesWritten, NULL);
        WriteFile(hPipe, activityJson.c_str(), frameLength, &bytesWritten, NULL);

        CloseHandle(hPipe);
#endif
    }
};
