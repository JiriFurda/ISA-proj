# Feed reader in Atom format with TLS support (ISA project)
Bachelor study at FIT VUT Brno  
5th semester - winter 2018  
Subject: **Network Applications and Network Administration (ISA)**

Task: [zadani.pdf](./zadani.pdf) (in Czech)

## Score
* Overall: **15.2/20**
* [Corresponding version](./commit/36920f86fc708913750cb06b1024b23025718f3a)

### Commentary (in Czech)
* Podpora formátu Atom
* Podpora formátu RSS
* Podpora TLS
* Úspěšné zpracování formátu Atom získaného přes HTTP
* Úspěšný test RSS2 (HTTP)
* Atom: Aplikace se nevypořádala s přijímáním dat po malých částech
* RSS: Aplikace se nevypořádala s přijímáním dat po malých částech
* Neúspěšný test RSS-v1
* Ověřená podpora parametru -f
* Úspěšný test Atom s explicitně zadaným číslem portu
* Úspěšný test RSS při připojení na explicitně zadaný port
* Úspěšné ověření certifikátu a navázání spojení přes TLS
* Úspěšný test Atom s parametrem -T
* Úspěšný test Atom -a
* Úspěšný test Atom s parametrem -u
* Úspěšný test Atom s parametry -Tau
* Úspěšný test RSS s parametrem -T
* Chybí podpora elementu author pro RSS
* Nedetekováno rozšíření: podpora Dublin Core pro RSS1 (dc:creator)
* Rozšíření: podpora elementu dc:creator v RSS2 (Dublin Core)
* Neúspěšný test RSS s parametrem -u
* Neúspěšný test RSS s parametry -Tau
* Program vypíše chybu při odpovědi serveru Forbiden
* Aplikace detekovala pokus o připojení k uzavřenému portu
* Nedetekováno rozšíření: přesměrování HTTP
* Neúspěšný test SSLbezověření
* Podpora parametru -c
* Podpora parametru -C
* Aplikace detekovala využití certifikátu vystaveného pro cizí doménu
* Strohá/nepřehledná/nevysvětlující nápověda programu
* Strohé README
* Literatura není využitá v textu dokumentace
* V úpravě textu je potřeba zlepšit dodržování typografických pravidel.
* Chybí popis teorie potřebné k vypracování projektu
* Dokumentován popis implementace
* Chybí popis testování
