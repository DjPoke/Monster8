#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include "sdl_config.h"
#include "M8.hpp"

// Pointeurs globaux pour manette/joystick
static SDL_GameController* gController = nullptr;
static SDL_Joystick* gJoystick = nullptr;

// Constantes pour la fenêtre
const int WINDOW_WIDTH = 320;
const int WINDOW_HEIGHT = 200;
const int PIXEL_SIZE = 3;  // Taille d'un pixel de l'émulateur à l'écran

// Fonction pour gérer les événements SDL (clavier + manette/joystick)
void handleInput(Monster8 &m8, bool &quit, bool &spacePressed, bool &enterPressed, int &menuMove) {
    SDL_Event event;

    // État persistant du joystick (ne pas réinitialiser à chaque frame)
    static uint8_t joyState = 0; // bits: 0x01 Up, 0x02 Down, 0x04 Left, 0x08 Right, 0x10 A, 0x20 B, 0x40 Select, 0x80 Start
    const int DEADZONE = 8000;   // zone morte pour les axes analogiques

    auto setBit = [&](uint8_t mask) { joyState |= mask; };
    auto clearBit = [&](uint8_t mask) { joyState &= static_cast<uint8_t>(~mask); };

    spacePressed = false;
    enterPressed = false;
    menuMove = 0;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                return;

            // ===== Clavier =====
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_SPACE:
                        spacePressed = true;
                        setBit(0x40);  // Select
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        enterPressed = true;
                        setBit(0x80);  // Start
                        break;
                    case SDLK_UP:
                        setBit(0x01);
                        menuMove = -1;
                        break;
                    case SDLK_DOWN:
                        setBit(0x02);
                        menuMove = 1;
                        break;
                    case SDLK_LEFT:
                        setBit(0x04);
                        break;
                    case SDLK_RIGHT:
                        setBit(0x08);
                        break;
                    case SDLK_x:
                        setBit(0x10);  // A
                        break;
                    case SDLK_c:
                        setBit(0x20);  // B
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:    clearBit(0x01); break;
                    case SDLK_DOWN:  clearBit(0x02); break;
                    case SDLK_LEFT:  clearBit(0x04); break;
                    case SDLK_RIGHT: clearBit(0x08); break;
                    case SDLK_x:     clearBit(0x10); break;
                    case SDLK_c:     clearBit(0x20); break;
                    case SDLK_SPACE: clearBit(0x40); break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER: clearBit(0x80); break;
                }
                break;

            // ===== Manette (SDL_GameController) =====
            case SDL_CONTROLLERBUTTONDOWN:
                switch (event.cbutton.button) {
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:    setBit(0x01); menuMove = -1; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:  setBit(0x02); menuMove = 1;  break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:  setBit(0x04); break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: setBit(0x08); break;
                    case SDL_CONTROLLER_BUTTON_A:          setBit(0x10); break; // A
                    case SDL_CONTROLLER_BUTTON_B:          setBit(0x20); break; // B
                    case SDL_CONTROLLER_BUTTON_BACK:       setBit(0x40); spacePressed = true; break; // Select
                    case SDL_CONTROLLER_BUTTON_START:      setBit(0x80); enterPressed = true; break; // Start
                    default: break;
                }
                break;
            case SDL_CONTROLLERBUTTONUP:
                switch (event.cbutton.button) {
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:    clearBit(0x01); break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:  clearBit(0x02); break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:  clearBit(0x04); break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: clearBit(0x08); break;
                    case SDL_CONTROLLER_BUTTON_A:          clearBit(0x10); break;
                    case SDL_CONTROLLER_BUTTON_B:          clearBit(0x20); break;
                    case SDL_CONTROLLER_BUTTON_BACK:       clearBit(0x40); break;
                    case SDL_CONTROLLER_BUTTON_START:      clearBit(0x80); break;
                    default: break;
                }
                break;
            case SDL_CONTROLLERAXISMOTION: {
                // Utiliser le stick gauche comme D-Pad
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                    if (event.caxis.value < -DEADZONE) {
                        setBit(0x04);    // Left
                        clearBit(0x08);  // Clear Right
                    } else if (event.caxis.value > DEADZONE) {
                        setBit(0x08);    // Right
                        clearBit(0x04);  // Clear Left
                    } else {
                        clearBit(0x04);
                        clearBit(0x08);
                    }
                } else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                    if (event.caxis.value < -DEADZONE) {
                        setBit(0x01);    // Up
                        clearBit(0x02);  // Clear Down
                    } else if (event.caxis.value > DEADZONE) {
                        setBit(0x02);    // Down
                        clearBit(0x01);  // Clear Up
                    } else {
                        clearBit(0x01);
                        clearBit(0x02);
                    }
                }
                break;
            }

            // ===== Joystick brut (fallback) =====
            case SDL_JOYHATMOTION: {
                // Réinitialiser les directions puis appliquer l'état du HAT
                clearBit(0x01); clearBit(0x02); clearBit(0x04); clearBit(0x08);
                uint8_t v = event.jhat.value;
                if (v & SDL_HAT_UP)    setBit(0x01);
                if (v & SDL_HAT_DOWN)  setBit(0x02);
                if (v & SDL_HAT_LEFT)  setBit(0x04);
                if (v & SDL_HAT_RIGHT) setBit(0x08);
                if (v & SDL_HAT_UP)    menuMove = -1;
                if (v & SDL_HAT_DOWN)  menuMove = 1;
                break;
            }
            case SDL_JOYBUTTONDOWN:
                switch (event.jbutton.button) {
                    case 0: setBit(0x10); break; // A
                    case 1: setBit(0x20); break; // B
                    case 6: setBit(0x40); spacePressed = true; break; // Select (souvent "Back")
                    case 7: setBit(0x80); enterPressed = true; break; // Start
                    default: break;
                }
                break;
            case SDL_JOYBUTTONUP:
                switch (event.jbutton.button) {
                    case 0: clearBit(0x10); break;
                    case 1: clearBit(0x20); break;
                    case 6: clearBit(0x40); break;
                    case 7: clearBit(0x80); break;
                    default: break;
                }
                break;
        }
    }

    // Appliquer l'état persistant au système d'entrée de l'émulateur
    m8.joystick = (uint8_t)joyState;
}


