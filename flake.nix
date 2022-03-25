{
  inputs.nixpkgs.url = "nixpkgs/nixos-21.11";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils, michaeladler }:

    flake-utils.lib.eachDefaultSystem
      (system:

        let pkgs = import nixpkgs {
          inherit system;
          overlays = [
            (final: prev: {
              fmt_8 = prev.fmt_8.overrideAttrs (old: {

                # this ensures that meson keeps the rpath
                postInstall = ''
                  sed -i -e 's|^Libs:.*|Libs: -L''${libdir} -lfmt  -Wl,-rpath=''${libdir}|' $out/lib/pkgconfig/fmt.pc
                '';

              });
            })
          ];
        };

        in
        rec {

          devShell = with pkgs; stdenv.mkDerivation {
            name = "aoc-shell";
            nativeBuildInputs = [ clang_13 lld_13 pkg-config meson ninja ];
            buildInputs = [ doctest fmt_8 ];
            shellHook = ''
              export CXX=clang++ CXX_LD=lld
              [ -d build/release ] || meson setup --buildtype=release --debug build/release
              [ -d build/debug ] || meson setup --buildtype=debug build/debug
              ln -sf build/debug/compile_commands.json .
              cd build/debug && ln -sf ../../input . && cd -
              cd build/release && ln -sf ../../input . && cd -
            '';
          };

        });
}
