{fetchFromGitHub, fetchzip}: {
  "AccelStepper" = fetchzip {
    url = "http://www.airspayce.com/mikem/arduino/AccelStepper/AccelStepper-1.61.zip";
    sha256 = "sha256-p907J2KYQOo0Df/ywNVbhWvX5kR+s/3/CS9HSiFxMC4=";
  };
  "Adafruit_BusIO" = fetchFromGitHub {
    owner = "adafruit";
    repo = "Adafruit_BusIO";
    rev = "1.9.1";
    sha256 = "sha256-MjtR8s7vVcweld74Bd0RsyKDH+re8UioacHKGUmWLgs=";
  };
  "Adafruit_GFX" = fetchFromGitHub {
    owner = "adafruit";
    repo = "Adafruit-GFX-Library";
    rev = "1.10.11";
    sha256 = "sha256-YedWLdarygS8CF6ALe/YTkqhgVaNiLEDuOswN/BKiLg=";
  };
  "Adafruit_ILI9341" = fetchFromGitHub {
    owner = "adafruit";
    repo = "Adafruit_ILI9341";
    rev = "1.5.9";
    sha256 = "sha256-CZC7DDIsXD/MA45PWcjXnsYtRktpjMks+k1XzkasFvs=";
  };
  "Adafruit_NeoPixel" = fetchFromGitHub {
    owner = "adafruit";
    repo = "Adafruit_NeoPixel";
    rev = "1.8.6";
    sha256 = "sha256-6ANB5ZS3PXorCK1eNpMYMT+Tp9QwrpIp0j8b5lvbhwQ=";
  };
  "Adafruit_SSD1351" = fetchFromGitHub {
    owner = "adafruit";
    repo = "Adafruit-SSD1351-library";
    rev = "1.2.7";
    sha256 = "sha256-xGU+BHavnVg1dTkPnS0q4ZBJ3uKOOwhfyS2mRMltvUI=";
  };
  "Adafruit_STMPE610" = fetchFromGitHub {
    owner = "adafruit";
    repo = "Adafruit_STMPE610";
    rev = "1.1.3";
    sha256 = "sha256-f0NbDLej/2xMha6WcHNz1CKa0uTrXadMnX4hdynN0Mw=";
  };
  "Adafruit_TouchScreen" = fetchFromGitHub {
    owner = "adafruit";
    repo = "Adafruit_TouchScreen";
    rev = "1.1.2";
    sha256 = "sha256-MFM9g8e+L6bC7hqBtJdRqvQGaoPIm74SNbZvdhRvu1Y=";
  };
  "DS3231" = fetchFromGitHub {
    owner = "whonore";
    repo = "DS3231";
    rev = "v1.01";
    sha256 = "sha256-gRwSiGzYB0WTkvLfAyMZIM/yY6H6JE+Mb1ZuZ8MqF8E=";
  };
  "TimerInterrupt" = fetchFromGitHub {
    owner = "khoih-prog";
    repo = "TimerInterrupt";
    rev = "v1.7.0";
    sha256 = "sha256-Xz2VlGq3VKFnMQhKpSFOvG5v6oEq0k6CUbv+5eXp/eM=";
  };
}