// Fonction pour dessiner l'écran de l'émulateur (optimisée avec texture)
void swapScreen(SDL_Renderer *renderer, SDL_Texture *screenTexture, Monster8 &m8) {
    const int baseW = 320;
    const int baseH = 200;
    const int screenSize = 64000; // 320 * 200
    
    // Buffer pour les pixels RGBA
    static uint32_t pixels[baseW * baseH];
    
    // Lire la couleur de bordure directement
    uint32_t paletteBase = screenSize; // palette commence à 64000
    uint32_t bi = (static_cast<uint32_t>(m8.border) & 0xFFu) * 3u;
    uint32_t borderColorIndex = paletteBase + bi;
    uint8_t borderR = m8.screen[borderColorIndex + 2];
    uint8_t borderG = m8.screen[borderColorIndex + 1];
    uint8_t borderB = m8.screen[borderColorIndex + 0];
    
    // Convertir le buffer de l'émulateur en pixels RGBA directement
    for (int i = 0; i < baseW * baseH; ++i) {
        uint8_t paletteIndex = m8.screen[i];
        uint32_t colorIndex = paletteBase + ((uint32_t)paletteIndex) * 3;
        
        // Format RGBA8888
        uint8_t r = m8.screen[colorIndex + 2];
        uint8_t g = m8.screen[colorIndex + 1];
        uint8_t b = m8.screen[colorIndex + 0];
        
        // RGBA8888 format
        pixels[i] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
    }
    
    // Mettre à jour la texture avec les pixels
    SDL_UpdateTexture(screenTexture, nullptr, pixels, baseW * sizeof(uint32_t));
    
    // Calculer l'échelle et centrer l'image (cache statique pour éviter les recalculs)
    static int cachedOutW = 0, cachedOutH = 0;
    static int cachedOffsetX = 0, cachedOffsetY = 0, cachedDrawW = 0, cachedDrawH = 0;
    static uint8_t cachedBorderR = 0, cachedBorderG = 0, cachedBorderB = 0;
    
    int outW = 0, outH = 0;
    SDL_GetRendererOutputSize(renderer, &outW, &outH);
    
    // Recalculer seulement si la taille de la fenêtre a changé
    if (outW != cachedOutW || outH != cachedOutH) {
        cachedOutW = outW;
        cachedOutH = outH;
        
        int scaleX = outW / baseW;
        int scaleY = outH / baseH;
        int scale = (scaleX < scaleY) ? scaleX : scaleY;
        if (scale < 1) scale = 1;
        
        cachedDrawW = baseW * scale;
        cachedDrawH = baseH * scale;
        cachedOffsetX = (outW - cachedDrawW) / 2;
        cachedOffsetY = (outH - cachedDrawH) / 2;
    }
    
    // Effacer seulement si la couleur de bordure a changé
    if (borderR != cachedBorderR || borderG != cachedBorderG || borderB != cachedBorderB) {
        cachedBorderR = borderR;
        cachedBorderG = borderG;
        cachedBorderB = borderB;
        SDL_SetRenderDrawColor(renderer, borderR, borderG, borderB, 255);
    }
    
    SDL_RenderClear(renderer);
    
    // Dessiner screenTexture
    SDL_Rect dstRect = { cachedOffsetX, cachedOffsetY, cachedDrawW, cachedDrawH };
    SDL_RenderCopy(renderer, screenTexture, nullptr, &dstRect);
    
    // Mettre à jour l'écran
    SDL_RenderPresent(renderer);
}

