SUMMARY = "Quickly share a folder using Samba from Thunar"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=59530bdf33659b29e73d4adb9f9f6552"

inherit thunar-plugin features_check

ANY_OF_DISTRO_FEATURES = "${GTK3DISTROFEATURES}"
REQUIRED_DISTRO_FEATURES = "pam"

SRC_URI += "file://0001-build-Bump-GLib-minimum-required-to-2.26.patch"
SRC_URI[sha256sum] = "dc1d8c7caa727e76d033d4653dc0742613f57a1711d0050900659c90a84452a0"

RDEPENDS:${PN} += "samba-server"
