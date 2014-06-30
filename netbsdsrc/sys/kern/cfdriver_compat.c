/*
 * XXX Gross and disgusting hack to allow us to use newer versions
 * XXX of drivers from NetBSD without modifications.
 * XXX
 * XXX In the current NetBSD sources, 'cfdriver' declarations are
 * XXX emitted by config.  Since we don't want to update _all_ of
 * XXX our drivers, we just put the cfdrivers for new drivers from
 * XXX NetBSD here.
 */

#include <sys/param.h>
#include <sys/device.h>

struct cfdriver atapibus_cd = { NULL, "atapibus", DV_DULL };
struct cfdriver cd_cd = { NULL, "cd", DV_DISK };
struct cfdriver ch_cd = { NULL, "ch", DV_DULL };
struct cfdriver com_cd = { NULL, "com", DV_TTY };
struct cfdriver isp_cd = { NULL, "isp", DV_DULL };
struct cfdriver lpt_cd = { NULL, "lpt", DV_DULL };
struct cfdriver ofbus_cd = { NULL, "ofbus", DV_DULL };
struct cfdriver ofcons_cd = { NULL, "ofcons", DV_TTY };
struct cfdriver ofdisk_cd = { NULL, "ofdisk", DV_DISK };
struct cfdriver ofnet_cd = { NULL, "ofnet", DV_IFNET };
struct cfdriver ofroot_cd = { NULL, "ofroot", DV_DULL };
struct cfdriver ofrtc_cd = { NULL, "ofrtc", DV_DULL };
struct cfdriver scsibus_cd = { NULL, "scsibus", DV_DULL };
struct cfdriver sd_cd = { NULL, "sd", DV_DISK };
struct cfdriver se_cd = { NULL, "se", DV_IFNET };
struct cfdriver ss_cd = { NULL, "ss", DV_DULL };
struct cfdriver st_cd = { NULL, "st", DV_TAPE };
struct cfdriver uk_cd = { NULL, "uk", DV_DULL };
struct cfdriver wd_cd = { NULL, "wd", DV_DISK };
struct cfdriver wdc_cd = { NULL, "wdc", DV_DULL };