// Afficher l'écran de chargement PNG (plein écran, centré et aggrandi)
void renderSplash(SDL_Renderer* renderer, SDL_Texture* splashTex, Monster8& m8) {
    // Effacer avec la couleur de bordure issue de la palette de l'émulateur
    uint32_t paletteBase = 64000; // palette 0 par défaut
    uint32_t bi = (static_cast<uint32_t>(m8.border) & 0xFFu) * 3u;
    uint32_t colorIndex = paletteBase + bi;
    SDL_SetRenderDrawColor(renderer,
                           m8.screen[colorIndex + 2],
                           m8.screen[colorIndex + 1],
                           m8.screen[colorIndex + 0],
                           255);
    SDL_RenderClear(renderer);

    if (splashTex) {
        int texW = 0, texH = 0;
        SDL_QueryTexture(splashTex, nullptr, nullptr, &texW, &texH);

        int outW = 0, outH = 0;
        SDL_GetRendererOutputSize(renderer, &outW, &outH);

        // Échelle flottante pour l'image PNG (pas besoin d'échelle entière)
        float scale = 1.0f;
        if (texW > 0 && texH > 0) {
            float scaleX = static_cast<float>(outW) / static_cast<float>(texW);
            float scaleY = static_cast<float>(outH) / static_cast<float>(texH);
            scale = (scaleX < scaleY) ? scaleX : scaleY;
        }

        int drawW = static_cast<int>(texW * scale);
        int drawH = static_cast<int>(texH * scale);
        int offsetX = (outW - drawW) / 2;
        int offsetY = (outH - drawH) / 2;

        SDL_Rect dst{ offsetX, offsetY, drawW, drawH };
        SDL_RenderCopy(renderer, splashTex, nullptr, &dst);
    }

    SDL_RenderPresent(renderer);
}

