// ####################################################################################################################
// 
// Code part of CarCluster project by Andrej Rolih. See .ino file more details
// 
// ####################################################################################################################

#include "BeamNGGame.h"

struct __attribute__((packed)) outgauge_lights_t {
  boolean SHIFT       :1; // shift light
  boolean FULLBEAM    :1; // full beam
  boolean HANDBRAKE   :1; // handbrake
  boolean PITSPEED    :1; // pit speed limiter // N/A
  boolean TC          :1; // tc active or switched off
  boolean SIGNAL_L    :1; // left turn signal
  boolean SIGNAL_R    :1; // right turn signal
  boolean SIGNAL_ANY  :1; // shared turn signal // N/A
  boolean OILWARN     :1; // oil pressure warning
  boolean BATTERY     :1; // battery warning
  boolean ABS         :1; // abs active or switched off
  boolean SPARE       :1; // N/A
  uint16_t unused ;
};

struct beamng_packet_t {
    unsigned       time;            // time in milliseconds (to check order) // N/A, hardcoded to 0
    char           car[4];          // Car name // N/A, fixed value of "beam"
    unsigned short flags;           // Info (see OG_x below)
    char           gear;            // Reverse:0, Neutral:1, First:2...
    char           plid;            // Unique ID of viewed player (0 = none) // N/A, hardcoded to 0
    float          speed;           // M/S
    float          rpm;             // RPM
    float          turbo;           // BAR
    float          engTemp;         // C
    float          fuel;            // 0 to 1
    float          oilPressure;     // BAR // N/A, hardcoded to 0
    float          oilTemp;         // C
    outgauge_lights_t dashLights;   // Dash lights available (see DL_x below)
    outgauge_lights_t showLights;   // Dash lights currently switched on
    float          throttle;        // 0 to 1
    float          brake;           // 0 to 1
    float          clutch;          // 0 to 1
    char           display1[16];    // Usually Fuel // N/A, hardcoded to ""
    char           display2[16];    // Usually Settings // N/A, hardcoded to ""
    int            id;              // optional - only if OutGauge ID is specified
};

BeamNGGame::BeamNGGame(GameState& game, int port): Game(game) {
  this->port = port;
}

void BeamNGGame::begin() {
  // BeamNG sends data as a UDP blob of data
  // Telemetry protocol described here: https://github.com/fuelsoft/out-gauge-cluster

  if (beamUdp.listen(port)) {
    beamUdp.onPacket([this](AsyncUDPPacket packet) {
      if (packet.length() >= sizeof(beamng_packet_t)) {
        beamng_packet_t outgauge = *((beamng_packet_t*)packet.data());

        // GEAR
        int beamGear = outgauge.gear;
        if (beamGear == 0) {
          gameState.gear = GearState_Auto_R;
        } else if (beamGear == 1) {
          gameState.gear = GearState_Auto_D;
        } else if (beamGear >= 10) {
          gameState.gear = GearState_Auto_D;
        } else {
          gameState.gear = static_cast<GearState>(beamGear - 1);
        }

        // SPEED
        int someSpeed = outgauge.speed * 3.6;               // Speed is in m/s
        gameState.speed = someSpeed;

        // CURRENT_ENGINE_RPM
        gameState.rpm = outgauge.rpm;

        // ENGINE TEMPERATURE
        gameState.coolantTemperature = outgauge.engTemp;

        // LIGHTS
        // only set fields if the game supports them
        if (outgauge.dashLights.SIGNAL_L) gameState.leftTurningIndicator = outgauge.showLights.SIGNAL_L;
        if (outgauge.dashLights.SIGNAL_R) gameState.rightTurningIndicator = outgauge.showLights.SIGNAL_R;
        if (outgauge.dashLights.FULLBEAM) gameState.highBeam = outgauge.showLights.FULLBEAM;
        if (outgauge.dashLights.BATTERY) gameState.batteryLight = outgauge.showLights.BATTERY;
        if (outgauge.dashLights.ABS) gameState.absLight = outgauge.showLights.ABS;
        if (outgauge.dashLights.HANDBRAKE) gameState.handbrake = outgauge.showLights.HANDBRAKE;
        if (outgauge.dashLights.TC) gameState.offroadLight = outgauge.showLights.TC;
      }
    });
  }
}