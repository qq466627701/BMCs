SUMMARY = "Telemetry service"
DESCRIPTION = "Middleware between Redfish Telemetry Service and dbus-sensors"
HOMEPAGE = "https://github.com/openbmc/telemetry"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${S}/LICENSE;md5=e3fc50a88d0a364313df4b21ef20c29e"
DEPENDS = "boost \
           gtest \
           nlohmann-json \
           phosphor-logging \
           sdbusplus \
           systemd"
SRCREV = "a06626d1ad963f6998237a332eab9ac9392b7c7a"
PV = "1.0+git${SRCPV}"

SRC_URI = "git://github.com/openbmc/telemetry;branch=master;protocol=https"

S = "${WORKDIR}/git"
SYSTEMD_SERVICE:${PN} = "xyz.openbmc_project.Telemetry.service"

inherit pkgconfig meson
inherit systemd

EXTRA_OEMESON = "-Dbuildtest=false"
