
# Projektübersicht
Dieses Projekt steuert einen ODrive-Motorcontroller über einen ESP32, zeigt wichtige Informationen auf einem OLED-Display an und speichert Konfigurationen im EEPROM. Das System bietet verschiedene Modi zur Steuerung der Motoren, wie z.B. Geschwindigkeitsregelung, und ermöglicht es dem Benutzer, Einstellungen über Buttons zu ändern. Die Konfiguration erfolgt hauptsächlich über die `Config`-Dateien.

## Hauptbestandteile des Projekts

### 1. Konfiguration (`Config.h` und `Config.cpp`)
- **Allgemeine Einstellungen**: Die Datei `Config.cpp` enthält die meisten hardwarebezogenen Einstellungen, wie z.B. die Zuordnung der GPIO-Pins für Buttons und das Display.
- **GPIO-Zuordnungen**: Die Pins für die Benutzerbuttons (hoch, runter, OK) sowie für das Display und andere Hardware-Komponenten sind hier definiert.
- **Konfigurationsmöglichkeiten**: Über diese Dateien können verschiedene Hardwareparameter und Verhalten des Systems eingestellt werden. Beispiele sind die Geschwindigkeitseinstellungen und die Initialisierung des Displays.

### 2. Geschwindigkeitssteuerung (`SpeedController.h` und `SpeedController.cpp`)
- **Motorensteuerung**: Diese Komponenten sind für die Steuerung der Motoren verantwortlich, insbesondere für die Regelung der Geschwindigkeit und Richtung.
- **Sicherheitsfunktion bei 0 km/h**: Wenn die Geschwindigkeit auf 0 km/h fällt, werden die Motoren automatisch in einen sicheren Zustand (IDLE) versetzt, um unnötigen Energieverbrauch und mögliche Gefahren zu vermeiden.
- **Regelmodi**: Das System unterstützt verschiedene Modi wie Geschwindigkeitsregelung und Positionsregelung, die über den `SpeedController` gesteuert werden können.

### 3. Display-Steuerung (`DisplayManager.h` und `DisplayManager.cpp`)
- **Anzeige von Systemzuständen**: Das OLED-Display zeigt wichtige Informationen wie aktuelle Geschwindigkeit, Fehlerzustände und Systemmeldungen an.
- **Boot-Logo**: Beim Start des Systems wird ein Boot-Logo angezeigt, das für eine kurze Zeit sichtbar bleibt, bevor das Hauptmenü erscheint.
- **Menüsystem**: Das Display-Management bietet ein einfaches Menüsystem, durch das der Benutzer navigieren kann, um verschiedene Parameter anzuzeigen oder Einstellungen zu ändern.

### 4. EEPROM-Einstellungen (`EEPROMSettings.h`)
- **Speichern und Laden von Konfigurationen**: Das System speichert verschiedene Einstellungen wie Geschwindigkeitsmodi und Steuerungsparameter im EEPROM, sodass diese auch nach einem Neustart des Systems erhalten bleiben.
- **Konfiguration im EEPROM**: Die wichtigsten Parameter, wie z.B. Verstärkungseinstellungen für die Geschwindigkeit, können hier gespeichert und bei Bedarf angepasst werden.

### 5. ODrive-Steuerung über UART (`ODriveUART.h` und `ODriveUART.cpp`)
- **Kommunikation mit ODrive**: Diese Dateien verwalten die serielle Kommunikation mit dem ODrive-Motorcontroller. Über UART können Befehle gesendet und Statusinformationen vom ODrive abgerufen werden.
- **Fehlerbehandlung**: Es gibt Funktionen zum Zurücksetzen und Überwachen von Fehlern, um sicherzustellen, dass die Motoren immer in einem betriebsbereiten Zustand sind.

## Weitere wichtige Funktionen

- **Debug-Modus**: Der Debug-Modus kann aktiviert werden, indem bestimmte Buttons beim Systemstart gedrückt gehalten werden. Dies ermöglicht zusätzliche Diagnosen und detaillierte Ausgabe von Systeminformationen über die serielle Schnittstelle.
- **Benutzerinteraktion**: Das System wird hauptsächlich über die definierten Buttons gesteuert. Der Benutzer kann durch die Menüs navigieren, Parameter anpassen und das System sicher starten oder herunterfahren.

## Zusammenfassung der Bedienung
- **Systemstart**: Beim Einschalten des ESP32 werden das Display und der ODrive initialisiert, und ein Boot-Logo wird angezeigt. Das System führt einen Start-Check durch, um sicherzustellen, dass alle Komponenten betriebsbereit sind.
- **Geschwindigkeitsregelung**: Die Motoren werden basierend auf den Benutzereinstellungen und dem aktuellen Modus gesteuert. Bei 0 km/h werden die Motoren automatisch deaktiviert.
- **Systemabschaltung**: Über einen definierten Button kann das System sicher heruntergefahren werden, wobei die Motoren in den IDLE-Zustand versetzt und das Display ausgeschaltet werden.
