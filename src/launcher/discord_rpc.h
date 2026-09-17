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

        auto safeWrite = [](HANDLE pipe, const void* data, DWORD size) -> bool {
            DWORD written = 0;
            return WriteFile(pipe, data, size, &written, NULL) && (written == size);
        };

        // 1. Stuur Discord Handshake (Opcode 0)
        // TODO: Vervang client_id door het officiële KytyPS5 Discord App ID zodra beschikbaar.
        std::string handshakeJson = "{\"v\":1,\"client_id\":\"123456789012345678\"}";
        uint32_t handshakeOpcode = 0;
        uint32_t handshakeLength = static_cast<uint32_t>(handshakeJson.size());

        if (!safeWrite(hPipe, &handshakeOpcode, sizeof(handshakeOpcode)) ||
            !safeWrite(hPipe, &handshakeLength, sizeof(handshakeLength)) ||
            !safeWrite(hPipe, handshakeJson.c_str(), handshakeLength)) {
            CloseHandle(hPipe);
            return;
        }

        // 2. Stuur Activity Frame met Process ID (Opcode 1)
        DWORD pid = GetCurrentProcessId();
        std::string activityJson = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + std::to_string(pid) +
                                    ",\"activity\":{\"details\":\"" + gameTitle + "\",\"state\":\"" + titleId + "\"}},\"nonce\":\"1\"}";
        uint32_t frameOpcode = 1;
        uint32_t frameLength = static_cast<uint32_t>(activityJson.size());

        if (!safeWrite(hPipe, &frameOpcode, sizeof(frameOpcode)) ||
            !safeWrite(hPipe, &frameLength, sizeof(frameLength)) ||
            !safeWrite(hPipe, activityJson.c_str(), frameLength)) {
            CloseHandle(hPipe);
            return;
        }

        CloseHandle(hPipe);
#endif
    }
};
