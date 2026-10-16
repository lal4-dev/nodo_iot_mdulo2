#include <WebServer.h>
#include <WiFi.h>

// Aclaracion previa en esencia esta actividad son los codigos de los ejercicios
// 2 y 3 del profe pero adaptados a los que se necesita y pide pero ademas a no
// tener la placa en casa tuve que instalar wokwi en visual con la extension de
// plataform IO que me recomendo un compañero para poder hacer y probar todo lo
// tuve que hacer en c++ porque al parecer en archivos punto ino la extension no
// funca o no se puede descargar pero en c++ si, asi que realizo todo en c++ y
// cambio la extension segun gemini deberia funcionar igualmente por en esencia
// son casi lo mismo dejo esto aclarado por que al no tener el material donde
// probarlo no puedo corroborarlo solo confiar que es asi

// ==================================================
// DECLARACIÓN DE PROTOTIPOS(para que no se rompa nada
// y pueda desarrollar al ritmo que avanza el tp
// ==================================================
void conectarWiFi();
String paginaWeb();
void paginaPrincipal();
void setColor(uint8_t rojo, uint8_t verde, uint8_t azul);
void mostrarResultado(String estado, String color, String resultado);
void manejarLed();
void manejarEstado();

// ==================================================
// ACTIVIDAD A — CONEXIÓN WI-FI
// ==================================================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

// ==================================================
// ACTIVIDAD B
// ==================================================
WebServer servidor(80);

String paginaWeb() {
  String pagina = "";
  pagina += "<!DOCTYPE html>";
  pagina += "<html>";
  pagina += "<head>";
  pagina +=
      "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  pagina += "<title>Control de LED RGB - ESP32-S3</title>";
  pagina += "</head>";
  pagina += "<body>";
  pagina += "<h1>Control de LED RGB - ESP32-S3</h1>";
  pagina += "<p><a href='/led?estado=normal' onclick='fetch(this.href); return "
            "false;'><button type='button'>NORMAL</button></a></p>";
  pagina += "<p><a href='/led?estado=alerta' onclick='fetch(this.href); return "
            "false;'><button type='button'>ALERTA</button></a></p>";
  pagina += "<p><a href='/led?estado=precaucion' onclick='fetch(this.href); "
            "return false;'><button type='button'>PRECAUCION</button></a></p>";
  pagina += "<p><a href='/led?estado=apagado' onclick='fetch(this.href); "
            "return false;'><button type='button'>APAGAR</button></a></p>";
  pagina += "</body>";
  pagina += "</html>";
  return pagina;
}
// aca por la dudas usoe el fetch para "cancelar el redireccionamiento que me
// obligaba a volver a tras con las flechas del navegador, para poder probar
// todos los colores seguidos"
void paginaPrincipal() { servidor.send(200, "text/html", paginaWeb()); }

// ==================================================
// ACTIVIDAD C
// ==================================================
#define LED_RGB 48
#define INTENSIDAD 30

void setColor(uint8_t rojo, uint8_t verde, uint8_t azul) {
  rojo = (rojo * INTENSIDAD) / 100;
  verde = (verde * INTENSIDAD) / 100;
  azul = (azul * INTENSIDAD) / 100;
  neopixelWrite(LED_RGB, rojo, verde, azul);
}

// ==================================================
// ACTIVIDAD D
// ==================================================
void mostrarResultado(String estado, String color, String resultado) {
  Serial.println("------------------------------");
  Serial.println("SOLICITUD HTTP RECIBIDA");
  Serial.println("ESTADO: " + estado);
  Serial.println("LED RGB: " + color);
  Serial.println("RESULTADO: " + resultado);
  Serial.println("------------------------------");
}

// ==================================================
// ACTIVIDAD E
// ==================================================
String estadoActual = "apagado";
String colorActual = "APAGADO";

void manejarLed() {
  if (!servidor.hasArg("estado")) {
    mostrarResultado("", "SIN CAMBIOS", "ERROR");
    servidor.send(400, "text/plain", "ERROR");
    return;
  }

  String estado = servidor.arg("estado");
  estado.toLowerCase();

  if (estado == "normal") {
    setColor(0, 255, 0);
    estadoActual = "normal";
    colorActual = "VERDE";
    mostrarResultado(estado, "VERDE", "OK");
    servidor.send(200, "text/plain", "OK");
  } else if (estado == "alerta") {
    setColor(255, 0, 0);
    estadoActual = "alerta";
    colorActual = "ROJO";
    mostrarResultado(estado, "ROJO", "OK");
    servidor.send(200, "text/plain", "OK");
  } else if (estado == "precaucion") {
    setColor(255, 255, 0);
    estadoActual = "precaucion";
    colorActual = "AMARILLO";
    mostrarResultado(estado, "AMARILLO", "OK");
    servidor.send(200, "text/plain", "OK");
  } else if (estado == "apagado") {
    setColor(0, 0, 0);
    estadoActual = "apagado";
    colorActual = "APAGADO";
    mostrarResultado(estado, "APAGADO", "OK");
    servidor.send(200, "text/plain", "OK");
  } else {
    mostrarResultado(estado, "SIN CAMBIOS", "ERROR");
    servidor.send(400, "text/plain", "ERROR");
  }
}

void manejarEstado() {
  Serial.println("------------------------------");
  Serial.println("CONSULTA DE ESTADO");
  Serial.println("ESTADO ACTUAL: " + estadoActual);
  Serial.println("LED RGB: " + colorActual);
  Serial.println("------------------------------");

  servidor.send(200, "text/plain", estadoActual);
}

// ==================================================
// SETUP Y LOOP
// ==================================================
void setup() {
  // Configurar el Monitor Serie a 115200 baudios parte de la Actividad A
  Serial.begin(115200);

  // Color inicial del LED
  setColor(0, 0, 0);

  // Conexión Wi-Fi Actividad A
  conectarWiFi();

  // Configuración de rutas Actividades B, C y E
  servidor.on("/", paginaPrincipal);
  servidor.on("/led", manejarLed);
  servidor.on("/estado", manejarEstado);

  // Iniciar servidor HTTP en el puerto 80 parte de la Actividad B
  servidor.begin();
  Serial.println();
  Serial.println("Servidor HTTP iniciado");
}

void loop() { servidor.handleClient(); }