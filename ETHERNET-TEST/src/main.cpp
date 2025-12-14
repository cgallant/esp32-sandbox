#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

// ---------------- W5500 PINS (ESP32-S3 DevKitC-1) ----------------
#define W5500_CS   10
#define W5500_RST  14
#define W5500_MOSI 11
#define W5500_MISO 13
#define W5500_SCK  12

// ---------------- NETWORK ----------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress espIP(192, 168, 1, 177);
IPAddress macServer(192, 168, 1, 152);   // your Mac IP (server machine)

// ---------------- CLIENTS ----------------
EthernetClient c1;   // connects to macServer:5001
EthernetClient c2;   // connects to macServer:5002

// ---------------- PER-CLIENT STATE ----------------
struct TcpClientState {
  EthernetClient* client;
  const char*     name;
  uint16_t        port;

  bool            everConnected;
  unsigned long   lastTryMs;
  unsigned long   lastHbMs;

  // non-blocking line buffer
  static const size_t BUFSZ = 160;
  char           buf[BUFSZ];
  size_t         len;
};

TcpClientState s1;   // will be set in setup()
TcpClientState s2;   // will be set in setup()

const unsigned long RETRY_MS = 1000;   // try connect every 1s (fast feedback)
const unsigned long HB_MS    = 500;    // heartbeat every 0.5s (adjust later)

static void initState(TcpClientState &s, EthernetClient &c, const char* name, uint16_t port) {
  s.client = &c;
  s.name   = name;
  s.port   = port;
  s.everConnected = false;
  s.lastTryMs = 0;
  s.lastHbMs  = 0;
  s.len = 0;
  memset(s.buf, 0, sizeof(s.buf));
}

static void pumpRxNonBlocking(TcpClientState &s) {
  EthernetClient &cli = *s.client;

  while (cli.available() > 0) {
    int ch = cli.read();
    if (ch < 0) break;

    char c = (char)ch;

    // treat \n as end-of-line; ignore \r
    if (c == '\r') continue;

    if (c == '\n') {
      // complete line in s.buf[0..len-1]
      s.buf[s.len] = '\0';
      Serial.print("[");
      Serial.print(s.name);
      Serial.print("] RX: ");
      Serial.println(s.buf);
      s.len = 0;
      continue;
    }

    // append if room, else drop line to avoid wedging
    if (s.len < TcpClientState::BUFSZ - 1) {
      s.buf[s.len++] = c;
    } else {
      // overflow: reset buffer (drop current line)
      s.len = 0;
    }
  }
}

static void serviceOneClient(TcpClientState &s, unsigned long now) {
  EthernetClient &cli = *s.client;

  // connection state
  if (!cli.connected()) {
    if (s.everConnected) {
      Serial.print("[");
      Serial.print(s.name);
      Serial.println("] disconnected");
      s.everConnected = false;
    }

    // attempt reconnect on timer
    if (now - s.lastTryMs >= RETRY_MS) {
      s.lastTryMs = now;
      Serial.print("[");
      Serial.print(s.name);
      Serial.print("] connecting... ");

      if (cli.connect(macServer, s.port)) {
        Serial.println("SUCCESS");
        s.everConnected = true;
        s.len = 0;

        // identify which socket
        cli.print("HELLO");
        cli.print(s.port == 5001 ? "1" : "2");
        cli.println(",PANEL");
      } else {
        Serial.println("FAILED");
      }
    }
    return;
  }

  // RX (non-blocking)
  pumpRxNonBlocking(s);

  // heartbeat (timer-based)
  if (now - s.lastHbMs >= HB_MS) {
    s.lastHbMs = now;
    Serial.print("[");
    Serial.print(s.name);
    Serial.print("] TX: ");

    if (s.port == 5001) {
      cli.println("HEARTBEAT1,PANEL");
      Serial.println("HEARTBEAT1,PANEL");
    } else {
      cli.println("HEARTBEAT2,PANEL");
      Serial.println("HEARTBEAT2,PANEL");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(7000);

  Serial.println("\n=== W5500 Dual Client Test (5001 + 5002) NON-BLOCKING ===");

  // Reset W5500
  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, LOW);
  delay(50);
  digitalWrite(W5500_RST, HIGH);
  delay(250);

  // SPI + Ethernet
  SPI.begin(W5500_SCK, W5500_MISO, W5500_MOSI, W5500_CS);
  Ethernet.init(W5500_CS);
  Ethernet.begin(mac, espIP);
  delay(500);

  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Link: ");
  Serial.println(Ethernet.linkStatus() == LinkON ? "UP" : "DOWN");

  // init per-client states (THIS AVOIDS THE PLATFORMIO BRACE-INIT ERRORS)
  initState(s1, c1, "S1", 5001);
  initState(s2, c2, "S2", 5002);

  Serial.println("Expect Python servers listening on Mac:");
  Serial.println("  5001 and 5002");
}

void loop() {
  unsigned long now = millis();

  // keep lease alive if you ever switch to DHCP later (harmless for static)
  Ethernet.maintain();

  serviceOneClient(s1, now);
  serviceOneClient(s2, now);

  // tiny yield so we don't hammer CPU (NOT REQUIRED, but helps Serial)
  delay(1);
}