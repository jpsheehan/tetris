{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs.buildPackages;
  [ gnumake pkgconfig allegro5 ];
  }
