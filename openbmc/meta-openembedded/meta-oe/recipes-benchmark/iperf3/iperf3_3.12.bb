SUMMARY = "Network benchmark tool"
DESCRIPTION = "\
iperf is a tool for active measurements of the maximum achievable bandwidth \
on IP networks. It supports tuning of various parameters related to timing, \
protocols, and buffers. For each test it reports the bandwidth, loss, and \
other parameters."

HOMEPAGE = "http://software.es.net/iperf/"
SECTION = "console/network"
BUGTRACKER = "https://github.com/esnet/iperf/issues"
AUTHOR = "ESNET <info@es.net>, Lawrence Berkeley National Laboratory <websupport@lbl.gov>"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=68ae8cfc577a2c8c51bb51e9628e80b7"

SRC_URI = "git://github.com/esnet/iperf.git;branch=master;protocol=https \
           file://0002-Remove-pg-from-profile_CFLAGS.patch \
           file://0001-configure.ac-check-for-CPP-prog.patch \
           "

SRCREV = "e61aaf8c95df956cefbc54fab7b3d78914664180"

S = "${WORKDIR}/git"

inherit autotools

PACKAGECONFIG ?= "openssl"

PACKAGECONFIG[lksctp] = "ac_cv_header_netinet_sctp_h=yes,ac_cv_header_netinet_sctp_h=no,lksctp-tools"
PACKAGECONFIG[openssl] = "--with-openssl=${RECIPE_SYSROOT}${prefix},--without-openssl,openssl"

CFLAGS += "-D_GNU_SOURCE"

CVE_PRODUCT = "iperf_project:iperf"
