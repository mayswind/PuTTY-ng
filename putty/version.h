/*
 * This header file provides the various versioning-related #defines
 * for a particular PuTTY build.
 *
 * When my automated build system does a full build, Buildscr
 * completely overwrites this file with information derived from the
 * circumstances and type of that build. The information _here_ is
 * default stuff used for local development runs of 'make'.
 */

#define VERSION_MAJOR "0"
#define VERSION_MINOR "67"
#define PUTTY_ND_VERSION_MAJOR "6"
#define PUTTY_ND_VERSION_MINOR "6"
#define PUTTY_NG_RELEASE "15"
#define BINARY_VERSION 0,67,66,15

#define TEXTVER "PuTTY-ng " VERSION_MAJOR "." VERSION_MINOR "." PUTTY_ND_VERSION_MAJOR "" PUTTY_ND_VERSION_MINOR "." PUTTY_NG_RELEASE
#define SSHVER "PuTTY-ng-" VERSION_MAJOR "." VERSION_MINOR
