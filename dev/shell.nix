{pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    packages = with pkgs; [
        gcc
        gnumake
        cmake
        libcpr          # Analog requests
        nlohmann_json   # For work with json
        curl
        pkg-config
    ];

    shellHook = ''
        echo Cpp environment is active!
    '';
}
