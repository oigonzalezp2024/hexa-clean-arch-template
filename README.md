# HexaCleanArch.ino: Plantilla de Arquitectura Limpia para Arduino (Archivo Único)

## 🌟 1. Visión General del Proyecto

Este archivo único (`HexaCleanArch.ino`) es una **Plantilla Base** que implementa la **Arquitectura Hexagonal (Ports and Adapters)** en C++ para **Arduino/ESP8266**.

El objetivo es demostrar y facilitar el uso de la **Inversión de Dependencia (DIP)** en *firmware*, logrando que la Lógica Central sea completamente independiente del hardware.

### 

[Image of Clean Architecture layers diagram]


El código está optimizado para:
1.  **Testeabilidad Conceptual:** Aunque el archivo no incluye las pruebas, su estructura con **Interfaces (Puertos)** permite ejecutar pruebas unitarias del *Core Logic* en un PC.
2.  **Portabilidad:** Facilita el cambio de hardware (ej., cambiar un LCD por un OLED) sin modificar la lógica de negocio.

***

## 🏗️ 2. Estructura Arquitectónica (Organización en un Solo Archivo)

El código se organiza mediante comentarios que demarcan las cuatro capas de la Arquitectura Limpia:

| Sección del Código | Capa | Rol Principal |
| :--- | :--- | :--- |
| **`CAPA DE DOMINIO 🧠`** | **Núcleo** | Define Entidades (`User`) e **Interfaces/Puertos** (`IUserRepository`, `ITimer`, etc.). |
| **`CAPA DE SERVICIOS ✨`** | **Núcleo** | Lógica de negocio (`UserCycleService`). Depende **solo de interfaces**. |
| **`CAPA DE APLICACIÓN ⚙️`** | **Núcleo** | Orquestación y *timing* (`Controller`). Depende **solo de interfaces**. |
| **`CAPA DE INFRAESTRUCTURA 💾`** | **Adaptadores** | Implementaciones concretas atadas a Arduino (`LCDAdapter`, `ArduinoTimer`, etc.). |
| **`Composition Root ⚡`** | **Plataforma** | Sección `setup()` y `loop()`: instancia clases concretas e **inyecta las dependencias**. |

***

## ⚙️ 3. Puntos Clave de la Arquitectura

### 3.1. Abstracción del Tiempo (ITimer)

Para aislar la lógica de negocio de la función **`millis()`** de Arduino (una dependencia directa de la plataforma), se utiliza el puerto **`ITimer`**.

* El `Controller` (Lógica Central) solo llama a `timer->getTimeMs()`.
* El **`ArduinoTimer`** (Adaptador) es la única clase que conoce y llama a `millis()`.

### 3.2. Robustez del Timing (Manejo de Intervalos)

El método `Controller::update()` utiliza un bucle **`while`** en lugar de un `if` simple para manejar el tiempo. Esta corrección asegura que, si el `loop()` de Arduino se retrasa (por alguna tarea pesada), el sistema se **ponga al día** y ejecute la lógica la cantidad de veces correcta para mantener la cadencia definida de 1000ms.

***

## 🚀 4. Guía de Reutilización

Este archivo es la plantilla perfecta para iniciar nuevos proyectos de *firmware*. Para usarlo:

1.  **Modifica el Núcleo:** Edita las clases dentro de las secciones **DOMINIO**, **SERVICIOS** y **APLICACIÓN** para definir la lógica de negocio de tu nuevo proyecto.
2.  **Crea Adaptadores:** Reemplaza o modifica los adaptadores en la sección **INFRAESTRUCTURA**. Por ejemplo, reemplaza `SimulatedUserRepository` por un `EEPROMRepository` o un `WiFiClientAdapter`.
3.  **Actualiza el Composition Root:** En la sección **`Composition Root`**, inyecta tus nuevos adaptadores y servicios al `Controller`.
