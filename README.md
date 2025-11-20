# Windows-Keylogger-AZERTY V1
Keylogger codé en C++ pour Windows exclusivement pour les claviers AZERTY

Capture toutes les touches utiles ainsi que les caractères spéciaux de toutes les Virtual_Key (Winuser.h) (sauf les touches logo Windows droit et gauche et les touches F1-12 car jugé inutile)
Petit point : le symbole "€" ne s'affiche pas, par conséquent il sera affiché tel que : [RALT]e ou [LALT]e

Ce keylogger est une première version, il est encore en cours de développement pour de nouvelles MAJ





Ajout prochainement pour la V2 :


Capture de fenêtre : Information sur les onglets ouverts

Cryptage des logs : Le fichier keylog.txt sera chiffré et pourra être déchiffré uniquement par l'attaquant

Obfuscation : Keylogger indétectable par Windows Defender

Log à distance : Probablement une version où les logs pourraient être envoyé à l'attaquant via SMTP

Registre : Démarrage du programme depuis le registre windows après intégration
