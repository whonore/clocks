{
  arduino-cli,
  arduino-libs,
  avr,
  clang-tools,
  cppcheck,
  fetchurl,
  lib,
  ncurses, # for tput
  python3,
  stdenv,
}: name: clock: let
  libs = map (lib: builtins.getAttr lib arduino-libs) clock.libs;

  featureMatrix = lib.cartesianProductOfSets (lib.genAttrs clock.features (feat: [0 1]));
  matrix = lib.cartesianProductOfSets {
    board = clock.boards;
    features = featureMatrix;
  };
  catFeatures = feats:
    lib.concatStringsSep "-" (lib.mapAttrsToList (feat: val: "${feat}${toString val}") feats);

  mkClockForBoard = {
    board,
    features,
  }: let
    build-flags =
      lib.concatStringsSep " "
      (lib.mapAttrsToList (feat: val: "-D${feat}=${toString val}") features);
  in
    stdenv.mkDerivation {
      name = "${name}-${board}-${catFeatures features}";
      src = ./..;
      buildInputs = [arduino-cli ncurses python3] ++ libs;
      checkInputs = [clang-tools cppcheck];

      doCheck = true;

      postPatch = "patchShebangs scripts";

      configurePhase = ''
        runHook preConfigure

        ARDUINO_HOME=$(mktemp --directory)
        ARDUINO_PKGS=$ARDUINO_HOME/packages
        ARDUINO_LIBS=$ARDUINO_HOME/libraries
        mkdir -p $ARDUINO_PKGS
        mkdir -p $ARDUINO_LIBS

        CONFIG=$ARDUINO_HOME/arduino-cli.yaml
        arduino-cli config init --dest-dir $ARDUINO_HOME
        arduino-cli config set directories.data $ARDUINO_HOME --config-file $CONFIG
        arduino-cli config set directories.user $ARDUINO_LIBS --config-file $CONFIG

        echo '{"libraries":[]}' > $ARDUINO_HOME/library_index.json
        echo '{"packages":[]}' > $ARDUINO_HOME/package_index.json

        # Copied from pkgs/development/embedded/arduino/arduino-core/default.nix
        pkg_src=(${toString (builtins.attrValues avr)})
        pkg_dst=(${toString (builtins.attrNames avr)})
        while [[ "''${#pkg_src[@]}" -ne 0 ]]; do
          file_src=''${pkg_src[0]}
          file_dst=$ARDUINO_PKGS/''${pkg_dst[0]}
          pkg_src=(''${pkg_src[@]:1})
          pkg_dst=(''${pkg_dst[@]:1})
          mkdir -p $(dirname $file_dst)
          ln -sT $file_src $file_dst
        done

        lib_src=(${toString libs})
        lib_dst=(${toString clock.libs})
        while [[ "''${#lib_src[@]}" -ne 0 ]]; do
          file_src=''${lib_src[0]}
          file_dst=$ARDUINO_LIBS/''${lib_dst[0]}
          lib_src=(''${lib_src[@]:1})
          lib_dst=(''${lib_dst[@]:1})
          mkdir -p $(dirname $file_dst)
          ln -sT $file_src $file_dst
        done

        runHook postConfigure
      '';

      buildPhase = ''
        runHook preBuild

        foundMakefile=1
        cd clocks/${name}

        echo "${clock.platform}:${board} ${build-flags}"
        arduino-cli compile \
          -b ${clock.platform}:${board} \
          --build-property "build.extra_flags=${build-flags}" \
          --config-file $CONFIG \
          --libraries $ARDUINO_LIBS \
          .

        ls
        ls simulator
        ls simulator/scripts
        cat simulator/Makefile
        make -C simulator

        runHook postBuild
      '';

      preCheck = "pushd simulator";
      postCheck = "popd";

      installPhase = ''
        runHook preInstall

        install -Dm755 simulator/${name} "$out/bin/${name}"

        runHook postInstall
      '';
    };
in
  builtins.listToAttrs (map ({
      board,
      features,
    } @ args:
      lib.nameValuePair "${board}-${catFeatures features}" (mkClockForBoard args))
  matrix)
