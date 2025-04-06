#ifndef ALFAN_TERMINAL_HELPER_H
#define ALFAN_TERMINAL_HELPER_H

/* 
 * Class yang digunakan untuk membangun terminal
 * Contoh penggunaan ketika Rekam Gerak
 * serta menguji gerakan yang telah direkam
 */
class TerminalHelper {
    private:
    static bool is_running; // Apakah terminal sudah dijalankan?

    public:
    static int key_pressed; // Menampung key yang sedang dipencet

    // Function to make terminal non-blocking
    static void setNonBlockingInput();

    // Fungsi untuk menyimpan state original dari terminal
    static void saveOriginalTerminal();

    // Fungsi untuk me-reset kembali terminal seperti semula
    static void resetTerminal();

    // Function to check for a key press
    static int getKeyPress();

    // Fungsi untuk membangun loop terminal (contoh penggunaan pada Rekam Gerak dan Standalone Gerak Tari untuk uji gerakan)
    static void buildTerminalLoop(void (*function)());
};

#endif