#include <program_rekam_gerak/tangan_controller.h>


TanganController tanganController;

/* Replika sebagian dari Transmit.ino */
void runAll(uint times, int music) {
    if (music == 1) {
        tanganController.transmit();
    }
    else if (music == 0) {
        // music di-mute maka tidak perlu transmit gerakan
    }

    /* TO DO: Bagaimana caranya agar fungsi ini tidak langsung selesai,
       namun perlu menunggu selama times milidetik terlebih dahulu? */
}

/* Replika sebagian dari GerakTari.ino */
void cobaGerakTari(int isWalking, int music_state, int speed, int times) {
    tanganController.bacaGerak(190, speed);
    runAll(times, music_state);
    tanganController.bacaGerak(191, speed);
    runAll(times, music_state);
    tanganController.bacaGerak(192, speed);
    runAll(times, music_state);
}


int main() {
    cobaGerakTari(0, 1, 6, 585);
    return 0;
}