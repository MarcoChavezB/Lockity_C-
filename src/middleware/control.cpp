#include "control.h"

bool is_drawer_owner(String id_usuario, String drawer_id) {
    for (int i = 0; i < usuarioCount; i++) {
        if (usuarios[i].id_usuario == id_usuario) {
            for (int j = 0; j < usuarios[i].cajon_count; j++) {
                if (usuarios[i].cajones_usuario[j] == drawer_id) {
                    return true;
                }
            }
        }
    }
    return false;
}
