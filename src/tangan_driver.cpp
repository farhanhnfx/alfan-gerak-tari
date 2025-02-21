#include <program_rekam_gerak/gerak_tari.h>

int main() {
std::vector<GerakTari> gerak_tari_var_kiri;
    // Preload config gerak tari harapannya akan dijalankan ketika program dimulai

    GerakTariHandler gerakTariHandler;
    gerakTariHandler.preload_config();

    gerakTariHandler.play();

    return 0;
}
