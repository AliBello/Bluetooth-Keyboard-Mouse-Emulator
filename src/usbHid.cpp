#include "usbHid.h"

USBHIDMouse mouse;
USBHIDKeyboard keyboard;

void handleUsbMode(bool mouseMode) {
    if (mouseMode) {
        usbMouse();
    } else  {
        usbKeyboard();
    }
    delay(5);
}
void usbMouse() {
    mouse.begin();
    int moveX = 0;
    int moveY = 0;
    if (M5Cardputer.Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

        if (M5Cardputer.Keyboard.isKeyPressed('/')) {
            // droite
            moveX = 1;
        } 
        
        if (M5Cardputer.Keyboard.isKeyPressed(',')) {
            // gauche
            moveX = -1;
        }  
        
        if (M5Cardputer.Keyboard.isKeyPressed(';')) {
            // haut
            moveY = -1;
        } 
        
        if (M5Cardputer.Keyboard.isKeyPressed('.')) {
            // bas
            moveY = 1;
        }

        // clics souris
        if (status.enter) {
            // gauche
            mouse.press(MOUSE_BUTTON_LEFT);
        } else if (M5Cardputer.Keyboard.isKeyPressed('\\')) {
            // droit
            mouse.press(MOUSE_BUTTON_RIGHT);
        }
        // Send
        mouse.move(moveX, moveY);

    } else {
        mouse.release(MOUSE_BUTTON_LEFT);
        mouse.release(MOUSE_BUTTON_RIGHT);
    }
}

void usbKeyboard() {
    static bool inited = false;
    if (!inited) { keyboard.begin(); inited = true; }

    if (!M5Cardputer.Keyboard.isChange()) return;

    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

    KeyReport report = {0};
    report.modifiers = status.modifiers;

    uint8_t idx = 0;

    // Replace , . / ; with arrow keys
    if (M5Cardputer.Keyboard.isKeyPressed(',') && idx < 6)
        report.keys[idx++] = 0x50;  // LEFT

    if (M5Cardputer.Keyboard.isKeyPressed('.') && idx < 6)
        report.keys[idx++] = 0x51;  // DOWN

    if (M5Cardputer.Keyboard.isKeyPressed('/') && idx < 6)
        report.keys[idx++] = 0x4F;  // RIGHT

    if (M5Cardputer.Keyboard.isKeyPressed(';') && idx < 6)
        report.keys[idx++] = 0x52;  // UP

    // Add all other keys EXCEPT , . / ;
    for (auto k : status.hid_keys) {
        if (idx >= 6) break;

        // HID keycodes for , . / ;
        if (k == 0x36 ||  // ,
            k == 0x37 ||  // .
            k == 0x38 ||  // /
            k == 0x33) {  // ;
            continue;
        }

        report.keys[idx++] = k;
    }

    if (idx == 0 && report.modifiers == 0) {
        keyboard.releaseAll();
    } else {
        keyboard.sendReport(&report);
    }
}
