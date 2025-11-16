#include "audio.h"
#include <iostream>
#include <glm/glm.hpp> // Para usar glm::clamp

using namespace irrklang;


// Definición de las variables
float g_GlobalVolume = 0.5f; // Volumen por defecto 50%
ISound* g_CurrentMusic = nullptr;
std::string g_CurrentMusicFile = "";

// Configura el volumen inicial en irrKlang
void audio_init() {
    // ¡DESCOMENTA ESTO! Ahora es necesario.
    if (SoundEngine) {
        SoundEngine->setSoundVolume(g_GlobalVolume);
    }
}

// Establece un volumen global (0.0 a 1.0)
void setGlobalVolume(float volume) {
    g_GlobalVolume = glm::clamp(volume, 0.0f, 1.0f);

    // AHORA USAMOS EL VOLUMEN GLOBAL DEL MOTOR
    if (SoundEngine) {
        SoundEngine->setSoundVolume(g_GlobalVolume);
    }
    std::cout << "Volumen Global: " << (int)(g_GlobalVolume * 100) << "%" << std::endl;
}

// Reproduce la música de fondo (VERSIÓN SIMPLIFICADA SIN FUNDIDO)
void playMusic(const char* path, bool loop) {
    if (!SoundEngine) return;

    // 1. Si ya estamos tocando esta canción, no hagas nada
    if (g_CurrentMusicFile == path) return;

    // --- 2. Fundido de SALIDA (Ahora es un corte directo) ---
    if (g_CurrentMusic) {
        g_CurrentMusic->stop();
        g_CurrentMusic->drop(); // Libera la memoria del sonido anterior
    }

    // --- 3. Fundido de ENTRADA (Ahora es un inicio directo) ---
    g_CurrentMusic = SoundEngine->play2D(path, loop, false, true); // (path, loop, paused, track)

    if (g_CurrentMusic) {
        // El volumen ya está controlado por setGlobalVolume
        g_CurrentMusicFile = path;
    }
    else {
        g_CurrentMusicFile = "";
    }
}

// Detiene la música de fondo
void stopMusic() {
    if (g_CurrentMusic) {
        g_CurrentMusic->stop();
        g_CurrentMusic->drop();
        g_CurrentMusic = nullptr;
        g_CurrentMusicFile = "";
    }
}

// Reproduce un efecto de sonido "dispara y olvida"
void playSoundEffect(const char* path) {
    if (SoundEngine) {
        // "Dispara y olvida". Usará el volumen global
        // que ya establecimos en setGlobalVolume
        SoundEngine->play2D(path, false);
    }
}

void changeGlobalVolume(float amount) {
    // 1. Calcula el nuevo volumen
    float newVolume = g_GlobalVolume + amount;

    // 2. Llama a la función que ya teníamos,
    //    que se encarga de limitarlo (clamp) y aplicarlo.
    setGlobalVolume(newVolume);
}