#ifndef AUDIO_H
#define AUDIO_H
#include "globals.h"

// --- Variables de Estado de Audio ---
extern float g_GlobalVolume;    // El volumen maestro (0.0 a 1.0)
extern ISound* g_CurrentMusic;  // Puntero a la música actual para poder pararla
extern std::string g_CurrentMusicFile; // Para no reiniciar la música si ya está sonando

const int FADE_TIME_MS = 1500;

// --- Funciones de Audio ---
void audio_init(); // Para configurar el volumen inicial
void setGlobalVolume(float volume);
void playMusic(const char* path, bool loop = true);
void stopMusic();
void playSoundEffect(const char* path); // Para "eventos" (talar, fuego, etc.)
void changeGlobalVolume(float amount);

#endif