#!/usr/bin/env bash
# Install a pinned, relocatable Linux x86-64 -> AArch64 GNU toolchain.
set -euo pipefail

fail() { printf 'Error: %s\n' "$*" >&2; exit 1; }
for tool in uname dirname mkdir mktemp curl sha256sum tar xz mv rm; do
    command -v "$tool" >/dev/null || fail "Missing prerequisite: $tool"
done
[[ $(uname -s) == Linux && $(uname -m) == x86_64 ]] ||
    fail 'This bundle requires a Linux x86-64 host.'

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
bundle=arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu
archive=$bundle.tar.xz
checksum=322f0b4482fc0d9fa0bb468134841f08d8c554c54ff5aa29a13a7a24bf7e1eb5
url=https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/$archive
install_dir=$script_dir/$bundle

validate() {
    [[ -x $1/bin/aarch64-none-linux-gnu-gcc && -x $1/bin/aarch64-none-linux-gnu-g++ ]] &&
    [[ $("$1/bin/aarch64-none-linux-gnu-gcc" -dumpmachine) == aarch64-none-linux-gnu ]] &&
    [[ $("$1/bin/aarch64-none-linux-gnu-gcc" -dumpfullversion) == 13.3.1 ]] &&
    [[ -d $("$1/bin/aarch64-none-linux-gnu-gcc" -print-sysroot) ]]
}
if [[ -e $install_dir ]]; then
    [[ -f $install_dir/.verified-sha256 ]] &&
        [[ $(< "$install_dir/.verified-sha256") == "$checksum" ]] &&
        validate "$install_dir" || fail "Incomplete installation: remove $install_dir and rerun setup."
    printf 'Toolchain already installed: %s\n' "$install_dir"
    exit 0
fi

# Extract only after checksum verification, then publish the complete directory.
work_dir=$(mktemp -d "$script_dir/.setup.XXXXXXXX")
trap 'rm -rf -- "$work_dir"' EXIT
curl --fail --location --retry 3 --output "$work_dir/$archive" "$url"
(cd "$work_dir" && printf '%s  %s\n' "$checksum" "$archive" | sha256sum --check -) ||
    fail 'Archive checksum mismatch; installation aborted.'
tar -xJf "$work_dir/$archive" -C "$work_dir"
validate "$work_dir/$bundle" || fail 'Extracted compiler validation failed.'
printf '%s\n' "$checksum" > "$work_dir/$bundle/.verified-sha256"
# -T prevents nesting if another setup process installed the directory meanwhile.
mv -T -- "$work_dir/$bundle" "$install_dir"
printf 'Installed toolchain: %s\n' "$install_dir"
