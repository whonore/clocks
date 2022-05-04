{pkgs ? import <nixpkgs> {}}:
with pkgs; let
  gitignoreSrc = pkgs.fetchFromGitHub {
    owner = "hercules-ci";
    repo = "gitignore.nix";
    rev = "bff2832ec341cf30acb3a4d3e2e7f1f7b590116a";
    sha256 = "sha256-kekOlTlu45vuK2L9nq8iVN17V3sB0WWPqTTW3a2SQG0=";
  };
  inherit (import gitignoreSrc {inherit (pkgs) lib;}) gitignoreSource;

  clocks = {
    binary = {
      platform = "arduino:avr";
      boards = ["uno" "mega"];
      libs = ["Adafruit_NeoPixel" "DS3231"];
      features = ["NEOPIXEL" "REALTIME" "STARTUP"];
      defines = {};
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
      defines = {
        "MIN_ZOFF" = "0";
        "HOUR_ZOFF" = "0";
      };
    };
  };
  avr = import ./avr.nix {inherit fetchzip;};
  arduino-libs = import ./arduino-libs.nix {inherit fetchFromGitHub fetchzip;};
  mkClock = callPackage ./mk-clock.nix {inherit arduino-libs avr gitignoreSource;};

  allDrvs = m:
    if lib.isAttrs m && !lib.isDerivation m
    then lib.concatMap (m: allDrvs m) (builtins.attrValues m)
    else [m];
  clocksDrvs = lib.mapAttrs mkClock clocks;
in
  clocksDrvs // {all = allDrvs clocksDrvs;}