// Lister les cartouches dans le dossier "Cartridges"
std::vector<std::string> listCartridges(const std::string& dir) {
    std::vector<std::string> items;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                items.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur en listant les cartouches: " << e.what() << std::endl;
    }
    std::sort(items.begin(), items.end());
    return items;
}

// Dessiner le menu de sélection de cartouches via le système de texte de l'émulateur
void drawCartridgeMenu(Monster8& m8, const std::vector<std::string>& carts, int selected) {
    m8.border = 107; // bordure violette
    m8.paper = 107;   // fond violet
    m8.ClearScreen();

    uint32_t base = 0x00000500;

    m8.pen = 255;       // texte blanc
    m8.Locate(2, 2);
    m8.WriteString(base, "PLEASE SELECT A CARTRIDGE:");
    m8.PrintString(base);

    m8.pen = 0;       // texte noir
    m8.Locate(2, 4);
    m8.WriteString(base + 64, "SPACE: next   RETURN: choose");
    m8.PrintString(base + 64);

    // Afficher la liste (limiter si trop longue)
    int maxVisible = 20;
    int y = 7;
    for (int i = 0; i < (int)carts.size() && i < maxVisible; ++i) {
        m8.pen = 255;       // texte blanc
        std::string line = (i == selected ? "> " : "  ") + carts[i];
        m8.Locate(4, y + i);
        m8.WriteString(base + 128 + i * 64, line.c_str());
        m8.PrintString(base + 128 + i * 64);
    }
}

