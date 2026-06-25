#pragma once

class GameManager {
public:
    GameManager();
    ~GameManager();

    bool Initialize();
    void Run();
    void Shutdown();
};