#!/usr/bin/env python3

import json
import os
import platform
import shutil
import subprocess
import sys


CONFIG_FILE = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "packages.json"
)


def error(message):
    print(f"\nERROR: {message}")
    sys.exit(1)


def run(command, sudo=False):
    if sudo and os.geteuid() != 0:
        if shutil.which("sudo") is None:
            error("sudo is not installed.")

        command = ["sudo"] + command

    print("\n$ " + " ".join(command))

    result = subprocess.run(command)

    if result.returncode != 0:
        error(
            f"Error :("
            f"(code: {result.returncode})."
        )


def detect_system():
    system = platform.system()

    if system == "Darwin":
        return "macos"

    if system != "Linux":
        error(f"Your OS is not supported ({system})")

    try:
        with open("/etc/os-release", "r", encoding="utf-8") as file:
            data = {}

            for line in file:
                line = line.strip()

                if "=" not in line:
                    continue

                key, value = line.split("=", 1)
                data[key] = value.strip('"')

    except FileNotFoundError:
        error("Failed to find /etc/os-release.")

    distro = data.get("ID", "").lower()

    if distro in {
        "ubuntu",
        "debian",
        "linuxmint",
    }:
        return "debian"

    if distro == "fedora":
        return "fedora"

    if distro in {
        "rhel",
        "centos",
        "rocky",
        "almalinux"
    }:
        return "rhel"

    if distro == "arch":
        return "arch"

    if distro == "manjaro":
        return "manjaro"

    if distro.startswith("opensuse"):
        return "opensuse"

    if distro == "alpine":
        return "alpine"

    error(f"Not supported linux distro: {distro}")


def load_config():
    if not os.path.isfile(CONFIG_FILE):
        error(f"Failed to find {CONFIG_FILE}")

    try:
        with open(CONFIG_FILE, "r", encoding="utf-8") as file:
            return json.load(file)

    except json.JSONDecodeError as e:
        error(f"Incorrect JSON: {e}")


def install_packages(manager, packages):
    if not packages:
        print("Brak packages to install.")
        return

    print("\nPackages:")
    for package in packages:
        print(f"  - {package}")

    print()

    if manager == "apt":
        run(["apt-get", "update"], sudo=True)
        run(["apt-get", "install", "-y"] + packages, sudo=True)

    elif manager == "dnf":
        run(["dnf", "install", "-y"] + packages, sudo=True)

    elif manager == "yum":
        run(["yum", "install", "-y"] + packages, sudo=True)

    elif manager == "pacman":
        run(
            ["pacman", "-Sy", "--needed", "--noconfirm"] + packages,
            sudo=True
        )

    elif manager == "zypper":
        run(
            ["zypper", "--non-interactive", "install"] + packages,
            sudo=True
        )

    elif manager == "apk":
        run(["apk", "add"] + packages, sudo=True)

    elif manager == "brew":
        if shutil.which("brew") is None:
            error(
                "Homebrew is not installed.\n"
                "Install homebrew and run script again."
            )

        run(["brew", "install"] + packages)

    else:
        error(f"Unknown packet manager: {manager}")


def main():
    print("=" * 45)
    print("        Dependency Installer")
    print("=" * 45)

    system = detect_system()

    print(f"\nDetected OS: {system}")

    config = load_config()

    if system not in config:
        error(
            f"No configuration for OS '{system}' "
            "in packages.json."
        )

    system_config = config[system]

    manager = system_config.get("manager")
    packages = system_config.get("packages", [])

    if not manager:
        error(
            f"No 'manager' for OS '{system}'."
        )

    print(f"Packet manager: {manager}")

    install_packages(manager, packages)

    print("\n" + "=" * 45)
    print("     Installation finished succesfully.")
    print("=" * 45)


if __name__ == "__main__":
    main()

