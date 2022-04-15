{pkgs ? import <nixpkgs> {}}:
with pkgs; let
  clocks = {
    binary = {
      platform = "arduino:avr";
      boards = ["uno" "mega"];
      libs = ["Adafruit_NeoPixel" "DS3231"];
      features = ["NEOPIXEL" "REALTIME" "STARTUP"];
    };
    gravity = {
      platform = "arduino:avr";
      boards = ["mega"];
      libs = [
        "Adafruit_SSD1351"
        "Adafruit_GFX"
        "Adafruit_ILI9341"
        "Adafruit_BusIO"
        "Adafruit_STMPE610"
        "Adafruit_TouchScreen"
        "DS3231"
        "AccelStepper"
        "TimerInterrupt"
      ];
      features = ["DISPLAY_SEC"];
    };
  };
  avr = import ./avr.nix {inherit fetchzip;};
  arduino-libs = import ./arduino-libs.nix {inherit fetchFromGitHub fetchzip;};
  mkClock = callPackage ./mk-clock.nix {inherit arduino-libs avr;};

  allDrvs = m:
    if lib.isAttrs m && !lib.isDerivation m
    then lib.concatMap (m: allDrvs m) (builtins.attrValues m)
    else [m];
  clocksDrvs = lib.mapAttrs mkClock clocks;
in
  clocksDrvs // {all = allDrvs clocksDrvs;}