int main(int argc, char* argv[]) {
    // Forcer le driver audio PipeWire en priorité pour accélérer l’ouverture audio
    //SDL_setenv("SDL_AUDIODRIVER", "pipewire", 1);

    // Initialiser SDL (incluant audio et périphériques) afin d'éviter des réinitialisations tardives
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "Erreur lors de l'initialisation de SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_JoystickEventState(SDL_ENABLE);
    int numJoy = SDL_NumJoysticks();
    for (int i = 0; i < numJoy; ++i) {
        if (SDL_IsGameController(i)) {
            gController = SDL_GameControllerOpen(i);
            if (gController) {
                gJoystick = SDL_GameControllerGetJoystick(gController);
                std::cout << "Manette détectée: " << SDL_GameControllerName(gController) << std::endl;
                break;
            }
        }
    }
    if (!gController && numJoy > 0) {
        gJoystick = SDL_JoystickOpen(0);
        if (gJoystick) {
            std::cout << "Joystick détecté: " << SDL_JoystickName(gJoystick) << std::endl;
        }
    }
    if (!gController && !gJoystick) {
        std::cout << "Aucun joystick/manette détecté(e). Utilisez le clavier ou connectez un périphérique puis redémarrez." << std::endl;
    }

    // Initialiser SDL_image (PNG)
    int imgFlags = IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        std::cerr << "Avertissement IMG_Init (PNG): " << IMG_GetError() << std::endl;
        // Continuer: l'image de chargement est optionnelle
    }

    // Initialiser l'audio immédiatement pour éviter des re-créations de fenêtre
    int mixFlags = MIX_INIT_OGG;
    if ((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
        std::cerr << "Avertissement Mix_Init: " << Mix_GetError() << " (continuer sans prise en charge OGG)" << std::endl;
    }
    bool audioReady = true;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        std::cerr << "Erreur Mix_OpenAudio: " << Mix_GetError() << std::endl;
        audioReady = false;
    }
    bool audioInitialized = audioReady;
    
    // Parse arguments: -w for windowed; first non-flag is ROM
    bool windowed = false;
    std::string romPath;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-w" || arg == "--windowed") {
            windowed = true;
        } else if (romPath.empty()) {
            romPath = arg;
        }
    }

    // Créer la fenêtre
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (!windowed) {
        // Fullscreen windowed by default
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Monster8 Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH * PIXEL_SIZE,
        WINDOW_HEIGHT * PIXEL_SIZE,
        windowFlags
    );

    if (!windowed && window) {
        // Try exclusive fullscreen if desired later; for now we stick to fullscreen
        // If you want true exclusive, replace above with SDL_WINDOW_FULLSCREEN and call SDL_SetWindowFullscreen.
    }

    if (window == nullptr) {
        std::cerr << "Erreur lors de la création de la fenêtre: " << SDL_GetError() << std::endl;
        if (audioReady) {
            Mix_CloseAudio();
        }
        Mix_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Masquer le pointeur de souris dès le départ
    SDL_ShowCursor(SDL_DISABLE);
    
    // Forcer le double buffering pour éviter le clignotement
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // Nearest neighbor (plus rapide)
    
    // Créer le renderer avec VSync pour éviter le clignotement
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (renderer == nullptr) {
        std::cerr << "Erreur lors de la création du renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        if (audioReady) {
            Mix_CloseAudio();
        }
        Mix_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Vérifier si le VSync est activé
    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);
    if (rendererInfo.flags & SDL_RENDERER_PRESENTVSYNC) {
        std::cout << "VSync activé avec succès" << std::endl;
    } else {
        std::cout << "ATTENTION: VSync non disponible - clignotement possible" << std::endl;
    }

    // Créer une texture pour l'écran de l'émulateur (optimisation de performance)
    SDL_Texture* screenTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );
    
    // Configurer le mode de blend pour éviter les artefacts
    if (screenTexture) {
        SDL_SetTextureBlendMode(screenTexture, SDL_BLENDMODE_NONE);
    }
    
    if (screenTexture == nullptr) {
        std::cerr << "Erreur lors de la création de la texture d'écran: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        if (audioReady) {
            Mix_CloseAudio();
        }
        Mix_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialiser l'émulateur uniquement après la création réussie de la fenêtre et du renderer
    static Monster8 m8;

    // Afficher l'écran initial de l'émulateur
    swapScreen(renderer, screenTexture, m8);

    // Mode avec ou sans ROM
    bool romLoaded = false;
    if (!romPath.empty()) {
        m8.loadROM(romPath.c_str());
        
        m8.border = 0;
        m8.paper = 0;
        m8.pen = 255;
        m8.ClearScreen();

        std::cout << "ROM chargée avec succès: " << romPath << std::endl;
        romLoaded = true;
        if (!audioInitialized) {
            audioInitialized = true;
            if (!audioReady) {
                std::cerr << "Audio indisponible. L'émulateur continuera sans son." << std::endl;
            }
        }
    }

    // Charger l'écran de chargement
    SDL_Texture* splashTexture = nullptr;
    SDL_Surface* splashSurface = IMG_Load((std::string("./") + "LoadScreen.png").c_str());
    if (splashSurface) {
        splashTexture = SDL_CreateTextureFromSurface(renderer, splashSurface);
        SDL_FreeSurface(splashSurface);
    } else {
        std::cerr << "Impossible de charger LoadScreen.png: " << IMG_GetError() << std::endl;
    }

    // Etat du menu
    bool showMenu = false;
    int selectedIndex = 0;
    std::vector<std::string> cartridges;
    bool menuNeedsRedraw = false;
    bool splashNeedsRedraw = true; // afficher le splash au démarrage

    // Boucle principale
    bool quit = false;
    const int TARGET_FPS = 30;  // Réduire à 30 FPS pour tester
    const int FRAME_DELAY = 1000 / TARGET_FPS; // ~33ms par frame
    uint32_t frameStart = 0;
    
    while (!quit) {
        frameStart = SDL_GetTicks();
        
        // Entrées
        bool spacePressed = false, enterPressed = false;
        int menuMove = 0;
        handleInput(m8, quit, spacePressed, enterPressed, menuMove);
        if (quit) break;

        // Gestion du menu quand pas de ROM
        if (!romLoaded) {
            if (spacePressed) {
                if (!showMenu) {
                    cartridges = listCartridges("./Cartridges");
                    selectedIndex = 0;
                    showMenu = true;
                    menuNeedsRedraw = true; // forcer le redessin lors de l'entrée dans le menu
                    splashNeedsRedraw = false; // ne plus afficher le splash
                } else if (!cartridges.empty()) {
                    // Espace fait défiler les cartouches
                    selectedIndex = (selectedIndex + 1) % (int)cartridges.size();
                    menuNeedsRedraw = true;
                }
            }
            if (showMenu) {
                if (!cartridges.empty()) {
                    if (menuMove < 0) {
                        selectedIndex = (selectedIndex - 1 + (int)cartridges.size()) % (int)cartridges.size();
                        menuNeedsRedraw = true;
                    }
                    if (menuMove > 0) {
                        selectedIndex = (selectedIndex + 1) % (int)cartridges.size();
                        menuNeedsRedraw = true;
                    }
                    
                    if (enterPressed) {
                        std::string path = std::string("./Cartridges/") + cartridges[selectedIndex];
                        // Reset colors after selecting a cartridge
                        m8.border = 0;
                        m8.paper = 0;
                        m8.pen = 255;
                        m8.ClearScreen();
                        m8.loadROM(path.c_str());
                        std::cout << "ROM chargée avec succès: " << path << std::endl;
                        romLoaded = true;
                        showMenu = false;
                        menuNeedsRedraw = false;
                        splashNeedsRedraw = false;
                        if (!audioInitialized) {
                            audioInitialized = true;
                            if (!audioReady) {
                                std::cerr << "Audio indisponible. L'émulateur continuera sans son." << std::endl;
                            }
                        }
                    } else if (menuNeedsRedraw) {
                        // Redessiner le menu seulement si nécessaire ET si on n'a pas chargé de ROM
                        drawCartridgeMenu(m8, cartridges, selectedIndex);
                        swapScreen(renderer, screenTexture, m8);
                        menuNeedsRedraw = false;
                    }
                }
            } else {
                // Afficher l'image de chargement une seule fois
                if (splashNeedsRedraw) {
                    renderSplash(renderer, splashTexture, m8);
                    splashNeedsRedraw = false;
                }
            }
        } else {
            // Émuler un cycle uniquement si une ROM est chargée
            for(int i = 0; i < 1000; i++) {
                m8.emulateCycle();
            }

            // Afficher l'écran de l'émulateur
            swapScreen(renderer, screenTexture, m8);
        }
        
        // Limiter à 60 FPS de manière explicite
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    if (splashTexture) {
        SDL_DestroyTexture(splashTexture);
    }
    
    if (screenTexture) {
        SDL_DestroyTexture(screenTexture);
    }
    
    // Nettoyer et quitter
    // Rétablir le pointeur de souris avant de quitter
    SDL_ShowCursor(SDL_ENABLE);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Fermer proprement manette/joystick
    if (gController) {
        SDL_GameControllerClose(gController);
        gController = nullptr;
        gJoystick = nullptr; // associé à la manette
    } else if (gJoystick) {
        SDL_JoystickClose(gJoystick);
        gJoystick = nullptr;
    }

    // Arrêter et libérer l'audio de l'émulateur avant la fermeture du mixeur
    if (audioReady) {
        if (Mix_PlayingMusic() || Mix_FadingMusic() != MIX_NO_FADING) {
            Mix_HaltMusic();
        }
        if (m8.music) {
            Mix_FreeMusic(m8.music);
            m8.music = nullptr;
        }
        if (m8.sound) {
            Mix_HaltChannel(-1);
            Mix_FreeChunk(m8.sound);
            m8.sound = nullptr;
        }
        // Fermer l'audio
        Mix_CloseAudio();
    }
    Mix_Quit();
    IMG_Quit();

    SDL_Quit();
    
    return 0;
}
