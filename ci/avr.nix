{fetchzip}: {
  # Copied from https://downloads.arduino.cc/packages/package_index.json
  "arduino/hardware/avr/1.8.5" = fetchzip {
    url = "http://downloads.arduino.cc/cores/staging/avr-1.8.5.tar.bz2";
    sha256 = "sha256-N6VUgRS7Qh9gmuadDuA/8xP1net40e6KARcvbuqmHxQ=";
  };
  "arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7" = fetchzip {
    url = "http://downloads.arduino.cc/tools/avr-gcc-7.3.0-atmel3.6.1-arduino7-x86_64-pc-linux-gnu.tar.bz2";
    sha256 = "sha256-nx9eBcvcDZ9aoc8In/ueBVXwH9OwPlXNmKfqjmP7I6Y=";
  };
  "builtin/tools/ctags/5.8-arduino11" = fetchzip {
    url = "https://downloads.arduino.cc/tools/ctags-5.8-arduino11-pm-x86_64-pc-linux-gnu.tar.bz2";
    sha256 = "sha256-+/FMzCcEc3LLO5WRV37QlEj255cXTVSiMHV+94LaOBo=";
  };
}
